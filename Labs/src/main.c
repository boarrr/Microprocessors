#include <stm32l031xx.h> // Header file for STM32L031

// Simple delay function, volatile is used to prevent compiler optimizing away loop
void delay(volatile unsigned long dly)
{
    while(dly--);
}

int main(void)
{
    RCC->IOPENR = (1 << 1); // Enable the clock for GPIO B
    GPIOB->MODER = (1 << 2*3); // Configure pin 3 of GPIO B as output pin

    while(1)
    {
        GPIOB->ODR = (1 << 3); // Set pin 3 of GPIOB to HIGH (1)
        delay(100000);
        GPIOB->ODR = 0; // Clear all bits in ODR, setting pin 3 to LOW (0)
        delay(100000);
    }
}