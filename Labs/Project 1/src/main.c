#include <stm32f031x6.h>
#include "display.h"

#define SHIP_WIDTH 16
#define SHIP_HEIGHT 16
#define BULLET_WIDTH 2
#define BULLET_HEIGHT 2
#define BULLET_SPEED 4
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 160

// Player struct
struct player {
    uint8_t x;
    uint8_t y;
    uint8_t rotation;
    uint8_t lives;
    uint16_t score;
    const uint16_t *sprites[5];
};

// Bullet struct
struct bullet {
    uint8_t x;
    uint8_t y;
    uint8_t direction;
    uint8_t exists;
};

// MPU Function Prototypes
void initClock(void);
void initSysTick(void);
void SysTick_Handler(void);
void delay(volatile uint32_t);
void setupIO(void);
void enablePullUp(GPIO_TypeDef*, uint32_t);
void pinMode(GPIO_TypeDef*, uint32_t, uint32_t);
void initAudio(void);
void initADC(void);
int readADC(void);

// Game Function Prototypes
void playSound(uint32_t, uint32_t);
void mainMenu(void);
void gameLoop(struct player*);
void handleInput(struct player*);
void drawShip(struct player*);
void drawBullet(int*, int*, int*, int, int, int, int*);

// Global variables
volatile uint32_t milliseconds;

// Spaceships bitmaps stored in an array, different sprite for each rotation of the ship
const uint16_t spaceship[] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7936,7936,0,0,0,0,0,0,0,0,0,0,0,0,0,0,65535,65535,0,0,0,0,0,0,0,0,0,0,0,0,0,0,65535,65535,0,0,0,0,0,0,0,0,0,0,0,0,0,65535,65535,65535,65535,0,0,0,0,0,0,0,0,0,0,0,0,65535,65535,65535,65535,0,0,0,0,0,0,0,0,0,7936,0,0,65535,65535,65535,65535,0,0,7936,0,0,0,0,0,0,65535,0,0,65535,7936,7936,65535,0,0,65535,0,0,0,0,7936,0,65535,0,65535,7936,65535,65535,7936,65535,0,65535,0,7936,0,0,65535,0,0,65535,65535,65535,65535,65535,65535,65535,65535,0,0,65535,0,0,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,0,0,65535,65535,65535,65535,65535,7936,65535,65535,7936,65535,65535,65535,65535,65535,0,0,65535,65535,65535,0,65535,7936,65535,65535,7936,65535,0,65535,65535,65535,0,0,65535,65535,0,0,7936,65535,65535,65535,65535,7936,0,0,65535,65535,0,0,65535,0,0,0,7936,7936,0,0,7936,7936,0,0,0,65535,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

const uint16_t spaceship_left[] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7936,7936,0,0,0,0,0,0,0,7936,0,0,0,0,0,0,7936,65535,65535,0,0,0,7936,0,0,0,65535,0,0,0,0,0,0,65535,65535,65535,0,0,0,65535,0,0,0,65535,0,0,0,0,0,0,65535,65535,65535,65535,0,0,65535,0,0,65535,65535,0,0,0,0,0,0,65535,65535,7936,65535,7936,0,65535,65535,65535,65535,65535,0,0,0,0,0,65535,7936,7936,65535,65535,65535,65535,65535,65535,65535,65535,0,0,0,7936,0,0,65535,65535,65535,65535,65535,7936,65535,65535,0,0,0,0,0,0,65535,0,7936,65535,65535,65535,65535,65535,7936,65535,7936,0,0,0,0,0,0,65535,0,65535,65535,65535,65535,65535,65535,7936,7936,0,0,0,7936,0,0,0,65535,65535,7936,65535,65535,65535,65535,7936,7936,0,0,0,0,65535,0,0,65535,65535,65535,7936,65535,65535,0,0,0,0,0,0,0,0,65535,65535,65535,65535,65535,65535,7936,7936,0,0,0,0,0,0,0,0,0,65535,65535,65535,0,7936,7936,7936,0,0,0,0,0,0,0,0,0,0,65535,65535,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

