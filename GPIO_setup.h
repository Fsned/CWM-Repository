
#define PORT_0		0
#define PORT_1		1
#define PORT_2		2
#define	PORT_3		3
#define PORT_4		4

#define PIN_0		0
#define PIN_1		1
#define PIN_2		2
#define PIN_3		3
#define PIN_4		4
#define PIN_5		5
#define PIN_6		6
#define PIN_7		7
#define PIN_8		8
#define PIN_9		9
#define PIN_10		10
#define PIN_11		11
#define PIN_12		12
#define PIN_13		13
#define PIN_14		14
#define PIN_15		15
#define PIN_16		16
#define PIN_17		17
#define PIN_18		18
#define PIN_19		19
#define PIN_20		20
#define PIN_21		21
#define PIN_22		22
#define PIN_23		23
#define PIN_24		24
#define PIN_25		25
#define PIN_26		26
#define PIN_27		27
#define PIN_28		28
#define PIN_29		29
#define PIN_30		30
#define PIN_31		31

#define LED_1		18
#define LED_2		20
#define LED_3		21
#define LED_4		23

#define GPIO_FREE			0
#define GPIO_INPUT		1
#define GPIO_OUTPUT		2

void nGPIOSetup( void );

uint8_t ySetupDigitalO ( uint8_t Port, int Pin);

uint8_t ySetupDigitalI ( uint8_t Port, int Pin);

uint8_t vReadDigitalInput(uint8_t Port , int Pin);

uint8_t ySetDigitalLow(uint8_t Port , int Pin);
	
uint8_t ySetDigitalHigh(uint8_t Port , int Pin);

void nInitializePinLibrary( void );
