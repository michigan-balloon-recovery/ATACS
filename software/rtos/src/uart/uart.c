/*
* Copyright (c) 2012 All Right Reserved, Gustavo Litovsky
*
* You may use this file for any purpose, provided this copyright notice and
* the attribution remains.
*
* THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
* PARTICULAR PURPOSE.
*
* Author: Gustavo Litovsky
* gustavo@glitovsky.com
*
* File:    uart.c
* Summary: Implementation of generic MSP430 UART Driver
*
*/

/* Modified by: Paul Young
 * Date: Nov 2019
 */

#include <msp430.h>
#include <math.h>
#include <string.h>
#include "uart.h"

// Port Information List so user isn't forced to pass information all the time
UARTConfig * prtInfList[5];

/* ----- Private Function Prototypes ----- */
static void uartRxIsr(UARTConfig * prtInf);
static void uartTxIsr(UARTConfig * prtInf);

/*!
 * \brief Initializes the UART Driver
 *
 *
 * @param None
 * \return None
 *
 */
void initUartDriver()
{
	int i = 0;
	for(i = 0; i < 5; i++)
	{
		prtInfList[i] = NULL;
	}
}

int initUSCIUart(UARTConfig * prtInf, ring_buff_t *txbuf, ring_buff_t *rxbuf){
    int res = UART_SUCCESS;
	prtInf->rxBuf = rxbuf;
	prtInf->rxCallback = NULL;
	prtInf->rxCallbackParams = NULL;
	prtInf->txBuf = txbuf;
	prtInf->txCallback = NULL;
	prtInf->txCallbackParams = NULL;
	switch(prtInf->moduleName){
		case USCI_A0:
			memcpy(&USCI_A0_cnf, prtInf, sizeof(UARTConfig));
			res = configUSCIUart(&USCI_A0_cnf,&USCI_A0_regs);
			if(res != UART_SUCCESS) {
				// Failed to initialize UART for some reason
				return UART_UNKNOWN;
			}
			enableUartRx(&USCI_A0_cnf);
			break;
		case USCI_A1:
			memcpy(&USCI_A1_cnf, prtInf, sizeof(UARTConfig));
			res = configUSCIUart(&USCI_A1_cnf,&USCI_A1_regs);
			if(res != UART_SUCCESS) {
				// Failed to initialize UART for some reason
				return UART_UNKNOWN;
			}
			enableUartRx(&USCI_A1_cnf);
			break;
		case USCI_A2:
			memcpy(&USCI_A2_cnf, prtInf, sizeof(UARTConfig));
			res = configUSCIUart(&USCI_A2_cnf,&USCI_A2_regs);
			if(res != UART_SUCCESS) {
				// Failed to initialize UART for some reason
				return UART_UNKNOWN;
			}
			enableUartRx(&USCI_A2_cnf);
			break;
		case USCI_A3:
			memcpy(&USCI_A3_cnf, prtInf, sizeof(UARTConfig));
			res = configUSCIUart(&USCI_A3_cnf,&USCI_A3_regs);
			if(res != UART_SUCCESS) {
				// Failed to initialize UART for some reason
				return UART_UNKNOWN;
			}
			enableUartRx(&USCI_A3_cnf);
			break;
		default:
			return UART_INVALID_MODULE;
	}
	return UART_SUCCESS;
}

/*!
 * \brief Registers the RX Callback for the UART module
 * 
 * This function allows a RX Callback function being run when a UART message is recieved.
 * The callback function will be called from within an Interrupt Service Routine (ISR).
 * The callback function can access application specific data through a void pointer stored in the UARTConfig instance.
 * This application data should be global in scope.
 * 
 * @param prtInf is UARTConfig instance with the configuration settings
 * @param callback is the function pointer of the callback function
 * @param params is a pointer to the memory storing application parameters to the callback function
 * 
 */
void initUartRxCallback(UARTConfig * prtInf, void (*callback) (void *params, uint8_t datum), void *params) {
	prtInf->rxCallback = callback;
	prtInf->rxCallbackParams = params;
}

/*!
 * \brief Registers the TX Callback for the UART module
 * 
 * This function allows a TX Callback function being run when a byte is transmitted over UART using interrupts.
 * The callback function will be called from within an Interrupt Service Routine (ISR).
 * The callback function can access application specific data through a void pointer stored in the UARTConfig instance.
 * This application data should be global in scope.
 * 
 * @param prtInf is UARTConfig instance with the configuration settings
 * @param callback is the function pointer of the callback function
 * @param params is a pointer to the memory storing application parameters to the callback function
 * 
 */
