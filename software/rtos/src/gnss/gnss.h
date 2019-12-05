#ifndef GNSS_H
#define GNSS_H

#ifdef __cplusplus
extern "C" {
#endif

// standard type definitions
#include <stdint.h>
#include <stdbool.h>
// MSP430 hardware includes
#include <msp430.h>
#include <driverlib.h>
// freeRTOS includes
#include "FreeRTOS.h"
#include "semphr.h"
// application includes
#include "ring_buff.h"
#include "uart.h"

/* protocol selection:
 *      - GNSS_NMEA for standardized NMEA communications
 *          * Compatible with most GNSS modules
 *          * human readable ASCII encoding
 *      - GNSS_UBX for proprietary UBX communications
 *          * Only compatible with UBLOX modules
 *          * higher performance binary protocol
 *          * not yet implemented TODO: implement UBX
 */
#define GNSS_NMEA

#ifdef GNSS_UBX

#define GNSS_RX_BUFF_SIZE                1000
#define GNSS_TX_BUFF_SIZE                1000

#endif /* GNSS_UBX */

#ifdef GNSS_NMEA

#define GNSS_TX_BUFF_SIZE               100
#define GNSS_RX_BUFF_SIZE               512
#define GNSS_RX_MAX_PAYLOAD             100

#endif /* GNSS_NMEA */

typedef enum {
    no_fix = 0,
    auto_fix = 1,
    diff_fix = 2,
    rtk_fix = 4,
    rtk_float = 5,
    dead_reckon = 6
} gnss_quality_t;

/** @struct gnss_coordinate_t
 *  @brief Location data for a coordinate in a single direction
 *
 */
typedef struct {
    uint16_t deg;
    uint8_t min;
    uint16_t msec;
    char dir;
} gnss_coordinate_t;

/** @struct gnss_coordinate_pair_t
 *  @brief pair of coordinates for 2 dimensional location
 *
 */
typedef struct {
    gnss_coordinate_t latitude;
    gnss_coordinate_t longitude;
} gnss_coordinate_pair_t;

/** @struct gnss_time_t
 *  @brief time in hours, minutes, and milliseconds
 *
 */
typedef struct {
    uint8_t hour;
    uint8_t min;
    uint16_t msec;
} gnss_time_t;

/** @struct gnss_fix_t
 *  @brief GNSS fix data
 *
 */
typedef struct {
    gnss_time_t time;
    gnss_coordinate_pair_t location;
    gnss_quality_t quality;
    uint8_t num_satellites;
    int32_t altitude;
} gnss_fix_t;

/** @struct gnss_t
 *  @brief config object for the entire GNSS module
 *
 */
typedef struct {
    uint8_t gnss_rx_mem[GNSS_RX_BUFF_SIZE];
    uint8_t gnss_tx_mem[GNSS_TX_BUFF_SIZE];
    ring_buff_t gnss_rx_buff;
    ring_buff_t gnss_tx_buff;
    UART_MODULE_NAMES uart_module;
    gnss_fix_t last_fix;
    SemaphoreHandle_t uart_semaphore;
    SemaphoreHandle_t data_mutex;
    bool decoding_message;
    bool is_valid;
} gnss_t;

#ifdef GNSS_NMEA
#include "nmea.h"
#endif

#ifdef GNSS_UBX
#include "ubx.h"
#endif

/*!
 * \brief freeRTOS task to decode GNSS messages
 *
 * \return None
 */
void task_gnss(void);

/*!
 * \brief initializes the GNSS object
 * 
 * @param gnss_obj the GNSS object to be initialized
 * \return None
 */
void gnss_init(gnss_t *gnss_obj);

/*!
 * \brief UART RX callback function
 *
 * Callback function to be called within the UART RX Interrupt Service Routine (ISR).
 * Identifies start or end of sentence and adds data to the ring buffer if part of a sentence.
 * Must be registered with the UART driver with initUartRxCallback().
 *
 * @param param is the passed in parameter from the UART driver. should be set as the GNSS object.
 * @param datum is the byte read over UART passed in by the UART driver.
 * \return None
 *
 */
void gnss_rx_callback(void *param, uint8_t datum);

/*!
 * \brief get time of the last GNSS fix
 *
 * @param gnss_obj is the GNSS object to retrieve the fix from.
 * @param time is a pointer to the memory address to store the time in.
 * \return None
 *
 */
bool gnss_get_time(gnss_t *gnss_obj, gnss_time_t *time);

/*!
 * \brief get location of the last GNSS fix
 *
 * @param gnss_obj is the GNSS object to retrieve the fix from.
 * @param location is a pointer to the memory address to store the location in.
 * \return None
 *
 */
bool gnss_get_location(gnss_t *gnss_obj, gnss_coordinate_pair_t *location);

/*!
 * \brief get altitude of the last GNSS fix
 *
 * @param gnss_obj is the GNSS object to retrieve the fix from.
 * @param altitude is a pointer to the memory address to store the altitude in.
 * \return None
 *
 */
bool gnss_get_altitude(gnss_t *gnss_obj, int32_t *altitude);

/*!
 * \brief convert GNSS coordinate to deciSeconds
 *
 * @param coordinate is the GNSS coordinate to convert.
 * \return coordinate location in deciMilliSeconds
 *
 */
int32_t gnss_coord_to_decMilliSec(gnss_coordinate_t *coordinate);

/*!
 * \brief disable GNSS interrupts
 *
 * Call at the beginning of a critical section to disable GNSS interrupts.
 *
 * @param GNSS object to disable interrupts for
 * \return None
 *
 */
void gnss_disable_interrupts(gnss_t *gnss_obj);

/*!
 * \brief enable GNSS interrupts
 *
 * Call at the end of a critical section to re-enable GNSS interrupts.
 *
 * @param GNSS object to enable interrupts for
 * \return None
 *
 */
void gnss_enable_interrupts(gnss_t *gnss_obj);

#ifdef __cplusplus
}
#endif

#endif /* GNSS_H */
