#include <lpc17xx.h>   
#include "setup_file.h"
#include "LED_control.h"
#include "utilities.h"
#include "UART_control.h"
#include "stdio.h"

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "queue.h"
#include "list.h"






char last_char;
char input_buffer[128];
unsigned int i = 0;

int main() 
{
	
	portBASE_TYPE return_value = pdTRUE;
	
	SystemInit();                    //Clock and PLL configuration
	LED_setup();
	UART0_init(9600);
	
	return_value &= xTaskCreate( my_task , (signed portCHAR * ) "my task" , 32 , NULL , 1 , NULL);
	
	
	
  while(1) {
		
		
		last_char = uart_RxChar();	
		if (last_char == '\r'){
			uart_TxChar('\r');
			uart_TxChar('\n');
			UART_chk_for_match(input_buffer);
			i = 0;
		}
		input_buffer[i] = last_char;
		uart_TxChar(input_buffer[i]);
		i++;
	}
}

