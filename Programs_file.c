/**********************************************************************************************
 * Source file : Programs_file
 * Author : Frederik Snedevind
 *		    Frs@Jeros.com
 *
 * Company: Jeros A/S
 *		   HTTP://www.Jeros.com/
 *
 * Date created : 9/4 - 2018
 * Version			 : 001
 * Revised			 : 9/4 - 2018
 **********************************************************************************************
 * Description:
 * 	File containing various programs orders of operations, time constraints, included sensors
 *  	and more. use these tasks to load a program into the program buffer. then start 
 *		executing them by running execute_programs();
 *	Should 
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
#include "Programs_file.h"
#include "Sensor_file.h"
#include "UART_control.h"
#include "utilities.h"


#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"


// ****************************************************************************************
//
//					Variables
//
// ****************************************************************************************
//char CONSTANT_STRINGS[16][25] = {	{"Ready for new program"},			// S0
//																	{"Occupied with program"},			// S1
//																	{"Finishing up"},								// S2
//																	{"Stopped by sensors"},					// S3
//																	{"N/A"},												// S4
//																	{"N/A"},												// S5													
//																	{"N/A"},												// S6
//																	{"N/A"},												// S7
//																	{"N/A"},												// S8
//																	{"N/A"},												// S9
//																	{"N/A"},												// S10
//																	{"N/A"},												// S11
//																	{"N/A"},												// S12
//																	{"N/A"},												// S13
//																	{"N/A"},												// S14
//																	{"N/A"}}; 											// S15


//void (*execution_array[MAX_NUMBER_OF_OPERATIONS])();						// Array Containing pointers to various operations

xQueueHandle OperationQ			 	= NULL;																	// Queue to talk to execution task, signalling e.g. program start, program stop, etc.
xQueueHandle ProgramLibrary		= NULL;
xQueueHandle ProgramHandlerQ	= NULL;

const uint8_t SENSORSKIP = 1;
const uint8_t TIMERSKIP = 1;

uint8_t CurrentProgram = 0;
				// 																Operation Timer Library
				//														 		P1		P1		P2		P2		P3		P3
				//														 		STD		 			STD		 			STD
				//																[0]		[1]		[2]		[3]		[4]		[5]
uint8_t ProgramTimerLibrary[16][6] = {	{	10	,	10	,	10	,	10	,	10	,	10	}, 	// 0 Rinsing Operation
																				{	10	,	10	,	10	,	10	,	10	,	10	},	// 1 Washing Operation
																				{	10	,	10	,	10	,	10	,	10	,	10	},	// 2 Softener Operation
																				{	10	,	10	,	10	,	10	,	10	,	10	},	// 3 
																				{	10	,	10	,	10	,	10	,	10	,	10	},	// 4
																				{	10	,	10	,	10	,	10	,	10	,	10	},	// 5
																				{	10	,	10	,	10	,	10	,	10	,	10	},	// 6
																				{	10	,	10	,	10	,	10	,	10	,	10	},	// 7 
																				{	10	,	10	,	10	,	10	,	10	,	10	},	// 8
																				{	10	,	10	,	10	,	10	,	10	,	10	},	// 9
																				{	10	,	10	,	10	,	10	,	10	,	10	},	// 10
																				{	10	,	10	,	10	,	10	,	10	,	10	},	// 11
																				{	10	,	10	,	10	,	5 	,	10	,	10	},	// 12	Wash
																				{	10	,	10	,	10	,	10	,	10	,	10	},	// 13	Wait
																				{	10	,	10 	,	10	,	8 	,	10	,	10	},	// 14	Rinse
																				{	10	,	10	,	10	,	10	,	10	,	10	}};	// 15

															//{ [0] Value , [1] Minimum , [2] Maximum , [3] Req. Perm. Level };
//uint16_t ParameterLibrary[16][4] = { 	{	1	,	2	,	3	,	4	}, 	// Washing Temperature
//										{	1	,	2	,	3	,	4	},	// Rinsing Temperature
//										{	1	,	2	,	3	,	4	}, 	// 
//										{	1	,	2	,	3	,	4	},
//										{	1	,	2	,	3	,	4	}, 
//										{	1	,	2	,	3	,	4	},
//										{	1	,	2	,	3	,	4	}, 
//										{	1	,	2	,	3	,	4	},
//										{	1	,	2	,	3	,	4	}, 
//										{	1	,	2	,	3	,	4	},
//										{	1	,	2	,	3	,	4	}, 
//										{	1	,	2	,	3	,	4	},
//										{	1	,	2	,	3	,	4	}, 
//										{	1	,	2	,	3	,	4	},
//										{	1	,	2	,	3	,	4	}, 
//										{	1	,	2	,	3	,	4	}};

uint16_t HardwareLibrary[16][2] = 	{	{ 1 , 2 },					
										{ 1 , 2 },
										{ 1 , 2 },
										{ 1 , 2 },
										{ 1 , 2 },
										{ 1 , 2 },
										{ 1 , 2 },
										{ 1 , 2 },
										{ 1 , 2 },
										{ 1 , 2 },
										{ 1 , 2 },
										{ 1 , 2 },
										{ 1 , 2 },
										{ 1 , 2 },
										{ 1 , 2 },
										{ 1 , 2 }};
																			

// ****************************************************************************************
//	Type				: YES_RETURN Functions
//	Example			:	yXxXxX();
//	Description	:	Returns true (1) or false (0) depending on the success of the function
// ****************************************************************************************
uint8_t ySetHWStatus ( uint8_t HardwareHandle , uint8_t NewStatus) {
/* ******************************************************************
//	Function name : ySetHWStatus
//	Functionality :	Set Hardware status to a desired value, if it's not in failure or alarm mode.
// 	Returns				:	true or false. if hw is in failure or alarm mode, returns false, otherwise true	
//  Input range		: 0 - NUMBER_OF_HW - 1 , 0 - 3
// *****************************************************************/		
	if (HardwareLibrary[HardwareHandle][0] != HARDWARE_ALARM && HardwareLibrary[HardwareHandle][0] != HARDWARE_FAILURE) {
		HardwareLibrary[HardwareHandle][0] = NewStatus;
		return 1;
	}
	return 0;
}	
																				
																				
																				
