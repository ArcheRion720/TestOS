#pragma once
#include <stdint.h>

extern uint32_t cpu_cpuid_capabilities[9];
extern uint32_t cpu_version;

/*
    PAGE    CPUID_Path 
    0       CPUID.01.EDX
    1       CPUID.01.ECX
    2       CPUID.07.00.EBX
    3       CPUID.07.00.ECX
    4       CPUID.07.00.EDX
    5       CPUID.07.01.EAX
    6       CPUID.07.01.EBX
    7       CPUID.07.01.EDX
    8       CPUID.07.02.EDX
*/
#define __CPU_CAPABILITY_PRESENT(PAGE, FEATURE) (cpu_cpuid_capabilities[(PAGE)] & (FEATURE))

#define CPU_CAPABILITY_APIC_PRESENT     __CPU_CAPABILITY_PRESENT(0, 1 << 9)
#define CPU_CAPABILITY_SEP_PRESENT      __CPU_CAPABILITY_PRESENT(0, 1 << 11)
#define CPU_CAPABILITY_PCID_PRESENT     __CPU_CAPABILITY_PRESENT(1, 1 << 17)
#define CPU_CAPABILITY_NX_PRESENT       __CPU_CAPABILITY_PRESENT(0, 1 << 20)
#define CPU_CAPABILITY_RDRAND_PRESENT   __CPU_CAPABILITY_PRESENT(1, 1 << 30)
#define CPU_CAPABILITY_INVPCID_PRESENT  __CPU_CAPABILITY_PRESENT(2, 1 << 10)

#define CPU_FAMILY_ID   ((cpu_version >> 8) & 0xF)
#define CPU_STEPPING    (cpu_version & 0xF)
#define CPU_MODEL       ((CPU_FAMILY_ID == 6 || CPU_FAMILY_ID == 15) ? \
                            ((cpu_version >> 12) & 0xF0) | ((cpu_version >> 4) & 0xF) : \
                            ((cpu_version >> 4) & 0xF))

#define CPU_FAMILY      ((CPU_FAMILY_ID == 15) ? \
                            (CPU_FAMILY_ID + (cpu_version >> 20 & 0xFF)) : \
                            (CPU_FAMILY_ID))
                            
#define CPU_TYPE        ((cpu_version >> 12) & 0x3)

void read_cpu_features();