/**********************************************************************************************
 * Header file : GPIO_setup.h
 * Author : Frederik Snedevind
 *		    Frs@Jeros.com
 *
 * Company: Jeros A/S
 *		   HTTP://www.Jeros.com/
 *
 * Date created : 14/3 - 2018 
 **********************************************************************************************
 * Description:
 * Header file belonging to the source file GPIO_setup.h
 *  to provide an API for GPIO manipulation, setup and configuration
 * For further documentation check the corresponding .c-file	
 *
 *********************************************************************************************/

// ****************************************************************************************
//
//					Constants
//
// ****************************************************************************************


#ifndef _GPIO_SETUP_H
#define _GPIO_SETUP_H





enum MbedPorts {
	PORT_0 = 0,
	PORT_1 = 1,
	PORT_2 = 2,
	PORT_3 = 3,
	PORT_4 = 4
};

enum MbedPins {
	PIN_0 = 0,
	PIN_1 = 1,
	PIN_2 = 2,
	PIN_3 = 3,
	PIN_4 = 4,
	PIN_5 = 5,
	PIN_6 = 6,
	PIN_7 = 7,
	PIN_8 = 8,
	PIN_9 = 9,
	PIN_10 = 10,
	PIN_11 = 11,
	PIN_12 = 12,
	PIN_13 = 13,
	PIN_14 = 14,
	PIN_15 = 15,
	PIN_16 = 16,
	PIN_17 = 17,
	PIN_18 = 18,
	PIN_19 = 19,
	PIN_20 = 20,
	PIN_21 = 21,
	PIN_22 = 22,
	PIN_23 = 23,
	PIN_24 = 24,
	PIN_25 = 25,
	PIN_26 = 26,
	PIN_27 = 27,
	PIN_28 = 28,
	PIN_29 = 29,
	PIN_30 = 30,
	PIN_31 = 31
};

// PINS for the 4 LEDS on MBED LPC1768 Board
enum MbedLEDPins {
	LED_1 = 18,
	LED_2 = 20,
	LED_3 = 21,
	LED_4 = 23,
	PORT_LED = PORT_1
};


enum PinLIbraryValues {
	GPIO_FREE 	= 0,
	GPIO_INPUT 	= 1,
	GPIO_OUTPUT = 2
};

typedef enum GPIOBounds {
	GPIO_LOW 	= 0,
	GPIO_HIGH = 1
} GPIOPIN;

//#define PORT_0				0
//#define PORT_1				1
//#define PORT_2				2
//#define	PORT_3				3
//#define PORT_4				4

//#define PIN_0					0
//#define PIN_1					1
//#define PIN_2					2
//#define PIN_3					3
//#define PIN_4					4
//#define PIN_5					5
//#define PIN_6					6
//#define PIN_7					7
//#define PIN_8					8
//#define PIN_9					9
//#define PIN_10				10
//#define PIN_11				11
//#define PIN_12				12
//#define PIN_13				13
//#define PIN_14				14
//#define PIN_15				15
//#define PIN_16				16
//#define PIN_17				17
//#define PIN_18				18
//#define PIN_19				19
//#define PIN_20				20
//#define PIN_21				21 
//#define PIN_22				22
//#define PIN_23				23
//#define PIN_24				24
//#define PIN_25				25
//#define PIN_26				26
//#define PIN_27				27
//#define PIN_28				28
//#define PIN_29				29
//#define PIN_30				30
//#define PIN_31				31

//#define LED_1					18
//#define LED_2					20
//#define LED_3					21
//#define LED_4					23

//// Different values of PinLibrary
//#define GPIO_FREE			0
//#define GPIO_INPUT		1
//#define GPIO_OUTPUT		2

//// High or Low setting of digital pin ( I/O )
//#define GPIO_LOW			0 
//#define	GPIO_HIGH			1 



// ****************************************************************************************
//	Type		: 	YES_RETURN Functions
//	Example		:	yXxXxX();
//	Description	:	Returns true (1) or false (0) depending on the success of the function
// ****************************************************************************************
uint8_t ySetupDigitalIO   ( uint8_t Port , int Pin , uint8_t InputOutput ); 

uint8_t yDigitalWrite			( uint8_t Port , int Pin , uint8_t State );

// ****************************************************************************************
//	Type		: 	NO_RETURN Functions
//	Example		:	nXxXxX();
//	Description	:	Does not return anything.
// ****************************************************************************************
void nGPIOSetup								( void );

void nInitializePinLibrary		( void );

extern void nGPIO_STOP				( void );
// ****************************************************************************************
//	Type		: 	VALUE_RETURN Functions
//	Example		:	vXxXxX();
//	Description	:	Returns a value, either 1 or 0. no confirmation if successful or not.
// ****************************************************************************************
extern uint8_t vDigitalRead		( uint8_t Port , int Pin ); 

extern uint8_t vGetPinStatus	( uint8_t Port , int Pin );

#endif /* _GPIO_SETUP_H */ 
