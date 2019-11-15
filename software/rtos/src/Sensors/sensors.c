#include "sensors.h"

/**
 * sensors.c
 */


 
void initPressure()
{
	setup();
	unsigned int j;
	volatile unsigned int i;
	uint8_t cmd[1];
	
	cmd[0] = 0x1E;
    i2c_write(0x77, cmd, 1);
    for(j = 0; j<8; j++) {
        uint8_t data[2];
        for(i=1000; i>0; i--);
        cmd[0] = 0xA0 + j;
        i2c_write(0x77, cmd, 1);
        for(i=1000; i>0; i--);
        i2c_read(0x77, data, 2);
        c[j] = data[0];
        c[j] = c[j] << 8;
        c[j] += data[1];
    }
}
 

 

