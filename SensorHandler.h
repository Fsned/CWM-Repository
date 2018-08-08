/* =======================================================================================
 * Header file : 
 * Author : Frederik Snedevind
 *		    Frs@Jeros.com
 *
 * Company: Jeros A/S
 *		   HTTP://www.Jeros.com/
 *
 * Date created : 9/4 - 2018
 =========================================================================================
 * Description:
 * 	Header file providing sensor API to the rest of the program.
 *  
 * Check the corresponding .c-file for further explanation of functions.	
 *
 =========================================================================================	*/
 #ifndef SENSOR_FILE_H
 #define SENSOR_FILE_H
 
 #include "FreeRTOS.h"
 #include "task.h"
 #include "queue.h"
 
 
/* ======================================================================================
  					
  					Constants
  					
   ====================================================================================== */
//#define NUMBER_OF_SENSORS 16

// Sensor task STATEs
typedef enum
{
	SENSOR_STATE_IDLE,
	SENSOR_STATE_DATA_COLLECTION,
	SENSOR_STATE_ALARM
} SENSOR_TASK_STATE;

// Sensor Data anchors
typedef enum
{
	SENSOR_LOW,
	SENSOR_HIGH
} DIGITAL_SENSOR_VALUE;

// Sensor status'
typedef enum
{
	SENSOR_VACANT,
	SENSOR_ACTIVE,
	SENSOR_PAUSED
} SENSOR_STATUS;

// Sensor Indexing definitions -----------------
#define ANALOG_SENSORS_START			0			/* Index of first Analog Sensor */
#define ANALOG_SENSORS_END				4			/* Index of last Analog Sensor */
		
#define DIGITAL_SENSORS_START			5			/* Index of first Digital Sensor */
#define DIGITAL_SENSORS_END				14			/* Index of last Digital Sensor*/

enum SENSORS
{
	/* Analog Sensors */
	TEMPERATURE_SENSOR_WASH,
	TEMPERATURE_SENSOR_RINSE,
	SOAP_SENSOR,
	REVERSAL_CURRENT_SENSOR,
	SOFTENER_SENSOR,
	
	/* Digital Sensors */
	PRESSOSWITCH_WASH,			
	PRESSOSWITCH_RINSE,
	BOILERSWITCH_WASH,
	BOILERSWITCH_RINSE,
	DOORSWITCH,
	EMERGENCY_STOP_1,
	EMERGENCY_STOP_2,
	THERMAL_FUSE_1,
	THERMAL_FUSE_2,
	THERMAL_FUSE_3,
		
	NUMBER_OF_SENSORS
};



// Hardware Status Modes

enum DoorStatus
{
	HardwareDoorOpen,
	HardwareDoorClosed,

	NumberOfDoorStates
};

enum ValveStatus
{
	HardwareValveClosed,
	HardwareValveOpen,

	NumberOfValveStates
};


// Sensor Indexing end -------------------------

// ADC Relevant Constants
#define ADC_UNITS			1
#define ADC_CHANNELS		8
#define ADC_CAPACITY	ADC_UNITS * ADC_CHANNELS

#define SBIT_BURST      16u
#define SBIT_START      24u
#define SBIT_PDN        21u
#define SBIT_EDGE       27u 
#define SBIT_DONE       31u
#define SBIT_RESULT     4u
#define SBIT_CLCKDIV    8u

#define ADC_VACANT		0
#define ADC_PAUSE		1
#define ADC_HOLD		2
#define ADC_ACTIVE		3

// Error codes to return in various erroneous conditions
#define ADC_NO_DATA		0x1000
#define ADC_PAUSED		0x2000
#define ADC_NO_ACCESS 	0x3000

// Queue Handles for Sensor task
extern xQueueHandle SensorQ;


/* =======================================================================================
  	Type		: 	BINARY_RETURN Functions
  	Example		:	bXxXxX();
  	Description	:	Returns true (1) or false (0) depending on the success of the function
   ===================================================================================== */	
uint8_t bChangeSensorStatus(uint8_t SENSOR_HANDLE, SENSOR_STATUS NewSensorStatus);

/* =======================================================================================
	Type		: 	NO_RETURN Functions
	Example		:	nXxXxX();
	Description	:	Does not return anything.
   ===================================================================================== */
extern void nSensorData						( void );

extern void nInitializeDigitalSensors		( void );

extern void nInitializeAnalogSensors		( void );

extern void nSensorLibrary_Init				( void );
/* =======================================================================================
  	Type		: 	VALUE_RETURN Functions
  	Example		:	vXxXxX();
  	Description	:	Returns a value, no confirmation if successful or not. can be a handle for e.g. ADC pin
   ===================================================================================== */
extern uint16_t vGetSensorData				( uint8_t SENSOR_HANDLE );

extern uint8_t  vGetSensorStatus			( uint8_t SENSOR_HANDLE );
/* =======================================================================================
  	Type		: 	Tasks
  	Example		:	tXx();
  	Description	:	FreeRTOS Task Structure
   ===================================================================================== */
extern void tSensor_Task					( void* );

#endif /* SENSOR_FILE_H */

