/* Standard includes. */
#include <MSP430-5438STK_HAL/hal_MSP430-5438STK.h>
#include <stdio.h>
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* Hardware includes. */
#include "driverlib.h"
#include "uart.h"
#include "gnss.h"
#include "ax25.h"

/*-----------------------------------------------------------*/

gnss_t GNSS;

/*-----------------------------------------------------------*/

/*
 * Configures clocks, LCD, port pints, etc. necessary to execute this demo.
 */
static void prvSetupHardware( void );

/*-----------------------------------------------------------*/

//// toggle LED_1 every 500ms
void example_task( void ) {
    portTickType xLastWakeTime;
    const portTickType xFrequency = 500 / portTICK_RATE_MS;

    xLastWakeTime = xTaskGetTickCount();

    while(1) {
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
        LED_PORT_OUT ^= LED_1;
    }
}

void task_gnss() {

    while (1) {
        xSemaphoreTake(GNSS.uart_semaphore, portMAX_DELAY);
        gnss_nmea_decode(&GNSS);
    }
}

void task_ax25() {
    portTickType xLastWakeTime;
    const portTickType xFrequency = 15000 / portTICK_RATE_MS;
    xLastWakeTime = xTaskGetTickCount();
    address_t addresses[2] = {
      {"APRS", 0},
      {"KD2OHS", 11}
    };

    // TODO, add digipeater path to addresses[]

    while (1){
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
        vTaskSuspendAll();
            LED_PORT_OUT |= LED_1;

            // Header
            ax25_send_header(addresses, 2);
            ax25_send_byte('/');
            ax25_send_string("000000", 6);
            ax25_send_byte('h');
            ax25_send_string("0000.00N", strlen("0000.00N"));
            ax25_send_byte('/');
            ax25_send_string("00000.00E", strlen("00000.00E"));
            ax25_send_byte('O');
            ax25_send_string("000", 3);

//            ax25_send_string("/A=000000", strlen("/A=000000"));
//            ax25_send_string("/Ti=27", strlen("/Ti=27"));
//            ax25_send_string("/Te=93", strlen("/Te=93"));
//            ax25_send_string("/V=7435", strlen("/V=7435"));

            // Comment
            ax25_send_byte(' ');
            ax25_send_string("HELLO 473", strlen("473 RULES"));

            // Footer
            ax25_send_footer();

            // Send!
            ax25_flush_frame();

            LED_PORT_OUT &= ~LED_1;
        xTaskResumeAll();
    }
}

void main( void ) {
    /* Initialize Hardware */
    prvSetupHardware();

//    gnss_init(&GNSS);

    /* Create Tasks */
//    xTaskCreate((TaskFunction_t)example_task,      "LED_1 Toggle",          128, NULL, 1, NULL);
//    xTaskCreate((TaskFunction_t)task_gnss,         "gnss",                  128, NULL, 1, NULL);
    xTaskCreate((TaskFunction_t)task_ax25,           "afsk_ax25",             128, NULL, 1, NULL);

    /* Start the scheduler. */
    vTaskStartScheduler();

	/* If all is well then this line will never be reached.  If it is reached
	then it is likely that there was insufficient (FreeRTOS) heap memory space
	to create the idle task.  This may have been trapped by the malloc() failed
	hook function, if one is configured. */	
	for( ;; );
}
/*-----------------------------------------------------------*/

static void prvSetupHardware( void ) {
	taskDISABLE_INTERRUPTS();
	
	/* Disable the watchdog. */
	WDTCTL = WDTPW + WDTHOLD;
  
	halBoardInit();

	// LFXT_Start( XT1DRIVE_0 );
	hal430SetSystemClock( configCPU_CLOCK_HZ, configLFXT_CLOCK_HZ );

    /* UART */
    initUartDriver();

    // UARTConfig a0_cnf;
    // a0_cnf.moduleName = USCI_A0;

    // // Use UART Pins P3.5 and P3.4
    // a0_cnf.portNum = PORT_3;
    // a0_cnf.RxPinNum = PIN5;
    // a0_cnf.TxPinNum = PIN4;

    // // 38400 Baud from 16MHz SMCLK
    // a0_cnf.clkRate = configCPU_CLOCK_HZ;
    // a0_cnf.baudRate = 38400L;
    // a0_cnf.clkSrc = UART_CLK_SRC_SMCLK;

    // // 8N1
    // a0_cnf.databits = 8;
    // a0_cnf.parity = UART_PARITY_NONE;
    // a0_cnf.stopbits = 1;

    // initUSCIUart(&a0_cnf, A0_TX, A0_RX);
}
/*-----------------------------------------------------------*/


void vApplicationTickHook( void ) {
	return;
}
/*-----------------------------------------------------------*/

/* The MSP430X port uses this callback function to configure its tick interrupt.
This allows the application to choose the tick interrupt source.
configTICK_VECTOR must also be set in FreeRTOSConfig.h to the correct
interrupt vector for the chosen tick interrupt source.  This implementation of
vApplicationSetupTimerInterrupt() generates the tick from timer A0, so in this
case configTICK_VECTOR is set to TIMER0_A0_VECTOR. */
void vApplicationSetupTimerInterrupt( void )
{
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

void vApplicationIdleHook( void )
{
	/* Called on each iteration of the idle task.  In this case the idle task
	just enters a low(ish) power mode. */
	__bis_SR_register( LPM1_bits + GIE );
}
/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	/* Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues or
	semaphores. */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	( void ) pxTask;
	( void ) pcTaskName;

	/* Run time stack overflow checking is performed if
	configconfigCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}
/*-----------------------------------------------------------*/

