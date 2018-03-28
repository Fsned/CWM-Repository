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


#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h" 

#define TASK_STACK_SIZE 32

uint8_t led_flipper = 0;
uint8_t wave_status;
int adc_result = 0;
unsigned int myarr[4] = {0,0,0,1};

int main() 
{
	
	BaseType_t xReturned;
	SystemInit();                    //Clock and PLL configuration
	nGPIOSetup();
//	
	nUART0_init(9600);
	
//	nSystickSetup(1000000);					// Should be setup last, to avoid interrupts generated during setup phase
//	uint16_t myvalue = 0;
	
//	uint8_t ADC_Handle_1 = vSetupADC();
	
	while(1) {
//		
//		tUART_Task();			// Uart task latches until character is received, which spoils the scheduling.	
//	  tADC_Task	

		
		// Create a task.
    
		xReturned = xTaskCreate( tUART_Task , "UARTC0", 128, NULL, configMAX_PRIORITIES - 1, NULL ); 
//		
    
		
		if (xReturned != pdPASS)
			nLED_SET(1,1,1,1);
		// LED Alive task
		xTaskCreate( tLEDAlive , 	"T01"					, TASK_STACK_SIZE, NULL, configMAX_PRIORITIES - 1, NULL );
		
		
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