const uint16_t spaceship_right[] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7936,0,0,0,0,0,0,0,7936,7936,0,0,0,0,0,65535,0,0,0,7936,0,0,0,65535,65535,7936,0,0,0,0,65535,0,0,0,65535,0,0,0,65535,65535,65535,0,0,0,0,65535,65535,0,0,65535,0,0,65535,65535,65535,65535,0,0,0,0,65535,65535,65535,65535,65535,0,7936,65535,7936,65535,65535,0,0,0,0,0,65535,65535,65535,65535,65535,65535,65535,65535,7936,7936,65535,0,0,0,0,0,0,0,65535,65535,7936,65535,65535,65535,65535,65535,0,0,7936,0,0,0,0,7936,65535,7936,65535,65535,65535,65535,65535,7936,0,65535,0,0,0,0,0,7936,7936,65535,65535,65535,65535,65535,65535,0,65535,0,0,0,0,0,0,7936,7936,65535,65535,65535,65535,7936,65535,65535,0,0,0,7936,0,0,0,0,0,0,65535,65535,7936,65535,65535,65535,0,0,65535,0,0,0,0,0,0,0,7936,7936,65535,65535,65535,65535,65535,65535,0,0,0,0,0,0,0,0,7936,7936,7936,0,65535,65535,65535,0,0,0,0,0,0,0,0,0,0,0,0,0,65535,65535,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

const uint16_t spaceship_left_full[] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7936,65535,65535,65535,65535,65535,65535,0,0,0,0,0,0,0,0,0,0,0,65535,65535,65535,65535,0,0,0,0,0,0,0,0,7936,65535,65535,0,65535,65535,65535,0,0,0,0,0,0,0,0,0,0,0,0,65535,65535,65535,0,0,0,0,0,0,0,0,0,0,0,0,65535,65535,65535,65535,65535,7936,7936,0,0,0,0,0,65535,65535,65535,65535,7936,65535,65535,7936,7936,65535,7936,0,0,7936,65535,65535,65535,65535,65535,7936,65535,65535,65535,65535,65535,65535,0,0,0,7936,65535,65535,65535,65535,65535,7936,65535,65535,65535,65535,65535,65535,0,0,0,0,0,0,65535,65535,65535,65535,7936,65535,65535,7936,7936,65535,7936,0,0,0,0,0,0,0,0,0,65535,65535,65535,65535,65535,7936,7936,0,0,0,0,0,0,0,0,0,0,65535,65535,65535,0,0,0,0,0,0,0,0,0,0,7936,65535,65535,0,65535,65535,65535,0,0,0,0,0,0,0,0,0,0,0,0,0,65535,65535,65535,65535,0,0,0,0,0,0,0,0,0,0,7936,65535,65535,65535,65535,65535,65535,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

const uint16_t spaceship_right_full[] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,65535,65535,65535,65535,65535,65535,7936,0,0,0,0,0,0,0,0,0,0,65535,65535,65535,65535,0,0,0,0,0,0,0,0,0,0,0,0,0,65535,65535,65535,0,65535,65535,7936,0,0,0,0,0,0,0,0,0,0,65535,65535,65535,0,0,0,0,0,0,0,0,0,0,7936,7936,65535,65535,65535,65535,65535,0,0,0,0,0,0,0,0,0,7936,65535,7936,7936,65535,65535,7936,65535,65535,65535,65535,0,0,0,0,0,0,65535,65535,65535,65535,65535,65535,7936,65535,65535,65535,65535,65535,7936,0,0,0,65535,65535,65535,65535,65535,65535,7936,65535,65535,65535,65535,65535,7936,0,0,7936,65535,7936,7936,65535,65535,7936,65535,65535,65535,65535,0,0,0,0,0,7936,7936,65535,65535,65535,65535,65535,0,0,0,0,0,0,0,0,0,0,0,0,65535,65535,65535,0,0,0,0,0,0,0,0,0,0,0,0,65535,65535,65535,0,65535,65535,7936,0,0,0,0,0,0,0,0,65535,65535,65535,65535,0,0,0,0,0,0,0,0,0,0,0,65535,65535,65535,65535,65535,65535,7936,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