void initUartTxCallback(UARTConfig * prtInf, bool (*callback) (void *params, uint8_t *txAddress), void *params) {
	prtInf->txCallback = callback;
	prtInf->txCallbackParams = params;
}
/*!
 * \brief Configures the MSP430 pins for UART module
 *
 *
 * @param prtInf is UARTConfig instance with the configuration settings
 * * Restarts reading in the current packet.
 * * Restarts reading in the current packet.
 */
int initUartPort(UARTConfig * prtInf)
{
	unsigned char * prtSelReg = NULL;
	switch(prtInf->portNum)
	{
#ifdef __MSP430_HAS_PORT1_R__
		case 1:
			prtSelReg = (unsigned char *)&P1SEL;
			break;
#endif
#ifdef __MSP430_HAS_PORT2_R__
		case 2:
			prtSelReg = (unsigned char *)&P2SEL;
			break;
#endif
#ifdef __MSP430_HAS_PORT3_R__
		case 3:
			prtSelReg = (unsigned char *)&P3SEL;
			break;
#endif
#ifdef __MSP430_HAS_PORT4_R__
		case 4:
			prtSelReg = (unsigned char *)&P4SEL;
			break;
#endif
#ifdef __MSP430_HAS_PORT5_R__
		case 5:
			prtSelReg = (unsigned char *)&P5SEL;
			break;
#endif
#ifdef __MSP430_HAS_PORT6_R__
		case 6:
			prtSelReg = (unsigned char *)&P6SEL;
			break;
#endif
#ifdef __MSP430_HAS_PORT7_R__
		case 7:
			prtSelReg = (unsigned char *)&P7SEL;
			break;
#endif
#ifdef __MSP430_HAS_PORT8_R__
		case 8:
			prtSelReg = (unsigned char *)&P8SEL;
			break;
#endif
#ifdef __MSP430_HAS_PORT9_R__
		case 9:
			prtSelReg = (unsigned char *)&P9SEL;
			break;
#endif
#ifdef __MSP430_HAS_PORT10_R__
        case 10:
            prtSelReg = (unsigned char *)&P10SEL;
            break;
#endif
		default:
			prtSelReg = NULL;
			break;
	}

	if(prtSelReg == NULL)
	{
		return UART_BAD_PORT_SELECTED;
	}

	// Configure Port to use UART Module by setting the corresponding bits on the PxSEL
	// register.
	*prtSelReg |=  (BIT0 << prtInf->RxPinNum) | (BIT0 << prtInf->TxPinNum);

	return UART_SUCCESS;
}


/*!
 * \brief Configures the UART Pins and Module for communications
 *
 * This function accepts a UARTConfig instance and initializes
 * the UART module appropriately. See UARTConfig for more info.
 *
 *
 * @param prtInf is UARTConfig instance with the configuration settings
 * @param confRegs is a pointer to a struct holding the configuration register
 * \return Success or errors as defined by UART_ERR_CODES
 *
 */
