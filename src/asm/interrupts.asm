global keyboard_ISR
global interrupt_ISR
global rtc_ISR

extern keyboard_handler
extern interrupt_handler
extern rtc_handler

%macro push_regs 0
    push rbx
    push rbp
    push rsi
    push rdi
    push r12
    push r13
    push r14
    push r15
    cld
%endmacro

%macro pop_regs 0
    pop r15
    pop r14
    pop r13
    pop r12
    pop rdi
    pop rsi
    pop rbp
    pop rbx
    iretq
%endmacro

keyboard_ISR:
    push_regs
    call keyboard_handler
    pop_regs

interrupt_ISR:
    push_regs
    call interrupt_handler
    pop_regs

rtc_ISR:
    push_regs
    call rtc_handler
    pop_regs