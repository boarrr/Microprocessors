.global _start
_start:
	ldr r0, =destination       
	ldr r1, =source
	
	bl a_strcpy            
	b .                    

a_strcpy:
	loop:
		ldrb r3, [r1]         
		strb r3, [r0]          
		cmp r3, #0             
		beq a_strcpy_exit      
		adds r1, r1, #1        
		adds r0, r0, #1       
		b loop                 

a_strcpy_exit:
	bx lr                  

source: .asciz "source"     
destination: .space 20      