#if defined(__MSP430_HAS_USCI__) || defined(__MSP430_HAS_USCI_A0__) || defined(__MSP430_HAS_USCI_A1__) || defined(__MSP430_HAS_USCI_A2__) || defined(__MSP430_HAS_USCI_A3__)
int configUSCIUart(UARTConfig * prtInf,USCIUARTRegs * confRegs)
{
	initUartPort(prtInf);

	// Configure the pointers to the right registers
	switch(prtInf->moduleName)
	{
		case USCI_A0:
#if (defined(__MSP430_HAS_USCI_A0__)) && (!defined(__MSP430_HAS_USCI__))
			confRegs->CTL0_REG = (unsigned char *)&UCA0CTL0;
			confRegs->CTL1_REG = (unsigned char *)&UCA0CTL1;
			confRegs->MCTL_REG = (unsigned char *)&UCA0MCTL;
			confRegs->BR0_REG  = (unsigned char *)&UCA0BR0;
			confRegs->BR1_REG  = (unsigned char *)&UCA0BR1;
			confRegs->IE_REG  = (unsigned char *)&UCA0IE;
			confRegs->RX_BUF = (unsigned char *)&UCA0RXBUF;
			confRegs->TX_BUF = (unsigned char *)&UCA0TXBUF;
			confRegs->IFG_REG = (unsigned char *)&UCA0IFG;
#endif
			break;
#if defined(__MSP430_HAS_USCI__) && (!defined(__MSP430_HAS_USCI_A0__)) && (!defined(__MSP430_HAS_USCI_A1__)) && (!defined(__MSP430_HAS_USCI_A2__))
		case UCA0:
			confRegs->CTL0_REG = (unsigned char *)&UCA0CTL0;
			confRegs->CTL1_REG = (unsigned char *)&UCA0CTL1;
			confRegs->MCTL_REG = (unsigned char *)&UCA0MCTL;
			confRegs->BR0_REG  = (unsigned char *)&UCA0BR0;
			confRegs->BR1_REG  = (unsigned char *)&UCA0BR1;
			confRegs->IE_REG  = (unsigned char *)&UC0IE;
			confRegs->RX_BUF = (unsigned char *)&UCA0RXBUF;
			confRegs->TX_BUF = (unsigned char *)&UCA0TXBUF;
			confRegs->IFG_REG = (unsigned char *)&UC0IFG;
			break;
#endif

#if (defined(__MSP430_HAS_USCI_A1__)) && (!defined(__MSP430_HAS_USCI__))
		case USCI_A1:
			confRegs->CTL0_REG = (unsigned char *)&UCA1CTL0;
			confRegs->CTL1_REG = (unsigned char *)&UCA1CTL1;
			confRegs->MCTL_REG = (unsigned char *)&UCA1MCTL;
			confRegs->BR0_REG  = (unsigned char *)&UCA1BR0;
			confRegs->BR1_REG  = (unsigned char *)&UCA1BR1;
			confRegs->IE_REG  =  (unsigned char *)&UCA1IE;
			confRegs->RX_BUF =   (unsigned char *)&UCA1RXBUF;
			confRegs->TX_BUF =   (unsigned char *)&UCA1TXBUF;
			confRegs->IFG_REG =  (unsigned char *)&UCA1IFG;
			break;
#endif
#if (defined(__MSP430_HAS_USCI_A2__)) && (!defined(__MSP430_HAS_USCI__))
		case USCI_A2:
			confRegs->CTL0_REG = (unsigned char *)&UCA2CTL0;
			confRegs->CTL1_REG = (unsigned char *)&UCA2CTL1;
			confRegs->MCTL_REG = (unsigned char *)&UCA2MCTL;
			confRegs->BR0_REG  = (unsigned char *)&UCA2BR0;
			confRegs->BR1_REG  = (unsigned char *)&UCA2BR1;
			confRegs->IE_REG  = (unsigned char *)&UCA2IE;
			confRegs->RX_BUF = (unsigned char *)&UCA2RXBUF;
			confRegs->TX_BUF = (unsigned char *)&UCA2TXBUF;
			confRegs->IFG_REG = (unsigned char *)&UCA2IFG;
			break;
#endif
#if (defined(__MSP430_HAS_USCI_A3__)) && (!defined(__MSP430_HAS_USCI__))
        case USCI_A3:
            confRegs->CTL0_REG = (unsigned char *)&UCA3CTL0;
            confRegs->CTL1_REG = (unsigned char *)&UCA3CTL1;
            confRegs->MCTL_REG = (unsigned char *)&UCA3MCTL;
            confRegs->BR0_REG  = (unsigned char *)&UCA3BR0;
            confRegs->BR1_REG  = (unsigned char *)&UCA3BR1;
            confRegs->IE_REG  = (unsigned char *)&UCA3IE;
            confRegs->RX_BUF = (unsigned char *)&UCA3RXBUF;
            confRegs->TX_BUF = (unsigned char *)&UCA3TXBUF;
            confRegs->IFG_REG = (unsigned char *)&UCA3IFG;
            break;
#endif

	}

	// Place Module in reset to allow us to modify its bits
	*confRegs->CTL1_REG |= UCSWRST;

	// Configure UART Settings
	if(prtInf->parity == UART_PARITY_EVEN)
	{
		*confRegs->CTL0_REG |= UCPEN | UCPAR;
	}
	else if(prtInf->parity == UART_PARITY_ODD)
	{
		*confRegs->CTL0_REG |= UCPEN;
	}

	if(prtInf->databits == 7)
	{
		*confRegs->CTL0_REG |= UC7BIT;
	}

	if(prtInf->stopbits == 2)
	{
		*confRegs->CTL0_REG |= UCSPB;
	}

	// Configure clock source

	// Clear clock source bits, then set the proper ones;
	*confRegs->CTL1_REG &= ~(UCSSEL1 | UCSSEL0);

	switch(prtInf->clkSrc)
	{
		case UART_CLK_SRC_ACLK:
			*confRegs->CTL1_REG |= (UCSSEL0);
			break;
		case UART_CLK_SRC_SMCLK:
			*confRegs->CTL1_REG |= (UCSSEL1);
			break;
	}

	// Set the baudrate dividers and modulation
	unsigned int N_div;
	N_div = prtInf->clkRate / prtInf->baudRate;

	float N_div_f;
	N_div_f = (float)prtInf->clkRate / (float)prtInf->baudRate;

	if(N_div >= 16)
	{
		// We can use Oversampling mode
		N_div /= 16;
		*confRegs->BR0_REG = (N_div & 0x00FF);
		*confRegs->BR1_REG = ((N_div & 0xFF00) >> 8);

		N_div_f /= 16.0;
		*confRegs->MCTL_REG = (unsigned char)(((N_div_f) - round(N_div_f))*16.0f) << 4; // Set BRF
		*confRegs->MCTL_REG |= UCOS16; // Enable Oversampling Mode
	}
	else
	{
		// We must use the Low Frequency mode
		*confRegs->BR0_REG = (N_div & 0x00FF);
		*confRegs->BR1_REG = ((N_div & 0xFF00) >> 8);

		*confRegs->MCTL_REG = (unsigned char)((N_div_f - round(N_div_f))*8.0f) << 1; // Set BRS
	}

	// Take Module out of reset
	*confRegs->CTL1_REG &= ~UCSWRST;

	// Sets the pointer to the register configuration so we don't
	// have to keep passing it around and can pass only the UART configuration
	prtInf->usciRegs = confRegs;

	// Assign pointer to port information to the array so it can be accessed later
	prtInfList[prtInf->moduleName] = prtInf;

	return UART_SUCCESS;
}
#endif

