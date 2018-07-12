/*============================================================================================
 * Source file : ErrorHandler.c
 * Author : Frederik Snedevind
 *		    Frs@Jeros.com
 *
 * Company: Jeros A/S
 *		   HTTP://www.Jeros.com/
 *
 * Date created : 20/6 - 2018
 =============================================================================================
 * Description:
 * 	File containing various alarms for the Jeros Utensil Washer MK5
 * 	
 =============================================================================================*/


/* ==========================================================
   
						Libraries
  
  ========================================================== */
#include "ErrorHandler.h"
#include "stdio.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* ==========================================================
   
						Constants
  
  ========================================================== */
xQueueHandle ErrorHandlerQ			= NULL;


/* ==========================================================
   
						Variables
  
  ========================================================== */
uint16_t ErrorCounter = 0;



/* ========================================================================================
  	Type		: 	VALUE_RETURN Functions
  	Example		:	vXxXxX();
  	Description	:	Returns a value, either 1 or 0. no confirmation if successful or not.
   ========================================================================================*/
uint16_t vGetErrorCounter()
{
	return ErrorCounter;
}

/* ========================================================================================
  	Type		: 	BINARY_RETURN Functions
  	Example		:	bXxXxX();
  	Description	:	Returns true (1) or false (0) depending on the success of the function
   ========================================================================================*/
uint8_t bResetErrorCounter()
{
	ErrorCounter = 0;
	
	return !ErrorCounter;
}

/* ========================================================================================
	Type		: 	Task Functions
	Example		:	tXxXxX();
	Description	:	Task designed to run on the FreeRTOS Kernel
   ========================================================================================*/
void tErrorHandler(void *param)
{
	enum ErrorHandlerTaskStates
	{
		IDLE_STATE,
		ACTIVE_STATE,
		STOP_ALL_STATE
	};
	
	uint8_t ErrorHandlerState = IDLE_STATE;
	uint8_t receive;
	
	switch (ErrorHandlerState)
	{
	case IDLE_STATE:
		
		break;
	
	case ACTIVE_STATE:
		if (xQueueReceive(ErrorHandlerQ, &receive, 10))
		{
			switch (receive)
			{
			case 0:
			break;
				
			default:
			break;
				
			}
			
		}


		if (SensorStatus[i] == ACTIVE)
		{
			if (SensorValue[i] > SensorMaxValues[i])
				SensorMaxBroken[i] = 1;
			else if (SensorValue[i] < SensorMinValues[i])
				SensorMinBroken[i] = 1;
		}



		break;
		
	case STOP_ALL_STATE:
		break;
	}
}


