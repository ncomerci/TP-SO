GLOBAL _sys_system
GLOBAL _sys_process
GLOBAL _sys_timet
GLOBAL _sys_rtc
GLOBAL _sys_fd
GLOBAL _sys_screen
GLOBAL _sys_video
GLOBAL _sys_sound
GLOBAL _sys_ksem

GLOBAL getRegisters
GLOBAL testDivException

GLOBAL spin_lock
GLOBAL spin_unlock

section .text
; rdi, rsi, rdx, rcx, r8, r9
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

_sys_system:
    BuildSF
    preserve

    mov r8, rcx   ; Arguments Shifting
    mov rcx, rdx
    mov rdx, rsi
    mov rsi, rdi
    mov rdi, SYS_SYSTEM_CODE
    int 80h

    recover
    breakSF
    ret   

_sys_process:
    BuildSF
    preserve

    mov r8, rcx   ; Arguments Shifting
    mov rcx, rdx
    mov rdx, rsi
    mov rsi, rdi
    mov rdi, SYS_PROCESS_CODE
    int 80h

    recover
    breakSF
    ret

_sys_timet:
    BuildSF
    preserve

    mov rcx, rdx  ; Arguments Shifting
    mov rdx, rsi
    mov rsi, rdi
    mov rdi, SYS_TICKS_CODE
    int 80h

    recover
    breakSF
    ret

_sys_rtc:
    BuildSF
    preserve

    mov rsi, rdi  ; Arguments Shifting
    mov rdi, SYS_RTC_CODE
    int 80h

    recover
    breakSF
    ret

_sys_fd:
    BuildSF
    preserve

    mov r9, r8    ; Arguments Shifting
    mov r8, rcx
    mov rcx, rdx
    mov rdx, rsi
    mov rsi, rdi
    mov rdi, SYS_FD_CODE
    int 80h

    recover
    breakSF
    ret

_sys_screen:
    BuildSF
    preserve

    mov rcx, rdx  ; Arguments Shifting
    mov rdx, rsi
    mov rsi, rdi
    mov rdi, SYS_SCREEN_CODE
    int 80h

    recover
    breakSF
    ret    

_sys_video:
    BuildSF
    preserve

    mov r9, r8    ; Arguments Shifting
    mov r8, rcx
    mov rcx, rdx
    mov rdx, rsi
    mov rsi, rdi
    mov rdi, SYS_VIDEO_CODE
    int 80h

    recover
    breakSF    
    ret

_sys_sound:
    BuildSF
    preserve

    mov rcx, rdx  ; Arguments Shifting 
    mov rdx, rsi
    mov rsi, rdi
    mov rdi, SYS_SOUND_CODE
    int 80h

    recover
    breakSF    
    ret

_sys_ksem:
    BuildSF
    preserve    

    mov r8, rcx   ; Arguments Shifting
    mov rcx, rdx 
    mov rdx, rsi
    mov rsi, rdi
    mov rdi, SYS_KSEM_CODE
    int 80h

    recover
    breakSF
    ret

getRegisters:
    BuildSF
    preserve

    mov [array], rax
    mov [array + 8], rbx
    mov [array + 8*2], rcx
    mov [array + 8*3], rdx
    mov [array + 8*4], rbp
    mov [array + 8*5], rdi
    mov [array + 8*6], rsi
    mov [array + 8*7], r8
    mov [array + 8*8], r9
    mov [array + 8*9], r10
    mov [array + 8*10], r11
    mov [array + 8*11], r12
    mov [array + 8*12], r13
    mov [array + 8*13], r14
    mov [array + 8*14], r15
    
    mov rax, array

    recover
    breakSF
	ret


testDivException:
    mov rax, 3
    mov rbx, 0
    DIV rbx
    ret

spin_lock:
    mov eax, 1
    xchg eax, [rdi]
    test eax, eax

    jnz spin_lock
    ret

spin_unlock: 
    mov     eax, 0     
    xchg    eax, [rdi] 
    ret                

section .bss
array resb 8*16

; Sys_calls codes
SYS_SYSTEM_CODE equ 0
SYS_PROCESS_CODE equ 1
SYS_TICKS_CODE equ 2
SYS_RTC_CODE equ 3
SYS_FD_CODE equ 4
SYS_SCREEN_CODE equ 5
SYS_VIDEO_CODE equ 6
SYS_SOUND_CODE equ 7
SYS_KSEM_CODE equ 8