/**********************************************************************************************
 * Source file : UART_control.c
 * Author : Frederik Snedevind
 *		    Frs@Jeros.com
 *
 * Company: Jeros A/S
 *		   HTTP://www.Jeros.com/
 *
 * Date created : 9/3 - 2018
 * Version			 : 0.1.0
 * Revised			 : 16/3 - 2018
 **********************************************************************************************
 * Description:
 * 	Source file providing functionality to the UART API.
 **********************************************************************************************
 * Dependencies:
 *	This library uses the following files:
 *	"stdutils.h"		-		library containg typedef for various datatypes used
 *	"UART_control.h"-		own header file, providing API to other files
 *	"lpc17xx.h"			-		Processor header, used to setup and access various registers
 * 	"LED_control.h"	-		Handles LED functions on the DEV. board, used for debugging and development purposes
 **********************************************************************************************/

// ****************************************************************************************
//
//					Libraries
//
// ****************************************************************************************
#include "lpc17xx.h"
#include "stdutils.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "UART_control.h"
#include "LED_control.h"
#include "GPIO_setup.h"
#include "Programs_file.h"
#include "Sensor_file.h"



// ****************************************************************************************
//
//					Variables
//
// ****************************************************************************************
uint8_t LOGGED_IN = 0;
uint8_t USERNAME_MATCHED = 0;
uint8_t PASSWORD_MATCHED = 0;

xQueueHandle qUART_TxQ = NULL;
xQueueHandle qUART_RxQ = NULL;

SemaphoreHandle_t UART0_TxSemaphore = NULL;

static uint8_t UART_STATE = UartState_FindUser;
static uint8_t OutedStatusMsg = 0;

char input_buffer[32];

static uint8_t inputs = 0;

char USER_LIBRARY[USERS][8] 				= {{"guest"} 					, {"map"} 					 , {"ab"} 							, {"frs"} 							 , {"bj"}};		// Brugernavne
char PASS_LIBRARY[USERS][8] 				= {{"guest"} 					, {"123"} 					 , {"123"} 							, {"123"} 							 , {"246"}};		// Passwords											
char USERS_NAMES[USERS][30]					= {{"Guest"} , {"Mark Appelgren"} , {"Anders B. Hansen"} , {"Frederik Snedevind"} , {"Brian Jorgensen"}};
char USER_PERMISSIONS[USERS]				= {		'1'		 ,				'0'					,				'0'						 ,				'5'							,						'0'}; 

char branch_string[16] = {"Sandbox"};

char keyword_strings[][12] 		 = {{" "},									// F0
//																	{"Help"},								// F1
//																	{"HELP"},								// F2
																	{"LEDOFF"},							// F3
																	{"LEDALL"},							// F4
//																	{"status"},							// F5
																	{"clear"},							// F6
//																	{"wash2"},							// F7
//																	{"setuppin1"},					// F8
//																	{"setuppin2"},					// F9
//																	{"pf1"},								// F10
//																	{"pf2"},								// F11
																	{"ADC"},								// F12
																	{"uptime"},							// F13
																	{"logout"},							// F14
																	{"stop"},								// F15
																	{"help"},								// F16
																	{"sensordata"},					// F17
//																	{"alive stop"},					// F18
//																	{"alive start"},				// F19
																	{"flip p20"},						// F20
																	{"flip p21"},						// F21
																	{"flip p22"},						// F22
																	{"flip p23"},						// F23
																	{"flip p24"},						// F24
																	{"flip p25"},						// F25
																	{"flip p26"},						// F26
																	{"flip p27"},						// F27
																	{"flip p28"}};					// F28

void (*keyword_functions[])()  = {nTerminalUndefined, 		// F0
//																	nTerminalHelp  	 	, 		// F1
//																	nTerminalHelp 		 	, 	// F2
																	nTerminal_LED_OFF		, 	// F3
																	nTerminal_LED_ALL_ON, 	// F4
//																	nTerminalStatus		, 		// F5
																	nTerminalClear		 	, 	// F6
//																	nWashProgram_2 	  , 		// F7
//																	nPinSetup_1 	, 				// F8
//																	nPinSetup_2	, 					// F9
//																	nPinFlip_1	, 					// F10
//																	nPinFlip_2 	, 					// F11
																	nADC_Status 	, 				// F12
																	nPrintAlive					, 	// F13
																	nTerminalLogout	, 			// F14
																	nGPIO_STOP,							// F15
																	nTerminalHelp,					// F16
																	nSensorData,						// F17
//																	nAliveSuspend,					// F18
//																	nAliveResume,						// F19
																	nPinFlip_P20,						// F20
																	nPinFlip_P21,						// F21
																	nPinFlip_P22,						// F22
																	nPinFlip_P23,						// F23
																	nPinFlip_P24,						// F24
																	nPinFlip_P25,						// F25
																	nPinFlip_P26,						// F26
																	nPinFlip_P27,						// F27
																	nPinFlip_P28,						// F28
																								
																								
																								
	
	
};					// F19

																							 
// ****************************************************************************************
//	Type		: 	YES_RETURN Functions
//	Example		:	yXxXxX();
//	Description	:	Returns true (1) or false (0) depending on the success of the function
// ****************************************************************************************
uint8_t yKeyHit( uint8_t KEY_CHECK , uint8_t KEY_HIT ) {
/* ******************************************************************
//	Function name : yKeyHit
//	Functionality :	Compares 2 characters, returns 1 if equal, 0 if not. used by UART to check for certain inputs
// 	Returns				:	None	
//  Input range		: None
// *****************************************************************/
	uint8_t yKeyHit_ret;
	
	if (KEY_HIT == KEY_CHECK)
		yKeyHit_ret = 1;
	else
		yKeyHit_ret = 0;
	
	return yKeyHit_ret;
}


