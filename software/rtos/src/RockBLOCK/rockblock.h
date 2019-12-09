/*
 * rockblock.h
 *
 *  Created on: Nov 6, 2019
 *      Author: Scott Smith
 */

#ifndef SRC_ROCKBLOCK_ROCKBLOCK_H_
#define SRC_ROCKBLOCK_ROCKBLOCK_H_

// -------------------------------------------------------------- //
// -------------------- include dependencies -------------------- //
// -------------------------------------------------------------- //

#include <stdint.h>
#include <stdbool.h>
#include <msp430.h>
#include <driverlib.h>

#include "ring_buff.h"
#include "uart.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "gnss.h"
#include "sensors.h"


// ------------------------------------------------------- //
// -------------------- public macros -------------------- //
// ------------------------------------------------------- //

// maximum message sizes for our buffers.
// We know TX is going to be be 340 bytes at most by the RockBLOCK spec.
// RX buffer needs to be 270 bytes at most by the RockBLOCK spec.
// add 30 bytes for overhead from message echos.
#define RB_TX_SIZE 340+30
#define RB_RX_SIZE 270+30
#define RB_SOF '\0'
#define RB_EOF '\0'

// Message Retry Paramaters
#define RB_TRANSMIT_RATE_MS (uint32_t) 60000    // this is 1 minute
#define RB_RETRY_RATE_MS    15000               // this is 15 seconds
#define RB_MAX_TX_RETRIES   10                  // Retry at most 10 times. This means we try for 10*15=150 seconds.
#define RB_MAX_RX_RETRIES   5                   // retry at most 5 times. This means we try for 5*15=75 seconds.


// ---------------------------------------------------------- //
// -------------------- type definitions -------------------- //
// ---------------------------------------------------------- //

// enum type of message for controlling the RockBLOCK.
typedef enum {
    AT = 0, // OK
    ATK0 = 1, // disable flow control
    SBDWT = 2, // send message
    SBDIX = 3, // start SBD session
    SBDRT = 4, // pull downloaded ASCI message from RockBLOCK
    SBDRB = 5 // pull downloaded binary message from RockBLOCK
} rb_message_t;

// enum type of message sent from the ground station to the ATACS payloads
typedef enum {
    CUT_FTU_NOW = 1, // immediately cut the ftu
    GET_TELEM = 2, // get current data immediately
    CONFIG_BUZZER = 3, // change how often the buzzer beeps
    SET_FTU_TIMER = 4, // set the amount of time, in ms, that the ftu should wait to fire
    START_FTU_TIMER = 5, // start counting down the ftu time
    STOP_FTU_TIMER = 6 // stop counting down the ftu time
} rb_command_t;

/** @struct rb_rx_buffer_t
 *  @brief UART RX buffer and necessary pointers for the RockBLOCK 9603
 *
 */
typedef struct {
    volatile uint8_t buff[RB_RX_SIZE];      // Simple RX buffer for our data. Do not modify directly. Do not read unless finished is true.
    volatile uint8_t * volatile cur_ptr;    // pointer to next spot in the rb_rx_buff, where newest values will be put when received.
    volatile uint8_t * volatile last_ptr;   // pointer to the final valid value in rb_rx_buff.
    volatile uint8_t numReturns;            // number of carriage returns in the message. This is how we know when the RockBLOCK has finished its response
    volatile bool finished;                 // true if the RockBLOCK is done sending us data.
    SemaphoreHandle_t rxSemaphore;          // lets us know when RX is done, so the task can go to sleep while waiting for the data.
} rb_rx_buffer_t;

/** @struct rb_tx_buffer_t
 *  @brief UART TX buffer and necessary pointers for the RockBLOCK 9603
 *
 */
typedef struct {
    volatile uint8_t buff[RB_TX_SIZE];      // Simple TX buffer for our data. Do not modify directly.
    volatile uint8_t * volatile cur_ptr;    // pointer to next spot in the rb_tx_buff, where newest values will be taken from when sending.
    volatile uint8_t * volatile last_ptr;   // pointer to the final valid value in rb_tx_buff.
    volatile uint8_t * volatile tx_ptr;     // pointer to the value being transmitted by callback.
    SemaphoreHandle_t txSemaphore;          // lets us know when TX is done, so the task can go to sleep while transmitting.
} rb_tx_buffer_t;

