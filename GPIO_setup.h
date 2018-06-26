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

#define SBIT_BURST      16u
#define SBIT_START      24u
#define SBIT_PDN        21u
#define SBIT_EDGE       27u 
#define SBIT_DONE       31u
#define SBIT_RESULT     4u
#define SBIT_CLCKDIV    8u


enum MbedPorts {
	PORT_0 = 0,
	PORT_1 = 1,
	PORT_2 = 2,
	PORT_3 = 3,
	PORT_4 = 4
};

enum MbedPins {
	PIN_0,
	PIN_1,
	PIN_2,
	PIN_3,
	PIN_4,
	PIN_5,
	PIN_6,
	PIN_7,
	PIN_8,
	PIN_9,
	PIN_10,
	PIN_11,
	PIN_12,
	PIN_13,
	PIN_14,
	PIN_15,
	PIN_16,
	PIN_17,
	PIN_18,
	PIN_19,
	PIN_20,
	PIN_21,
	PIN_22,
	PIN_23,
	PIN_24,
	PIN_25,
	PIN_26,
	PIN_27,
	PIN_28,
	PIN_29,
	PIN_30,
	PIN_31
};

// PINS for the 4 LEDS on MBED LPC1768 Board
enum MbedLEDPins {
	LED_1 = 18,
	LED_2 = 20,
	LED_3 = 21,
	LED_4 = 23,
	
	PORT_LED = PORT_1
};

/* Possible values for pins in the pinlibrary */
enum PinLIbraryValues {
	GPIO_FREE 	= 0,
	GPIO_INPUT 	= 1,
	GPIO_OUTPUT = 2
};

/* Possible values of GPIO when reading from them, or writing to them */
typedef enum GPIOBounds {
	GPIO_LOW 	= 0,
	GPIO_HIGH = 1
} GPIOPIN;



// ****************************************************************************************
//	Type		: 	BINARY_RETURN Functions
//	Example		:	bXxXxX();
//	Description	:	Returns true (1) or false (0) depending on the success of the function
// ****************************************************************************************
uint8_t bSetupDigitalIO			( uint8_t Port , int Pin , uint8_t InputOutput );

uint8_t bSetupAnalogI			(uint8_t ADCNumber);

uint8_t bDigitalWrite			( uint8_t Port , int Pin , uint8_t State );

// ****************************************************************************************
//	Type		: 	NO_RETURN Functions
//	Example		:	nXxXxX();
//	Description	:	Does not return anything.
// ****************************************************************************************
void nGPIOSetup					( void );

void nInitializePinLibrary		( void );

extern void nGPIO_STOP			( void );
// ****************************************************************************************
//	Type		: 	VALUE_RETURN Functions
//	Example		:	vXxXxX();
//	Description	:	Returns a value, either 1 or 0. no confirmation if successful or not.
// ****************************************************************************************
extern uint8_t vDigitalRead		( uint8_t Port , int Pin ); 

extern uint16_t vAnalogRead		( uint8_t ADCNumber );

extern uint8_t vGetPinStatus	( uint8_t Port , int Pin );

#endif /* _GPIO_SETUP_H */ 
