´/* ============================================================================================
 * Source file : 
 * Author : Frederik Snedevind
 *		    Frs@Jeros.com
 *
 * Company: Jeros A/S
 *		   HTTP://www.Jeros.com/
 *
 * Date created : 9/4 - 2018
 ===============================================================================================
 * Description: Within the Sensor_file, the sensor task is contained.
 * 	this task is responsible for collecting data from every single sensor located in and
 *	around the machine. This also 
 *  
 *	
 =============================================================================================== */

/* ========================================================================================
   
   				Libraries
   
   ======================================================================================== */
#include "Sensor_file.h"
#include "GPIO_setup.h"
#include "UART_control.h"
#include "LED_control.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "stdutils.h"
#include "lpc17xx.h"

/* ========================================================================================
  
  					Variables
  
   ======================================================================================= */
xQueueHandle SensorQ = NULL;
uint8_t	ACTIVE_SENSORS = 0;
uint8_t SensorStatusLibrary[NUMBER_OF_SENSORS];

uint16_t SensorDataLibrary[NUMBER_OF_SENSORS][3] = {{1,1,1},
													{1,1,1},
													{1,1,1},
													{1,1,1},
													{1,1,1},
													{1,1,1},
													{1,1,1},
													{1,1,1},
													{1,1,1},
													{1,1,1},
													{1,1,1},
													{1,1,1},
													{1,1,1},
													{1,1,1},
													{1,1,1},
													{1,1,1}};



/* ========================================================================================
	Type		: 	BINARY_RETURN Functions
	Example		:	bXxXxX();
	Description	:	Returns true (1) or false (0) depending on the success of the function
   ======================================================================================== */


/* ========================================================================================
  	Type		: 	NO_RETURN Functions
  	Example		:	nXxXxX();
  	Description	:	Does not return anything.
   ======================================================================================== */
void nInitializeAnalogSensors() 
{
/* ==================================================================
  	Function name : nInitializeAnalogSensors
  	Functionality :	Initializes 
   	Returns		  :	None
    Input range	  : None
   ================================================================ */
	nSensorLibrary_Init(); 
	
	for(uint8_t i = ANALOG_SENSORS_START ; i < ANALOG_SENSORS_END ; i++)
	{
		bSetupAnalogI(i);
		SensorStatusLibrary[i] = SENSOR_ACTIVE;		
	}
		
}

void nInitializeDigitalSensors() 
{
/* ==================================================================
  	Function name	: nInitializeDigitalSensors
  	Functionality	: 
   	Returns			: None
    Input range		: None
   ================================================================ */
	nSensorLibrary_Init(); 

	// Digital Inputs:
				/* 		DI0:    		DI1: P24		DI2: P25		DI3: P26	*/
				/* 		DI4: P27		DI5: P28		DI6: P29		DI7: P30	*/
	// Sensor:			 	   				   6			   7			   8
	//				           9			  10			  11			  12
	
	uint8_t	Digital_sensors_ports[] = {2,2,2,0 ,0 ,0,0};		// PORTS for the 7 digital inputs
	uint8_t Digital_sensors_pins [] = {2,1,0,11,10,5,4};		// PINS for the 7 digital inputs
	
	for (uint8_t i = 0; i < (sizeof(Digital_sensors_pins) / sizeof(Digital_sensors_pins[0])); i++) 
	{
		bSetupDigitalIO(Digital_sensors_ports[i] , Digital_sensors_pins[i] , GPIO_INPUT );
		SensorStatusLibrary[i + DIGITAL_SENSORS_START] = SENSOR_ACTIVE;
	}
}
																										
																										
void nSensorData() 
{
/* ==================================================================
  	Function name	: nSensorData
  	Functionality	: Called from UART to output sensordata from all active analog and digital sensors
   	Returns			: None
    Input range		: None
   ================================================================ */
	
	while(! bUART_RxReady()) 
	{
		nNewLine( 1 );
		nUART_TxString("Sensordata");
		nNewLine( 1 );
		
		for (uint8_t i = 0; i < NUMBER_OF_SENSORS; i++) 
		{
			
			if ( i == ANALOG_SENSORS_START )
				nUART_TxString("--Analog Sensors--\r\n");
			
			if ( i == DIGITAL_SENSORS_START )
				nUART_TxString("--Digital Sensors--\r\n");
			
			if (SensorStatusLibrary[i] == SENSOR_PAUSED) 
			{
				nUART_TxString("S ");
				nPrintInteger(i);
				nUART_TxString(" : ");
				nUART_TxString("PAUSED");
				nNewLine( 1 );
			}
			
			else if (SensorStatusLibrary[i] == SENSOR_VACANT)
			{
				nUART_TxString("S ");
				nPrintInteger(i);
				nUART_TxString(" : ");
				nUART_TxString("VACANT");
				nNewLine( 1 );
			}
				
			if (SensorStatusLibrary[i] != SENSOR_ACTIVE)
				continue;
			
			nUART_TxString("S ");
			nPrintInteger(i);
			nUART_TxString(" : ");
			
			// Analog sensor output needs to be split into up to 4 characters (12 bit ADC top value is 4095).
			if ( i <= ANALOG_SENSORS_END) 
			{
				
				int ADC_Sample = SensorDataLibrary[i][0];
				nPrintInteger(ADC_Sample);
			}
			// Digital Sensor output is only 1 character (0 or 1). It gets output by typecasting it to a char
			else
				nUART_TxChar(SensorDataLibrary[i][0] + '0');
			
			nNewLine( 1 );
			
		}
		vTaskDelay(1500);
		nTerminalClear();
	}	
}




