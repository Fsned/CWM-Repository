#include "systick.h"
#include "lpc17xx.h"


#define STCTRL      (*( ( volatile unsigned long *) 0xE000E010 ))
#define STRELOAD    (*( ( volatile unsigned long *) 0xE000E014 ))
#define STCURR      (*( ( volatile unsigned long *) 0xE000E018 ))  

/*******STCTRL bits*******/
#define SBIT_ENABLE     0
#define SBIT_TICKINT    1
#define SBIT_CLKSOURCE  2


/* 100000000Mhz * 1ms = 100000 - 1 */
#define RELOAD_VALUE  99999


void SystickSetup(int frequency) {
	STRELOAD = ( 100000000 / ((frequency*2)*1.0425) );
	
	STCTRL = (1 << SBIT_ENABLE) | (1 << SBIT_TICKINT) | (1 << SBIT_CLKSOURCE);
	
	LPC_PINCON->PINSEL1 &= ~(0x003C0000);		// Set pins to function 0 (GPIO)
	LPC_GPIO0->FIODIR |= (0x06000000);
	LPC_GPIO0->FIOPIN = (1 << 25);
}


