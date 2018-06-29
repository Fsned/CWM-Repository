/*============================================================================================
 * Source file : Programs_file
 * Author : Frederik Snedevind
 *		    Frs@Jeros.com
 *
 * Company: Jeros A/S
 *		   HTTP://www.Jeros.com/
 *
 * Date created : 9/4 - 2018
 =============================================================================================
 * Description:
 * 	File containing various programs orders of operations, time constraints, included sensors
 *  	and more. use these tasks to load a program into the program buffer. then start 
 *		executing them by running execute_programs();
 *	Should 
 =============================================================================================*/

// ============================================================================================
//
//					Libraries
//
// ============================================================================================
#include "GPIO_control.h"
#include "UART_control.h"

#include "ProgramsHandler.h"
#include "SensorHandler.h"
#include "ErrorHandler.h"

#include "utilities.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

// ============================================================================================
//
//					Variables
//
// ============================================================================================

xQueueHandle OperationQ			= NULL;					/* Queue to talk to execution task, signalling e.g. program start, program stop, etc.	*/
xQueueHandle ProgramLibrary		= NULL;					/* Queue to store Handles for various Operations										*/
xQueueHandle ProgramHandlerQ	= NULL;					/* Queue to signal Operation Start, Operation End, etc.									*/
														 
const uint8_t SENSORSKIP = 0;							 
const uint8_t TIMERSKIP  = 0;							 
														 
uint8_t CurrentProgram = 0;								/* 0 == No Program Selected.															*/

StateType ProgramHandlerState = IDLE;

/* =============================================================================================
																				PROGRAM TIMER LIBRARY
		Used to modify timers for various operations in washing programs. also contains 
		Standard values for all operations, to allow to return settings to default values
		Values are in seconds.
		
		Columns [0][2][4] are STD Values, and should not be changed.
		Columns [1][3][5] are Values that the program operations retrieve, and can be edited.
		Boundaries should be implemented, either here or in the HMI section
   ============================================================================================= */
/* 																				Operation Timer Library									*/
/*							         	   					  P1	P1		P2		P2		P3		P3									*/
/*							         						  STD			STD		 	    STD											*/
/*							         						  [0]	[1]	    [2]		[3]	    [4]	    [5]									*/
uint8_t ProgramTimerLibrary[NumberofOperationTimers][6] = {	{ 180,	180,	180,	180,	180,	180 },				/* 12	Wash	*/
															{ 8,	8,		8,		8,		8,		8	},				/* 13	Wait	*/
															{ 20,	20,		20,		20,		20,		20	},				/* 14	Rinse	*/
															{ 120,	120,	120,	120,	120,	120 },				/* 15	Drain	*/
															{ 0,	0,		0,		0,		0,		0	}};				/* Blowing time */


/* =============================================================================================
																				PARAMETER LIBRARY
Used to Determine various boundaries, such as soap level, rinsing temperature, 
washing temperature, etc.
   ============================================================================================= */
																				
//								{ [0] Value , [1] Minimum , [2] Maximum , [3] Std };
																				
uint16_t ParameterLibrary[NumberOfParameters][4] = {	{	56	,	20	,	70	,	56	}, 					/* WashingTemperature in *C					*/
														{	3	,	1	,	10	,	3	},					/* WashingTemperatureDeviation in %			*/												
														{	84	,	20	,	90	,	84	},					/* RinsingTemperature in *C					*/
														{	3	,	1	,	10	,	3	},					/* RinsingTemperatureDeviation in %			*/
														{	30	,	0	,	99	,	30	},					/* SoapConductivity							*/
														{	3	,	1	,	10	,	3	},					/* SoapConductivityDeviation in %			*/
														{	62	,	0	,	90	,	62	},					/* StandbyTemperatureRinse in *C			*/
														{	3	,	1	,	10	,	3	},					/* StandbyTemperatureRinseDeviation in %	*/
														{	1	,	0	,	1	,	1	},					/* AutomaticLidOpening						*/
														{	0	,	0	,	1	,	0	},					/* TemperatureShownInFahrenheit				*/
														{	0	,	0	,	1	,	0	},					/* ActivateSteamCondenser					*/
														{	1	,	2	,	3	,	4	},
														{	1	,	2	,	3	,	4	}, 
														{	1	,	2	,	3	,	4	},
														{	1	,	2	,	3	,	4	}, 
														{	1	,	2	,	3	,	4	}};

/* =============================================================================================
																				HARDWARE STATUS LIBRARY
Used to keep track of current Hardware status. to see if a certain pump should be flipped or not
depending on current program state. 
   ============================================================================================= */
/*										{[Current HW Status]	, [Not in Use]} */				
	uint16_t HardwareLibrary[15][2] = 	{	{ HardwareOff		, 2 },					
											{ HardwareOff		, 2 },
											{ HardwareOff		, 2 },
											{ HardwareOff		, 2 },
											{ HardwareOff		, 2 },
											{ HardwareOff		, 2 },
											{ HardwareOff		, 2 },
											{ HardwareOff		, 2 },
											{ HardwareOff		, 2 },
											{ HardwareOff		, 2 },
											{ HardwareOff		, 2 },
											{ HardwareOff		, 2 },
											{ HardwareOff		, 2 },
											{ HardwareOff		, 2 },
											{ HardwareOff		, 2 }};
				
/* =============================================================================================
																				HARDWARE PIN MAP
Mapping of all controllable hardware in the machines, to their corresponding PORTS and PINS
Should be remade once new processor hits the table

Currently PinMapped for: LPC1768
Should be remapped to: i.MX RT1020
   ============================================================================================= */
