#ifndef RING_BUFF_H
#define RING_BUFF_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct {
    uint16_t size;
    uint8_t *start;
    uint8_t *read_ptr_byte;
    uint8_t *read_ptr_packet;
    uint8_t *write_ptr_byte;
    uint8_t *write_ptr_packet;
} ring_buff_t;

void ring_buff_init(ring_buff_t *buff, uint8_t *memory, uint16_t size);

bool ring_buff_write(ring_buff_t *buff, uint8_t datum);

uint16_t ring_buff_write_finish_packet(ring_buff_t *buff);

void ring_buff_write_clear_packet(ring_buff_t *buff);

bool ring_buff_read(ring_buff_t *buff, uint8_t *datum);

uint16_t ring_buff_read_finish_packet(ring_buff_t *buff);

void ring_buff_read_clear_packet(ring_buff_t *buff);

#ifdef __cplusplus
}
#endif

#endif /* RING_BUFF_H */
