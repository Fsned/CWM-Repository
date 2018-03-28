



void delay_ms(unsigned int n) {
	unsigned int o = n*12000*2;
	while(--o);
}

void delay_us(unsigned int n) {
	unsigned int o = n*12*2;
	while(--o);
}



#if ( configUSE_MALLOC_FAILED_HOOK == 1 )
void vApplicationMallocFailedHook( void )
{
    taskDISABLE_INTERRUPTS();

    for( ;; )
    {
        nLED_SET(10,10,10,1);
        delay_ms(1000);
        nLED_SET(10,10,10,0);
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
        nLED_SET(10,10,10,1);
        delay_ms(500);
        nLED_SET(10,10,10,0);
        delay_ms(500);
    }
}
#endif