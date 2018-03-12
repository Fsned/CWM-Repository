#include <lpc17xx.h>   
#include "setup_file.h"
#include "LED_control.h"
#include "utilities.h"
#include "UART_control.h"
#include "stdio.h"

int main() 
{
	SystemInit();                    //Clock and PLL configuration
	LED_setup();
	UART0_init(9600);
	
  while(1) {
		
		uart_task();
		
	}
}