/* Function to transmit a char */

uint8_t yUART_TxReady( void ) {
/* ******************************************************************
//	Function name : yUART_TxReady
//	Functionality :	Check if bit 0 & bit 5 is set in LSR register, indicating data ready to be read, and transmission complete
// 	Returns				:	1 after a while. Should have a timeout timer as well, to avoid infinite loops in failure conditions	
//  Input range		: None
// *****************************************************************/	
	while (! LPC_UART0->LSR & 0x21 );
	return 1;
}

uint8_t yUART_RxReady( void ) {
/* ******************************************************************
//	Function name : yUART_RxReady
//	Functionality :	Check if bit 1 is set in LSR register, indicating receive data ready flag has been set
// 	Returns				:	1 after a while. Should have a timeout timer as well, to avoid infinite loops in failure conditions	
//  Input range		: None
// *****************************************************************/		
	uint8_t ret_val = 0;
	if (LPC_UART0->LSR & 0x1)
		ret_val = 1;
	
	return ret_val;
}

// ****************************************************************************************
//	Type		: 	NO_RETURN Functions
//	Example		:	nXxXxX();
//	Description	:	Does not return anything.
// ****************************************************************************************

void nADC_Status() {
/* ******************************************************************
//	Function name : nADC_Status
//	Functionality :	Print the current data from 8 ADC Channels to UART
// 	Returns				:	None	
//  Input range		: None
// *****************************************************************/	
	if (xSemaphoreTake(UART0_TxSemaphore , 5)) {
		nUART_TxString("Analog Inputs");
		nNewLine( 1 );
		for (uint8_t i = 0; i < ANALOG_SENSORS_END; i++) {
			nUART_TxString("Pin ");
			nUART_TxChar(i + '0');
			nUART_TxString(" : ");
			if (vGetSensorStatus(i) == SENSOR_VACANT)
				nUART_TxString("Vacant");
			else if (vGetSensorStatus(i) == SENSOR_ACTIVE)
				nUART_TxString("Active");
			else if (vGetSensorStatus(i) == SENSOR_PAUSED)
				nUART_TxString("Paused");
			nNewLine( 1 );
		}
		
		xSemaphoreGive(UART0_TxSemaphore);
	}
}

void nPinSetup_1() {
/* ******************************************************************
//	Function name : nPinSetup_1
//	Functionality :	Setup Port 0 Pin 0 (P9) for debugging, can be called from UART
// 	Returns				:	None	
//  Input range		: None	
// *****************************************************************/	
		ySetupDigitalIO( PORT_0 , PIN_0 , GPIO_OUTPUT);			// P9   for driving soappump relay		
}

void nPinSetup_2() {
/* ******************************************************************
//	Function name : nPinSetup_2
//	Functionality :	Setup Port 0 Pin 1 (P10) for debugging, can be called from UART
// 	Returns				:	None	
//  Input range		: None
// *****************************************************************/	
	ySetupDigitalIO( PORT_0 , PIN_1 , GPIO_OUTPUT );			// P10	for driving soappump relay
}


void nPinFlip_1() {
/* ******************************************************************
//	Function name : nPinFlip_1
//	Functionality :	Flip P9 from high/low/high depending on current status.
// 	Returns				:	None	
//  Input range		: None
// *****************************************************************/	
	static int pin_1_status = 0;

	if (xSemaphoreTake(UART0_TxSemaphore , 5)) {
		nUART_TxString("Flipped pin 1");
		nNewLine( 1 );
		xSemaphoreGive(UART0_TxSemaphore);
	}
	
	if ( pin_1_status == 0 )
		pin_1_status = 1;
	else
		pin_1_status = 0;
	
	yDigitalWrite(PORT_0 , PIN_0 , pin_1_status);
}


