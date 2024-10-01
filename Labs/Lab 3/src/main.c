#include <stm32l031xx.h>
#include <stdint.h>

void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode);
void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber);
void delay(volatile uint32_t dly);
void initClockHSI16(void);
void initADC(void);
int readADC(void);
uint32_t prbs(void);
void randomize(void);
uint32_t random(uint32_t lower,uint32_t upper);
void redOn(void);
void redOff(void);
void yellowOn(void);
void yellowOff(void);
void greenOn(void);
void greenOff(void);
void initSound(void);
void playNote(uint32_t freq);

int redButtonPressed();
int yellowButtonPressed();
int greenButtonPressed();

int main()
{
    uint8_t choice = 0;
    uint8_t prev_choice = 0;
    int timer_limit = 500000;   // Initial time limit for guessing
    const int min_timer_limit = 50000;  // Minimum allowed time limit for guessing

    RCC->IOPENR |= (1 << 0) + (1 << 1);
    initClockHSI16();

    pinMode(GPIOA,0,1); // Make GPIOA bit 0 an output
    pinMode(GPIOA,1,1); // Make GPIOA bit 1 an output
    pinMode(GPIOA,2,1); // Make GPIOA bit 2 an output
    pinMode(GPIOA,7,3); // Use PA7 as an analog input to seed the random number generator

    // Set buttons as input
    pinMode(GPIOA,11,0);
    pinMode(GPIOB,4,0);
    pinMode(GPIOB,5,0);

    // Enable Pull-up on buttons
    enablePullUp(GPIOA,11);
    enablePullUp(GPIOB,4);
    enablePullUp(GPIOB,5);

    initADC();
    initSound();
    randomize();

    while(1)
    {   
        int timer = timer_limit;

        // Ensure a different choice each time
        while (1) {
            choice = random(0, 3);

            if (choice != prev_choice) {
                prev_choice = choice;
                break;
            }
        }

        switch(choice) {
            case 0: {
                greenOn();

                while (timer) {
                    if (greenButtonPressed()) {
                        playNote(1000);

                        while (greenButtonPressed());

                        // Reduce the time limit for the next round, down to the minimum value
                        if (timer_limit > min_timer_limit) {
                            timer_limit -= 50000;
                        }

                        break;
                    } else if (redButtonPressed() || yellowButtonPressed()) {
                        playNote(400);

                        while (redButtonPressed() || yellowButtonPressed());

                        // Reset the time limit if the user makes a wrong guess
                        timer_limit = 500000;

                        break;
                    }

                    timer--;
                }

                break;
            }
            case 1: {
                yellowOn();

                while (timer) {
                    if (yellowButtonPressed()) {
                        playNote(1000);
                        
                        while (yellowButtonPressed());

                        // Reduce the time limit for the next round
                        if (timer_limit > min_timer_limit) {
                            timer_limit -= 50000;
                        }

                        break;
                    } else if (redButtonPressed() || greenButtonPressed()) {
                        playNote(400);
                        
                        while(redButtonPressed() || greenButtonPressed());

                        // Reset the time limit if the user makes a wrong guess
                        timer_limit = 500000;

                        break;
                    }

                    timer--;
                }

                break;
            }
            case 2: {
                redOn();

                while (timer) {
                    if (redButtonPressed()) {
                        playNote(1000);

                        while(redButtonPressed());

                        // Reduce the time limit for the next round
                        if (timer_limit > min_timer_limit) {
                            timer_limit -= 50000;
                        }

                        break;
                    } else if (greenButtonPressed() || yellowButtonPressed()) {
                        playNote(400);

                        while(greenButtonPressed() || yellowButtonPressed());
                
                        // Reset the time limit if the user makes a wrong guess
                        timer_limit = 500000;

                        break;
                    }

                    timer--;
                }

                break;
            }
        } 

        // If timer hit 0, the user did not guess in time
        if (!timer) timer_limit = 500000;

        delay(500000);

        playNote(0);
        redOff();
        yellowOff();
        greenOff();
    }

    return 0;
}


