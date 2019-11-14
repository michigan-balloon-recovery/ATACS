#ifndef NMEA_H
#define NMEA_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "gnss.h"

#define TALKER(x,y)             ((x << 8) | y)
#define SENTENCE(x,y,z)         ((x << 16) | (y << 8) | z)

// ----- Talker IDs ----- //
#define TALKER_UBX              TALKER('P','U')
#define TALKER_GPS              TALKER('G','P')
#define TALKER_GLONASS          TALKER('G','L')
#define TALKER_GALILEO          TALKER('G','A')
#define TALKER_BEIDOU           TALKER('G','B')
#define TALKER_GNSS             TALKER('G','N')

// ----- Sentence Format IDs ----- //
#define SENTENCE_DTM            SENTENCE('D','T','M')
#define SENTENCE_GBQ            SENTENCE('G','B','Q')
#define SENTENCE_GBS            SENTENCE('G','B','S')
#define SENTENCE_GGA            SENTENCE('G','G','A')
#define SENTENCE_GLL            SENTENCE('G','L','L')
#define SENTENCE_GLQ            SENTENCE('G','L','Q')
#define SENTENCE_GNQ            SENTENCE('G','N','Q')
#define SENTENCE_GNS            SENTENCE('G','N','S')
#define SENTENCE_GPQ            SENTENCE('G','P','Q')
#define SENTENCE_GRS            SENTENCE('G','R','S')
#define SENTENCE_GSA            SENTENCE('G','S','A')
#define SENTENCE_GST            SENTENCE('G','S','T')
#define SENTENCE_GSV            SENTENCE('G','S','V')
#define SENTENCE_RMC            SENTENCE('R','M','C')
#define SENTENCE_TXT            SENTENCE('T','X','T')
#define SENTENCE_VLW            SENTENCE('V','L','W')
#define SENTENCE_VTG            SENTENCE('V','T','G')
#define SENTENCE_ZDA            SENTENCE('Z','D','A')
#define SENTENCE_UBX            SENTENCE('B','X',',')

// ----- NMEA Faults ----- //
#define UNKNOWN_TALKER          -1
#define UNKNOWN_SENTENCE        -2
#define UNKNOWN_PROPRIETARY     -3
#define PAYLOAD_OVERFLOW        -4

bool gnss_nmea_queue(void *param, uint8_t datum);

int8_t gnss_nmea_decode(gnss_t *gnss_obj);

int8_t gnss_nmea_decode_standard_msg(gnss_t *gnss_obj, uint32_t sentence, uint8_t *payload);

int8_t gnss_nmea_decode_PUBX();

bool gnss_nmea_decode_field(uint8_t *payload, uint8_t **field, bool (*format_data)(uint8_t*, uint8_t*, void*), void *data);

// ----- field formatting decoders ----- //
bool gnss_nmea_field_latitude(uint8_t *start, uint8_t *end, gnss_coordinate_t *data);

bool gnss_nmea_field_longitude(uint8_t *start, uint8_t *end, gnss_coordinate_t *data);

bool gnss_nmea_field_direction(uint8_t *start, uint8_t *end, gnss_coordinate_t *data);

bool gnss_nmea_field_time(uint8_t *start, uint8_t *end, gnss_time_t *data);

bool gnss_nmea_field_char(uint8_t *start, uint8_t *end, char *output);

bool gnss_nmea_field_int32(uint8_t *start, uint8_t *end, uint32_t *output);

bool gnss_nmea_field_int8(uint8_t *start, uint8_t *end, uint8_t *output);


#ifdef __cplusplus
}
#endif

#endif /* NMEA_H */
