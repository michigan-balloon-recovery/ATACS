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

#define BUZZ_OFF_MS 2000 // time buzzer spends off
#define BUZZ_ON_MS 1000 // time buzzer spends on


void buzzer_init(void); // call before trying to turn on the buzzer
void buzzer_control(bool on); // turns the buzzer on/off depending on the bool
void task_buzzer(void); // task that controls whether the buzzer is on/off

#endif /* SRC_BUZZER_BUZZER_H_ */
