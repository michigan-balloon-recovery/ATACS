#include "aprs.h"

#include "FreeRTOS.h"
#include "afsk.h"
#include "ax25.h"
#include "uart.h"
#include "sensors.h"
#include "rockblock.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

extern gnss_t GNSS;
extern ROCKBLOCK_t rb;
extern sensor_data_t sensor_data;

void configDRA818V(const char* freq_str);

/*
 * Exported Functions Definitions
 */

void task_aprs() {
    const portTickType xFrequency = APRS_PERIOD_MS / portTICK_RATE_MS;
    portTickType xLastWakeTime = xTaskGetTickCount();

    // P1.2 is PD (sleep)
    // P1.3 is PTT (push-to-talk)
    // P2.2 is MIC_IN (AFSK tones go here)
    // ptt_active_high is false for DRA818
    aprs_setup(GPIO_PORT_P1, GPIO_PIN2,
               GPIO_PORT_P1, GPIO_PIN3,
               GPIO_PORT_P2, GPIO_PIN2,
               false);

    // wait for all sensors to initialize.
    while(!sensor_data.humid_init);
    while(!sensor_data.pres_init);
    while(!GNSS.is_valid);

    while (1){
        vTaskDelayUntil(&xLastWakeTime, xFrequency);

        // Fetch GPS and sensor data
        gnss_time_t time = {0};
        gnss_coordinate_pair_t loc = {0};
        loc.latitude.dir = 'N';
        loc.longitude.dir = 'E';
        int32_t alt = -1;

        gnss_get_time(&GNSS, &time);
        gnss_get_location(&GNSS, &loc);
        gnss_get_altitude(&GNSS, &alt);

        // Disable everything with interrupts so that our sine is clean
        gnss_disable_interrupts(&GNSS);
        while(!rb_disable_interrupts(&rb));
//        while(!sens_disable_interrupts());

        vTaskSuspendAll();
        aprs_beacon(&time, &loc, &alt);
        xTaskResumeAll();

        gnss_enable_interrupts(&GNSS);
        rb_enable_interrupts(&rb);
//        sens_enable_interrupts();
    }
}

void aprs_setup(const uint16_t pd_port,  const uint8_t pd_pin,
                const uint16_t ptt_port, const uint8_t ptt_pin,
                const uint16_t tx_port,  const uint8_t tx_pin,
                const bool ptt_active_level){
    // Turn on radio
    GPIO_setAsOutputPin(pd_port, pd_pin);
    GPIO_setOutputHighOnPin(pd_port, pd_pin);
    __delay_cycles(2 * configCPU_CLOCK_HZ);

    // Send configuration command to radio
    configDRA818V("144.3900");

    // Initialize AFSK library
    afsk_setup(ptt_port, ptt_pin, tx_port, tx_pin, ptt_active_level);
}

address_t addresses[2] = {
    {"APRS", 0},
    {"KD2OHS", 11},
//    {"WIDE2", 1},
};

void aprs_beacon(gnss_time_t* time, gnss_coordinate_pair_t* loc, int32_t* alt){
    char temp_str[16];

    // Header
    ax25_send_header(addresses, 2);

    // Time
//    ax25_send_string("/000000h");
    ax25_send_byte('/');
    snprintf(temp_str, 7, "%02hu%02hu%02hu", time->hour, time->min, time->msec/1000);
    ax25_send_string(temp_str);
    ax25_send_byte('h');

    //int32_t gnss_coord_to_decMilliSec(gnss_coordinate_t *coordinate) {
    //    return ((uint32_t) coordinate->deg) * 3600000 + ((uint32_t) coordinate->min) * 60000 + ((uint32_t) coordinate->msec);
    //}

    // Latitude
//    ax25_send_string("0000.00N");
    uint32_t deg = loc->latitude.decMilliSec / 3600000;
    loc->latitude.decMilliSec -= deg*3600000;
    uint32_t min = loc->latitude.decMilliSec / 600000;
    loc->latitude.decMilliSec -= min*60000;
    uint32_t percent_min = loc->latitude.decMilliSec / 600;
    snprintf(temp_str, 8, "%02hu%02hu.%02hu", (uint8_t)deg, (uint8_t)min, (uint8_t)percent_min);
    ax25_send_string(temp_str);
    ax25_send_byte(loc->latitude.dir);

    ax25_send_byte('/');

    // Longitude
//    ax25_send_string("00000.00E");
    deg = loc->longitude.decMilliSec / 3600000;
    loc->longitude.decMilliSec -= deg*3600000;
    min = loc->longitude.decMilliSec / 600000;
    loc->longitude.decMilliSec -= min*60000;
    percent_min = loc->longitude.decMilliSec / 600;
    snprintf(temp_str, 9, "%03hu%02hu.%02hu", (uint8_t)deg, (uint8_t)min, (uint8_t)percent_min);
    ax25_send_string(temp_str);
    ax25_send_byte(loc->longitude.dir);

    // Course
//    ax25_send_byte('O');
//    ax25_send_string("000");

    // Altitude
    ax25_send_string("/A=");
    snprintf(temp_str, 7, "%6ld", *alt);
    ax25_send_string(temp_str);

    // Comment
    ax25_send_byte(' ');
    ax25_send_string("MBuRST ATACS");

    // Footer
    ax25_send_footer();

    // Send!
    ax25_flush_frame();
}

/*
 * Local Functions Definitions
 */
void configDRA818V(const char* freq_str){
    // UART initialization
    UARTConfig a3_cnf = {
                    .moduleName = USCI_A3,
                    .portNum = PORT_10,
                    .RxPinNum = PIN5,
                    .TxPinNum = PIN4,
                    .clkRate = configCPU_CLOCK_HZ,
                    .baudRate = 9600,
                    .clkSrc = UART_CLK_SRC_SMCLK,
                    .databits = 8,
                    .parity = UART_PARITY_NONE,
                    .stopbits = 1
    };

    uint8_t buf[50];
    ring_buff_t txbuf;
    ring_buff_init(&txbuf, buf, 50);
    initUSCIUart(&a3_cnf, &txbuf, NULL);

    char cmd[50];
    sprintf(cmd, "AT+DMOSETGROUP=0,%s,%s,0000,4,0000\r\n", freq_str, freq_str);

    uartSendDataBlocking(&USCI_A3_cnf, (uint8_t*)cmd, strlen(cmd));

    disableUSCIUartInterrupts(&USCI_A3_cnf);
}



