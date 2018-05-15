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
 * For further documentation please check the corresponding .c-file	
 *
 *********************************************************************************************/
 #ifndef LED_CONTROL_H
 #define LED_CONTOL_H
 
 
 #include "stdutils.h"
 #include "FreeRTOS.h"
 #include "task.h"
// #include "queue.h"
 
// ****************************************************************************************
//
//					Constants
//
// ****************************************************************************************
#define		LED_DONT_CARE		2 

extern int alive_ones;
extern int alive_tens;
extern int alive_hundreds;
extern int alive_thousands;

extern TaskHandle_t AliveHandle;

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
extern void nLED_SET	( uint8_t led_0 , uint8_t led_1, uint8_t led_2, uint8_t led_3 );

extern void nLEDFlip	( uint8_t led_flip[] );

extern void nPrintAlive( void );

extern void nAliveSuspend ( void );

extern void nAliveResume  ( void );
// ****************************************************************************************
//	Type		: 	VALUE_RETURN Functions
//	Example		:	vXxXxX();
//	Description	:	Returns a value, either 1 or 0. no confirmation if successful or not.
// ****************************************************************************************
extern void led_delay_ms(unsigned int n);

extern void tLEDAlive( void* );


#endif /* LED_CONTROL_H */