/*										PORT,	PIN																	*/
uint8_t HW_Pinmap [15][2] =  {			{0	,	 9},			/* F0		|| P5	|| WASH PUMP 1					*/
										{0 	,	 8},			/* F1		|| P6	|| WASH PUMP 2					*/
										{0 	,	 7},			/* F2		|| P7	|| RINSE PUMP 1					*/
										{0 	,	 6},			/* F3		|| P8	|| RINSE PUMP 2					*/
										{0 	,	 0},			/* F4		|| P9	|| REVERSAL ENGINE				*/
										{0 	,	 1},			/* F5		|| P10	|| SOAP PUMP					*/
										{0 	,	 18},			/* F6		|| P11	|| SOFTENER PUMP				*/
										{0 	,	 17},			/* F7		|| P12	|| HEATER RINSE 1				*/
										{0 	,	 15},			/* F8		|| P13	|| HEATER RINSE 2				*/
										{0 	,	 16},			/* F9		|| P14	|| HEATER WASH 1				*/
										{1 	,	 30},			/* F10		|| P19	|| HEATER WASH 2				*/
										{1 	,	 31},			/* F11		|| P20	|| CURTAIN ENGINE				*/
										{2 	,	 5},			/* F12		|| P21	|| DRAIN PUMP					*/
										{2 	,	 4},			/* F13		|| P22	|| ELECTROVALVE					*/
										{2 	,	 3}};			/* F14		|| P23	|| REVERSAL ENGINE DIRECTION	*/
																			

// ========================================================================================
//	Type		: BINARY_RETURN Functions
//	Example		: bXxXxX();
//	Description	: Functions that return TRUE (1) or FALSE (0) based on the success of the function.
// ========================================================================================
																			
																			
																			
uint8_t bSetHWStatus ( uint8_t HardwareHandle , uint8_t NewStatus) 
{
/* ******************************************************************
//	Function name	: bSetHWStatus
//	Functionality	:	Set Hardware status to a desired value, if it's not in failure or alarm mode.
// 	Returns			:	true or false. if hw is in failure or alarm mode, returns false, otherwise true	
//  Input range		: 0 - NUMBER_OF_HW - 1 , 0 - 3
// *****************************************************************/
	if (HardwareLibrary[HardwareHandle][0] != HARDWARE_ALARM && HardwareLibrary[HardwareHandle][0] != HARDWARE_FAILURE) 
	{
		HardwareLibrary[HardwareHandle][0] = NewStatus;
		bDigitalWrite(HW_Pinmap[HardwareHandle][0] , HW_Pinmap[HardwareHandle][1] , NewStatus);
		return 1;
	}
	return 0;
}	
																				
// ========================================================================================
//	Type		: 	NO_RETURN Functions
//	Example		:	nXxXxX();
//	Description	:	Does not return anything.
// ========================================================================================																			
																				
void nWashProgram_1() 
{
/* ******************************************************************
//	Function name	: nWashprogram_1
//	Functionality	: Fills Execution Queue with operations for washing program number 1
// 	Returns			: True / False depending on whether or not program has been loaded
//  Input range		: None
// *****************************************************************/
	uint8_t transmit;
	
	uint8_t Program_1_Recipe[10] =  {	START_PROGRAM,
																		PROGRAM_1,
																		CHECK_WATER_LEVEL,
																		CHECK_WASH_TEMPERATURE,
																		CHECK_SOAP,
																		RUN_WASH,
																		RUN_WAIT,
																		CHECK_RINSE_TEMPERATURE,
																		RUN_RINSE,
																		END_PROGRAM};
	
	if (!uxQueueMessagesWaiting(ProgramLibrary)) {
		for (int i = 0; i < (sizeof(Program_1_Recipe) / sizeof(Program_1_Recipe[0])); i++) {
			transmit = Program_1_Recipe[i];
			xQueueSend(ProgramLibrary, &transmit, 10);
		}
		
		transmit = START_PROGRAM;
		xQueueSend(ProgramHandlerQ, &transmit, 10);
		
	}
	vTaskDelay(10);
}



void nWashProgram_2() 
{
/* ==================================================================
  	Function name	: nWashprogram_2
  	Functionality	: Fills Execution Queue with operations for washing program number 2
   	Returns			: True / False depending on whether or not program has been loaded
    Input range		: None
   ================================================================== */
	uint8_t transmit;
	uint8_t Program_2_Recipe[] = {	PROGRAM_2,
									CHECK_WATER_LEVEL,
									CHECK_SOAP,
									CHECK_WASH_TEMPERATURE,
									RUN_WASH,
									RUN_WAIT,
									CHECK_RINSE_TEMPERATURE,
									RUN_RINSE,
									END_PROGRAM};								
																	
	int TotalRuntime = 0;
																				
	TotalRuntime += ProgramTimerLibrary[WashingOperation][3];
	TotalRuntime += ProgramTimerLibrary[RinsingOperation][3];
	TotalRuntime += ProgramTimerLibrary[WaitingOperation][3];
																	
	nUART_TxString("Total Program Runtime: ");														
	nPrintInteger(TotalRuntime);
	nUART_TxString(" Seconds.");
	nNewLine( 1 );													
																	
	if (! uxQueueMessagesWaiting(ProgramLibrary)) 
	{
		for (uint8_t i = 0; i < (sizeof(Program_2_Recipe) / sizeof(Program_2_Recipe[0])); i++) 
		{
			transmit = Program_2_Recipe[i];
			xQueueSend(ProgramLibrary , &transmit , 10);
		}
		
		transmit = START_PROGRAM;
		xQueueSend(ProgramHandlerQ , &transmit , 10);
		
		nUART_TxString("WASH PROGRAM 2: Completed Initialization.");
		nNewLine( 1 );
	}
//	vTaskDelay(10);
}


