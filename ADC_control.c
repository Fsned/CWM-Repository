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
#include "utilities.h"

#include "FreeRTOS.h"
#include "task.h"



// ****************************************************************************************
//
//					Variables
//
// ****************************************************************************************
uint8_t AnalogInputs = 0;
uint8_t ADC_PinLibrary[8];
uint8_t ADC_Library_Initialized = 0;
uint16_t ADC_DataLibrary[8];

// ****************************************************************************************
//	Type		: 	YES_RETURN Functions
//	Example		:	yXxXxX();
//	Description	:	Returns true (1) or false (0) depending on the success of the function
// ****************************************************************************************
uint8_t yPauseADC(uint8_t ADCHandle) {
/* ******************************************************************
//	Function name : yPauseADC
//	Functionality :	Function to pause a specific ADC channel. Keeping it setup and keeping last collected dataset in the ADC datalibrary
// 	Returns				:	True or false depending on the success	
//  Input range		: ADCHandle
// *****************************************************************/
	return 0;
}

uint8_t yResumeADC(uint8_t ADCHandle) {
/* ******************************************************************
//	Function name : yResumeADC
//	Functionality :	Resumes an ADC channel,
// 	Returns				:	True or false depending on the success. requires channel to be SETUP and PAUSED in order to succeed. will return false otherwise
//  Input range		: ADCHandle gotten from vSetupADC();
// *****************************************************************/
	return 0;
}

uint8_t ySetdownADC(uint8_t ADCHandle) {
/* ******************************************************************
//	Function name : ySetdownADC
//	Functionality :	Eliminates an ADC channel
// 	Returns				:	True or false depending on success. channel has to be SETUP and either PAUSED or RUNNING to succeed
//  Input range		: ADCHandle gotten from vSetupADC();
// *****************************************************************/
	return 0;
}

uint8_t yADC_ChangeStatus( uint8_t ADCHandle) {
/* ******************************************************************
//	Function name : yADC_ChangeStatus
//	Should this function be here at all?
// *****************************************************************/
	return 0;
}

// ****************************************************************************************
//	Type		: 	NO_RETURN Functions
//	Example		:	nXxXxX();
//	Description	:	Does not return anything.
// ****************************************************************************************
void nInitialize_ADC_Library() {
/* ******************************************************************
//	Function name : nInitialize_ADC_Library
//	Functionality :	Creates a library for each ADC channel, and sets them all to VACANT. is necessary for further use of the ADC library
// 	Returns				:	Nothing. internal calling
//  Input range		: None
// *****************************************************************/
		for (int n = 0; n < 8; n++) {
			ADC_PinLibrary[n] = ADC_VACANT;
			ADC_DataLibrary[n] = 0; 
		}
	ADC_Library_Initialized = 1;
}

// ****************************************************************************************
//	Type		: 	VALUE_RETURN Functions
//	Example		:	vXxXxX();
//	Description	:	Returns a value, no confirmation if successful or not. can be a handle for e.g. ADC pin
// ****************************************************************************************


uint16_t vReadADC(uint8_t ADCHandle) {
/* ******************************************************************
//	Function name : vReadADC
//	Functionality :	Reads latest data from an ADC channel in the ADC_DataLibrary
// 	Returns				:	Latest datavalue if ADC pin is paused or running, 0 otherwise
//  Input range		: ADC Handle
// *****************************************************************/	
	uint16_t vReadADC_ret = 0;
	
//	ADC_DataLibrary[0] = 11;
	if (ADC_PinLibrary[ADCHandle] >= ADC_PAUSE)
		vReadADC_ret = ADC_DataLibrary[ADCHandle];
//	else
//		vReadADC_ret = 0xF000; 
	
	return vReadADC_ret;
}