// Main Function
int main() 
{
    // System initialization for clock, systick, input/output, analogue to digital converter, and audio
    initClock();
    initSysTick();
    setupIO();
    initADC();
    initAudio();

    // Create and initialize the player instance
    struct player player1 = {
        .x = (SCREEN_WIDTH / 2) - (SHIP_WIDTH / 2),
        .y = SCREEN_HEIGHT / 2,
        .rotation = 2,
        .lives = 3,
        .score = 0,
        .sprites = {spaceship_left_full, spaceship_left, spaceship, spaceship_right, spaceship_right_full}
    };

    // Display the main menu at game start
    mainMenu();

    // Primary game loop, passing the player instance
    gameLoop(&player1);

    return 0;
}

void mainMenu(void)
{
    uint8_t game_start = 0;

    playSound(0,0);

    while(game_start == 0) {
        uint8_t rotation = readADC() / 819;

        printText("Move knob center", 10, SCREEN_HEIGHT / 2.5, RGBToWord(255,255,255), RGBToWord(0,0,0));
        printText("and press up", 20, SCREEN_HEIGHT / 2, RGBToWord(255,255,255), RGBToWord(0,0,0));
        printText("to start", 35, SCREEN_HEIGHT / 1.65, RGBToWord(255,255,255), RGBToWord(0,0,0));

        // If the potentiometer is in the center and the up button is pressed, start the game
        if (rotation == 2 && (GPIOA->IDR & (1 << 8)) == 0) {
            // Clear the screen and start the game
            fillRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, RGBToWord(0,0,0));
            game_start = 1;
        }
    }
}

void gameLoop(struct player *p) 
{
    // Placeholders for bullet position
    int bullet_x = 0, bullet_y = 0, bullet_exists = 0, bullet_dir = -1;

    // Primary game loop, each iteration of the loop will equal one frame
    while (1) {
        // Get the rotation from the ADC and map it between 0 and 4
        p->rotation = readADC() / 819;
        if (p->rotation > 4)
            p->rotation = 4;

        // Draw the ship based on the player's current rotation and position
        drawShip(p);

        // Handle player input
        handleInput(p);

        // Draw the bullet based on current bullet position, player position, rotation, and bullet direction
        drawBullet(&bullet_x, &bullet_y, &bullet_exists, p->x, p->y, p->rotation, &bullet_dir);

        // Delay for 25ms to prevent the game from running too fast
        delay(25);
    }
}

// Draw the ship based off of the current rotation and player position
void drawShip(struct player *p) {
    putImage(p->x, p->y, SHIP_WIDTH, SHIP_HEIGHT, p->sprites[p->rotation], 0, 0);
}

// Draw the bullet based off of the current bullet position, player position, rotation, and bullet direction
void drawBullet(int *bullet_x, int *bullet_y, int *bullet_exists, int player_x, int player_y, int rotation, int *bullet_dir)
{
    
    // If bullet does not exist, create a bullet based off of the players current rotation angle, and store it in bullet_dir
    // 0 = left, 1 = left-up, 2 = up, 3 = right-up, 4 = right
    if (!(*bullet_exists)) {
        switch (rotation) {
            case 0:
                *bullet_x = player_x; 
                *bullet_y = player_y + (SHIP_HEIGHT / 2) - 1;
                *bullet_dir = 0;
                break;
            case 1:
                *bullet_x = player_x; 
                *bullet_y = player_y;
                *bullet_dir = 1;
                break;
            case 2:
                *bullet_x = player_x + (SHIP_WIDTH / 2) - 1; 
                *bullet_y = player_y;
                *bullet_dir = 2;
                break;
            case 3:
                *bullet_x = player_x + (SHIP_WIDTH);
                *bullet_y = player_y;
                *bullet_dir = 3;
            break;
            case 4:
                *bullet_x = player_x + (SHIP_WIDTH); 
                *bullet_y = player_y + (SHIP_HEIGHT / 2) - 1;
                *bullet_dir = 4;
                break;
        }

        playSound(500, 25);
        *bullet_exists = 1;
    }
    else
    {
        fillRectangle(*bullet_x, *bullet_y, BULLET_WIDTH, BULLET_HEIGHT, RGBToWord(0,0,0));
        
        switch (*bullet_dir) {
            case 0:
                *bullet_x -= BULLET_SPEED; 
                break;
            case 1:
                *bullet_x -= BULLET_SPEED / 2;
                *bullet_y -= BULLET_SPEED / 2;
                break;
            case 2:
                *bullet_y -= BULLET_SPEED;
                break;
            case 3:
                *bullet_x += BULLET_SPEED / 2;
                *bullet_y -= BULLET_SPEED / 2;
            break;
            case 4:
                *bullet_x += BULLET_SPEED;
                break;
        }
        fillRectangle(*bullet_x, *bullet_y, BULLET_WIDTH, BULLET_HEIGHT, RGBToWord(255,255,255));

        // If bullet goes out of bounds
        if (*bullet_y < 0 || *bullet_y > SCREEN_HEIGHT || *bullet_x < 0 || *bullet_x > SCREEN_WIDTH) {
            fillRectangle(*bullet_x, *bullet_y, BULLET_WIDTH, BULLET_HEIGHT, RGBToWord(0,0,0));
            *bullet_exists = 0;
        }
    }
}

