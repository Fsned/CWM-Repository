/*=================================================================
   Author : Frederik Snedevind
  		   Frs@Jeros.com
  
   Company: Jeros A/S
  		   HTTP://www.Jeros.com/
  
   Date created  : 14/3 - 2018 
   Version			 : 0.0.1
   Revised			 : 15/3 - 2018 
 ==================================================================
   Description:
   GPIO_setup.c is used with NXP LPC1768 Processor, to setup
    pins to GPIO function, as well as read/write from/to
  	them.
  
 ==================================================================
   Dependencies:
  	This library uses the following files:
  	"lpc17xx.h"			-		Contains information for Pin manipulation
  	"GPIO_setup.h"	-		Associated header file
  	"stdutils.h"		-		Contains typedef for among many, 'uint8_t' to limit memory usage
   
 ================================================================== */

/* ================================================================  
  
  					Libraries
  
  ================================================================= */ 
#include "stdint.h"
#include "lpc17xx.h"
#include "GPIO_control.h"




// ==================================================================
//
//					Variables
//
// ==================================================================
static uint8_t ADC_SETUP_REGISTER = 0x00;  // Variable to keep track of Which ADC's are initialized

// Array to contain the setup status for 32 pins on each port, (Port 0 - Port 4). 
// Will be initialized to "GPIO_FREE" = 0, once first function in file is called
// Each pin can hold 1 of 3 statuses:
// 		GPIO_FREE
// 		GPIO_INPUT
// 		GPIO_OUTPUT
uint8_t PinLibrary[5][32];

/* ==================================================================
  	Type		: 	BINARY_RETURN Functions
  	Example		:	bXxXx();
  	Description	:	Returns true (1) or false (0) depending on the success of the function
   ================================================================== */
