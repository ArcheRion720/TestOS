#include "sync.h"

ticket_lock_t ticket_lock_common[3];

void ticket_lock_init(ticket_lock_t* lock)
{
    atomic_init(&lock->next, 0);
    atomic_init(&lock->serving, 0);
}

void ticket_lock_acquire(ticket_lock_t* lock)
{
    uint32_t ticket = atomic_fetch_add(&lock->next, 1);
    while (lock->serving != ticket) 
    {
        asm volatile ("pause");
    }
}

void ticket_lock_release(ticket_lock_t* lock)
{
    atomic_fetch_add(&lock->serving, 1);
}