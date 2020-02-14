/*
 * xbee.c
 *
 *  Created on: Nov 17, 2019
 *      Author: SSS
 */

#include "xbee.h"


void xb_init(XBEE_t *xb) {

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

    initUartRxCallback(xb->uart, &xb_rx_callback, xb);
    initUartTxCallback(xb->uart, &xb_tx_callback, xb);
}

void xb_rx_callback(void *param, uint8_t datum) {

}

bool xb_tx_callback(void *param, uint8_t * txAddress) {
    static int i = 0;
    XBEE_t *xb = (XBEE_t *) param;

    *txAddress = xb->tx_buff[i++];

    if(i == XBEE_TX_BUFF_SIZE) {
        return false;
        i = 0;
    }
    return true;

}

bool xb_transmit(XBEE_t *xb, uint8_t *buff, uint16_t len) {
    int i = 0;
    for(i = 0; i < len; i++)
        xb->tx_buff[i] = buff[i];

    uartSendDataInt(xb->uart, xb->tx_buff[i], len);
}