/*!
 * \brief Configures the UART Pins and Module for communications
 *
 * This function accepts a UARTConfig instance and initializes
 * the UART module appropriately. See UARTConfig for more info.
 *
 *
 * @param prtInf is UARTConfig instance with the configuration settings
 *
 * \return Success or errors as defined by UART_ERR_CODES
 *
 */
#if defined(__MSP430_HAS_UART0__) || defined(__MSP430_HAS_UART1__)
int configUSARTUart(UARTConfig * prtInf, USARTUARTRegs * confRegs)
{
	initUartPort(prtInf);

	// Configure the pointers to the right registers
	switch(prtInf->moduleName)
	{
#ifdef __MSP430_HAS_UART0__
		case USART_0:
			*confRegs->ME_REG =     ME1;
			*confRegs->U0CTL_REG =  UCTL0;
			*confRegs->UTCLT0_REG = UTCTL0;
			*confRegs->UBR0_REG =   UBR00;
			*confRegs->UBR1_REG =   UBR10;
			*confRegs->UMCTL_REG = UMCTL0;
			*confRegs->IE_REG =     IE1;
			*confRegs->RX_BUF =     RXBUF0;
			*confRegs->TX_BUF =     TXBUF0;
			*confRegs->IFG_REG =    IFG1 ;
			confRegs->TXIFGFlag =  UTXIFG0;
			confRegs->RXIFGFlag =  URXIFG0;
			confRegs->TXIE = UTXIE0;
			confRegs->RXIE = URXIE0;
			break;
#endif

#ifdef __MSP430_HAS_UART1__
		case USART_1:
			*confRegs->ME_REG =     ME2;
			*confRegs->U0CTL_REG =  UCTL1;
			*confRegs->UTCLT0_REG = UTCTL1;
			*confRegs->UBR0_REG =   UBR01;
			*confRegs->UBR1_REG =   UBR11;
			*confRegs->UMCTL_REG = UMCTL1;
			*confRegs->IE_REG =     IE2;
			*confRegs->RX_BUF =     RXBUF1;
			*confRegs->TX_BUF =     TXBUF1;
			confRegs->TXIFGFlag =  UTXIFG1;
			confRegs->RXIFGFlag =  URXIFG1;
			confRegs->TXIE = UTXIE1;
			confRegs->RXIE = URXIE1;

			break;
#endif
	}

	// Place Module in reset to allow us to modify its bits
	*confRegs->U0CTL_REG |= SWRST;

	// Configure UART Settings
	if(prtInf->parity == UART_PARITY_EVEN)
	{
		*confRegs->U0CTL_REG |= PENA;
		*confRegs->U0CTL_REG |= PEV;
	}
	else if(prtInf->parity == UART_PARITY_ODD)
	{
		*confRegs->U0CTL_REG |= PENA;
		*confRegs->U0CTL_REG &= ~PEV;
	}
	else if(prtInf->parity == UART_PARITY_NONE)
	{
		*confRegs->U0CTL_REG &= ~PENA;
		*confRegs->U0CTL_REG &= ~PEV;
	}

	if(prtInf->databits == 7)
	{
		*confRegs->U0CTL_REG &= ~CHAR;
	}
	else if(prtInf->databits == 8)
	{
		*confRegs->U0CTL_REG |= CHAR;
	}

	if(prtInf->stopbits == 1)
	{
		*confRegs->U0CTL_REG &= ~SPB;
	}
	else if(prtInf->stopbits == 2)
	{
		*confRegs->U0CTL_REG |= SPB;
	}

	// Configure clock source

	// Clear clock source bits, then set the proper ones;
	*confRegs->UTCLT0_REG &= ~(SSEL1 | SSEL0);

	switch(prtInf->clkSrc)
	{
		case UART_CLK_SRC_ACLK:
			*confRegs->UTCLT0_REG |= (SSEL0);
			break;
		case UART_CLK_SRC_SMCLK:
			*confRegs->UTCLT0_REG |= (SSEL1);
			break;
	}

	// Set the baudrate dividers and modulation
	unsigned int N_div;
	N_div = prtInf->clkRate / prtInf->baudRate;


	*confRegs->UBR0_REG = (N_div & 0x00FF);
	*confRegs->UBR1_REG = ((N_div & 0xFF00) >> 8);

	// Modulation currently set to 0. Needs proper handling
	*confRegs->UMCTL_REG = 0;

	// Take Module out of reset
	*confRegs->U0CTL_REG &= ~SWRST;


	prtInf->usartRegs = confRegs;

	// Assign pointer to port information to the array so it can be accessed later
	prtInfList[prtInf->moduleName] = prtInf;

	return UART_SUCCESS;
}

