



void delay_ms(unsigned int n) {
	unsigned int o = n*12000*2;
	while(--o);
}

void delay_us(unsigned int n) {
	unsigned int o = n*12*2;
	while(--o);
}
