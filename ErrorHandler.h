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


void tErrorHandler(void *param);

#endif /* _ERRORHANDLER_H */
