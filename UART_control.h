extern char help[];
extern char Help[];
extern char HELP[];
extern char LED_1_ON[];
extern char LED_2_ON[];
extern char LED_3_ON[];
extern char LED_4_ON[];
extern char LED_OFF[];
extern char LED_ALL_ON[];
extern char status[];
extern char clear[];
extern char LEDSTATE1[];
extern char LEDSTATE2[];
extern char LEDSTATE3[];
extern char LEDSTATE4[];
extern char LED[];
extern char disco_str[];



extern void UART0_init( unsigned int baudrate);

extern void uart_TxChar(char ch);

extern char uart_RxChar(void);

extern void uart_string(char ch_s[]);


extern void terminal_LED_1_ON(void);
extern void terminal_LED_2_ON(void);
extern void terminal_LED_3_ON(void);
extern void terminal_LED_4_ON(void);
extern void terminal_LED_OFF(void);
extern void terminal_LED_ALL_ON(void);

extern void terminal_status(void);

extern void terminal_help(void);

extern void terminal_LED_STATE_1(void);
extern void terminal_LED_STATE_2(void);
extern void terminal_LED_STATE_3(void);
extern void terminal_LED_STATE_4(void);

extern void terminal_clear(void);

extern void terminal_undefined(void); 

extern void terminal_no_function_found(void);
extern int checkstring(char string_1[] , char string_2[]);

extern void UART_chk_for_match(char input_array[]);