void nWashProgram_3() 
{
/* ==================================================================
  	Function name	: nWashprogram_3
  	Functionality	: Fills Execution Queue with operations for washing program number 3
   	Returns			: True / False depending on whether or not program has been loaded
    Input range		: None
   ================================================================== */
		uint8_t transmit;
	
		uint8_t Program_3_Recipe[10] = {	START_PROGRAM,
											PROGRAM_3,
											CHECK_WATER_LEVEL,
											CHECK_WASH_TEMPERATURE,
											CHECK_SOAP,
											RUN_WASH,
											RUN_WAIT,
											CHECK_RINSE_TEMPERATURE,
											RUN_RINSE,
											END_PROGRAM
																		};
	
	if (!uxQueueMessagesWaiting(ProgramLibrary)) {
		for (int i = 0; i < (sizeof(Program_3_Recipe) / sizeof(Program_3_Recipe[0])); i++) {
			transmit = Program_3_Recipe[i];
			xQueueSend(ProgramLibrary, &transmit, 10);
		}
		
		transmit = START_PROGRAM;
		xQueueSend(ProgramHandlerQ, &transmit, 10);
		
	}
	vTaskDelay(10);
}


void nFillTanksOperation() 
{
/* =======================================================================================================
  	Function name	: nFillTanksOperation
  	Functionality	: Operation that checks the pressoswitches in both tanks and opens the electrovalve. only fills wash tank if door lid is closed
   	Returns			: None
    Input range		: None
   ======================================================================================================= */
/*	@Description The operation to fill tanks is not limited by a time duration, but by whether or not the tanks are filled.
		The operation should keep an eye firstly on pressure switches, but also on the current drawn by the inlet pump.
		If anything mishaps, put down all pumps and return an error code to the error handler
*/	
	uint8_t transmit;
	nNewLine( 2 );
	nUART_TxString("Started CHECK_WATER_LEVEL.");
	nNewLine( 1 );
	
// =============================================================================
		
	while ( ( vGetSensorData(PRESSOSWITCH_RINSE) == SENSOR_LOW || vGetSensorData(PRESSOSWITCH_WASH) == SENSOR_LOW ) && ! SENSORSKIP) 
	{
		
		if (vGetSensorData(PRESSOSWITCH_RINSE) == SENSOR_LOW)
		{
			if (vGetHWStatus(ELECTROVALVE) == HardwareValveClosed)
				bSetHWStatus(ELECTROVALVE , HardwareValveOpen);
		}
		else if (vGetSensorData(PRESSOSWITCH_RINSE) == SENSOR_HIGH) 
			bSetHWStatus(ELECTROVALVE , HardwareValveOpen);

			
		if (vGetSensorData(PRESSOSWITCH_WASH) == SENSOR_LOW) 
		{
			if (vGetSensorData(DOORSWITCH) == HardwareDoorClosed) 
			{
				if (vGetHWStatus(ELECTROVALVE) == HardwareValveClosed)
					bSetHWStatus(ELECTROVALVE , HardwareValveOpen);
			}
			else 
			{
				/* Wait for door to close. */
				/* display "door not closed" message here */
			}
		}

		if ( vGetSensorData(PRESSOSWITCH_RINSE) == SENSOR_HIGH && vGetSensorData(PRESSOSWITCH_WASH) == SENSOR_HIGH ) 
		{
			bSetHWStatus(ELECTROVALVE, HardwareValveClosed);
			transmit = OPERATION_ENDED;
			xQueueSend(ProgramHandlerQ , &transmit , 10);
		}
		
		/* Check if the program has been paused or stopped, and wait for the change to stop. allow scheduling still, to allow for state change */
		while (ProgramHandlerState == PAUSED || ProgramHandlerState == STOPPED_FOR_SENSORS)
		{
			if (vGetHWStatus(ELECTROVALVE) == HardwareValveOpen)
				bSetHWStatus(ELECTROVALVE, HardwareValveClosed);

			vTaskDelay(25);			/* Set a delay of ~25 milliseconds. Keep checking the state for updates. if the state changes, resume the program. */
		}

		vTaskDelay(100);
	}
// =============================================================================
	if (SENSORSKIP) 
	{
		transmit = OPERATION_ENDED;
		xQueueSend(ProgramHandlerQ , &transmit , 10);
	}
	
	nUART_TxString("Ended CHECK_WATER_LEVEL.");
	nNewLine( 1 );
}

void nFillSoapOperation()
{
/* ========================================================================================================
  	Function name	: nFillSoapOperation
  	Functionality	: Fills soap in the wash tank, up to the set level
   	Returns			: None
    Input range		: None
   ======================================================================================================== */
	/*	@Description
				The Soap operation is supposed to fill soap in the washing tank. 
				if measured soap level reaches 95% of the pre-measured and entered soaplevel, the operation will complete.
				the operation cuts the pump at 90% to allow the soap to settle in the mixture, and then remeasures to see
				if more has to be added.
	*/
	uint8_t transmit;
 
	nNewLine( 2 );
	nUART_TxString("Started CHECK_SOAP Operation.");
	nNewLine( 1 );
// ==========================================================================================================	
	while (vGetSensorData(SOAP_SENSOR) < 0.95 * SOAP_LEVEL && ! SENSORSKIP) 
	{
		bSetHWStatus(SOAP_PUMP , HardwareActive);
		
		if (vGetSensorData(SOAP_SENSOR) >= 0.95*SOAP_LEVEL && vGetHWStatus(SOAP_PUMP) == HardwareActive)
		{
			bSetHWStatus(SOAP_PUMP , HardwareOff);
			vTaskDelay(1000);
		}
	
		if (vGetSensorData(SOAP_SENSOR) < 0.95*SOAP_LEVEL && vGetHWStatus(SOAP_PUMP) == HardwareOff)
		{
			bSetHWStatus( SOAP_PUMP , HardwareActive);
			vTaskDelay(1000);
		}
		
		if (vGetSensorData(SOAP_SENSOR) > 0.95*SOAP_LEVEL && vGetHWStatus(SOAP_PUMP) == HardwareOff)
		{
			transmit = OPERATION_ENDED;
			xQueueSend(ProgramHandlerQ , &transmit , 10);
		}
	
		else if (vGetSensorData(SOAP_SENSOR) > 0.95*SOAP_LEVEL && vGetHWStatus(SOAP_PUMP) == HardwareActive)
		{
			bSetHWStatus( SOAP_PUMP , HardwareOff);
			transmit = OPERATION_ENDED;
			xQueueSend(ProgramHandlerQ , &transmit , 10);
		}

		/* Check if the program has been paused or stopped, and wait for the change to stop. allow scheduling still, to allow for state change */
		while (ProgramHandlerState == PAUSED || ProgramHandlerState == STOPPED_FOR_SENSORS)
		{
			if (vGetHWStatus(SOAP_PUMP) == HardwareActive)
				bSetHWStatus(SOAP_PUMP, HardwareOff);
				

			vTaskDelay(25);			/* Set a delay of ~25 milliseconds. Keep checking the state for updates. if the state changes, resume the program. */
		}
		
		vTaskDelay(100);
	}
	
	
// ==========================================================================================================
	
	nUART_TxString("Ended CHECK_SOAP.");
	nNewLine( 1 );
	
	if (SENSORSKIP) 
	{
		transmit = OPERATION_ENDED;
		xQueueSend(ProgramHandlerQ , &transmit , 10);
	}
	
	vTaskDelay(100);	
}

