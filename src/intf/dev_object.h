#pragma once
#include <stdint.h>
#include "datastruct/linked_list.h"
#include "datastruct/splay.h"

#define DEV_INTERFACE_IDENTIFY_PCI 0xFFAAFFAA

struct dev_object
{
    uint64_t id;
    uint32_t refs;
    uint32_t int_count;
    struct splay_tree_node node;
    struct link interfaces;
};

struct dev_object_interface
{
    uint64_t type;
    uint64_t owner;
    uint64_t length;
    struct link dev_link;
    struct link global_link;
    uint8_t spec_data[];
};

typedef struct splay_tree object_manager_t;
extern object_manager_t dev_object_manager; 

void init_dev_object_manager();
struct dev_object* dev_object_create();
struct dev_object_interface* dev_interface_create(struct dev_object* obj, uint64_t size);
