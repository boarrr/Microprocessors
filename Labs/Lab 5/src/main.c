#include <stdint.h>
#include <stm32l031xx.h>

void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode);
void delay(volatile uint32_t dly);
void eputchar(char c);
char egetchar(void);
void eputs(char *String); 
void printDecimal(uint32_t Value);
void printHex(uint32_t Value);
void initSerial(void);
void checkChar(void);
void initClockHSI16();

int main()
{	
	uint32_t Count = 0;
	char ch;

	initSerial();

	pinMode(GPIOA, 0, 1);

	while(1)
	{
		ch = egetchar();
		eputchar(ch);
		checkChar();
		eputs("\r\n");
	}
}

void checkChar(void)
{
	if (USART2->TDR == 'a' || USART2->TDR == 'A') {
		GPIOA->ODR ^= (1 << 0);
	}
}

void eputchar(char c)
{
	while( (USART2->ISR & (1 << 6))==0); // wait for ongoing transmission to finish
	USART2->TDR = c;
}

char egetchar()
{
	while( (USART2->ISR & (1 << 5))==0); // wait for character to arrive
	return (char)USART2->RDR;
}

void eputs(char *String)
{
	while(*String) // keep printing until a NULL is found
	{
		eputchar(*String);
		String++;
	}
}

void printDecimal(uint32_t Value)
{
	char DecimalString[11]; // a 32 bit value can go up to about 4 billion
													// That's 10 digits plus a NULL character 
	DecimalString[10] = 0;  // terminate the string

	int index = 9;
	
	while (Value > 0 && index >= 0)
	{
		DecimalString[index] = (Value % 10) + '0';
		Value = Value / 10;
		index--;
	}

	eputs(&DecimalString[index + 1]);
}

void printHex(uint32_t Value)
{
	char HexString[11];
	int index = 9;

	HexString[10] = 0;

	while (Value > 0 && index >= 0) {
		uint32_t remainder = Value % 16;
		
		if (remainder < 10)
			HexString[index] = remainder + '0';
		else
			HexString[index] = (remainder - 10) + 'A';
		
		Value = Value / 16;
		index--;
	}

	eputs("0x");
	eputs(&HexString[index + 1]);
}

void initSerial()
{
	const uint32_t CLOCK_SPEED=16000000;
	const uint32_t BAUD_RATE = 9600;

    initClockHSI16();
	
	uint32_t BaudRateDivisor;
	RCC->IOPENR |= (1 << 0);  // Turn on GPIOA
	RCC->APB1ENR |= (1 << 17); // Turn on USART2
	GPIOA->MODER |= ( (1 << 5) | ((uint32_t)1 << 31));
	GPIOA->MODER &= (uint32_t)~(1 << 4);
	GPIOA->MODER &= (uint32_t)~(1 << 30);
	
	GPIOA->PUPDR |= ( (1 << 4) | (1 << 30));
	GPIOA->PUPDR &= (uint32_t)~(1 << 5);
	GPIOA->PUPDR &= ~((uint32_t)1 << 31);
	GPIOA->AFR[0] &= (uint32_t)( (1 << 11) | (1 << 10) | (1 << 9) | (1 << 8));
	GPIOA->AFR[0] |= (1 << 10);
	GPIOA->AFR[1] &= (uint32_t)( ((uint32_t)1 << 31) | (1 << 30) | (1 << 29) | (1 << 28));
	GPIOA->AFR[1] |= (1 << 30);
	BaudRateDivisor = CLOCK_SPEED/BAUD_RATE;
	RCC->APB1RSTR &= (uint32_t)~(1 << 17);
	USART2->CR1 = 0;
	USART2->CR2 = 0;
	USART2->CR3 = (1 << 12); // disable over-run errors
	USART2->BRR = BaudRateDivisor;
	USART2->CR1 = ( (1 << 2) | (1 << 3) );
	USART2->CR1 |= (1 << 0);
}


void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode)
{
	// This function writes the given Mode bits to the appropriate location for
	// the given BitNumber in the Port specified.  It leaves other bits unchanged
	// Mode values:
	// 0 : digital input
	// 1 : digital output
	// 2 : Alternative function
	// 3 : Analog input
	uint32_t mode_value = Port->MODER; // read current value of Mode register 
	Mode = Mode << (2 * BitNumber);    // There are two Mode bits per port bit so need to shift
																	   // the mask for Mode up to the proper location
	mode_value = mode_value & ~(3u << (BitNumber * 2)); // Clear out old mode bits
	mode_value = mode_value | Mode; // set new bits
	Port->MODER = mode_value; // write back to port mode register
}

void delay(volatile uint32_t dly)
{
	while(dly--);
}

void initClockHSI16()
{
    // Use the HSI16 clock as the system clock - allows operation down to 1.5V
    RCC->CR &= ~(1 << 24);
    RCC->CR |= (1 << 0); // turn on HSI16 (16MHz clock)
    while ((RCC->CR & (1 << 2))==0); // wait for HSI to be ready
    // set HSI16 as system clock source 
    RCC->CFGR |= (1<<0);
}