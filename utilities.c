/**********************************************************************************************
 * Source file : 
 * Author : Frederik Snedevind
 *		    Frs@Jeros.com
 *
 * Company: Jeros A/S
 *		   HTTP://www.Jeros.com/
 *
 * Date created : D/M - YYYY 
 * Version			 : X.Y.Z
 * Revised			 : D/M - YYYY 
 **********************************************************************************************
 * Description:
 * 
 *  
 *	
 *
 **********************************************************************************************
 * Dependencies:
 *	This library uses the following files:
 *	
 *	
 *	
 * 
 **********************************************************************************************/

// ****************************************************************************************
//
//					Libraries
//
// ****************************************************************************************
#include "stdutils.h"
#include "stdio.h"

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


// ****************************************************************************************
//	Type		: 	NO_RETURN Functions
//	Example		:	nXxXxX();
//	Description	:	Does not return anything.
// ****************************************************************************************


// ****************************************************************************************
//	Type		: 	VALUE_RETURN Functions
//	Example		:	vXxXxX();
//	Description	:	Returns a value, no confirmation if successful or not. can be a handle for e.g. ADC pin
// ****************************************************************************************


// ****************************************************************************************
// 																Undocumented functions 
// ****************************************************************************************
void delay_ms(unsigned int n) {
/* ******************************************************************
//	Function name : delay_ms
//	Functionality :	Used to create a delay, with the amount of milliseconds as only input argument. do note this is done with a while(), preventing any proper scheduling
// 	Returns				:	None
//  Input range		: 0 - uint_range
// *****************************************************************/		
	uint32_t o = n*12000*2;
	while(--o);
}

void delay_us(unsigned int n) {
/* ******************************************************************
//	Function name : delay_us
//	Functionality :	Used to create a delay, with the amount of microseconds as only input argument. do note this is done with a while(), preventing any proper scheduling
// 	Returns				:	None
//  Input range		: 0 - uint_range
// *****************************************************************/
	unsigned int o = n*12*2;
	while(--o);
}









//int int_to_char_1000(int input) {
///* ******************************************************************
////	Function name : int_to_char_1000
////	Functionality :	Returns the amount of 1000's in the number, used to various UART contexts.
//// 	Returns				:	uint8_t with amount of 1000's in the input (e.g. 9000 as input, returns 9).
////  Input range		: 0 - int_range
//// *****************************************************************/
//	int output = 0;
//	while(input >= 1000) {
//		input -= 1;
//		output += 1;
//	}
//	return output;
//}

//int int_to_char_100(int input) {
///* ******************************************************************
////	Function name : int_to_char_100
////	Functionality :	Returns the amount of 100's in the number, used to various UART contexts.
//// 	Returns				:	uint8_t with amount of 100's in the input (e.g. 900 as input, returns 9).
////  Input range		: 0 - int_range
//// *****************************************************************/
//	int output = 0;
//	while(input >= 100) {
//		input -= 1;
//		output += 1;
//	}
//	return output;
//}

//int int_to_char_10(int input) {
///* ******************************************************************
////	Function name : int_to_char_10
////	Functionality :	Returns the amount of 10's in the number, used to various UART contexts.
//// 	Returns				:	uint8_t with amount of 10's in the input (e.g. 90 as input, returns 9).
////  Input range		: 0 - int_range
//// *****************************************************************/
//	int output = 0;
//	while(input >= 10) {
//		input -= 1;
//		output += 1;
//	}
//	return output;
//}

//int int_to_char_1(int input) {
///* ******************************************************************
////	Function name : int_to_char_1
////	Functionality :	Returns the amount of 1's in the number, used to various UART contexts.
//// 	Returns				:	uint8_t with amount of 10's in the input (e.g. 9 as input, returns 9).
////  Input range		: 0 - int_range
//// *****************************************************************/
//	int output = 0;
//	while(input >= 1) {
//		input -= 1;
//		output += 1;
//	}
//	return output;
//}


