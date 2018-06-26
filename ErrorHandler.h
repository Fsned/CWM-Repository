/*=============================================================================================
 * Header file : ErrorHandler.h
 * Author : Frederik Snedevind
 *		    Frs@Jeros.com
 *
 * Company: Jeros A/S
 *		   HTTP://www.Jeros.com/
 *
 * Date created : 20/6 - 2018 
 =============================================================================================
 * Description:
 *  Header  file  belonging to  the source  file  ErrorHandler.c
 *	Provides  every single  alarm and  error  recognizable  by  the 
 *	system, along with strings to write them out to the UART or HMI
 =============================================================================================*/



#ifndef _ERRORHANDLER_H
#define _ERRORHANDLER_H

#include "FreeRTOS.h"
#include "queue.h"


extern xQueueHandle ErrorHandlerQ;

enum Alarms
{
	TemperatureSensorWashDisconnected,
	TemperatureSensorRinseDisconnected,
	ThermalCutoutWash1Tripped,
	ThermalCutoutWash2Tripped,
	ThermalCutoutRinseTripped,
	WashframeMotorMalfunction,
	LidContactMalfunction,
	LidHoistTravelTimeExceeded,
	OverheatWash85Deg,
	OverheatRinse97Deg,
	FillingTimeRinseExceeded,
	PhaseFault,
//	WatchdogResetActivated,
	
	NumberOfAlarms
};


enum Errors
{
	
	NumberOfErrors
};
/* ========================================================================================
  	Type		: 	VALUE_RETURN Functions
  	Example		:	vXxXxX();
  	Description	:	Returns a value, based on the function type. typically of up to 8 or 16 bits value
   ========================================================================================*/
uint16_t vGetErrorCounter( void );

/* ========================================================================================
  	Type		: 	BINARY_RETURN Functions
  	Example		:	bXxXxX();
  	Description	:	Returns true (1) or false (0) depending on the success of the function
   ========================================================================================*/
uint8_t bResetErrorCounter( void );

/* ========================================================================================
	Type		: 	Task Functions
	Example		:	tXxXxX();
	Description	:	Task designed to run on the FreeRTOS Kernel
   ========================================================================================*/
void tErrorHandler(void *param);

#endif /* _ERRORHANDLER_H */
