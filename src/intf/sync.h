#pragma once
#include <stdatomic.h>
#include <stdint.h>

#define PRINT_LOCK &ticket_lock_common[0]
#define FRAMEBUFFER_LOCK &ticket_lock_common[1]
#define MEMORY_ALLOC_LOCK &ticket_lock_common[2]

struct ticket_lock
{
    atomic_uint_fast32_t next;
    atomic_uint_fast32_t serving;
};
typedef struct ticket_lock ticket_lock_t;

void ticket_lock_init(ticket_lock_t* lock);
void ticket_lock_acquire(ticket_lock_t* lock);
void ticket_lock_release(ticket_lock_t* lock);

extern ticket_lock_t ticket_lock_common[3];