void yWashProgram_1() {
/* ******************************************************************
//	Function name : yWashProgram_1
//	Functionality :	Fills Execution Queue with operations for washing program number 1
// 	Returns				:	True / False depending on whether or not program has been loaded
//  Input range		: None
// *****************************************************************/
//	uint8_t transmit;
	
//	uint8_t Program_1_Recipe[10] =  {	START_PROGRAM,
//																		PROGRAM_1,
//																		CHECK_WATER_LEVEL,
//																		CHECK_WASH_TEMPERATURE,
//																		CHECK_SOAP,
//																		RUN_WASH,
//																		RUN_WAIT,
//																		CHECK_RINSE_TEMPERATURE,
//																		RUN_RINSE,
//																		END_PROGRAM};
//	
//	if (! uxQueueMessagesWaiting(ProgramQ)) {
//		for (int i = 0; i < (sizeof(Program_1_Recipe) / sizeof(Program_1_Recipe[0])); i++) {
//			transmit = Program_1_Recipe[i];
//			xQueueSend(ProgramQ , &transmit , 10);
//		}
//	}
//	vTaskDelay(10);
}


void yWashProgram_2() {
/* ******************************************************************
//	Function name : yWashProgram_2
//	Functionality :	Fills Execution Queue with operations for washing program number 2
// 	Returns				:	True / False depending on whether or not program has been loaded
//  Input range		: None
// *****************************************************************/
	uint8_t transmit;
	
	nTerminalClear();
	uint8_t Program_2_Recipe[10] = {PROGRAM_2,
																	CHECK_WATER_LEVEL,
																	CHECK_SOAP,
																	CHECK_WASH_TEMPERATURE,
																	RUN_WASH,
																	RUN_WAIT,
																	CHECK_RINSE_TEMPERATURE,
																	RUN_RINSE,
																	END_PROGRAM};	
	/*
	uint8_t Program_2_Recipe[10] = {PROGRAM_2,
																	CHECK_WATER_LEVEL,
																	CHECK_SOAP,
																	CHECK_WASH_TEMPERATURE,
																	RUN_WASH,
																	RUN_WAIT,
																	CHECK_RINSE_TEMPERATURE,
																	RUN_RINSE,
																	END_PROGRAM};																
		*/															
																	
	int TotalRuntime;
																	
	uint8_t TotalRuntime_Hundreds = 0;
	uint8_t TotalRuntime_Tens = 0;
	uint8_t TotalRuntime_Ones = 0;
																				
	TotalRuntime += ProgramTimerLibrary[RUN_WASH][3];
	TotalRuntime += ProgramTimerLibrary[RUN_RINSE][3];
	TotalRuntime += ProgramTimerLibrary[RUN_WAIT][3];
																	
	while(TotalRuntime >= 100) {
		TotalRuntime -= 100;
		TotalRuntime_Hundreds++;
	}
	while(TotalRuntime >= 10) {
		TotalRuntime -= 10;
		TotalRuntime_Tens++;
	}
	while(TotalRuntime >= 1) {
		TotalRuntime--;
		TotalRuntime_Ones++;
	}
	nUART_TxString("Total Program Runtime: ");
	
	if (TotalRuntime_Hundreds)
		nUART_TxChar(TotalRuntime_Hundreds + '0');
	if (TotalRuntime_Hundreds || TotalRuntime_Tens)
		nUART_TxChar(TotalRuntime_Tens + '0');
  nUART_TxChar(TotalRuntime_Ones + '0');																
	nUART_TxString(" Seconds.\r\n");
	
																	
	if (! uxQueueMessagesWaiting(ProgramLibrary)) {
		for (unsigned int i = 0; i < (sizeof(Program_2_Recipe) / sizeof(Program_2_Recipe[0])); i++) {
			transmit = Program_2_Recipe[i];
			xQueueSend(ProgramLibrary , &transmit , 10);
		}
		
		transmit = START_PROGRAM;
		xQueueSend(ProgramHandlerQ , &transmit , 10);
		
		nUART_TxString("WASH PROGRAM 2: Completed Initialization. \r\n");
	}
	vTaskDelay(10);
}


