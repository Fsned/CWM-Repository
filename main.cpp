// ==========================================================
//	Controller software for Jeros A/S' Coarse washing Machines
//
//	Copyright Notice :
//
//	Year of Creation : 2018
//	Author : Frederik Snedevind , Head of Software Development, Jeros A/S
//
//	© :		All rights reserved for Jeros A/S.
//				No Distribution of this software for other use than
//				originally intended is allowed. Cannot be used for
//				any kind of exercises, nor further development within any
//				other establishments than Jeros A/S.
//
//
// =============================================================
//			Libraries
// =============================================================
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


// =============================================================
//			Main
// =============================================================
int main()
{
// =============================================================
//			Initializers
// =============================================================
	SystemInit();                    	//Clock and PLL configuration
	nGPIOSetup();										 	// Setup various GPIO pins for Digital & Analog sensors
	nUART0_init(9600);								// Initialize UART Channel 0, with baud rate = 9600 bps
	nInitializeDigitalSensors();			// Initialize all Digital Sensors
	nInitializeAnalogSensors();				// Initialize all Analog Sensors

	
// =============================================================
//			Tasks
	BaseType_t xReturned = pdPASS;
// =============================================================
	xReturned &= xTaskCreate( tUART_RxTask , "UART Receive"		, 128, NULL, configMAX_PRIORITIES - 1, NULL );					// Task to Receive input from the terminal, and process it in the program
	xReturned &= xTaskCreate( tUART_TxTask , "UART Transmit"	, 128, NULL, configMAX_PRIORITIES - 1, NULL ); 					// Task to transmit response and info to the terminal to assist and validate the user
	xReturned &= xTaskCreate( tSensor_Task , "Sensor Handler"       , 24, NULL, configMAX_PRIORITIES - 1, NULL );				// Task to setup, control and collect data from sensors
//  xReturned &= xTaskCreate(tProgram_Handler,"Program Handler", 128 , NULL, configMAX_PRIORITIES - 1, NULL );			// Task to execute programs
		
	
	
// =============================================================
//			FreeRTOS Stack check
// =============================================================
	if (xReturned == pdPASS)
		xTaskCreate( tLEDAlive 	, 	"LED Alive task"	, 32 , NULL, configMAX_PRIORITIES - 1, &AliveHandle );
	
// =============================================================
//			Queues
// =============================================================		
	
// Queue(s) for UART
	qUART_RxQ				= xQueueCreate(24 , sizeof(char));
	qUART_TxQ				= xQueueCreate(24 , sizeof(char));
		
// Queue(s) for Sensor Handler
	SensorQ					= xQueueCreate(24 , sizeof(uint8_t));
	
// Queue(s) for Program Handler
	ProgramHandlerQ = xQueueCreate(12 , sizeof(uint8_t));
	ProgramLibrary	= xQueueCreate(12 , sizeof(uint8_t));
	OperationQ 			= xQueueCreate(12 , sizeof(uint8_t));
	
// =============================================================
//			Start Scheduling
// =============================================================
	vTaskStartScheduler();
}
// ********************** End of Main **************************

























// =============================================================
//			FreeRTOS Stack Shortage Hooks
// =============================================================
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
//		nTerminal_LED_ALL_ON();
	
    for( ;; )
    {
        nLED_SET(LED_DONT_CARE,LED_DONT_CARE,LED_DONT_CARE,1);
        delay_ms(500);
        nLED_SET(LED_DONT_CARE,LED_DONT_CARE,LED_DONT_CARE,0);
        delay_ms(500);
    }
}
#endif


