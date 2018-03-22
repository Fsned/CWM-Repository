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


uint8_t led_flipper = 0;
uint8_t wave_status;
int adc_result = 0;

int main() 
{
	SystemInit();                    //Clock and PLL configuration
	nGPIOSetup();
	
	nUART0_init(9600);
	
//	nSystickSetup(1000000);					// Should be setup last, to avoid interrupts generated during setup phase
	
	vSetupADC(1,1,1);
	
	while(1) {
		
		tUART_Task();
		
		LPC_ADC->ADCR |= 0x01;
		
<<<<<<< HEAD
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
=======
		delay_ms(1);
		
		util_BitSet(LPC_ADC->ADCR,SBIT_START);
		
		while(util_GetBitStatus(LPC_ADC->ADGDR,SBIT_DONE)==0);
		
		adc_result = ( LPC_ADC->ADGDR >> SBIT_RESULT ) & 0xFFF;		
>>>>>>> 809097d4f49cd6f5f56ee9fc7dc81f9d200d2cd2
	}
}




void SysTick_Handler(void) {
	
	static int led_timer = 1;
	
	if (!(--led_timer)) {
		led_timer = 1000000;
	}
}


