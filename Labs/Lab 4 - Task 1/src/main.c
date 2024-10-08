#include <stm32l031xx.h>

void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode);
void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber);
void initClockHSI16(void);
void initPWM(void);
void doPWM(uint32_t pwm_percent);
void initADC(void);
int readADC(void);

// Simple delay funciton
void delay(volatile uint32_t dly)
{
    while(dly--);
}

// Global Array
const int32_t ary[]={50,70,80,90,100,90,80,70,50,30,20,10,0,10,20,30};

int percent = 0;

int main()
{
    
    initClockHSI16();
    initPWM();
    initADC();

    while(1)
    {     
		percent = readADC();
		percent = (percent * 100) / 4095;
		
        doPWM(percent);     
    }
}


// Enable pull up resistor
void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber)
{
	Port->PUPDR = Port->PUPDR &~(3u << BitNumber*2); // clear pull-up resistor bits
	Port->PUPDR = Port->PUPDR | (1u << BitNumber*2); // set pull-up bit
}

// Set the pin mode
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

// Init the PWM
void initPWM()
{    
    RCC->IOPENR |= (1 << 1); // ensure GPIOB is enabled

	// PWM output will be produced by sending the output from Timer 2, channel 4 out PB1
    pinMode(GPIOB,1,2); // select alternative function for PB1
    GPIOB->AFR[0] &= ~( (0b1111) << 1*4); // zero out alternate function bits
    GPIOB->AFR[0] |= ( (0b0101) << 1*4); // set required alternate function bits (AF5)
    
    RCC->APB1ENR |= (1 << 0);  // enable Timer 2
    TIM2->CR1 = 0; // Set Timer 2 to default values
	TIM2->CCMR2 = (1 << 14) + (1 << 13);
	TIM2->CCER |= (1 << 12);
	TIM2->PSC = 3;
	TIM2->ARR = 200;  // Input clock frequency is 4MHz so divide by 4095 to give 976Hz rate
	TIM2->CCR4 = 0;	
	TIM2->CNT = 0;
   	TIM2->CR1 |= (1 << 0); // ensure counter is enabled

}


void doPWM(uint32_t pwm_percent)
{
	//  Parameter : pwm_percent = a value between 0 and 100
	TIM2->CCR4 = (pwm_percent * TIM2->ARR) / 100; // Write to compare register
}

void initADC()
{
    RCC->IOPENR |= (1 << 0); // ensure GPIOA is enabled

	pinMode(GPIOA,3,3);

	// Turn on ADC 
	RCC->APB2ENR |= (1 << 9);	

    // Enable the reference voltage
	ADC->CCR |= (1 << 22);	

	// Begin ADCCalibration	
	ADC1->CR = ( 1 << 31);

	// Wait for calibration complete:  
	while ((ADC1->CR & ( 1 << 31)));

	// Select Channel 7
	ADC1->CHSELR |= (1 << 3);	
	 
	// Enable the ADC
	ADC1->CR |= (1 << 0);  
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