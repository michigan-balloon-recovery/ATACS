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

/*-----------------------------------------------------------*/

#define RB_TRANSMIT_RATE_MS (uint32_t) 300000      // this is 5 minutes (1000 ms/sec * 60 sec/min * 5min)
#define RB_RETRY_RATE_MS    15000       // this is 15 seconds (1000 ms/sec / * 15 sec)
#define RB_MAX_TX_RETRIES   10          // Retry at most 10 times. This means we try for 10*15=150 seconds.
#define RB_MAX_RX_RETRIES   5           // retry at most 5 times. This means we try for 5*15=75 seconds.

#define APRS_PERIOD_MS      10000

/*-----------------------------------------------------------*/

static void prvSetupHardware( void );
void task_heartbeat();
void task_gnss();
void task_aprs();
void task_getPressure();
void task_getHumidity();
void task_rockblock();

gnss_t GNSS;
ROCKBLOCK_t rb;

/*-----------------------------------------------------------*/

void main( void ) {
    /* Initialize Hardware */
    prvSetupHardware();

    /* Create Tasks */

    xTaskCreate((TaskFunction_t) task_heartbeat,     "LED heartbeat",    128, NULL, 1, NULL);
//    xTaskCreate((TaskFunction_t)task_gnss,         "gnss",                  128, NULL, 1, NULL);
//    xTaskCreate((TaskFunction_t) task_aprs,          "aprs",             128, NULL, 1, NULL);
//    xTaskCreate((TaskFunction_t) task_getPressure,   "getPressure",           128, NULL, 1, NULL);
//    xTaskCreate((TaskFunction_t) task_getHumidity,   "getHumidity",           128, NULL, 1, NULL);
//    xTaskCreate((TaskFunction_t) task_rockblock,     "RockBLOCK",             128, NULL, 1, NULL);

    /* Start the scheduler. */

    __bis_SR_register(GIE);

    vTaskStartScheduler();

    /* If all is well then this line will never be reached.  If it is reached
    then it is likely that there was insufficient (FreeRTOS) heap memory space
    to create the idle task.  This may have been trapped by the malloc() failed
    hook function, if one is configured. */
    for( ;; );
}

/*-----------------------------------------------------------*/

void task_heartbeat() {
    portTickType xLastWakeTime;
    const portTickType xFrequency = 100 / portTICK_RATE_MS;  // 100ms?
    xLastWakeTime = xTaskGetTickCount();

    volatile uint32_t i;

    while (1) {
        P8OUT ^= 0x04;              // toggle P8.2
        for(i=5000; i>0; i--);     // delay
        P8OUT ^= 0x08;              // toggle P8.3
        for(i=5000; i>0; i--);     // delay
        P8OUT ^= 0x10;              // toggle P8.4
        for(i=5000; i>0; i--);     // delay

        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

void task_gnss() {
    gnss_init(&GNSS);

    portTickType xLastWakeTime;
    const portTickType xFrequency = 100 / portTICK_RATE_MS;  // 100ms?
    xLastWakeTime = xTaskGetTickCount();

    while (1) {
        xSemaphoreTake(GNSS.uart_semaphore, portMAX_DELAY);
        gnss_nmea_decode(&GNSS);
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

void task_aprs() {
    portTickType xLastWakeTime;
    const portTickType xFrequency = APRS_PERIOD_MS / portTICK_RATE_MS;
    xLastWakeTime = xTaskGetTickCount();
    while (1){
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
        vTaskSuspendAll();
        aprs_beacon(0);   //altitude 0m
        xTaskResumeAll();
    }
}

void task_getHumidity(){

    sensor_data.humiditySemaphore = xSemaphoreCreateBinary();
    portTickType xLastWakeTime;
    const portTickType xFrequency = 1000 / portTICK_RATE_MS;
    xLastWakeTime = xTaskGetTickCount();

    while(1){
	vTaskDelayUntil(&xLastWakeTime, xFrequency);

    	int32_t data[1];
        calculateHumidity(data);

	xSemaphoreTake(sensor_data.humiditySemaphore, portMAX_DELAY);
	sensor_data.humidity = data[0];
	sensor_data.hTemp = data[1];
	xSemaphoreGive(sensor_data.humiditySemaphore);
    }

}

void task_getPressure(){
	sensor_data.pressureSemaphore = xSemaphoreCreateBinary();
	portTickType xLastWakeTime;
  	const portTickType xFrequency = 1000 / portTICK_RATE_MS;
  	xLastWakeTime = xTaskGetTickCount();

	while(1)
	{
		vTaskDelayUntil(&xLastWakeTime, xFrequency);
    	int32_t data[1];
    	calculatePressure(data);

		xSemaphoreTake(sensor_data.pressureSemaphore,portMAX_DELAY);
		sensor_data.pressure = data[0];
		sensor_data.pTemp = data[1];
		xSemaphoreGive(sensor_data.pressureSemaphore);
	}
}

void task_rockblock(void) {
    portTickType xLastWakeTime;
    const portTickType xTaskFrequency =  (uint16_t) ((uint32_t) RB_TRANSMIT_RATE_MS / (uint32_t) portTICK_RATE_MS);
    const portTickType xRetryFrequency = RB_RETRY_RATE_MS / portTICK_RATE_MS;
    xLastWakeTime = xTaskGetTickCount();

    uint8_t msg[RB_TX_SIZE];
    uint16_t len = 0;
    bool msgSent = false;
    int8_t msgReceived = 0;
    int8_t msgsQueued = 0;
    uint8_t numRetries = 0;
    uint8_t i = 0;

    int32_t pressure, humidity, hTemp, pTemp;
    int32_t altitude;
    gnss_time_t time;
    gnss_coordinate_pair_t location;
    bool success[7];

    for(i = 0; i < 7; i++)
        success[i] = true;

    rb_init(&rb);

    while(1) {
        vTaskDelayUntil(&xLastWakeTime, xTaskFrequency);

        i = 0;
        success[i++] = getPressure(&pressure);
        success[i++] = getHumidity(&humidity);
        success[i++] = getHTemp(&hTemp);
        success[i++] = getPTemp(&pTemp);
        success[i++] = gnss_get_altitude(&GNSS, &altitude);
        success[i++] = gnss_get_time(&GNSS, &time);
        success[i++] = gnss_get_location(&GNSS, &location);

        rb_create_telemetry_packet(msg, &len, pressure, humidity, pTemp, hTemp, altitude, &time, &location, success);

        msgSent = false;
        msgReceived = 0;
        msgsQueued = 0;
        numRetries = 0;

        rb_send_message(&rb, msg, len, &msgSent, &msgReceived, &msgsQueued);

        while(!msgSent && numRetries < RB_MAX_TX_RETRIES) {
            numRetries++;
            vTaskDelayUntil(&xLastWakeTime, xRetryFrequency);
            rb_start_session(&rb, &msgSent, &msgReceived, &msgsQueued);
        }

        numRetries = 0;

        if(msgReceived == 1) { // we received a message
            rb_retrieve_message(&rb);
            rb_process_message(&rb.rx);

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

