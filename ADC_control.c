/**********************************************************************************************
 * Source file : ADC_control.c
 * Author : Frederik Snedevind
 *		    Frs@Jeros.com
 *
 * Company: Jeros A/S
 *		   HTTP://www.Jeros.com/
 *
 * Date created : 21/3 - 2018
 * Version			 : 001
 * Revised			 : D/M - YYYY 
 **********************************************************************************************
 * Description:
 * 	ADC control, handling setup, setdown, reading, storing, handling and managing of ADC,
 *  ADC data, ADC sampling rate, number of ADC's, ADC -channel and -unit management,
 *	reporting and controlling ADC errors.
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
#include "ADC_control.h"
#include "LED_control.h"
#include "GPIO_setup.h"


// ****************************************************************************************
//
//					Variables
//
// ****************************************************************************************
uint8_t AnalogInputs = 0;
uint8_t ADC_Library[ADC_CAPACITY];

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
uint8_t vReadADC(uint8_t ADCHandle) {
	uint8_t vReadADC_ret = 0;
	
	vReadADC_ret = ADC_Library[ADCHandle];
	
	return vReadADC_ret;
}





