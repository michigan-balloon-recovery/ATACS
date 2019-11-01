/* Standard includes. */
#include <MSP430-5438STK_HAL/hal_MSP430-5438STK.h>
#include <stdio.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
// #include "timers.h"
// #include "queue.h"

/* Hardware includes. */
#include "msp430.h"
#include "uart.h"

/*-----------------------------------------------------------*/

/*
 * UART
 */
unsigned char A0_TX[200];
unsigned char A0_RX[200];

/*-----------------------------------------------------------*/

/*
 * Configures clocks, LCD, port pints, etc. necessary to execute this demo.
 */
static void prvSetupHardware( void );

/*-----------------------------------------------------------*/

// toggle LED_1 every 50ms
void task1( void ) {
    portTickType xLastWakeTime;
    const portTickType xFrequency = 50 / portTICK_RATE_MS;

    xLastWakeTime = xTaskGetTickCount();

    while(1) {
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
        LED_PORT_OUT ^= LED_1;
    }
}

// toggle LED_2 every 300ms
void task2( void ) {
    portTickType xLastWakeTime;
    const portTickType xFrequency = 300 / portTICK_RATE_MS;

    xLastWakeTime = xTaskGetTickCount();

    while(1) {
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
        LED_PORT_OUT ^= LED_2;
    }
}

// Print "DEADBEEF\r\n" every 100ms
void task3( void ) {
    portTickType xLastWakeTime;
    const portTickType xFrequency = 100 / portTICK_RATE_MS;

    xLastWakeTime = xTaskGetTickCount();

    while(1) {
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
        vTaskSuspendAll(); // Suspend scheduler while we transmit
        uartSendDataBlocking(&USCI_A0_cnf, (unsigned char*)"DEADBEEF\r\n", 10);
        xTaskResumeAll();
        LED_PORT_OUT ^= LED_1;
    }
}

// Read from UART 0 every 10ms;
void task4( void ) {
    portTickType xLastWakeTime;
    const portTickType xFrequency = 100 / portTICK_RATE_MS;

    xLastWakeTime = xTaskGetTickCount();

    while(1) {
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
        int bytesAvailable = numUartBytesReceived(&USCI_A0_cnf);
        if(bytesAvailable == 10){
            unsigned char tempBuf[10];
            volatile int bytesRead = readRxBytes(&USCI_A0_cnf, tempBuf, bytesAvailable, 0);
            if(bytesRead == bytesAvailable){
                // If we receive "DEADBEEF\r\n", we toggle LED_2
                if(!memcmp(tempBuf, "DEADBEEF\r\n", 10)){
                    LED_PORT_OUT ^= LED_2;
                }
            }
        }
    }
}

void main( void ) {

    /* Initialize Hardware */
    prvSetupHardware();

    /* Create Tasks */
//	xTaskCreate((TaskFunction_t)task1, "t1", 128, NULL, 1, NULL);
//	xTaskCreate((TaskFunction_t)task2, "t2", 128, NULL, 1, NULL);
	xTaskCreate((TaskFunction_t)task3, "t3", 128, NULL, 1, NULL);
	xTaskCreate((TaskFunction_t)task4, "t4", 128, NULL, 1, NULL);

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

    UARTConfig a0_cnf;
    a0_cnf.moduleName = USCI_A0;

    // Use UART Pins P3.5 and P3.4
    a0_cnf.portNum = PORT_3;
    a0_cnf.RxPinNum = PIN5;
    a0_cnf.TxPinNum = PIN4;

    // 38400 Baud from 18MHz SMCLK
    a0_cnf.clkRate = 18000000L;
    a0_cnf.baudRate = 38400L;
    a0_cnf.clkSrc = UART_CLK_SRC_SMCLK;

    // 8N1
    a0_cnf.databits = 8;
    a0_cnf.parity = UART_PARITY_NONE;
    a0_cnf.stopbits = 1;

    initUSCIUart(&a0_cnf, A0_TX, A0_RX);
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