void yWashProgram_3() {
/* ******************************************************************
//	Function name : yWashProgram_3
//	Functionality :	Fills Execution Queue with operations for washing program number 3
// 	Returns				:	True / False depending on whether or not program has been loaded
//  Input range		: None
// *****************************************************************/
//	uint8_t transmit;
//	
//	
//	uint8_t Program_3_Recipe[10] = {START_PROGRAM,
//																	PROGRAM_3,
//																	CHECK_WATER_LEVEL,
//																	CHECK_WASH_TEMPERATURE,
//																	CHECK_SOAP,
//																	RUN_WASH,
//																	RUN_WAIT,
//																	CHECK_RINSE_TEMPERATURE,
//																	RUN_RINSE,
//																	END_PROGRAM};
//																	
//	if (! uxQueueMessagesWaiting(ProgramQ)) {
//		for (int i = 0; i < (sizeof(Program_3_Recipe) / sizeof(Program_3_Recipe[0])); i++) {
//			transmit = Program_3_Recipe[i];
//			xQueueSend(ProgramQ , &transmit , 10);
//		}
//	}
//	vTaskDelay(10);
}



// ****************************************************************************************
//	Type		: 	NO_RETURN Functions
//	Example		:	nXxXxX();
//	Description	:	Does not return anything.
// ****************************************************************************************
void nFillTanksOperation() {
/* *********************************************************************************************************
//	Function name : nFillTanksOperation
//	Functionality : Operation that checks the pressoswitches in both tanks and opens the electrovalve. only fills wash tank if door lid is closed
// 	Returns				:	None
//  Input range		: None
// ********************************************************************************************************/
/*	@Description The operation to fill tanks is not limited by a time duration, but by whether or not the tanks are filled.
			The operation should keep an eye firstly on pressure switches, but also on the current drawn by the inlet pump.
			If anything mishaps, put down all pumps and return an error code to the error handler
*/	
	uint8_t transmit;
	uint8_t receive;
	
	int timer;
	
	nUART_TxString("\r\n\r\nStarted CHECK_WATER_LEVEL. \r\n");
	
// =============================================================================
		
	while ( ( vGetSensorData(PRESSOSWITCH_RINSE) == SENSOR_LOW || vGetSensorData(PRESSOSWITCH_WASH) == SENSOR_LOW ) && ! SENSORSKIP) {
		
		if (vGetSensorData(PRESSOSWITCH_RINSE) == SENSOR_LOW) {
			if (vGetHWStatus(ELECTROVALVE) == HARDWARE_VALVE_CLOSED) {
				ySetHWStatus(ELECTROVALVE , HARDWARE_VALVE_OPEN);
			}
		}
		else if (vGetSensorData(PRESSOSWITCH_RINSE) == SENSOR_HIGH) {
			ySetHWStatus(ELECTROVALVE , HARDWARE_VALVE_OPEN);
		}
			
		if (vGetSensorData(PRESSOSWITCH_WASH) == SENSOR_LOW) {
			if (vGetSensorData(DOORSWITCH) == HARDWARE_DOOR_CLOSED) {
				if (vGetHWStatus(ELECTROVALVE) == HARDWARE_VALVE_CLOSED)
					ySetHWStatus(ELECTROVALVE , HARDWARE_VALVE_OPEN);
			}
			else {
				// display "door not closed" message here
			}
		}

		if ( vGetSensorData(PRESSOSWITCH_RINSE) == SENSOR_HIGH && vGetSensorData(PRESSOSWITCH_WASH) == SENSOR_HIGH ) {
			
			ySetHWStatus(ELECTROVALVE , HARDWARE_VALVE_CLOSED);
			
			transmit = OPERATION_ENDED;
			xQueueSend(ProgramHandlerQ , &transmit , 10);
		}
		
		vTaskDelay(100);
	}
// =============================================================================
	if (SENSORSKIP) {
		transmit = OPERATION_ENDED;
		xQueueSend(ProgramHandlerQ , &transmit , 10);
	}
	
	nUART_TxString("Ended CHECK_WATER_LEVEL. \r\n");
}

