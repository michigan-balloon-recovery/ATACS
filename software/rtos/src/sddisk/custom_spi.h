#ifndef CUSTOM_SPI_H
#define CUSTOM_SPI_H

#include <stdint.h>
#include <msp430.h>

#define CS_LOW()    P3OUT &= ~(0x01)
#define CS_HIGH()   P3OUT |= 0x01

void spi_init();
uint8_t spi_byte(uint8_t datum);

#endif /* CUSTOM_SPI_H */
