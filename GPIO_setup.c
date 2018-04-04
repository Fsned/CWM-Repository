/**********************************************************************************************
 * Author : Frederik Snedevind
 *		   Frs@Jeros.com
 *
 * Company: Jeros A/S
 *		   HTTP://www.Jeros.com/
 *
 * Date created : 14/3 - 2018 
 * Version			 : 0.0.1
 * Revised			 : 15/3 - 2018 
 **********************************************************************************************
 * Description:
 * GPIO_setup.c is used with NXP LPC1768 Processor, to setup
 *  pins to GPIO function, as well as read/write from/to
 *	them.
 *
 **********************************************************************************************
 * Dependencies:
 *	This library uses the following files:
 *	"lpc17xx-h"			-		Contains information for Pin manipulation
 *	"GPIO_setup.h"	-		Associated header file
 *	"stdutils.h"		-		Contains typedef for among many, 'uint8_t' to limit memory usage
 * 
 **********************************************************************************************/

// ****************************************************************************************
//
//					Libraries
//
// ****************************************************************************************
#include "lpc17xx.h"
#include "GPIO_setup.h"


// ****************************************************************************************
//
//					Variables
//
// ****************************************************************************************

// Array to contain the setup status for 32 pins on each port, (Port 0 - Port 4). 
// Will be initialized to "GPIO_FREE" = 0, once first function in file is called
uint8_t PinLibrary[5][32];

// Variable to keep track of whether PinLibrary has been initialized or no. will
// Be set to 1 once it has been initialized, preventing further initializations
uint8_t PinLibrary_Initialized = 0;


// ****************************************************************************************
//	Type		: 	YES_RETURN Functions
//	Example		:	yX();
//	Description	:	Returns true (1) or false (0) depending on the success of the function
// ****************************************************************************************


uint8_t ySetupDigitalO ( uint8_t Port, int Pin ) {
/* ******************************************************************
//	Function name : ySetupDigitalO
//	Functionality :	Setup a GPIO pin to digital output
// 	Returns				:	True (1) or false (0)
//  Input range		: PORT_0 : PORT_4  ,  PIN_0 : PIN_31
//		
// *****************************************************************/
	
	if (!PinLibrary_Initialized)
		nInitializePinLibrary();
	
	uint8_t SetupDigitalO_ret = 0;
	
	if (PinLibrary[Port][Pin] == GPIO_FREE) {
		switch (Port) {
			case PORT_0:
				LPC_GPIO0->FIODIR |= (1 << Pin);
				if (Pin >= PIN_0 && Pin <= PIN_15) {
					LPC_PINCON->PINSEL0 &= ~(0b11 << Pin*2);
					SetupDigitalO_ret = 1;
				}
				else if (Pin >= PIN_16 && Pin <= PIN_31) {
					LPC_PINCON->PINSEL1 &= ~(0b11 << (Pin-16)*2);
					SetupDigitalO_ret = 1;
				}
				else
					SetupDigitalO_ret = 0;
				break;
			
			case PORT_1:
				LPC_GPIO1->FIODIR |= (1 << Pin);
				if (Pin >= PIN_0 && Pin <= PIN_15) {
					LPC_PINCON->PINSEL2 &= ~(0b11 << Pin*2);
					SetupDigitalO_ret = 1;
				}
				else if (Pin >= PIN_16 && Pin <= PIN_31) {
					LPC_PINCON->PINSEL3 &= ~(0b11 << (Pin-16)*2);
					SetupDigitalO_ret = 1;
				}
				else
					SetupDigitalO_ret = 0;
				break;
			
			case PORT_2:
				LPC_GPIO2->FIODIR |= (1 << Pin);
				if (Pin >= PIN_0 && Pin <= PIN_15) {
					LPC_PINCON->PINSEL4 &= ~(0b11 << Pin*2);
					SetupDigitalO_ret = 1;
				}
				else if (Pin >= PIN_16 && Pin <= PIN_31) {
					LPC_PINCON->PINSEL5 &= ~(0b11 << (Pin-16)*2);
					SetupDigitalO_ret = 1;
				}
				else
					SetupDigitalO_ret = 0;
				break;
			
			case PORT_3:
				LPC_GPIO3->FIODIR |= (1 << Pin);
				if (Pin >= PIN_0 && Pin <= PIN_15) {
					LPC_PINCON->PINSEL6 &= ~(0b11 << Pin*2);
					SetupDigitalO_ret = 1;
				}
				else if (Pin >= PIN_16 && Pin <= PIN_31) {
					LPC_PINCON->PINSEL7 &= ~(0b11 << (Pin-16)*2);
					SetupDigitalO_ret = 1;
				}
				else
					SetupDigitalO_ret = 0;
				break;
			
			case PORT_4:
				LPC_GPIO4->FIODIR |= (1 << Pin);
				if (Pin >= PIN_0 && Pin <= PIN_15) {
					LPC_PINCON->PINSEL8 &= ~(0b11 << Pin*2);
					SetupDigitalO_ret = 1;
				}
				else if (Pin >= PIN_16 && Pin <= PIN_31) {
					LPC_PINCON->PINSEL9 &= ~(0b11 << (Pin-16)*2);
					SetupDigitalO_ret = 1;
				}
				else
					SetupDigitalO_ret = 0;
				break;
			
			default :
				SetupDigitalO_ret = 0;
		}
	}
	else
		SetupDigitalO_ret = 0;
	
	if (SetupDigitalO_ret)
		PinLibrary[Port][Pin] = GPIO_OUTPUT;
	
	return SetupDigitalO_ret;
}
// ***** End of Function ********************************************