void nFillSoapOperation () {
/* *********************************************************************************************************
//	Function name : nFillSoapOperation
//	Functionality : Fills soap in the wash tank, up to the set level
// 	Returns				:	None
//  Input range		: None
// ********************************************************************************************************/
	/*	@Description
				The Soap operation is supposed to fill soap in the washing tank. 
				if measured soap level reaches 95% of the pre-measured and entered soaplevel, the operation will complete.
				the operation cuts the pump at 90% to allow the soap to settle in the mixture, and then remeasures to see
				if more has to be added.
	
	*/
	uint8_t transmit;
	uint8_t receive;
	
	nUART_TxString("\r\n\r\nStarted CHECK_SOAP Operation. \r\n");
// ==========================================================================================================	
	while (vGetSensorData(SOAP_SENSOR) < 0.95 * SOAP_LEVEL && ! SENSORSKIP) {
		ySetHWStatus(SOAP_PUMP , HARDWARE_ACTIVE);
		
		if (vGetSensorData(SOAP_SENSOR) >= 0.95*SOAP_LEVEL && vGetHWStatus(SOAP_PUMP) == HARDWARE_ACTIVE) {
			ySetHWStatus(SOAP_PUMP , HARDWARE_OFF);
			vTaskDelay(1000);
		}
	
		if (vGetSensorData(SOAP_SENSOR) < 0.95*SOAP_LEVEL && vGetHWStatus(SOAP_PUMP) == HARDWARE_OFF) {
			ySetHWStatus( SOAP_PUMP , HARDWARE_ACTIVE );
			vTaskDelay(1000);
		}
		
		if (vGetSensorData(SOAP_SENSOR) > 0.95*SOAP_LEVEL && vGetHWStatus(SOAP_PUMP) == HARDWARE_OFF) {
			transmit = OPERATION_ENDED;
			xQueueSend(ProgramHandlerQ , &transmit , 10);
		}
	
		else if (vGetSensorData(SOAP_SENSOR) > 0.95*SOAP_LEVEL && vGetHWStatus(SOAP_PUMP) == HARDWARE_ACTIVE) {
			ySetHWStatus( SOAP_PUMP , HARDWARE_OFF );
			transmit = OPERATION_ENDED;
			xQueueSend(ProgramHandlerQ , &transmit , 10);
		}
		
		vTaskDelay(100);
	}
	
	
// ==========================================================================================================
	
	nUART_TxString("Ended CHECK_SOAP. \r\n");
	
	if (SENSORSKIP) {
		transmit = OPERATION_ENDED;
		xQueueSend(ProgramHandlerQ , &transmit , 10);
	}
	
	vTaskDelay(100);	
}

void nWashOperation		 () {
/* *********************************************************************************************************
//	Function name : nWashOperation
//	Functionality : Runs the wash operation, turns on wash pumps, decrements timer, keeps track of the 
										temperature and keeps it up if it falls below washing (temperature - a set percentage)
// 	Returns				:	None
//  Input range		: None
// ********************************************************************************************************/
	
	/* @Description
			The washing operation should run for a fixed amount of time. this time will be grabbed from the operation_times library (or so) and loaded at start, then decremented accordingly.
			The water temperature should match the temperature stored in a library (somewhere), and the operation must not begin before the water is at least AT the specific temperature (-.5% degrees)
			And then keep the water temperature with a 3% degree range. (2.4 deg. C. @ 80 deg. C. Washing temperature)
	*/
	int timer = ProgramTimerLibrary[RUN_WASH][CurrentProgram] * 1000; 
	
	
	uint8_t transmit;
	uint8_t receive;
	
	const uint8_t NOT_INIT	= 0;
	const uint8_t COOLING		= 1;
	const uint8_t HEATING		= 2;
	
	uint8_t HeatingStatus = NOT_INIT;
	
	uint8_t Reversal_Direction = 0;			// Either 0 for towards the user (forwards) or 1 for towards the back of the machine (backwards).
	
	nUART_TxString("\r\n\r\nStarted RUN_WASH Operation. \r\n");
	nUART_TxString("Operation Runtime: ");
	if (int_to_char_100(ProgramTimerLibrary[RUN_WASH][CurrentProgram])) nUART_TxChar(int_to_char_100(ProgramTimerLibrary[RUN_WASH][CurrentProgram]) + '0');
	nUART_TxChar(int_to_char_10	(ProgramTimerLibrary[RUN_WASH][CurrentProgram]) + '0');
	nUART_TxChar(int_to_char_1	(ProgramTimerLibrary[RUN_WASH][CurrentProgram]) + '0');
	nUART_TxString(" Seconds.\r\n");
	
	while ( timer && ! TIMERSKIP) {
		
		if (vGetHWStatus(WASH_PUMP_1) == HARDWARE_OFF)
			ySetHWStatus(WASH_PUMP_1 , HARDWARE_ACTIVE);
		
		if (vGetHWStatus(WASH_PUMP_2) == HARDWARE_OFF)
			ySetHWStatus(WASH_PUMP_2 , HARDWARE_ACTIVE);
		
		if (vGetHWStatus(REVERSAL_ENGINE) == HARDWARE_OFF)
			ySetHWStatus(REVERSAL_ENGINE , HARDWARE_ACTIVE);
		
		if (vGetSensorData(TEMPERATURE_SENSOR_WASH) < (1 - (TEMPERATURE_DEVIATION/100)) * WASHING_TEMPERATURE && HeatingStatus == COOLING) {
			ySetHWStatus(HEATING_WASH_1 , HARDWARE_ACTIVE);
			ySetHWStatus(HEATING_WASH_2 , HARDWARE_ACTIVE);
			HeatingStatus = HEATING;
		}
		
		if (vGetSensorData(TEMPERATURE_SENSOR_WASH) >= WASHING_TEMPERATURE && HeatingStatus == HEATING) {
			ySetHWStatus(HEATING_WASH_1 , HARDWARE_READY);
			ySetHWStatus(HEATING_WASH_2 , HARDWARE_READY);
			HeatingStatus = COOLING;
		}
		
		
		if (vGetSensorData(REVERSAL_CURRENT_SENSOR) > REVERSAL_CURRENT_MAX) {
			
			ySetHWStatus(REVERSAL_ENGINE , HARDWARE_OFF);
			
			if (Reversal_Direction) {
				Reversal_Direction = 0;
				ySetHWStatus(REVERSAL_ENG_DIRECTION , HARDWARE_OFF);
			}
			else {
				Reversal_Direction = 1;
				ySetHWStatus(REVERSAL_ENG_DIRECTION , HARDWARE_ACTIVE);
			}
			
			ySetHWStatus(REVERSAL_ENGINE , HARDWARE_ACTIVE);
			
		}
		
		if (! timer ) {
			transmit = OPERATION_ENDED;
			xQueueSend(ProgramHandlerQ , &transmit , 10);
			
			ySetHWStatus(WASH_PUMP_1 			, HARDWARE_READY);
			ySetHWStatus(WASH_PUMP_2 			, HARDWARE_READY);
			ySetHWStatus(REVERSAL_ENGINE 	, HARDWARE_READY);
			ySetHWStatus(HEATING_WASH_1		, HARDWARE_READY);
			ySetHWStatus(HEATING_WASH_2		, HARDWARE_READY);
		}
		vTaskDelay(100);
	}
	if (TIMERSKIP) {
		transmit = OPERATION_ENDED;
		xQueueSend( ProgramHandlerQ , &transmit , 10 );
	}
	
	nUART_TxString("Ended RUN WASH Operation.\r\n");
}


