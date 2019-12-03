/*
 * rockblock.h
 *
 *  Created on: Nov 6, 2019
 *      Author: Scott Smith
 */

#ifndef SRC_ROCKBLOCK_ROCKBLOCK_H_
#define SRC_ROCKBLOCK_ROCKBLOCK_H_

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

// maximum message sizes for our buffers.
// We know TX is going to be be 340 bytes at most by the RockBLOCK spec.
// RX buffer needs to be 270 bytes at most by the RockBLOCK spec.
// add 30 bytes for overhead from message echos.
#define RB_TX_SIZE 340+30
#define RB_RX_SIZE 270+30
#define RB_SOF '\0'
#define RB_EOF '\0'

#define RB_TRANSMIT_RATE_MS (uint32_t) 60000//300000      // this is 5 minutes (1000 ms/sec * 60 sec/min * 5min)
#define RB_RETRY_RATE_MS    15000       // this is 15 seconds (1000 ms/sec / * 15 sec)
#define RB_MAX_TX_RETRIES   10          // Retry at most 10 times. This means we try for 10*15=150 seconds.
#define RB_MAX_RX_RETRIES   5           // retry at most 5 times. This means we try for 5*15=75 seconds.

typedef enum {
    AT = 0, // OK
    ATK0 = 1, // disable flow control
    SBDWT = 2, // send message
    SBDIX = 3, // start SBD session
    SBDRT = 4, // pull downloaded ASCI message from RockBLOCK
    SBDRB = 5 // pull downloaded binary message from RockBLOCK
} rb_message_t;

typedef enum {
    CUT_FTU_NOW = 1, // immediately cut the ftu
    GET_TELEM = 2, // get current data immediately
    CONFIG_BUZZER = 3, // change how often the buzzer beeps
    SET_FTU_TIMER = 4, // set the amount of time, in ms, that the ftu should wait to fire
    START_FTU_TIMER = 5, // start counting down the ftu time
    STOP_FTU_TIMER = 6 // stop counting down the ftu time
} rb_command_t;

typedef struct {
    volatile uint8_t buff[RB_RX_SIZE];      // Don't need ring buffer for the RockBLOCK due its simplicity.
    volatile uint8_t * volatile cur_ptr;    // pointer to next spot in the rb_rx_buff, where newest values will be put when received.
    volatile uint8_t * volatile last_ptr;   // pointer to the final valid value in rb_rx_buff.
    volatile uint8_t numReturns;
    volatile bool finished;
    SemaphoreHandle_t rxSemaphore;
} rb_rx_buffer_t;

typedef struct {
    volatile uint8_t buff[RB_TX_SIZE];               // Don't need ring buffer for the RockBLOCK due its simplicity.
    volatile uint8_t * volatile cur_ptr;    // pointer to next spot in the rb_tx_buff, where newest values will be taken from when sending.
    volatile uint8_t * volatile last_ptr;   // pointer to the final valid value in rb_tx_buff.
    volatile uint8_t * volatile tx_ptr;     // pointer to the value being transmitted by callback.
    SemaphoreHandle_t txSemaphore;
} rb_tx_buffer_t;

typedef struct {
    rb_tx_buffer_t tx; // all tx info is stored here
    rb_rx_buffer_t rx; // all rx info is stored here
    bool is_valid;
    SemaphoreHandle_t busy_semaphore;
} ROCKBLOCK_t;


void task_rockblock();

// Initializes the RockBLOCK module and UART ports on the msp430 to what we need.
void rb_init(ROCKBLOCK_t *rb);

// Queries and determines if we have a message on satellite ready to be received.
// If there is at least one message, it fills the rx buffer with the data from the first message.
// need to download other messages later.
// This function will return 1 if there are messages available, 0 if not. Returns 2 if there is an error.
// Consumes credit if no message was on satellite!
uint8_t rb_check_mailbox(ROCKBLOCK_t *rb, int8_t *msgsQueued);

// Check if we have message waiting for us.
// Does not consume a credit; this is done locally.
// Returns true if phone is ringing, otherwise false.
// Recommend to use this first, as it does not consume credits.
// Potentially possible for no ring to have been received but do have message.
bool rb_check_ring();