void nWashOperation() 
{
/* *********************************************************************************************************
//	Function name	: nWashOperation
//	Functionality	: Runs the wash operation, turns on wash pumps, decrements timer, keeps track of the 
						temperature and keeps it up if it falls below washing (temperature - a set percentage)
// 	Returns			:	None
//  Input range		: None
// ********************************************************************************************************/
	
	/* @Description
			The washing operation should run for a fixed amount of time. this time will be grabbed from the operation_times library (or so) and loaded at start, then decremented accordingly.
			The water temperature should match the temperature stored in a library (somewhere), and the operation must not begin before the water is at least AT the specific temperature (-.5% degrees)
			And then keep the water temperature with a 3% degree range. (2.4 deg. C. @ 80 deg. C. Washing temperature)
	*/
	int timer = ProgramTimerLibrary[WashingOperation][CurrentProgram] * 1000;			/* Setup a timer to time the wash operation */
	
	typedef enum																		/* Define various states the Heating can be in during the operation*/
	{
		NOT_INIT,
		COOLING,
		HEATING
	} HEATING_STATE;			
	
	
	HEATING_STATE HeatingStatus	= NOT_INIT;												/* Start the */
	uint8_t Reversal_Direction = 0;			// Either 0 for towards the user (forwards) or 1 for towards the back of the machine (backwards).
	uint8_t transmit;
	nNewLine( 2 );
	nUART_TxString("Started RUN_WASH Operation.");
	
	nNewLine( 1 );
	nUART_TxString("Operation Runtime: ");
	
	nPrintInteger(ProgramTimerLibrary[WashingOperation][CurrentProgram]);
	
	nUART_TxString(" Seconds.");
	nNewLine( 1 );
	
	while ( timer && ! TIMERSKIP) {
		
		if (vGetHWStatus(WASH_PUMP_1) == HardwareOff)
			bSetHWStatus(WASH_PUMP_1 , HardwareActive);
		
		if (vGetHWStatus(WASH_PUMP_2) == HardwareOff)
			bSetHWStatus(WASH_PUMP_2 , HardwareActive);
		
		if (vGetHWStatus(REVERSAL_ENGINE) == HardwareOff)
			bSetHWStatus(REVERSAL_ENGINE , HardwareActive);
		
		if (vGetSensorData(TEMPERATURE_SENSOR_WASH) < (1 - (ParameterLibrary[0][WashingTemperatureDeviation]/100)) * ParameterLibrary[0][WashingTemperature] && HeatingStatus == COOLING) /* Turn ON the heaters if temperature is below the set temperature, minus a deviation found in the parameter library*/
		{
			bSetHWStatus(HEATING_WASH_1, HardwareActive);
			bSetHWStatus(HEATING_WASH_2, HardwareActive);
			HeatingStatus = HEATING;
		}
		
		if (vGetSensorData(TEMPERATURE_SENSOR_WASH) >= WASHING_TEMPERATURE && HeatingStatus == HEATING) 
		{
			bSetHWStatus(HEATING_WASH_1, HardwareOff);
			bSetHWStatus(HEATING_WASH_2, HardwareOff);
			HeatingStatus = COOLING;
		}
		
		
		if (vGetSensorData(REVERSAL_CURRENT_SENSOR) > REVERSAL_CURRENT_MAX) 
		{
			
			bSetHWStatus(REVERSAL_ENGINE, HardwareOff);
			
			if (Reversal_Direction) {
				Reversal_Direction = 0;
				bSetHWStatus(REVERSAL_ENG_DIRECTION, HardwareOff);
			}
			else {
				Reversal_Direction = 1;
				bSetHWStatus(REVERSAL_ENG_DIRECTION, HardwareActive);
			}
			bSetHWStatus(REVERSAL_ENGINE, HardwareActive);
		}
		
		/* Check if the program has been paused or stopped, and wait for the change to stop. allow scheduling, to allow for state change */
		while (ProgramHandlerState == PAUSED || ProgramHandlerState == STOPPED_FOR_SENSORS)
		{
			if (vGetHWStatus(WASH_PUMP_1) == HardwareActive)
				bSetHWStatus(WASH_PUMP_1, HardwareOff);
			
			if (vGetHWStatus(WASH_PUMP_2) == HardwareActive)
				bSetHWStatus(WASH_PUMP_2, HardwareOff);
			
			if (vGetHWStatus(HEATING_WASH_1) == HardwareActive)
				bSetHWStatus(HEATING_WASH_1, HardwareOff);
			
			if (vGetHWStatus(HEATING_WASH_2) == HardwareActive)
				bSetHWStatus(HEATING_WASH_2, HardwareOff);
			
			if (vGetHWStatus(REVERSAL_ENGINE) == HardwareActive)
				bSetHWStatus(REVERSAL_ENGINE, HardwareOff);
				
			HeatingStatus = COOLING;
			
			vTaskDelay(25); /* Set a delay of ~25 milliseconds. Keep checking the state for updates. if the state changes, resume the program. */
		}
		
		timer -= 100;
		vTaskDelay(100);
		
		if (! timer ) {
			transmit = OPERATION_ENDED;
			xQueueSend(ProgramHandlerQ , &transmit , 10);
			
			bSetHWStatus(WASH_PUMP_1 		, HardwareOff);
			bSetHWStatus(WASH_PUMP_2		, HardwareOff);
			bSetHWStatus(REVERSAL_ENGINE	, HardwareOff);
			bSetHWStatus(HEATING_WASH_1		, HardwareOff);
			bSetHWStatus(HEATING_WASH_2		, HardwareOff);
		}
	}
	if (TIMERSKIP) 
	{
		transmit = OPERATION_ENDED;
		xQueueSend( ProgramHandlerQ , &transmit , 10 );
	}
	
	nUART_TxString("Ended RUN WASH Operation.");
	nNewLine( 1 );
}