void nRinseOperation	 () {
/* *********************************************************************************************************
//	Function name : nRinseOperation
//	Functionality : Runs the rinse operation
// 	Returns				:	None
//  Input range		: None
// ********************************************************************************************************/
	/* @Description
			The Rinsing operation will run for a fixed amount of time. this time will be grabbed from the operation_times library (or so)
			and loaded at start, then decremented accordingly. The rinsing temperature has to match the temperature stored in a library somehere,
			and the operation must not begin before the water is at least AT the specific temperature (-.5% degrees)
			and then keep the water temperature with a 3% degree range. (2.4 deg. C. @ 80 deg. C. Rinsing temperature)
	*/
	
	int timer = ProgramTimerLibrary[RUN_RINSE][CurrentProgram] * 1000;
	
	uint8_t transmit;
	uint8_t receive;
	const uint8_t NOT_INIT	= 0;
	const uint8_t COOLING		= 1;
	const uint8_t HEATING		= 2;
	
	uint8_t HeatingStatus = NOT_INIT;
	
	uint8_t Reversal_Direction = 0;
	
	nUART_TxString("\r\n\r\nStarted RUN_RINSE Operation. \r\n");
	nUART_TxString("Operation Runtime: ");
	if (int_to_char_100(ProgramTimerLibrary[RUN_RINSE][CurrentProgram])) nUART_TxChar(int_to_char_100(ProgramTimerLibrary[RUN_RINSE][CurrentProgram]) + '0');
	nUART_TxChar(int_to_char_10	(ProgramTimerLibrary[RUN_RINSE][CurrentProgram]) + '0');
	nUART_TxChar(int_to_char_1	(ProgramTimerLibrary[RUN_RINSE][CurrentProgram]) + '0');
	nUART_TxString(" Seconds.\r\n");

	while ( timer && ! TIMERSKIP) {
		
		if (vGetHWStatus(RINSE_PUMP_1) == HARDWARE_OFF)
			ySetHWStatus(RINSE_PUMP_1 , HARDWARE_ACTIVE);
		
		if (vGetHWStatus(RINSE_PUMP_2) == HARDWARE_OFF)
			ySetHWStatus(RINSE_PUMP_2 , HARDWARE_ACTIVE);
		
		if (vGetHWStatus(REVERSAL_ENGINE) == HARDWARE_OFF)
			ySetHWStatus(REVERSAL_ENGINE , HARDWARE_ACTIVE);
		
		if (vGetSensorData(TEMPERATURE_SENSOR_RINSE) < (1 - (TEMPERATURE_DEVIATION/100)) * RINSING_TEMPERATURE && HeatingStatus == COOLING) {
			ySetHWStatus(HEATING_RINSE_1 , HARDWARE_ACTIVE);
			ySetHWStatus(HEATING_RINSE_2 , HARDWARE_ACTIVE);
			HeatingStatus = HEATING;
		}
		
		if (vGetSensorData(TEMPERATURE_SENSOR_RINSE) >= RINSING_TEMPERATURE && HeatingStatus == HEATING) {
			ySetHWStatus(HEATING_RINSE_1 , HARDWARE_READY);
			ySetHWStatus(HEATING_RINSE_2 , HARDWARE_READY);
			HeatingStatus = COOLING;
		}
		
		
		if (vGetSensorData(REVERSAL_CURRENT_SENSOR) > REVERSAL_CURRENT_MAX) {
			ySetHWStatus(REVERSAL_ENGINE , HARDWARE_OFF);
			if (Reversal_Direction) {
				Reversal_Direction = 0;
				ySetHWStatus(REVERSAL_ENG_DIRECTION , HARDWARE_OFF);
			}
			else {
				Reversal_Direction = 1;
				ySetHWStatus(REVERSAL_ENG_DIRECTION , HARDWARE_ACTIVE);
			}
			ySetHWStatus(REVERSAL_ENGINE , HARDWARE_ACTIVE);
		}
		
		
		timer -= 100;
		vTaskDelay(100);
		
		if (! timer ) {
			transmit = OPERATION_ENDED;
			xQueueSend(ProgramHandlerQ , &transmit , 10);
			
			ySetHWStatus(RINSE_PUMP_1 		, HARDWARE_READY);
			ySetHWStatus(RINSE_PUMP_2 		, HARDWARE_READY);
			ySetHWStatus(REVERSAL_ENGINE 	, HARDWARE_READY);
			ySetHWStatus(HEATING_RINSE_1	, HARDWARE_READY);
			ySetHWStatus(HEATING_RINSE_2	, HARDWARE_READY);
			
			
		}
	}
	
	nUART_TxString("Ended RUN RINSE Operation.\r\n");
	
	if (TIMERSKIP) {
		transmit = OPERATION_ENDED;
		xQueueSend( ProgramHandlerQ , &transmit , 10 );
	}
}