void nPinFlip_2() {
/* ******************************************************************
//	Function name : nPinFlip_2
//	Functionality :	Flip P10 from high/low/high depending on current status.
// 	Returns				:	None	
//  Input range		: None
// *****************************************************************/
	static uint8_t pin_2_status = 0;

	if (xSemaphoreTake(UART0_TxSemaphore , 5)) {
		nUART_TxString("Flipped pin 2");
		nNewLine( 1 );
		xSemaphoreGive(UART0_TxSemaphore);
	}
	
	if ( pin_2_status == 0 )
		pin_2_status = 1;
	else
		pin_2_status = 0;
	
	yDigitalWrite(PORT_0 , PIN_1 , pin_2_status);
}


void nPinFlip_P20() {
/* ******************************************************************
//	Function name : nPinFlip_P20
//	Functionality :	None. P20 is sacred! NO TOUCHING!
// 	Returns				:	None	
//  Input range		: None
// *****************************************************************/	

	// THIS PIN IS USED FOR THE SYSTEM CLOCK.
	nUART_TxString("No action.");
	nNewLine( 1 );
}

void nPinFlip_P21() {
/* ******************************************************************
//	Function name : nPinFlip_P21
//	Functionality :	Flip P21
// 	Returns				:	None	
//  Input range		: None
// *****************************************************************/	
	static int8_t pin_status = 1;									// Variable to contain the current pin status
	
	if (pin_status == GPIO_HIGH)									// If pin is high, set new status to low
		pin_status = GPIO_LOW;
	
	else if (! pin_status)												// If pin is low, set new status to high
		pin_status = GPIO_HIGH;
	
	yDigitalWrite( PORT_2 , PIN_5 , pin_status );	// Write the new status to the pin
	
	nUART_TxString("Flipped P21");
	nNewLine( 1 );
}

void nPinFlip_P22() {
/* ******************************************************************
//	Function name : nPinFlip_P22
//	Functionality :	Flip P22
// 	Returns				:	None	
//  Input range		: None
// *****************************************************************/	
	static int8_t pin_status = 1;									// Variable to contain the current pin status
	
	if (pin_status == GPIO_HIGH)									// If pin is high, set new status to low
		pin_status = GPIO_LOW;
	
	else if (! pin_status)												// If pin is low, set new status to high
		pin_status = GPIO_HIGH;
	
	yDigitalWrite( PORT_2 , PIN_4 , pin_status );	// Write the new status to the pin
	nUART_TxString("Flipped P22");
	nNewLine( 1 );
}
void nPinFlip_P23() {
/* ******************************************************************
//	Function name : nPinFlip_P23
//	Functionality :	Flip P23
// 	Returns				:	None	
//  Input range		: None
// *****************************************************************/	
	static int8_t pin_status = 1;									// Variable to contain the current pin status
	
	if (pin_status == GPIO_HIGH)									// If pin is high, set new status to low
		pin_status = GPIO_LOW;
	
	else if (! pin_status)												// If pin is low, set new status to high
		pin_status = GPIO_HIGH;
	
	yDigitalWrite( PORT_2 , PIN_3 , pin_status );	// Write the new status to the pin
	nUART_TxString("Flipped P23");
	nNewLine( 1 );
}
void nPinFlip_P24() {
/* ******************************************************************
//	Function name : nPinFlip_P24
//	Functionality :	Flip P24
// 	Returns				:	None	
//  Input range		: None
// *****************************************************************/	
	static int8_t pin_status = 1;									// Variable to contain the current pin status
	
	if (pin_status == GPIO_HIGH)									// If pin is high, set new status to low
		pin_status = GPIO_LOW;
	
	else if (! pin_status)												// If pin is low, set new status to high
		pin_status = GPIO_HIGH;
	
	yDigitalWrite( PORT_2 , PIN_2 , pin_status );	// Write the new status to the pin
	nUART_TxString("Flipped P24");
	nNewLine( 1 );
}
void nPinFlip_P25() {
/* ******************************************************************
//	Function name : nPinFlip_P25
//	Functionality :	Flip P25
// 	Returns				:	None	
//  Input range		: None
// *****************************************************************/	
	static int8_t pin_status = 1;									// Variable to contain the current pin status
	
	if (pin_status == GPIO_HIGH)									// If pin is high, set new status to low
		pin_status = GPIO_LOW;
	
	else if (! pin_status)												// If pin is low, set new status to high
		pin_status = GPIO_HIGH;
	
	yDigitalWrite( PORT_2 , PIN_1 , pin_status );	// Write the new status to the pin
	nUART_TxString("Flipped P25");
	nNewLine( 1 );
}