/** @struct ROCKBLOCK_t
 *  @brief Struct for the Rockblock. This is what you should instantiate directly to use the RockBLOCK properly.
 *
 */
typedef struct {
    rb_tx_buffer_t tx; // all tx info is stored here
    rb_rx_buffer_t rx; // all rx info is stored here
    bool is_valid;      // is true if rb_init() has been called and completed.
    SemaphoreHandle_t busy_semaphore; // Semaphore to prevent interrupts from being disabled while doing something important.
} ROCKBLOCK_t;

// ----------------------------------------------------------- //
// -------------------- public prototypes -------------------- //
// ----------------------------------------------------------- //

/*!
 * \brief freeRTOS task to use the RockBLOCK.
 *
 * \return None
 */
void task_rockblock();


/*!
 * \brief Initializes the RockBLOCK passed in as an argument. It must not leave scope.
 *
 * @param rb: is a pointer to a ROCKBLOCK_t struct to initialize.
 *
 * \return None
 *
 */
// Initializes the RockBLOCK module and UART ports on the msp430 to what we need.
void rb_init(ROCKBLOCK_t *rb);


/*!
 * \brief Queries and determines if we have a message on satellite ready to be received.
 * If there is at least one message, it fills the rx buffer with the data from the first message. Need to download other messages later.
 * Consumes a credit if no message was on satellite!
 *
 * @param rb: is the RockBLOCK struct to use.
 * @param msgsQueued: is an output from this function that lets you know how many messages are on the satellite waiting to download.
 *
 * \return uint8_t value. Returns 1 if there are messages available, 0 if not. Returns 2 if there is an error.
 *
 */
uint8_t rb_check_mailbox(ROCKBLOCK_t *rb, int8_t *msgsQueued);


/*!
 * \brief Check if we have message ringing for us. Does not consume a credit; this is done locally.
 * Returns true if phone is ringing, otherwise false. Recommend to use this first, as it does not consume credits.
 * It is possible for no ring to have been received but there is a message available.
 *
 * \return bool value. Returns true if there are messages available. Returns false if we are not ringing.
 *
 */
bool rb_check_ring();


/*!
 * \brief Check to see if the network is available. This returns true if the network is available.
 * This returns false if the network is not available. Note, that this isn't a guarantee.
 * We might fail to transmit if it returns true, or have been able to transmit if it returns false.
 * This is just likely to be the same as our ability to transmit.
 *
 * \return bool value. Returns true if there network is available. Returns false if it was not available.
 *
 */
bool rb_check_netav();

/*!
 * \brief Gets the signal strength our RockBLOCK has with the Iridium Network.
 * This can take considerable time, so not always worth it to use this function. Included here in case you want it.
 * This is just likely to be the same as our ability to transmit.
 *
 * \return uint8_t value. The number indicates the signal strength.
 *
 */
uint8_t rb_get_ssi();


/*!
 * \brief Sends a message over the Iridum Network from our RockBLOCK.
 * If there was a message waiting to be received, the message will be pulled into the RockBLOCK as well.
 * This means we don't waste credits to check our mailbox if you are sending data at the same time.
 * NOTE: Always pull any outstanding messages from the RockBLOCK into MSP430 RX buffer and process them
 * before sending a message, or you might lose a message. We return values into the passed by reference arguments of this function.
 * 
 * @param rb: is the RockBLOCK struct
 * @param msg: is the message to send on the network
 * @param len: is the length of the message to send on the network
 * @param msgReceived: indicates if messages were received. 0-> no message, 1 or more -> yes message, -1-> error downloading.
 * @param msgSent: indicates if we successfully sent a message to the network.
 * @param msgsQueued: is the number of queued messages waiting to be downloaded.
 *
 * \return none.
 */
void rb_send_message(ROCKBLOCK_t *rb, uint8_t *msg, uint16_t len, bool *msgSent, int8_t *msgReceived, int8_t *msgsQueued);


/*!
 * \brief Starts an SBD session with the Irdium Network.
 * This is called by the rb_send_message(), rb_retrieve_message() functions.
 * This can also be manually called to retry transmission of a previously failed message still in the queue.
 *
 * @param rb: is the RockBLOCK struct
 * @param msgReceived: indicates if messages were received. 0-> no message, 1 or more -> yes message, -1-> error downloading.
 * @param msgSent: indicates if we successfully sent a message to the network.
 * @param msgsQueued: is number of queued messages waiting to be downloaded.
 *
 * \return none.
 */
