#pragma once

#define offset_of(type, member) ((uintptr_t) &((type*)0)->member)
#define container_of(ptr, type, member) ({               \
    const typeof(((type*)0)->member) *__ptr = (ptr);     \
    (type*)((uintptr_t)__ptr - offset_of(type, member)); \
})

#define linked_list_init(item) { &(item), &(item) }
#define linked_list_head(item) struct link item = linked_list_init(item)

#define linked_list_foreach(iterator, head, member) \
    for(iterator = container_of((head)->next, typeof(*iterator), member); !(&iterator->member == (head)); iterator = container_of((iterator)->member.next, typeof(*(iterator)), member))

#define linked_list_foreach_link(iterator, head) \
    for(iterator = (head)->next; iterator != (head); iterator = iterator->next)

struct link
{
    struct link *prev, *next;
};

static inline void __linked_list_add(struct link* new, struct link* prev, struct link* next)
{
    next->prev = new;
    new->next = next;
    new->prev = prev;
    prev->next = new;
}

static inline void __linked_list_remove(struct link* prev, struct link* next)
{
    next->prev = prev;
    prev->next = next;
}

static inline void linked_list_add_forward(struct link* new, struct link* head)
{
    __linked_list_add(new, head, head->next);
}

static inline void linked_list_add_back(struct link* new, struct link* head)
{
    __linked_list_add(new, head->prev, head);
}

static inline void linked_list_remove(struct link* item)
{
    __linked_list_remove(item->prev, item->next);
}