uint8_t ySetupDigitalI ( uint8_t Port, int Pin) {
/* ******************************************************************
//	Function name : ySetupDigitalI
//	Functionality :	Setup a GPIO pin to digital Input
// 	Returns				:	True (1) or false (0)
//  Input range		: PORT_0 : PORT_4  ,  PIN_0 : PIN_31
//		
// *****************************************************************/
	
	if (!PinLibrary_Initialized)
		nInitializePinLibrary();
	
	uint8_t SetupDigitalI_ret = 0;
	
	if (PinLibrary[Port][Pin] == GPIO_FREE) {
		switch (Port) {
			case PORT_0:
				LPC_GPIO0->FIODIR &= ~(1 << Pin);
				if (Pin >= PIN_0 && Pin <= PIN_15) {
					LPC_PINCON->PINSEL0 &= ~(0b11 << Pin*2);
					SetupDigitalI_ret = 1;
				}
				else if (Pin >= PIN_16 && Pin <= PIN_31) {
					LPC_PINCON->PINSEL1 &= ~(0b11 << (Pin-16)*2);
					SetupDigitalI_ret = 1;
				}
				else
					SetupDigitalI_ret = 0;
				break;
			
			case PORT_1:
				LPC_GPIO1->FIODIR &= ~(1 << Pin);
				if (Pin >= PIN_0 && Pin <= PIN_15) {
					LPC_PINCON->PINSEL2 &= ~(0b11 << Pin*2);
					SetupDigitalI_ret = 1;
				}
				else if (Pin >= PIN_16 && Pin <= PIN_31) {
					LPC_PINCON->PINSEL3 &= ~(0b11 << (Pin-16)*2);
					SetupDigitalI_ret = 1;
				}
				else
					SetupDigitalI_ret = 0;
				break;
			
			case PORT_2:
				LPC_GPIO2->FIODIR &= ~(1 << Pin);
				if (Pin >= PIN_0 && Pin <= PIN_15) {
					LPC_PINCON->PINSEL4 &= ~(0b11 << Pin*2);
					SetupDigitalI_ret = 1;
				}
				else if (Pin >= PIN_16 && Pin <= PIN_31) {
					LPC_PINCON->PINSEL5 &= ~(0b11 << (Pin-16)*2);
					SetupDigitalI_ret = 1;
				}
				else
					SetupDigitalI_ret = 0;
				break;
			
			case PORT_3:
				LPC_GPIO3->FIODIR &= ~(1 << Pin);
				if (Pin >= PIN_0 && Pin <= PIN_15) {
					LPC_PINCON->PINSEL6 &= ~(0b11 << Pin*2);
					SetupDigitalI_ret = 1;
				}
				else if (Pin >= PIN_16 && Pin <= PIN_31) {
					LPC_PINCON->PINSEL7 &= ~(0b11 << (Pin-16)*2);
					SetupDigitalI_ret = 1;
				}
				else
					SetupDigitalI_ret = 0;
				break;
			
			case PORT_4:
				LPC_GPIO4->FIODIR &= ~(1 << Pin);
				if (Pin >= PIN_0 && Pin <= PIN_15) {
					LPC_PINCON->PINSEL8 &= ~(0b11 << Pin*2);
					SetupDigitalI_ret = 1;
				}
				else if (Pin >= PIN_16 && Pin <= PIN_31) {
					LPC_PINCON->PINSEL9 &= ~(0b11 << (Pin-16)*2);
					SetupDigitalI_ret = 1;
				}
				else
					SetupDigitalI_ret = 0;
				break;
			
			default :
				SetupDigitalI_ret = 0;
		}
	}
	else
		SetupDigitalI_ret = 0;
	
	if (SetupDigitalI_ret)
		PinLibrary[Port][Pin] = GPIO_INPUT;
	
	return SetupDigitalI_ret;
}
// ***** End of Function ********************************************

