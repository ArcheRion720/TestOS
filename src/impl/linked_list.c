#include "linked_list.h"

// void add_link_forward(struct link* new, struct link* item)
// {
//     item->next->prev = new;
//     new->next = item->next;
//     new->prev = item;
//     item->next = new;
// }

// void add_link_back(struct link* new, struct link* item)
// {
//     item->prev->next = new;
//     new->next = item;
//     new->prev = item->prev;
//     item->prev = new;
// }

// void rem_link(struct link* item)
// {
//     item->next->prev = item->prev;
//     item->prev->next = item->next;
// }