void nPinFlip_P26() {
/* ******************************************************************
//	Function name : nPinFlip_P26
//	Functionality :	Flip P26
// 	Returns				:	None	
//  Input range		: None
// *****************************************************************/	
	static int8_t pin_status = 1;									// Variable to contain the current pin status
	
	if (pin_status == GPIO_HIGH)									// If pin is high, set new status to low
		pin_status = GPIO_LOW;
	
	else if (! pin_status)												// If pin is low, set new status to high
		pin_status = GPIO_HIGH;
	
	yDigitalWrite( PORT_2 , PIN_0 , pin_status );	// Write the new status to the pin
	nUART_TxString("Flipped P26");
	nNewLine( 1 );
}

void nPinFlip_P27() {
/* ******************************************************************
//	Function name : nPinFlip_P27
//	Functionality :	Flip P27
// 	Returns				:	None	
//  Input range		: None
// *****************************************************************/	
	static int8_t pin_status = 1;									// Variable to contain the current pin status
	
	if (pin_status == GPIO_HIGH)									// If pin is high, set new status to low
		pin_status = GPIO_LOW;
	
	else if (! pin_status)												// If pin is low, set new status to high
		pin_status = GPIO_HIGH;
	
	yDigitalWrite( PORT_0 , PIN_11 , pin_status );	// Write the new status to the pin
	nUART_TxString("Flipped P27");
	nNewLine( 1 );
}

void nPinFlip_P28() {
/* ******************************************************************
//	Function name : nPinFlip_P28
//	Functionality :	Flip P28
// 	Returns				:	None	
//  Input range		: None
// *****************************************************************/	
	static int8_t pin_status = 1;									// Variable to contain the current pin status
	
	if (pin_status == GPIO_HIGH)									// If pin is high, set new status to low
		pin_status = GPIO_LOW;
	
	else if (! pin_status)												// If pin is low, set new status to high
		pin_status = GPIO_HIGH;
	
	yDigitalWrite( PORT_0 , PIN_10 , pin_status );	// Write the new status to the pin
	nUART_TxString("Flipped P28");
	nNewLine( 1 );
}


void nNewLine( uint8_t NumberOfLineSkips ) {
/* ******************************************************************
//	Function name : nNewLine
//	Functionality :	Go to a new line in UART terminal
// 	Returns				:	None	
//  Input range		: 0:? 
// *****************************************************************/
	for (uint8_t i = 0; i < NumberOfLineSkips; i++)
		nUART_TxString("\r\n");
}

void nTerminalLogout() {
/* ******************************************************************
//	Function name : nTerminalLogout
//	Functionality :	Logout from the terminal by resetting state and bools for username found & password matched
// 	Returns				:	None	
//  Input range		: 0:? 
// *****************************************************************/
	USERNAME_MATCHED = 0;
	PASSWORD_MATCHED = 0;
	LOGGED_IN 			 = 0;
	
	UART_STATE = UartState_FindUser;
	
	nNewLine( 2 );
	nUART_TxString("Logged out.");
	nNewLine( 2 );
	
}

/* Function to initialize UART0 */
void nUART0_init( unsigned int baudrate) {
/* ******************************************************************
//	Function name : nUART0_init
//	Functionality :	Initialize UART0 on the mbed board
// 	Returns				:	None	
//  Input range		: 0:? 
// *****************************************************************/
	unsigned int var_UartPclk_u32 , var_Pclk_u32 , var_RegValue_u32;
	
	LPC_PINCON->PINSEL0 &= ~0x000000F0;
	LPC_PINCON->PINSEL0 |= 0x00000050;            // Enable TxD0 P0.2 and p0.3 
	
	// Enable FIFO and reset Rx/Tx FIFO buffers    
	LPC_UART0->FCR = (1<<SBIT_FIFO) | (1<<SBIT_RxFIFO) | (1<<SBIT_TxFIFO); 
	
	// 8bit data, 1Stop bit, No parity
	LPC_UART0->LCR = (0x03<<SBIT_WordLenght) | (1<<SBIT_DLAB); 
	
	/** Baud Rate Calculation :
       PCLKSELx registers contains the PCLK info for all the clock dependent peripherals.
       Bit6,Bit7 contains the Uart Clock(ie.UART_PCLK) information.
       The UART_PCLK and the actual Peripheral Clock(PCLK) is calculated as below.
       (Refer data sheet for more info)
       
	
	
	
       UART_PCLK    PCLK
         0x00       SystemFreq/4        
         0x01       SystemFreq
         0x02       SystemFreq/2
         0x03       SystemFreq/8   
	 **/
	
	
	var_UartPclk_u32 = (LPC_SC->PCLKSEL0 >> 6) & 0x03;

	switch( var_UartPclk_u32 )
	{
		case 0x00:
			var_Pclk_u32 = SystemCoreClock/4;
			break;
		
		case 0x01:
			var_Pclk_u32 = SystemCoreClock;
			break; 
		
		case 0x02:
			var_Pclk_u32 = SystemCoreClock/2;
			break; 
		
		case 0x03:
			var_Pclk_u32 = SystemCoreClock/8;
			break;
	}
	
	var_RegValue_u32 = ( var_Pclk_u32 / (16 * baudrate )); 
	LPC_UART0->DLL =  var_RegValue_u32 & 0xFF;
	LPC_UART0->DLM = (var_RegValue_u32 >> 0x08) & 0xFF;

	util_BitClear(LPC_UART0->LCR,(SBIT_DLAB));  // Clear DLAB after setting DLL,DLM
	
}


