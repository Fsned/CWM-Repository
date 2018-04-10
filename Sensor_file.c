/**********************************************************************************************
 * Source file : 
 * Author : Frederik Snedevind
 *		    Frs@Jeros.com
 *
 * Company: Jeros A/S
 *		   HTTP://www.Jeros.com/
 *
 * Date created : 9/4 - 2018
 * Version			 : 001
 * Revised			 : D/M - YYYY 
 **********************************************************************************************
 * Description: Within the Sensor_file, the sensor task is contained.
 * 	this task is responsible for collecting data from every single sensor located in and
 *	around the machine. This also 
 *  
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
#include "Sensor_file.h"
#include "GPIO_setup.h"
#include "UART_control.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "stdutils.h"
#include "lpc17xx.h"



// ****************************************************************************************
//
//					Variables
//
// ****************************************************************************************
xQueueHandle SensorQ = NULL;

uint8_t  SensorStatusLibrary[NUMBER_OF_SENSORS];

uint8_t SensorDataLibrary[NUMBER_OF_SENSORS][3] = {{1,2,3},
																										{1,2,3},
																										{1,2,3},
																										{1,2,3},
																										{1,2,3},
																										{1,2,3},
																										{1,2,3},
																										{1,2,3},
																										{1,2,3},
																										{1,2,3},
																										{1,2,3},
																										{1,2,3},
																										{1,2,3},
																										{1,2,3},
																										{1,2,3},
																										{1,2,3}};



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
void nSensorData() {
	
	while(! yUART_RxReady()) {
		nNewLine( 1 );
		nUART_TxString("Current Sensordata ---\r\n");
		for (uint8_t i = 0; i < NUMBER_OF_SENSORS; i++) {
			if (SensorStatusLibrary[i] != SENSOR_ACTIVE)
				continue;
			
			nUART_TxString("Sensor");
			nUART_TxChar(i + '0');
			nUART_TxString(" : ");
			nUART_TxChar(SensorDataLibrary[i][0] + '0');
			nUART_TxString("\r\n");
		}
		
		vTaskDelay(1500);
		nTerminalClear();
	}
}




void nSensorDataLibrary_Init() {
	for (uint8_t i = 0; i < NUMBER_OF_SENSORS; i++) {
		SensorDataLibrary[i][0] = '0';
		SensorStatusLibrary[i] 	= SENSOR_VACANT;
	}
}
// ****************************************************************************************
//	Type		: 	VALUE_RETURN Functions
//	Example		:	vXxXxX();
//	Description	:	Returns a value, no confirmation if successful or not. can be a handle for e.g. ADC pin
// ****************************************************************************************


// ****************************************************************************************
//	Type		: 	Tasks
//	Example		:	tXx();
//	Description	:	FreeRTOS Task Structure
// ****************************************************************************************

void tSensor_Task( void *param) {
	nSensorDataLibrary_Init();
	// ------------------------------------------------------------------------------------------------------------
	//		SETUP DIGITAL SENSOR INPUTS					Digital Inputs : 8
	// ------------------------------------------------------------------------------------------------------------
	// Digital Inputs:
				/* 		DI0: P23		DI1: P24		DI2: P25		DI3: P26
				 * 		DI4: P27		DI5: P28		DI6: P29		DI7: P30	*/
	// Sensor:			 	5						6						7						8
	//								9						10					11					12
	
	uint8_t  Digital_sensors_ports[8] = {2,2,2,2,0 ,0 ,0,0};		// PORTS for the 8 digital inputs
	uint8_t Digital_sensors_pins [8] = {3,2,1,0,11,10,5,4};		// PINS for the 8 digital inputs
	
	for (uint8_t i = 0; i < 8; i++) {
		ySetupDigitalI(Digital_sensors_ports[i] , Digital_sensors_pins[i]);
		SensorStatusLibrary[i + DIGITAL_SENSORS_START] = SENSOR_ACTIVE;
		vTaskDelay(5);
	}
	// Digital input setup end	------------------------------------------------------------------------------------
	
	
	
	
	// ------------------------------------------------------------------------------------------------------------
	//		SETUP Analog SENSOR INPUTS					Analog Inputs : 4
	// ------------------------------------------------------------------------------------------------------------
	LPC_SC->PCONP |= ( 1 << 12 );				// Enable clock for Internal ADC controller
	vTaskDelay(1);
	LPC_ADC->ADCR |= (( 1 << 21) | (10 << SBIT_CLCKDIV ));	
	vTaskDelay(1);
// ACTIVE ADC Channels
	LPC_PINCON->PINSEL1 |= 1 << 14;					// AD0, P0.23			// Sensor 0
	LPC_PINCON->PINSEL1 |= 1 << 16;					// AD1, P0.24			// Sensor 1
	LPC_PINCON->PINSEL1 |= 1 << 18;					// AD2, P0.25			// Sensor 2
	LPC_PINCON->PINSEL1 |= 1 << 20;					// AD3, P0.26			// Sensor 3
	
// INACTIVE ADC Channels
//	LPC_PINCON->PINSEL3 |= 1 << 30;					// AD4, P1.30
//	LPC_PINCON->PINSEL3 |= 1 << 30;					// AD5, P1.31
//	LPC_PINCON->PINSEL0 |= 1 << 3;					// AD6, P0.3
//	LPC_PINCON->PINSEL0 |= 1 << 2;					// AD7, P0.2
	for (uint8_t i = 0; i < 4; i++)
		SensorStatusLibrary[i] = SENSOR_ACTIVE;
		
	// Analog input setup end --------------------------------------------------------------------------------------
	
	
	
	
	
	uint8_t SENSOR_ITERATOR = 0;
	uint8_t SENSOR_TASK_STATE = SENSOR_STATE_IDLE;
	
	
	while( 1 ) {
		
		switch ( SENSOR_TASK_STATE ) {
			
			case SENSOR_STATE_IDLE :
				vTaskDelay(10);
				SENSOR_TASK_STATE = SENSOR_STATE_DATA_COLLECTION;
				break;
			
			
			
			case SENSOR_STATE_DATA_COLLECTION :
				for (uint8_t i = 0; i < 16; i++) {
					
					if ( SensorStatusLibrary[i] != SENSOR_ACTIVE )
						continue;
					
					if ( i < ANALOG_SENSORS_END) {
						LPC_ADC->ADCR |= ( 0x1 << 0 );		// Select corresponding ADC Channel to sample from
						vTaskDelay(1);																  // Allow the bit to settle
						LPC_ADC->ADCR |= ( 0x1 << 24);									// Set the START-CONVERSION-NOW-bit
						
						while (! LPC_ADC -> ADGDR & (0x80000000)); 				// Wait until Conversion Complete flag is set
						
						SensorDataLibrary[i][0] = (LPC_ADC->ADGDR >> SBIT_RESULT) & 0xFFF;
						vTaskDelay(1);
					}
					
					else
						SensorDataLibrary[i][0] = vDigitalRead( Digital_sensors_ports[i - DIGITAL_SENSORS_START] , Digital_sensors_pins[i - DIGITAL_SENSORS_START] );
				}
				SENSOR_TASK_STATE = SENSOR_STATE_IDLE;
				vTaskDelay(10);
				break;
			
				
				
			case SENSOR_STATE_ALARM :
				
				break;
			
			
			
			
			default :
				SENSOR_TASK_STATE = SENSOR_STATE_IDLE;
				break;
		}
		vTaskDelay(10);
	}
}