void handleInput(struct player *p)
{
    // Handle right input by increasing the players current X
    if ((GPIOB->IDR & (1 << 4)) == 0) {
        // Check if player is within bounds first
        if (p->x < (SCREEN_WIDTH - SHIP_WIDTH))
            p->x++;
    }

    // Handle left input by decreasing the players current 
    if ((GPIOB->IDR & (1 << 5)) == 0) {
        // Check if player is within bounds first
        if (p->x > 0)
            p->x--;
    }

    // Handle up input by increasing the players current Y
    if ((GPIOA->IDR & (1 << 8)) == 0) {
        // Check if player is within bounds first
        if (p->y > 0)
            p->y--;
    }

    // Handle down input by decreasing the players current Y
    if ((GPIOA->IDR & (1 << 11)) == 0) {
        // Check if player is within bounds first
        if (p->y < (SCREEN_HEIGHT - SHIP_HEIGHT))
            p->y++;
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

void initADC()
{
    // Enable GPIOA clock
    RCC->AHBENR |= (1 << 17); // Port A is bit 17 in AHBENR

    // Set PA0 to analog mode
    pinMode(GPIOA, 0, 3); // Analog mode configuration

    // Enable ADC clock
    RCC->APB2ENR |= (1 << 9); // ADC1 clock enable

    // Configure ADC prescaler (optional, but good practice)
    ADC1->CFGR2 &= ~(3 << 30); // Set prescaler to no division (ADCCLK = PCLK)

    // Begin ADC calibration
    ADC1->CR |= ADC_CR_ADCAL; // Start calibration
    while (ADC1->CR & ADC_CR_ADCAL); // Wait until calibration is complete

    // Configure the ADC
    ADC1->CHSELR = (1 << 0); // Select channel 0(PA0)

    // Enable ADC
    ADC1->CR |= ADC_CR_ADEN; // Enable the ADC
    while (!(ADC1->ISR & ADC_ISR_ADRDY)); // Wait until ADC is ready
}

int readADC()
{
	// Trigger a conversion
	ADC1->CR |=  (1 << 2);

	// Wait for End of Conversion
	while ( (ADC1->CR & 1 << 2));

	// return result
	return ADC1->DR;
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

void initAudio() {
    // Enable GPIOB and TIM3
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    
    // Set PB1 to alternate function (AF1) for TIM3_CH4
    GPIOB->MODER &= ~(3u << (1 * 2));
    GPIOB->MODER |= (2u << (1 * 2));   // Alternate function mode
    GPIOB->AFR[0] |= (1u << (1 * 4));  // AF1 for PB1 (TIM3_CH4)

    // Set PWM mode for TIM3 Channel 4
    TIM3->PSC = 0;  // No prescaler, adjust if needed for pitch
    TIM3->ARR = 48000;  // Auto-reload value for 1kHz tone (48MHz / 48000)
    TIM3->CCR4 = 24000;  // 50% duty cycle
    TIM3->CCMR2 |= (6 << 12);  // PWM mode 1 on CH4
    TIM3->CCER |= TIM_CCER_CC4E;  // Enable TIM3 CH4
    TIM3->CR1 |= TIM_CR1_CEN;  // Enable timer
}

void playSound(uint32_t frequency, uint32_t duration) {
    TIM3->ARR = 48000000 / frequency;  // Set ARR for desired frequency
    TIM3->CCR4 = TIM3->ARR / 2;  // 50% duty cycle for the tone
    delay(duration);  // Play for specified duration
    TIM3->CCR4 = 0;  // Stop the sound after the delay
}