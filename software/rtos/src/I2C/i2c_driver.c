#include <msp430.h>
#include "i2c_driver.h"
#include <driverlib.h>

unsigned char receiveBuffer[10] = { 0x01, 0x01, 0x01, 0x01, 0x01,
                                    0x01, 0x01, 0x01, 0x01, 0x01};
unsigned char *receiveBufferPointer;
unsigned char receiveCount = 0;

uint8_t transmitData[10];
uint8_t txlength = 0;
uint8_t transmitCounter = 0;

int i2c_setup(void) {
    // Configure GPIO
    GPIO_setAsPeripheralModuleFunctionInputPin(
        GPIO_PORT_P3,
        GPIO_PIN1 + GPIO_PIN2
    );

    P3SEL = BIT1 | BIT2;
    P3OUT = BIT1 | BIT2;
    P3REN = BIT1 | BIT2;
    __delay_cycles(500000);
    USCI_B_I2C_initMasterParam param = {0};
    param.selectClockSource = USCI_B_I2C_CLOCKSOURCE_SMCLK;
    param.i2cClk = UCS_getSMCLK();
    param.dataRate = USCI_B_I2C_SET_DATA_RATE_400KBPS;
    USCI_B_I2C_initMaster(USCI_B0_BASE, &param);
    i2c_busy_semaphore = xSemaphoreCreateMutex();

    return 0;
}

bool i2c_write(uint8_t addr, uint8_t * data, uint8_t numBytes) {

    if(xSemaphoreTake(i2c_busy_semaphore, 20000 / portTICK_RATE_MS) == pdFALSE) {
        return false;
    }
	
    USCI_B_I2C_setSlaveAddress(USCI_B0_BASE,addr);

    USCI_B_I2C_setMode(USCI_B0_BASE,USCI_B_I2C_TRANSMIT_MODE);

	int i;
	for(i=0; i < numBytes; i++) {
		transmitData[i] = *(data + (i)); 			// Max data length is 8 bytes
	}
	
	transmitCounter = 1;
	txlength = numBytes;

    USCI_B_I2C_enable(USCI_B0_BASE);

    USCI_B_I2C_clearInterrupt(USCI_B0_BASE,
        USCI_B_I2C_TRANSMIT_INTERRUPT
    );
    USCI_B_I2C_enableInterrupt(USCI_B0_BASE,
        USCI_B_I2C_TRANSMIT_INTERRUPT
    );

    __delay_cycles(50);

    if(numBytes < 2)
        USCI_B_I2C_masterSendSingleByte(USCI_B0_BASE, transmitData[0]);
    else
        USCI_B_I2C_masterSendMultiByteStart(USCI_B0_BASE, transmitData[0]);
	
	while (USCI_B_I2C_isBusBusy(USCI_B0_BASE));

//    USCI_B_I2C_disable(USCI_B0_BASE);
//    USCI_B_I2C_disableInterrupt(USCI_B0_BASE,USCI_B_I2C_TRANSMIT_INTERRUPT);

	xSemaphoreGive(i2c_busy_semaphore);
	return true;
}

bool i2c_read(uint8_t addr, uint8_t * data, uint8_t numBytes) {
	
    if(xSemaphoreTake(i2c_busy_semaphore, 20000 / portTICK_RATE_MS) == pdFALSE) {
        return false;
    }

    receiveBufferPointer = (unsigned char *) receiveBuffer;
    receiveCount = numBytes;

    //Specify slave address
    USCI_B_I2C_setSlaveAddress(USCI_B0_BASE, addr);
    //Set receive mode
    USCI_B_I2C_setMode(USCI_B0_BASE, USCI_B_I2C_RECEIVE_MODE);

    //Enable I2C Module to start operations
    USCI_B_I2C_enable(USCI_B0_BASE);
    USCI_B_I2C_clearInterrupt(USCI_B0_BASE, USCI_B_I2C_RECEIVE_INTERRUPT);
    USCI_B_I2C_enableInterrupt(USCI_B0_BASE, USCI_B_I2C_RECEIVE_INTERRUPT);

    while (USCI_B_I2C_isBusBusy(USCI_B0_BASE));

//    USCI_B_I2C_disable(USCI_B0_BASE);
//    USCI_B_I2C_disableInterrupt(USCI_B0_BASE, USCI_B_I2C_RECEIVE_INTERRUPT);

    if(numBytes > 1)
        USCI_B_I2C_masterReceiveMultiByteStart(USCI_B0_BASE);
    else
        USCI_B_I2C_masterReceiveSingleStart(USCI_B0_BASE);
    int i = 0;
    for(i = 0; i < numBytes; i++)
        data[i] = receiveBuffer[i];

    xSemaphoreGive(i2c_busy_semaphore);
    return true;
}

