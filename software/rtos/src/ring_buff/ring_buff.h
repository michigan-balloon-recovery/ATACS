#ifndef RING_BUFF_H
#define RING_BUFF_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/** @struct ring_buff_t
 *  @brief object storing ring buffer configuration and status data
 *
 */
typedef struct {
    uint16_t size;
    uint8_t *start;
    uint8_t *read_ptr_byte;
    uint8_t *read_ptr_packet;
    uint8_t *write_ptr_byte;
    uint8_t *write_ptr_packet;
} ring_buff_t;





// ----------------------------------------------------------- //
// -------------------- public prototypes -------------------- //
// ----------------------------------------------------------- //

/*!
 * \brief Initializes the ring buffer
 *
 * Assigns memory to the ring buffer and initializes the pointers to indicate an empty buffer.
 * The number of bytes that can be stored in the buffer is size-1.
 * This function initializes all members of the ring_buff_t struct.
 * 
 * @param buff is the ring_buff_t instance to be initialized
 * @param memory is the memory array to store the buffer's data. This should be global scope
 * @param size is the length of the memory array in bytes
 * \return None
 *
 */
void ring_buff_init(ring_buff_t *buff, uint8_t *memory, uint16_t size);

/*!
 * \brief Write a byte to the ring buffer
 * 
 * Writes a byte to the current packet being writen to the ring buffer.
 * This byte cannot be read out of the buffer until the packet is finished.
 * 
 * @param buff is the ring_buff_t instance
 * @param datum is the byte to add to the buffer
 * \return true if the byte was sucessfully writen to the buffer
 * 
 */
bool ring_buff_write(ring_buff_t *buff, uint8_t datum);

/*!
 * \brief Mark the current packet being written as finished
 * 
 * Marks the current packet being written to the ring buffer as finished.
 * This finalizes the previous bytes written and allows them to be read in.
 * 
 * @param buff is the ring_buff_t instance
 * \return size of the finished packet
 * 
 */
uint16_t ring_buff_write_finish_packet(ring_buff_t *buff);

/*!
 * \brief Clears the packet currently being written
 * 
 * Removes all bytes written to the buffer since the last ring_buff_write_finish_packet() call.
 * Starts a new packet by discarding the old one.
 * 
 * @param buff is the ring_buff_t instance
 * \return None
 * 
 */
void ring_buff_write_clear_packet(ring_buff_t *buff);

/*!
 * \brief Reads a byte from the ring buffer
 * 
 * Reads in the next byte of the current packet.
 * The byte must be part of a complete packet to be read in.
 * 
 * @param buff is the ring_buff_t instance
 * @param datum is updated to the value read from the buffer
 * \return true if a value was available to read from the buffer
 * 
 */
bool ring_buff_read(ring_buff_t *buff, uint8_t *datum);

/*!
 * \brief Marks a packet as done being read
 * 
 * Indicates to the ring buffer that the application has sucessfully read in the packet.
 * This will allow the packet's memory to be overwritten by new data.
 * 
 * @param buff is the ring_buff_t instance
 * \return size of the finished packet
 * 
 */
uint16_t ring_buff_read_finish_packet(ring_buff_t *buff);

/*!
 * \brief Restarts reading a packet
 * 
 * @param buff is the ring_buff_t instance
 * \return None
 * 
 */
void ring_buff_read_clear_packet(ring_buff_t *buff);

/*!
 * \brief Clears the entire ring buffer
 * 
 * @param buff is the ring_buff_t instance
 * \return None
 * 
 */
void ring_buff_clear_buff(ring_buff_t *buff);

#ifdef __cplusplus
}
#endif

#endif /* RING_BUFF_H */
