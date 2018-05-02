// ==========================================================
//	Controller software for Jeros A/S' Coarse washing Machines
//	All copyright
//
//	Copyright Notice :
//
//	Year of Creation : 2018
//	Author : Frederik Snedevind , Head of Software Development, Jeros A/S
//	� :
//		All rights reserved for Jeros A/S.
//		No Distribution of this software for other use than
//		originally intended is allowed. Cannot be used for
//		any kind of exercises, further development within any
//    other establishments than Jeros A/S.
//
//
// ==========================================================



#include <lpc17xx.h>
#include "utilities.h"
#include "stdio.h"

#include "Programs_file.h"
#include "LED_control.h"
#include "GPIO_setup.h"
#include "Sensor_file.h"
#include "UART_control.h"


#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

#define TASK_STACK_SIZE 32



int main()
{
	// Create variable to accumulate return status from taskcreation process, to detect if any tasks are not created
	BaseType_t xReturned = pdPASS;
	
	// Initialize system clock, interrupts, and more.
	SystemInit();                    //Clock and PLL configuration
	
	// Setup various GPIO, LEDS, Wavegen inputs
//	nGPIOSetup();
	
	// Initialize the UART Channel 0 to 9600 baud
	nUART0_init(9600);
	
//		// Create a task.
		xReturned &= xTaskCreate( tUART_RxTask , "UART Receive"		, 256, NULL, configMAX_PRIORITIES - 1, NULL );				// Task to Receive input from the terminal, and process it in the program
		xReturned &= xTaskCreate( tUART_TxTask , "UART Transmit"	, 256, NULL, configMAX_PRIORITIES - 1, NULL ); 			// Task to transmit response and info to the terminal to assist and validate the user
//		xReturned &= xTaskCreate( tSensor_Task , "Sensor Handler"    , 24, NULL, configMAX_PRIORITIES - 1, NULL );		// Task to setup, control and collect data from sensors
//		xReturned &= xTaskCreate(tProgram_Handler,"Program Handler", 128 , NULL, configMAX_PRIORITIES - 1, NULL );		// Task to execute programs
////////		
//		if (xReturned == pdPASS)
//			xTaskCreate( tLEDAlive 	, 	"LED Alive task"	, 32 , NULL, configMAX_PRIORITIES - 1, &AliveHandle );
	
/* *********************************************		
//
//									Queues
//		
***********************************************/		
	
	
	
	qUART_RxQ		= xQueueCreate(24 , sizeof(char));
	qUART_TxQ		= xQueueCreate(24 , sizeof(char));
	SensorQ			= xQueueCreate(24 , sizeof(uint8_t));
	
	ProgramHandlerQ = xQueueCreate(12 , sizeof(uint8_t));
	ProgramLibrary	= xQueueCreate(12 , sizeof(uint8_t));
	OperationQ 			= xQueueCreate(12 , sizeof(uint8_t));
	
	// Start FreeRTOS scheduler.
	vTaskStartScheduler();
}



























#if ( configUSE_MALLOC_FAILED_HOOK == 1 )
void vApplicationMallocFailedHook( void )
{
    taskDISABLE_INTERRUPTS();
		nTerminal_LED_ALL_ON();
//    for( ;; )
//    {
//        nLED_SET(LED_DONT_CARE,LED_DONT_CARE,LED_DONT_CARE,1);
//        delay_ms(1000);
//        nLED_SET(LED_DONT_CARE,LED_DONT_CARE,LED_DONT_CARE,0);
//        delay_ms(1000);
//    }
}
#endif

#if ( configCHECK_FOR_STACK_OVERFLOW > 0 )
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
    ( void ) pcTaskName;
    ( void ) pxTask;

    taskDISABLE_INTERRUPTS();
		nTerminal_LED_ALL_ON();
//	
//    for( ;; )
//    {
//        nLED_SET(LED_DONT_CARE,LED_DONT_CARE,LED_DONT_CARE,1);
//        delay_ms(500);
//        nLED_SET(LED_DONT_CARE,LED_DONT_CARE,LED_DONT_CARE,0);
//        delay_ms(500);
//    }
}
#endif


