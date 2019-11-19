#ifndef GNSS_H
#define GNSS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <msp430.h>
#include <driverlib.h>

#include "ring_buff.h"
#include "uart.h"
#include "FreeRTOS.h"
#include "semphr.h"


#define GNSS_NMEA

#ifdef GNSS_UBX

#define GNSS_RX_BUFF_SIZE                1000
#define GNSS_TX_BUFF_SIZE                1000

#endif /* GNSS_UBX */

#ifdef GNSS_NMEA

#define GNSS_TX_BUFF_SIZE               100
#define GNSS_RX_BUFF_SIZE               100
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

typedef struct {
    uint16_t deg;
    uint8_t min;
    uint16_t msec;
    char dir;
} gnss_coordinate_t;

typedef struct {
    gnss_coordinate_t latitude;
    gnss_coordinate_t longitude;
} gnss_coordinate_pair_t;

typedef struct {
    uint8_t hour;
    uint8_t min;
    uint16_t msec;
} gnss_time_t;

typedef struct {
    gnss_time_t time;
    gnss_coordinate_pair_t location;
    gnss_quality_t quality;
    uint8_t num_satellites;
    int32_t altitude;
} gnss_fix_t;

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
} gnss_t;

#ifdef GNSS_NMEA
#include "nmea.h"
#endif

#ifdef GNSS_UBX
#include "ubx.h"
#endif

/*!
 * \brief initializes the GNSS object
 * 
 * @param gnss_obj the GNSS object to be initialized
 * \return None
 */
void gnss_init(gnss_t *gnss_obj);

bool gnss_get_time(gnss_t *gnss_obj, gnss_time_t *time);

bool gnss_get_location(gnss_t *gnss_obj, gnss_coordinate_pair_t *location);

bool gnss_get_altitude(gnss_t *gnss_obj, int32_t *altitude);

// converts location struct into a position decimal seconds.
int32_t gnss_location_to_decSec(gnss_coordinate_t gct);

#ifdef __cplusplus
}
#endif

#endif /* GNSS_H */
