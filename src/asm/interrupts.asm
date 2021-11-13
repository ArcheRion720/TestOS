global keyboard_ISR
global interrupt_ISR
extern keyboard_handler
extern interrupt_handler

keyboard_ISR:
    push rbx
    push rbp
    push rsi
    push rdi
    push r12
    push r13
    push r14
    push r15
    cld
    call keyboard_handler
    pop r15
    pop r14
    pop r13
    pop r12
    pop rdi
    pop rsi
    pop rbp
    pop rbx
    iretq

interrupt_ISR:
    push rbx
    push rbp
    push rsi
    push rdi
    push r12
    push r13
    push r14
    push r15
    cld
    call interrupt_handler
    pop r15
    pop r14
    pop r13
    pop r12
    pop rdi
    pop rsi
    pop rbp
    pop rbx
    iretq
