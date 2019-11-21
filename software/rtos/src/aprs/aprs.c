#include "aprs.h"

#include "afsk.h"
#include "ax25.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

void aprs_setup(const uint16_t pd_port,  const uint8_t pd_pin,
                const uint16_t ptt_port, const uint8_t ptt_pin,
                const uint16_t tx_port,  const uint8_t tx_pin){
    afsk_setup(pd_port, pd_pin, ptt_port, ptt_pin, tx_port, tx_pin);
}

address_t addresses[2] = {
    {"APRS", 0},
    {"KD2OHS", 11},
//    {"WIDE2", 1},
};

void aprs_beacon(gnss_time_t* time, gnss_coordinate_pair_t* loc, int32_t* alt){
    char temp_str[10];

    // Header
    ax25_send_header(addresses, 2);

    // Time
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
    snprintf(temp_str, 8, "%02hu%02hu.%02hu",
                 loc->longitude.deg, loc->longitude.min, loc->longitude.msec/600); //msec to %min
    ax25_send_string(temp_str);
    ax25_send_byte(loc->longitude.dir);

    // Course
//    ax25_send_byte('O');
//    ax25_send_string("000");

    // Altitude
    ax25_send_string("/A=");
    snprintf(temp_str, 7, "%06ld", alt);
    ax25_send_string(temp_str);

    // Comment
    ax25_send_byte(' ');
    ax25_send_string("MBuRST ATACS");

    // Footer
    ax25_send_footer();

    // Send!
    ax25_flush_frame();
}
