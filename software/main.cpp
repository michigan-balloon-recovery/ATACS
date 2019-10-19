#include <msp430.h> 
#include <rockblock.h>

/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
//	padpin a(0,0);
//	RockBLOCK rb(a);

	return 0;
}
