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
#include "lpc17xx.h"
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
uint8_t ADC_Library_Initialized = 0;

// ****************************************************************************************
//	Type		: 	YES_RETURN Functions
//	Example		:	yXxXxX();
//	Description	:	Returns true (1) or false (0) depending on the success of the function
// ****************************************************************************************
uint8_t yPauseADC(uint8_t ADCHandle) {
	
}

uint8_t yResumeADC(uint8_t ADCHandle) {
	
}

uint8_t ySetdownADC(uint8_t ADCHandle) {
	
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
uint8_t vReadADC(uint8_t ADCHandle) {
	uint8_t vReadADC_ret = 0;
	
	vReadADC_ret = ADC_Library[ADCHandle];
	
	return vReadADC_ret;
}


uint8_t vSetupADC(uint8_t PORT , uint8_t PIN , uint32_t FREQ) {
	
	if (! ADC_Library_Initialized)
		nInitialize_ADC_Library();
	
	
	
	
	LPC_SC->PCONP |= (1 << 12);
	LPC_ADC->ADCR = ( (1 << SBIT_PDN) | (10 << SBIT_CLCKDIV ));
	LPC_PINCON->PINSEL1 |= 0x01 << 14;
}

uint8_t vSetupADC_Avg(uint8_t PORT , uint8_t PIN , uint32_t FREQ , uint8_t Resolution) {
	
}

void nInitialize_ADC_Library() {

		for (int n = 0; n < ADC_CHANNELS; n++)
			ADC_Library[n] = ADC_FREE;
	
	
}









