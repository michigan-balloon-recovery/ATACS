#ifndef SENSORS_H
#define SENSORS_H

#include <msp430.h>
#include "i2c_driver.h"
#include "task.h"
#include <stdbool.h>

typedef struct{
	int32_t pressure;
	int32_t humidity;
	int32_t pTemp;
	int32_t hTemp;
	SemaphoreHandle_t pressureSemaphore;
	SemaphoreHandle_t humiditySemaphore;
	bool humid_init;
	bool pres_init;
} sensor_data_t;

uint16_t c[8];

void task_pressure(void);

void task_humidity(void);

void sens_init_pres(void);

void sens_calc_pres(int32_t* return_data);

void sens_calc_humid(int32_t* return_data);

bool sens_get_pres(int32_t* pressure);

bool sens_get_ptemp(int32_t* temp);

bool sens_get_humid(int32_t* humidity);

bool sens_get_htemp(int32_t* temp);



#endif

