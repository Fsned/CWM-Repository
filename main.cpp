#include <lpc17xx.h>   
#include "setup_file.h"
#include "LED_control.h"
#include "utilities.h"
#include "UART_control.h"
#include "stdio.h"
#include "systick.h"
#include "pwm_setup.h"
#include "GPIO_setup.h"

uint8_t led_flipper = 0;
uint8_t wave_status;

int main() 
{
	SystemInit();                    //Clock and PLL configuration
	nGPIOSetup();
	
	nUART0_init(9600);
	
	nSystickSetup(1000000);					// Should be setup last, to avoid interrupts generated during setup phase
	
	while(1) {
		tUART_Task();
	}
}




void SysTick_Handler(void) {
	
	static int led_timer = 1;
	
	if (!(--led_timer)) {
		led_timer = 1000000;
	}
}


