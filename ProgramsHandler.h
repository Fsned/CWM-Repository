/* ============================================================================================
   Header file : Programs_file.h
   Author : Frederik Snedevind
  		    Frs@Jeros.com
  
   Company: Jeros A/S
  		   HTTP://www.Jeros.com/
  
   Date created : 9/4 - 2018
 ==============================================================================================
   Description:
	Header file belonging to the ProgramsHandler module
    Provides API, constants, states etc. to the ProgramHandler task
  	Helps keep track of hardware status in libraries
	Helps keep track of various timers and parameters, in libraries.
 ============================================================================================== */
 #ifndef PROGRAMS_FILE_H
 #define PROGRAMS_FILE_H
 
 
 #include "stdutils.h"
 #include "FreeRTOS.h"
 #include "queue.h"
 
 
/* ========================================================================================
  					Constants
   ======================================================================================== */

// Program Handler Task States
typedef enum {
	IDLE,
	READY,
	BUSY,
	SENSORSTOP,
	PAUSED
} StateType;

// Operation Status messages
enum OperationQueue Messages
{
	NoDefinition,
	RequestOperationStart,
	OperationEnded,
	OperationStopped,
	AckOperationStart,
	DenyOperationStart,
	PauseProgram,
	ResumeProgram
};

#define REQUEST_OPERATION_START			1
#define OPERATION_ENDED					2
#define OPERATION_STOPPED				3
#define ACK_OPERATION_START				4
#define DENY_OPERATION_START			5
#define PAUSE_PROGRAM					6

#define READY_FOR_NEW_PROGRAM			1
#define OCCUPIED_WITH_PROGRAM			2
#define FINISHING_UP					3
#define STOPPED_FOR_SENSORS				4

enum HARDWARE_STATES 
{
	HardwareOff,
	HardwareActive,
	HardwareFailure,
	HardwareAlarm
};

#define HARDWARE_OFF					0
#define HARDWARE_READY					0
#define HARDWARE_ACTIVE					1
#define HARDWARE_FAILURE				2
#define HARDWARE_ALARM					3


enum ParameterLibraryIndeces
{
	Value,
	Minimum,
	Maximum,
	Std
};



// Hardware Handles


// Percentage that temperature can deviate from nominal washing / rinsing temperature
#define TEMPERATURE_DEVIATION			3
#define SOFTENER_DEVIATION				3

// Washing / Rinsing water temperature. Never gets higher, but falls below by 3% before reheating to set temperature
#define WASHING_TEMPERATURE				60
#define RINSING_TEMPERATURE				85

// Current Constants
#define REVERSAL_CURRENT_MIN			0.3				/* Minimum current drawn by reversal is 0.3 Amps	*/
#define REVERSAL_CURRENT_MAX			3				/* Max current drawn by reversal is 3 Amperes.		*/

// SOAP and SOFTENER levels
#define SOAP_LEVEL						10
#define SOFTENER_LEVEL					11

// Program Operation Indexes


#define START_PROGRAM					5
#define	END_PROGRAM						6
#define CHECK_WATER_LEVEL				7 
#define CHECK_WASH_TEMPERATURE			8 
#define	CHECK_RINSE_TEMPERATURE			9 
#define CHECK_SOAP						10
#define CHECK_SOFTENER					11
#define RUN_WASH						12
#define RUN_WAIT						13
#define RUN_RINSE						14

enum HardwareIndeces {
	WASH_PUMP_1				,	
	WASH_PUMP_2 			,	
	RINSE_PUMP_1 			,	
	RINSE_PUMP_2 			,	
	REVERSAL_ENGINE			,	
	SOAP_PUMP 				,	
	SOFTENER_PUMP 			,	
	HEATING_RINSE_1			,	
	HEATING_RINSE_2			,	
	HEATING_WASH_1 			,	
	HEATING_WASH_2 			,	
	CURTAIN_ENGINE 			,	
	DRAIN_PUMP 				,	
	ELECTROVALVE 			,	
	REVERSAL_ENG_DIRECTION	,
	
	NumberOfHardware
};

enum ProgramIndeces {
	PROGRAM_1 = 1,
	PROGRAM_2 = 2,
	PROGRAM_3 = 3,
	PROGRAM_MODIFIED = 4,
	
	NumberOfPrograms
};

enum OperationTimers
{
	WashingOperation,
	WaitingOperation,
	RinsingOperation,
	DrainingOperation,
	BlowingOperation,
	
	NumberofOperationTimers
};


/* Indexing to access the parameters in ParameterLibrary	*/
enum Parameters
{
	WashingTemperature,
	WashingTemperatureDeviation,
	RinsingTemperature,
	RinsingTemperatureDeviation,
	SoapConductivity,
	SoapConductivityDeviation,
	StandbyTemperatureRinse,
	StandbyTemperatureRinseDeviation,
	StandbyTemperatureWash,
	StandbyTemperatureWashDeviation,
	AutomaticLidOpening,
	TemperatureShownInFahrenheit,
	ActivateSteamCondenser,
	
		
	NumberOfParameters
};

#define MAX_NUMBER_OF_OPERATIONS 				16

extern xQueueHandle ProgramHandlerQ;
extern xQueueHandle OperationQ;
extern xQueueHandle ProgramLibrary; 

// ****************************************************************************************
//	Type		: 	BINARY_RETURN Functions
//	Example		:	bXxXxX();
//	Description	:	Returns true (1) or false (0) depending on the success of the function
// ***************************************************************************************							
extern uint8_t 	bSetHWStatus 	( uint8_t HardwareHandle , uint8_t NewStatus );													

// ****************************************************************************************
//	Type		: 	NO_RETURN Functions
//	Example		:	nXxXxX();
//	Description	:	Does not return anything.
// ****************************************************************************************
extern void nFillTanksOperation				( void );

extern void nFillSoapOperation 				( void );

extern void nWashOperation		 			( void );

extern void nRinseOperation	 	 			( void );

extern void nWaitOperation		 			( void );

extern void nFillSoftenerOperation 			( void );

extern void nCheckWashTemperature			( void );

extern void nCheckRinseTemperature			( void );

extern void nEmptyTanks						( void );

extern void nWashProgram_1					( void );
						
extern void nWashProgram_2					( void );
	
extern void nWashProgram_3					( void );


// ****************************************************************************************
//	Type		: 	VALUE_RETURN Functions
//	Example		:	vXxXxX();
//	Description	:	Returns a value, no confirmation if successful or not. can be a handle for e.g. ADC pin
// ****************************************************************************************
extern uint8_t vGetHWStatus					( uint8_t HardwareHandle );

// ****************************************************************************************
//	Type		: 	Tasks
//	Example		:	txX();
//	Description	:	Task functions designed for FreeRTOS scheduling
// ****************************************************************************************
extern void tProgram_Handler				( void* );

#endif /* PROGAMS_FILE_H */

