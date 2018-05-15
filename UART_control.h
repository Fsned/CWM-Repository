/**********************************************************************************************
 * Header file : UART_control.h
 * Author : Frederik Snedevind
 *		    Frs@Jeros.com
 *
 * Company: Jeros A/S
 *		   HTTP://www.Jeros.com/
 *
 * Date created : 8/3 - 2018 
 **********************************************************************************************
 * Description:
 * 	File used to provide UART Functionality & API to the rest of the program.
 *  Includes Setup of UART0, with wanted Baudrate as argument, Login function, user library,
 *	Keyword library etc.
 *
 * For further documentation please check the corresponding .c-file
 *********************************************************************************************/
 #ifndef UART_CONTROL_H
 #define UART_CONTROL_H
 
 #include "FreeRTOS.h"
 #include "queue.h"
 
 
// ****************************************************************************************
//
//					Constants
//
// ****************************************************************************************

enum UART_Chars {
CHAR_a = 'a',			
CHAR_A = 'A',
CHAR_b = 'b',		
CHAR_B = 'B',
CHAR_c = 'c',		
CHAR_C = 'C',
CHAR_d = 'd',		
CHAR_D = 'D',
CHAR_e = 'e',		
CHAR_E = 'E',
CHAR_f = 'f',		
CHAR_F = 'F',
CHAR_g = 'g',		
CHAR_G = 'G',
CHAR_h = 'h',		
CHAR_H = 'H',
CHAR_i = 'i',		
CHAR_I = 'I',
CHAR_j = 'j',		
CHAR_J = 'J',
CHAR_k = 'k',		
CHAR_K = 'K',
CHAR_l = 'l',		
CHAR_L = 'L',
CHAR_m = 'm',		
CHAR_M = 'M',
CHAR_n = 'n',		
CHAR_N = 'N',
CHAR_o = 'o',		
CHAR_O = 'O',
CHAR_p = 'p',		
CHAR_P = 'P',
CHAR_q = 'q',		
CHAR_Q = 'Q',
CHAR_r = 'r',		
CHAR_R = 'R',
CHAR_s = 's',		
CHAR_S = 'S',
CHAR_t = 't',			
CHAR_T = 'T',
CHAR_u = 'u',		
CHAR_U = 'U',
CHAR_v = 'v',		
CHAR_V = 'V',
CHAR_w = 'w',		
CHAR_W = 'W',
CHAR_x = 'x',		
CHAR_X = 'X',
CHAR_y = 'y',		
CHAR_Y = 'Y',
CHAR_z = 'z',		
CHAR_Z = 'Z',

CHAR_ENTER = '\r',
CHAR_BACKSPACE = 0x7F
};

//#define CHAR_a	'a'			
//#define CHAR_A	'A'
//#define CHAR_b	'b'			
//#define CHAR_B	'B'
//#define CHAR_c	'c'			
//#define CHAR_C	'C'
//#define CHAR_d	'd'			
//#define CHAR_D	'D'
//#define CHAR_e	'e'			
//#define CHAR_E	'E'
//#define CHAR_f	'f'			
//#define CHAR_F	'F'
//#define CHAR_g	'g'			
//#define CHAR_G	'G'
//#define CHAR_h	'h'			
//#define CHAR_H	'H'
//#define CHAR_i	'i'			
//#define CHAR_I	'I'
//#define CHAR_j	'j'			
//#define CHAR_J	'J'
//#define CHAR_k	'k'			
//#define CHAR_K	'K'
//#define CHAR_l	'l'			
//#define CHAR_L	'L'
//#define CHAR_m	'm'			
//#define CHAR_M	'M'
//#define CHAR_n	'n'			
//#define CHAR_N	'N'
//#define CHAR_o	'o'			
//#define CHAR_O	'O'
//#define CHAR_p	'p'			
//#define CHAR_P	'P'
//#define CHAR_q	'q'			
//#define CHAR_Q	'Q'
//#define CHAR_r	'r'			
//#define CHAR_R	'R'
//#define CHAR_s	's'			
//#define CHAR_S	'S'
//#define CHAR_t	't'			
//#define CHAR_T	'T'
//#define CHAR_u	'u'			
//#define CHAR_U	'U'
//#define CHAR_v	'v'			
//#define CHAR_V	'V'
//#define CHAR_w	'w'			
//#define CHAR_W	'W'
//#define CHAR_x	'x'			
//#define CHAR_X	'X'
//#define CHAR_y	'y'			
//#define CHAR_Y	'Y'
//#define CHAR_z	'z'			
//#define CHAR_Z	'Z'

