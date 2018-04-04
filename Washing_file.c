/**********************************************************************************************
 * Source file : Washing_file.c
 * Author : Frederik Snedevind
 *		    Frs@Jeros.com
 *
 * Company: Jeros A/S
 *		   HTTP://www.Jeros.com/
 *
 * Date created : 4/4 - 2018
 * Version			 : 001
 * Revised			 : D/M - YYYY 
 **********************************************************************************************
 * Description: File containing the task for washing programs
 *  This contains the task that will be called once a washing program is asked to start.
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
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"


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
void tWashing_Task( void *param ) {
	
	static uint8_t WASHING_STATE;
	
	while(1) {
		switch(WASHING_STATE) {
			case 1 :
				break;
			
			case 2 :
				break;
			
			default :
				break;
			
			
		}





		vTaskDelay(10);
	}
}

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