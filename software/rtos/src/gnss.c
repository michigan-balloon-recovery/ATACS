#include "gnss.h"

gnss_time_t UTC_time;
gnss_coordinate_pair_t position;

extern UARTConfig * prtInfList[5];

// ----- public API ----- //
void gnss_init(gnss_t *gnss_obj) {
    gnss_obj->decoding_message = false;

    // initialize ring buffers
    ring_buff_init(&gnss_obj->gnss_rx_buff, gnss_obj->gnss_rx_mem, GNSS_RX_BUFF_SIZE);
    ring_buff_init(&gnss_obj->gnss_tx_buff, gnss_obj->gnss_tx_mem, GNSS_TX_BUFF_SIZE);

    // initialize semaphore
    vSemaphoreCreateBinary(gnss_obj->uart_semaphore);

    // initialize UART
    UARTConfig a0_cnf = {
                    .moduleName = USCI_A0,
                    .portNum = PORT_3,
                    .RxPinNum = PIN5,
                    .TxPinNum = PIN4,
                    .clkRate = configCPU_CLOCK_HZ,
                    .baudRate = 9600L,
                    .clkSrc = UART_CLK_SRC_SMCLK,
                    .databits = 8,
                    .parity = UART_PARITY_NONE,
                    .stopbits = 1
                    };
    initUSCIUart(&a0_cnf, &gnss_obj->gnss_tx_buff, &gnss_obj->gnss_rx_buff);

    initUartRxCallback(&USCI_A0_cnf, &gnss_nmea_queue, gnss_obj);
}
