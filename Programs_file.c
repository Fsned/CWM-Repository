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
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"


// ****************************************************************************************
//
//					Variables
//
// ****************************************************************************************
void (*execution_array[MAX_NUMBER_OF_OPERATIONS])();						// Array Containing pointers to various operations

xQueueHandle ExecutionQ = NULL;																	// Queue to talk to execution task, signalling e.g. program start, program stop, etc.

xQueueHandle ProgramQ		= NULL;

// ****************************************************************************************
//	Type		: 	YES_RETURN Functions
//	Example		:	yXxXxX();
//	Description	:	Returns true (1) or false (0) depending on the success of the function
// ****************************************************************************************
void yWashProgram_1() {
/* ******************************************************************
//	Function name : yWashProgram_1
//	Functionality :	Fills Execution Queue with operations for washing program number 1
// 	Returns				:	True / False depending on whether or not program has been loaded
//  Input range		: None
// *****************************************************************/
	int transmit;
	int receive;
	
	uint8_t NUMBER_OF_OPERATIONS = 7;
	
	void (*program_1_operations[MAX_NUMBER_OF_OPERATIONS])() = {nFillTanksOperation,
																															nFillSoapOperation,
																															nWashOperation,
																															nWaitOperation,
																															nFillSoftenerOperation,
																															nWaitOperation};
	
	
	xQueueSend(ExecutionQ , &transmit , 10);
	
	vTaskDelay(10);
	
	if (xQueueReceive(ProgramQ , &receive , 10)) {
		if (receive == READY_FOR_NEW_PROGRAM) {
			for (int i = 0; i < NUMBER_OF_OPERATIONS; i++);
				
			
			
			
		}
	}
}


void yWashProgram_2() {
/* ******************************************************************
//	Function name : yWashProgram_2
//	Functionality :	Fills Execution Queue with operations for washing program number 2
// 	Returns				:	True / False depending on whether or not program has been loaded
//  Input range		: None
// *****************************************************************/
	if (! uxQueueMessagesWaiting(ExecutionQ)) {

	}	
}


void yWashProgram_3() {
/* ******************************************************************
//	Function name : yWashProgram_3
//	Functionality :	Fills Execution Queue with operations for washing program number 3
// 	Returns				:	True / False depending on whether or not program has been loaded
//  Input range		: None
// *****************************************************************/
	if (! uxQueueMessagesWaiting(ExecutionQ)) {

	}	
}



// ****************************************************************************************
//	Type		: 	NO_RETURN Functions
//	Example		:	nXxXxX();
//	Description	:	Does not return anything.
// ****************************************************************************************
void nFillTanksOperation() {
/*	@Description The operation to fill tanks is not limited by a time duration, but by whether or not the tanks are filled.
			The operation should keep an eye firstly on pressure switches, but also on the current drawn by the inlet pump.
			If anything mishaps, put down all pumps and return an error code to the error handler
	
	
	while (vGetSensorData(PRESSOSWITCH_1) == LOW || vGetSensorData(PRESSOSWITCH_2) == LOW) {
		water_inflow(ON);		// Maybe manipulate bits here, accordingly to where the pump is connected?
	
		if (vGetSensorData(PRESSOSWITCH_1) == HIGH && vGetSensorData(PRESSOSWITCH_2) == HIGH) {
			water_inflow(OFF);		// Also manipulate bits here.
	
			xQueueSend(ExecutionQ , OPERATION_COMPLETE , 10);
			xQueueSend(ExecutionQ , MY_OPERATION_NAME  , 10);
			
		}
	
	vTaskDelay(10);		
	}
	
*/
}

void nFillSoapOperation () {
	/*	@Description
				The Soap operation is supposed to fill soap in the washing tank. 
				if measured soap level reaches 95% of the pre-measured and entered soaplevel, the operation will complete.
				the operation cuts the pump at 90% to allow the soap to settle in the mixture, and then remeasures to see
				if more has to be added.
	
	
	while (vGetSensorData(SOAP_SENSOR) < SOAPLEVEL) {
		soap_inflow(ON);
		
		if (vGetSensorData(SOAP_SENSOR) >= 0.90*SOAPLEVEL && vGetPumpStatus[SOAP_PUMP] == ON) {
			soap_inflow(OFF);
			wait(a_while);
		}
	
		if (vGetSensorData(SOAP_SENSOR) < 0.95*SOAPLEVEL && vGetPumpStatus[SOAP_PUMP] == OFF) {
			soap_inflow(ON);
			wait(a_while);
		}
		
		if (vGetSensorData(SOAP_SENSOR) > 0.95*SOAPLEVEL && vGetPumpStatus[SOAP_PUMP] == OFF) {
			xQueueSend(ExecutionQ , OPERATION_COMPLETE , 10);
			xQueueSend(ExecutionQ , MY_OPERATION_NAME  , 10);
		}
	
		else if (vGetSensorData(SOAP_SENSOR > 0.95*SOAPLEVEL && vGetHWStatus[SOAP_PUMP] == ON) {
			soap_inflow(OFF);
			xQueueSend(ExecutionQ , OPERATION_COMPLETE , 10);
			xQueueSend(ExecutionQ , MY_OPERATION_NAME  , 10);
		}
		
		vTaskDelay(10);
	}
	
	*/
}

