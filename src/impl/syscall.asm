%include "src/impl/macros.asm"

global syscall_stub
syscall_stub:
    cli
    push rcx
    push rdx

    extern syscall_handler
    call syscall_handler

    pop rdx
    pop rcx
    sti
    o64 sysexit