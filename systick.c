/**********************************************************************************************
* Source file : systick.c
* Author : Frederik Snedevind
*		    Frs@Jeros.com
*
* Company: Jeros A/S
*		   HTTP://www.Jeros.com/
*
* Date created : 14/3 - 2018 
* Version			 : 0.1.0
* Revised			 : 15/3 - 2018
**********************************************************************************************
* Description: 
* 	File is used to setup the systick timer, and manage it as well.
*  
*	
*
**********************************************************************************************
* Dependencies:
*	This library uses the following files:
*	"systick.h"				-				Own header file, used to present functions to other sources.
*	"lpc17xx.h"				-				Processor header file, used to access various pins, register, etc.
*	
* 
**********************************************************************************************/


// ****************************************************************************************
//
//					Libraries
//
// ****************************************************************************************
#include "systick.h"
#include "lpc17xx.h"


// ****************************************************************************************
//
//					Variables
//
// ****************************************************************************************


// ****************************************************************************************
//	Type		: 	YES_RETURN Functions
//	Example		:	yXxXxX();
//	Description	:	Returns true (1) or false (0) depending on the success of the function
// ****************************************************************************************


// ****************************************************************************************
//	Type		: 	NO_RETURN Functions
//	Example		:	nXxXxX();
//	Description	:	Does not return anything.
// ****************************************************************************************

void nSystickSetup(int frequency) {
/* ******************************************************************
//	Function name : nSystickSetup
//	Functionality :	Setup systick timer to a chosen frequency (inprecise below 1kHz, & above 5MHz)
// 	Returns				:	nothing. Calls function "SysTick_Handler()" with chosen frequency
//  Input range		: 0 - SystemCoreClock
// *****************************************************************/	
	
	STRELOAD = ( 100000000 / ((frequency*2)*1.0425) );
	
	STCTRL = (1 << SBIT_ENABLE) | (1 << SBIT_TICKINT) | (1 << SBIT_CLKSOURCE);
	
	LPC_PINCON->PINSEL1 &= ~(0x003C0000);		// Set pins to function 0 (GPIO)
	LPC_GPIO0->FIODIR |= (0x06000000);
	LPC_GPIO0->FIOPIN = (1 << 25);
}


// ****************************************************************************************
//	Type		: 	VALUE_RETURN Functions
//	Example		:	vXxXxX();
//	Description	:	Returns a value, either 1 or 0. no confirmation if successful or not.
// ****************************************************************************************