void nWaitOperation		 () {
/* *********************************************************************************************************
//	Function name : nWaitOperation
//	Functionality : Waits for a fixxed amount of time, between wash- and rinse operations to allow soap to fall off subjects etc.
// 	Returns				:	None
//  Input range		: None
// ********************************************************************************************************/
	/* @Description
			The wait operation can be called between other operations. this could be after a washing operation
			and before a rinsing operation, to allow soap to fall off subjects, to provide a better end result.
	*/
	int timer = ProgramTimerLibrary[RUN_WAIT][CurrentProgram] * 1000;
	uint8_t transmit;
	uint8_t receive;
	
	nUART_TxString("\r\n\r\nStarted WAIT Operation. \r\n");
	nUART_TxString("Operation Runtime: ");
	if (int_to_char_100(ProgramTimerLibrary[RUN_WAIT][CurrentProgram])) nUART_TxChar(int_to_char_100(ProgramTimerLibrary[RUN_WAIT][CurrentProgram]) + '0');
  nUART_TxChar(int_to_char_10	(ProgramTimerLibrary[RUN_WAIT][CurrentProgram]) + '0');
	nUART_TxChar(int_to_char_1	(ProgramTimerLibrary[RUN_WAIT][CurrentProgram]) + '0');
	nUART_TxString(" Seconds.\r\n");
		
	while ( timer && ! TIMERSKIP) {
		
		timer -= 10;
		vTaskDelay(10);
		
		if (! timer ) {
			transmit = OPERATION_ENDED;
			xQueueSend(ProgramHandlerQ , &transmit , 10);
		}
	}
	
	nUART_TxString("Ended WAIT Operation. \r\n");
	
	if (TIMERSKIP) {
		transmit = OPERATION_ENDED;
		xQueueSend( ProgramHandlerQ , &transmit , 10 );
	}
}

