#include <stdint.h>
#include <stm32l031xx.h>

void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode);
void enablePullUp(GPIO_TypeDef *Port,uint32_t BitNumber);
void delay(volatile uint32_t dly);
void redOn();
void redOff();
void yellowOn();
void yellowOff();
void greenOn();
void greenOff();
void crossWalk();
int buttonPressed();

int main()
{
	// Enable clock for GPIOA and B
	RCC->IOPENR |= (1 << 0) + (1 << 1);

	// Set GPIOA0 to 2 as output
	pinMode(GPIOA,0,1); // Make GPIOA bit 0 an output
	pinMode(GPIOA,1,1); // Make GPIOA bit 1 an output
	pinMode(GPIOA,2,1); // Make GPIOA bit 2 an output
	
	// Set GPIOB3 as Output and 4 as input
	pinMode(GPIOB,3,1); // Make GPIOB bit 3 an output
	pinMode(GPIOB,4,0); // Make GPIOB bit 4 an input
	
	// Turn on pull up resister on GPIOB4, to keep it HIGH to detect button press
	enablePullUp(GPIOB,4);

	// Turn on green LED as default
	greenOn();

	while (1)
	{
		// If button gets pressed, run the crosswalk code
		if (buttonPressed()) {
			crossWalk();	
		}

		delay(10000);
	}	
}

void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber)
{
	Port->PUPDR = Port->PUPDR &~(3u << BitNumber*2); // clear pull-up resistor bits
	Port->PUPDR = Port->PUPDR | (1u << BitNumber*2); // set pull-up bit
}

void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode)
{
	/*
	*/
	uint32_t mode_value = Port->MODER;
	Mode = Mode << (2 * BitNumber);
	mode_value = mode_value & ~(3u << (BitNumber * 2));
	mode_value = mode_value | Mode;
	Port->MODER = mode_value;
}

void delay(volatile uint32_t dly)
{
	while(dly--);	
}

// Turn on any LED by using bitwise operations to send a 1 to that output pin
void redOn()
{
	GPIOA->ODR |= (1 << 2);
}

void redOff()
{
	GPIOA->ODR &= ~(1 << 2);
}

void yellowOn()
{
	GPIOA->ODR |= (1 << 1);
}

void yellowOff()
{
	GPIOA->ODR &= ~(1 << 1);
}


void greenOn()
{
	GPIOA->ODR |= (1 << 0);
}

void greenOff()
{
	GPIOA->ODR &= ~(1 << 0);
}

// Basic crosswalk functionality using delays
void crossWalk()
{
	delay(300000);
	greenOff();
	delay(50000);
	yellowOn();
	delay(500000);
	yellowOff();
	delay(50000);
	redOn();
	delay(900000);
	redOff();
	delay(50000);
	greenOn();
}

// If the input register of GPIOB is low, button is pressed
int buttonPressed()
{
	if (!GPIOB->IDR)
		return 1;
	
	return 0;	
}