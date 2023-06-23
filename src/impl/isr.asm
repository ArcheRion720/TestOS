%include "src/impl/macros.asm"

isr_common:
    ; push all registers onto stack
    push_all

    ; set segment registers 
    mov ax, 0x10
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    cld

	cli
    extern isr_handler
    call isr_handler
	sti

    pop_all
    ; discard interrupt number and error code pushed by stub
    add rsp, 16

    iretq

%macro isr 2
global irq%1
irq%1:
    push byte 0
    push byte %2
    jmp isr_common
%endmacro

isr 0, 32
isr 1, 33
isr 2, 34
isr 3, 35 
isr 4, 36
isr 5, 37
isr 6, 38
isr 7, 39
isr 8, 40
isr 9, 41
isr 10, 42
isr 11, 43
isr 12, 44
isr 13, 45
isr 14, 46
isr 15, 47

%macro isr_err_code 1
global isr%1
isr%1:
    push byte %1
    jmp isr_common
%endmacro

%macro isr_no_error 1
global isr%1
isr%1:
    push byte 0
    push byte %1
    jmp isr_common
%endmacro

isr_no_error 0
isr_no_error 1
isr_no_error 2
isr_no_error 3
isr_no_error 4
isr_no_error 5
isr_no_error 6
isr_no_error 7
isr_err_code 8
isr_no_error 9
isr_err_code 10
isr_err_code 11
isr_err_code 12
isr_err_code 13
isr_err_code 14
isr_no_error 15
isr_no_error 16
isr_no_error 17
isr_no_error 18
isr_no_error 19
isr_no_error 20
isr_no_error 21
isr_no_error 22
isr_no_error 23
isr_no_error 24 
isr_no_error 25
isr_no_error 26
isr_no_error 27
isr_no_error 28
isr_no_error 29
isr_no_error 30
isr_no_error 31