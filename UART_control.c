#include "UART_control.h"
#include "lpc17xx.h"
#include "stdutils.h"
#include "LED_control.h"
#include "string.h"


#define SBIT_WordLenght    0x00u
#define SBIT_DLAB          0x07u
#define SBIT_FIFO          0x00u
#define SBIT_RxFIFO        0x01u
#define SBIT_TxFIFO        0x02u

#define SBIT_RDR           0x00u
#define SBIT_THRE          0x05u

#define NO_OF_KEYWORDS 			20

#define USERS								5

int LOGGED_IN = 0;
int USERNAME_MATCHED = 0;
int PASSWORD_MATCHED = 0;
//int USER_ID_MATCHED;

char USER_IDS[USERS][5] = {{"oHo"},
													 {"frs"},
													 {"map"},
													 {"abh"},
													 {"asd"}};

char USER_PASS[USERS][5] = {{"aa4"},
														{"880"},
														{"123"},
														{"123"},
														{"123"}};

														
char USER_LIBRARY[3][USERS][5] ={{"oHo" , "frs" , "map" , "abh" , "asd"},
																	{"aa4" , "880" , "123" , "123" , "123"},
																	{'1' , '1' , '1' , '1' , '1'}};

													
char branch_string[20] = {"Sandbox"};

char keyword_strings[NO_OF_KEYWORDS][10] = {{"help"},			// F1
																						{"Help"},			// F2
																						{"HELP"},			// F3
																						{"LED1"},			// F4
																						{"LED2"},			// F5
																						{"LED3"},			// F6
																						{"LED4"},			// F7
																						{"LEDOFF"},		// F8
																						{"LEDALL"},		// F9
																						{"status"},		// F10
																						{"clear"},		// F11
																						{"Clear"},		// F12
																						{"LEDSTATE1"},// F13
																						{"LEDSTATE2"},// F14
																						{"LEDSTATE3"},// F15
																						{"LEDSTATE4"},// F16
																						{"LED"},			// F17
																						{"DiScO"},		// F18
																						{"undef"},		// F19
																						{"undef"}};		// F20

void (*keyword_functions[20]) = {terminal_help , terminal_help , terminal_help , LED_1_ON, LED_2_ON, LED_3_ON, LED_4_ON, LED_OFF, terminal_LED_ALL_ON, terminal_status,
												terminal_clear, terminal_clear, terminal_LED_1_ON, terminal_LED_2_ON, terminal_LED_3_ON, terminal_LED_4_ON, terminal_LED_1_ON, disco_func,
												terminal_undefined, terminal_undefined};
/*
void (*keyword_functions[NO_OF_KEYWORDS])()={{terminal_help}, 		 // 
																					 {terminal_help}, 		 //
																					 {terminal_help},			 //
																					 {terminal_LED_1_ON},	 //
																					 {terminal_LED_2_ON},	 //
																					 {terminal_LED_3_ON},	 //
																					 {terminal_LED_4_ON},	 //
																					 {terminal_LED_OFF},	 //
																					 {terminal_LED_ALL_ON},//
																					 {terminal_status},		 //
																					 {terminal_clear},		 //
																					 {terminal_clear},		 //
																					 {terminal_LED_1_ON},	 //
																					 {terminal_LED_2_ON},	 //
																					 {terminal_LED_3_ON},	 //
																					 {terminal_LED_4_ON},	 //
																					 {terminal_LED_1_ON},	 //
																					 {disco_func},				 //
																					 {terminal_undefined}, //
																					 {terminal_undefined}};//
																					 */

char help[]				= "help";
char Help[]				= "Help";
char HELP[]				= "HELP";
char LED_1_ON[]		= "LED1";
char LED_2_ON[]		= "LED2";
char LED_3_ON[]		= "LED3";
char LED_4_ON[]		= "LED4";
char LED_OFF[]		= "LEDOFF";
char LED_ALL_ON[] = "LEDALL";
char status[]			= "status";
char clear[]			= "clear";
char LEDSTATE1[]	= "LEDSTATE1";
char LEDSTATE2[]	= "LEDSTATE2";
char LEDSTATE3[]	= "LEDSTATE3";
char LEDSTATE4[]	= "LEDSTATE4";
char LED[]				= "LED";
char disco_str[]	= "DiScO";