void nRinseOperation() 
{
/* ========================================================================================================
  	Function name	: nRinseOperation
  	Functionality	: Runs the rinse operation
   	Returns			: None
    Input range		: None
   ======================================================================================================== */
	/* @Description
			The Rinsing operation will run for a fixed amount of time. this time will be grabbed from the operation_times library (or so)
			and loaded at start, then decremented accordingly. The rinsing temperature has to match the temperature stored in a library somehere,
			and the operation must not begin before the water is at least AT the specific temperature (-.5% degrees)
			and then keep the water temperature with a 3% degree range. (2.4 deg. C. @ 80 deg. C. Rinsing temperature)
	*/
	
	int timer = ProgramTimerLibrary[RinsingOperation][CurrentProgram] * 1000;

	typedef enum
	{
		NOT_INIT,
		COOLING,
		HEATING
	} HEATING_STATE;
	
	HEATING_STATE HeatingStatus	= NOT_INIT;
	uint8_t Reversal_Direction 	= 0;
	uint8_t transmit;
	
	nNewLine( 2 );
	nUART_TxString("Started RUN_RINSE Operation.");
	nNewLine( 1 );
	nUART_TxString("Operation Runtime: ");
	nPrintInteger(ProgramTimerLibrary[RinsingOperation][CurrentProgram]);
	nUART_TxString(" Seconds.");
	nNewLine( 1 );

	while ( timer && ! TIMERSKIP) 
	{
		
		if (vGetHWStatus(RINSE_PUMP_1) == HardwareOff)
			bSetHWStatus(RINSE_PUMP_1 , HardwareActive);
		
		if (vGetHWStatus(RINSE_PUMP_2) == HardwareOff)
			bSetHWStatus(RINSE_PUMP_2 , HardwareActive);
		
		if (vGetHWStatus(REVERSAL_ENGINE) == HardwareOff)
			bSetHWStatus(REVERSAL_ENGINE , HardwareActive);
		
		if (vGetSensorData(TEMPERATURE_SENSOR_RINSE) < (1 - (TEMPERATURE_DEVIATION/100)) * RINSING_TEMPERATURE && HeatingStatus == COOLING) 
		{
			bSetHWStatus(HEATING_RINSE_1, HardwareActive);
			bSetHWStatus(HEATING_RINSE_2, HardwareActive);
			HeatingStatus = HEATING;
		}
		
		if (vGetSensorData(TEMPERATURE_SENSOR_RINSE) >= RINSING_TEMPERATURE && HeatingStatus == HEATING) 
		{
			bSetHWStatus(HEATING_RINSE_1 , HardwareOff);
			bSetHWStatus(HEATING_RINSE_2 , HardwareOff);
			HeatingStatus = COOLING;
		}
		
		
		if (vGetSensorData(REVERSAL_CURRENT_SENSOR) > REVERSAL_CURRENT_MAX) 
		{
			bSetHWStatus(REVERSAL_ENGINE , HardwareOff);
			if (Reversal_Direction) 
			{
				Reversal_Direction = 0;
				bSetHWStatus(REVERSAL_ENG_DIRECTION , HardwareOff);
			}
			else 
			{
				Reversal_Direction = 1;
				bSetHWStatus(REVERSAL_ENG_DIRECTION, HardwareActive);
			}
			bSetHWStatus(REVERSAL_ENGINE, HardwareActive);
		}
		/* Check if the program has been paused or stopped, and wait for the change to stop. allow scheduling still, to allow for state change */
		while (ProgramHandlerState == PAUSED || ProgramHandlerState == STOPPED_FOR_SENSORS)
		{
			if (vGetHWStatus(RINSE_PUMP_1) == HardwareActive)
				bSetHWStatus(RINSE_PUMP_1, HardwareOff);
			
			if (vGetHWStatus(RINSE_PUMP_2) == HardwareActive)
				bSetHWStatus(RINSE_PUMP_2, HardwareOff);
			
			if (vGetHWStatus(HEATING_RINSE_1) == HardwareActive)
				bSetHWStatus(HEATING_RINSE_1, HardwareOff);
			
			if (vGetHWStatus(HEATING_RINSE_2) == HardwareActive)
				bSetHWStatus(HEATING_RINSE_2, HardwareOff);
			
			if (vGetHWStatus(REVERSAL_ENGINE) == HardwareActive)
				bSetHWStatus(REVERSAL_ENGINE, HardwareOff);
				
			HeatingStatus = COOLING;
			
			vTaskDelay(25); /* Set a delay of ~25 milliseconds. Keep checking the state for updates. if the state changes, resume the program. */
		}
		
		
		timer -= 100;
		vTaskDelay(100);
		
		if (! timer ) 
		{
			transmit = OPERATION_ENDED;
			xQueueSend(ProgramHandlerQ , &transmit , 10);
			
			bSetHWStatus(RINSE_PUMP_1 		, HardwareOff);
			bSetHWStatus(RINSE_PUMP_2 		, HardwareOff);
			bSetHWStatus(REVERSAL_ENGINE 	, HardwareOff);
			bSetHWStatus(HEATING_RINSE_1	, HardwareOff);
			bSetHWStatus(HEATING_RINSE_2	, HardwareOff);
		}
	}
	
	nUART_TxString("Ended RUN RINSE Operation.");
	nNewLine( 1 );
	
	if (TIMERSKIP) 
	{
		transmit = OPERATION_ENDED;
		xQueueSend( ProgramHandlerQ , &transmit , 10 );
	}
}

