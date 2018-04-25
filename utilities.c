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
	unsigned int o = n*12000*2;
	while(--o);
}

void delay_us(unsigned int n) {
	unsigned int o = n*12*2;
	while(--o);
}









int int_to_char_1000(int input) {
	
	int output = 0;
	while(input >= 1000) {
		input -= 1;
		output += 1;
	}
	return output;
}

int int_to_char_100(int input) {
	int output = 0;
	while(input >= 100) {
		input -= 1;
		output += 1;
	}
	return output;
}

int int_to_char_10(int input) {
	int output = 0;
	while(input >= 10) {
		input -= 1;
		output += 1;
	}
	return output;
}

int int_to_char_1(int input) {
	int output = 0;
	while(input >= 1) {
		input -= 1;
		output += 1;
	}
	return output;
}