void redOn()
{
    GPIOA->ODR |= (1 << 2);
}

void redOff()
{
    GPIOA->ODR &= ~(1 << 2);
}

void yellowOn()
{
    GPIOA->ODR |= (1 << 1);
}

void yellowOff()
{
    GPIOA->ODR &= ~(1 << 1);
}

void greenOn()
{
    GPIOA->ODR |= (1 << 0);
}

void greenOff()
{
    GPIOA->ODR &= ~(1 << 0);
}

void initSound()
{    
    // Sound will be produced by sending the output from Timer 2, channel 4 out PB1
    pinMode(GPIOB,1,2); // select alternative function for PB1
    GPIOB->AFR[0] &= ~( (0b1111) << 1*4); // zero out alternate function bits
    GPIOB->AFR[0] |= ( (0b0101) << 1*4); // set required alternate function bits (AF5)
    
    RCC->APB1ENR |= (1 << 0);  // enable Timer 2
    TIM2->CR1 = 0; // Set Timer 2 to default values
	TIM2->CCMR2 = (1 << 14) + (1 << 13);
	TIM2->CCER |= (1 << 12);
	TIM2->PSC = 3;
	TIM2->ARR = 1UL;
	TIM2->CCR4 = TIM2->ARR/2;	
	TIM2->CNT = 0;
}

void playNote(uint32_t freq)
{
    // Counter is running at 4MHz 
    // Lowest possible frequency = 4000000/65536 = 61 Hz approx
    if (freq == 0)
    {
        TIM2->CR1 &= ~(1 << 0); // disable the counter
    }

	TIM2->ARR = (uint32_t)4000000/((uint32_t)freq); 
	TIM2->CCR4= TIM2->ARR/2;	
	TIM2->CNT = 0; // set the count to zero initially
	TIM2->CR1 |= (1 << 0); // and enable the counter
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

void delay(volatile uint32_t dly)
{
	while(dly--);	
}

uint32_t random(uint32_t lower,uint32_t upper)
{
    return (prbs()%(upper-lower))+lower;
}

uint32_t shift_register=0;

void randomize(void)
{
    // uses ADC noise values to seed the shift_register
    while(shift_register==0)
    {
        for (int i=0;i<10;i++)
        {
            shift_register+=(readADC()<<i);
        }
    }
}

uint32_t prbs()
{
	// This is an unverified 31 bit PRBS generator
	// It should be maximum length but this has not been verified 
	unsigned long new_bit=0;	

    new_bit= ((shift_register & (1<<27))>>27) ^ ((shift_register & (1<<30))>>30);
    new_bit= ~new_bit;
    new_bit = new_bit & 1;
    shift_register=shift_register << 1;
    shift_register=shift_register | (new_bit);
		
	return shift_register & 0x7fffffff; // return 31 LSB's 
}

void initClockHSI16()
{
    // Use the HSI16 clock as the system clock - allows operation down to 1.5V
    RCC->CR &= ~(1<<24);
    RCC->CR |= (1 << 0); // turn on HSI16 (16MHz clock)
    while ((RCC->CR & (1<<2))==0); // wait for HSI to be ready
    // set HSI16 as system clock source 
    RCC->CFGR |= (1<<0);
}

void initADC()
{
	// Turn on ADC 
	RCC->APB2ENR |= (1 << 9);		
    // Enable the reference voltage
	ADC->CCR |= (1 << 22);	
	// Begin ADCCalibration	
	ADC1->CR = ( 1 << 31);
	// Wait for calibration complete:  
	while ((ADC1->CR & ( 1 << 31)));
	// Select Channel 7
	ADC1->CR |= (1 << 7);	
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

int redButtonPressed()
{
    if ((GPIOA->IDR & (1 << 11)) == 0) {
        return 1;
    }

    return 0;
}

int yellowButtonPressed()
{
    if ((GPIOB->IDR & (1 << 5)) == 0) {
        return 1;
    }

    return 0;
}

int greenButtonPressed()
{
    if ((GPIOB->IDR & (1 << 4)) == 0) {
        return 1;
    }

    return 0;
}