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
 *********************************************************************************************/
 
 
// ****************************************************************************************
//
//					Constants
//
// ****************************************************************************************
#define CHAR_a	'a'			
#define CHAR_A	'A'
#define CHAR_b	'b'			
#define CHAR_B	'B'
#define CHAR_c	'c'			
#define CHAR_C	'C'
#define CHAR_d	'd'			
#define CHAR_D	'D'
#define CHAR_e	'e'			
#define CHAR_E	'E'
#define CHAR_f	'f'			
#define CHAR_F	'F'
#define CHAR_g	'g'			
#define CHAR_G	'G'
#define CHAR_h	'h'			
#define CHAR_H	'H'
#define CHAR_i	'i'			
#define CHAR_I	'I'
#define CHAR_j	'j'			
#define CHAR_J	'J'
#define CHAR_k	'k'			
#define CHAR_K	'K'
#define CHAR_l	'l'			
#define CHAR_L	'L'
#define CHAR_m	'm'			
#define CHAR_M	'M'
#define CHAR_n	'n'			
#define CHAR_N	'N'
#define CHAR_o	'o'			
#define CHAR_O	'O'
#define CHAR_p	'p'			
#define CHAR_P	'P'
#define CHAR_q	'q'			
#define CHAR_Q	'Q'
#define CHAR_r	'r'			
#define CHAR_R	'R'
#define CHAR_s	's'			
#define CHAR_S	'S'
#define CHAR_t	't'			
#define CHAR_T	'T'
#define CHAR_u	'u'			
#define CHAR_U	'U'
#define CHAR_v	'v'			
#define CHAR_V	'V'
#define CHAR_w	'w'			
#define CHAR_W	'W'
#define CHAR_x	'x'			
#define CHAR_X	'X'
#define CHAR_y	'y'			
#define CHAR_Y	'Y'
#define CHAR_z	'z'			
#define CHAR_Z	'Z'

#define CHAR_ENTER	'\r'

#define SBIT_WordLenght    	0x00u
#define SBIT_DLAB          	0x07u
#define SBIT_FIFO          	0x00u
#define SBIT_RxFIFO        	0x01u
#define SBIT_TxFIFO        	0x02u
#define SBIT_RDR           	0x00u
#define SBIT_THRE          	0x05u
#define NO_OF_KEYWORDS 				 20

#define USERS										5
 
// ****************************************************************************************
//
//					Variables
//
// ****************************************************************************************


// ****************************************************************************************
//	Type		: 	YES_RETURN Functions
//	Example		:	yXxXxX();
//	Description	:	Returns true (1) or false (0) depending on the success of the function
// ****************************************************************************************
extern uint8_t yKeyHit( uint8_t KEY_CHECK , uint8_t KEY_HIT );

// ****************************************************************************************
//	Type		: 	NO_RETURN Functions
//	Example		:	nXxXxX();
//	Description	:	Does not return anything.
// ****************************************************************************************
extern void nNewLine							( void );

extern void nUART0_init						( unsigned int baudrate);

extern void nUART_TxChar					( char ch );

extern char nUART_RxChar					( void );

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

extern void tUART_Task						( void );

// ****************************************************************************************
//	Type		: 	VALUE_RETURN Functions
//	Example		:	vXxXxX();
//	Description	:	Returns a value, either 1 or 0. no confirmation if successful or not.
// ****************************************************************************************
extern uint8_t vFindStringMatch(char InputString[] , uint8_t length);

extern uint8_t vCheckUsernames(char InputString[] , uint8_t length);

extern uint8_t vCheckPasscode(char InputString[] , uint8_t length);
