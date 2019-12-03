#ifndef NMEA_H
#define NMEA_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "gnss.h"





// ------------------------------------------------------- //
// -------------------- public macros -------------------- //
// ------------------------------------------------------- //

#define TALKER(x,y)             ( (x << 8) | y )
#define SENTENCE(x,y,z)         ( ( (uint32_t)x << 16 ) | ( (uint32_t)y << 8 ) | (uint32_t)z )

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
 */
int8_t gnss_nmea_decode(gnss_t *gnss_obj);

#ifdef __cplusplus
}
#endif

#endif /* NMEA_H */
