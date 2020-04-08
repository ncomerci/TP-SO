GLOBAL cpuVendor
GLOBAL getRTC
GLOBAL kbFlag
GLOBAL kbGet
GLOBAL _inportb
GLOBAL _outportb
GLOBAL _set_rsp
GLOBAL getRegisters

section .text
	
cpuVendor:
	push rbp
	mov rbp, rsp

	push rbx

	mov rax, 0
	cpuid


	mov [rdi], ebx
	mov [rdi + 4], edx
	mov [rdi + 8], ecx

	mov byte [rdi+13], 0

	mov rax, rdi

	pop rbx

	mov rsp, rbp
	pop rbp
	ret

getRTC:
    push rbp
    mov rbp, rsp

    mov rax, rdi
 
    out 70h, al
    in al, 71h
 
    mov rsp, rbp
    pop rbp
    ret

kbGet:
	push rbp
	mov rbp, rsp

	mov rax, 0
	in al, 0x60

	mov rsp, rbp
	pop rbp
	ret

_inportb:

    push rbp
    mov rbp, rsp
    mov rdx, rdi
    mov rax, 0
    in al, dx
    mov rsp, rbp
    pop rbp
    ret

_outportb:

    push rbp
    mov rbp, rsp
    mov rax, rsi
    mov rdx, rdi
    out dx, al
    mov rsp, rbp
    pop rbp
    ret
	
_set_rsp:
    mov rsp, rdi
    ret