void nWaitOperation() 
{
/* =========================================================================================================
  	Function name	: nWaitOperation
  	Functionality	: Waits for a fixxed amount of time, between wash- and rinse operations to allow soap to fall off subjects etc.
   	Returns			: None
    Input range		: None
   ========================================================================================================= */
	/* @Description
			The wait operation can be called between other operations. this could be after a washing operation
			and before a rinsing operation, to allow soap to fall off subjects, to provide a better end result.
	*/
	int timer = ProgramTimerLibrary[WaitingOperation][CurrentProgram] * 1000;
	uint8_t transmit;
 
	nNewLine( 1 );
	nUART_TxString("Started WAIT Operation.");
	nNewLine( 1 );
	nUART_TxString("Operation Runtime: ");
	nPrintInteger(ProgramTimerLibrary[WaitingOperation][CurrentProgram]);
	nUART_TxString(" Seconds.");
	nNewLine( 1 );
		
	while ( timer && ! TIMERSKIP) 
	{
		/* Check if the program has been paused or stopped, and wait for the change to stop. allow scheduling still, to allow for state change */
		while (ProgramHandlerState == PAUSED || ProgramHandlerState == STOPPED_FOR_SENSORS)
		{
			/* Do nothing, just pause the waiting operation. */
			
			vTaskDelay(25); /* Set a delay of ~25 milliseconds. Keep checking the state for updates. if the state changes, resume the program. */
		}
		
		timer -= 10;
		vTaskDelay(10);
		
		if (! timer ) 
		{
			transmit = OPERATION_ENDED;
			xQueueSend(ProgramHandlerQ , &transmit , 10);
		}
	}
	
	nUART_TxString("Ended WAIT Operation.");
	nNewLine( 1 );
	
	if (TIMERSKIP) 
	{
		transmit = OPERATION_ENDED;
		xQueueSend( ProgramHandlerQ , &transmit , 10 );
	}
}

void nFillSoftenerOperation() 
{
/* =========================================================================================================
  	Function name	: nFillSoftenerOperation
  	Functionality	: Fills softener into the rinsing tank. this might not be necessary, 
  					  unless a new electrical pump is implemented in the machines.
   	Returns			: None
    Input range		: None
   ========================================================================================================= */
	/* @Description
			The fill softener operation is either timed or controlled by a conductivity sensor. at this moment, the operation is timed.
			Because of this, the task will for the moment also be controlled by a timer. the softener dosage isn't required to be 
			indisputably precise.
	*/
			
	typedef enum
	{
		NOT_INIT,
		FILLING,
		FALLING
	} SOFTENER_PUMP_STATES;
	
	SOFTENER_PUMP_STATES SoftenerPumpState = NOT_INIT;
	uint8_t transmit;
	
	nNewLine( 2 );
	nUART_TxString("Started CHECK_SOFTENER Operation.");
	nNewLine( 1 );
	
	while (vGetSensorData(SOFTENER_SENSOR) < SOFTENER_LEVEL && ! SENSORSKIP) 
	{
		if ( vGetSensorData(SOFTENER_SENSOR) < (1 - ( SOFTENER_DEVIATION / 100)) * SOFTENER_LEVEL && (SoftenerPumpState == FALLING || SoftenerPumpState == NOT_INIT)) 
		{
			bSetHWStatus( SOFTENER_PUMP , HardwareActive );
			SoftenerPumpState = FILLING;
		}
		
		if (vGetSensorData(SOFTENER_SENSOR) >= SOFTENER_LEVEL && SoftenerPumpState == FILLING) 
		{
			bSetHWStatus( SOFTENER_PUMP , HardwareOff );
			SoftenerPumpState = FALLING;
		}
		
		if (vGetSensorData(SOFTENER_SENSOR) > SOFTENER_LEVEL) 
		{
			bSetHWStatus( SOFTENER_PUMP , HardwareOff );
			SoftenerPumpState = FALLING;
			
			transmit = OPERATION_ENDED;
			xQueueSend(ProgramHandlerQ , &transmit , 10);
			break;
		}
		
		/* Check if the program has been paused or stopped, and wait for the change to stop. allow scheduling still, to allow for state change */
		while (ProgramHandlerState == PAUSED || ProgramHandlerState == STOPPED_FOR_SENSORS)
		{
			/* Do nothing, just pause the waiting operation. */
			if (vGetHWStatus(SOFTENER_PUMP) == HardwareActive)
				bSetHWStatus(SOFTENER_PUMP, HardwareOff);
			
			SoftenerPumpState = FALLING;
			
			vTaskDelay(25); /* Set a delay of ~25 milliseconds. Keep checking the state for updates. if the state changes, resume the program. */
		}
		
		nUART_TxString("Stopped nFillSoftenerOperation.");
		nNewLine( 1 );
		vTaskDelay(100);
	}
	
	if (SENSORSKIP) 
	{
		transmit = OPERATION_ENDED;
		xQueueSend( ProgramHandlerQ , &transmit , 10);
	}
}