void rb_start_session(ROCKBLOCK_t *rb, bool *msgSent, int8_t *msgReceived, int8_t *msgsQueued);


/*!
 * \brief Grabs message from the RockBLOCK and downloads to MSP430. Message will be in rb->rx.buff
 * Must have already been downloaded onto RockBLOCK. Does not consume credits, as we are polling our RockBLOCK's memory.
 *
 * @param rb: is the RockBLOCK struct
 *
 * \return bool. True if a message was successfully retrieved. False if not 
 */
bool rb_retrieve_message(ROCKBLOCK_t *rb);


/*!
 * \brief This is the function that will be used by the UART driver when we receive RX messages.
 * Pass this into the driver via the initUartRxCallback function. Recommend to not edit this function.
 * 
 * @param datum: is the byte that was just grabbed from the driver.
 * @param *param: MUST hold the ROCKBLOCK struct.
 *
 * \return none.
 */
void rb_rx_callback(void *param, uint8_t datum);


/*!
 * \brief This is the function that will be used by the UART driver when we send TX messages.
 * Pass this into the driver via the initUartTxCallback function. Recommend to not edit this function.
 *
 * @param txAddress: is where we put the byte we want the UART driver to send over the port.
 * @param *param: MUST hold the ROCKBLOCK struct.
 *
 * \return bool. Returns true while TX is ongoing. Returns false if TX is finished.
 */
bool rb_tx_callback(void *param, uint8_t *txAddress);


/*!
 * \brief Controls the sleep/awake state of the RockBLOCK. Consumes less current while asleep but cannot be used until awakened.
 *
 * @param awake controls if RockBLOCK is awake. If awake == true, the RockBLOCK is set to awake, if false it is set to sleep.
 *
 * \return none.
 */
void rb_set_awake(bool awake);

/*!
 * \brief Creates a telemetry packet to send with the RockBLOCK.
 *
 * @param msg: output from this function. Holds the completed message when done.
 * @param len: output from this function. Holds the length of the message when done.
 * @param pressure: input to this function. Holds the pressure sensor data.
 * @param humidity: input to this function. Holds the humidity sensor data.
 * @param pTemp: input to this function. Holds the humidity sensor's temperature sensor data.
 * @param hTemp: input to this function. Holds the pressure sensor's temperature sensor data.
 * @param altitude: input to this function. Holds the GPS altitude.
 * @param time: input to this function. Holds the current GPS time.
 * @param location: input to this function. Holds the current GPS location.
 * @param success: input to this function. Array of bools which lets you know if each of the prior paramaters are valid.
 * For example, if success[0] is false, then pressure is invalid. If success[2] is false, then pTemp is invalid data.
 *
 * \return none.
 */
void rb_create_telemetry_packet(uint8_t *msg, uint16_t *len, int32_t pressure,
                       int32_t humidity, int32_t pTemp, int32_t hTemp, int32_t altitude,
                       gnss_time_t *time, gnss_coordinate_pair_t *location, bool *success);


/*!
 * \brief Processes the message isnide of the rockblock's RX buffer.
 *
 * @param rx: buffer to be processed.
 *
 * \return bool. TODO: Returns true if it was a valid message.
 * Currently, always just cuts the FTU upon receipt of any message.
 */
bool rb_process_message(rb_rx_buffer_t *rx);


/*!
 * \brief Controls the flight termination unit.
 *
 * @param cut: if true, the FTU is turned on. Note this is hot, do not touch. If false, turns FTU off.
 *
 * \return none
 */
void rb_cut_ftu(bool cut);


/*!
 * \brief Enables interrupts for the RockBLOCK. Do not call unless you previously disabled interrupts from rb_set_disabled()
 *
 * @param rb: rockblock to be enabled.
 *
 * \return none.
 */
void rb_enable_interrupts(ROCKBLOCK_t *rb);

/*!
 * \brief Disables interrupts for the RockBLOCK. Does this FreeRTOS safely. Do NOT call twice in a row. Only call if absolutely necessary.
 *
 * @param rb: rockblock to be disabled.
 *
 * \return bool. If true, interrupts were successfully disabled. If false, they failed to be disabled for some reason. Try again.
 */
bool rb_disable_interrupts(ROCKBLOCK_t *rb);

#endif /* SRC_ROCKBLOCK_ROCKBLOCK_H_ */
