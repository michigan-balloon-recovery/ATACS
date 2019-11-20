#ifndef SENSORS_H
#define SENSORS_H

#include <msp430.h>
#include "i2c_driver.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include <stdbool.h>

uint16_t c[8];

typedef struct{
	int32_t pressure;
	int32_t humidity;
	int32_t pTemp;
	int32_t hTemp;
	SemaphoreHandle_t pressureSemaphore;
	SemaphoreHandle_t humiditySemaphore;
} sensor_data_t;


sensor_data_t sensor_data;

void initPressure();

void calculatePressure(int32_t* return_data);

void calculateHumidity(int32_t* return_data);

bool getPressure(int32_t* pressure);

bool getPTemp(int32_t* temp);

bool getHumidity(int32_t* humidity);

bool getHTemp(int32_t* temp);


#endif

