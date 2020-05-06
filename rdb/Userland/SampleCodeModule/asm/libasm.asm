GLOBAL _sys_system
GLOBAL _sys_process
GLOBAL _sys_timet
GLOBAL _sys_rtc
GLOBAL _sys_fd
GLOBAL _sys_screen
GLOBAL _sys_video
GLOBAL _sys_sound

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
    mov r8, rcx   ; Arguments Shifting
    mov rcx, rdx
    mov rdx, rsi
    mov rsi, rdi
    mov rdi, SYS_SYSTEM_CODE
    int 80h
    ret   

_sys_process:
    mov r8, rcx   ; Arguments Shifting
    mov rcx, rdx
    mov rdx, rsi
    mov rsi, rdi
    mov rdi, SYS_PROCESS_CODE
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

_sys_fd:
    mov r9, r8    ; Arguments Shifting
    mov r8, rcx
    mov rcx, rdx
    mov rdx, rsi
    mov rsi, rdi
    mov rdi, SYS_FD_CODE
    int 80h
    ret

_sys_screen:
    mov rcx, rdx  ; Arguments Shifting
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
    mov rcx, rdx  ; Arguments Shifting 
    mov rdx, rsi
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

spin_lock:
    BuildSF
    preserve

    mov     al, 1          ; Set the EAX register to 1.

    xchg    al, [rdi]       ; Atomically swap the EAX register with
                            ;  the lock variable.
                            ; This will always store 1 to the lock, leaving
                            ;  the previous value in the EAX register.

    test    al, al          ; Test EAX with itself. Among other things, this will
                            ;  set the processor's Zero Flag if EAX is 0.
                            ; If EAX is 0, then the lock was unlocked and
                            ;  we just locked it.
                            ; Otherwise, EAX is 1 and we didn't acquire the lock.

    jnz     spin_lock       ; Jump back to the MOV instruction if the Zero Flag is
                            ;  not set; the lock was previously locked, and so
                            ; we need to spin until it becomes unlocked.

    recover
    breakSF
    ret                     ; The lock has been acquired, return to the calling
                            ;  function.

spin_unlock:
    BuildSF
    preserve    

    mov     al, 0          ; Set the EAX register to 0.

    xchg    al, [rdi]   ; Atomically swap the EAX register with
                            ;  the lock variable.

    recover
    breakSF
    ret                     ; The lock has been released.

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