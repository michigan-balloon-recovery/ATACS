#include "aprs.h"

#include "ax25.h"
#include <string.h>
#include <stdio.h>

address_t addresses[2] = {
    {"APRS", 0},
    {"KD2OHS", 11},
//    {"WIDE2", 1}
};

void aprs_beacon(uint16_t altitude){
    // Header
    ax25_send_header(addresses, 2);
    ax25_send_byte('/');
    ax25_send_string("000000", 6);
    ax25_send_byte('h');
    ax25_send_string("0000.00N", strlen("0000.00N"));
    ax25_send_byte('/');
    ax25_send_string("00000.00E", strlen("00000.00E"));
    ax25_send_byte('O');
    ax25_send_string("000", strlen("000"));

    char temp_str[9];
    snprintf(temp_str, 10, "/A=%06d", altitude);
    ax25_send_string((uint8_t*)temp_str, 9); // Altitude (m)

    // Comment
    ax25_send_byte(' ');
    ax25_send_string("EECS 473", strlen("EECS 473"));

    // Footer
    ax25_send_footer();

    // Send!
    ax25_flush_frame();

}
