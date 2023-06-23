#pragma once

struct link
{
    struct link *prev, *next;
};
typedef struct link linked_list_t;

#define LINKED_LIST(head) (linked_list_t){ &(head), &(head) }
#define for_each_link(it, head) \
    for(it = (head)->next; it != (head); it = it->next)

void add_link_forward(struct link* new, struct link* item);
void add_link_back(struct link* new, struct link* item);
void rem_link(struct link* item);