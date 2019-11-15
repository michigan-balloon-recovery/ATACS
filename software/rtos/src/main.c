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
#include "rockblock.h"
#include "sensors.h"

#include <stdlib.h>

/*-----------------------------------------------------------*/

gnss_t GNSS;
ROCKBLOCK_t rb;
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/
#define RB_TRANSMIT_RATE_MS 300000      // this is 5 minutes (1000 ms/sec * 60 sec/min * 5min)
#define RB_RETRY_RATE_MS    15000       // this is 15 seconds (1000 ms/sec / * 15 sec)
#define RB_MAX_TX_RETRIES   10          // Retry at most 10 times. This means we try for 10*15=150 seconds.
#define RB_MAX_RX_RETRIES   5           // retry at most 5 times. This means we try for 5*15=75 seconds.
/*-----------------------------------------------------------*/

/*
 * Configures clocks, LCD, port pints, etc. necessary to execute this demo.
 */
static void prvSetupHardware( void );
SemaphoreHandle_t pressureSemaphore;
SemaphoreHandle_t humiditySemaphore;

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

void task_getHumidity(){
	
	humiditySemaphore = xSemaphoreCreateBinary();
    portTickType xLastWakeTime;
    const portTickType xFrequency = 1000 / portTICK_RATE_MS;
    xLastWakeTime = xTaskGetTickCount();
	
	uint32_t hum = 0;
	uint8_t data[4] = {0x0, 0x0, 0x0, 0x0};
	uint32_t temp = 0;
	
	while(1){
		vTaskDelayUntil(&xLastWakeTime, xFrequency);
		
		i2c_write(0x27, 0, 0);
		vTaskDelay(1/portTICK_PERIOD_MS);
		i2c_read(0x27, data, 4);

		hum = data[0] & 0x3F;
		hum = hum << 8;
		hum += data[1];
		hum = 100*hum;
		hum += 8191; // add 1/2 LSB for accurate rounding
		hum = hum/16382;
		temp = data[2];
		temp = temp << 6;
		temp += (data[3] >> 2);
		temp = 165*temp;
		temp += 8191; // add 1/2 LSB for accurate rounding
		temp = temp / 16382;
		temp -= 40;
		temp = 1.8*temp + 32;
		
		xSemaphoreTake(humiditySemaphore, portMAX_DELAY);
		sensor_data.humidity = hum;
		sensor_data.hTemp = temp;
		xSemaphoreGive(humiditySemaphore);
	}

}

void task_getPressure(){
	pressureSemaphore = xSemaphoreCreateBinary();
	portTickType xLastWakeTime;
    const portTickType xFrequency = 1000 / portTICK_RATE_MS;
    xLastWakeTime = xTaskGetTickCount();
	
	
	volatile unsigned int i,j;      // volatile to prevent optimization
    uint8_t data[4] = {0x0, 0x0, 0x0, 0x0};
    uint8_t cmd[1];
    uint8_t get_data_cmd[1] = {0x00};
    
    
    int32_t pressure = 0;

    uint32_t d1, d2;

    int32_t dT, pTemp;
    int64_t offset, sens;

	while(1)
	{
		vTaskDelayUntil(&xLastWakeTime, xFrequency);
		//retrieves digital pressure value
		vTaskDelay(1/portTICK_PERIOD_MS);
		cmd[0] = 0x48;
		i2c_write(0x77, cmd, 1);
		vTaskDelay(1/portTICK_PERIOD_MS);
		cmd[0] = 0x00;
		i2c_write(0x77, cmd, 1);
		vTaskDelay(1/portTICK_PERIOD_MS);
		i2c_read(0x77, data, 3);
		d1 = data[0];
		d1 = d1 << 8;
		d1 += data[1];
		d1 = d1 << 8;
		d1 += data[2];

		//retrieves digital temperature value
		cmd[0] = 0x58;
		i2c_write(0x77, cmd, 1);
		vTaskDelay(1/portTICK_PERIOD_MS);
		cmd[0] = 0x00;
		i2c_write(0x77, cmd, 1);
		vTaskDelay(1/portTICK_PERIOD_MS);
		i2c_read(0x77, data, 3);
		d2 = data[0];
		d2 = d2 << 8;
		d2 += data[1];
		d2 = d2 << 8;
		d2 += data[2];

		//calculate temperature
		dT = c[5];
		dT *= 256;
		dT = d2 - dT;

		pTemp = c[6];
		pTemp *= dT;
		pTemp /= 8388608;
		pTemp += 2000;

		//calculate temperature compensated pressure
		int64_t temporary;
		offset = c[2];
		offset *= 65536;
		temporary = c[4];
		temporary += dT;
		temporary /= 128;
		offset = offset + temporary;

		sens = c[1];
		sens *= 32768;
		temporary = c[3];
		temporary *= dT;
		temporary /= 256;


		pressure = d1;
		pressure *= sens;
		pressure /= 2097152;
		pressure -= offset;
		pressure /= 16384; // 32768
		pressure /= 100;

		pTemp += 50;
		pTemp /= 100;
		pTemp *= 1.8;
		pTemp += 32;
		
		xSemaphoreTake(pressureSemaphore,portMAX_DELAY);
		sensor_data.pressure = pressure;
		sensor_data.pTemp = pTemp;
		xSemaphoreGive(pressureSemaphore);
	}
}

