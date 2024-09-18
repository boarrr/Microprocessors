#include <stdint.h>
#include <stm32l031xx.h>

void delay(volatile uint32_t dly)
{
    while(dly--);
}

int main(void)
{
    RCC->IOPENR |= (1 << 1);
    GPIOB->MODER = (1 << 2*3);

    while(1)
    {
        GPIOB->ODR = (1 << 3);
        delay(100000);
        GPIOB->ODR = 0;
        delay(100000);
    }
}