#include <stdint.h>

uint32_t* GPIOA_MODER = 0x50000000;
uint32_t* GPIOA_IDR = 0x50000010;
uint32_t* GPIOA_ODR = 0x50000014;

int main(void)
{
    *GPIOA_MODER = 1; // Configure bit 0 as an output;
    *GPIOA_ODR = 1; // Make bit 0 a 1;
    *GPIOA_ODR = 0; // Make bit 0 a 0;
}