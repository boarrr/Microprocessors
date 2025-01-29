    .data
        
    .text
        
    .syntax unified
    .cpu cortex-m0plus
    .global  main

main:
    ldr r0, =0x7fffffff
    adds r0, #2
    
    
exit:
    b .

    .align 4




