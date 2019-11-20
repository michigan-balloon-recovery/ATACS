#include <sddisk.h>

uint8_t sd_command(uint8_t cmd, uint32_t argument, uint8_t crc) {
    unsigned int i;
    uint8_t response = 0xFF;
    CS_HIGH();
    for(i=0; i<20000;i++);
    CS_LOW();
    spi_byte(0xFF);
    spi_byte(0x40 | cmd);
    spi_byte(argument >> 24);
    spi_byte(argument >> 16);
    spi_byte(argument >> 8);
    spi_byte(argument);
    spi_byte(crc);
    while(response == 0xFF) {
        response = spi_byte(0xFF);
    }
    spi_byte(0xFF);
    spi_byte(0xFF);
    spi_byte(0xFF);
    CS_HIGH();
    return response;
}

void sd_boot() {
    unsigned int i;
    uint8_t response;
    spi_init();
    for(i=0; i<10;i++) {
        spi_byte(0xFF);
    }

    for(i=0; i<20000;i++);
    while(response != 0x01) {
        response = sd_command(0, 0, 0x95);
    }
    while(response != 0x03) {
        response = sd_command(8, 0x1AA, 0x87);
    }
}
