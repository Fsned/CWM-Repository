/**********************************************************************************************
 * Header file : LED_control.h
 * Author : Frederik Snedevind
 *		    Frs@Jeros.com
 *
 * Company: Jeros A/S
 *		   HTTP://www.Jeros.com/
 *
 * Date created  : 11/3 - 2018 
 * Version			 : 0.1.0
 * Revised			 : 15/3 - 2018 
 **********************************************************************************************
 * Description:
 * 	Header file for LED_control.c, providing LED API to other files
 *  
 *	
 *
 *********************************************************************************************/
 #include "stdutils.h"
 
// ****************************************************************************************
//
//					Constants
//
// ****************************************************************************************
#define		LED_DONT_CARE		2 


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
extern void nLEDSetup	( void );

extern void nLED_SET	( uint8_t led_0 , uint8_t led_1, uint8_t led_2, uint8_t led_3 );

extern void nDiscoFunc( void );

extern void nLEDFlip	( unsigned int led_flip[] );

extern void nDelayLED	( void );

// ****************************************************************************************
//	Type		: 	VALUE_RETURN Functions
//	Example		:	vXxXxX();
//	Description	:	Returns a value, either 1 or 0. no confirmation if successful or not.
// ****************************************************************************************
extern void led_delay_ms(unsigned int n);

extern void tLEDAlive( void* );



