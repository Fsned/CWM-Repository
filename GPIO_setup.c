/**********************************************************************************************
* Author : Frederik Snedevind
 *		   Frs@Jeros.com
 *
* Company: Jeros A/S
 *		   HTTP://www.Jeros.com/
 *
* Date   : 14/3 - 2018 
 * 
 **********************************************************************************************
* Description:
 * GPIO_setup.c is used with NXP LPC1768 Processor, to setup
 *  pins to GPIO function, as well as read/write from/to
 *	them.
 *
 * 
 **********************************************************************************************
* Dependencies:
*	This library uses the following files:
 *	"lpc17xx-h"		-		Contains information for Pin manipulation
 *	"GPIO_setup.h"	-		Associated header file
 *	"stdutils.h"	-		Contains typedef for among many, 'uint8_t' to limit memory usage
 * 
 **********************************************************************************************/

#include "lpc17xx.h"
#include "GPIO_setup.h"

uint8_t PinLibrary[5][32];

uint8_t PinLibrary_Initialized = 0;


// ********************************************
//		Function nGPIOSetup() is not necessary. can be used
//		to quickly set up several GPIO ports
// 			
//
//		
// ********************************************

void nGPIOSetup() {
	
	if (!PinLibrary_Initialized)
		nInitializePinLibrary();
	
	// Setup Pins for LEDS 1:4
	ySetupDigitalO( PORT_1 , LED_1 );
	ySetupDigitalO( PORT_1 , LED_2 );
	ySetupDigitalO( PORT_1 , LED_3 );
	ySetupDigitalO( PORT_1 , LED_4 );

	// Setup GPIO_OUTPUT pins for measuring purposes
	ySetupDigitalO( PORT_0 , PIN_25 );
	ySetupDigitalO( PORT_0 , PIN_26 );

	// Setup 1 GPIO_INPUT pin, to receive signal from wavegen
	ySetupDigitalI( PORT_0 , PIN_1 );
}

// ********************************************
//		Set pin to digital [O]utput
//		Return: True (1) or False (0),
// 			depending on successful setup or no
//
//		GPIO_INPUT: Port_0 : Port_4, Pin_0 : Pin_31
// ********************************************
uint8_t ySetupDigitalO ( uint8_t Port, int Pin ) {
	
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
	
	return SetupDigitalO_ret;
}
// ***********************************************************************************************************************************************************




