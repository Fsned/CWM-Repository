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

/* ==========================================================
   
						Constants
  
  ========================================================== */



/* ==========================================================
   
						Functions
  
  ========================================================== */
void tErrorHandler(void *param)
{
	enum ErrorHandlerTaskStates
	{
		IDLE_STATE,
		ACTIVE_STATE,
		STOP_ALL_STATE
	};
	
	uint8_t ErrorHandlerState = IDLE_STATE;
	
	
	switch (ErrorHandlerState)
	{
	case IDLE_STATE:
		break;
	
	case ACTIVE_STATE:
		break;
		
	case STOP_ALL_STATE:
		break;
	}
}


