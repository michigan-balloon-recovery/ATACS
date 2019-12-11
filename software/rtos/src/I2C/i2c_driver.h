/*
 * i2c_driver.h
 *
 *  Created on: Nov 10, 2019
 *      Author: jjmau
 */

#ifndef I2C_DRIVER_H_
#define I2C_DRIVER_H_

// -------------------------------------------------------------- //
// -------------------- include dependencies -------------------- //
// -------------------------------------------------------------- //

//standard libraries
#include <stdint.h>
#include <stdbool.h>
//FreeRTOS
#include "FreeRTOS.h"
#include "semphr.h"

SemaphoreHandle_t i2c_rx_semaphore;
SemaphoreHandle_t i2c_busy_semaphore;
SemaphoreHandle_t i2c_tx_semaphore;
static char status;

// ----------------------------------------------------------- //
// -------------------- public prototypes -------------------- //
// ----------------------------------------------------------- //

/*!
 * \sets up the i2c and all of its configurations. Enables all interrupts
 *  must be called before write or read are ever called.
 *
 * \return None 
 */
int i2c_setup(void);

/*!
 * \sends a message over the i2c bus
 *
 * \return true if successfully transmitted the data
 */
bool i2c_write(uint8_t addr, uint8_t * data, uint8_t numBytes);

/*!
 * \reads a message over the i2c bus
 *
 * \return true if successfully received the data
 */
bool i2c_read(uint8_t addr, uint8_t * data, uint8_t numBytes);


/*!
 * \disables all interrupts associated with i2c. Returns true if it successfully disabled interrupts
 * 
 * \return true if successfully disabled interrupts 
 */
bool i2c_disable_interrupts(void);

/*!
 * \enables all interrupts associated with the i2c bus. 
 *  Do not call unless you used i2c_disable_interrupts() successfully prior to calling this.
 *
 * \return None
 */
void i2c_enable_interrupts(void);

__interrupt void USCI_B0_ISR(void);

#endif /* I2C_DRIVER_H_ */
