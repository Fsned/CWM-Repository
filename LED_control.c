/**********************************************************************************************
 * Source file : LED_control.c
 * Author : Frederik Snedevind
 *		    Frs@Jeros.com
 *
 * Company: Jeros A/S
 *		   HTTP://www.Jeros.com/
 *
 * Date created : 11/3 - 2018 
 * Version			 : 0.1.0
 * Revised			 : 15/3 - 2018 
 **********************************************************************************************
 * Description:
 * 	This library is used to control 4 LED's on the mbed LPC1768 development board.
 *  
 *	
 *
 **********************************************************************************************
 * Dependencies:
 *	This library uses the following files:
 *	"LED_control.h"			-			Own header file, to publish API to secondary files
 *	"lpc17xx.h"					-			Processor header file, used to access various registers
 *	"stdutils.h"				-			Defines the various datatypes e.g. 8-bits integer, 16-bits integer
 * 
 **********************************************************************************************/

// ****************************************************************************************
//
//					Libraries
//
// ****************************************************************************************
#include "LED_control.h"
#include "lpc17xx.h"
#include "stdutils.h"
#include "UART_control.h"
#include "utilities.h"

#include "FreeRTOS.h"
#include "task.h"

// ****************************************************************************************
//
//					Variables
//
// ****************************************************************************************
uint8_t LED_status[4] = {0,0,0,0};
int LED_pins[4]   = {18, 20, 21, 23};

int alive_timer = 0;
int alive_ones = 0;
int alive_tens = 0;
int alive_hundreds = 0;
int alive_thousands = 0;

TaskHandle_t AliveHandle = NULL;


// ****************************************************************************************
//	Type				: 	YES_RETURN Functions
//	Example			:	yXxXxX();
//	Description	:	Returns true (1) or false (0) depending on the success of the function
// ****************************************************************************************


// ****************************************************************************************
//	Type				: 	NO_RETURN Functions
//	Example			:	nXxXxX();
//	Description	:	Does not return anything.
// ****************************************************************************************


void nLED_SET(uint8_t led_0 , uint8_t led_1, uint8_t led_2, uint8_t led_3) {
/* ******************************************************************
//	Function name : nLED_SET
//	Functionality :	Clears all LEDS and sets the given values on the 4 LEDS on the dev. board
// 	Returns				:	None.	
//  Input range		: 0:1 , 0:1 , 0:1 , 0:1
// *****************************************************************/	
	LPC_GPIO1->FIOCLR = 0x00B40000;
	led_delay_ms(1);
	
	if (led_0 != LED_DONT_CARE)
		LED_status[0] = led_0;
	
	if (led_1 != LED_DONT_CARE)
		LED_status[1] = led_1;
	
	if (led_2 != LED_DONT_CARE)
		LED_status[2] = led_2;
	
	if (led_3 != LED_DONT_CARE)
		LED_status[3] = led_3;
	
	
	LPC_GPIO1->FIOSET |= (led_0 << 18);
	led_delay_ms(1);
	LPC_GPIO1->FIOSET |= (led_1 << 20);
	led_delay_ms(1);
	LPC_GPIO1->FIOSET |= (led_2 << 21);
	led_delay_ms(1);
	LPC_GPIO1->FIOSET |= (led_3 << 23);
	led_delay_ms(1);
}
// ***** End of Function ********************************************


void nLEDFlip(uint8_t led_flip[]) {
/* ******************************************************************
//	Function name : nLEDFlip
//	Functionality :	Takes 4 ints in an array, and flips the corresponding LEDS, to the 1's in the array
// 	Returns				:	none.
//  Input range		: [1:0 , 1:0 , 1:0 , 1:0];
// *****************************************************************/	
	
	for (int i = 0; i < 4; i++) {
		if (led_flip[i]) {
			if (LED_status[i])
				LED_status[i] = 0;
			else
				LED_status[i] = 1;
		}
	}
	nLED_SET(LED_status[0], LED_status[1], LED_status[2], LED_status[3]);
}
// ***** End of Function ********************************************



void nAliveSuspend() {
/* ******************************************************************
//	Function name : nAliveSuspend
//	Functionality :	Used to suspend the Alive LED Task
// 	Returns				:	None
//  Input range		: None
// *****************************************************************/
	vTaskSuspend(AliveHandle);
}

void nAliveResume() {
/* ******************************************************************
//	Function name : nAliveResume
//	Functionality :	Used to resume the Alive LED Task
// 	Returns				:	None
//  Input range		: None
// *****************************************************************/
	vTaskResume(AliveHandle);
}
// ***** End of Function ********************************************

void tLEDAlive( void *param ) {
/* ******************************************************************
//	Function name : tLEDAlive
//	Functionality :	Used to blink the rightmost LED on the MBED LPC1768 board
// 	Returns				:	None
//  Input range		: None
// *****************************************************************/	
	uint8_t LED_status = 0;			// Used to keep track of current LED status
	
	while(1) {
		
		if (LED_status)
			LED_status = 0;
		else
			LED_status = 1;
		
		nLED_SET(LED_status,2,2,2); 
		
		alive_timer++;
		
		vTaskDelay(1000);
//			vTaskDelayUntil( &xPreviousWakeTime, ( TickType_t ) 1000 / portTICK_PERIOD_MS );
	}
}
// ***** End of Function ********************************************


void nPrintAlive() {
/* ******************************************************************
//	Function name : tLEDAlive
//	Functionality :	Used to blink the rightmost LED on the MBED LPC1768 board
// 	Returns				:	None
//  Input range		: None
// *****************************************************************/	
	uint32_t 	seconds;
	uint8_t 	minutes = 0;
	uint8_t 	hours = 0;
	
	seconds = alive_timer;

	while(seconds >= 3600) {
		hours++;
		seconds -= 3600;
	}
	
	while(seconds >= 60) {
		minutes++;
		seconds -= 60;
	}
	
	nUART_TxString("Current alive time : ");
	
	if ( hours >= 10 ) 
		nUART_TxChar(int_to_char_10(hours) + '0');
	nUART_TxChar(int_to_char_1 (hours) + '0');
	
	nUART_TxChar(':');
	
	if ( minutes >= 10 )
		nUART_TxChar(int_to_char_10(minutes) + '0');
	nUART_TxChar(int_to_char_1 (minutes) + '0');
	
	nUART_TxChar(':');
	
	if ( seconds >= 10 )
		nUART_TxChar(int_to_char_10(seconds) + '0');
	nUART_TxChar(int_to_char_1 (seconds) + '0');
	
	nNewLine( 1 );
}

// ****************************************************************************************
//	Type				: 	VALUE_RETURN Functions
//	Example			:	vXxXxX();
//	Description	:	Returns a value, either 1 or 0. no confirmation if successful or not.
// ****************************************************************************************

void led_delay_ms(unsigned int n) {
/* ******************************************************************
//	Function name : led_delay_ms
//	Functionality :	static delay used for various LED functions.
// 	Returns				:	None	
//  Input range		: 0 - ?, delay multiplier, probably roughly in milliseconds
// *****************************************************************/
	unsigned int o = n*12000*2;
	while(--o);
}

