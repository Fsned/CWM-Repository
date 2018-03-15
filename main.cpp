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
	//UART0_init(9600);
	
	nSystickSetup(1000000);					// Should be setup last, to avoid interrupts generated during setup phase
	
	while(1) {
		wave_status = vDigitalRead( PORT_0 , PIN_26 );
		yDigitalWrite(PORT_1 , LED_1 , wave_status);
	}
}




void SysTick_Handler(void) {
	
	static int led_timer = 1;
	
	if (!(--led_timer)) {
		led_timer = 1000000;
	}
}


