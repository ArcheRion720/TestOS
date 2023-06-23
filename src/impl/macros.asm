%macro push_all 0
    push rax
	push rbx
	push rcx
	push rdx
	push rdi
	push rsi
	push rbp
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
	mov rax, cr3
	push rax
    mov rax, cr2
	push rax
	mov rax, ds
	push rax
	mov rax, es
	push rax
	mov rax, fs
	push rax
	mov rax, gs
	push rax
%endmacro

%macro pop_all 0
	pop rax
	mov gs, ax
	pop rax
	mov fs, ax
	pop rax
	mov es, ax
	pop rax
	mov ds, ax
	pop rax
    pop rax
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rbp
	pop rsi
	pop rdi
	pop rdx
	pop rcx
	pop rbx
	pop rax
%endmacro
