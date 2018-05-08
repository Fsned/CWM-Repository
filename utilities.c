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
