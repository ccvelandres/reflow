#pragma once

#include <stdint.h>

enum i2c_flags
{
    I2C_FLAGS_SLAVE,
    I2C_FLAGS_SDA_PULLUP,
    I2C_FLAGS_SCL_PULLUP
};

struct i2c_config
{
    char *name;
    int bus_num;
    enum i2c_flags flags;
    int scl;
    int sda;
};

struct i2c_device_cfg
{
    int max_freq;
};

typedef void *i2c_instance_t;
typedef void *i2c_device_t;

i2c_instance_t i2c_init(struct i2c_config *cfg);
int i2c_deinit(i2c_instance_t in);
i2c_device_t i2c_device_add(i2c_instance_t in, const struct i2c_device_cfg *devcfg);
int i2c_device_remove(i2c_device_t dv);

int i2c_start(i2c_device_t dv);
int i2c_read(i2c_device_t dv, uint8_t *data, uint32_t len);
int i2c_write(i2c_device_t dv, const uint8_t* data, uint32_t len);
int i2c_end(i2c_device_t dv);
