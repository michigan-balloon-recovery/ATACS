
#include "nmea.h"

// ------------------------------------------------------------ //
// -------------------- private prototypes -------------------- //
// ------------------------------------------------------------ //

/*!
 * \brief Decode a standard NMEA sentence
 *
 * @param gnss_obj is the GNSS object
 * @param sentence_id is the identifier code for the sentence type using the SENTENCE() macro
 * @param payload is a byte array containing the sentence payload
 * \return NMEA fault code
 */
static int8_t gnss_nmea_decode_standard_msg(gnss_t *gnss_obj, uint32_t sentence_id, uint8_t *payload);

/*!
 * \brief Decode a PUBX (Ublox proprietary) NMEA sentence
 *
 * currently not implemented
 *
 */
static int8_t gnss_nmea_decode_PUBX();

/*!
 * \brief Decode a field in a NMEA sentence
 *
 * @param payload is a byte array containing the sentence payload
 * @param field is a pass by reference to the pointer indicating the start of the field
 * @format_data is a function pointer to the function that decodes the field
 * @param data is a void pointer to the data passed into the field decoder function
 * \return true if more fields remain in the sentence
 */
static bool gnss_nmea_decode_field(uint8_t *payload, uint8_t **field, bool (*format_data)(uint8_t*, uint8_t*, void*), void *data);

// ----- field formatting decoders ----- //
static bool gnss_nmea_field_latitude(uint8_t *start, uint8_t *end, void *data);
static bool gnss_nmea_field_longitude(uint8_t *start, uint8_t *end, void *data);
static bool gnss_nmea_field_direction(uint8_t *start, uint8_t *end, void *data);
static bool gnss_nmea_field_time(uint8_t *start, uint8_t *end, void *data);
static bool gnss_nmea_field_char(uint8_t *start, uint8_t *end, void *data);
static bool gnss_nmea_field_int32(uint8_t *start, uint8_t *end, void *data);
static bool gnss_nmea_field_int8(uint8_t *start, uint8_t *end, void *data);

// ----- utility functions ----- //
static inline long gnss_nmea_atoi(uint8_t *temp, uint8_t *ascii, uint8_t length);





// ---------------------------------------------------- //
// -------------------- public API -------------------- //
// ---------------------------------------------------- //

bool gnss_nmea_queue(gnss_t *gnss_obj, uint8_t datum) {
    ring_buff_t *buff = &gnss_obj->gnss_rx_buff;
    bool end_of_packet;

    // check if start of a packet
    if(datum == '$') {
        gnss_obj->decoding_message = true;
        ring_buff_write_clear_packet(buff);
        end_of_packet = false;
    }
    // check if end of a packet
    else if(gnss_obj->decoding_message && (datum == '\n') ) {
        gnss_obj->decoding_message = false;
        ring_buff_write_finish_packet(buff);
        end_of_packet = true;
        // give semaphore
    }
    // contents of message
    else if(gnss_obj->decoding_message){
        ring_buff_write(buff, datum);
        end_of_packet = false;
    }

    return end_of_packet;
}

int8_t gnss_nmea_decode(gnss_t *gnss_obj) {
    uint8_t address[5];
    uint8_t payload[GNSS_RX_MAX_PAYLOAD];
    uint16_t payload_size = 0;
    ring_buff_t *buff = &gnss_obj->gnss_rx_buff;

    // read in address
    uint16_t talker;
    uint32_t sentence;
    uint8_t i = 0;
    for(i = 0; i < 5; i++) {
        if(!ring_buff_read(buff, address + i)) {
            return EMPTY_BUFFER;
        }
    }
    talker = TALKER(address[0], address[1]);
    sentence = SENTENCE(address[2], address[3], address[4]);

    // read in payload
    i = 0;
    do {
        if(!ring_buff_read(buff, payload + i)) {
            return EMPTY_BUFFER;
        }
        i++;

        // enure payload isn't too long
        if(i > GNSS_RX_MAX_PAYLOAD) return PAYLOAD_OVERFLOW;
    } while(payload[i-1] != '*');

    // read in checksum
    char checksum_ascii[3] = {'0','0','\0'};
    uint8_t checksum;
    ring_buff_read(buff, (uint8_t*)checksum_ascii);
    ring_buff_read(buff, (uint8_t*)(checksum_ascii + 1));
    checksum = strtol(checksum_ascii, NULL, 16);

    // mark packet as read
    ring_buff_read_finish_packet(buff);

    switch (talker) {
    // GPS message
    case TALKER_GPS:
        return gnss_nmea_decode_standard_msg(gnss_obj, sentence, payload);
    // GLONASS message
    case TALKER_GLONASS:
        return gnss_nmea_decode_standard_msg(gnss_obj, sentence, payload);
    // Galileo message
    case TALKER_GALILEO:
        return gnss_nmea_decode_standard_msg(gnss_obj, sentence, payload);
    // BeiDou message
    case TALKER_BEIDOU:
        return gnss_nmea_decode_standard_msg(gnss_obj, sentence, payload);
    // multiple GNSS combination message
    case TALKER_GNSS:
        return gnss_nmea_decode_standard_msg(gnss_obj, sentence, payload);
    // proprietary PUBX message
    case TALKER_UBX:
        switch (sentence) {
        // all PUBX messages have the same sentence format field
        case SENTENCE_UBX:
            return UNKNOWN_PROPRIETARY;
        // invalid proprietary sentence format
        default:
            return UNKNOWN_PROPRIETARY;
        }
    // unknown talker
    default:
        return UNKNOWN_TALKER;
    }
}





