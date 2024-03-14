#include "x86/capabilities.h"
#include "print.h"

#include <stdint.h>
#include <cpuid.h>

uint32_t cpu_cpuid_capabilities[11];
uint32_t cpu_version;

void read_cpu_features()
{
    uint32_t eax, ebx, ecx, edx;
    if(__get_cpuid(1, &eax, &ebx, &ecx, &edx))
    {
        cpu_version = eax;

        cpu_cpuid_capabilities[0] = edx;
        cpu_cpuid_capabilities[1] = ecx;
    }

    if(__get_cpuid_count(7, 0, &eax, &ebx, &ecx, &edx))
    {
        cpu_cpuid_capabilities[2] = ebx;
        cpu_cpuid_capabilities[3] = ecx;
        cpu_cpuid_capabilities[4] = edx;
    }
    
    if(__get_cpuid_count(7, 1, &eax, &ebx, &ecx, &edx))
    {
        cpu_cpuid_capabilities[5] = eax;
        cpu_cpuid_capabilities[6] = ebx;
        cpu_cpuid_capabilities[7] = edx;
    }

    if(__get_cpuid_count(7, 2, &eax, &ebx, &ecx, &edx))
    {
        cpu_cpuid_capabilities[8] = edx;
    }

    if(__get_cpuid(0x80000001, &eax, &ebx, &ecx, &edx))
    {
        cpu_cpuid_capabilities[9] = ecx;
        cpu_cpuid_capabilities[10] = edx;
    }
}