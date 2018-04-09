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

#include "FreeRTOS.h"
#include "task.h"

// ****************************************************************************************
//
//					Variables
//
// ****************************************************************************************
int disco_arr[20][4] = {{0,0,0,0},{0,1,1,0},{1,0,0,1},{0,1,1,0},{1,1,1,1},
												{1,0,0,0},{1,1,0,0},{0,1,0,0},{0,1,1,0},{0,0,1,0},
												{0,0,1,1},{0,0,0,1},{0,0,1,1},{1,1,1,1},{1,1,1,0},
												{1,1,0,0},{1,0,0,0},{0,0,0,0},{1,0,1,0},{0,1,0,1}};
uint8_t LED_status[4] = {0,0,0,0};
int LED_pins[4]   = {18, 20, 21, 23};

int alive_timer = 0;
int alive_ones = 0;
int alive_tens = 0;
int alive_hundreds = 0;
int alive_thousands = 0;


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

void nLEDSetup(void) {
/* ******************************************************************
//	Function name : nLEDSetup
//	Functionality :	Setup the 4 LEDS on LPC1768 mbed Dev board
// 	Returns				:	None	
//  Input range		: None
// *****************************************************************/
	LPC_PINCON->PINSEL4 &= ~(0x0000CF30);	// Set P1:18, P1:20, P1:21, P1:23 to function 1 (GPIO)
	LPC_GPIO0->FIODIR   |=   0x0000000F;
	LPC_GPIO1->FIODIR   |=   0x00B40000;	// Set Port 1 as output
}
// ***** End of Function ********************************************


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


void nLEDFlip(unsigned int led_flip[]) {
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


void nDiscoFunc() {
/* ******************************************************************
//	Function name : nDiscoFunc
//	Functionality :	Disco's 4 LEDS on Dev board. can be used to verify various stuff
// 	Returns				:	None
//  Input range		: None
// *****************************************************************/	
	for (int n = 0; n < 1; n++) {
		for(int i = 0; i < 20; i++) {
			nLED_SET(disco_arr[i][0] , disco_arr[i][1] , disco_arr[i][2] , disco_arr[i][3]);
			nDelayLED();
		}
	}
}
// ***** End of Function ********************************************


void nDelayLED() {
/* ******************************************************************
//	Function name : nDelayLED
//	Functionality :	Used rarely, as static non-dynamic delay for setting up LEDS
// 	Returns				:	None
//  Input range		: None
// *****************************************************************/	
	for (int i = 0; i < 1500000; i++);
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
	
	TickType_t xPreviousWakeTime = ( TickType_t ) 0U;
	
	while(1) {
		
		if (LED_status)
			LED_status = 0;
		else
			LED_status = 1;
		
		nLED_SET(LED_status,2,2,2); 
		
		alive_timer++;
		
//		vTaskDelay(1000);
			vTaskDelayUntil( &xPreviousWakeTime, ( TickType_t ) 1000 / portTICK_PERIOD_MS );
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
	uint8_t 	seconds_ones = 0;
	uint8_t 	seconds_tens = 0;
	
	uint8_t 	minutes = 0;
	uint8_t 	minutes_ones = 0;
	uint8_t 	minutes_tens = 0;
	
	uint8_t 	hours = 0;
	uint8_t 	hours_ones = 0;
	uint8_t 	hours_tens = 0;
	
	seconds = alive_timer;

	while(seconds >= 3600) {
		hours++;
		seconds -= 3600;
	}
	
	while(seconds >= 60) {
		minutes++;
		seconds -= 60;
	}
	
	while(hours >= 10) {
		hours_tens++;
		hours -= 10;
	}
	
	while(hours >= 1) {
		hours_ones++;
		hours -= 1;
	}
	
	while(minutes >= 10) {
		minutes_tens++;
		minutes -= 10;
	}
	
	while(minutes >= 1) {
		minutes_ones++;
		minutes -= 1;
	}
	
	while(seconds >= 10) {
		seconds_tens++;
		seconds -= 10;
	}
	
	while(seconds >= 1) {
		seconds_ones++;
		seconds -= 1;
	}
	
	nUART_TxString("Current alive time :    ");
	
	if (hours_tens > 0)
		nUART_TxChar(hours_tens + '0');
	nUART_TxChar(hours_ones + '0');
	
	nUART_TxString(":");
	
	if (minutes_tens > 0)
		nUART_TxChar(minutes_tens + '0');
	nUART_TxChar(minutes_ones + '0');
	
	nUART_TxString(":");
	
	if (seconds_tens > 0)
		nUART_TxChar(seconds_tens + '0');
	nUART_TxChar(seconds_ones + '0');
	
	nUART_TxString("\r\n");
	nNewLine( 1 );
	nUART_TxString("Error counter        : 0");										// Should get Error_Counter variable from error library
	nNewLine( 1 );
	nUART_TxString("Warning counter      : 0");										// Should get Warning_Counter variable from error library
	nNewLine( 1 );	
	nUART_TxString("Safety Actions taken : 0");										// Should get Safety_Action_Counter variable from error library
	nNewLine( 2 );
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