// ----------------------------------------------------- //
// -------------------- private API -------------------- //
// ----------------------------------------------------- //

int8_t gnss_nmea_decode_standard_msg(gnss_t *gnss_obj, uint32_t sentence_id, uint8_t *payload) {
    uint8_t *ptr = payload;
    gnss_fix_t current_fix = {.quality = no_fix};
    switch (sentence_id) {
        // datum reference
        case SENTENCE_DTM:

            break;
        // poll a standard message (if current talker ID is GB)
        case SENTENCE_GBQ:

            break;
        // GNSS satellite fault detection
        case SENTENCE_GBS:

            break;
        // global positioning system fix data
        case SENTENCE_GGA:
            gnss_nmea_decode_field(payload, &ptr, &gnss_nmea_field_time, &(current_fix.time));
            gnss_nmea_decode_field(payload, &ptr, &gnss_nmea_field_latitude, &(current_fix.location.latitude));
            gnss_nmea_decode_field(payload, &ptr, &gnss_nmea_field_direction, &(current_fix.location.latitude));
            gnss_nmea_decode_field(payload, &ptr, &gnss_nmea_field_longitude, &(current_fix.location.longitude));
            gnss_nmea_decode_field(payload, &ptr, &gnss_nmea_field_direction, &(current_fix.location.longitude));
            gnss_nmea_decode_field(payload, &ptr, &gnss_nmea_field_int8, &(current_fix.quality));
            gnss_nmea_decode_field(payload, &ptr, &gnss_nmea_field_int8, &(current_fix.num_satellites));
            gnss_nmea_decode_field(payload, &ptr, NULL, NULL);
            gnss_nmea_decode_field(payload, &ptr, &gnss_nmea_field_int32, &(current_fix.altitude));
            if( (current_fix.quality != no_fix) && (current_fix.quality != 0xFFFF)) {
                gnss_obj->last_fix = current_fix;
            }
            break;
        // lattitude and longitude, with time of position fix and status
        case SENTENCE_GLL:

            break;
        // poll a standard message (if current talker ID is GL)
        case SENTENCE_GLQ:

            break;
        // poll a standard message (if current talker ID is GN)
        case SENTENCE_GNQ:

            break;
        // GNSS fix data
        case SENTENCE_GNS:

            break;
        // poll a standard message (if current talker ID is GP)
        case SENTENCE_GPQ:

            break;
        // GNSS range residuals
        case SENTENCE_GRS:

            break;
        // GNSS DOP and active satellites
        case SENTENCE_GSA:

            break;
        // GNSS pseduo range error statistics
        case SENTENCE_GST:

            break;
        // GNSS satellites in view
        case SENTENCE_GSV:

            break;
        // recommended minimum data
        case SENTENCE_RMC:

            break;
        // text transmission
        case SENTENCE_TXT:
            __no_operation();
            break;
        // dual ground/water distance
        case SENTENCE_VLW:

            break;
        // course over ground and ground speed
        case SENTENCE_VTG:

            break;
        // time and date
        case SENTENCE_ZDA:

            break;
        // unknown sentence format
        default:
            return UNKNOWN_SENTENCE;
    }
    return NO_FAULT;
}

int8_t gnss_nmea_decode_PUBX() {
    return 0;
}

bool gnss_nmea_decode_field(uint8_t *payload, uint8_t **field, bool (*format_data)(uint8_t*, uint8_t*, void*), void *data) {
    uint8_t *start;
    if(**field == ',') {
        *field += 1;
    }
    start = *field;
    while(*field < (payload + GNSS_RX_MAX_PAYLOAD)) {
        if(**field == '*') {
            if(format_data != NULL) {
                format_data(start, *field, data);
            }
            return false;
        }
        if(**field == ',') {
            if(format_data != NULL) {
                format_data(start, *field, data);
            }
            return true;
        }
        *field += 1;
    }
    return false;
}