void nFillSoftenerOperation () {
/* *********************************************************************************************************
//	Function name : nFillSoftenerOperation
//	Functionality : Fills softener into the rinsing tank. this might not be necessary, 
										unless a new electrical pump is implemented in the machines.
// 	Returns				:	None
//  Input range		: None
// ********************************************************************************************************/
	/* @Description
			The fill softener operation is either timed or controlled by a conductivity sensor. at this moment, the operation is timed.
			Because of this, the task will for the moment also be controlled by a timer. the softener dosage isn't required to be 
			indisputably precise.
	*/
	uint8_t transmit;
	uint8_t receive;
//	int timer = ProgramTimerLibrary[CHECK_SOFTENER][CurrentProgram];
	
	const uint8_t NOT_INIT 	= 0;
	const uint8_t FILLING 	= 1;
	const uint8_t FALLING 	= 2;
	
	uint8_t SoftenerPumpState = NOT_INIT;
	
	nUART_TxString("\r\n\r\nStarted CHECK_SOFTENER Operation. \r\n");
	
	while (vGetSensorData(SOFTENER_SENSOR) < SOFTENER_LEVEL && ! SENSORSKIP) {
		if ( vGetSensorData(SOFTENER_SENSOR) < (1 - ( SOFTENER_DEVIATION / 100)) * SOFTENER_LEVEL && (SoftenerPumpState == FALLING || SoftenerPumpState == NOT_INIT)) {
			ySetHWStatus( SOFTENER_PUMP , HARDWARE_ACTIVE );
			SoftenerPumpState = FILLING;
		}
		
		if (vGetSensorData(SOFTENER_SENSOR) >= SOFTENER_LEVEL && SoftenerPumpState == FILLING) {
			ySetHWStatus( SOFTENER_PUMP , HARDWARE_READY );
			SoftenerPumpState = FALLING;
		}
		
		if (vGetSensorData(SOFTENER_SENSOR) > SOFTENER_LEVEL) {
			ySetHWStatus( SOFTENER_PUMP , HARDWARE_READY );
			SoftenerPumpState = FALLING;
			
			transmit = OPERATION_ENDED;
			xQueueSend(ProgramHandlerQ , &transmit , 10);
			break;
		}
		
		nUART_TxString("Stopped nFillSoftenerOperation.\r\n");
		vTaskDelay(100);
	}
	
	if (SENSORSKIP) {
		transmit = OPERATION_ENDED;
		xQueueSend( ProgramHandlerQ , &transmit , 10);
	}
}






void nCheckWashTemperature	() {
/* *********************************************************************************************************
//	Function name : nCheckWashTemperature
//	Functionality : Checks the washing temperature, before starting a washing program, and warms the water if needed
// 	Returns				:	None
//  Input range		: None
// ********************************************************************************************************/
	uint8_t transmit;
	uint8_t receive;
	
	const uint8_t NOT_INIT 	= 0;
	const uint8_t HEATING  	= 1;
	const uint8_t COOLING		= 2;
	
	uint8_t WashHeatingState = NOT_INIT;
	
	
	nUART_TxString("\r\n\r\nStarted CHECK WASH TEMPERATURE. \r\n");
	while ( vGetSensorData(WASHING_TEMPERATURE) < WASHING_TEMPERATURE && ! SENSORSKIP) {
		
		if (vGetSensorData(WASHING_TEMPERATURE) < (1 - (TEMPERATURE_DEVIATION / 100)) * WASHING_TEMPERATURE && (WashHeatingState == COOLING || WashHeatingState == NOT_INIT)) {
			ySetHWStatus(HEATING_WASH_1 , HARDWARE_ACTIVE);
			ySetHWStatus(HEATING_WASH_2 , HARDWARE_ACTIVE);
			WashHeatingState = HEATING;
		}
		
		if (vGetSensorData(WASHING_TEMPERATURE) >= WASHING_TEMPERATURE ) {
			ySetHWStatus(HEATING_WASH_1 , HARDWARE_OFF);
			ySetHWStatus(HEATING_WASH_2 , HARDWARE_OFF);
			WashHeatingState = COOLING;
			
			transmit = OPERATION_ENDED;
			xQueueSend(ProgramHandlerQ , &transmit , 10);
		}
		vTaskDelay(100);
	}
	
	if (SENSORSKIP) {
		transmit = OPERATION_ENDED;
		xQueueSend( ProgramHandlerQ , &transmit , 10 );
	}
	
	nUART_TxString("Ended CHECK WASH TEMPERATURE. \r\n");
	
}

void nCheckRinseTemperature	() {
/* *********************************************************************************************************
//	Function name : nCheckRinseTemperature
//	Functionality : Checks the rinsing temperature, before starting a rinsing program, and warms the water if needed
// 	Returns				:	None
//  Input range		: None
// ********************************************************************************************************/
	uint8_t transmit;
	uint8_t receive;
	
	const uint8_t NOT_INIT 	= 0;
	const uint8_t HEATING  	= 1;
	const uint8_t COOLING		= 2;
	
	uint8_t RinseHeatingState = NOT_INIT;
	
	nUART_TxString("\r\n\r\nStarted CHECK RINSE TEMPERATURE. \r\n");
	while ( vGetSensorData(RINSING_TEMPERATURE) < RINSING_TEMPERATURE) {
		
		if (vGetSensorData(RINSING_TEMPERATURE) < (1 - (TEMPERATURE_DEVIATION / 100)) * RINSING_TEMPERATURE && (RinseHeatingState == COOLING || RinseHeatingState == NOT_INIT)) {
			ySetHWStatus(HEATING_RINSE_1 , HARDWARE_ACTIVE);
			ySetHWStatus(HEATING_RINSE_2 , HARDWARE_ACTIVE);
			RinseHeatingState = HEATING;
		}
		
		if (vGetSensorData(WASHING_TEMPERATURE) >= WASHING_TEMPERATURE ) {
			ySetHWStatus(HEATING_RINSE_1 , HARDWARE_OFF);
			ySetHWStatus(HEATING_RINSE_2 , HARDWARE_OFF);
			RinseHeatingState = COOLING;
			
			transmit = OPERATION_ENDED;
			xQueueSend(ProgramHandlerQ , &transmit , 10);
			break;
		}	
		vTaskDelay(100);
	}
	
	if (SENSORSKIP) {
		transmit = OPERATION_ENDED;
		xQueueSend( ProgramHandlerQ , &transmit , 10 );
	}
	
	nUART_TxString("Ended CHECK RINSE TEMPERATURE. \r\n");	
}



