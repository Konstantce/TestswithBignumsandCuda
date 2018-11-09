PUBLIC long_div_asm

;The first four integer arguments are passed in registers. 
;Integer values are passed (in order left to right) in RCX, RDX, R8, and R9. 
;Arguments five and higher are passed on the stack. 

.data

    hello1 db "Hello from ASM.",0

.code

;div(amd64): Unsigned divide RDX:RAX by r/m64, with result stored in RAX ← Quotient, RDX ← Remainder.
long_div_asm PROC
    mov rax, rdx
	mov rdx, rcx
	div r8
	mov [r9], rdx
    ret
long_div_asm ENDP

END