uint8_t bSetupAnalogI(uint8_t ADCNumber)
{
/* ==================================================================
	Function name	: bSetupAnalogI
	Functionality	: Setup a GPIO pin to Analog input
 	Returns			: True (1) or false (0)
    Input range		: 0 - 7, depending on what ADC is wanted
	================================================================== */
	static uint8_t PeriphClock_Initialized = 0; // Variable to keep track of Peripheral clock for ADC units
	
	
	uint8_t bSetupAnalogI_ret = 0;
	
	if (!ADC_SETUP_REGISTER & (1 << ADCNumber))
	{
		ADC_SETUP_REGISTER |= (1 << ADCNumber);
		
		switch (ADCNumber)
		{
			if (!PeriphClock_Initialized)
			{
				LPC_SC->PCONP |= (1 << 12);  				// Enable clock for Internal ADC controller
				LPC_ADC->ADCR |= ((1 << 21) | (10 << SBIT_CLCKDIV));	
			}
			
		case 0:
			LPC_PINCON->PINSEL1 |= 1 << 14;  	/* AD0, P0.23	Sensor 0	P15					*/
			break;									 				  									
													 				  									
		case 1:										 				  									
			LPC_PINCON->PINSEL1 |= 1 << 16;  	/* AD1, P0.24	Sensor 1	P16					*/
			break;									 				  									
													 				  									
		case 2:										 				  									
			LPC_PINCON->PINSEL1 |= 1 << 18;  	/* AD2, P0.25	Sensor 2	P17					*/
			break;									 				  									
													 				  									
		case 3:										 				  									
			LPC_PINCON->PINSEL1 |= 1 << 20;  	/* AD3, P0.26	Sensor 3	P18					*/
			break;									 				  									
													 				  									
		case 4:										 				  									
			LPC_PINCON->PINSEL3 |= 2 << 18;		/* AD4, P1.30	Sensor 4	P19					*/
			break;									 				  									
													 				  									
		case 5:										 				  									
			LPC_PINCON->PINSEL3 |= 2 << 19;		/* AD5, P1.31	Sensor 5	P20					*/
			break;									 				  									
													 				  									
		case 6:										 				  									
			LPC_PINCON->PINSEL0 |= 2 << 2;		/* AD6, P0.3	Sensor 6	NOT ON EVAL BOARD	*/
			break;									 				  									
													 				  									
		case 7:										 				  									
			LPC_PINCON->PINSEL0 |= 2 << 1;		/* AD7, P0.2	Sensor 7	NOT ON EVAL BOARD	*/
			break;
		
		default:
			break;
		}
		return bSetupAnalogI_ret;
	}
}
uint8_t bSetupDigitalIO ( uint8_t Port, int Pin, uint8_t InputOutput) 
{
/* ==================================================================
  	Function name	: bSetupDigitalIO
  	Functionality	: Setup a GPIO pin to digital Input or Output
   	Returns			: True (1) or false (0)
    Input range		: PORT_0 : PORT_4  ,  PIN_0 : PIN_31
   ================================================================== */
	nInitializePinLibrary();
	
	
	uint8_t SetupDigitalIO_ret = 0;
	
	if (PinLibrary[Port][Pin] == GPIO_FREE) 
	{
		switch (Port) {
			case PORT_0:
			
				if ( InputOutput == GPIO_INPUT )
					LPC_GPIO0->FIODIR &= ~(1 << Pin);
				else if ( InputOutput == GPIO_OUTPUT )
					LPC_GPIO0->FIODIR |= (1 << Pin);
					
				if (Pin >= PIN_0 && Pin <= PIN_15) 
				{
					LPC_PINCON->PINSEL0 &= ~(0b11 << Pin*2);
					SetupDigitalIO_ret = 1;
				}
				else if (Pin >= PIN_16 && Pin <= PIN_31) 
				{
					LPC_PINCON->PINSEL1 &= ~(0b11 << (Pin-16)*2);
					SetupDigitalIO_ret = 1;
				}
				else
					SetupDigitalIO_ret = 0;
				break;
			
			case PORT_1:
				
				if ( InputOutput == GPIO_INPUT )
					LPC_GPIO1->FIODIR &= ~(1 << Pin);
				else if ( InputOutput == GPIO_OUTPUT )
					LPC_GPIO1->FIODIR |= (1 << Pin);
					
				if (Pin >= PIN_0 && Pin <= PIN_15) 
				{
					LPC_PINCON->PINSEL2 &= ~(0b11 << Pin*2);
					SetupDigitalIO_ret = 1;
				}
				else if (Pin >= PIN_16 && Pin <= PIN_31) 
				{
					LPC_PINCON->PINSEL3 &= ~(0b11 << (Pin-16)*2);
					SetupDigitalIO_ret = 1;
				}
				else
					SetupDigitalIO_ret = 0;
				break;
			
			case PORT_2:
			
				if ( InputOutput == GPIO_INPUT )
					LPC_GPIO2->FIODIR &= ~(1 << Pin);
				else if ( InputOutput == GPIO_OUTPUT )
					LPC_GPIO2->FIODIR |= (1 << Pin);
					
				if (Pin >= PIN_0 && Pin <= PIN_15) 
				{
					LPC_PINCON->PINSEL4 &= ~(0b11 << Pin*2);
					SetupDigitalIO_ret = 1;
				}
				else if (Pin >= PIN_16 && Pin <= PIN_31) 
				{
					LPC_PINCON->PINSEL5 &= ~(0b11 << (Pin-16)*2);
					SetupDigitalIO_ret = 1;
				}
				else
					SetupDigitalIO_ret = 0;
				break;
			
			case PORT_3:
				
				if ( InputOutput == GPIO_INPUT )
					LPC_GPIO3->FIODIR &= ~(1 << Pin);
				else if ( InputOutput == GPIO_OUTPUT )
					LPC_GPIO3->FIODIR |= (1 << Pin);
					
				if (Pin >= PIN_0 && Pin <= PIN_15) 
				{
					LPC_PINCON->PINSEL6 &= ~(0b11 << Pin*2);
					SetupDigitalIO_ret = 1;
				}
				else if (Pin >= PIN_16 && Pin <= PIN_31) 
				{
					LPC_PINCON->PINSEL7 &= ~(0b11 << (Pin-16)*2);
					SetupDigitalIO_ret = 1;
				}
				else
					SetupDigitalIO_ret = 0;
				break;
			
			case PORT_4:
				
				if ( InputOutput == GPIO_INPUT )
					LPC_GPIO4->FIODIR &= ~(1 << Pin);
				else if ( InputOutput == GPIO_OUTPUT )
					LPC_GPIO4->FIODIR |= (1 << Pin);
					
				if (Pin >= PIN_0 && Pin <= PIN_15) 
				{
					LPC_PINCON->PINSEL8 &= ~(0b11 << Pin*2);
					SetupDigitalIO_ret = 1;
				}
				else if (Pin >= PIN_16 && Pin <= PIN_31) 
				{
					LPC_PINCON->PINSEL9 &= ~(0b11 << (Pin-16)*2);
					SetupDigitalIO_ret = 1;
				}
				else
					SetupDigitalIO_ret = 0;
				break;
			
			default :
				SetupDigitalIO_ret = 0;
		}
	}
	else
		SetupDigitalIO_ret = 0;
	
	if (SetupDigitalIO_ret)
		PinLibrary[Port][Pin] = InputOutput;
	
	return SetupDigitalIO_ret;
}

