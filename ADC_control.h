/**********************************************************************************************
 * Header file : ADC_control.h
 * Author : Frederik Snedevind
 *		    Frs@Jeros.com
 *
 * Company: Jeros A/S
 *		   HTTP://www.Jeros.com/
 *
 * Date created : 21/3 - 2018 
 **********************************************************************************************
 * Description:
 * 	Header file providing ADC API to the rest of the program
 *  
 *	
 *
 *********************************************************************************************/
 #include "stdutils.h"
 
// ****************************************************************************************
//
//					Constants
//
// ****************************************************************************************
#define ADC_UNITS		1
#define ADC_CHANNELS	8
#define ADC_CAPACITY	ADC_UNITS * ADC_CHANNELS

#define SBIT_BURST      16u
#define SBIT_START      24u
#define SBIT_PDN        21u
#define SBIT_EDGE       27u 
#define SBIT_DONE       31u
#define SBIT_RESULT     4u
#define SBIT_CLCKDIV    8u

#define ADC_FREE		0
#define ADC_

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
extern void nInitialize_ADC_Library();

// ****************************************************************************************
//	Type		: 	VALUE_RETURN Functions
//	Example		:	vXxXxX();
//	Description	:	Returns a value, no confirmation if successful or not. can be a handle for e.g. ADC pin
// ****************************************************************************************
extern uint8_t vSetupADC(uint8_t PORT , uint8_t PIN , uint32_t FREQ);

extern uint8_t vReadADC(uint8_t ADCHandle);