void nCheckWashTemperature() 
{
/* =========================================================================================================
  	Function name	: nCheckWashTemperature
  	Functionality	: Checks the washing temperature, before starting a washing program, and warms the water if needed
   	Returns			: None
    Input range		: None
   ========================================================================================================= */
	
	typedef enum
	{
		NOT_INIT,
		HEATING,
		COOLING
	} WASH_HEATING_STATES;
	
	WASH_HEATING_STATES WashHeatingState = NOT_INIT;
	
	uint8_t transmit;
	
	nNewLine( 2 );
	nUART_TxString("Started CHECK WASH TEMPERATURE.");
	nNewLine( 1 );
	
	while ( vGetSensorData(WASHING_TEMPERATURE) < WASHING_TEMPERATURE && ! SENSORSKIP) 
	{
		
		if (vGetSensorData(WASHING_TEMPERATURE) < (1 - (TEMPERATURE_DEVIATION / 100)) * WASHING_TEMPERATURE && (WashHeatingState == COOLING || WashHeatingState == NOT_INIT)) 
		{
			bSetHWStatus(HEATING_WASH_1 , HardwareActive);
			bSetHWStatus(HEATING_WASH_2 , HardwareActive);
			WashHeatingState = HEATING;
		}
		
		if (vGetSensorData(WASHING_TEMPERATURE) >= WASHING_TEMPERATURE ) 
		{
			bSetHWStatus(HEATING_WASH_1 , HardwareOff);
			bSetHWStatus(HEATING_WASH_2 , HardwareOff);
			WashHeatingState = COOLING;
			
			transmit = OPERATION_ENDED;
			xQueueSend(ProgramHandlerQ , &transmit , 10);
		}
		
		/* Check if the program has been paused or stopped, and wait for the change to stop. allow scheduling still, to allow for state change */
		while (ProgramHandlerState == PAUSED || ProgramHandlerState == STOPPED_FOR_SENSORS)
		{
			/* Do nothing, just pause the waiting operation. */
			if (vGetHWStatus(HEATING_WASH_1) == HardwareActive)
				bSetHWStatus(HEATING_WASH_1, HardwareOff);
			
			if (vGetHWStatus(HEATING_WASH_2) == HardwareActive)
				bSetHWStatus(HEATING_WASH_2, HardwareOff);
			
			WashHeatingState = COOLING;
			
			vTaskDelay(25); /* Set a delay of ~25 milliseconds. Keep checking the state for updates. if the state changes, resume the program. */
		}
		
		vTaskDelay(100);
	}
	
	if (SENSORSKIP) 
	{
		transmit = OPERATION_ENDED;
		xQueueSend( ProgramHandlerQ , &transmit , 10 );
	}
	
	nUART_TxString("Ended CHECK WASH TEMPERATURE.");
	nNewLine( 1 );
	
}

void nCheckRinseTemperature() 
{
/* ========================================================================================================
  	Function name	: nCheckRinseTemperature
  	Functionality	: Checks the rinsing temperature, before starting a rinsing program, and warms the water if needed
   	Returns			: None
    Input range		: None
   ======================================================================================================== */
				
	typedef enum
	{
		NOT_INIT,
		HEATING,
		COOLING
	} RINSE_HEATING_STATES;
	
	RINSE_HEATING_STATES RinseHeatingState = NOT_INIT;
	uint8_t transmit;
	
	nNewLine( 2 );
	nUART_TxString("Started CHECK RINSE TEMPERATURE.");
	nNewLine( 1 );
	
	while ( vGetSensorData(RINSING_TEMPERATURE) < RINSING_TEMPERATURE) 
	{
		
		if (vGetSensorData(RINSING_TEMPERATURE) < (1 - (TEMPERATURE_DEVIATION / 100)) * RINSING_TEMPERATURE && (RinseHeatingState == COOLING || RinseHeatingState == NOT_INIT)) 
		{
			bSetHWStatus(HEATING_RINSE_1 , HardwareActive);
			bSetHWStatus(HEATING_RINSE_2 , HardwareActive);
			RinseHeatingState = HEATING;
		}
		
		if (vGetSensorData(WASHING_TEMPERATURE) >= WASHING_TEMPERATURE ) 
		{
			bSetHWStatus(HEATING_RINSE_1 , HardwareOff);
			bSetHWStatus(HEATING_RINSE_2 , HardwareOff);
			RinseHeatingState = COOLING;
			
			transmit = OPERATION_ENDED;
			xQueueSend(ProgramHandlerQ , &transmit , 10);
			break;
		}	
		
		/* Check if the program has been paused or stopped, and wait for the change to stop. allow scheduling still, to allow for state change */
		while (ProgramHandlerState == PAUSED || ProgramHandlerState == STOPPED_FOR_SENSORS)
		{
			/* Do nothing, just pause the waiting operation. */
			if (vGetHWStatus(HEATING_RINSE_1) == HardwareActive)
				bSetHWStatus(HEATING_RINSE_1, HardwareOff);
			
			if (vGetHWStatus(HEATING_RINSE_2) == HardwareActive)
				bSetHWStatus(HEATING_RINSE_2, HardwareOff);
			
			RinseHeatingState = COOLING;
			
			vTaskDelay(25); /* Set a delay of ~25 milliseconds. Keep checking the state for updates. if the state changes, resume the program. */
		}
		
		vTaskDelay(100);
	}
	
	if (SENSORSKIP) 
	{
		transmit = OPERATION_ENDED;
		xQueueSend( ProgramHandlerQ , &transmit , 10 );
	}
	
	nUART_TxString("Ended CHECK RINSE TEMPERATURE.");	
	nNewLine( 1 );
}
/* ========================================================================================================
  	Function name	: nEmptyTanks
  	Functionality	: Empty both Wash and Rinse tanks
   	Returns			: None
    Input range		: None
   ======================================================================================================== */