uint8_t bDigitalWrite( uint8_t Port , int Pin , uint8_t State )
{
/* ==================================================================
  	Function name	: bDigitalWrite
  	Functionality	: Write a value to a digital output
   	Returns			: True (1) or false (0), depending on successful operation or no
    Input range		: PORT_0 : PORT_4  ,  PIN_0 : PIN_31  ,  0 : 1
   ================================================================== */
	// Make sure PinLibrary is initialized before manipulating any bits
	nInitializePinLibrary();
	
	// Init return variable to 0, to only return TRUE if operation is succesful
	uint8_t bDigitalWrite_ret = 0;
	
	// Check if Pin is set to output before writing
	if ( PinLibrary[Port][Pin] == GPIO_OUTPUT ) 
	{
		switch(State) {
			case GPIO_HIGH:
				switch(Port) 
				{
					case PORT_0:
						LPC_GPIO0->FIOPIN |= (State << Pin);
						bDigitalWrite_ret = 1;
					break;
			
					case PORT_1:
						LPC_GPIO1->FIOPIN |= (State << Pin);
						bDigitalWrite_ret = 1;
					break;
			
					case PORT_2:
						LPC_GPIO2->FIOPIN |= (State << Pin);
						bDigitalWrite_ret = 1;
					break;
			
					case PORT_3:
						LPC_GPIO3->FIOPIN |= (State << Pin);
						bDigitalWrite_ret = 1;
					break;
			
					case PORT_4:
						LPC_GPIO4->FIOPIN |= (State << Pin);
						bDigitalWrite_ret = 1;
					break;
			
					default: 
						bDigitalWrite_ret = 0;
					break;
				}
				break;
				
			case GPIO_LOW:
				switch(Port) 
				{
					case PORT_0:
						LPC_GPIO0->FIOPIN &= ~(1 << Pin);
						bDigitalWrite_ret = 1;
					break;
			
					case PORT_1:
						LPC_GPIO1->FIOPIN &= ~(1 << Pin);
						bDigitalWrite_ret = 1;
					break;
			
					case PORT_2:
						LPC_GPIO2->FIOPIN &= ~(1 << Pin);
						bDigitalWrite_ret = 1;
					break;
			
					case PORT_3:
						LPC_GPIO3->FIOPIN &= ~(1 << Pin);
						bDigitalWrite_ret = 1;
					break;
			
					case PORT_4:
						LPC_GPIO4->FIOPIN &= ~(1 << Pin);
						bDigitalWrite_ret = 1;
					break;
			
					default:
						bDigitalWrite_ret = 0;
					break;
				}
				break;
			
			default:
				bDigitalWrite_ret = 0;
				break;
		}
	}
	else
		bDigitalWrite_ret = 0;
	
	return bDigitalWrite_ret;
}



