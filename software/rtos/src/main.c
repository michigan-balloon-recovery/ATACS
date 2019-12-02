/* Standard includes. */
#include <MSP430-5438STK_HAL/hal_MSP430-5438STK.h>
#include <stdio.h>
/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* Hardware includes. */
#include "driverlib.h"
#include "uart.h"
#include "gnss.h"
#include "aprs.h"
#include "rockblock.h"
#include "sensors.h"
#include "logging.h"

/*-----------------------------------------------------------*/

static void prvSetupHardware(void);

/*-----------------------------------------------------------*/

void main( void ) {
    /* Initialize Hardware */
    prvSetupHardware();

    /* Create Tasks */
    xTaskCreate((TaskFunction_t) task_gnss,           "gnss",             128, NULL, 1, NULL);
//    xTaskCreate((TaskFunction_t) task_aprs,           "aprs",             512, NULL, 1, NULL);
//    xTaskCreate((TaskFunction_t) task_pressure,       "pressure",         128, NULL, 1, NULL);
//    xTaskCreate((TaskFunction_t) task_humidity,       "humidity",         128, NULL, 1, NULL);
//    xTaskCreate((TaskFunction_t) task_rockblock,      "RockBLOCK",        512, NULL, 1, NULL);
    xTaskCreate((TaskFunction_t) task_log,            "Logging",          512, NULL, 1, NULL);


    /* Start the scheduler. */
    __bis_SR_register(GIE); // Global interrupt enable
    vTaskStartScheduler();

    /* If all is well then this line will never be reached.  If it is reached
    then it is likely that there was insufficient (FreeRTOS) heap memory space
    to create the idle task.  This may have been trapped by the malloc() failed
    hook function, if one is configured. */
    for( ;; );
}

/*-----------------------------------------------------------*/

static void prvSetupHardware(void) {
	taskDISABLE_INTERRUPTS();
	
	/* Disable the watchdog. */
	WDTCTL = WDTPW + WDTHOLD;

	/* Set DCO to 16MHz, uses configCPU_CLOCK_HZ, but PMM_CORE_LEVEL_x is hardcoded */
	GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P1, GPIO_PIN6);
	GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN0);

	//Set VCore = 2 for 16MHz clock
    PMM_setVCore(PMM_CORE_LEVEL_2);

    //Set DCO FLL reference = REFO
    UCS_initClockSignal(
        UCS_FLLREF,
        UCS_REFOCLK_SELECT,
        UCS_CLOCK_DIVIDER_1
        );

    UCS_initFLLSettle(
            configCPU_CLOCK_HZ / 1000,
            configCPU_CLOCK_HZ / 32768
    );

    halBoardInit();

    /* debug LEDs */
    GPIO_setAsOutputPin(GPIO_PORT_P8, GPIO_PIN2);
    GPIO_setAsOutputPin(GPIO_PORT_P8, GPIO_PIN3);
    GPIO_setAsOutputPin(GPIO_PORT_P8, GPIO_PIN4);
    GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN2);
    GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN3);
    GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN4);

    /* UART */
    initUartDriver();

    /* I2C and */
    i2c_setup();
}
/*-----------------------------------------------------------*/


void vApplicationTickHook(void) {
	return;
}

/*-----------------------------------------------------------*/

/* The MSP430X port uses this callback function to configure its tick interrupt.
This allows the application to choose the tick interrupt source.
configTICK_VECTOR must also be set in FreeRTOSConfig.h to the correct
interrupt vector for the chosen tick interrupt source.  This implementation of
vApplicationSetupTimerInterrupt() generates the tick from timer A0, so in this
case configTICK_VECTOR is set to TIMER0_A0_VECTOR. */
void vApplicationSetupTimerInterrupt(void) {
    const unsigned short usACLK_Frequency_Hz = 32768;

	/* Ensure the timer is stopped. */
	TA0CTL = 0;

	/* Run the timer from the ACLK. */
	TA0CTL = TASSEL_1;

	/* Clear everything to start with. */
	TA0CTL |= TACLR;

	/* Set the compare match value according to the tick rate we want. */
	TA0CCR0 = usACLK_Frequency_Hz / configTICK_RATE_HZ;

	/* Enable the interrupts. */
	TA0CCTL0 = CCIE;

	/* Start up clean. */
	TA0CTL |= TACLR;

	/* Up mode. */
	TA0CTL |= MC_1;
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook(void) {
	/* Called on each iteration of the idle task.  In this case the idle task
	just enters a low(ish) power mode. */
//	__bis_SR_register( LPM1_bits + GIE );
}
/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook(void) {
	/* Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues or
	semaphores. */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName) {
	( void ) pxTask;
	( void ) pcTaskName;

	/* Run time stack overflow checking is performed if
	configconfigCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}
/*-----------------------------------------------------------*/