//#define CHAR_ENTER	'\r'
//#define CHAR_BACKSPACE 0x7F


#define SBIT_WordLenght    	0x00u
#define SBIT_DLAB          	0x07u
#define SBIT_FIFO          	0x00u
#define SBIT_RxFIFO        	0x01u
#define SBIT_TxFIFO        	0x02u
#define SBIT_RDR           	0x00u
#define SBIT_THRE          	0x05u
#define NO_OF_KEYWORDS 				 25

#define USERS										5


#define UartState_FindUser			1
#define UartState_FindPass			2
#define	UartState_Functioncall	3


#define ENTER_USERNAME					255
#define ENTER_PASSWORD					254

 
// ****************************************************************************************
//
//					Variables
//
// ****************************************************************************************
extern xQueueHandle qUART_RxQ;
extern xQueueHandle qUART_TxQ;

// ****************************************************************************************
//	Type		: 	YES_RETURN Functions
//	Example		:	yXxXxX();
//	Description	:	Returns true (1) or false (0) depending on the success of the function
// ****************************************************************************************
extern uint8_t yKeyHit( uint8_t KEY_CHECK , uint8_t KEY_HIT );

extern uint8_t yUART_TxReady( void );

extern uint8_t yUART_RxReady( void );

// ****************************************************************************************
//	Type		: 	NO_RETURN Functions
//	Example		:	nXxXxX();
//	Description	:	Does not return anything.
// ****************************************************************************************
extern void nNewLine							( uint8_t NumberOfLineSkips );

extern void nUART0_init						( unsigned int baudrate);

extern void nUART_TxChar					( char ch );

extern void nUART_RxChar					( void );

extern void nUART_TxString				( char ch_s[] );

extern void nTerminalHelp					( void );

extern void nTerminal_LED_1_ON		( void );

extern void nTerminal_LED_2_ON		( void );

extern void nTerminal_LED_3_ON		( void );

extern void nTerminal_LED_4_ON		( void );

extern void nTerminal_LED_OFF 		( void );

extern void nTerminal_LED_ALL_ON	( void );

extern void nTerminalStatus				( void );

extern void nTerminalClear				( void );

extern void nTerminalUndefined		( void ); 

extern void nTerminalNoFunctionFound( void );

extern void nTerminalLogout				( void );

extern void tUART_Task						( void* );

extern void tUART_RxTask					( void* );

extern void tUART_TxTask					( void* );

extern void nPinSetup_1						( void );

extern void nPinSetup_2						( void );

extern void nPinFlip_1						( void );
	
extern void nPinFlip_2 						( void );

extern void nADC_Status 					( void );

extern void nPinFlip_P20					( void );
extern void nPinFlip_P21					( void );
extern void nPinFlip_P22					( void );
extern void nPinFlip_P23					( void );
extern void nPinFlip_P24					( void );
extern void nPinFlip_P25					( void );
extern void nPinFlip_P26					( void );
extern void nPinFlip_P27					( void );
extern void nPinFlip_P28					( void );

extern void nPrintInteger					( int input );

// ****************************************************************************************
//	Type		: 	VALUE_RETURN Functions
//	Example		:	vXxXxX();
//	Description	:	Returns a value, either 1 or 0. no confirmation if successful or not.
// ****************************************************************************************
extern uint8_t vFindStringMatch	( char InputString[] , uint8_t length );

extern uint8_t vCheckUsernames	( char InputString[] , uint8_t length );

extern uint8_t vCheckPasscode	( char InputString[] , uint8_t length );

#endif /* UART_CONTROL_H */

