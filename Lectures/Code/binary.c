#include <stdint.h>
#include <stdio.h>

uint8_t ODR; // fake output data register

void printBinary(uint8_t b)
{
	for ( int i=0; i < 8; i++) {
		if (b & 128) // check to see if most significant bit is set
			printf("1");	
		else
			printf("0");
	
		b = b << 1; // shift value left to test next bit
	}
}

void green_on()
{
	ODR = ODR | (1 << 0);
}

void yellow_on()
{
	ODR = ODR | (1 << 1);
}

void yellow_off()
{
	ODR = ODR & ~(1 << 1);
}

int testBit(uint8_t b, uint8_t bit_number)
{
	// returns 1 if the specified bit is a 1
	if (b & (1 << bit_number) )
		return 1;
	
	return 0;	
}

void red_on()
{
	ODR = ODR | (1 << 2);
}

int main()
{
	/*
	// AND operations
	printf("AND \n");
	printBinary(112);
	printf("\n");
	printBinary(~(1<<5));
	printf("\n");
	printf("=========\n");
	printBinary(112 & ~(1<<5));
	printf("\n");
	*/
	// OR operations
	/*
	printf("ODR: ");
	printf("\n");
	printBinary(ODR);
	printf("\n");
	green_on();
	printBinary(ODR);
	printf("\n");
	yellow_on();
	printBinary(ODR);
	printf("\n");
	red_on();
	printBinary(ODR);
	printf("\n");
	yellow_off();
	printBinary(ODR);
	printf("\n");
	*/
	/*
	// XOR operations
	ODR = 0;
	while(1)
	{
		ODR = ODR ^ (1 << 2);
		printBinary(ODR);
		printf("\n");
	}
	*/

	// Bit testing
	printBinary(7);
	printf("\nTest bit 1: Bit is a %d\n",testBit(7,1));
	
}