// ****************************************************************************************
//	Type		: 	VALUE_RETURN Functions
//	Example		:	vXxXxX();
//	Description	:	Returns a value, no confirmation if successful or not. can be a handle for e.g. ADC pin
// ****************************************************************************************


uint8_t vGetHWStatus	( uint8_t HardwareHandle ) {
/* ******************************************************************
//	Function name : vGetHWStatus
//	Functionality :	Check if bit 1 is set in LSR register, indicating receive data ready flag has been set
// 	Returns				:	1 after a while. Should have a timeout timer as well, to avoid infinite loops in failure conditions	
//  Input range		: None
// *****************************************************************/
	return HardwareLibrary[HardwareHandle][0];
}

// ****************************************************************************************
//	Type		: 	Tasks
//	Example		:	tXx();
//	Description	:	FreeRTOS Task Structure
// ****************************************************************************************
void tProgram_Handler		( void *param ) {
/* *********************************************************************************************************
//	Function name : tProgram_Handler
//	Functionality : Task to read the Queues "ProgramHandlerQ" and "ProgramQ" to call corresponding functions 
										accordingly, to handle the selected machine program.
// 	Returns				:	None
//  Input range		: None
// ********************************************************************************************************/
	
	uint8_t receive;
	uint8_t transmit;
	
	uint8_t	ProgramHandlerState = IDLE;
	uint8_t OutedMsg = 0;
	uint8_t PROGRAM_STARTED = 0;
	
	yWashProgram_2();
	
	while(1) {
		// The program Handler makes sure each operation for every wash program is executed.
		// The Program Handler can be in 3 states:
		//		IDLE : Waiting for a Washing Program to start.
		//		READY: Inbetween Operations in a washing Program (e.g. After CHECK_WASH_TEMPERATURE Operation, before WASH Operation).
		//		BUSY : While an operation is being executed. e.g. While a WASH is running, or HEATING etc.
		switch(ProgramHandlerState) {
			
			case IDLE :
				if (! OutedMsg) {
					OutedMsg = 1;
				}
				
				if (xQueueReceive(ProgramHandlerQ , &receive , 10)) {
					switch(receive) {
						case START_PROGRAM:
							ProgramHandlerState = READY;
							OutedMsg = 0;
						break;
					}
				}
			break;
				
			case READY :
				if (xQueueReceive(ProgramLibrary , &receive, 10)) {
					switch(receive) {
						
						// Define Program Number
						case PROGRAM_1:
							CurrentProgram = PROGRAM_1 * 2 - 1;
						break;
						case PROGRAM_2:
							CurrentProgram = PROGRAM_2 * 2 - 1;
						break;
						case PROGRAM_3:
							CurrentProgram = PROGRAM_3 * 2 - 1;
						break;
						
						// Operation cases
						case CHECK_WATER_LEVEL:
							ProgramHandlerState = BUSY;
							nFillTanksOperation();
						break;
						
						case CHECK_WASH_TEMPERATURE:
							ProgramHandlerState = BUSY;
							nCheckWashTemperature();
						break;
						
						case CHECK_RINSE_TEMPERATURE:
							ProgramHandlerState = BUSY;
							nCheckRinseTemperature();
						break;
						
						case CHECK_SOAP:
							ProgramHandlerState = BUSY;
							nFillSoapOperation();
						break;
						
						case RUN_WASH:
							ProgramHandlerState = BUSY;
							nWashOperation();
						break;
						
						case RUN_RINSE:
							ProgramHandlerState = BUSY;
							nRinseOperation();
						break;
						
						case RUN_WAIT:
							ProgramHandlerState = BUSY;
							nWaitOperation();
						break;
						
						case END_PROGRAM:
							ProgramHandlerState = IDLE;
							CurrentProgram = 0;
							nUART_TxString("\r\nEnded Program Successfully. \r\n \r\n");
						break;
						
						default:
							nUART_TxString("Didn't Understand Input to ProgramLibrary. \r\n");
							break;
					}
				}
			break;
				
			case BUSY :
			if (xQueueReceive(ProgramHandlerQ , &receive , 10)) {
				switch(receive) {
					case OPERATION_ENDED:
						ProgramHandlerState = READY;
					break;
					default:
						break;
				}
			}
			break;
			
			default :
				break;
		}
		vTaskDelay(10);
	}
}