void nUART_TxChar(char ch) {
/* ******************************************************************
//	Function name : nUART_TxChar
//	Functionality :	Transmit a char to the UART0 channel
// 	Returns				:	None	
//  Input range		: 0 - 255, all chars
// *****************************************************************/
	
	xQueueSend(qUART_TxQ , &ch , 10);
}


/* Function to Receive a char */
void nUART_RxChar() {
/* ******************************************************************
//	Function name : nUART_RxChar
//	Functionality :	Receive a char from UART channel, and send it to the 
//									UART RxQueue to allow The UART Task to process it
// 	Returns				:	None	
//  Input range		: None
// *****************************************************************/
	char ch;
  if (yUART_RxReady()) {
		ch = LPC_UART0->RBR;                                // Read received data    
		xQueueSend(qUART_RxQ , &ch , 2);
	}
}

/* Function to transmit a string to UART */
void nUART_TxString(char ch_s[]) {
/* ******************************************************************
//	Function name : nUART_TxString
//	Functionality :	Transmit a string to the UART0 Channel by simply utilizing 
//										nUART_TxChar function until a string termination char '\0' is met.
// 	Returns				:	None	
//  Input range		: None
// *****************************************************************/
	for (uint8_t i = 0; ch_s[i] != '\0'; i++)
		nUART_TxChar(ch_s[i]);
}


void nTerminalHelp() {
/* ******************************************************************
//	Function name : nTerminalHelp
//	Functionality :	Prints all the keywords that UART Checks for when input is given.
// 	Returns				:	None, but prints stuff to UART
//  Input range		: None
// *****************************************************************/
	if (xSemaphoreTake(UART0_TxSemaphore , 5)) {
		nUART_TxString("The following commands are available");
		nNewLine( 2 );
		
		for (uint8_t i = 0; i < (sizeof(keyword_strings) / sizeof(keyword_strings[0])) ; i++) {
			nUART_TxString("-> ");
			nUART_TxString(keyword_strings[i]);
			nNewLine( 1 );
		}
		nNewLine( 1 );
		
		xSemaphoreGive(UART0_TxSemaphore);
		
	}
}


void nTerminal_LED_1_ON() {
/* ******************************************************************
//	Function name : nTerminal_LED_1_ON
//	Functionality :	Turn LED 1 On from the UART terminal
// 	Returns				:	None	
//  Input range		: None
// *****************************************************************/
	uint8_t arr[4] = {1,0,0,0};
	nLEDFlip(arr);
	
}

void nTerminal_LED_2_ON() {
/* ******************************************************************
//	Function name : nTerminal_LED_2_ON
//	Functionality :	Turn LED 2 On from the UART terminal
// 	Returns				:	None	
//  Input range		: None
// *****************************************************************/
	uint8_t arr[4] = {0,1,0,0};
	nLEDFlip(arr);
}

void nTerminal_LED_3_ON() {
/* ******************************************************************
//	Function name : nTerminal_LED_3_ON
//	Functionality :	Turn LED 3 On from the UART terminal
// 	Returns				:	None	
//  Input range		: None
// *****************************************************************/
	uint8_t arr[4] = {0,0,1,0};
	nLEDFlip(arr);
}

void nTerminal_LED_4_ON() {
/* ******************************************************************
//	Function name : nTerminal_LED_4_ON
//	Functionality :	Turn LED 4 On from the UART terminal
// 	Returns				:	None	
//  Input range		: None
// *****************************************************************/
	uint8_t arr[4] = {0,0,0,1};
	nLEDFlip(arr);
}

void nTerminal_LED_OFF() {
/* ******************************************************************
//	Function name : nTerminal_LED_OFF
//	Functionality :	Turn all 4 LEDS OFF
// 	Returns				:	None	
//  Input range		: None
// *****************************************************************/
	nLED_SET(0,0,0,0);
}

void nTerminal_LED_ALL_ON() {
/* ******************************************************************
//	Function name : nTerminal_LED_ALL_ON
//	Functionality :	Turn all 4 LEDS ON
// 	Returns				:	None	
//  Input range		: None
// *****************************************************************/
	nLED_SET(1,1,1,1);
}

