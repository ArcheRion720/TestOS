#include <stdint.h>
#include "capabilities.h"
#include "gdt.h"
#include "syscall.h"
#include "msr.h"
#include "print.h"

static uint8_t syscall_stack[8192];
extern void syscall_stub();

void init_syscall()
{
    if(!CPU_CAPABILITY_SEP_PRESENT)
        __builtin_trap();

    if((CPU_FAMILY == 6) && (CPU_MODEL < 3) && (CPU_STEPPING < 3))
        __builtin_trap();

    uint64_t cs = GDT_SEGMENT_OFFSET(GDT_CS0_64);
    uintptr_t sp = (uintptr_t)&syscall_stack + sizeof(syscall_stack);
    uintptr_t ip = (uintptr_t)&syscall_stub;

    uint64_t efer;
    rdmsr(MSR_IA32_EFER, efer);
    efer |= IA32_EFER_SCE;
    wrmsr(MSR_IA32_EFER, efer);

    wrmsr(MSR_SYSENTER_CS, cs);
    wrmsr(MSR_SYSENTER_SP, sp);
    wrmsr(MSR_SYSENTER_IP, ip);
}

void syscall_handler(/*uint64_t sp3, uint64_t ip3*/)
{
    print_fmt("System call from user mode!\n");
    __asm__ volatile ("hlt");
}