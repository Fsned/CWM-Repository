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
#include "stdutils.h"
#include "UART_control.h"
#include "lpc17xx.h"
#include "LED_control.h"
#include "GPIO_setup.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

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

char input_buffer[128];

static uint8_t inputs = 0;

char USER_LIBRARY[USERS][5] = {{"NON"} 					, {"map"} 					 , {"ab"} 							, {"frs"} 							 , {"**"}};		// Brugernavne
char PASS_LIBRARY[USERS][5] = {{"213"} 					, {"123"} 					 , {"666"} 							, {"123"} 							 , {"123"}};		// Passwords											
char USERS_NAMES[USERS][30]	= {{"Blind Makker"} , {"Mark Appelgren"} , {"Anders B. Hansen"} , {"Frederik Snedevind"} , {"Blind Makker"}};
																
																
char branch_string[20] = {"Sandbox"};

char keyword_strings[NO_OF_KEYWORDS][10] 	 = {{"help"},								// F0
																							{"Help"},								// F1
																							{"HELP"},								// F2
																							{"LED1"},								// F3
																							{"LED2"},								// F4
																							{"LED3"},								// F5
																							{"LED4"},								// F6
																							{"LEDOFF"},							// F7
																							{"LEDALL"},							// F8
																							{"status"},							// F9
																							{"clear"},							// F10
																							{"Clear"},							// F11
																							{"setuppin1"},					// F12
																							{"setuppin2"},					// F13
																							{"pf1"},					// F14
																							{"pf2"},					// F15
																							{"LED"},								// F16
																							{"DiScO"},							// F17
																							{"logout"},							// F18
																							{"undef"}};							// F19

void (*keyword_functions[NO_OF_KEYWORDS])() = {nTerminalHelp 		 	, 			// F0
																							 nTerminalHelp  	 	, 			// F1
																							 nTerminalHelp 		 	, 			// F2
																							 nTerminal_LED_1_ON 	, 		// F3
																							 nTerminal_LED_2_ON 	, 		// F4
																							 nTerminal_LED_3_ON 	, 		// F5
																							 nTerminal_LED_4_ON  , 			// F6
																							 nTerminal_LED_OFF		, 		// F7
																							 nTerminal_LED_ALL_ON, 			// F8
																							 nTerminalStatus		, 			// F9
																							 nTerminalClear		 	, 			// F10
																							 nTerminalClear 	  , 			// F11
																							 nPinSetup_1 	, 		// F12
																							 nPinSetup_2	, 			// F13
																							 nPinFlip_1	, 			// F14
																							 nPinFlip_2 	, 		// F15
																							 nTerminalUndefined	, 			// F16
																							 nDiscoFunc					, 			// F17
																							 nTerminalLogout	, 				// F18
																							 nTerminalUndefined };			// F19

																							 
// ****************************************************************************************
//	Type		: 	YES_RETURN Functions
//	Example		:	yXxXxX();
//	Description	:	Returns true (1) or false (0) depending on the success of the function
// ****************************************************************************************
uint8_t yKeyHit( uint8_t KEY_CHECK , uint8_t KEY_HIT ) {
	uint8_t yKeyHit_ret;
	
	if (KEY_HIT == KEY_CHECK)
		yKeyHit_ret = 1;
	else
		yKeyHit_ret = 0;
	
	return yKeyHit_ret;
}

// ****************************************************************************************
//	Type		: 	NO_RETURN Functions
//	Example		:	nXxXxX();
//	Description	:	Does not return anything.
// ****************************************************************************************
void nPinSetup_1() {
		ySetupDigitalO( PORT_0 , PIN_0 );			// P9   for driving soappump relay
}

void nPinSetup_2() {
	ySetupDigitalO( PORT_0 , PIN_1 );			// P10	for driving soappump relay
}


void nPinFlip_1( ) {
	static int pin_1_status = 0;

	if ( pin_1_status == 0 )
		pin_1_status = 1;
	else
		pin_1_status = 0;
	
	yDigitalWrite(PORT_0 , PIN_0 , pin_1_status);
}


void nPinFlip_2( ) {
	static uint8_t pin_2_status = 0;

	if ( pin_2_status == 0 )
		pin_2_status = 1;
	else
		pin_2_status = 0;
	
	yDigitalWrite(PORT_0 , PIN_1 , pin_2_status);
}




void nNewLine( uint8_t NumberOfLineSkips ) {
	
	for (int i = 0; i < NumberOfLineSkips; i++)
		nUART_TxString("\r\n");
}

void nTerminalLogout() {
	USERNAME_MATCHED = 0;
	PASSWORD_MATCHED = 0;
	LOGGED_IN 			 = 0;
	
	UART_STATE = UartState_FindUser;
	
	nNewLine( 1 );
	nNewLine( 1 );
	nUART_TxString("Logged out.\r\n");
	nNewLine( 1 );
	
}

