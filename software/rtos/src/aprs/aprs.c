#include "aprs.h"
/*-------------------------------------------------------------------------------- /
/ ATACS APRS (Automatic Packet Reporting System) driver
/ -------------------------------------------------------------------------------- /
/ Part of the ATACS (Aerial Termination And Communication System) project
/       https://github.com/michigan-balloon-recovery/ATACS
/       released under the GPLv2 license (see ATACS/LICENSE in git repository)
/ Creation Date: November 2019
/ Contributors: Justin Shetty
/ --------------------------------------------------------------------------------*/



// ---------------------------------------------------------- //
// -------------------- global variables -------------------- //
// ---------------------------------------------------------- //

extern gnss_t GNSS;
extern ROCKBLOCK_t rb;
extern sensor_data_t sensor_data;
address_t addresses[2] = {
    {APRS_DEST_CALLSIGN, 0},
    {APRS_SRC_CALLSIGN, 11},
};


// ------------------------------------------------------------ //
// -------------------- private prototypes -------------------- //
// ------------------------------------------------------------ //

/*!
 * \brief Initializes appropriate hardware to enable APRS transmissions
 *
 * @param pd_port Port to which the radio's ON/OFF pin is connected
 * @param pd_pin  Pin to which the radio's ON/OFF pin is connected
 * @param ptt_port Port to which the radio's PTT pin is connected
 * @param ptt_pin  Pin to which the radio's PTT pin is connected
 * @param tx_port Port to which the radio's MIC pin is connected
 * @param tx_pin  Pin to which the radio's MIC pin is connected
 * @param ptt_active_high Whether activity level (true: transmit when high)
 * \return None
 */
void aprs_setup(const uint16_t pd_port,  const uint8_t pd_pin,
                const uint16_t ptt_port, const uint8_t ptt_pin,
                const uint16_t tx_port,  const uint8_t tx_pin,
                const bool ptt_active_high);

/*!
 * \brief Turns on the DRA818V and sets the frequency
 *
 * @param pd_port Port to which the radio's ON/OFF pin is connected
 * @param pd_pin  Pin to which the radio's ON/OFF pin is connected
 * @param freq_str Pointer to beginning of string which contains the desired transmit frequency
 * \return None
 */
void configDRA818V(const uint16_t pd_port, const uint16_t pd_pin,
                   const char* freq_str);

/*!
 * \brief Transmits an APRS packet
 *
 * @param time Pointer to a gnss_time_t object which contains the current time
 * @param loc  Pointer to a gnss_coordinate_pair_t object which contains the current position
 * @param alt  Pointer to a uint32_t which contains the current altitude in meters
 * \return None
 */
void aprs_beacon(gnss_time_t* time, gnss_coordinate_pair_t* loc, int32_t* alt);

// -------------------------------------------------------------- //
// ----------------------- FreeRTOS task ------------------------ //
// -------------------------------------------------------------- //

void task_aprs() {
    const portTickType xFrequency = APRS_PERIOD_MS / portTICK_RATE_MS;
    portTickType xLastWakeTime = xTaskGetTickCount();

    aprs_setup(APRS_PD_PORT, APRS_PD_PIN,
               APRS_PTT_PORT, APRS_PTT_PIN,
               APRS_PWM_PORT, APRS_PWM_PIN,
               APRS_ACTIVE_HIGH);

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
        if(!gnss_get_location(&GNSS, &loc)){
            continue;
        }
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


// ----------------------------------------------------- //
// -------------------- private API -------------------- //
// ----------------------------------------------------- //

void aprs_setup(const uint16_t pd_port,  const uint8_t pd_pin,
                const uint16_t ptt_port, const uint8_t ptt_pin,
                const uint16_t tx_port,  const uint8_t tx_pin,
                const bool ptt_active_level){

    // Send configuration command
    configDRA818V(pd_port, pd_pin, "144.3900");

    // Initialize AFSK library
    afsk_setup(ptt_port, ptt_pin, tx_port, tx_pin, ptt_active_level);
}

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
    uint32_t min = loc->latitude.decMilliSec / 60000;
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
    min = loc->longitude.decMilliSec / 60000;
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

void configDRA818V(const uint16_t pd_port, const uint16_t pd_pin,
                   const char* freq_str){
    // Turn on radio
    GPIO_setAsOutputPin(pd_port, pd_pin);
    GPIO_setOutputHighOnPin(pd_port, pd_pin);
    __delay_cycles(2 * configCPU_CLOCK_HZ);

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
