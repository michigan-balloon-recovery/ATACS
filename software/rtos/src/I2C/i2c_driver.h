/*
 * i2c_driver.h
 *
 *  Created on: Nov 10, 2019
 *      Author: jjmau
 */

#ifndef I2C_DRIVER_H_
#define I2C_DRIVER_H_
#include <stdint.h>
#include "FreeRTOS.h"

SemaphoreHandle_t i2cSemaphore;

int i2c_setup(void);

void i2c_write(uint8_t addr, uint8_t * data, uint8_t numBytes);

void i2c_read(uint8_t addr, uint8_t * data, uint8_t numBytes);

__interrupt void USCI_B0_ISR(void);

#endif /* I2C_DRIVER_H_ */
