/*
 * xbee.c
 *
 *  Created on: Nov 17, 2019
 *      Author: SSS
 */

#include "xbee.h"

void xb_init(XBEE_t *xb) {

    ring_buff_init(&xb->rx_buff, &xb->rx_mem, XBEE_RX_BUFF_SIZE);
    ring_buff_init(&xb->tx_buff, &xb->tx_mem, XBEE_TX_BUFF_SIZE);

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

    initUSCIUart(&a2_cnf, &xb->tx_buff, &xb->rx_buff);

    initUartRxCallback(&USCI_A2_cnf, &xb_rx_callback, xb);
    initUartTxCallback(&USCI_A2_cnf, &xb_tx_callback, xb);
}

