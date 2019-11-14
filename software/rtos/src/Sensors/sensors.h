#include <msp430.h>
#include "i2c_driver.h"

uint16_t c[8] = {0,0,0,0,0,0,0,0};

typedef struct{
	int32_t pressure;
	int32_t humidity;
	int32_t pTemp;
	int32_t hTemp;
} sensor_data_t

sensor_data_t sensor_data; 

void initPressure();