/* Function to initialize UART0 */
void UART0_init( unsigned int baudrate) {
	
	unsigned int var_UartPclk_u32 , var_Pclk_u32 , var_RegValue_u32;
	
	LPC_PINCON->PINSEL0 &= ~0x000000F0;
	LPC_PINCON->PINSEL0 |= 0x00000050;            // Enable TxD0 P0.2 and p0.3 
	
	LPC_UART0->FCR = (1<<SBIT_FIFO) | (1<<SBIT_RxFIFO) | (1<<SBIT_TxFIFO); // Enable FIFO and reset Rx/Tx FIFO buffers    
	LPC_UART0->LCR = (0x03<<SBIT_WordLenght) | (1<<SBIT_DLAB); // 8bit data, 1Stop bit, No parity
	
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
	terminal_clear();
	uart_string("\r\n");
	uart_string("____________________________________________ \r\n");
	uart_string("|                                          | \r\n");
	uart_string("|           Jeros Control Panel            | \r\n");
	uart_string("|");
	uart_string(branch_string);
	uart_string(" branch____________________________| \r\n\n");
	uart_string("Press 'Enter' to continue\r\n");
}







/* Function to transmit a char */
void uart_TxChar(char ch) {
    while(util_IsBitCleared(LPC_UART0->LSR,SBIT_THRE)); // Wait for Previous transmission
    LPC_UART0->THR=ch;                                  // Load the data to be transmitted
}



/* Function to Receive a char */
char uart_RxChar() {
    char ch; 
    while(util_IsBitCleared(LPC_UART0->LSR,SBIT_RDR));  // Wait till the data is received
    ch = LPC_UART0->RBR;                                // Read received data    
    return ch;
}


/* Function to transmit a string to UART */
void uart_string(char ch_s[]) {
	int i = 0;
	while(ch_s[i] != '\0') {
		uart_TxChar(ch_s[i]);
		i++;
	}
}


/* Keyword function for "help" */
void terminal_help() {
	uart_string("The following commands are available\n\r");
	uart_string("The following commands are available\n\r");
	uart_string("The following commands are available\n\r");
	uart_string("The following commands are available\n\r");
	uart_string("The following commands are available\n\r");
}


/* Keyword funtion for "LED1" */ 
void terminal_LED_1_ON() {
	unsigned int arr[4] = {1,0,0,0};
	LED_flip(arr);
}


/* Keyword funtion for "LED2" */ 
void terminal_LED_2_ON() {
	unsigned int arr[4] = {0,1,0,0};
	LED_flip(arr);
}


/* Keyword funtion for "LED3" */ 
void terminal_LED_3_ON() {
	unsigned int arr[4] = {0,0,1,0};
	LED_flip(arr);
}


/* Keyword funtion for "LED4" */ 
void terminal_LED_4_ON() {
	unsigned int arr[4] = {0,0,0,1};
	LED_flip(arr);
}


/* Keyword funtion for "LEDOFF" */ 
void terminal_LED_OFF() {
	LED_SET(0,0,0,0);
}


/* Keyword funtion for "LEDALL" */ 
void terminal_LED_ALL_ON() {
	LED_SET(1,1,1,1);
}


/* Keyword funtion for "status" */ 
void terminal_status() {
	uart_string("\r\nCurrent state       : snoo\n\r");
	uart_string("Current Temperature : \n\r");
	uart_string("Current Program     : \n\r");
}
	

/* Keyword funtion for "LED1" */ 
void terminal_LED_STATE_1() {
	LED_SET(1,0,0,0);
	
}


/* Keyword funtion for "LED2" */ 
void terminal_LED_STATE_2() {
	LED_SET(0,1,0,0);
}


/* Keyword funtion for "LED3" */ 
void terminal_LED_STATE_3() {
	LED_SET(0,0,1,0);
}


/* Keyword funtion for "LED4" */ 
void terminal_LED_STATE_4() {
	LED_SET(0,0,0,1);
}
	

/* Keyword funtion for "clear" */ 
void terminal_clear() {
	for(int i = 0; i < 25; i++)
		uart_string("\r\n");
}


/* Keyword funtion for undefined inputs */ 
void terminal_undefined() {
		uart_TxChar('\r');
		uart_TxChar('\n');
		uart_string("Undefined function.");
	
}


/* Keyword funtion for undefined inputs */ 
void terminal_no_function_found() {
	uart_string("No matching function found. try 'help' for more info. \r\n");
}



 


