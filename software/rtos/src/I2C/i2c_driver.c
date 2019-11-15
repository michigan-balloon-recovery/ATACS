/* --COPYRIGHT--,BSD
 * Copyright (c) 2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
//*****************************************************************************
//  MSP430FR2311 eUSCI_B0 I2C Master
//
//  Description: This demo connects two MSP430's via the I2C bus. The master is
//  MSP430FR2311 with hardware I2C(eUSCI_B0). The slave is MSP430FR2111 using GPIOs
//  to implement software I2C. The master write to and read from the slave.
//  This is the master code.
//  ACLK = default REFO ~32768Hz, MCLK = SMCLK = default DCODIV ~1MHz.
//
//    *****used with "FR2111_SW_I2C_Slave.c"****
//
//                                /|\  /|\
//               MSP430FR2311      10k  10k     MSP430FR2111
//                   master         |    |        slave
//             -----------------   |    |   -----------------
//            |     P1.2/UCB0SDA|<-|----|->|P2.0(SW I2C)     |
//            |                 |  |       |                 |
//            |                 |  |       |                 |
//            |     P1.3/UCB0SCL|<-|------>|P1.0(SW I2C)     |
//            |                 |          |                 |
//
//   Texas Instruments Inc.
//   June. 2016
//******************************************************************************
#include <msp430.h>
#include "i2c_driver.h"

unsigned char RXData[9];                 // RX data
unsigned char RXByteCtr;
unsigned char TotalRXBytes;
unsigned char TXData[9]; // TX data
unsigned char TXByteCtr;
unsigned char TotalTXBytes;
char STATUS = 'i';

int setup(void)
{
    WDTCTL = WDTPW | WDTHOLD;

    //PM5CTL0 &= ~LOCKLPM5;

    // Configure GPIO
    UCB0TXBUF = 0x1;
    P3SEL = BIT1 | BIT2;                  // I2C pins
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
    UCB0BRW = 8;                           // baudrate = SMCLK / 40
    UCB0CTL1 &= ~UCSWRST;
    UCB0IE |= UCTXIE | UCRXIE | UCNACKIE;

    __bis_SR_register(GIE);
    __no_operation();
    __no_operation();

    return 0;
/*
    while (1)
    {
        __delay_cycles(1000);               // Delay between transmissions
        TXByteCtr = 4;                      // Load TX byte counter
        while (UCB0CTLW0 & UCTXSTP);        // Ensure stop condition got sent
        UCB0CTLW0 |= UCTR | UCTXSTT;        // I2C TX, start condition

        __bis_SR_register(LPM0_bits | GIE); // Enter LPM0 w/ interrupts
                                            // Remain in LPM0 until all data
                                            // is TX'd
    	__delay_cycles(1000);               // Delay between transmissions
        while (UCB0CTL1 & UCTXSTP);         // Ensure stop condition got sent
        UCB0CTLW0 &= ~UCTR;                 // I2C RX
        UCB0CTLW0 |= UCTXSTT;               // I2C start condition
        __bis_SR_register(LPM0_bits|GIE);   // Enter LPM0 w/ interrupt
    }
*/
}

void i2c_write(uint8_t addr, uint8_t * data, uint8_t numBytes){
	
	// check for inactive condition here
	while(STATUS != 'i');
	
	// set writing condition TRUE
	STATUS = 'w';
	
	UCB0I2CSA = addr;
//	TXData[0] = addr;
	TXByteCtr = numBytes;
	TotalTXBytes = TXByteCtr;
	int i;
	
	for(i=0; i < numBytes; i++){
		TXData[i] = *(data + (i)); 			// Max data length is 8 bytes
	}
	
	while (UCB0CTL1 & UCTXSTP);        // Ensure stop condition got sent
    UCB0CTLW0 |= UCTR | UCTXSTT;        // I2C TX, start condition
	
	// create writing condition here
	while(STATUS == 'w');
}

void i2c_read(uint8_t addr, uint8_t * data, uint8_t numBytes){
	
	// check for inactive condition here
	while(STATUS != 'i');
	int i = 0;
	
	// set reading condition TRUE
	STATUS = 'r';
	
	UCB0I2CSA = addr;
	TXByteCtr = 0;
	TotalTXBytes = TXByteCtr;
	RXByteCtr = numBytes;
	TotalRXBytes = RXByteCtr;
		
	while (UCB0CTL1 & UCTXSTP);        // Ensure stop condition got sent
	UCB0CTLW0 &= ~UCTR;
    UCB0CTLW0 |= UCTXSTT;        // I2C TX, start condition
	
	// create reading condition here
	while(STATUS == 'r');
	for(i = 0; i < numBytes; i++){
	    data[i] = RXData[i];
	}
}


#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = USCI_B0_VECTOR
__interrupt void USCI_B0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCI_B0_VECTOR))) USCI_B0_ISR (void)
#else
#error Compiler not supported!
#endif
{
  switch(__even_in_range(UCB0IV, USCI_I2C_UCTXIFG))
  {
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
             STATUS = 'i';			   // change status to inactive
             __bic_SR_register_on_exit(LPM0_bits);
             __no_operation();
             __no_operation();
             break;
         }
     } else{                           // Set debug flag and kill interrupt flag
         UCB0IFG &= ~UCRXIFG;
         STATUS = 'i';				   // change status to inactive
         __bic_SR_register_on_exit(LPM0_bits);
         __no_operation();
         __no_operation();
     }
      break;

    case USCI_I2C_UCTXIFG:
     if (TXByteCtr){                        // Check TX byte counter
         UCB0TXBUF = TXData[TotalTXBytes - TXByteCtr];   // Load TX buffer (starting from 0)
         TXByteCtr--;                       // Decrement TX byte counter
     }
	 else if(STATUS == 'r'){				//reading status
		 UCB0CTLW0 &= ~UCTR;
		 if(TotalRXBytes > 1){
			 UCB0CTLW0 |= UCTXSTT;          // I2C start condition
		 }
		 else{		//according to Scott, single byte read needs start/stop simultaneously
			 UCB0CTLW0 |= UCTXSTT;          // I2C start condition
			 while(UCB0CTLW0 & UCTXSTT);
			 UCB0CTLW0 |= UCTXSTP;
		 }
	 }
     else{
         UCB0CTLW0 |= UCTXSTP;              // I2C stop condition
         UCB0IFG &= ~UCTXIFG;               // Clear USCI_B0 TX int flag
		 STATUS = 'i';						// change status to inactive
         __bic_SR_register_on_exit(LPM0_bits);// Exit LPM0
         __no_operation();
         __no_operation();
     }
       break;

    default: break;
  }
}
