#include "gnss.h"

gnss_time_t UTC_time;
gnss_coordinate_pair_t position;

// ----- public API ----- //
void gnss_init(gnss_t *gnss_obj) {
    // initialize gpio
//    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P3, GPIO_PIN4);
//    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P3, GPIO_PIN5);


//    USCI_A_UART_initParam init_struct = {
//                .selectClockSource = USCI_A_UART_CLOCKSOURCE_SMCLK,
//                .clockPrescalar = 3,
//                .firstModReg = 0,
//                .secondModReg = 3,
//                .parity = USCI_A_UART_NO_PARITY,
//                .msborLsbFirst = USCI_A_UART_LSB_FIRST,
//                .numberofStopBits = USCI_A_UART_ONE_STOP_BIT,
//                .uartMode = USCI_A_UART_MODE,
//                .overSampling = USCI_A_UART_LOW_FREQUENCY_BAUDRATE_GENERATION
//    };
//    // configure uart module
//    if(STATUS_FAIL == USCI_A_UART_init(gnss_obj->uart_base_address, &init_struct)) return;

    // initialize the ring buffer
    ring_buff_init(&gnss_obj->gnss_rx_buff, gnss_obj->gnss_rx_mem, GNSS_RX_BUFF_SIZE);

    // enable the UART module
//    USCI_A_UART_enable(gnss_obj->uart_base_address);

    // enable interrupts
//    USCI_A_UART_clearInterrupt(gnss_obj->uart_base_address, USCI_A_UART_RECEIVE_INTERRUPT);
//    USCI_A_UART_enableInterrupt(gnss_obj->uart_base_address, USCI_A_UART_RECEIVE_INTERRUPT);
}
