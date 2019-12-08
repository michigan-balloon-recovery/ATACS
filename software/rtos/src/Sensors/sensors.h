#ifndef SENSORS_H
#define SENSORS_H

// -------------------------------------------------------------- //
// -------------------- include dependencies -------------------- //
// -------------------------------------------------------------- //

//MSP430 Hardware
#include <msp430.h>
//FreeRTOS
#include "FreeRTOS.h"
#include "task.h"
//standard libraries
#include <stdbool.h>
//application drivers
#include "i2c_driver.h"


// ---------------------------------------------------------- //
// -------------------- type definitions -------------------- //
// ---------------------------------------------------------- //

typedef struct{
	int32_t pressure;
	int32_t humidity;
	int32_t pTemp;
	int32_t hTemp;
	SemaphoreHandle_t pressureSemaphore;
	SemaphoreHandle_t humiditySemaphore;
	bool humid_init;
	bool pres_init;
	bool pres_valid;
	bool humid_valid;
} sensor_data_t;

uint16_t c[8];

// ----------------------------------------------------------- //
// -------------------- public prototypes -------------------- //
// ----------------------------------------------------------- //

/* \brief FreeRTOS task to get pressure and temperature reading 
 *
 * \return None 
 */
void task_pressure(void);

/* \brief FreeRTOS task to get humidity and temperature reading 
 *
 * \return None 
 */
void task_humidity(void);


void sens_init_pres(void);

bool sens_calc_pres(int32_t* return_data);

bool sens_calc_humid(int32_t* return_data);

bool sens_get_pres(int32_t* pressure);

bool sens_get_ptemp(int32_t* temp);

bool sens_get_humid(int32_t* humidity);

bool sens_get_htemp(int32_t* temp);

// disables interrupts for the sensors (basically just i2c)
// this function returns true if you were able to disable interrupts.
bool sens_disable_interrupts(void);

// enables interrupts associated with the sensors (i2c)
// you can always assume you succeeded enabling.
// do not call unless you successfully disabled interrupts previously.
void sens_enable_interrupts(void);

#endif

