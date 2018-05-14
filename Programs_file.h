/**********************************************************************************************
 * Header file : Programs_file.h
 * Author : Frederik Snedevind
 *		    Frs@Jeros.com
 *
 * Company: Jeros A/S
 *		   HTTP://www.Jeros.com/
 *
 * Date created : 9/4 - 2018
 **********************************************************************************************
 * Description:
 * 
 *  
 *	
 *
 *********************************************************************************************/
 #include "stdutils.h"
 #include "FreeRTOS.h"
 #include "queue.h"
 
 
// ****************************************************************************************
//
//					Constants
//
// ****************************************************************************************

// Program Handler Task States
#define IDLE													1
#define READY													2
#define BUSY													3
#define SENSORSTOP										4
#define PAUSED												5

// Operation Status messages
#define REQUEST_OPERATION_START				1
#define OPERATION_ENDED								2
#define OPERATION_STOPPED							3
#define ACK_OPERATION_START						4
#define DENY_OPERATION_START					5

#define READY_FOR_NEW_PROGRAM					1
#define OCCUPIED_WITH_PROGRAM					2
#define FINISHING_UP									3
#define STOPPED_FOR_SENSORS						4

// Hardware Status Modes
#define HARDWARE_DOOR_OPEN						0
#define HARDWARE_DOOR_CLOSED					1

#define HARDWARE_VALVE_CLOSED					0
#define HARDWARE_VALVE_OPEN						1

#define HARDWARE_OFF									0
#define HARDWARE_READY								0
#define HARDWARE_ACTIVE								1
#define HARDWARE_FAILURE							2
#define HARDWARE_ALARM								3

// Hardware Handles

enum HardwareIndeces {
	WASH_PUMP_1							= 0,
	WASH_PUMP_2 						= 1,
	RINSE_PUMP_1 						= 2,
	RINSE_PUMP_2 						= 3,
	REVERSAL_ENGINE 				= 4,
	SOAP_PUMP 							= 5,
	SOFTENER_PUMP 					= 6,
	HEATING_RINSE_1 				= 7,
	HEATING_RINSE_2 				= 8,
	HEATING_WASH_1 					= 9,
	HEATING_WASH_2 					= 10,
	CURTAIN_ENGINE 					= 11,
	DRAIN_PUMP 							= 12,
	ELECTROVALVE 						= 13,
	REVERSAL_ENG_DIRECTION 	= 14
};

//#define WASH_PUMP_1										0
//#define WASH_PUMP_2										1
//#define RINSE_PUMP_1									2
//#define RINSE_PUMP_2									3
//#define REVERSAL_ENGINE								4
//#define	SOAP_PUMP											5
//#define SOFTENER_PUMP									6
//#define	HEATING_RINSE_1								7
//#define HEATING_RINSE_2								8
//#define HEATING_WASH_1								9
//#define HEATING_WASH_2								10
//#define CURTAIN_ENGINE								11
//#define DRAIN_PUMP										12
//#define	ELECTROVALVE									13
//#define REVERSAL_ENG_DIRECTION				14



// Percentage that temperature can deviate from nominal washing / rinsing temperature
#define TEMPERATURE_DEVIATION					3

#define SOFTENER_DEVIATION						3

// Washing / Rinsing water temperature. Never gets higher, but falls below by 3% before reheating to set temperature
#define WASHING_TEMPERATURE						60
#define RINSING_TEMPERATURE						85



// Current Constants
#define REVERSAL_CURRENT_MAX					3

// SOAP and SOFTENER levels
#define SOAP_LEVEL										10
#define SOFTENER_LEVEL								11


// Program Operation Indexes
enum ProgramIndeces {
	PROGRAM_1 = 1,
	PROGRAM_2 = 2,
	PROGRAM_3 = 3,
	PROGRAM_4 = 4,
	PROGRAM_5 = 5
};

//#define PROGRAM_1											1
//#define PROGRAM_2											2
//#define PROGRAM_3											3

#define START_PROGRAM									5
#define	END_PROGRAM										6

#define CHECK_WATER_LEVEL							7 
#define CHECK_WASH_TEMPERATURE				8 
#define	CHECK_RINSE_TEMPERATURE				9 
#define CHECK_SOAP										10
#define CHECK_SOFTENER								11
#define RUN_WASH											12
#define RUN_WAIT											13
#define RUN_RINSE											14

#define WASHING_OPERATION							0
#define WAITING_OPERATION							1
#define RINSING_OPERATION							2

#define MAX_NUMBER_OF_OPERATIONS 			16

extern xQueueHandle ProgramHandlerQ;
extern xQueueHandle OperationQ;
extern xQueueHandle ProgramLibrary; 


// ****************************************************************************************
//	Type		: 	YES_RETURN Functions
//	Example		:	yXxXxX();
//	Description	:	Returns true (1) or false (0) depending on the success of the function
// ***************************************************************************************

																	
extern uint8_t 	ySetHWStatus 	( uint8_t HardwareHandle , uint8_t NewStatus );													

// ****************************************************************************************
//	Type		: 	NO_RETURN Functions
//	Example		:	nXxXxX();
//	Description	:	Does not return anything.
// ****************************************************************************************
extern void nFillTanksOperation			( void );

extern void nFillSoapOperation 			( void );

extern void nWashOperation		 			( void );

extern void nRinseOperation	 	 			( void );

extern void nWaitOperation		 			( void );

extern void nFillSoftenerOperation 	( void );

extern void nCheckWashTemperature		( void );

extern void nCheckRinseTemperature	( void );

extern void nWashProgram_1					( void );
						
extern void nWashProgram_2					( void );
	
extern void nWashProgram_3					( void );
// ****************************************************************************************
//	Type		: 	VALUE_RETURN Functions
//	Example		:	vXxXxX();
//	Description	:	Returns a value, no confirmation if successful or not. can be a handle for e.g. ADC pin
// ****************************************************************************************
extern uint8_t vGetHWStatus	( uint8_t HardwareHandle );

// ****************************************************************************************
//	Type		: 	Tasks
//	Example		:	txX();
//	Description	:	Task functions designed for FreeRTOS scheduling
// ****************************************************************************************
extern void tProgram_Handler				( void* );