uint8_t yDigitalWrite( uint8_t Port , int Pin , uint8_t State ) {
/* ******************************************************************
//	Function name : yDigitalWrite
//	Functionality :	Write a value to a digital output
// 	Returns				:	True (1) or false (0), depending on successful operation or no
//  Input range		: PORT_0 : PORT_4  ,  PIN_0 : PIN_31  ,  0 : 1
//		
// *****************************************************************/
	if (! PinLibrary_Initialized)
		nInitializePinLibrary();
	
	uint8_t yDigitalWrite_ret = 0;
	
	if (PinLibrary[Port][Pin] == GPIO_OUTPUT) {
		switch(State) {
			case GPIO_HIGH:
				switch(Port) {
					case PORT_0:
						LPC_GPIO0->FIOPIN |= (State << Pin);
						yDigitalWrite_ret = 1;
					break;
			
					case PORT_1:
						LPC_GPIO1->FIOPIN |= (State << Pin);
						yDigitalWrite_ret = 1;
					break;
			
					case PORT_2:
						LPC_GPIO2->FIOPIN |= (State << Pin);
						yDigitalWrite_ret = 1;
					break;
			
					case PORT_3:
						LPC_GPIO3->FIOPIN |= (State << Pin);
						yDigitalWrite_ret = 1;
					break;
			
					case PORT_4:
						LPC_GPIO4->FIOPIN |= (State << Pin);
						yDigitalWrite_ret = 1;
					break;
			
					default: 
						yDigitalWrite_ret = 0;
					break;
				}
				break;
				
			case GPIO_LOW:
				switch(Port) {
					case PORT_0:
						LPC_GPIO0->FIOPIN &= ~(1 << Pin);
						yDigitalWrite_ret = 1;
					break;
			
					case PORT_1:
						LPC_GPIO1->FIOPIN &= ~(1 << Pin);
						yDigitalWrite_ret = 1;
					break;
			
					case PORT_2:
						LPC_GPIO2->FIOPIN &= ~(1 << Pin);
						yDigitalWrite_ret = 1;
					break;
			
					case PORT_3:
						LPC_GPIO3->FIOPIN &= ~(1 << Pin);
						yDigitalWrite_ret = 1;
					break;
			
					case PORT_4:
						LPC_GPIO4->FIOPIN &= ~(1 << Pin);
						yDigitalWrite_ret = 1;
					break;
			
					default:
						yDigitalWrite_ret = 0;
					break;
				}
				break;
			
			default:
				yDigitalWrite_ret = 0;
				break;
		}
	}
	else
		yDigitalWrite_ret = 0;
	
	return yDigitalWrite_ret;
}
// ***** End of Function ********************************************



// ****************************************************************************************
//	Type		: 	NO_RETURN Functions
//	Example		:	nX();
//	Description	:	Does not return anything.
// ****************************************************************************************


void nGPIOSetup() {
/* ******************************************************************
//	Function name : nGPIOSetup
//	Functionality :	Used to quickly setup several pins, can be used at startup
// 	Returns				:	Nothing
//  Input range		: None, Static coding
//		
// *****************************************************************/	
	if (! PinLibrary_Initialized)
		nInitializePinLibrary();
	
	// Setup Pins for LEDS 1:4
	ySetupDigitalO( PORT_1 , LED_1 );
	ySetupDigitalO( PORT_1 , LED_2 );
	ySetupDigitalO( PORT_1 , LED_3 );
	ySetupDigitalO( PORT_1 , LED_4 );
	
	// Setup 1 GPIO_INPUT pin, to receive signal from wavegen
	ySetupDigitalI( PORT_0 , PIN_26 );
}



// ***** End of Function ********************************************

void nInitializePinLibrary() {
/* ******************************************************************
//	Function name : nInitializePinLibrary
//	Functionality :	Used to set all pins to GPIO_FREE, to allow setups to reserve pins, and prevent overlapping on pins
// 	Returns				:	Nothing
//  Input range		: None
//		
// *****************************************************************/		
	for (int column = 0; column < 5; column++) {
		for (int row = 0; row < 32; row++)
			PinLibrary[column][row] = GPIO_FREE;
	}
	PinLibrary_Initialized = 1;
}
// ***** End of Function ********************************************



// ****************************************************************************************
//	Type		: 	VALUE_RETURN Functions
//	Example		:	vX();
//	Description	:	Returns a value, either 1 or 0. no confirmation if successful or not.
// ****************************************************************************************


uint8_t vDigitalRead( uint8_t Port , int Pin ) {
/* ******************************************************************
//	Function name : vReadDigitalInput
//	Functionality :	Read a digital input from a pin, and return it
// 	Returns				:	A value 0 or 1, depending on status of pin
//  Input range		: PORT_0 : PORT_4  ,  PIN_0 : PIN_31
//		
// *****************************************************************/
	if (! PinLibrary_Initialized)
		nInitializePinLibrary();
	
	uint8_t vDigitalRead_ret;
	
	if (PinLibrary[Port][Pin] == GPIO_INPUT) {
		switch(Port) {
			case PORT_0:
				vDigitalRead_ret = (LPC_GPIO0->FIOPIN>>Pin) & 0x01;
				break;
			
			case PORT_1:
				vDigitalRead_ret = (LPC_GPIO1->FIOPIN>>Pin) & 0x01;
				break;
			
			case PORT_2:
				vDigitalRead_ret = (LPC_GPIO2->FIOPIN>>Pin) & 0x01;
				break;
			
			case PORT_3:
				vDigitalRead_ret = (LPC_GPIO3->FIOPIN>>Pin) & 0x01;
				break;
			
			case PORT_4:
				vDigitalRead_ret = (LPC_GPIO4->FIOPIN>>Pin) & 0x01;
				break;
			
			default: 
				break;
		}
	}
	else
		vDigitalRead_ret = 0xFF;
	
	return vDigitalRead_ret;
}
// ***** End of Function ********************************************