#endif

/*!
 * \brief Returns a pointer to the RX Buffer
 *
 *
 * @param prtInf is a pointer to the UART configuration
 *
 * \return pointer to the RX buffer of the UART
 *
 */
ring_buff_t * getUartRxBuffer(UARTConfig * prtInf)
{
	return prtInf->rxBuf;
}


/*!
 * \brief Sends len number of bytes from the buffer using the specified
 * UART, but does so by blocking.
 *
 * This function is blocking, although interrupts may trigger during its
 * execution.
 *
 * @param prtInf is a pointer to the UART configuration
 * @param buf is a pointer to the buffer containing the bytes to be sent.
 * @param len is an integer containing the number of bytes to send.
 *
 * \return Success or errors as defined by UART_ERR_CODES
 *
 */
int uartSendDataBlocking(UARTConfig * prtInf,unsigned char * buf, int len)
{
	int i = 0;
	for(i = 0; i < len; i++)
	{
#if (defined(__MSP430_HAS_USCI_A0__) || defined(__MSP430_HAS_USCI_A1__) || defined(__MSP430_HAS_USCI_A2__)) || defined(__MSP430_HAS_USCI_A3__) && (!defined(__MSP430_HAS_USCI__))
		if(prtInf->moduleName == USCI_A0|| prtInf->moduleName == USCI_A1 || prtInf->moduleName == USCI_A2 || prtInf->moduleName == USCI_A3)
		{

			while(!( *prtInf->usciRegs->IFG_REG & UCTXIFG));
			*prtInf->usciRegs->TX_BUF = buf[i];
		}
#else
		if(prtInf->moduleName == UCA0)
		{
			while(!( *prtInf->usciRegs->IFG_REG & UCA0TXIFG));
			*prtInf->usciRegs->TX_BUF = buf[i];
		}
#endif


		if(prtInf->moduleName == USART_0|| prtInf->moduleName == USART_1)
		{
#if defined(__MSP430_HAS_UART0__) || defined(__MSP430_HAS_UART1__)
			while(!(*prtInf->usartRegs->IFG_REG & prtInf->usartRegs->TXIFGFlag));
			*prtInf->usciRegs->TX_BUF = buf[i];
#endif
		}
	}

	return UART_SUCCESS;
}


/*!
 * \brief Sends a string over the specified UART
 *
 * This function is blocking, although interrupts may trigger during its
 * execution.
 *
 * @param prtInf is a pointer to the UART configuration
 * @param uartId is the unique UART identifier, usually provided by the call to configUart.
 * @param string is a pointer to the string to be sent
 *
 * \par The string provided to the function must be null terminated.
 *
 * \return 0 if successful , -1 if failed
 *
 */
int uartSendStringBlocking(UARTConfig * prtInf,char * string)
{
	int res = 0;
	res = uartSendDataBlocking(prtInf,(unsigned char*)string, strlen(string));

	return res;
}

