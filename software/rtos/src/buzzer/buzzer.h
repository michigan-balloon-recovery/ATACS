/*
 * buzzer.h
 *
 *  Created on: Dec 3, 2019
 *      Author: SSS
 */

#ifndef SRC_BUZZER_BUZZER_H_
#define SRC_BUZZER_BUZZER_H_

#include "stdbool.h"
#include <msp430.h>
#include <stdint.h>
#include "FreeRTOS.h"
#include <driverlib.h>
#include "semphr.h"
#include "task.h"

#define BUZZ_OFF_MS 30000 // time buzzer spends off
#define BUZZ_ON_MS 1000 // time buzzer spends on

/*!
 * \brief Initializes the GPIO pin associated with the buzzer.
 *
 * \return None
 */
void buzzer_init(void);


/*!
 * \brief Turns the buzzer on or off.
 *
 * @param on: If true, turns the buzzer on. If false, turns the buzzer off.
 * \return None
 */
void buzzer_control(bool on);


/*!
 * \brief Task which decides whether or not the buzzer should be on.
 *
 * \return None
 */
void task_buzzer(void);

#endif /* SRC_BUZZER_BUZZER_H_ */