void i2c_enable_interrupts(void) {
    xSemaphoreGive(i2c_busy_semaphore);
}

bool i2c_disable_interrupts(void) {

    if(xSemaphoreTake(i2c_busy_semaphore, 20000 / portTICK_RATE_MS) == pdFALSE) {
        return false;
    }

    USCI_B_I2C_disable(USCI_B0_BASE);

    USCI_B_I2C_clearInterrupt(USCI_B0_BASE, USCI_B_I2C_TRANSMIT_INTERRUPT);

    USCI_B_I2C_clearInterrupt(USCI_B0_BASE,USCI_B_I2C_RECEIVE_INTERRUPT);
    USCI_B_I2C_disableInterrupt(USCI_B0_BASE,USCI_B_I2C_TRANSMIT_INTERRUPT);

    USCI_B_I2C_disableInterrupt(USCI_B0_BASE,USCI_B_I2C_RECEIVE_INTERRUPT);

    return true;
    //UCB0CTL1 &= ~UCSWRST;
}

#pragma vector = USCI_B0_VECTOR
__interrupt void USCI_B0_ISR(void) {
  switch(__even_in_range(UCB0IV, USCI_I2C_UCTXIFG)) {
    case USCI_NONE: break;                  // Vector 0: No interrupts
    case USCI_I2C_UCALIFG: break;           // Vector 2: ALIFG
    case USCI_I2C_UCNACKIFG:                // Vector 4: NACKIFG
    break;
    case USCI_I2C_UCSTTIFG: break;          // Vector 6: STTIFG
    case USCI_I2C_UCSTPIFG: break;          // Vector 8: STPIFG
    case USCI_I2C_UCRXIFG:
    {
        //Decrement RX byte counter
        receiveCount--;

        if (receiveCount){
            if (receiveCount == 1) {
                //Initiate end of reception -> Receive byte with NAK
                *receiveBufferPointer++ =
                    USCI_B_I2C_masterReceiveMultiByteFinish(USCI_B0_BASE);
            }
            else {
                //Keep receiving one byte at a time
                *receiveBufferPointer++ = USCI_B_I2C_masterReceiveMultiByteNext(USCI_B0_BASE);
            }
        }
        else {
            //Receive last byte
            *receiveBufferPointer = USCI_B_I2C_masterReceiveMultiByteNext(USCI_B0_BASE);
            __bic_SR_register_on_exit(LPM0_bits);
        }
        break;
    }
    case USCI_I2C_UCTXIFG:
    {
        //Check TX byte counter
        if (transmitCounter < txlength){
            //Initiate send of character from Master to Slave
            USCI_B_I2C_masterSendMultiByteNext(USCI_B0_BASE, transmitData[transmitCounter]);
            //Increment TX byte counter
            transmitCounter++;
        } else {
            //Initiate stop only
            USCI_B_I2C_masterSendMultiByteStop(USCI_B0_BASE);
            //Clear master interrupt status
            USCI_B_I2C_clearInterrupt(USCI_B0_BASE, USCI_B_I2C_TRANSMIT_INTERRUPT);
        }
        break;
    }
    default: break;
  }
}
