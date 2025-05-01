#pragma once

#include <stdint.h>
#include <stddef.h>

struct driver_ops
{
    int (*write)(const uint8_t *_data, size_t *_len);
    int (*read)(uint8_t *_data, size_t *_len);
    int (*lock)();
};

struct device;

int write(struct device* dev, const uint8_t *data, uint32_t len);