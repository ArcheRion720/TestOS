#include "memory/pmm.h"
#include "memory/vmm.h"
#include "linked_list.h"
#include "devmgr.h"
#include "string.h"

static linked_list_head(devices);

static pool_allocator_t* device_meta_allocator;
static pool_allocator_t* stream_devices_allocator;

void init_devices()
{
    device_meta_allocator = pool_allocator_acquire(sizeof(struct device_meta), PMM_PAGE_SIZE);
    stream_devices_allocator = pool_allocator_acquire(sizeof(struct stream_device), PMM_PAGE_SIZE);
}

struct device_meta* stream_device_register(const uint8_t* name)
{
    uint32_t name_len = strlen(name);
    if(name_len > 32 || name_len == 0)
        return 0;

    struct device_meta* meta = pool_fetch(device_meta_allocator);
    if(!meta)
        return 0;

    meta = HH_ADDR(meta);

    memcpy(name, meta->name, name_len);

    struct stream_device* dev = pool_fetch(stream_devices_allocator);
    if(!dev)
    {
        pool_drop(device_meta_allocator, meta);
        return 0;
    }

    dev = HH_ADDR(dev);

    meta->device_type = DEV_STREAM;
    meta->assoc_dev = dev;
    linked_list_add_forward(&meta->node, &devices);

    return meta;
}

void stream_device_dispose(struct device_meta* dev)
{
    switch(dev->device_type)
    {
        case DEV_STREAM:
            linked_list_remove(&dev->node);
            pool_drop(stream_devices_allocator, dev->assoc_dev);
            break;
        case DEV_BLOCK:
            __builtin_trap();
            break;
        case DEV_OTHER:
            __builtin_trap();
            break;
    }
}

struct device_meta* query_device(const uint8_t* name)
{
    struct device_meta* dev;
    string_t search;
    search.data = name;
    search.length = strlen(name);

    string_t checked;
    linked_list_foreach(dev, &devices, node)
    {
        checked.data = dev->name;
        checked.length = strlen(dev->name);
        if(str_cmp(search, checked))
        {
            return dev;
        }
    }

    return 0;
}