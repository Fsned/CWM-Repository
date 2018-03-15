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
	SystickSetup(1000000);					// Should be setup last, to avoid interrupts generated during setup phase
	
	//UART0_init(9600);

	while(1) {
//		wave_status = vReadDigitalInput( PORT_0 , PIN_1 );
//		LED_SET(wave_status,0,0,0);
	}
}


void SysTick_Handler(void) {
	
	static int led_timer = 1;
	
	if (!(--led_timer)) {
		led_timer = 1000000;
		
		if (led_flipper) {
			led_flipper = !led_flipper;
//			LED_SET(0,0,0,0);
			if (! ySetDigitalHigh( PORT_1 , LED_1 ))
				LED_SET(0,0,0,1);
		}
		else if (led_flipper == 0) {
			led_flipper = 1;
//			LED_SET(1,0,0,0);
			if (! ySetDigitalLow( PORT_1 , LED_1 ))
				LED_SET(0,0,1,0);
		}
		

		LPC_GPIO0->FIOPIN |= (1 << 25);
		LPC_GPIO0->FIOPIN ^= (1 << 26);
	}
}