void nTerminalStatus() {
/* ******************************************************************
//	Function name : nTerminalStatus
//	Functionality :	Print out various status messages
// 	Returns				:	None	
//  Input range		: None
// *****************************************************************/
	nUART_TxString("Current state       : snoo\n\r");
	nUART_TxString("Current Temperature : \n\r");
	nUART_TxString("Current Program     : \n\r");
}


void nTerminalClear() {
/* ******************************************************************
//	Function name : nTerminalClear
//	Functionality :	Prints 25 lineskips to terminal making a new clear screen appear
// 	Returns				:	None	
//  Input range		: None
// *****************************************************************/
	nNewLine( 25 );
}

void nTerminalUndefined() {
/* ******************************************************************
//	Function name : nTerminalUndefined
//	Functionality :	Prints "Undefined function to UART. Should be called every time undefined input is given to the terminal, while logged in.
// 	Returns				:	None	
//  Input range		: None
// *****************************************************************/
		nNewLine( 1 );
		nUART_TxString("Undefined function.");
		nNewLine( 1 );
}


void nTerminalNoFunctionFound() {
/* ******************************************************************
//	Function name : nTerminalNoFunctionFound
//	Functionality :	Prints Stuff to terminal. not really used?
// 	Returns				:	None	
//  Input range		: None
// *****************************************************************/
	nUART_TxString("No matching function found. Try 'help' for more info.");
	nNewLine( 1 );
}


void nPrintInteger ( int input ) {
/* ******************************************************************
//	Function name : nPrintInteger
//	Functionality :	Prints an integer correctly to the UART Terminal.
// 	Returns				:	None	
//  Input range		: 0 - 9999
// *****************************************************************/
	uint8_t ones 					= 0;
	uint8_t tens 					= 0;
	uint8_t hundreds 			= 0;
	uint8_t thousands 		= 0;
	uint8_t tenthousands 	= 0;
	
	while (input >= 10000) {
		tenthousands++;
		input -= 10000;
	}
	
	while (input >= 1000) {
		thousands++;
		input -= 1000;
	}
	while (input >= 100) {
		hundreds++;
		input -= 100;
	}
	while (input >= 10) {
		tens++;
		input -= 10;
	}
	while (input >= 1) {
		ones++;
		input -= 1;
	}
	
	if (tenthousands) {
		nUART_TxChar(tenthousands + '0');
	}
	
	if (tenthousands || thousands)
		nUART_TxChar(thousands + '0');
	
	if (tenthousands || thousands || hundreds)
		nUART_TxChar(hundreds + '0');
	
	if (tenthousands || thousands || hundreds || tens)
		nUART_TxChar(tens + '0');
	
	nUART_TxChar(ones + '0');
}




// ****************************************************************************************
//	Type		: 	Task
//	Example		:	tXx();
//	Description	:	Designed for FreeRTOS. runs in a while loop, with *param to pass arguments for vTask*(); calls
// ****************************************************************************************


