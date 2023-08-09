#include "stddevice.h"
#include <stdint.h>

#define DEV_STREAM  0x1
#define DEV_BLOCK   0x2
#define DEV_OTHER   0x3
#define DEV_PCI     0x4

void init_devices();
struct device_meta* query_device(const uint8_t* name);
struct device_meta* device_register(const uint8_t* name);
void stream_device_dispose(struct device_meta* dev);