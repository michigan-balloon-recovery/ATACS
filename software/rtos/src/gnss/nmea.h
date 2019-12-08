#ifndef NMEA_H
#define NMEA_H

#ifdef __cplusplus
extern "C" {
#endif





// -------------------------------------------------------------- //
// -------------------- include dependencies -------------------- //
// -------------------------------------------------------------- //

// standard libraries
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
// application drivers
#include "gnss.h"





// ------------------------------------------------------- //
// -------------------- public macros -------------------- //
// ------------------------------------------------------- //

#define TALKER(x,y)                 ( (x << 8) | y )
#define SENTENCE(x,y,z)             ( ( (uint32_t)x << 16 ) | ( (uint32_t)y << 8 ) | (uint32_t)z )

// ----- Talker IDs ----- //
#define TALKER_UBX                  TALKER('P','U')
#define TALKER_GPS                  TALKER('G','P')
#define TALKER_GLONASS              TALKER('G','L')
#define TALKER_GALILEO              TALKER('G','A')
#define TALKER_BEIDOU               TALKER('G','B')
#define TALKER_GNSS                 TALKER('G','N')

// ----- Sentence Format IDs ----- //
#define SENTENCE_DTM                SENTENCE('D','T','M')
#define SENTENCE_GBQ                SENTENCE('G','B','Q')
#define SENTENCE_GBS                SENTENCE('G','B','S')
#define SENTENCE_GGA                SENTENCE('G','G','A')
#define SENTENCE_GLL                SENTENCE('G','L','L')
#define SENTENCE_GLQ                SENTENCE('G','L','Q')
#define SENTENCE_GNQ                SENTENCE('G','N','Q')
#define SENTENCE_GNS                SENTENCE('G','N','S')
#define SENTENCE_GPQ                SENTENCE('G','P','Q')
#define SENTENCE_GRS                SENTENCE('G','R','S')
#define SENTENCE_GSA                SENTENCE('G','S','A')
#define SENTENCE_GST                SENTENCE('G','S','T')
#define SENTENCE_GSV                SENTENCE('G','S','V')
#define SENTENCE_RMC                SENTENCE('R','M','C')
#define SENTENCE_TXT                SENTENCE('T','X','T')
#define SENTENCE_VLW                SENTENCE('V','L','W')
#define SENTENCE_VTG                SENTENCE('V','T','G')
#define SENTENCE_ZDA                SENTENCE('Z','D','A')
#define SENTENCE_UBX                SENTENCE('B','X',',')
#define SENTENCE_UBX_CONFIG         41
#define SENTENCE_UBX_POSITION       00
#define SENTENCE_UBX_RATE           40
#define SENTENCE_UBX_SVSTATUS       03
#define SENTENCE_UBX_TIME           04

// ----- NMEA Faults ----- //
#define NMEA_NO_FAULT                0
#define NMEA_UNKNOWN_TALKER          -1
#define NMEA_UNKNOWN_SENTENCE        -2
#define NMEA_UNKNOWN_PROPRIETARY     -3
#define NMEA_PAYLOAD_OVERFLOW        -4
#define NMEA_EMPTY_BUFFER            -5





// ----------------------------------------------------------- //
// -------------------- public prototypes -------------------- //
// ----------------------------------------------------------- //

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
 * \return error code defined by "NMEA Faults" macros
 * 
 */
int8_t gnss_nmea_decode(gnss_t *gnss_obj);

#ifdef __cplusplus
}
#endif

#endif /* NMEA_H */
