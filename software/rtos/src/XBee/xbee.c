/*
 * xbee.c
 *
 *  Created on: Nov 17, 2019
 *      Author: SSS
 */

#include "xbee.h"

XBEE_t XBee;

void xb_init(XBEE_t *xb) {

    ring_buff_init(&xb->rx_buff, xb->rx_mem, XBEE_RX_BUFF_SIZE);
    ring_buff_init(&xb->tx_buff, xb->tx_mem, XBEE_TX_BUFF_SIZE);

    // initialize semaphore
    //vSemaphoreCreateBinary(xb->uart_semaphore);

    // initialize UART
    UARTConfig a2_cnf = {
                    .moduleName = USCI_A2,
                    .portNum = PORT_9,
                    .RxPinNum = PIN5,
                    .TxPinNum = PIN4,
                    .clkRate = configCPU_CLOCK_HZ,
                    .baudRate = 9600L,
                    .clkSrc = UART_CLK_SRC_SMCLK,
                    .databits = 8,
                    .parity = UART_PARITY_NONE,
                    .stopbits = 1
    };
    xb->uart = &USCI_A2_cnf;

    initUSCIUart(&a2_cnf, &xb->tx_buff, &xb->rx_buff);

    initUartRxCallback(xb->uart, NULL, xb);
    initUartTxCallback(xb->uart, NULL, xb);
}

void xb_rx_callback(void *param, uint8_t datum) {
    //stub
    return;
}

bool xb_tx_callback(void *param, uint8_t * txAddress) {
    //stub
    return true;
}

bool xb_transmit(XBEE_t *xb, uint8_t *buff, uint16_t len) {
    return uartSendDataInt(xb->uart, buff, len) == UART_SUCCESS;
}


