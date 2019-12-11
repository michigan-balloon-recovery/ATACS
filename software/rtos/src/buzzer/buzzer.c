/*
 * buzzer.c
 *
 *  Created on: Dec 3, 2019
 *      Author: SSS
 */


#include "buzzer.h"


void task_buzzer(void) {

    buzzer_init();
    while(1) {
        buzzer_control(true);
        vTaskDelay(BUZZ_ON_MS / portTICK_RATE_MS);
        buzzer_control(false);
        vTaskDelay(BUZZ_OFF_MS / portTICK_RATE_MS);
    }

}

void buzzer_init(void) {

    P2DIR |= BIT3;

}

void buzzer_control(bool on) {

    if(on)
        P2OUT |= BIT3;
    else
        P2OUT &= ~BIT3;
}