/*!
 * \brief Sends len number of bytes from the buffer using the specified
 * UART using interrupt driven.
 *
 * TX Interrupts are enabled and each time that the UART TX Buffer is empty
 * and there is more data to send, data is sent. Once the byte is sent, another
 * interrupt is triggered, until all bytes in the buffer sent.
 *
 * @param prtInf is a pointer to the UART configuration
 * @param buf is a pointer to the buffer containing the bytes to be sent.
 * @param len is an integer containing the number of bytes to send.
 *
 * \return Success or errors as defined by UART_ERR_CODES
 *
 */
int uartSendDataInt(UARTConfig * prtInf,unsigned char * buf, int len)
{
	int i = 0;
	if(prtInf->txBuf != NULL) {
		for(i = 0; i < len; i++)
		{
			if(!ring_buff_write(prtInf->txBuf, buf[i])) {
				ring_buff_write_clear_packet(prtInf->txBuf);
				return UART_INSUFFICIENT_TX_BUF;
			}
		}
		ring_buff_write_finish_packet(prtInf->txBuf);
	}
	// if no ring buffer is registered with the UART config, data transfer must be handled
	// through the TX callback function
	else if(buf == NULL) {
		return UART_NO_TX_BUFF;
	}

	// Send the first byte. Since UART interrupt is enabled, it will be called once the byte is sent and will
	// send the rest of the bytes

#if (defined(__MSP430_HAS_USCI_A0__) || defined(__MSP430_HAS_USCI_A1__) || defined(__MSP430_HAS_USCI_A2__)) && (!defined(__MSP430_HAS_USCI__))

	if(prtInf->moduleName == USCI_A0 || prtInf->moduleName == USCI_A1 || prtInf->moduleName == USCI_A2 || prtInf->moduleName == USCI_A3)
	{
		// Enable TX IE
		*prtInf->usciRegs->IFG_REG &= ~UCTXIFG;
		*prtInf->usciRegs->IE_REG |= UCTXIE;

		// Trigger the TX IFG. This will cause the Interrupt Vector to be called
		// which will send the data one byte at a time at each interrupt trigger.
		*prtInf->usciRegs->IFG_REG |= UCTXIFG;
	}
#else
	if(prtInf->moduleName == UCA0)
	{
		// Enable TX IE
		*prtInf->usciRegs->IFG_REG &= ~UCA0TXIFG;
		*prtInf->usciRegs->IE_REG |= UCA0TXIE;

		// Trigger the TX IFG. This will cause the Interrupt Vector to be called
		// which will send the data one byte at a time at each interrupt trigger.
		*prtInf->usciRegs->IFG_REG |= UCA0TXIFG;
	}
#endif

#if defined(__MSP430_HAS_UART0__) || defined(__MSP430_HAS_UART1__)
	if(prtInf->moduleName == USART_0|| prtInf->moduleName == USART_1)
	{
		// Clear TX IFG and Enable TX IE
		*prtInf->usartRegs->IFG_REG &= ~ prtInf->usartRegs->TXIFGFlag;
		*prtInf->usartRegs->IE_REG |= prtInf->usartRegs->TXIE;

		// Trigger the TX IFG. This will cause the Interrupt Vector to be called
		// which will send the data one byte at a time at each interrupt trigger.
		*prtInf->usartRegs->IFG_REG |= prtInf->usartRegs->TXIFGFlag;

	}
#endif
	return UART_SUCCESS;
}

void enableUartRx(UARTConfig * prtInf)
{
#if (defined(__MSP430_HAS_USCI_A0__) || defined(__MSP430_HAS_USCI_A1__) || defined(__MSP430_HAS_USCI_A2__)) && (!defined(__MSP430_HAS_USCI__))
	if(prtInf->moduleName == USCI_A0|| prtInf->moduleName == USCI_A1 || prtInf->moduleName == USCI_A2 || prtInf->moduleName == USCI_A3)
	{
		// Enable RX IE
		*prtInf->usciRegs->IFG_REG &= ~UCRXIFG;
		*prtInf->usciRegs->IE_REG |= UCRXIE;
	}
#else
	if(prtInf->moduleName == UCA0)
	{
		// Enable RX IE
		*prtInf->usciRegs->IFG_REG &= ~UCA0RXIFG;
		*prtInf->usciRegs->IE_REG |= UCA0RXIE;
	}
#endif

#if defined(__MSP430_HAS_UART0__) || defined(__MSP430_HAS_UART1__)
	if(prtInf->moduleName == USART_0|| prtInf->moduleName == USART_1)
	{
		// Enable RX IE
		*prtInf->usartRegs->IFG_REG &= ~   prtInf->usartRegs->RXIFGFlag;
		*prtInf->usartRegs->IE_REG |= prtInf->usartRegs->RXIE;
	}
#endif
}

