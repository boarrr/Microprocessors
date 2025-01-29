.global _start
_start:

	ldr R0,=string
	BL a_strlen
	B .

a_strlen:
	MOV R9,#0
a_strlen_loop:
	LDRB R10,[R0] 	// R10 = *R0
	CMP	R10,#0		// reached the null?
	BEQ a_strlen_exit // if so then exit
	ADDS R0,R0,#1
	ADDS R9,R9,#1
	B a_strlen_loop
		
a_strlen_exit:
	MOV R0,R9		// return R9
	BX LR		

string: .asciz "hello"