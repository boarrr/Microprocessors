#include <stm32f031x6.h>
#include "display.h"

#define SHIP_WIDTH 16
#define SHIP_HEIGHT 16
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 160

// Display range for single character -> 0 - 118

void initClock(void);
void initSysTick(void);
void SysTick_Handler(void);
void delay(volatile uint32_t dly);
void setupIO();
void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber);
void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode);

void handleInput(int*, int*);

volatile uint32_t milliseconds;

const uint16_t spaceship[] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,54057,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7663,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7663,7663,7663,0,0,0,0,0,0,0,0,0,0,0,0,0,7663,7663,7663,0,0,0,0,0,0,0,0,0,0,0,54057,0,7663,7663,7663,0,54057,0,0,0,0,0,0,0,0,0,7663,0,7663,54057,7663,0,7663,0,0,0,0,0,0,0,54057,0,0,7663,54057,7663,54057,7663,0,0,54057,0,0,0,0,0,7663,0,7663,7663,7663,7663,7663,7663,7663,0,7663,0,0,0,0,0,7663,7663,7663,7663,54057,7663,54057,7663,7663,7663,7663,0,0,0,0,0,7663,7663,0,54057,54057,0,54057,54057,0,7663,7663,0,0,0,0,0,7663,0,0,0,0,0,0,0,0,0,7663,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

int main()
{
    int player_x = 0, player_y = 60;

	initClock();
	initSysTick();
	setupIO();
	
	while(1)
	{
        putImage(player_x, player_y, 16, 16, spaceship, 0, 0);
        printNumberX2(player_x, 10, 10, RGBToWord(255,255,255), 0);

        handleInput(&player_x, &player_y);

        delay(25);
	}

	return 0;
}

void handleInput(int *player_x, int *player_y)
{
    // Handle right input by increasing the players current X
    if ((GPIOB->IDR & (1 << 4)) == 0) {
        (*player_x)++;

        if (*player_x > (SCREEN_WIDTH - SHIP_WIDTH)) {
            fillRectangle(*player_x, *player_y, SHIP_WIDTH, SHIP_WIDTH, 0);
            *player_x = 0;
        }
    }

    // Handle left input by decreasing the players current Y
    if ((GPIOB->IDR & (1 << 5)) == 0) {
        (*player_x)--;

        if (*player_x < 0) {
            fillRectangle(*player_x, *player_y, SHIP_WIDTH, SHIP_WIDTH, 0);
            *player_x = (SCREEN_WIDTH - SHIP_WIDTH);
        }
    }

    // Handle up input by increasing the players current Y
    if ((GPIOA->IDR & (1 << 8)) == 0) {
        (*player_y)--;

        if (*player_y < 0) {
            fillRectangle(*player_x, *player_y, SHIP_WIDTH, SCREEN_HEIGHT, 0);
            *player_y = (SCREEN_HEIGHT - SHIP_HEIGHT);
        }
    }

    // Handle down input by decreasing the players current Y
    if ((GPIOA->IDR & (1 << 11)) == 0) {
        (*player_y)++;

        if (*player_y > (SCREEN_HEIGHT - SHIP_HEIGHT)) {
            fillRectangle(*player_x, *player_y, SHIP_WIDTH, SCREEN_HEIGHT, 0);
            *player_y = 0;
        }
    }    
}
void initSysTick(void)
{
	SysTick->LOAD = 48000;
	SysTick->CTRL = 7;
	SysTick->VAL = 10;
	__asm(" cpsie i "); // enable interrupts
}

void SysTick_Handler(void)
{
	milliseconds++;
}

void initClock(void)
{
    // This is potentially a dangerous function as it could
    // result in a system with an invalid clock signal - result: a stuck system
    // Set the PLL up
    // First ensure PLL is disabled

    RCC->CR &= ~(1u<<24);
    while( (RCC->CR & (1 <<25))); // wait for PLL ready to be cleared
        
    // Warning here: if system clock is greater than 24MHz then wait-state(s) need to be
    // inserted into Flash memory interface
            
    FLASH->ACR |= (1 << 0);
    FLASH->ACR &=~((1u << 2) | (1u<<1));
    // Turn on FLASH prefetch buffer
    FLASH->ACR |= (1 << 4);
    // set PLL multiplier to 12 (yielding 48MHz)
    RCC->CFGR &= ~((1u<<21) | (1u<<20) | (1u<<19) | (1u<<18));
    RCC->CFGR |= ((1<<21) | (1<<19) ); 

    // Need to limit ADC clock to below 14MHz so will change ADC prescaler to 4
    RCC->CFGR |= (1<<14);

    // and turn the PLL back on again
    RCC->CR |= (1<<24);        

    // set PLL as system clock source 
    RCC->CFGR |= (1<<1);
}

void delay(volatile uint32_t dly)
{
	uint32_t end_time = dly + milliseconds;
	while(milliseconds != end_time)
		__asm(" wfi "); // sleep
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

void setupIO()
{
	RCC->AHBENR |= (1 << 18) + (1 << 17); // enable Ports A and B
	display_begin();
	pinMode(GPIOB,4,0);
	pinMode(GPIOB,5,0);
	pinMode(GPIOA,8,0);
	pinMode(GPIOA,11,0);
	enablePullUp(GPIOB,4);
	enablePullUp(GPIOB,5);
	enablePullUp(GPIOA,11);
	enablePullUp(GPIOA,8);
}