/* ================================================================== 
  	Type		: 	NO_RETURN Functions
  	Example		:	nX();
  	Description	:	Does not return anything.
   ================================================================== */

void nGPIO_STOP() 
{
/* ==================================================================
  	Function name	: nGPIO_STOP
  	Functionality	: Sets all GPIO that has been setup to LOW. note that any functions that does not take use of the GPIO API for setting up pins might be excluded from this feature.
   	Returns			: None	
    Input range		: None
   ================================================================== */
	for (uint8_t i = PORT_0; i <= PORT_4; i++) 
	{
		for (uint8_t n = PIN_0; n <= PIN_31; n++) 
		{
			if ( PinLibrary[i][n] == GPIO_OUTPUT ) 
			{
				bDigitalWrite(/*PORT*/ i , /*PIN*/ n , /* STATE */ GPIO_LOW);
				PinLibrary[i][n] = GPIO_FREE;																			// Setting the PinLibrary to GPIO_FREE prevents any calls to bDigitalWrite( PORT, PIN , status ).
			}
		}
	}
}

void nGPIOSetup() 
{
/* ==================================================================
  	Function name	: nGPIOSetup
  	Functionality	: Used to quickly setup several pins, can be used at startup
   	Returns			: Nothing
    Input range		: None
   ================================================================== */	
	nInitializePinLibrary();
	
	/* Setup Pins for LEDS 1:4 */
	bSetupDigitalIO( PORT_1 , LED_1 , GPIO_OUTPUT );
	bSetupDigitalIO( PORT_1 , LED_2 , GPIO_OUTPUT );
	bSetupDigitalIO( PORT_1 , LED_3 , GPIO_OUTPUT );
	bSetupDigitalIO( PORT_1 , LED_4 , GPIO_OUTPUT );
	
	/* Digital Outputs for hardware (see HW_Pinmap in Programs_file.c */
	bSetupDigitalIO( PORT_0 , PIN_9  , GPIO_OUTPUT );				/* P5	*/
	bSetupDigitalIO( PORT_0 , PIN_8  , GPIO_OUTPUT );				/* P6	*/
	bSetupDigitalIO( PORT_0 , PIN_7  , GPIO_OUTPUT );				/* P7	*/
	bSetupDigitalIO( PORT_0 , PIN_6  , GPIO_OUTPUT );				/* P8	*/
	bSetupDigitalIO( PORT_0 , PIN_0  , GPIO_OUTPUT );				/* P9	*/
	bSetupDigitalIO( PORT_0 , PIN_1  , GPIO_OUTPUT );				/* P10	*/
	bSetupDigitalIO( PORT_0 , PIN_18 , GPIO_OUTPUT );				/* P11	*/
	bSetupDigitalIO( PORT_0 , PIN_17 , GPIO_OUTPUT );				/* P12	*/
	bSetupDigitalIO( PORT_0 , PIN_15 , GPIO_OUTPUT );				/* P13	*/
	bSetupDigitalIO( PORT_0 , PIN_16 , GPIO_OUTPUT );				/* P14	*/
	bSetupDigitalIO( PORT_1 , PIN_30 , GPIO_OUTPUT );				/* P19	*/
	bSetupDigitalIO( PORT_1 , PIN_31 , GPIO_OUTPUT );				/* P20	*/
	bSetupDigitalIO( PORT_2 , PIN_5  , GPIO_OUTPUT );				/* P21	*/
	bSetupDigitalIO( PORT_2 , PIN_4  , GPIO_OUTPUT );				/* P22	*/
	bSetupDigitalIO( PORT_2 , PIN_3  , GPIO_OUTPUT );				/* P23	*/
	
}