uint8_t vSetupADC() {
/* ******************************************************************
//	Function name : vSetupADC
//	Functionality :	Sets up first possible ADC channel. initializes ADC library if it hasn't been before.
// 	Returns				:	0 to 7, Depending on the firstcome vacant ADC channel (starts from 0), vSetdownADC() returns the given channel to ADC_Vacant. 
			The returned value is the unique Handle for that specific ADC channel, used when reading the ADC channel, so it should be saved in a variable
//  Input range		: None
// *****************************************************************/	
	uint8_t vSetupADC_ret;
	
	// Check if library has been initialized
	if (! ADC_Library_Initialized)
		nInitialize_ADC_Library();
	
	// Run through library, check for available channels
	int i = 0;
	for (i = 0; i < 8; i++) {
		if (ADC_PinLibrary[i] == ADC_VACANT) {
			vSetupADC_ret = i;
			ADC_PinLibrary[i] = ADC_ACTIVE;
			i = 8;
			break;
		}
	}
	
	
	// Turn off entire ADC before setting up
//	LPC_ADC->ADCR &= ~(1 << 21);
//	delay_ms(1);
	// Enables Clock
	LPC_SC->PCONP |= ( 1 << 12 );				// Enable clock for Internal ADC controller
	delay_ms(1);
	LPC_ADC->ADCR |= (( 1 << 21) | (10 <<SBIT_CLCKDIV ));																				
	// Set power ON
	delay_ms(1);
	// Set the pin up for Secondary function
	
	if (vSetupADC_ret == 0)
		LPC_PINCON->PINSEL1 |= 1 << 14;																// Select P0_23 AD0[0] for ADC Function
	
	else if (vSetupADC_ret == 1)
		LPC_PINCON->PINSEL1 |= 1 << 16;
	
	else if (vSetupADC_ret == 2)
		LPC_PINCON->PINSEL1 |= 1 << 18;
	
	else if (vSetupADC_ret == 3)
		LPC_PINCON->PINSEL1 |= 1 << 20;
	
	else if (vSetupADC_ret == 4)
		LPC_PINCON->PINSEL3 |= 1 << 30;
	
	else if (vSetupADC_ret == 5)
		LPC_PINCON->PINSEL3 |= 0x01 << 30;
	
	else if (vSetupADC_ret == 6)
		LPC_PINCON->PINSEL0 |= 1 << 3;
	
	else if (vSetupADC_ret == 7)
		LPC_PINCON->PINSEL0 |= 1 << 2;
	
	delay_ms(1);
	return vSetupADC_ret;
}

uint8_t vSetupADC_Avg(uint8_t PORT , uint8_t PIN , uint32_t FREQ , uint8_t Resolution) {
	return 0;
	
}

// ****************************************************************************************
//	Type		: 	Task
//	Example		:	tXx();
//	Description	:	Designed for FreeRTOS. runs in a while loop, with *param to pass arguments for vTask*(); calls
// ****************************************************************************************

void tADC_Task(void *param) {
/* ******************************************************************
//	Function name : tADC_Task
//	Functionality :	FreeRTOS Task for the Receiving Part of the UART communication on channel 0
// 	Returns				:	None	
//  Input range		: None
// *****************************************************************/

	
	// Iterate through the Pinlibrary, checking what pins should be updated from the ADC. 
	// only ACTIVE pins get updated data. 
	// paused pins retain their data value until reactivated or setdown.
	while(1) {
		
		for (int i = 0; i < 8; i++) {
			if (ADC_PinLibrary[i] == ADC_ACTIVE) {
				
				// Select Channel to sample from
				LPC_ADC->ADCR |= ( 0x1 << i );
				
				// Enable conversion 'now'.
				LPC_ADC->ADCR |= ( 0x1 << 24 );			
				
				// Wait until conversion is ready, on the 31st bit in the ADGDR register
				
				while (! LPC_ADC->ADGDR & 0x8000000);
//				while ( util_GetBitStatus(LPC_ADC->ADGDR , SBIT_DONE) == 0 );
				
				ADC_DataLibrary[i] = (LPC_ADC->ADGDR >> SBIT_RESULT) & 0xFFF;			// Store result in Data Library.
				
				vTaskDelay(1);
			}
		}
		
		vTaskDelay(10);
	}
}












