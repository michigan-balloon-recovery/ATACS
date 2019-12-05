/*
 * xbee.h
 *
 *  Created on: Nov 17, 2019
 *      Author: SSS
 */

#ifndef SRC_XBEE_XBEE_H_
#define SRC_XBEE_XBEE_H_

#include <stdint.h>
#include <stdbool.h>
#include <msp430.h>
#include <driverlib.h>

#include "ring_buff.h"
#include "uart.h"
#include "FreeRTOS.h"
#include "semphr.h"

#define XBEE_TX_BUFF_SIZE 32
#define XBEE_RX_BUFF_SIZE 32

typedef struct {
    uint8_t rx_mem[XBEE_RX_BUFF_SIZE];
    uint8_t tx_mem[XBEE_TX_BUFF_SIZE];
    ring_buff_t rx_buff;
    ring_buff_t tx_buff;
    UARTConfig *uart;
} XBEE_t;

// does basic initialization of the xbee passed in as argument to this function.
void xb_init(XBEE_t *xb);

bool xb_transmit(XBEE_t *xb, uint8_t *buff, uint16_t len);

void xb_rx_callback(void *param, uint8_t datum);

bool xb_tx_callback(void *param, uint8_t * txAddress);

#endif /* SRC_XBEE_XBEE_H_ */