bool gnss_nmea_field_latitude(uint8_t *start, uint8_t *end, void *data) {
    gnss_coordinate_t *coord = (gnss_coordinate_t*)data;
    uint8_t temp[6];
    static uint32_t samples[32] = {0};
    uint8_t i;

    // shift FIR buffer
    for(i = 31; i > 0; i--) {
        samples[i+1] = samples[i];
    }

    // ensure the correct length
    if( (end - start) != 10) {
        return false;
    }

    // initialize direction to be invalid
    coord->dir = 'e';

    // degrees
    samples[0] = ((uint32_t)gnss_nmea_atoi(temp, start, 2)) * 3600000;
//    coord->deg = gnss_nmea_atoi(temp, start, 2);
    start += 2;
    // minutes
    samples[0] += ((uint32_t)gnss_nmea_atoi(temp, start, 2)) * 60000;
//    coord->min = gnss_nmea_atoi(temp, start, 2);
    start += 2;

    // milliseconds
    start++; // skip decimal place
    samples[0] += ((uint32_t)gnss_nmea_atoi(temp, start, 5));
//    coord->msec = gnss_nmea_atoi(temp, start, 5) * 60 * 100000;

    // reset FIR accumulation
    coord->decMilliSec = 0;

    // calculate FIR output
    for(i = 0; i < 32; i++) {
        coord->decMilliSec += samples[i] >> 5;
    }
    return true;
}

bool gnss_nmea_field_longitude(uint8_t *start, uint8_t *end, void *data) {
    gnss_coordinate_t *coord = (gnss_coordinate_t*)data;
    uint8_t temp[6];
    static uint32_t samples[32] = {0};
    uint8_t i;


    // ensure the correct length
    if( (end - start) != 11) {
        return false;
    }

    // shift FIR buffer
    for(i = 31; i > 0; i--) {
        samples[i+1] = samples[i];
    }

    // initialize direction to be invalid
    coord->dir = 'e';

    // degrees
    samples[0] = ((uint32_t)gnss_nmea_atoi(temp, start, 3)) * 3600000;
//    coord->deg = gnss_nmea_atoi(temp, start, 3);
    start += 3;
    // minutes
    samples[0] += ((uint32_t)gnss_nmea_atoi(temp, start, 2)) * 60000;
//    coord->min = gnss_nmea_atoi(temp, start, 2);
    start += 2;

    // milliseconds
    start++; // skip decimal place
    samples[0] += ((uint32_t)gnss_nmea_atoi(temp, start, 5));

    // reset FIR accumulation
    coord->decMilliSec = 0;

    // calculate FIR output
    for(i = 0; i < 32; i++) {
        coord->decMilliSec += samples[i] >> 5;
    }

//    coord->msec = gnss_nmea_atoi(temp, start, 5) * 60 * 100000;
    return true;
}

bool gnss_nmea_field_direction(uint8_t *start, uint8_t *end, void *data) {
    gnss_coordinate_t *coord = (gnss_coordinate_t*)data;
    // ensure correct length
    if( (end - start) != 1) {
        return false;
    }

    coord->dir = *start;
    return true;
}

bool gnss_nmea_field_time(uint8_t *start, uint8_t *end, void *data) {
    gnss_time_t *time = (gnss_time_t*)data;
    uint8_t temp[3];

    // ensure correct length
    if( (end - start) != 9) {
        return false;
    }

    // hours
    time->hour = gnss_nmea_atoi(temp, start, 2);
    start += 2;

    // minutes
    time->min = gnss_nmea_atoi(temp, start, 2);
    start += 2;

    // seconds
    time->msec = 1000 * gnss_nmea_atoi(temp, start, 2);
    start += 2;

    // milliseconds
    start++; // skip decimal place
    time->msec += 10 * gnss_nmea_atoi(temp, start, 2);
    return true;
}

bool gnss_nmea_field_char(uint8_t *start, uint8_t *end, void *data) {
    char *output = (char *)data;
    // ensure correct length
    if( (end - start) != 1) {
        *output = 0;
        return false;
    }

    *output = *start;
    return true;
}

bool gnss_nmea_field_int32(uint8_t *start, uint8_t *end, void *data) {
    int32_t *output = (int32_t *)data;
    uint8_t temp[10];
    if( (end - start) == 0) {
        *output = 0xFFFFFFFF;
        return false;
    }

    *output = gnss_nmea_atoi(temp, start, end - start);
    return true;
}

bool gnss_nmea_field_int8(uint8_t *start, uint8_t *end, void *data) {
    uint8_t *output = (uint8_t*)data;
    uint8_t temp[10];
    if( (end - start) == 0) {
        *output = 0xFF;
        return false;
    }

    *output = gnss_nmea_atoi(temp, start, end - start);
    return true;
}

static inline long gnss_nmea_atoi(uint8_t *temp, uint8_t *ascii, uint8_t length) {
    temp[length] = '\0';
    memcpy(temp, ascii, length);
    return atol((char *)temp);
}