/* Function to initialize UART0 */
void nUART0_init( unsigned int baudrate) {
	
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

/* Function to transmit a char */

uint8_t yUART_TxReady( void ) {
	
	while(! LPC_UART0->LSR & 0x21 );
	
	return 1;
}

uint8_t yUART_RxReady( void ) {
	
	if (LPC_UART0->LSR & 0x1)
		return 1;
	
}


void nUART_TxChar(char ch) {
	if (yUART_TxReady())
		LPC_UART0->THR=ch;                                  // Load the data to be transmitted
}


/* Function to Receive a char */
void nUART_RxChar() {
	char ch;

	ch = LPC_UART0->RBR;                                // Read received data    
	xQueueSend(qUART_RxQ , &ch , 2);
}

/* Function to transmit a string to UART */
void nUART_TxString(char ch_s[]) {
	int i = 0;
	while(ch_s[i] != '\0') {
		nUART_TxChar(ch_s[i]);
		i++;
	}
}

/* Keyword function for "help" */
void nTerminalHelp() {
	nUART_TxString("The following commands are available\n\r");
	nNewLine( 2 );
	
	for (int i = 0; i < NO_OF_KEYWORDS; i++) {
		nUART_TxString(keyword_strings[i]);
		nUART_TxString("\r\n");
	}
}

/* Keyword funtion for "LED1" */ 
void nTerminal_LED_1_ON() {
	unsigned int arr[4] = {1,0,0,0};
	nLEDFlip(arr);	
}

/* Keyword funtion for "LED2" */ 
void nTerminal_LED_2_ON() {
	unsigned int arr[4] = {0,1,0,0};
	nLEDFlip(arr);
}

/* Keyword funtion for "LED3" */ 
void nTerminal_LED_3_ON() {
	unsigned int arr[4] = {0,0,1,0};
	nLEDFlip(arr);
}

/* Keyword funtion for "LED4" */ 
void nTerminal_LED_4_ON() {
	unsigned int arr[4] = {0,0,0,1};
	nLEDFlip(arr);
}

/* Keyword funtion for "LEDOFF" */ 
void nTerminal_LED_OFF() {
	nLED_SET(0,0,0,0);
}

/* Keyword funtion for "LEDALL" */ 
void nTerminal_LED_ALL_ON() {
	nLED_SET(1,1,1,1);
}

/* Keyword funtion for "status" */ 
void nTerminalStatus() {
	nUART_TxString("\r\n Current state       : snoo\n\r");
	nUART_TxString("Current Temperature : \n\r");
	nUART_TxString("Current Program     : \n\r");
}


/* Keyword funtion for "clear" */ 
void nTerminalClear() {
	for(int i = 0; i < 25; i++)
		nUART_TxString("\r\n");
}

/* Keyword funtion for undefined inputs */ 
void nTerminalUndefined() {
		nUART_TxChar('\r');
		nUART_TxChar('\n');
		nUART_TxString("Undefined function. \n\r");
}

/* Keyword funtion for undefined inputs */ 
void nTerminalNoFunctionFound() {
	nUART_TxString("No matching function found. try 'help' for more info. \r\n");
}


//	---------------------------------------------------
//									UART Receive task
//	---------------------------------------------------
void tUART_RxTask( void *param ) {
	
	uint8_t receive;
	uint8_t transmit;
	
	

	
	char blocked_character = '*';
	
	
	UART0_TxSemaphore = xSemaphoreCreateBinary();
	xSemaphoreGive( UART0_TxSemaphore );
	
	while(1) {
		
		
		
		if (xQueueReceive(qUART_RxQ , &receive , 2)) {
			
			switch (UART_STATE) {
	// *******************************************************************
	//										Check Username State
	// *******************************************************************					
				case UartState_FindUser :
				
					if ( yKeyHit (CHAR_ENTER , receive ) && inputs == 0) {
						nNewLine( 1 );
						OutedStatusMsg = 0;
						vTaskDelay(10);
					}
					
					else if ( yKeyHit (CHAR_ENTER , receive ) && inputs > 0) {
						input_buffer[inputs] = '\0';
						inputs++;
						
						USERNAME_MATCHED = vCheckUsernames(  input_buffer , inputs );
						nNewLine( 1 );
					
						UART_STATE = UartState_FindPass;
						OutedStatusMsg = 0;
						inputs = 0;				
//						OutedUserMsg = 0;
						vTaskDelay(10);
					}
					
					else {
							input_buffer[inputs] = receive;
							xQueueSend(qUART_TxQ , &input_buffer[inputs] , 2);
							inputs++;						
					}
					
					vTaskDelay(10);
					break;
					
		// *******************************************************************
		//										Check Password State
					
				case UartState_FindPass : 
					
					if ( yKeyHit (CHAR_ENTER , receive ) && inputs == 0) {
						nNewLine( 1 );
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
								nUART_TxString("Logged in.\r\n" );
								nUART_TxString("Welcome ");
								nUART_TxString(USERS_NAMES[USERNAME_MATCHED]);
								xSemaphoreGive( UART0_TxSemaphore );
							}
							
							
							nNewLine( 2 );
						}
						else {
							nNewLine( 2 );
							nUART_TxString("Unrecognized user.");
							nNewLine( 1 );
							UART_STATE = UartState_FindUser;
							OutedStatusMsg = 0;
							nNewLine( 1 );
							inputs = 0;
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
					
//					if (yUART_RxReady())
//						receive = nUART_RxChar();
					
					if ( yKeyHit (CHAR_ENTER , receive ) && inputs == 0) {
						nNewLine( 1 );
						OutedStatusMsg = 0;
					}
					
					else if ( yKeyHit (CHAR_ENTER , receive ) && inputs > 0) {
						input_buffer[inputs] = '\0';
						inputs++;
						nNewLine( 1 );	
						keyword_functions[vFindStringMatch( input_buffer , inputs )]();
																																// Start a new line		
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
						nUART_TxString("You aren't supposed to be here.");
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
	
	
	
	uint8_t receive;

	while(1) {

		if (! OutedStatusMsg) {
			switch(UART_STATE) {
				case UartState_FindUser :
					if (xSemaphoreTake(UART0_TxSemaphore, ( TickType_t ) 10 ) == pdTRUE) {
						nUART_TxString("Enter Username :");
						xSemaphoreGive( UART0_TxSemaphore );
					}
				break;
				
				case UartState_FindPass :
					if (xSemaphoreTake(UART0_TxSemaphore, ( TickType_t ) 10 ) == pdTRUE) {
						nUART_TxString("Enter Password :");
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

		if (xQueueReceive(qUART_TxQ , &receive , 2) )
			nUART_TxChar(receive);
		
		vTaskDelay(10); 
	}
}
	


// ****************************************************************************************
//	Type		: 	VALUE_RETURN Functions
//	Example		:	vXxXxX();
//	Description	:	Returns a value, either 1 or 0. no confirmation if successful or not.
// ****************************************************************************************
uint8_t vFindStringMatch(char InputString[] , uint8_t length) {
	
	uint8_t MatchFound = 0;
	uint8_t KeywordMatched = 19;
	
	for (int i = 0; i < NO_OF_KEYWORDS; i++) {
		MatchFound = 1;
		
		int Str1Length , Str2Length;
		
		for ( Str1Length = 0; InputString[Str1Length] 			 != '\0'; Str1Length++);
		for ( Str2Length = 0; keyword_strings[i][Str2Length] != '\0'; Str2Length++); 
		
		if (Str1Length != Str2Length)
			continue;
		
		for (int n = 0; n < length; n++) {
			if ( (InputString[n] != keyword_strings[i][n]) )
				MatchFound = 0;
		}
		if (MatchFound)
			KeywordMatched = i;
	}
	return KeywordMatched;
}



uint8_t vCheckUsernames(char InputString[] , uint8_t length) {
	
	uint8_t MatchFound = 0;
	uint8_t UserMatched = 0;
	
	for (int i = 1; i < USERS; i++) {
		MatchFound = 1;
		
		int Str1Length , Str2Length;
		
		for ( Str1Length = 0; InputString[Str1Length] 		!= '\0'; Str1Length++);
		for ( Str2Length = 0; USER_LIBRARY[i][Str2Length]	!= '\0'; Str2Length++); 
		
		if (Str1Length != Str2Length)
			continue;
		
		for (int n = 0; n < length; n++) {
			if ( (InputString[n] != USER_LIBRARY[i][n]) )
				MatchFound = 0;
		}
			if (MatchFound)
				UserMatched = i;
	}
	return UserMatched;
}

uint8_t vCheckPasscode(char InputString[] , uint8_t length) {
	
	uint8_t MatchFound = 0;
	MatchFound = 1;
	
	int Str1Length , Str2Length;
	
	for ( Str1Length = 0; InputString[Str1Length] 		!= '\0'; Str1Length++);
	for ( Str2Length = 0; PASS_LIBRARY[USERNAME_MATCHED][Str2Length]	!= '\0'; Str2Length++); 
	
	if (Str1Length != Str2Length)
		MatchFound = 0;
	
	for (int n = 0; n < length; n++) {
		if ( (InputString[n] != PASS_LIBRARY[USERNAME_MATCHED][n]) )
			MatchFound = 0;
	}
	return MatchFound;
}