// Check to see if the network is available. This returns true if the network is available.
// this returns false if the network is not available. Note, that this isn't a guarantee.
// We might fail to transmit if it returns true, or have been able to transmit if it returns false.
// This is just likely to be the same as our ability to transmit.
bool rb_check_netav();

// Gets the signal strength our RockBLOCK has with the Iridium Network.
// This can take considerable time, so not always worth it to use this function.
// Included here in case you want it.
uint8_t rb_get_ssi();

// Sends a message over the Iridum Network from our RockBLOCK.
// If there was a message waiting to be received, the message will be pulled into the RockBLOCK as well.
// This means we don't waste credits to check our mailbox if you are sending data at the same time.
// NOTE: Always pull any outstanding messages from the RockBLOCK into MSP430 RX buffer and process them
// before sending a message, because we reduce number of spent credits.
// We return values into the passed by reference arguments of this function.
// int8_t msgReceived indicates if messages were received. 0-> no message, 1 or more -> yes message, -1-> error downloading.
// bool msgSent indicates if we successfully sent a message to the network.
// msgsQueued is number of queued messages waiting to be downloaded.
void rb_send_message(ROCKBLOCK_t *rb, uint8_t *msg, uint16_t len, bool *msgSent, int8_t *msgReceived, int8_t *msgsQueued);

// Starts an SBD session with the Irdium Network.
// This is called by the rb_send_message(), rb_retrieve_message() functions.
// This can also be manually called to retry transmission of a previously failed message still in the queue.
// int8_t msgReceived indicates if messages were received. 0-> no message, 1 or more -> yes message, -1-> error downloading.
// bool msgSent indicates if we successfully sent a message to the network.
// msgsQueued is number of queued messages waiting to be downloaded.
void rb_start_session(ROCKBLOCK_t *rb, bool *msgSent, int8_t *msgReceived, int8_t *msgsQueued);

// Grabs message from the RockBLOCK.
// Must have already been downloaded onto RockBLOCK. Does not consume credits, as we are polling our RockBLOCK's memory.
// value will be in rb->rx.buff
bool rb_retrieve_message(ROCKBLOCK_t *rb);

// This is the function that will be used by the UART driver when we receive RX messages.
// Pass this into the driver via the initUartRxCallback function.
// datum is the byte that was just grabbed from the driver.
// the void * param MUST hold the ROCKBLOCK struct.
void rb_rx_callback(void *param, uint8_t datum);

// This is the function that will be used by the UART driver when we send TX messages.
// Pass this into the driver via the initUartTxCallback function.
// txAddress is where we put the byte we want the UART driver to send over the port.
// the void * param MUST hold the ROCKBLOCK struct.
bool rb_tx_callback(void *param, uint8_t *txAddress);

// Controls the sleep/awake state of the RockBLOCK. If awake == 1, the RockBLOCK is set to awake, if 0 it is set to sleep.
void rb_set_awake(bool awake);

// Creates a rockblock telemetry packet which will hold all of our sensor data.
// This message uses at most 2 credits to send.
void rb_create_telemetry_packet(uint8_t *msg, uint16_t *len, int32_t pressure,
                       int32_t humidity, int32_t pTemp, int32_t hTemp, int32_t altitude,
                       gnss_time_t *time, gnss_coordinate_pair_t *location, bool *success);

// processes the message from the rockblock buffer.
// returns true if the message is a valid command, false if it is not a valid command.
bool rb_process_message(rb_rx_buffer_t *rx);

// controls the FTU
// if true, starts FTU.
// if false, turns off FTU
void rb_cut_ftu(bool cut);

// enables interrupts for the RockBLOCK. Do not call unless you previously disabled interrupts from rb_set_disabled()
void rb_enable_interrupts(ROCKBLOCK_t *rb);

// disables interrupts for the RockBLOCK. Returns true if interrupts were successfully disabled.
// False indicates we were unable to disable interrupts for some reason.
bool rb_disable_interrupts(ROCKBLOCK_t *rb);

#endif /* SRC_ROCKBLOCK_ROCKBLOCK_H_ */
