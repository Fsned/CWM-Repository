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
#define ADC_UNITS				1
#define ADC_CHANNELS		8
#define ADC_CAPACITY	ADC_UNITS * ADC_CHANNELS

#define SBIT_BURST      16u
#define SBIT_START      24u
#define SBIT_PDN        21u
#define SBIT_EDGE       27u 
#define SBIT_DONE       31u
#define SBIT_RESULT     4u
#define SBIT_CLCKDIV    8u

#define ADC_VACANT				0
#define ADC_PAUSE					1
#define ADC_HOLD					2
#define ADC_ACTIVE				3

#define ADC_NO_DATA 		0x1000
#define ADC_PAUSED			0x2000
#define ADC_NO_ACCESS 	0x3000



// ****************************************************************************************
//	Type		: 	YES_RETURN Functions
//	Example		:	yXxXxX();
//	Description	:	Returns true (1) or false (0) depending on the success of the function
// ****************************************************************************************
uint8_t yADC_ChangeStatus( uint8_t ADCHandle);

// ****************************************************************************************
//	Type		: 	NO_RETURN Functions
//	Example		:	nXxXxX();
//	Description	:	Does not return anything.
// ****************************************************************************************
extern void nInitialize_ADC_Library( void );

// ****************************************************************************************
//	Type		: 	VALUE_RETURN Functions
//	Example		:	vXxXxX();
//	Description	:	Returns a value, no confirmation if successful or not. can be a handle for e.g. ADC pin
// ****************************************************************************************
extern uint8_t vSetupADC( void );

extern uint16_t vReadADC(uint8_t ADCHandle);

extern void tADC_Task( void );