void task_rockblock(void) {
    portTickType xLastWakeTime;
    const portTickType xTaskFrequency = RB_TRANSMIT_RATE_MS / portTICK_RATE_MS;
    const portTickType xRetryFrequency = RB_RETRY_RATE_MS / portTICK_RATE_MS;
    xLastWakeTime = xTaskGetTickCount();

    uint8_t msg[RB_TX_SIZE];
    uint16_t len = 0;
    bool msgSent = false;
    int8_t msgReceived = 0;
    int8_t msgsQueued = 0;
    uint8_t numRetries = 0;

    rb_init(&rb);

    while(1) {
        vTaskDelayUntil(&xLastWakeTime, xTaskFrequency);
        //TODO: grab data and populate msg, len fields
        rb_send_message(&rb, &msg, len, &msgSent, &msgReceived, &msgsQueued);

        while(!msgSent && numRetries < RB_MAX_TX_RETRIES) {
            numRetries++;
            vTaskDelayUntil(&xLastWakeTime, xRetryFrequency);
            rb_start_session(&rb, &msgSent, &msgReceived, &msgsQueued);
        }

        numRetries = 0;

        if(msgReceived == 1) { // we received a message
            rb_retrieve_message(&rb);
            // TODO: process message

            while(msgsQueued > 0 && numRetries < RB_MAX_RX_RETRIES ) { // other messages to download
                rb_start_session(&rb, &msgSent, &msgReceived, &msgsQueued);
                if(msgReceived == 1) {
                    numRetries = 0;
                    rb_retrieve_message(&rb);
                    // TODO: process message
                } else {
                    numRetries++;
                    vTaskDelayUntil(&xLastWakeTime, xRetryFrequency);
                }
            }
        }
    }
}

void main( void ) {
    /* Initialize Hardware */
    prvSetupHardware();

//    gnss_init(&GNSS);

    /* Create Tasks */
//    xTaskCreate((TaskFunction_t)example_task,      "LED_1 Toggle",          128, NULL, 1, NULL);
//    xTaskCreate((TaskFunction_t)task_gnss,         "gnss",                  128, NULL, 1, NULL);
    xTaskCreate((TaskFunction_t) task_ax25,          "afsk_ax25",             128, NULL, 1, NULL);
	xTaskCreate((TaskFunction_t) task_getPressure,	 "getPressure",			  128, NULL, 1, NULL);
	xTaskCreate((TaskFunction_t) task_getHumidity,	 "getHumidity",			  128, NULL, 1, NULL);
	xTaskCreate((TaskFunction_t) task_rockblock,     "RockBLOCK",             128, NULL, 1, NULL);

    /* Start the scheduler. */

    __bis_SR_register(GIE);

    while(1);
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

	/* I2C and Pressure Sensor */
	initPressure();
	
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