void nWashOperation		 () {
	/* @Description
			The washing operation should run for a fixed amount of time. this time will be grabbed from the operation_times library (or so) and loaded at start, then decremented accordingly.
			The water temperature should match the temperature stored in a library (somewhere), and the operation must not begin before the water is at least AT the specific temperature (-.5% degrees)
			And then keep the water temperature with a 3% degree range. (2.4 deg. C. @ 80 deg. C. Washing temperature)
	int wash_timer = timer_library[washing_operation];
	
	while(wash_timer > 0) {
		if (vGetHWStatus(WASH_PUMP_1) == OFF)
			ySetHWStatus(WASH_PUMP_1 , ON);
		if (vGetHWStatus(WASH_PUMP_2) == OFF)
			ySetHWStatus(WASH_PUMP_2 , ON);
	
		if (vGetSensorData(WashTemperature) < 0.97*washing_temperature && vGetHWStatus(WASH_HEATER_1) == OFF) {
			vSetHWStatus(WASH_HEATER_1 , ON);
		}
		
		if (vGetSensorData(WashTemperature) < 0.97*washing_temperature && vGetHWStatus(WASH_HEATER_2) == OFF) {
			vSetHWStatus(WASH_HEATER_2 , ON);
		}
		
	
		
		wash_timer -= 10;
		vTaskDelay(10);
	}
	
	
	*/
}


void nRinseOperation	 () {
	/* @Description
			The Rinsing operation will run for a fixed amount of time. this time will be grabbed from the operation_times library (or so)
			and loaded at start, then decremented accordingly. The rinsing temperature has to match the temperature stored in a library somehere,
			and the operation must not begin before the water is at least AT the specific temperature (-.5% degrees)
			and then keep the water temperature with a 3% degree range. (2.4 deg. C. @ 80 deg. C. Rinsing temperature)
	
	int rinse_timer = timer_library[rinsing_operation];
	
	while (rinse_timer > 0) {
		
		if (vGetHWStatus[RINSE_PUMP_1] == OFF)
			ySetHWStatus(RINSE_PUMP_1 , ON);
		if (vGetHWStatus[RINSE_PUMP_1] == OFF)
			ySetHWStatus(RINSE_PUMP_1 , ON);
	
		if (vGetSensorData(RinseTemperature) < 0.97*rinse_temperature && vGetHWStatus(RINSE_HEATER_1) == OFF) {
			vSetHWStatus(RINSE_HEATER_1 , ON);
		}
		if (vGetSensorData(RinseTemperature < 0.97*rinse_temperature && vGetHWStatus(RINSE_HEATER_2) == OFF) {
			vSetHWStatus(RINSE_HEATER_2 , ON);
		}
		
		rinse_timer -= 10;
		vTaskDelay(10);
	}
	
	
	*/
}

void nWaitOperation		 () {
	/* @Description
			The wait operation can be called between other operations. this could be after a washing operation
			and before a rinsing operation, to allow soap to fall off subjects, to provide a better end result.
	
	int wait_timer = timer_library[wait_operation];
	
	
	while(wait_timer > 0) {
		wait_timer -= 10;
		vTaskDelay(10);
	}
	*/
}

void nFillSoftenerOperation () {
	/* @Description
			The fill softener operation is either timed or controlled by a conductivity sensor. at this moment, the operation is timed.
			Because of this, the task will for the moment also be controlled by a timer. the softener dosage isn't required to be 
			indisputably precise.
	
	int softener_timer = timer_library[softener];
	
	while(softener_timer > 0) {
		if (vGetHWStatus[SOFTENER_PUMP] == OFF)
			vSetHWStatus(SOFTENER_PUMP , ON);
	
		softener_timer -= 10;
		vTaskDelay(10);	
	}
	*/
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