static void uartRxIsr(UARTConfig * prtInf) {
	// rx Callback
	if(prtInf->rxCallback != NULL) {
		prtInf->rxCallback(prtInf->rxCallbackParams, *prtInf->usciRegs->RX_BUF);
	}
	// default
	else if(prtInf->rxBuf != NULL) {
		ring_buff_write(prtInf->rxBuf, *prtInf->usciRegs->RX_BUF);
		ring_buff_write_finish_packet(prtInf->rxBuf);
	}
}

static void uartTxIsr(UARTConfig * prtInf) {
	// tx Callback
	if(prtInf->txCallback != NULL) {
		if(!prtInf->txCallback(prtInf->txCallbackParams, prtInf->usciRegs->TX_BUF)) {
            *prtInf->usciRegs->IE_REG &= ~UCTXIE;
            // Clear TX IFG
            *prtInf->usciRegs->IFG_REG &= ~UCTXIFG;
		}
	}
	// default
	else if(prtInf->txBuf != NULL) {
		// Send data if the buffer has bytes to send
		if(!ring_buff_read(prtInf->txBuf, prtInf->usciRegs->TX_BUF)) {
			ring_buff_read_finish_packet(prtInf->txBuf);
			// Disable TX IE
			*prtInf->usciRegs->IE_REG &= ~UCTXIE;
			// Clear TX IFG
			*prtInf->usciRegs->IFG_REG &= ~UCTXIFG;
		}
	}
}

#if defined(__MSP430_HAS_UART0__)
// UART0 TX ISR
#pragma vector=USART0TX_VECTOR
__interrupt void usart0_tx (void)
{
	// Send data if the buffer has bytes to send
	if(prtInfList[USART_0]->txBytesToSend > 0)
	{
		*prtInfList[USART_0]->usciRegs->TX_BUF = prtInfList[USART_0]->txBuf[prtInfList[USART_0]->txBufCtr];
		prtInfList[USART_0]->txBufCtr++;

		// If we've sent all the bytes, set counter to 0 to stop the sending
		if(prtInfList[USART_0]->txBufCtr == prtInfList[USART_0]->txBytesToSend)
		{
		  prtInfList[USART_0]->txBufCtr = 0;
		}
	}
}


// UART0 RX ISR
#pragma vector=USART0RX_VECTOR
__interrupt void usart0_rx (void)
{
	UARTConfig * prtInf = prtInfList[USART_1];
	uartRxIsr(prtInf);
}

#endif

#if defined(__MSP430_HAS_UART1__)
// UART1 TX ISR
#pragma vector=USART1TX_VECTOR
__interrupt void usart1_tx (void)
{
	// Send data if the buffer has bytes to send
	if(prtInfList[USART_1]->txBytesToSend > 0)
	{
		*prtInfList[USART_1]->usciRegs->TX_BUF = prtInfList[USART_1]->txBuf[prtInfList[USART_1]->txBufCtr];
		prtInfList[USART_1]->txBufCtr++;

		// If we've sent all the bytes, set counter to 0 to stop the sending
		if(prtInfList[USART_1]->txBufCtr == prtInfList[USART_1]->txBytesToSend)
		{
		  prtInfList[USART_1]->txBufCtr = 0;
		}
	}
}


// UART1 RX ISR
#pragma vector=USART1RX_VECTOR
__interrupt void usart1_rx (void)
{
	UARTConfig * prtInf = prtInfList[USART_1];
	uartRxIsr(prtInf);
}

#endif

#if defined(__MSP430_HAS_USCI_A0__)
#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
{
	UARTConfig * prtInf = prtInfList[USCI_A0];
	switch(__even_in_range(UCA0IV,4))
	{
	  case 0:break;                             // Vector 0 - no interrupt
	  case 2:                                   // Vector 2 - RXIFG
		uartRxIsr(prtInf);
		break;
	  case 4:                                   // Vector 4 - TXIFG
		uartTxIsr(prtInf);
		break;
	  default: break;
	}
}
#endif

#if defined(__MSP430_HAS_USCI__) && (!defined(__MSP430_HAS_USCI_A0__)) && (!defined(__MSP430_HAS_USCI_A1__)) && (!defined(__MSP430_HAS_USCI_A2__))

