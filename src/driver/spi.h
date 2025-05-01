#pragma once

#ifdef cplusplus
extern "C" {
#endif

#include <stdint.h>

struct spi_config
{
    char* name;
    int bus_num;
    int miso;
    int mosi;
    int sck;
    int dma;
};

struct spi_device_cfg
{
    int cs;
    int freq;
    int mode;
};

typedef void* spi_instance_t;
typedef void* spi_device_t;

spi_instance_t spi_init(const struct spi_config* cfg);
int spi_deinit(spi_instance_t in);
spi_device_t spi_device_add(spi_instance_t in, const struct spi_device_cfg *devcfg);
int spi_device_remove(spi_device_t dv);

int spi_acquire(spi_device_t dv);
int spi_release(spi_device_t dv);

int spi_transmit(spi_device_t dv, const uint8_t *data, uint32_t len);
int spi_receive(spi_device_t dv, uint8_t *data, uint32_t len);
int spi_transfer(spi_device_t dv, const uint8_t *txdata, uint8_t *rxdata, uint32_t len);

int spi_transmit_async(spi_device_t dv, const uint8_t *data, uint32_t len);
int spi_receive_async(spi_device_t dv, uint8_t *data, uint32_t len);
int spi_transfer_async(spi_device_t dv, const uint8_t *txdata, uint8_t *rxdata, uint32_t len);

#ifdef cplusplus
}
#endif