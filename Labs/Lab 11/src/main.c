#include <stdint.h>
#include <stm32l031xx.h>

void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode);
void delay(volatile uint32_t dly);
void asm_delay(volatile uint32_t dly);
void setPortBit( GPIO_TypeDef *Port, uint32_t BitNumber);
void asm_setPortBit( GPIO_TypeDef *Port, uint32_t BitNumber);
void clearPortBit( GPIO_TypeDef *Port, uint32_t BitNumber);
void asm_clearPortBit( GPIO_TypeDef *Port, uint32_t BitNumber);
void asm_setup();

void setup()
{
    RCC->IOPENR = 2;   // Turn on GPIOB
	pinMode(GPIOB, 3, 1); // Make GPIOB_3 an output    
}

int main()
{
	asm_setup();
	while(1)
	{
            asm_setPortBit(GPIOB,3);
			asm_delay(100000);  // Wait
			asm_clearPortBit(GPIOB,3);
			asm_delay(100000);  // Wait
	}
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
	Mode = Mode << (2 * BitNumber);    // There are two Mode bits per port bit 
							     //so need to shift
																	// the mask for Mode up to the proper location
	mode_value = mode_value & ~(3u << (BitNumber * 2)); // Clear out old mode bits
	mode_value = mode_value | Mode; // set new bits
	Port->MODER = mode_value; // write back to port mode register
}
void delay(volatile uint32_t dly)
{
	while(dly--);
}
void setPortBit( GPIO_TypeDef *Port, uint32_t BitNumber)
{
	uint32_t mask;
	mask = 1u << BitNumber;
	Port->ODR = Port->ODR | mask;
}
void clearPortBit( GPIO_TypeDef *Port, uint32_t BitNumber)
{
	uint32_t mask;
	mask = 1u << BitNumber;
	Port->ODR = Port->ODR & ~mask;
}