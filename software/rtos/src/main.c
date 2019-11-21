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
#include "ff_msp430_sddisk.h"

/*-----------------------------------------------------------*/
#define APRS_PERIOD_MS      60000

/*-----------------------------------------------------------*/

static void prvSetupHardware(void);
void task_led_breathe(void);

/*-----------------------------------------------------------*/

void main( void ) {
    /* Initialize Hardware */
    prvSetupHardware();

    /* Create Tasks */

    xTaskCreate((TaskFunction_t) task_led_breathe,      "LED heartbeat",    128, NULL, 1, NULL);
//    xTaskCreate((TaskFunction_t) task_gnss,           "gnss",             128, NULL, 1, NULL);
//    xTaskCreate((TaskFunction_t) task_aprs,           "aprs",             128, NULL, 1, NULL);
//    xTaskCreate((TaskFunction_t) task_pressure,       "pressure",         128, NULL, 1, NULL);
//    xTaskCreate((TaskFunction_t) task_humidity,       "humidity",         128, NULL, 1, NULL);
//    xTaskCreate((TaskFunction_t) task_rockblock,      "RockBLOCK",        128, NULL, 1, NULL);
//    xTaskCreate((TaskFunction_t) task_logging,        "Logging"           512, NULL, 1, NULL);

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

//uint8_t buff[2048];
//uint8_t wbuf[512];
//uint8_t rbuf[512];
//
//void task_logging() {
//    uint16_t i;
//    uint32_t num;
//    uint32_t sd_size;
//    uint8_t status = 1;
//    uint32_t timeout = 0;
//    FF_Disk_t *disk;
//    FF_FILE *file;
//    FF_Error_t *error;
//    //Initialisation of the MMC/SD-card
//    while (status != 0)                       // if return in not NULL an error did occur and the
//                                              // MMC/SD-card will be initialized again
//    {
//      status = mmcInit();
//      timeout++;
//      if (timeout == 150)                      // Try 50 times till error
//      {
//        //printf ("No MMC/SD-card found!! %x\n", status);
//        break;
//      }
//    }
//    sd_size = mmcReadCardSize();
//
//    disk = FF_SDDiskInit("/", sd_size, 2048);
//
//    status = ff_mkdir("/sd", 0);
//
//    file = ff_fopen("/sd/data", "w");
//
//    memset(wbuf, 't', 512);
//    num = ff_fwrite(wbuf, sizeof(uint8_t), 512, file);
//    ff_rewind(file);
//    num = ff_fread(rbuf, sizeof(uint8_t), 512, file);
//
//    ff_fclose(file);
//}

void task_led_breathe() {
    const portTickType xFrequency = 1000 / portTICK_RATE_MS;
    portTickType xLastWakeTime = xTaskGetTickCount();

    GPIO_setAsOutputPin(GPIO_PORT_P8, GPIO_PIN2);
    GPIO_setAsOutputPin(GPIO_PORT_P8, GPIO_PIN3);
    GPIO_setAsOutputPin(GPIO_PORT_P8, GPIO_PIN4);

    GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN2);
    GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN3);
    GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN4);

    while (1) {
        GPIO_toggleOutputOnPin(GPIO_PORT_P8, GPIO_PIN2);
        vTaskDelay(100 / portTICK_RATE_MS);
        GPIO_toggleOutputOnPin(GPIO_PORT_P8, GPIO_PIN3);
        vTaskDelay(100 / portTICK_RATE_MS);
        GPIO_toggleOutputOnPin(GPIO_PORT_P8, GPIO_PIN4);
        vTaskDelay(100 / portTICK_RATE_MS);

        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

void task_aprs() {
    const portTickType xFrequency = APRS_PERIOD_MS / portTICK_RATE_MS;
    portTickType xLastWakeTime = xTaskGetTickCount();

    // P1.2 is PD (sleep)
    // P1.3 is PTT (push-to-talk)
    // MIC_IN is routed to P2.1 on board rev 1.0, but P2.1 and P2.2 are
    // bridged on the board because PWM from T1.0(P2.1) is inconvenient
    aprs_setup(GPIO_PORT_P1, GPIO_PIN2,
               GPIO_PORT_P1, GPIO_PIN3,
               GPIO_PORT_P2, GPIO_PIN2);

    while (1) {
        vTaskDelayUntil(&xLastWakeTime, xFrequency);

        // Fetch GPS and sensor data
        gnss_time_t time;
        gnss_coordinate_pair_t loc;
        int32_t alt;
        while(!gnss_get_time(&GNSS, &time));
        while(!gnss_get_location(&GNSS, &loc));
        while(!gnss_get_altitude(&GNSS, &alt));

        vTaskSuspendAll();
        aprs_beacon(&time, &loc, &alt);
        xTaskResumeAll();
    }
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
	__bis_SR_register( LPM1_bits + GIE );
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
