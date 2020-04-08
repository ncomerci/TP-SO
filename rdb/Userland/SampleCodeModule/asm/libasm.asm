GLOBAL _sys_system
GLOBAL _sys_timet
GLOBAL _sys_rtc
GLOBAL _sys_read
GLOBAL _sys_screen
GLOBAL _sys_video
GLOBAL _sys_sound

GLOBAL getRegisters
GLOBAL testDivException

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
    mov r8, rcx   ; Arguments Shifting
    mov rcx, rdx
    mov rdx, rsi
    mov rsi, rdi
    mov rdi, SYS_SYSTEM_CODE
    int 80h
    ret   

_sys_timet:
    mov rcx, rdx  ; Arguments Shifting
    mov rdx, rsi
    mov rsi, rdi
    mov rdi, SYS_TICKS_CODE
    int 80h
    ret

_sys_rtc:
    mov rsi, rdi  ; Arguments Shifting
    mov rdi, SYS_RTC_CODE
    int 80h
    ret

_sys_read:
    mov rsi, rdi  ; Arguments Shifting
    mov rdi, SYS_READ_CODE
    int 80h
    ret

_sys_screen:
    mov r8, rcx   ; Arguments Shifting
    mov rcx, rdx
    mov rdx, rsi
    mov rsi, rdi
    mov rdi, SYS_SCREEN_CODE
    int 80h
    ret    

_sys_video:
    mov r9, r8    ; Arguments Shifting
    mov r8, rcx
    mov rcx, rdx
    mov rdx, rsi
    mov rsi, rdi
    mov rdi, SYS_VIDEO_CODE
    int 80h
    ret

_sys_sound:
    mov rdx, rsi  ; Arguments Shifting
    mov rsi, rdi
    mov rdi, SYS_SOUND_CODE
    int 80h
    ret

getRegisters:
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
	ret


testDivException:
    mov rax, 3
    mov rbx, 0
    DIV rbx
    ret

section .bss
array resb 8*16

; Sys_calls codes
SYS_SYSTEM_CODE equ 0
SYS_TICKS_CODE equ 1
SYS_RTC_CODE equ 2
SYS_READ_CODE equ 3
SYS_SCREEN_CODE equ 4
SYS_VIDEO_CODE equ 5
SYS_SOUND_CODE equ 6