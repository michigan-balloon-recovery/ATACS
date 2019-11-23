#include "gnss.h"

extern UARTConfig * prtInfList[5];

gnss_t GNSS = {.is_valid = false};

// ----- public API ----- //

void task_gnss(void) {
    gnss_init(&GNSS);

    while (1) {
        xSemaphoreTake(GNSS.uart_semaphore, portMAX_DELAY);
        gnss_nmea_decode(&GNSS);
    }
}

void gnss_init(gnss_t *gnss_obj) {
    gnss_obj->decoding_message = false;
    gnss_obj->uart_module = USCI_A0;

    // initialize ring buffers
    ring_buff_init(&gnss_obj->gnss_rx_buff, gnss_obj->gnss_rx_mem, GNSS_RX_BUFF_SIZE);
    ring_buff_init(&gnss_obj->gnss_tx_buff, gnss_obj->gnss_tx_mem, GNSS_TX_BUFF_SIZE);

    // initialize semaphores
    gnss_obj->uart_semaphore = xSemaphoreCreateBinary();
    gnss_obj->data_mutex = xSemaphoreCreateMutex();

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

    initUartRxCallback(&USCI_A0_cnf, &gnss_nmea_rx_callback, gnss_obj);
    gnss_obj->is_valid = true;
}

bool gnss_get_time(gnss_t *gnss_obj, gnss_time_t *time) {
    bool data_valid = false;
    if(xSemaphoreTake(gnss_obj->data_mutex, 100) == pdFALSE) {
        return false;
    }
    // check if the previous fix is valid
    if(gnss_obj->last_fix.quality == no_fix) {

        data_valid = false;
    }
    else {
        *time = gnss_obj->last_fix.time;
        data_valid = true;
    }

    xSemaphoreGive(gnss_obj->data_mutex);
    return data_valid;
}

bool gnss_get_location(gnss_t *gnss_obj, gnss_coordinate_pair_t *location) {
    bool data_valid = false;
    if(xSemaphoreTake(gnss_obj->data_mutex, 100) == pdFALSE) {
        return false;
    }
    // check if the previous fix is valid
    if(gnss_obj->last_fix.quality == no_fix) {

        data_valid = false;
    }
    else {
        *location = gnss_obj->last_fix.location;
        data_valid = true;
    }

    xSemaphoreGive(gnss_obj->data_mutex);
    return data_valid;
}

bool gnss_get_altitude(gnss_t *gnss_obj, int32_t *altitude) {
    bool data_valid = false;
    if(xSemaphoreTake(gnss_obj->data_mutex, 100) == pdFALSE) {
        return false;
    }
    // check if the previous fix is valid
    if(gnss_obj->last_fix.quality == no_fix) {

        data_valid = false;
    }
    else {
        *altitude = gnss_obj->last_fix.altitude;
        data_valid = true;
    }

    xSemaphoreGive(gnss_obj->data_mutex);
    return data_valid;
}

int32_t gnss_coord_to_decSec(gnss_coordinate_t *coordinate) {
    return ((uint32_t) coordinate->deg) * 3600 + ((uint32_t) coordinate->min) * 60 + ((uint32_t) coordinate->msec) / 1000;
}
