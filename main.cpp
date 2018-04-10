#include <lpc17xx.h>
#include "setup_file.h"
#include "LED_control.h"
#include "utilities.h"
#include "UART_control.h"
#include "stdio.h"
#include "systick.h"
#include "pwm_setup.h"
#include "GPIO_setup.h"
#include "ADC_control.h"
#include "Washing_file.h"
#include "Sensor_file.h"

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

#define TASK_STACK_SIZE 32



int main()
{
	
	BaseType_t xReturned = pdPASS;
	SystemInit();                    //Clock and PLL configuration
	nGPIOSetup();
	nUART0_init(9600);
	
	while(1) {

		// Create a task.
    
		xReturned &= xTaskCreate( tUART_RxTask , "UART Receive"		, 48, NULL, configMAX_PRIORITIES - 1, NULL ); 
		xReturned &= xTaskCreate( tUART_TxTask , "UART Transmit"	, 48, NULL, configMAX_PRIORITIES - 1, NULL ); 
		xReturned &= xTaskCreate( tSensor_Task , "Sensor Task"    , 32 , NULL, configMAX_PRIORITIES - 1, NULL );
//		xReturned &= xTaskCreate(tProgram_Handler,"Program Handler",32, NULL, configMAX_PRIORITIES - 1, NULL );
//		xReturned &= xTaskCreate( tADC_Task		 , "ADC_Task"				, 32, NULL, configMAX_PRIORITIES - 1, NULL );			// ADC_TASK skal laves om til Sensor_Task, som holder styr på data på samtlige følere, analoge og digitale, samt giver API til at hente disse data.		
		
		if (xReturned == pdPASS)
			xTaskCreate( tLEDAlive 	, 	"LED Alive task"	, 32 , NULL, configMAX_PRIORITIES - 1, &AliveHandle );
		
		qUART_RxQ		= xQueueCreate(32 , sizeof(char));
		qUART_TxQ		= xQueueCreate(32 , sizeof(char));
		SensorQ			= xQueueCreate(16 , sizeof(uint8_t));
		
		// Start FreeRTOS scheduler.
    vTaskStartScheduler();
	}
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


