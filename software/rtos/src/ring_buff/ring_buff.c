#include "ring_buff.h"

// ------------------------------------------------------------ //
// -------------------- private prototypes -------------------- //
// ------------------------------------------------------------ //

/*!
 * \brief Increment ring buffer pointer
 *
 * @param buff is the ring_buff_t instance
 * @param ptr is a pointer to the ring buffer pointer to be incremented
 * \return None
 */
static inline void ring_buff_increment_ptr(ring_buff_t *buff, uint8_t **ptr);

/*!
 * \brief Calculate number of bytes between two ring buffer pointers
 * 
 * measures length from start_ptr to end_ptr.
 * if two pointers are the same, returns full length of the buffer.
 *
 * @param buff is the ring_buff_t instance
 * @param start_ptr is the start of the region to measure
 * @param end_ptr is the end of the region to measure
 * \return length between pointers
 */
static uint16_t ring_buff_length(ring_buff_t *buff, uint8_t *start_ptr, uint8_t *end_ptr);





// ---------------------------------------------------- //
// -------------------- public API -------------------- //
// ---------------------------------------------------- //

void ring_buff_init(ring_buff_t *buff, uint8_t *memory, uint16_t size) {
    buff->size = size;
    buff->start = memory;
    buff->read_ptr_byte = memory;
    buff->read_ptr_packet = memory;
    buff->write_ptr_byte = memory;
    buff->write_ptr_packet = memory;
}

bool ring_buff_write(ring_buff_t *buff, uint8_t datum) {
    if(ring_buff_length(buff, buff->write_ptr_byte, buff->read_ptr_packet) <= 1) {
        return false;
    }

    *buff->write_ptr_byte = datum;

    ring_buff_increment_ptr(buff, &buff->write_ptr_byte);
    return true;
}

uint16_t ring_buff_write_finish_packet(ring_buff_t *buff) {
    uint16_t packet_size;

    packet_size = ring_buff_length(buff, buff->write_ptr_packet, buff->write_ptr_byte);
    buff->write_ptr_packet = buff->write_ptr_byte;
    return packet_size;
}

void ring_buff_write_clear_packet(ring_buff_t *buff) {
    buff->write_ptr_byte = buff->write_ptr_packet;
}

bool ring_buff_read(ring_buff_t *buff, uint8_t *datum) {
    // check if buffer is empty
    if(buff->read_ptr_byte == buff->write_ptr_packet) {
        return false;
    }
    if(datum != NULL) {
        *datum = *buff->read_ptr_byte;
    }
    ring_buff_increment_ptr(buff, &buff->read_ptr_byte);
    return true;
}

void ring_buff_clear_buff(ring_buff_t *buff) {
    buff->read_ptr_byte = buff->start;
    buff->read_ptr_packet = buff->start;
    buff->write_ptr_byte = buff->start;
    buff->write_ptr_packet = buff->start;
}

uint16_t ring_buff_read_finish_packet(ring_buff_t *buff) {
    uint16_t packet_size;

    packet_size = ring_buff_length(buff, buff->read_ptr_packet, buff->read_ptr_byte);
    buff->read_ptr_packet = buff->read_ptr_byte;
    return packet_size;
}

void ring_buff_read_clear_packet(ring_buff_t *buff) {
    buff->read_ptr_byte = buff->read_ptr_packet;
}





// ----------------------------------------------------- //
// -------------------- private API -------------------- //
// ----------------------------------------------------- //

static inline void ring_buff_increment_ptr(ring_buff_t *buff, uint8_t **ptr) {
    *ptr += 1;
    if(*ptr >= (buff->start + buff->size)) {
        *ptr = buff->start;
    }
}

static uint16_t ring_buff_length(ring_buff_t *buff, uint8_t *start_ptr, uint8_t *end_ptr) {
    int32_t diff;

    diff = end_ptr - start_ptr;
    if(diff > 0) {
        return diff;
    }
    else {
        return buff->size + diff;
    }
}
