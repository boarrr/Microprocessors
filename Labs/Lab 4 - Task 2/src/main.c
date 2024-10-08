#include <stdint.h>
#include <stm32l031xx.h>

// Global milliseconds counter
volatile uint32_t milliseconds = 0;

void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode);
void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber);
void initClockHSI16(void);

// Simple Delay Function
void delay(volatile uint32_t dly)
{
    milliseconds = 0;
    while (milliseconds < dly);
}

// Called whenever a SysTick happens
void SysTick_Handler(void)
{
    milliseconds++;    
}

int main()
{
    initClockHSI16();
    RCC->IOPENR |= (1 << 1) + (1 << 0); // turn on GPIOA and GPIOB
    pinMode(GPIOB,3,1); // configure GPIOB bit 3 as an output.
    
    // Set pin GPIOB1 as output
    pinMode(GPIOB, 1, 1);
    
    SysTick->LOAD = 16000; // Clock frequency is 16MHz.  Divide by 16000 to get 1kHz.
    SysTick->CTRL = 7;  // turn on interrupts for the SysTick timer.
    SysTick->VAL = 100; // set an initial value for the counter so we don't have to wait long for first interrupt

    __enable_irq(); // enable interrupts.

    while(1)
    {
        GPIOB->ODR |= (1 << 1);
        delay(10000);
        GPIOB->ODR &= ~(1 << 1);
        delay(10000);
    }
}

// Enable pull up resistor
void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber)
{
	Port->PUPDR = Port->PUPDR &~(3u << BitNumber*2); // clear pull-up resistor bits
	Port->PUPDR = Port->PUPDR | (1u << BitNumber*2); // set pull-up bit
}

// Set a pin mode
void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode)
{
	/*
        Port : GPIOA,GPIOB,GPIOC etc. i.e. a pointer to the port memory block
        Bitnumber : which bit are you configuring (1,2,3 etc.)
        Mode :  00 = Input
                01 = Output
                10 = Alternative mode 
                11 = Analogue mode
	*/
	uint32_t mode_value = Port->MODER;
	Mode = Mode << (2 * BitNumber);
	mode_value = mode_value & ~(3u << (BitNumber * 2));
	mode_value = mode_value | Mode;
	Port->MODER = mode_value;
}

// Init the clock
void initClockHSI16()
{
    // Use the HSI16 clock as the system clock - allows operation down to 1.5V
    RCC->CR &= ~(1<<24);
    RCC->CR |= (1 << 0); // turn on HSI16 (16MHz clock)
    while ((RCC->CR & (1<<2))==0); // wait for HSI to be ready
    // set HSI16 as system clock source 
    RCC->CFGR |= (1<<0);
}