void tUART_RxTask( void *param ) {
/* ******************************************************************
//	Function name : tUART_RxTask
//	Functionality :	FreeRTOS Task for the Receiving Part of the UART communication on channel 0
// 	Returns				:	None	
//  Input range		: None
// *****************************************************************/
	
	uint8_t receive;
	uint8_t transmit;
	
	char blocked_character = '*';
	
	UART0_TxSemaphore = xSemaphoreCreateBinary();
	xSemaphoreGive( UART0_TxSemaphore );
	
	while(1) {	
		if ( xQueueReceive( qUART_RxQ , &receive , 2 )) {
			switch ( UART_STATE ) {
	// *******************************************************************
	//										Check Username State
	// *******************************************************************					
				case UartState_FindUser :
				
					if ( yKeyHit ( CHAR_BACKSPACE , receive )) {
						if ( inputs ) {
							inputs--;
							nUART_TxChar( CHAR_BACKSPACE );
							nUART_TxChar(' ');
							nUART_TxChar( CHAR_BACKSPACE );
						}
					}
				
					else if ( yKeyHit ( CHAR_ENTER , receive ) && inputs == 0) {
						nNewLine( 1 );
						OutedStatusMsg = 0;
					}
					
					else if ( yKeyHit ( CHAR_ENTER , receive ) && inputs > 0) {
						input_buffer[inputs] = '\0';
						inputs++;
						
						USERNAME_MATCHED = vCheckUsernames( input_buffer , inputs );
						nNewLine( 1 );
					
						UART_STATE = UartState_FindPass;
						OutedStatusMsg = 0;
						inputs = 0;				
					}
					
					else {						
						input_buffer[inputs] = receive;
						xQueueSend( qUART_TxQ , &input_buffer[inputs] , 2 );
						inputs++;						
					}
					
					vTaskDelay(10);
					break;
					
		// *******************************************************************
		//										Check Password State
					
				case UartState_FindPass : 
					
					if ( yKeyHit ( CHAR_BACKSPACE , receive )) {
						if (inputs > 0) {
							inputs--;
							nUART_TxChar( CHAR_BACKSPACE );
							nUART_TxChar(' ');
							nUART_TxChar( CHAR_BACKSPACE );
						}
					}
				
					else if ( yKeyHit ( CHAR_ENTER , receive ) && inputs == 0) {
						nNewLine( 1 );
						nUART_TxString("Unrecognized user.");
						nNewLine( 2 );
						UART_STATE = UartState_FindUser;
						OutedStatusMsg = 0; 
					}
					
					else if ( yKeyHit (CHAR_ENTER , receive ) && inputs > 0) {
						input_buffer[inputs] = '\0';
						inputs++;
						
						if ( vCheckPasscode( input_buffer , inputs ) ) {
							UART_STATE = UartState_Functioncall;
							OutedStatusMsg = 0;
							inputs = 0;
							nNewLine( 2 );
							
							if (xSemaphoreTake(UART0_TxSemaphore, ( TickType_t ) 10 ) == pdTRUE) {
								nUART_TxString("Logged in.\r\nWelcome " );
								nUART_TxString(USERS_NAMES[USERNAME_MATCHED]);
								nNewLine( 1 );
								nUART_TxString("Current Permission level: ");
								nUART_TxChar(USER_PERMISSIONS[USERNAME_MATCHED]);
								xSemaphoreGive( UART0_TxSemaphore );
							}
							
							if (xSemaphoreTake(UART0_TxSemaphore, ( TickType_t ) 10 ) == pdTRUE) {
								nNewLine( 2 );
								xSemaphoreGive( UART0_TxSemaphore );
							}
						}
						else {
							if (xSemaphoreTake(UART0_TxSemaphore, ( TickType_t ) 10 ) == pdTRUE) {
								nNewLine( 2 );
								nUART_TxString("Unrecognized user.\r\n");
								nNewLine( 1 );
								UART_STATE = UartState_FindUser;
								OutedStatusMsg = 0;
								nNewLine( 1 );
								inputs = 0;
								xSemaphoreGive( UART0_TxSemaphore );
							}
						}
						OutedStatusMsg = 0;
					}
					
					else {
						input_buffer[inputs] = receive;
						transmit = blocked_character;
						xQueueSend(qUART_TxQ , &transmit , 2);
						inputs++;
					}
					
					vTaskDelay(10);
					
					break;
					
		// *******************************************************************
		//										Check Function State		
				case UartState_Functioncall :
					
					if ( yKeyHit (CHAR_BACKSPACE , receive)) {
						if (inputs > 0) {
							inputs--;
							nUART_TxChar(CHAR_BACKSPACE);
							nUART_TxChar(' ');
							nUART_TxChar(CHAR_BACKSPACE);
						}
					}
				
					else if ( yKeyHit (CHAR_ENTER , receive ) && inputs == 0) {
						nNewLine( 1 );
						OutedStatusMsg = 0;
					}
					
					else if ( yKeyHit (CHAR_ENTER , receive ) && inputs > 0) {
						input_buffer[inputs] = '\0';
						inputs++;
						nNewLine( 1 );	
						keyword_functions[vFindStringMatch( input_buffer , inputs )]();	
						inputs = 0;	
						OutedStatusMsg = 0;
					}
					else {
						input_buffer[inputs] = receive;
						
						if (xSemaphoreTake(UART0_TxSemaphore, ( TickType_t ) 10 ) == pdTRUE) {
							nUART_TxChar(input_buffer[inputs]);
							xSemaphoreGive( UART0_TxSemaphore );
						}
						
						inputs++;
					}
					break;
				
				default :
					nNewLine( 4 );
					if (xSemaphoreTake(UART0_TxSemaphore, ( TickType_t ) 10 ) == pdTRUE) {
						nUART_TxString( "You're not supposed to be here." );
						nNewLine( 1 );
						nUART_TxString( "Returning to start." );
						xSemaphoreGive( UART0_TxSemaphore );
					}
					
					UART_STATE = UartState_FindUser;
					break;
			}
		}
		else
			vTaskDelay(10);
		
		if (yUART_RxReady())
			nUART_RxChar();
		
		vTaskDelay(10);
	}
}


