#pragma once
#include <stdint.h>
#include "linked_list.h"

typedef void (*device_open)();
typedef void (*device_close)();

struct device_meta
{
    uint8_t name[32];
    uint32_t device_type;
    uint32_t owner;
    struct link node;
    void* assoc_dev;
};

typedef uintmax_t (*stream_dev_write)(
    struct device_meta* dev,
    uint8_t* ubuffer,
    uintmax_t offset, 
    uintmax_t size);

typedef uintmax_t (*stream_dev_read)(
    struct device_meta* dev,
    uint8_t* ubuffer,
    uintmax_t offset,
    uintmax_t size);

typedef void (*stream_dev_control)(struct device_meta* dev, uint64_t ctrl, void* arg);
typedef void (*stream_dev_flush)(struct device_meta* dev);

struct stream_device
{
    stream_dev_read read;
    stream_dev_write write;
    stream_dev_flush flush;
    stream_dev_control control;
    void* private_data;
};