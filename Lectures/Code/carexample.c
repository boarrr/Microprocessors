#include <stdio.h>

int GPIOA, GPIOB;

int show_outputs()
{
    printf("%d\n", GPIOB);

    return 0;
}

int main(void)
{
    GPIOA = 6; // Wiper pressed in car and brakes

    // Is horn pressed
    if (GPIOA == 1)
        GPIOB = 16;
    
    // Is brake pressed
    if (GPIOA & 2)
        GPIOB = GPIOB | 64;
    
    // Is wiper on
    if (GPIOA & 4) {
        GPIOB = GPIOB | 32;
    }
    else {
        GPIOB = GPIOB & ~32;
    }

    show_outputs();
}