// USCI A0/B0 Transmit ISR
#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void)
{
	// Send data if the buffer has bytes to send
	if(prtInfList[UCA0]->txBytesToSend > 0)
	{
	  *prtInfList[UCA0]->usciRegs->TX_BUF = prtInfList[UCA0]->txBuf[prtInfList[UCA0]->txBufCtr];
	  prtInfList[UCA0]->txBufCtr++;

	  // If we've sent all the bytes, set counter to 0 to stop the sending
	  if(prtInfList[UCA0]->txBufCtr == prtInfList[UCA0]->txBytesToSend)
	  {
		  prtInfList[UCA0]->txBufCtr = 0;

		  // Disable TX IE
		  *prtInfList[UCA0]->usciRegs->IE_REG &= ~UCA0TXIE;

		  // Clear TX IFG
		  *prtInfList[UCA0]->usciRegs->IFG_REG &= ~UCA0TXIFG;
	  }
	}
}

// USCI A0/B0 Receive ISR
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{
	// Store received byte in RX Buffer
	prtInfList[UCA0]->rxBuf[prtInfList[UCA0]->rxBytesReceived] = *prtInfList[UCA0]->usciRegs->RX_BUF;
	prtInfList[UCA0]->rxBytesReceived++;

	// If the received bytes filled up the buffer, go back to beginning
	if(prtInfList[UCA0]->rxBytesReceived > prtInfList[UCA0]->rxBufLen)
	{
	  prtInfList[UCA0]->rxBytesReceived = 0;
	}
}

#endif


#if defined(__MSP430_HAS_USCI_A1__)
#pragma vector=USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void)
{
	UARTConfig * prtInf = prtInfList[USCI_A1];
	switch(__even_in_range(UCA1IV,4))
	{
	  case 0:break;                             // Vector 0 - no interrupt
	  case 2:                                   // Vector 2 - RXIFG
		uartRxIsr(prtInf);
		break;
	  case 4:                                   // Vector 4 - TXIFG
		uartTxIsr(prtInf);
		break;
	  default: break;
	}
}
#endif

#if defined(__MSP430_HAS_USCI_A2__)
#pragma vector=USCI_A2_VECTOR
__interrupt void USCI_A2_ISR(void)
{
	UARTConfig * prtInf = prtInfList[USCI_A2];
	switch(__even_in_range(UCA2IV,4))
	{
	  case 0:break;                             // Vector 0 - no interrupt
	  case 2:                                   // Vector 2 - RXIFG
		uartRxIsr(prtInf);
		break;
	  case 4:                                   // Vector 4 - TXIFG
		uartTxIsr(prtInf);
		break;
	  default: break;
	}
}
#endif

#if defined(__MSP430_HAS_USCI_A3__)
#pragma vector=USCI_A3_VECTOR
__interrupt void USCI_A3_ISR(void)
{
	UARTConfig * prtInf = prtInfList[USCI_A3];
	switch(__even_in_range(UCA3IV,4))
	{
	  case 0:break;                             // Vector 0 - no interrupt
	  case 2:                                   // Vector 2 - RXIFG
		uartRxIsr(prtInf);
		break;
	  case 4:                                   // Vector 4 - TXIFG
		uartTxIsr(prtInf);
		break;
	  default: break;
	}
}
#endif

/*!
 * \brief Reads bytes from the Rx buffer
 *
 *	Note that when bytes are read using this function, their count will be reset.
 *	Therefore, it is recommended to read all bytes of interest at once and buffer on
 *	the user side as necessary.
 *
 * @param prtInf is UARTConfig instance with the configuration settings
 * @param data is a pointer to a user provided buffer to which the data will be written
 * @param numBytesToRead is the number of bytes to read from the buffer
 * @param offset is the offset in the buffer to read. Default is 0 to start at beginning of RX buffer
 * \return number of bytes placed in the data buffer
 *
 */
//int readRxBytes(UARTConfig * prtInf, unsigned char * data, int numBytesToRead, int offset)
//{
//	int bytes = 0;
//
//	// Ensure we don't read past what we have in the buffer
//	if(numBytesToRead+offset <= prtInf->rxBytesReceived )
//	{
//		bytes = numBytesToRead;
//	}
//	else if(offset < prtInf->rxBufLen)
//	{
//		// Since offset is valid, we provide all possible bytes until the end of the buffer
//		bytes = prtInf->rxBytesReceived - offset;
//	}
//	else
//	{
//		return 0;
//	}
//
//	int i = 0;
//	for(i = 0; i < bytes; i++)
//	{
//		data[i] = prtInf->rxBuf[offset+i];
//	}
//
//	// reset number of bytes available, regardless of how many bytes are left
//	prtInf->rxBytesReceived = 0;
//
//	return i;
//
//}
