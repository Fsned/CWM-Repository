#include "stdio.h"
#include "setup.h"
#include "lpc17xx.h"


void setup(void) {
	LPC_PINCON->PINSEL0 &= ~(0x0000000C);
	LPC_PINCON->PINSEL4 &= ~(0x0000CF30);	// Set P1:18, P1:20, P1:21, P1:23 to function 1 (GPIO)
	LPC_GPIO0->FIODIR   |=   0x0000000F;
	LPC_GPIO1->FIODIR   |=   0x00B40000;	// Set Port 1 as output
}