void nSensorLibrary_Init() 
{
/* ========================================================================================
  	Function name	: nSensorLibrary_Init
  	Functionality	: Initializes the data in the SensorDataLibrary to 0, along with setting all sensors' status to VACANT (0).
   	Returns			: None
    Input range		: None
   ======================================================================================== */
	static uint8_t INITIALIZED = 0;
	if (! INITIALIZED) {
		for (uint8_t i = 0; i < NUMBER_OF_SENSORS; i++) 
		{
			SensorDataLibrary[i][0] = '0';
			SensorStatusLibrary[i] 	= SENSOR_VACANT;
		}
		INITIALIZED = 1;
	}
}
/* ========================================================================================
  	Type		: 	VALUE_RETURN Functions
  	Example		:	vXxXxX();
  	Description	:	Returns a value, no confirmation if successful or not. can be a handle for e.g. ADC pin
   ======================================================================================== */
uint16_t vGetSensorData( uint8_t SENSOR ) 
{
/* ========================================================================================
  	Function name	: vGetSensorData
  	Functionality	: Returns the newest data from a given sensor, from the SensorDataLibrary
   	Returns			: Value, either 12-bit from ADC's, or 1-bit from digital inputs
    Input range		: 0 to NUMBER_OF_SENSORS - 1
   ======================================================================================== */
	if (SensorStatusLibrary[SENSOR] == SENSOR_ACTIVE || SensorStatusLibrary[SENSOR] == SENSOR_PAUSED)
		return SensorDataLibrary[SENSOR][0];
	else
		return 0x1000;
}



/* ========================================================================================
  	Type		: 	Tasks
  	Example		:	tXx();
  	Description	:	FreeRTOS Task Structure
   ======================================================================================== */

void tSensor_Task( void *param) 
{
	
	uint8_t	Digital_sensors_ports[] = {2,2,2,0 ,0 ,0,0};		/* PORTS for the 7 digital inputs		*/
	uint8_t Digital_sensors_pins [] = {2,1,0,11,10,5,4};		/* PINS for the 7 digital inputs		*/

	uint8_t SENSOR_TASK_STATE = SENSOR_STATE_IDLE;
	
	while( 1 ) 
	{
		
		switch ( SENSOR_TASK_STATE ) 
		{
			
			case SENSOR_STATE_IDLE :
				
				ACTIVE_SENSORS = 0;
				for (uint8_t i = 0; i < NUMBER_OF_SENSORS; i++) 
				{
					if ( SensorStatusLibrary[i] == SENSOR_ACTIVE )
						ACTIVE_SENSORS++;
				}
			
				vTaskDelay(10);
				SENSOR_TASK_STATE = SENSOR_STATE_DATA_COLLECTION;
			break;
			
				
				
			case SENSOR_STATE_DATA_COLLECTION :
				for (uint8_t i = ANALOG_SENSORS_START; i < 16; i++) 
				{
					
					if ( SensorStatusLibrary[i] != SENSOR_ACTIVE )
						continue;
					
					if ( i < ANALOG_SENSORS_END) 
						SensorDataLibrary[i][0] = vAnalogRead(i);
					
					else
						SensorDataLibrary[i][0] = vDigitalRead( Digital_sensors_ports[i - DIGITAL_SENSORS_START] , Digital_sensors_pins[i - DIGITAL_SENSORS_START] );
				}
				SENSOR_TASK_STATE = SENSOR_STATE_IDLE;
				vTaskDelay(10);
			break;
			
			default :
				SENSOR_TASK_STATE = SENSOR_STATE_IDLE;
			break;
		}
		vTaskDelay(10);
	}
}

uint8_t  vGetSensorStatus	( uint8_t SENSOR_HANDLE ) 
{
/* ========================================================================================
  	Function name		: vGetSensorStatus
  	Functionality	: Returns the current status on the argumented sensor. This can either be:
  						SENSOR_VACANT (0)
  						SENSOR_ACTIVE (1)
  						SENSOR_PAUSED (2)
  
   	Returns				: SENSOR_VACANT = 0		,		SENSOR_ACTIVE = 1		,		SENSOR_PAUSED = 2
    Input range			: 0 to NUMBER_OF_SENSORS - 1
   ======================================================================================== */
	return SensorStatusLibrary[SENSOR_HANDLE];
}

uint8_t bChangeSensorStatus(uint8_t SENSOR_HANDLE, SENSOR_STATUS NewSensorStatus)
/* ========================================================================================
	Function name	: bChangeSensorStatus
	Functionality	: Change the current status of a chosen sensor.
	Returns			: True, if a change is made.
	Input range		: [0 : 2], SENSOR_VACANT, SENSOR_ACTIVE & SENSOR_PAUSED
   ======================================================================================== */
{
	uint8_t bChangeSensorStatus_ret = 0;
	
	if (SensorStatusLibrary[SENSOR_HANDLE] != NewSensorStatus)
	{
		SensorStatusLibrary[SENSOR_HANDLE] = NewSensorStatus;
		bChangeSensorStatus_ret = 1;
	}
	
	return bChangeSensorStatus_ret;
	
}
