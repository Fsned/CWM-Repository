/**********************************************************************************************
 * Header file : 
 * Author : Frederik Snedevind
 *		    Frs@Jeros.com
 *
 * Company: Jeros A/S
 *		   HTTP://www.Jeros.com/
 *
 * Date created : 9/4 - 2018
 **********************************************************************************************
 * Description:
 * 	Header file providing sensor API to the rest of the program.
 *  
 * Check the corresponding .c-file for further explanation of functions.	
 *
 *********************************************************************************************/
 #include "FreeRTOS.h"
 #include "task.h"
 #include "queue.h"
 
 
// ****************************************************************************************
//
//					Constants
//
// ****************************************************************************************
#define NUMBER_OF_SENSORS 16

// Sensor task STATEs
#define SENSOR_STATE_IDLE							0
#define SENSOR_STATE_DATA_COLLECTION	1
#define SENSOR_STATE_ALARM						2

// Sensor Data anchors
#define SENSOR_LOW										0
#define	SENSOR_HIGH										1

// Sensor status'
#define SENSOR_VACANT									0
#define SENSOR_ACTIVE									1
#define SENSOR_PAUSED									2

// Sensor Indexing definitions -----------------
#define ANALOG_SENSORS_START			0
#define ANALOG_SENSORS_END				4

#define DIGITAL_SENSORS_START			5
#define DIGITAL_SENSORS_END				14

// Analog Sensors			Amount : ~5
#define TEMPERATURE_SENSOR_WASH		0
#define TEMPERATURE_SENSOR_RINSE	1
#define SOAP_SENSOR								2
#define REVERSAL_CURRENT_SENSOR		3
#define SOFTENER_SENSOR						4
//#define VACANT ANALOG SENSOR    4

// Digital Sensors		Amount : ~10
#define PRESSOSWITCH_WASH					5
#define	PRESSOSWITCH_RINSE				6
#define BOILERSWITCH_WASH					7
#define BOILERSWITCH_RINSE				8
#define DOORSWITCH								9
#define EMERGENCY_STOP_1					10
#define EMERGENCY_STOP_2					11
#define THERMAL_FUSE_1						12
#define THERMAL_FUSE_2						13
#define THERMAL_FUSE_3						14
//#define VACANT DIGITAL SENSOR   15

// Vacant spots			: 2
// Analog sensors		: #4
// Digital sensors	: #15

// Sensor Indexing end -------------------------

// ADC Relevant Constants
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

// Error codes to return in various erroneous conditions
#define ADC_NO_DATA 		0x1000
#define ADC_PAUSED			0x2000
#define ADC_NO_ACCESS 	0x3000

// Queue Handles for Sensor task
extern xQueueHandle SensorQ;

// ****************************************************************************************
//	Type		: 	NO_RETURN Functions
//	Example		:	nXxXxX();
//	Description	:	Does not return anything.
// ****************************************************************************************
extern void nSensorData		( void );

// ****************************************************************************************
//	Type		: 	VALUE_RETURN Functions
//	Example		:	vXxXxX();
//	Description	:	Returns a value, no confirmation if successful or not. can be a handle for e.g. ADC pin
// ****************************************************************************************
extern uint16_t vGetSensorData		( uint8_t SENSOR_HANDLE );

extern uint8_t  vGetSensorStatus	( uint8_t SENSOR_HANDLE );
// ****************************************************************************************
//	Type		: 	Tasks
//	Example		:	tXx();
//	Description	:	FreeRTOS Task Structure
// ****************************************************************************************
extern void tSensor_Task( void* );

