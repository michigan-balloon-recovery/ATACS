#include <msp430.h>
#include "i2c_driver.h"
#include <driverlib.h>

unsigned char RXData[9];                 // RX data
unsigned char RXByteCtr;
unsigned char TotalRXBytes;
unsigned char TXData[9]; // TX data
unsigned char TXByteCtr;
unsigned char TotalTXBytes;

int i2c_setup(void) {
    // Configure GPIO
    P3SEL = BIT1 | BIT2;                  // I2C pins
    i2c_txrx_semaphore = xSemaphoreCreateBinary();
    i2c_busy_semaphore = xSemaphoreCreateMutex();
    P3OUT = BIT1 | BIT2;
    P3REN = BIT1 | BIT2;
    P4DIR |= BIT7;
    P4SEL |= BIT7;

    __delay_cycles(500000);                // Delay to wait I2C slave stable time

    // Configure USCI_B0 for I2C mode
    UCB0CTL1 |= UCSWRST;                   // Software reset enabled
    UCB0CTL0 |= UCMODE_3 | UCMST | UCSYNC; // I2C mode, Master mode, sync
    UCB0CTL1 |=  UCSSEL__SMCLK;
                                            // after UCB0TBCNT is reached
    UCB0BRW = 40;                           // baudrate = SMCLK / 40
    UCB0CTL1 &= ~UCSWRST;
    UCB0IE |= UCTXIE | UCRXIE | UCNACKIE;

    return 0;
}

bool i2c_write(uint8_t addr, uint8_t * data, uint8_t numBytes) {

    if(xSemaphoreTake(i2c_busy_semaphore, 20000 / portTICK_RATE_MS) == pdFALSE) {
        return false;
    }
	
	UCB0I2CSA = addr;
	TXByteCtr = numBytes;
	TotalTXBytes = TXByteCtr;
	int i;
	
	for(i=0; i < numBytes; i++){
		TXData[i] = *(data + (i)); 			// Max data length is 8 bytes
	}
	
	while (UCB0CTL1 & UCTXSTP);        // Ensure stop condition got sent
	UCB0CTLW0 |= UCTR;
//	while(UCB0STAT & UCBBUSY);
    UCB0CTLW0 |= UCTXSTT;        // I2C TX, start condition
	
    bool success = false;
	if(xSemaphoreTake(i2c_txrx_semaphore,1000/portTICK_RATE_MS) == pdTRUE) {
	    // it worked
	    success = true;
	}
	else {
	    success = false;
	    // it failed
	}

	xSemaphoreGive(i2c_busy_semaphore);
	return success;
}

bool i2c_read(uint8_t addr, uint8_t * data, uint8_t numBytes) {
	
	int i = 0;
	
    if(xSemaphoreTake(i2c_busy_semaphore, 20000 / portTICK_RATE_MS) == pdFALSE) {
        return false;
    }

	UCB0I2CSA = addr;
	TXByteCtr = 0;
	TotalTXBytes = TXByteCtr;
	RXByteCtr = numBytes;
	TotalRXBytes = RXByteCtr;

	UCB0CTL1 &= ~UCTXSTP;
	while (UCB0CTL1 & UCTXSTP);        // Ensure stop condition got sent
	UCB0CTLW0 &= ~UCTR;
    UCB0CTLW0 |= UCTXSTT;        // I2C TX, start condition

    bool success = false;
	
	if(xSemaphoreTake(i2c_txrx_semaphore,1000/portTICK_RATE_MS) == pdTRUE) {
	    // it worked TODO:
	    success = true;
	    for(i = 0; i < numBytes; i++) {
	        data[i] = RXData[i];
	    }
	}
	else{
	    success = false;
	    // it failed
	}

    xSemaphoreGive(i2c_busy_semaphore);
    return success;
}

void i2c_enable_interrupts(void) {
    UCB0CTL1 |= UCSWRST;
    UCB0IE |= UCTXIE | UCRXIE | UCNACKIE;
    UCB0CTL1 &= ~UCSWRST;
    xSemaphoreGive(i2c_busy_semaphore);
}

bool i2c_disable_interrupts(void) {

    if(xSemaphoreTake(i2c_busy_semaphore, 20000 / portTICK_RATE_MS) == pdFALSE) {
        return false;
    }

    UCB0CTL1 |= UCSWRST;
    UCB0IE &= ~(UCTXIE | UCRXIE | UCNACKIE);
    return true;
    //UCB0CTL1 &= ~UCSWRST;
}

#pragma vector = USCI_B0_VECTOR
__interrupt void USCI_B0_ISR(void) {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  switch(__even_in_range(UCB0IV, USCI_I2C_UCTXIFG)) {
    case USCI_NONE: break;                  // Vector 0: No interrupts
    case USCI_I2C_UCALIFG: break;           // Vector 2: ALIFG
    case USCI_I2C_UCNACKIFG:                // Vector 4: NACKIFG
	 /*if ((STATUS != 'r') && (TXByteCtr < TotalTXBytes) && (TXByteCtr >= 1)){
		 TXByteCtr++;						// resend byte that was dropped
	 }*/
      UCB0CTL1 |= UCTXSTT;                  // I2C start condition
      break;
    case USCI_I2C_UCSTTIFG: break;          // Vector 6: STTIFG
    case USCI_I2C_UCSTPIFG: break;          // Vector 8: STPIFG
    
	case USCI_I2C_UCRXIFG:                 // Vector 24: RXIFG0
	 if (RXByteCtr) {
         RXData[(TotalRXBytes - RXByteCtr)] = UCB0RXBUF;    // Get RX data
         if(RXByteCtr == 2){// Because one additional byte is always transferred after the stop is set??
             UCB0CTLW0 |= UCTXSTP;
         }
         RXByteCtr--;
         if(RXByteCtr == 0){           // Clear lpm0 on return from interrupt
             UCB0IFG &= ~UCRXIFG;
             break;
         }
     } else{                           // Set debug flag and kill interrupt flag
         UCB0IFG &= ~UCRXIFG;
         xSemaphoreGiveFromISR(i2c_txrx_semaphore, &xHigherPriorityTaskWoken);
     }
      break;

    case USCI_I2C_UCTXIFG:
     if (TXByteCtr){                        // Check TX byte counter
         UCB0TXBUF = TXData[TotalTXBytes - TXByteCtr];   // Load TX buffer (starting from 0)
         TXByteCtr--;                       // Decrement TX byte counter
     }
//	 else if(STATUS == 'r'){				//reading status
//		 UCB0CTLW0 &= ~UCTR;
//		 if(TotalRXBytes > 1){
//			 UCB0CTLW0 |= UCTXSTT;          // I2C start condition
//		 }
//		 else{		//according to Scott, single byte read needs start/stop simultaneously
//			 UCB0CTLW0 |= UCTXSTT;          // I2C start condition
//			 while(UCB0CTLW0 & UCTXSTT);
//			 UCB0CTLW0 |= UCTXSTP;
//		 }
//	 }
     else {
         UCB0CTLW0 |= UCTXSTP;              // I2C stop condition
         UCB0IFG &= ~UCTXIFG;               // Clear USCI_B0 TX int flag
         xSemaphoreGiveFromISR(i2c_txrx_semaphore, &xHigherPriorityTaskWoken);
     }
       break;

    default: break;
  }
  portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}
