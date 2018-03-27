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
 *  
 *	
 *
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

// ****************************************************************************************
//
//					Variables
//
// ****************************************************************************************
uint8_t LOGGED_IN = 0;
uint8_t USERNAME_MATCHED = 0;
uint8_t PASSWORD_MATCHED = 0;

static uint8_t UART_STATE = UartState_FindUser;

char input_buffer[128];


char USER_LIBRARY[USERS][5] = {{"NON"} , {"map"} , {"ab"} , {"frs"} , {"nig"}};		// Brugernavne
char PASS_LIBRARY[USERS][5] = {{"213"} , {"123"} , {"666"} , {"123"} , {"123"}};		// Passwords

																
char USERS_NAMES[USERS][30]	= {{"Blind Makker"},
															 {"Mark Appelgren"},
															 {"Anders B. Hansen"},
															 {"Frederik Snedevind"},
															 {"Blind Makker"}};
																
																
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
																							{"LEDSTATE1\0"},					// F12
																							{"LEDSTATE2"},					// F13
																							{"LEDSTATE3"},					// F14
																							{"LEDSTATE4"},					// F15
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
																							 nTerminal_LED_1_ON 	, 		// F12
																							 nTerminal_LED_2_ON	, 			// F13
																							 nTerminal_LED_3_ON	, 			// F14
																							 nTerminal_LED_4_ON 	, 		// F15
																							 nTerminal_LED_1_ON	, 			// F16
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
	nTerminalClear();
	
	nUART_TxString("\r\n");
	nUART_TxString("____________________________________________ \r\n");
	nUART_TxString("|                                          | \r\n");
	nUART_TxString("|           Jeros Control Panel            | \r\n");
	nUART_TxString("|");
	nUART_TxString(branch_string);
	nUART_TxString(" branch____________________________| \r\n\n");
}

/* Function to transmit a char */
void nUART_TxChar(char ch) {
    while(util_IsBitCleared(LPC_UART0->LSR,SBIT_THRE)); // Wait for Previous transmission
    LPC_UART0->THR=ch;                                  // Load the data to be transmitted
}

/* Function to Receive a char */
char nUART_RxChar() {
    char ch; 
    while(util_IsBitCleared(LPC_UART0->LSR,SBIT_RDR));  // Wait till the data is received
    ch = LPC_UART0->RBR;                                // Read received data    
    return ch;
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
	nUART_TxString("The following commands are available\n\r");
	nUART_TxString("The following commands are available\n\r");
	nUART_TxString("The following commands are available\n\r");
	nUART_TxString("The following commands are available\n\r");
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
	nUART_TxString("\r\nCurrent state       : snoo\n\r");
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
		nUART_TxString("Undefined function.");
}

/* Keyword funtion for undefined inputs */ 
void nTerminalNoFunctionFound() {
	nUART_TxString("No matching function found. try 'help' for more info. \r\n");
}

/* Task function for UART */
void tUART_Task() {
// *************************************
// 						Local variables
//	int ChosenFunction;
	char last_char;
	
	static uint8_t inputs = 0;
	static uint8_t OutedUserMsg 		= 0;
	static uint8_t OutedPassMsg 		= 0;
	static uint8_t OutedFunctionMsg = 0;
	
	switch (UART_STATE) {
		
// *******************************************************************
//										Check Username State
		case UartState_FindUser :
			
			if (! OutedUserMsg) {
				nUART_TxString("Enter User: ");
				OutedUserMsg = 1;
			}
			
			last_char = nUART_RxChar();
		
			if ( yKeyHit (CHAR_ENTER , last_char ) && inputs == 0) {
				nNewLine( 1 );
				OutedUserMsg = 0;
			}
			
			else if ( yKeyHit (CHAR_ENTER , last_char ) && inputs > 0) {
				input_buffer[inputs] = '\0';
				inputs++;
				
				USERNAME_MATCHED = vCheckUsernames(  input_buffer , inputs );
				nNewLine( 1 );
			
				UART_STATE = UartState_FindPass;
				inputs = 0;				
				OutedUserMsg = 0;
			}
			
			else {
				input_buffer[inputs] = last_char;
				nUART_TxChar(input_buffer[inputs]);
				inputs++;
				
			}
			break;
			
// *******************************************************************
//										Check Password State
		case UartState_FindPass :
			
			if (! OutedPassMsg) {
				nUART_TxString("Enter Password: ");
				OutedPassMsg = 1;
			}
			
			last_char = nUART_RxChar();
		
			if ( yKeyHit (CHAR_ENTER , last_char ) && inputs == 0) {
				nNewLine( 1 );
				UART_STATE = UartState_FindUser;
				OutedPassMsg = 0; 
			}
			
			else if ( yKeyHit (CHAR_ENTER , last_char ) && inputs > 0) {
				input_buffer[inputs] = '\0';
				inputs++;
				
				if ( vCheckPasscode( input_buffer , inputs ) ) {
					UART_STATE = UartState_Functioncall;
					inputs = 0;
					nNewLine( 2 );
					nUART_TxString("Logged In. Welcome ");
					nUART_TxString(USERS_NAMES[USERNAME_MATCHED]);
					nNewLine( 2 );
				}
				else {
					nNewLine( 2 );
					nUART_TxString("Unrecognized user.");
					nNewLine( 1 );
					UART_STATE = UartState_FindUser;
					nNewLine( 1 );
					inputs = 0;
				}
				OutedPassMsg = 0;
			}
			else {
				input_buffer[inputs] = last_char;
				nUART_TxChar('*');
				inputs++;
			}
			break;
			
// *******************************************************************
//										Check Function State		
		case UartState_Functioncall :
			
			if (! OutedFunctionMsg) {
				nUART_TxString("Command: ");
				OutedFunctionMsg = 1;
			}
			last_char = nUART_RxChar();
			if ( yKeyHit (CHAR_ENTER , last_char ) && inputs == 0) {
				nNewLine( 1 );
				OutedFunctionMsg = 0;
			}
			else if ( yKeyHit (CHAR_ENTER , last_char ) && inputs > 0) {
				input_buffer[inputs] = '\0';
				inputs++;
				keyword_functions[vFindStringMatch( input_buffer , inputs )]();
				nNewLine( 1 );																											// Start a new line		
				inputs = 0;	
				OutedFunctionMsg = 0; 
			}
			else {
				input_buffer[inputs] = last_char;
				nUART_TxChar(input_buffer[inputs]);
				inputs++;
			}
			break;
		
		default :
			nNewLine( 4 );
			nUART_TxString("You aren't supposed to be here.");
			UART_STATE = UartState_FindUser;
			break;
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