void nInitializePinLibrary()
{
	/* ==================================================================
		Function name	: nInitializePinLibrary
		Functionality	: Used to set all pins to GPIO_FREE, to allow setups to reserve pins, and prevent overlapping on pins
		Returns			: None
		Input range		: None
	   ================================================================== */
	static uint8_t Initialized = 0;

	if (!Initialized)
	{

		for (uint8_t column = 0; column < 5; column++)
		{
			for (uint8_t row = 0; row < 32; row++)
				PinLibrary[column][row] = GPIO_FREE;
		}

		Initialized = 1;
	}
}



/* ==================================================================
  	Type		: 	VALUE_RETURN Functions
  	Example		:	vX();
  	Description	:	Returns a value, either 1 or 0. no confirmation if successful or not.
   ================================================================== */
uint16_t vAnalogRead(uint8_t ADCNumber)
{
/* ====================================================================
  	Function name	: vAnalogRead
  	Functionality	: Read an analog input, and return it in a 16-bit variable
   	Returns			: A 16-bits value, ADC is only 12 bits, so last 4 bits won't be touched.
    Input range		: 0 - 7, ADC Channel 0 = 0, ADC Channel 7 = 7
   ==================================================================== */
	uint16_t vAnalogRead_ret = 0;
	
	if (ADC_SETUP_REGISTER & (1 << ADCNumber))
	{	
		LPC_ADC->ADCR &= ~(0xFF);
		LPC_ADC->ADCR |= (0x1 << ADCNumber);						/* Select corresponding ADC Channel to sample from		*/
		for (int i = 0; i < 1000; i++) ;							/* Allow the bit to settle								*/
		LPC_ADC->ADCR |= (0x1 << 24);								/* Set the START-CONVERSION-NOW-bit						*/
		while (!LPC_ADC->ADGDR & (0x80000000)) ; 					/* Wait until Conversion Complete flag is set			*/
	
		vAnalogRead_ret = (LPC_ADC->ADGDR >> SBIT_RESULT) & 0xFFF;	
	}
	return vAnalogRead_ret;
}

uint8_t vDigitalRead( uint8_t Port , int Pin ) 
{
/* =================================================================
  	Function name	: vReadDigitalInput
  	Functionality	: Read a digital input from a pin, and return it
   	Returns			: A value 0 or 1, depending on status of pin
    Input range		: PORT_0 : PORT_4  ,  PIN_0 : PIN_31
   ================================================================= */
	nInitializePinLibrary();
	
	uint8_t vDigitalRead_ret;
	
	if (PinLibrary[Port][Pin] == GPIO_INPUT) 
	{
		switch(Port) 
		{
			case PORT_0:
				vDigitalRead_ret = (LPC_GPIO0->FIOPIN>>Pin) & 0x01;
			break;
			
			case PORT_1:
				vDigitalRead_ret = (LPC_GPIO1->FIOPIN>>Pin) & 0x01;
			break;
			
			case PORT_2:
				vDigitalRead_ret = (LPC_GPIO2->FIOPIN>>Pin) & 0x01;
			break;
			
			case PORT_3:
				vDigitalRead_ret = (LPC_GPIO3->FIOPIN>>Pin) & 0x01;
			break;
			
			case PORT_4:
				vDigitalRead_ret = (LPC_GPIO4->FIOPIN>>Pin) & 0x01;
			break;
			
			default: 
			
			break;
		}
	}
	else
		vDigitalRead_ret = 0xFF;
	
	return vDigitalRead_ret;
}


uint8_t vGetPinStatus	( uint8_t Port , int Pin ) 
{
/* ==================================================================
  	Function name	: vGetPinStatus
  	Functionality	: Returns the PIN_STATUS of a given pin, from the pin library.
   	Returns			: None
    Input range		: PORT_0 - PORT_4 , PIN_0 - PIN_31
   ================================================================== */		
	nInitializePinLibrary();
	
	return PinLibrary[Port][Pin];
}

