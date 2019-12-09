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

/** @struct sensor_data_t
 *  @brief sensor data and configuration objects
 *
 */

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

//pressure sensor initialization values 
uint16_t c[8];

// ----------------------------------------------------------- //
// -------------------- public prototypes -------------------- //
// ----------------------------------------------------------- //

/*!
 * \brief FreeRTOS task to get pressure and temperature reading 
 *
 * \return None 
 */
void task_pressure(void);

/*!
 * \brief FreeRTOS task to get humidity and temperature reading 
 *
 * \return None 
 */
void task_humidity(void);

/*!
 *\brief initialize pressure sensor 
 *
 *\return none
 */
void sens_init_pres(void);

/*!
 *\brief calculate pressure and temperature from pressure sensor
 *
 * @param retrun_data is a pointer to the memory address to store data in
 *\return true if calulation was succesful
 */
bool sens_calc_pres(int32_t* return_data);

/*!
 *\brief calculate humidity and temperature from humidity sensor
 *
 * @param retrun_data is a pointer to the memory address to store data in
 *\return true if calulation was succesful
 */
bool sens_calc_humid(int32_t* return_data);

/*!
 *\brief retrieve current pressure calculattion
 *
 * @param pressure is a pointer to the memory address to store data in
 *\return true if retrieval was succesful
 */
bool sens_get_pres(int32_t* pressure);

/*!
 *\brief retrieve current temperature from pressure sensor
 *
 * @param temp is a pointer to the memory address to store data in
 *\return true if retrieval was succesful
 */
bool sens_get_ptemp(int32_t* temp);

/*!
 *\brief retrieve current humidity calculattion
 *
 * @param humidity is a pointer to the memory address to store data in
 *\return true if retrieval was succesful
 */
bool sens_get_humid(int32_t* humidity);

/*!
 *\brief retrieve current temperature from humidity sensor
 *
 * @param temp is a pointer to the memory address to store data in
 *\return true if retrieval was succesful
 */
bool sens_get_htemp(int32_t* temp);

// disables interrupts for the sensors (basically just i2c)
// this function returns true if you were able to disable interrupts.
bool sens_disable_interrupts(void);

// enables interrupts associated with the sensors (i2c)
// you can always assume you succeeded enabling.
// do not call unless you successfully disabled interrupts previously.
void sens_enable_interrupts(void);

#endif