/* Function to check UART input with keywords */
/*
int checkstring(char string_1[] , char string_2[]) {
	int re_val = 1;
	int length;
	
	for (int length = 0; string_2[length] != '\0'; length++);

	for (int i = 1; i < length; i++) {
		if (string_1[i] != string_2[i-1])
			re_val = 0;
	}
	
	return re_val;
}*/


int checkstring(char string_1[] , char string_2[]) {
	int re_val = 1;
	int length;
	for (int length = 0; string_2[length] != '\0'; length++);
	
	for(int i = length; i > 0; i--) {
		if (string_1[length-i+1] != string_2[length-i])
			re_val = 0;
	}
	return re_val;
}


void UART_chk_for_match(char input_array[]) {
	
	if (checkstring(input_array , LED))
		terminal_LED_1_ON();
	else if (checkstring(input_array , help))
		terminal_help();
	else if (checkstring(input_array , Help))
		terminal_help();
	else if (checkstring(input_array , HELP))
		terminal_help();
	
	else if (! strcmp(input_array , LED_1_ON))
		terminal_LED_1_ON();
	else if (! strcmp(input_array , LED_2_ON))
		terminal_LED_2_ON();
	else if (! strcmp(input_array , LED_3_ON))
		terminal_LED_3_ON();
	else if (! strcmp(input_array , LED_4_ON))
		terminal_LED_4_ON();
	else if (checkstring(input_array , LED_OFF))
		terminal_LED_OFF();
	else if (checkstring(input_array , status))
		terminal_status();
	else if (checkstring(input_array , clear))
		terminal_clear();
	else if (checkstring(input_array , LEDSTATE1))
		terminal_LED_STATE_1();
	else if (checkstring(input_array , LEDSTATE2))
		terminal_LED_STATE_2();
	else if (checkstring(input_array , LEDSTATE3))
		terminal_LED_STATE_3();
	else if (checkstring(input_array , LEDSTATE4))
		terminal_LED_STATE_4();
	else if (checkstring(input_array , disco_str))
		disco_func();
	else if (checkstring(input_array , LED_ALL_ON))
		terminal_LED_ALL_ON();
	else {
		uart_string(input_array);
		terminal_no_function_found();
	}
}

/************************************************
 *	UART Login Function

 *	@Description: Used to identify who's logged in to the system
 *		as well as pass correct permissions to the user, to allow
 * 		for several users with different privileges
 *
*************************************************/


void UART_LOGIN(char input_array[]) {
	
	if (! USERNAME_MATCHED) {
		uart_string("Indtast brugernavn :");
		for (int i = 0; i < USERS; i++) {
			if (! strcmp(input_array , USER_IDS[i]))
				USERNAME_MATCHED = i;
		}
	}
	
	if (USERNAME_MATCHED && ! PASSWORD_MATCHED) {
		uart_string("Indtast password:");
		if (! strcmp(input_array , USER_PASS[USERNAME_MATCHED]))
			PASSWORD_MATCHED = 1;
	}
	
	if (USERNAME_MATCHED && PASSWORD_MATCHED) {
		LOGGED_IN = 1;
		uart_string("\r\nLogged into user: ");
		uart_string(USER_IDS[USERNAME_MATCHED]);
		uart_string("\r\nYour priority level is : ");
		uart_string(USER_LIBRARY[USERNAME_MATCHED][USERNAME_MATCHED]);
		
	}
}


void read_username() {
	
}

void read_password() {
	
}





void uart_task() {
	
	static char last_char;
	static char input_buffer[128];
	static unsigned int i = 0;
	char temp_str[5];
	
	
	last_char = uart_RxChar();
	input_buffer[i] = last_char;
	i++;
	uart_TxChar(last_char);
	
//	if (input_buffer[i] == 'r') {
//		
//	if (! LOGGED_IN && ! USERNAME_MATCHED)
//		read_username();
//	else if (! LOGGED_IN && USERNAME_MATCHED)
//		read_password();
//	
//	}
//		
//		if (! LOGGED_IN) {
//			for (int n = 0; input_buffer[n] != '\0' && input_buffer[n] != '\r'; n++)
//				temp_str[n] = input_buffer[i-(3-n)];
//			UART_LOGIN(temp_str);
//		}
//			
//		else if ( LOGGED_IN) {
//			LED_SET(1,1,0,0);
//			UART_chk_for_match(input_buffer);
//		}
//		
//		last_char = ' ';
//		i = 0;
//	}

//	input_buffer[i] = last_char;
//	uart_TxChar(input_buffer[i]);
//	i++;
}










