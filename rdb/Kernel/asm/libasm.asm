GLOBAL cpuVendor
GLOBAL getRTC
GLOBAL kbFlag
GLOBAL kbGet
GLOBAL _inportb
GLOBAL _outportb
GLOBAL _set_rsp
GLOBAL getRegisters
GLOBAL _prepare_stack_process_create
GLOBAL _halt_and_wait
GLOBAL _halter
GLOBAL _start_process
GLOBAL _int81

GLOBAL spin_lock
GLOBAL spin_unlock

EXTERN exit

section .text

%macro preserve 0
	push rbx
    push r12
    push r13
    push r15
%endmacro

%macro recover 0
	pop r15
	pop r13
	pop r12
	pop rbx
%endmacro

%macro BuildSF 0
    push rbp
    mov rbp, rsp
%endmacro

%macro breakSF 0
    mov rsp, rbp
    pop rbp
%endmacro
	
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

_start_process:
    push rbx
    mov rbx, rdi       ; main
    mov rdi, rsi       ; argc
    mov rsi, rdx       ; argv

    call rbx           ; Llamada al main

    call exit

    pop rbx
    ret                ; Retorno lo mismo que main

_halt_and_wait:
    mov al, 20h
	out 20h, al
    sti

    hlt
    ret

_halter:
    sti
    hlt
    jmp _halter
    ret

_int81:
    int 81h
    ret

spin_lock:
    mov rax, 1
    xchg rax, [rdi]
    test rax, rax

    jnz spin_lock
    ret

spin_unlock:

    mov     rax, 0          ; Set the EAX register to 0.
    xchg    rax, [rdi]      ; Atomically swap the EAX register with
                            ;  the lock variable.
    ret                     ; The lock has been released.
