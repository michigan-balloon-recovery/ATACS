#ifndef SDDISK_H
#define SDDISK_H

#include <custom_spi.h>

uint8_t sd_command(uint8_t cmd, uint32_t argument, uint8_t crc);

void sd_boot();

#endif /* SDDISK_H */
