#include "dev_object.h"
#include "memory/buddy.h"

object_manager_t dev_object_manager;

static struct buddy_head* dev_object_allocator;
static uint64_t dev_global_id = 0;
static linked_list_head(interfaces);

static inline int dev_object_compare_id(uintptr_t a, uintptr_t b)
{
    return (int)a - (int)b;
}

static inline uintptr_t dev_object_get_id(struct splay_tree_node* ptr)
{
    struct dev_object* objd = container_of(ptr, struct dev_object, node);
    return objd->id;
}

void init_dev_object_manager()
{
    dev_object_allocator = buddy_create();

    dev_object_manager.root = 0;
    dev_object_manager.get_key = dev_object_get_id;
    dev_object_manager.compare = dev_object_compare_id;
}

struct dev_object* dev_object_create()
{
    struct dev_object* item = buddy_alloc(dev_object_allocator, sizeof(struct dev_object));
    item->id = (++dev_global_id);
    item->int_count = 0;
    item->refs = 0;
    item->interfaces = linked_list_init(item->interfaces);

    splay_tree_insert(&dev_object_manager, &item->node);
    return item;
}

struct dev_object_interface* dev_interface_create(struct dev_object* obj, uint64_t size)
{
    struct dev_object_interface* interface = buddy_alloc(dev_object_allocator, size);
    interface->length = size;
    interface->owner = obj->id;
    interface->type = 0;

    linked_list_add_back(&interface->dev_link, &obj->interfaces);
    linked_list_add_back(&interface->global_link, &interfaces);

    obj->int_count++;

    return interface;
}
