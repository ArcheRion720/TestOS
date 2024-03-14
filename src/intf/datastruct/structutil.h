#pragma once 

#define offset_of(type, member) ((uintptr_t) &((type*)0)->member)
#define container_of(ptr, type, member) ({               \
    const typeof(((type*)0)->member) *__ptr = (ptr);     \
    (type*)((uintptr_t)__ptr - offset_of(type, member)); \
})