// ********************************************
//		Set pin to digital [I]nput
//		Return: True (1) or False (0),
// 			depending on successful setup or no
//
//		GPIO_INPUT: Port_0 : Port_4, Pin_0 : Pin_31
// ********************************************
uint8_t ySetupDigitalI ( uint8_t Port, int Pin) {
	
	if (!PinLibrary_Initialized)
		nInitializePinLibrary();
	
	uint8_t SetupDigitalI_ret = 0;
	
	if (PinLibrary[Port][Pin] == GPIO_FREE) {
		switch (Port) {
			case PORT_0:
				LPC_GPIO0->FIODIR &= ~(1 << Pin);
				if (Pin >= PIN_0 && Pin <= PIN_15) {
					LPC_PINCON->PINSEL0 &= ~(0b11 << Pin*2);
					PinLibrary[Port][Pin] = GPIO_INPUT;
					SetupDigitalI_ret = 1;
				}
				else if (Pin >= PIN_16 && Pin <= PIN_31) {
					LPC_PINCON->PINSEL1 &= ~(0b11 << (Pin-16)*2);
					PinLibrary[Port][Pin] = GPIO_INPUT;
					SetupDigitalI_ret = 1;
				}
				else
					SetupDigitalI_ret = 0;
				break;
			
			case PORT_1:
				LPC_GPIO1->FIODIR &= ~(1 << Pin);
				if (Pin >= PIN_0 && Pin <= PIN_15) {
					LPC_PINCON->PINSEL2 &= ~(0b11 << Pin*2);
					PinLibrary[Port][Pin] = GPIO_INPUT;
					SetupDigitalI_ret = 1;
				}
				else if (Pin >= PIN_16 && Pin <= PIN_31) {
					LPC_PINCON->PINSEL3 &= ~(0b11 << (Pin-16)*2);
					PinLibrary[Port][Pin] = GPIO_INPUT;
					SetupDigitalI_ret = 1;
				}
				else
					SetupDigitalI_ret = 0;
				break;
			
			case PORT_2:
				LPC_GPIO2->FIODIR &= ~(1 << Pin);
				if (Pin >= PIN_0 && Pin <= PIN_15) {
					LPC_PINCON->PINSEL4 &= ~(0b11 << Pin*2);
					PinLibrary[Port][Pin] = GPIO_INPUT;
					SetupDigitalI_ret = 1;
				}
				else if (Pin >= PIN_16 && Pin <= PIN_31) {
					LPC_PINCON->PINSEL5 &= ~(0b11 << (Pin-16)*2);
					PinLibrary[Port][Pin] = GPIO_INPUT;
					SetupDigitalI_ret = 1;
				}
				else
					SetupDigitalI_ret = 0;
				break;
			
			case PORT_3:
				LPC_GPIO3->FIODIR &= ~(1 << Pin);
				if (Pin >= PIN_0 && Pin <= PIN_15) {
					LPC_PINCON->PINSEL6 &= ~(0b11 << Pin*2);
					PinLibrary[Port][Pin] = GPIO_INPUT;
					SetupDigitalI_ret = 1;
				}
				else if (Pin >= PIN_16 && Pin <= PIN_31) {
					LPC_PINCON->PINSEL7 &= ~(0b11 << (Pin-16)*2);
					PinLibrary[Port][Pin] = GPIO_INPUT;
					SetupDigitalI_ret = 1;
				}
				else
					SetupDigitalI_ret = 0;
				break;
			
			case PORT_4:
				LPC_GPIO4->FIODIR &= ~(1 << Pin);
				if (Pin >= PIN_0 && Pin <= PIN_15) {
					LPC_PINCON->PINSEL8 &= ~(0b11 << Pin*2);
					PinLibrary[Port][Pin] = GPIO_INPUT;
					SetupDigitalI_ret = 1;
				}
				else if (Pin >= PIN_16 && Pin <= PIN_31) {
					LPC_PINCON->PINSEL9 &= ~(0b11 << (Pin-16)*2);
					PinLibrary[Port][Pin] = GPIO_INPUT;
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
	
	return SetupDigitalI_ret;
}
// ***********************************************************************************************************************************************************



// ********************************************
//		Set digital GPIO_OUTPUT High
//		Return: True (1) or False (0),
// 			depending on successful operation or no
//
//		GPIO_INPUT: Port_0 : Port_4 , Pin_0 : Pin_31
// ********************************************
int ySetDigitalHigh(uint8_t Port , int Pin) {
	
	if (!PinLibrary_Initialized)
		nInitializePinLibrary();
	
	uint8_t SetDigitalHigh_ret = 0;
	
	if (PinLibrary[Port][Pin] == GPIO_OUTPUT) {
		switch (Port) {
			case PORT_0:
				LPC_GPIO0->FIOSET |= (1 << Pin);
				SetDigitalHigh_ret = 1;
				break;
			
			case PORT_1:
				LPC_GPIO1->FIOSET |= (1 << Pin);
				SetDigitalHigh_ret = 1;
				break;
			
			case PORT_2:
				LPC_GPIO2->FIOSET |= (1 << Pin);
				SetDigitalHigh_ret = 1;
				break;
			
			case PORT_3:
				LPC_GPIO3->FIOSET |= (1 << Pin);
				SetDigitalHigh_ret = 1;
				break;
			
			case PORT_4:
				LPC_GPIO4->FIOSET |= (1 << Pin);
				SetDigitalHigh_ret = 1;
				break;
			
			default:
				SetDigitalHigh_ret = 0;
				break;
		}
	}
	else
		SetDigitalHigh_ret = 0;
	
	return SetDigitalHigh_ret;
}
// ***********************************************************************************************************************************************************


// ********************************************
//		Set digital GPIO_OUTPUT High
//		Return: True (1) or False (0),
// 			depending on successful operation or no
//
//		GPIO_INPUT: Port_0 : Port_4 , Pin_0 : Pin_31
// ********************************************
uint8_t ySetDigitalLow(uint8_t Port , int Pin) {
	
	if (!PinLibrary_Initialized)
		nInitializePinLibrary();
	
	uint8_t SetDigitalLow_ret = 0;
	
	if (PinLibrary[Port][Pin] == GPIO_OUTPUT) {
		switch (Port) {
			case PORT_0:
				LPC_GPIO0->FIOSET |= (1 << Pin);
				SetDigitalLow_ret = 1;
				break;
			
			case PORT_1:
				LPC_GPIO1->FIOSET |= (1 << Pin);
				SetDigitalLow_ret = 1;
				break;
			
			case PORT_2:
				LPC_GPIO2->FIOSET |= (1 << Pin);
				SetDigitalLow_ret = 1;
				break;
			
			case PORT_3:
				LPC_GPIO3->FIOSET |= (1 << Pin);
				SetDigitalLow_ret = 1;
				break;
			
			case PORT_4:
				LPC_GPIO4->FIOSET |= (1 << Pin);
				SetDigitalLow_ret = 1;
				break;
			
			default:
				SetDigitalLow_ret = 0;
				break;
		}
	}
	else
		SetDigitalLow_ret = 0;
	return SetDigitalLow_ret;
}
// ***********************************************************************************************************************************************************



// ********************************************
//		Read GPIO_INPUT on a pin. returns value pin value (Digital)
//		Return: True (1) or False (0),
//
//
//		GPIO_INPUT: Port_0 : Port_4 , Pin_0 : Pin_31
// ********************************************
uint8_t vReadDigitalInput(uint8_t Port , int Pin) {
	
	if (!PinLibrary_Initialized)
		nInitializePinLibrary();
	
	uint8_t ReadDigitalGPIO_INPUT_ret;
	
	if (PinLibrary[Port][Pin] == GPIO_INPUT) {
		switch(Port) {
			case PORT_0:
				ReadDigitalGPIO_INPUT_ret = (LPC_GPIO0->FIOPIN>>Pin) & 0x01;
				break;
			
			case PORT_1:
				ReadDigitalGPIO_INPUT_ret = (LPC_GPIO1->FIOPIN>>Pin) & 0x01;
				break;
			
			case PORT_2:
				ReadDigitalGPIO_INPUT_ret = (LPC_GPIO2->FIOPIN>>Pin) & 0x01;
				break;
			
			case PORT_3:
				ReadDigitalGPIO_INPUT_ret = (LPC_GPIO3->FIOPIN>>Pin) & 0x01;
				break;
			
			case PORT_4:
				ReadDigitalGPIO_INPUT_ret = (LPC_GPIO4->FIOPIN>>Pin) & 0x01;
				break;
			
			default: 
				break;
		}
	}
	else
		ReadDigitalGPIO_INPUT_ret = 0xFF;
	
	return ReadDigitalGPIO_INPUT_ret;
}


// ********************************************
//		Initialize Pin Library first time GPIO
//			functions are used
// 		Return: none.
//
//		GPIO_INPUT: Called hands-off
// ********************************************
void nInitializePinLibrary() {
	for (int i = 0; i < 5; i++) {
		for (int n = 0; n < 32; n++)
			PinLibrary[i][n] = GPIO_FREE;
	}
	PinLibrary_Initialized = 1;
}
// ***********************************************************************************************************************************************************