//	---------------------------------------------------
//									UART Transmit task
//	---------------------------------------------------
void tUART_TxTask( void *param ) { 
/* ******************************************************************
//	Function name : tUART_TxTask
//	Functionality :	FreeRTOS Task for the Transmitting Part of the UART communication on channel 0
// 	Returns				:	None	
//  Input range		: None
// *****************************************************************/
	uint8_t receive;

	while(1) {

		if (! OutedStatusMsg) {
			switch(UART_STATE) {
				case UartState_FindUser :
					if (xSemaphoreTake(UART0_TxSemaphore, ( TickType_t ) 10 ) == pdTRUE) {
						nUART_TxString("Enter Username: ");
						xSemaphoreGive( UART0_TxSemaphore );
					}
				break;
				
				case UartState_FindPass :
					if (xSemaphoreTake(UART0_TxSemaphore, ( TickType_t ) 10 ) == pdTRUE) {
						nUART_TxString("Enter Password: ");
						xSemaphoreGive( UART0_TxSemaphore );
					}
				break;
				
				case UartState_Functioncall :
					if (xSemaphoreTake(UART0_TxSemaphore, ( TickType_t ) 10 ) == pdTRUE) {
						nUART_TxString(">> ");
						xSemaphoreGive( UART0_TxSemaphore );
					}
				break;
			}
						OutedStatusMsg = 1;
		}

		
		if (xQueueReceive(qUART_TxQ , &receive , 2) ) {
			if (yUART_TxReady())
				LPC_UART0->THR = receive;                                  // Load the data to be transmitted
		}
		//nUART_TxChar(receive);
		
		vTaskDelay(10); 
	}
}
	


// ****************************************************************************************
//	Type		: 	VALUE_RETURN Functions
//	Example		:	vXxXxX();
//	Description	:	Returns a value, either 1 or 0. no confirmation if successful or not.
// ****************************************************************************************

uint8_t vFindStringMatch(char InputString[] , uint8_t length) {
/* ******************************************************************
//	Function name : vFindStringMatch
//	Functionality :	Takes input from UART when ENTER is hit
// 	Returns				:	None	
//  Input range		: string & length of string
//  Fixes					: Check if (MatchFound) and return 0 if not, keep "undefined function" as 0'th function in keyword function register
// *****************************************************************/	
	uint8_t MatchFound = 0;
	uint8_t KeywordMatched = 0;
	
	for (uint16_t i = 0; i < NO_OF_KEYWORDS; i++) {															// 16 bits iterator allows for max ~65k keywords.
		MatchFound = 1;																														// 8 bits iterator allows for only 255 different keywords.
		
		uint8_t Str1Length , Str2Length;
		
		for ( Str1Length = 0; InputString[Str1Length] 			 != '\0'; Str1Length++);
		for ( Str2Length = 0; keyword_strings[i][Str2Length] != '\0'; Str2Length++); 
		
		if (Str1Length != Str2Length)
			continue;
		
		for (uint8_t n = 0; n < length; n++) {
			if ( (InputString[n] != keyword_strings[i][n]) )
				MatchFound = 0;
		}
		if (MatchFound)
			KeywordMatched = i;
	}
	return KeywordMatched;
}



uint8_t vCheckUsernames(char InputString[] , uint8_t length) {
/* ******************************************************************
//	Function name : vCheckUsernames
//	Functionality :	Takes input from UART when ENTER is hit, while inputting username. scans USER_LIBRARY for matching names
// 	Returns				:	0 - USER (definition) depending on whether or not user has been matched.
//  Input range		: None
// *****************************************************************/		
	uint8_t MatchFound = 0;
	uint8_t UserMatched = 0;
	
	for (uint8_t i = 1; i < USERS; i++) {
		MatchFound = 1;
		
		uint8_t Str1Length , Str2Length;
		
		for ( Str1Length = 0; InputString[Str1Length] 		!= '\0'; Str1Length++);
		for ( Str2Length = 0; USER_LIBRARY[i][Str2Length]	!= '\0'; Str2Length++); 
		
		if (Str1Length != Str2Length)
			continue;
		
		for (uint8_t n = 0; n < length; n++) {
			if ( (InputString[n] != USER_LIBRARY[i][n]) )
				MatchFound = 0;
		}
			if (MatchFound)
				UserMatched = i;
	}
	return UserMatched;
}

uint8_t vCheckPasscode(char InputString[] , uint8_t length) {
/* ******************************************************************
//	Function name : vCheckPasscode
//	Functionality :	Takes input from UART when ENTER is hit, while inputting passwords. scans PASS_LIBRARY for matching names
// 	Returns				:	0 - USER (definition) depending on whether or not user has been matched.
//  Input range		: None
// *****************************************************************/	
	uint8_t MatchFound = 1;
	uint8_t Str1Length , Str2Length;
	
	for ( Str1Length = 0; InputString[Str1Length] 										!= '\0'; Str1Length++);
	for ( Str2Length = 0; PASS_LIBRARY[USERNAME_MATCHED][Str2Length]	!= '\0'; Str2Length++); 
	
	if (Str1Length != Str2Length)
		MatchFound = 0;
	
	if (MatchFound) {
		for (uint8_t n = 0; n < length; n++) {
			if ( (InputString[n] != PASS_LIBRARY[USERNAME_MATCHED][n]) )
				MatchFound = 0;
		}
	}
	return MatchFound;
}


