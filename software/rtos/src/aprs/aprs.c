#include "aprs.h"

#include "FreeRTOS.h"
#include "afsk.h"
#include "ax25.h"
#include "uart.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

extern gnss_t GNSS;

void configDRA818V(const char* freq_str);

/*
 * Exported Functions Definitions
 */

void task_aprs() {
    const portTickType xFrequency = APRS_PERIOD_MS / portTICK_RATE_MS;
    portTickType xLastWakeTime = xTaskGetTickCount();

    // P1.2 is PD (sleep)
    // P1.3 is PTT (push-to-talk)
    // MIC_IN is routed to P2.1 on board rev 1.0, but P2.1 and P2.2 are
    // bridged on the board because PWM from T1.0(P2.1) is inconvenient
    // ptt_active_high is false for DRA818
//    aprs_setup(GPIO_PORT_P1, GPIO_PIN2,
//               GPIO_PORT_P1, GPIO_PIN3,
//               GPIO_PORT_P2, GPIO_PIN2,
//               false);

    // Devboard with HX-1 breakout
    aprs_setup(GPIO_PORT_P1, GPIO_PIN2,
               GPIO_PORT_P2, GPIO_PIN0,
               GPIO_PORT_P2, GPIO_PIN2,
               true);

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

        // Disable scheduler so that transmission is not interrupted by FreeRTOS ticks
        vTaskSuspendAll();
        aprs_beacon(&time, &loc, &alt);
        xTaskResumeAll();
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

    // Latitude
//    ax25_send_string("0000.00N");
    snprintf(temp_str, 8, "%02hu%02hu.%02hu",
             loc->latitude.deg, loc->latitude.min, loc->latitude.msec/600); //msec to %min
    ax25_send_string(temp_str);
    ax25_send_byte(loc->latitude.dir);

    ax25_send_byte('/');

    // Longitude
//    ax25_send_string("00000.00E");
    snprintf(temp_str, 9, "%03hu%02hu.%02hu",
                 loc->longitude.deg, loc->longitude.min, loc->longitude.msec/600); //msec to %min
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

    uartSendDataInt(&USCI_A3_cnf, (uint8_t*)cmd, strlen(cmd));

    // Clear RX interrupt flag
    *(USCI_A3_cnf.usciRegs->IFG_REG) &= ~UCRXIFG;
    // Disable RX interrupt
    *(USCI_A3_cnf.usciRegs->IE_REG) &= ~UCRXIE;

    // Clear TX interrupt flag
    *(USCI_A3_cnf.usciRegs->IFG_REG) &= ~UCTXIFG;
    // Disable TX interrupt
    *(USCI_A3_cnf.usciRegs->IE_REG) &= ~UCTXIE;

//    disableUSCIUart(&a3_cnf);
}



