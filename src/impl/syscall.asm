%include "src/impl/macros.asm"

global syscall_stub
syscall_stub:
    cli
    ; push rcx
    ; push rdx

    extern syscall_handler
    call syscall_handler

    ; pop rdx
    ; pop rcx
    sti
    o64 sysexit

global user_jmp
user_jmp:
    mov ax, 0x38 | 3
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push 0x38 | 3
    push rdi ;rsp
    push 0x202
    push 0x30 | 3
    push rsi ;rip
    iretq