void nEmptyTanks()
{
	// Get timer from TimerLibrary, and convert from seconds to milleseconds, (Base time unit of FreeRTOS)
	uint32_t	timer = ProgramTimerLibrary[DrainingOperation][CurrentProgram] * 1000;
	uint8_t		transmit;
	
	// Output operation init to UART
	nNewLine(1);
	nUART_TxString("Started Draining Operation.");
	nNewLine(1);
	nUART_TxString("Operation Runtime: ");
	nPrintInteger(ProgramTimerLibrary[DrainingOperation][CurrentProgram]);
	nUART_TxString(" Seconds.");
	nNewLine(1);
	// Run operation based on the timer, skip if TIMERSKIP is set
	while(timer && !TIMERSKIP)
	{
		// Check if timer has run out
		if (!timer) 
		{
			transmit = OPERATION_ENDED;
			xQueueSend(ProgramHandlerQ, &transmit, 10);
		}
		
		if (vGetHWStatus(DRAIN_PUMP) == HardwareOff)
		{
			if (vGetSensorData(DOORSWITCH) == HardwareDoorOpen)
			{
				/*
				 * Display Door Open Message
				 **/
			}
			else if (vGetSensorData(DOORSWITCH) == HardwareDoorClosed)
			{
				bSetHWStatus(DRAIN_PUMP, HardwareActive);
			}
		}
		
		/* Check if the program has been paused or stopped, and wait for the change to stop. allow scheduling still, to allow for state change */
		while (ProgramHandlerState == PAUSED || ProgramHandlerState == STOPPED_FOR_SENSORS)
		{
			/* Do nothing, just pause the waiting operation. */
			if (vGetHWStatus(DRAIN_PUMP) == HardwareActive)
				bSetHWStatus(DRAIN_PUMP, HardwareOff);
			
			
			vTaskDelay(25); /* Set a delay of ~25 milliseconds. Keep checking the state for updates. if the state changes, resume the program. */
		}
		
		// Decrement timer, schedule to other task
		timer -= 10;
		vTaskDelay(10);
	}
	
	nUART_TxString("Ended Draining Operation.");
	nNewLine(1);
	
	if (TIMERSKIP) 
	{
		transmit = OPERATION_ENDED;
		xQueueSend(ProgramHandlerQ, &transmit, 10);
	}
}



/* ========================================================================================
  	Type		: 	VALUE_RETURN Functions
  	Example		:	vXxXxX();
  	Description	:	Returns a value, no confirmation if successful or not. can be a handle for e.g. ADC pin
   ======================================================================================== */


uint8_t vGetHWStatus	( uint8_t HardwareHandle ) 
{
/* ========================================================================================
  	Function name	: vGetHWStatus
  	Functionality	: Check if bit 1 is set in LSR register, indicating receive data ready flag has been set
   	Returns			: 1 after a while. Should have a timeout timer as well, to avoid infinite loops in failure conditions	 
    Input range		: None
   ======================================================================================== */
	return HardwareLibrary[HardwareHandle][0];
}

/* ========================================================================================
  	Type		: 	Tasks
  	Example		:	tXx();
  	Description	:	FreeRTOS Task Structure
   ======================================================================================== */
void tProgram_Handler		( void *param ) 
{
/* ========================================================================================
  	Function name	: tProgram_Handler
  	Functionality	: Task to read the Queues "ProgramHandlerQ" and "ProgramQ" to call corresponding functions 
  						accordingly, to handle the selected machine program.
   	Returns			: None
    Input range		: None
   ======================================================================================== */
	
	uint8_t receive;

	uint8_t OutedMsg 			= 0;
//	uint8_t PROGRAM_STARTED 	 	= 0;
	
	/* This Functioncall should be removed once debugging / dev. stage is finished */
	/* DEBUGGING - Should be removed before V1.0 */	nWashProgram_2();
	
	while(1) 
	{
		// The program Handler makes sure each operation for every wash program is executed.
		// The Program Handler can be in 3 states:
		//		IDLE : Waiting for a Washing Program to start.
		//		READY: Inbetween Operations in a washing Program (e.g. After CHECK_WASH_TEMPERATURE Operation, before WASH Operation).
		//		BUSY : While an operation is being executed. e.g. While a WASH is running, or HEATING etc.
		switch(ProgramHandlerState) 
		{
			
			case IDLE :
				if (! OutedMsg) 
				{
					OutedMsg = 1;
				}
				
				if (xQueueReceive(ProgramHandlerQ , &receive , 10)) 
				{
					switch(receive) 
					{
						case START_PROGRAM:
							ProgramHandlerState = READY;
							OutedMsg = 0;
						break;
					}
				}
			break;
				
			case READY :
				if (xQueueReceive(ProgramLibrary , &receive, 10)) 
				{
					switch(receive) 
					{
						
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
							nNewLine( 1 );
							if (!vGetErrorCounter())	/* If Error counter is 0, type out success message to UART */
							{
								nUART_TxString("Ended Program Successfully.");
							}
							else						/* If errors exist, output error counter value to UART*/
							{
								nUART_TxString("Ended Program with ");
								nPrintInteger(vGetErrorCounter());
								nUART_TxString(" Error(s).");
							}
							
							nNewLine( 2 );
						break;
						
						default:
							nUART_TxString("Didn't Understand Input to ProgramLibrary. \r\n");
						break;
					}
				}
			break;
				
			case BUSY :
			if (xQueueReceive(ProgramHandlerQ , &receive , 10)) 
			{
				switch(receive) 
				{
					case OPERATION_ENDED:
						ProgramHandlerState = READY;
					break;
					
					case PAUSE_PROGRAM:
						ProgramHandlerState = PAUSED;
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
