// ==========================================================
//	Controller software for Jeros A/S' Coarse washing Machines
//	All copyright
//
//	Copyright Notice :
//
//	Year of Creation : 2018
//	Author : Frederik Snedevind , Head of Software Development, Jeros A/S
//	© :
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
	
	BaseType_t xReturned = pdPASS;
	
	SystemInit();                    //Clock and PLL configuration
//	nGPIOSetup();
	nUART0_init(9600);
	
//		// Create a task.
		xReturned &= xTaskCreate( tUART_RxTask , "UART Receive"		, 64, NULL, configMAX_PRIORITIES - 1, NULL ); 
//		xReturned &= xTaskCreate( tUART_TxTask , "UART Transmit"	, 128, NULL, configMAX_PRIORITIES - 1, NULL ); 
//		xReturned &= xTaskCreate( tSensor_Task , "Sensor Handler"    , 24, NULL, configMAX_PRIORITIES - 1, NULL );
//		
//		xReturned &= xTaskCreate(tProgram_Handler,"Program Handler", 128 , NULL, configMAX_PRIORITIES - 1, NULL );
//////////		
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

    for( ;; )
    {
        nLED_SET(LED_DONT_CARE,LED_DONT_CARE,LED_DONT_CARE,1);
        delay_ms(1000);
        nLED_SET(LED_DONT_CARE,LED_DONT_CARE,LED_DONT_CARE,0);
        delay_ms(1000);
    }
}
#endif

#if ( configCHECK_FOR_STACK_OVERFLOW > 0 )
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
    ( void ) pcTaskName;
    ( void ) pxTask;

    taskDISABLE_INTERRUPTS();

    for( ;; )
    {
        nLED_SET(LED_DONT_CARE,LED_DONT_CARE,LED_DONT_CARE,1);
        delay_ms(500);
        nLED_SET(LED_DONT_CARE,LED_DONT_CARE,LED_DONT_CARE,0);
        delay_ms(500);
    }
}
#endif


