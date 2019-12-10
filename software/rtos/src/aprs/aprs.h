#ifndef APRS_H_
#define APRS_H_

#ifdef __cplusplus
extern "C" {
#endif

// -------------------------------------------------------------- //
// -------------------- include dependencies -------------------- //
// -------------------------------------------------------------- //

// standard libraries
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
// FreeRTOS
#include "FreeRTOS.h"
// application drivers
#include "afsk.h"
#include "ax25.h"
#include "uart.h"
#include "sensors.h"
#include "rockblock.h"
#include "gnss.h"


// ------------------------------------------------------- //
// -------------------- public macros -------------------- //
// ------------------------------------------------------- //

#define APRS_DEST_CALLSIGN "APRS"
#define APRS_SRC_CALLSIGN  "ATACS"
#define APRS_COMMENT "ATACS - REMEMBER TO CHANGE SOURCE CALLSIGN"

#define APRS_PERIOD_MS 60000

#define APRS_PD_PORT     1
#define APRS_PD_PIN      2
#define APRS_PTT_PORT    1
#define APRS_PTT_PIN     3
#define APRS_PWM_PORT    2
#define APRS_PWM_PIN     2
#define APRS_UART_PORT   10
#define APRS_UART_TX     5
#define APRS_UART_RX     4
#define APRS_ACTIVE_HIGH false


// ----------------------------------------------------------- //
// -------------------- public prototypes -------------------- //
// ----------------------------------------------------------- //

/*!
 * \brief freeRTOS task to send APRS beacons
 *
 * \return None
 */
void task_aprs();

#endif /* APRS_H_ */
