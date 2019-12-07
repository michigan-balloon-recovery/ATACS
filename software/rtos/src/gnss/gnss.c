#include "gnss.h"
/*-------------------------------------------------------------------------------- /
/ ATACS GNSS (Global Navigation Satellite System) driver
/ -------------------------------------------------------------------------------- /
/ Part of the ATACS (Aerial Termination And Communication System) project
/       https://github.com/michigan-balloon-recovery/ATACS
/       released under the GPLv2 license (see ATACS/LICENSE in git repository)
/ Creation Date: November 2019
/ Contributors: Paul Young
/ --------------------------------------------------------------------------------*/




// ---------------------------------------------------------- //
// -------------------- global variables -------------------- //
// ---------------------------------------------------------- //

gnss_t GNSS = {.is_valid = false, .last_fix = 0};

extern UARTConfig * prtInfList[5];





// ------------------------------------------------------------ //
// -------------------- private prototypes -------------------- //
// ------------------------------------------------------------ //

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
static void gnss_rx_callback(void *param, uint8_t datum);

/*!
 * \brief initializes the GNSS object
 * 
 * @param gnss_obj the GNSS object to be initialized
 * \return None
 */
static void gnss_init(gnss_t *gnss_obj);





// -------------------------------------------------------------- //
// -------------------- FreeRTOS task & init -------------------- //
// -------------------------------------------------------------- //

void task_gnss(void) {
    gnss_init(&GNSS);
    while (1) {
        // wait for completed message to be received
        xSemaphoreTake(GNSS.uart_semaphore, portMAX_DELAY);
        gnss_nmea_decode(&GNSS);
    }
}

static void gnss_init(gnss_t *gnss_obj) {
    gnss_obj->decoding_message = false;
    gnss_obj->uart_module = USCI_A0;

    // initialize ring buffers
    ring_buff_init(&gnss_obj->gnss_rx_buff, gnss_obj->gnss_rx_mem, GNSS_RX_BUFF_SIZE);
    ring_buff_init(&gnss_obj->gnss_tx_buff, gnss_obj->gnss_tx_mem, GNSS_TX_BUFF_SIZE);

    // initialize semaphores
    gnss_obj->uart_semaphore = xSemaphoreCreateCounting(255,0);
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

    initUartRxCallback(&USCI_A0_cnf, &gnss_rx_callback, gnss_obj);

    GPIO_setAsOutputPin(GPIO_PORT_P8, GPIO_PIN4);
    gnss_obj->is_valid = true;
}





// ---------------------------------------------------- //
// -------------------- public API -------------------- //
// ---------------------------------------------------- //

bool gnss_get_time(gnss_t *gnss_obj, gnss_time_t *time) {
    bool data_valid = false;
    if(!gnss_obj->is_valid || xSemaphoreTake(gnss_obj->data_mutex, 100) == pdFALSE) {
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
    if(!gnss_obj->is_valid || xSemaphoreTake(gnss_obj->data_mutex, 100) == pdFALSE) {
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
    if(!gnss_obj->is_valid || xSemaphoreTake(gnss_obj->data_mutex, 100) == pdFALSE) {
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

//int32_t gnss_coord_to_decMilliSec(gnss_coordinate_t *coordinate) {
//    return ((uint32_t) coordinate->deg) * 3600000 + ((uint32_t) coordinate->min) * 60000 + ((uint32_t) coordinate->msec);
//}

void gnss_disable_interrupts(gnss_t *gnss_obj) {
    while(gnss_obj->decoding_message == true);
    *prtInfList[gnss_obj->uart_module]->usciRegs->IE_REG &= ~UCRXIE;
}

void gnss_enable_interrupts(gnss_t *gnss_obj) {
    enableUartRx(prtInfList[gnss_obj->uart_module]);
}





// ----------------------------------------------------- //
// -------------------- private API -------------------- //
// ----------------------------------------------------- //

static void gnss_rx_callback(void *param, uint8_t datum) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    gnss_t *gnss_obj = (gnss_t *)param;

    gnss_nmea_queue(gnss_obj, datum);

    // release parsing task
    xSemaphoreGiveFromISR(gnss_obj->uart_semaphore, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
