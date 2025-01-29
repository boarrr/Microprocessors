.global _start
_start:
	mov SP, #0xf0
	ldr R0, =x
	ldr R1, =y
	
	bl swap
	b .
	

// On entry: R0 points at x, R1 points at y
swap:
	push {R7,LR} // back up R7 and SP as they will change
	sub SP,#4 // make space for the local variable (uint32_t temp)
	mov R7,SP // will use R7 as a stack frame pointer in case SP moves around
	ldr R2,[R0] // read what x points at
	str R2,[R7] // write it to temp (temp=*x)
	ldr R2,[R1] // read what y points at
	str R2,[R0] // write it to where x points (*x=*y)
	LDR R2,[R7] // read temp
	str R2,[R1] // write it to where y points (*y=temp)
	add SP,#4 // free space used by local variables
	pop {R7,PC} // retore R7 and return to caller

x: .word 89
y: .word 200


