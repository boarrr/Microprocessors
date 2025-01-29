// For more information regarding assembler directives look here:
// https://sourceware.org/binutils/docs/as/ARM-Directives.html

    .text  // what follows goes in ROM
    .syntax unified // use the more modern form of ARM assembly language
    .cpu cortex-m0 // which model CPU we have
	// various constants for code that follows
    .equ ODR,0x14
	.equ IDR,0x10
	.equ IOPENR,0x2C
	.equ GPIOB,0x50000400
	.equ RCC,0x40021000

	// mark symbols as being globally visible	
    .global asm_delay  // allow other project elements link to the symbol asm_delay
    .global asm_setPortBit  // allow other project elements link to the symbol asm_setPortBit
	.global asm_clearPortBit  // allow other project elements link to the symbol asm_clearPortBit
	.global asm_setup  // allow other project elements link to the symbol asm_setup
	
asm_delay:
	// delay routine in assembler
	// on entry R0 contains a number
	// this function counts down to zero from this number
	// and then returns
	subs R0,R0,#1
	bne	asm_delay
	bx LR

asm_setPortBit:
// On entry: 
// R0 points at the port (structure)
// R1 contains the bit number
	push {R0-R3,LR}	
	movs R2, #1
	lsls R2,R2,R1
	ldr R3,[R0,#ODR]
	orrs R3,R2
	str R2,[R0,#ODR]
	pop {R0-R3,PC}
	

asm_clearPortBit:
// On entry:
// R0 points at the port (structure)
// R1 contains the bit number
	push {R0-R3,LR}	
	movs R2, #1
	lsls R2,R2,R1
	mvns R2,R2
	ldr R3,[R0,#ODR]
	ands R3,R2
	str R2,[R0,#ODR]
	pop {R0-R3,PC}
	
asm_setup:
    // Set up GPIOB
	push {R0-R3,LR}

    ldr R3, =RCC           
    movs R1, #2            
    str R1, [R3, #IOPENR]

	// Set up registers for pinMode
	// R0 contains the port address, R1 contains the pin number, R2 contains the mode
    ldr R0, =GPIOB        
    movs R1, #3            
    movs R2, #1            
    bl pinMode
	
	pop {R0-R3,PC}

   .end
