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
 
 
// ****************************************************************************************
//
//					Constants
//
// ****************************************************************************************
#define ADC_UNITS			2
#define ADC_CHANNELS	20
#define ADC_CAPACITY	ADC_UNITS * ADC_CHANNELS



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
extern uint8_t vSetupADC(uint8_t PORT , uint8_t PIN , uint32_t FREQ);

extern uint8_t vReadADC(uint8_t ADCHandle);

