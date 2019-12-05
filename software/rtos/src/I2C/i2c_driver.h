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
#include "semphr.h"
#include <stdbool.h>
#include "usci_b_i2c.h"

SemaphoreHandle_t i2c_busy_semaphore;

// sets up the i2c and all of its configurations. Enables all interrupts
// must be called before write or read are ever called.
int i2c_setup(void);

// sends a message over the i2c bus
// returns true if the message was successfully sent
bool i2c_write(uint8_t addr, uint8_t * data, uint8_t numBytes);

// reads a message over the i2c bus
// returns true if the message was successfully sent and a response was received
bool i2c_read(uint8_t addr, uint8_t * data, uint8_t numBytes);

// disables all interrupts associated with i2c. Returns true if it successfully disabled interrupts
bool i2c_disable_interrupts(void);

// enables all interrupts associated with the i2c bus. Do not call unless you used i2c_disable_interrupts() successfully prior to calling this.
void i2c_enable_interrupts(void);

__interrupt void USCI_B0_ISR(void);

#endif /* I2C_DRIVER_H_ */
