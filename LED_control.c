#include "LED_control.h"
#include "lpc17xx.h"

int disco_arr[20][4] = {{0,0,0,0},{0,1,1,0},{1,0,0,1},{0,1,1,0},{1,1,1,1},
												{1,0,0,0},{1,1,0,0},{0,1,0,0},{0,1,1,0},{0,0,1,0},
												{0,0,1,1},{0,0,0,1},{0,0,1,1},{1,1,1,1},{1,1,1,0},
												{1,1,0,0},{1,0,0,0},{0,0,0,0},{1,0,1,0},{0,1,0,1}};
int LED_status[4] = {0,0,0,0};
int LED_pins[4]   = {18, 20, 21, 23};
													
void delay_LED(void);

void LED_setup(void) {
	LPC_PINCON->PINSEL4 &= ~(0x0000CF30);	// Set P1:18, P1:20, P1:21, P1:23 to function 1 (GPIO)
	LPC_GPIO0->FIODIR   |=   0x0000000F;
	LPC_GPIO1->FIODIR   |=   0x00B40000;	// Set Port 1 as output
	
	LED_SET(1,0,0,0);
	delay_LED();
	LED_SET(1,1,0,0);
	delay_LED();
	LED_SET(1,1,1,0);
	delay_LED();
	LED_SET(1,1,1,1);
	delay_LED();
	LED_SET(0,1,1,1);
	delay_LED();
	LED_SET(0,0,1,1);
	delay_LED();
	LED_SET(0,0,0,1);
	delay_LED();
	LED_SET(0,0,0,0);
}

void LED_SET(unsigned int led_0 , unsigned int led_1, unsigned int led_2, unsigned int led_3) {
	LPC_GPIO1->FIOCLR = 0x00B40000;
	
	LPC_GPIO1->FIOSET |= (led_0 << 18);
	LPC_GPIO1->FIOSET |= (led_1 << 20);
	LPC_GPIO1->FIOSET |= (led_2 << 21);
	LPC_GPIO1->FIOSET |= (led_3 << 23);
}

void LED_flip(unsigned int led_flip[]) {
	for (int i = 0; i < 4; i++) {
		if (led_flip[i]) {
			if (LED_status[i])
				LED_status[i] = 0;
			else
				LED_status[i] = 1;
		}
	}
	LED_SET(LED_status[0], LED_status[1], LED_status[2], LED_status[3]);
}


void disco_func() {
	for (int n = 0; n < 1; n++) {
		for(int i = 0; i < 20; i++) {
			LED_SET(disco_arr[i][0] , disco_arr[i][1] , disco_arr[i][2] , disco_arr[i][3]);
			delay_LED();
		}
	}
}


void delay_LED() {
	for (int i = 0; i < 1500000; i++);
}

