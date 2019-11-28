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
#define SENTENCE(x,y,z)         (( (uint32_t)x << 16 ) | ( (uint32_t)y << 8 ) | (uint32_t)z )

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
#define NO_FAULT                0
#define UNKNOWN_TALKER          -1
#define UNKNOWN_SENTENCE        -2
#define UNKNOWN_PROPRIETARY     -3
#define PAYLOAD_OVERFLOW        -4
#define EMPTY_BUFFER            -5

/*!
 * \brief UART RX callback function
 * 
 * Callback function to be called within the UART RX Interrupt Service Routine (ISR).
 * Identifies start or end of sentence and adds data to the ring buffer if part of a sentence.
 * Must be registered with the UART driver with initUartRxCallback().
 * 
 * @param param is the passed in parameter from the UART driver. should be set as the GNSS object.
 * @param datum is the byte read over uart passed in by the UART driver.
 * \return None
 * 
 */
void gnss_nmea_rx_callback(void *param, uint8_t datum);

/*!
 * \brief NMEA queue byte
 * 
 * Identifies start or end of sentence and adds data to the ring buffer if part of an NMEA sentence.
 * 
 * @param gnss_obj is the GNSS object
 * @datum is the byte to add to the buffer
 * \return true if end of the sentence
 * 
 */
bool gnss_nmea_queue(gnss_t *gnss_obj, uint8_t datum);

/*!
 * \brief Decodes the next NMEA sentence in the ring buffer
 * 
 * 
 * @param gnss_obj is the GNSS object
 */
int8_t gnss_nmea_decode(gnss_t *gnss_obj);

/*!
 * \brief Decode a standard NMEA sentence
 * 
 * @param gnss_obj is the GNSS object
 * @param sentence_id is the identifier code for the sentence type using the SENTENCE() macro
 * @param payload is a byte array containing the sentence payload
 * \return NMEA fault code
 */
int8_t gnss_nmea_decode_standard_msg(gnss_t *gnss_obj, uint32_t sentence_id, uint8_t *payload);

/*!
 * \brief Decode a PUBX (Ublox proprietary) NMEA sentence
 * 
 * currently not implemented
 * 
 */
int8_t gnss_nmea_decode_PUBX();

/*!
 * \brief Decode a field in a NMEA sentence
 * 
 * @param payload is a byte array containing the sentence payload
 * @param field is a pass by reference to the pointer indicating the start of the field
 * @format_data is a function pointer to the function that decodes the field
 * @param data is a void pointer to the data passed into the field decoder function
 * \return true if more fields remain in the sentence
 */
bool gnss_nmea_decode_field(uint8_t *payload, uint8_t **field, bool (*format_data)(uint8_t*, uint8_t*, void*), void *data);

// ----- field formatting decoders ----- //
bool gnss_nmea_field_latitude(uint8_t *start, uint8_t *end, void *data);

bool gnss_nmea_field_longitude(uint8_t *start, uint8_t *end, void *data);

bool gnss_nmea_field_direction(uint8_t *start, uint8_t *end, void *data);

bool gnss_nmea_field_time(uint8_t *start, uint8_t *end, void *data);

bool gnss_nmea_field_char(uint8_t *start, uint8_t *end, void *data);

bool gnss_nmea_field_int32(uint8_t *start, uint8_t *end, void *data);

bool gnss_nmea_field_int8(uint8_t *start, uint8_t *end, void *data);


#ifdef __cplusplus
}
#endif

#endif /* NMEA_H */
