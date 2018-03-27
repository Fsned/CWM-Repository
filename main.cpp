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
	
	uint8_t ADC_Handle_1 = vSetupADC();
	delay_ms(1);
//	uint8_t ADC_Handle_2 = vSetupADC();
//	delay_ms(1);
	
	while(1) {
		
//		tUART_Task();		
		tADC_Task();
		int myvalue = vReadADC(ADC_Handle_1);
		
		if (vReadADC(ADC_Handle_1) > 10)
			nLED_SET(1,1,0,0);
		
		
//		
//		if (vReadADC(ADC_Handle_2) > 0)
//			nLED_SET(0,1,0,0);
		
		
		
		delay_ms(100);
		
		
//		delay_ms(1);
//		
//		util_BitSet(LPC_ADC->ADCR,SBIT_START);
//		
//		while(util_GetBitStatus(LPC_ADC->ADGDR,SBIT_DONE)==0);
//		
//		adc_result = ( LPC_ADC->ADGDR >> SBIT_RESULT ) & 0xFFF;
			
		
	}
}




void SysTick_Handler(void) {
	
	static int led_timer = 1;
	
	if (!(--led_timer)) {
		led_timer = 1000000;
	}
}


