#include <stm32f031x6.h>
#include "display.h"
#include <stdlib.h>  // for rand()

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 160
#define BLOCK_SIZE 10
#define BOARD_WIDTH (SCREEN_WIDTH / BLOCK_SIZE)
#define BOARD_HEIGHT (SCREEN_HEIGHT / BLOCK_SIZE)
#define NUM_PIECES 4 // Number of tetrominoes

// Function declarations
void initClock(void);
void initSysTick(void);
void SysTick_Handler(void);
void delay(volatile uint32_t dly);
void setupIO();
void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber);
void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode);

// Global variable for tracking time
volatile uint32_t milliseconds;

void drawTetrisPiece(uint16_t x, uint16_t y, const uint8_t piece[4][4], uint16_t colour);
void clearTetrisPiece(uint16_t x, uint16_t y, const uint8_t piece[4][4]);
uint8_t getPieceHeight(const uint8_t piece[4][4]);
uint8_t checkCollision(uint16_t x, uint16_t y, const uint8_t piece[4][4]);
void gameOver();

// Tetromino definitions
const uint8_t I_piece[4][4] = {
    {0, 0, 0, 0},
    {1, 1, 1, 1},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
};

const uint8_t O_piece[4][4] = {
    {1, 1, 0, 0},
    {1, 1, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
};

const uint8_t T_piece[4][4] = {
    {0, 1, 0, 0},
    {1, 1, 1, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
};

const uint8_t L_piece[4][4] = {
    {0, 0, 1, 0},
    {1, 1, 1, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
};

// Array of all pieces
const uint8_t *pieces[NUM_PIECES] = {I_piece, O_piece, T_piece, L_piece};

// Board structure: store the color of the locked pieces (0 means empty)
uint16_t board[BOARD_HEIGHT][BOARD_WIDTH] = {0};

// Piece colors, initialized at runtime
uint16_t piece_colors[NUM_PIECES];

int main()
{
    // Initialize the system and display
    initClock();
    initSysTick();
    setupIO();
    
    // Initialize piece colors
    piece_colors[0] = RGBToWord(0, 255, 0);  // Green for I_piece
    piece_colors[1] = RGBToWord(255, 255, 0);  // Yellow for O_piece
    piece_colors[2] = RGBToWord(255, 0, 255);  // Purple for T_piece
    piece_colors[3] = RGBToWord(255, 0, 0);     // Red for L_piece

    // Seed the random number generator
    srand(milliseconds);

    // Game variables
    uint16_t player_x = 50;  // Starting X position of the piece
    uint16_t player_y = 0;   // Starting Y position of the piece
    uint8_t piece_index = rand() % NUM_PIECES;  // Randomly select a piece
    const uint8_t (*current_piece)[4] = pieces[piece_index];  // Current falling piece
    uint8_t piece_height = getPieceHeight(current_piece);      // Get the height of the piece
    uint16_t current_color = piece_colors[piece_index];        // Get the color of the piece

    // Main game loop
    while (1)
    {
        // Clear the previous position of the piece
        clearTetrisPiece(player_x, player_y, current_piece);
        
        // Check for collision before moving the piece
        if (!checkCollision(player_x, player_y + BLOCK_SIZE, current_piece)) {
            // Move the piece down
            player_y += BLOCK_SIZE;
            // Handle right input by increasing the players current X
            if ((GPIOB->IDR & (1 << 4)) == 0 && player_x < (SCREEN_WIDTH - (BLOCK_SIZE * 4))) {
                player_x += 10;
            }

        } else {
            // Collision detected: lock the piece to the board
            for (uint8_t row = 0; row < 4; row++) {
                for (uint8_t col = 0; col < 4; col++) {
                    if (current_piece[row][col] == 1) {
                        board[(player_y / BLOCK_SIZE) + row][(player_x / BLOCK_SIZE) + col] = current_color;
                    }
                }
            }

            // Check if the piece reached the top and trigger game over
            if (player_y == 0) {
                gameOver();
                break;
            }

            // Pick a new piece and reset its position
            player_x = 50;
            player_y = 0;
            piece_index = rand() % NUM_PIECES;
            current_piece = pieces[piece_index];
            piece_height = getPieceHeight(current_piece);
            current_color = piece_colors[piece_index]; // Set the color of the new piece
        }

        // Draw the piece in its new position
        drawTetrisPiece(player_x, player_y, current_piece, current_color);  // Use the piece's color

        // Redraw the locked pieces on the board
        for (uint8_t row = 0; row < BOARD_HEIGHT; row++) {
            for (uint8_t col = 0; col < BOARD_WIDTH; col++) {
                if (board[row][col] != 0) {
                    fillRectangle(col * BLOCK_SIZE, row * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, board[row][col]); // Draw the locked piece in its original color
                }
            }
        }

        // Delay to control the falling speed
        delay(200);  // 200ms delay (adjust for faster or slower fall speed)
    }

    return 0;
}

// Function to draw a Tetris piece at (x, y)
void drawTetrisPiece(uint16_t x, uint16_t y, const uint8_t piece[4][4], uint16_t colour)
{
    for (uint8_t row = 0; row < 4; row++)
    {
        for (uint8_t col = 0; col < 4; col++)
        {
            if (piece[row][col] == 1)
            {
                fillRectangle(x + col * BLOCK_SIZE, y + row * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, colour);
            }
        }
    }
}

// Function to clear the Tetris piece by drawing over it with the background color (black)
void clearTetrisPiece(uint16_t x, uint16_t y, const uint8_t piece[4][4])
{
    for (uint8_t row = 0; row < 4; row++)
    {
        for (uint8_t col = 0; col < 4; col++)
        {
            if (piece[row][col] == 1)
            {
                fillRectangle(x + col * BLOCK_SIZE, y + row * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, RGBToWord(0, 0, 0)); // Black background
            }
        }
    }
}

// Function to calculate the height of the piece (number of non-empty rows)
uint8_t getPieceHeight(const uint8_t piece[4][4])
{
    uint8_t height = 0;
    for (uint8_t row = 0; row < 4; row++)
    {
        for (uint8_t col = 0; col < 4; col++)
        {
            if (piece[row][col] == 1)
            {
                height = row + 1;  // Track the bottom-most row containing a block
            }
        }
    }
    return height;
}

// Function to check if the piece would collide with another piece or the bottom
uint8_t checkCollision(uint16_t x, uint16_t y, const uint8_t piece[4][4])
{
    for (uint8_t row = 0; row < 4; row++)
    {
        for (uint8_t col = 0; col < 4; col++)
        {
            if (piece[row][col] == 1)
            {
                // Check if we're about to collide with the bottom of the screen
                if ((y / BLOCK_SIZE) + row >= BOARD_HEIGHT) {
                    return 1;  // Collision detected
                }

                // Check if we're about to collide with another piece
                if (board[(y / BLOCK_SIZE) + row][(x / BLOCK_SIZE) + col] != 0) {
                    return 1;  // Collision detected
                }
            }
        }
    }
    return 0;  // No collision
}

// Function to display "Game Over" and halt the game
void gameOver()
{
    // Clear the screen
    fillRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, RGBToWord(0, 0, 0)); // Black background

    // Display "Game Over" text
    printTextX2("Game Over", 20, 60, RGBToWord(255, 0, 0), RGBToWord(0, 0, 0)); // Red Game Over text
}

void initSysTick(void)
{
    SysTick->LOAD = 48000;
    SysTick->CTRL = 7;
    SysTick->VAL = 10;
    __asm(" cpsie i "); // Enable interrupts
}

void SysTick_Handler(void)
{
    milliseconds++;
}

void initClock(void)
{
    // Ensure PLL is disabled
    RCC->CR &= ~(1u<<24);
    while( (RCC->CR & (1 <<25))); // Wait for PLL ready to be cleared

    FLASH->ACR |= (1 << 0); // Flash latency
    FLASH->ACR |= (1 << 4); // Enable Flash prefetch buffer

    // Set PLL multiplier to 12 (yielding 48MHz)
    RCC->CFGR &= ~((1u<<21) | (1u<<20) | (1u<<19) | (1u<<18));
    RCC->CFGR |= ((1<<21) | (1<<19));

    // Set ADC prescaler to 4
    RCC->CFGR |= (1<<14);

    // Turn PLL back on
    RCC->CR |= (1<<24);

    // Set PLL as system clock source
    RCC->CFGR |= (1<<1);
}

void delay(volatile uint32_t dly)
{
    uint32_t end_time = dly + milliseconds;
    while(milliseconds != end_time)
        __asm(" wfi "); // Sleep until next interrupt
}

void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber)
{
    Port->PUPDR = Port->PUPDR &~(3u << BitNumber*2); // Clear pull-up resistor bits
    Port->PUPDR = Port->PUPDR | (1u << BitNumber*2); // Set pull-up bit
}

void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode)
{
    uint32_t mode_value = Port->MODER;
    Mode = Mode << (2 * BitNumber);
    mode_value = mode_value & ~(3u << (BitNumber * 2));
    mode_value = mode_value | Mode;
    Port->MODER = mode_value;
}

void setupIO()
{
    RCC->AHBENR |= (1 << 18) + (1 << 17); // Enable Ports A and B
    display_begin();                      // Initialize the display
    pinMode(GPIOB,4,0);
	pinMode(GPIOB,5,0);
	pinMode(GPIOA,8,0);
	pinMode(GPIOA,11,0);
	enablePullUp(GPIOB,4);
	enablePullUp(GPIOB,5);
	enablePullUp(GPIOA,11);
	enablePullUp(GPIOA,8);
}
