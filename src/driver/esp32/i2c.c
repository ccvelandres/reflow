#include "i2c.h"
#include <stdint.h>
#include <string.h>

#include <errno.h>
#include <esp_log.h>
#include <driver/i2c.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "driver/i2c";

#define I2C_MAX_INSTANCE (2)
#define I2C_MAX_DEVICE_PER_BUS (4)
#define I2C_MASTER_TIMEOUT (200 / portTICK_PERIOD_MS)
#define I2C_MAX_TRANSACTION (4)
#define I2C_MAX_CLOCK (400000)
struct i2c_instance_data;

struct i2c_device_data
{
    const struct i2c_device_cfg *cfg;
    struct i2c_instance_data *owner;

    int addr;
    uint8_t cmd_handle_buffer[I2C_LINK_RECOMMENDED_SIZE(I2C_MAX_TRANSACTION)];
    i2c_cmd_handle_t cmd_handle;
};

struct i2c_instance_data
{
    const struct i2c_config *cfg;
    struct i2c_device_data list_dev[I2C_MAX_DEVICE_PER_BUS];

    i2c_port_t port_num;
    i2c_config_t conf;

    int clock_hz;
    size_t rx_bufsize;
    size_t tx_bufsize;
};
struct i2c_instance_data _i2c_instance[I2C_MAX_INSTANCE];

static struct i2c_instance_data *_i2c_get_instance_data(i2c_instance_t in)
{
    return (struct i2c_instance_data *)in;
}

static struct i2c_device_data *_i2c_get_device_data(i2c_device_t *dv)
{
    return (struct i2c_device_data *)dv;
}

static struct i2c_instance_data *_i2c_validate_and_alloc_instance(const struct i2c_config *cfg)
{
    struct i2c_instance_data *idata = NULL;

    if (NULL == cfg ||
        0 > cfg->scl ||
        0 > cfg->sda ||
        2 >= cfg->bus_num)
    {
        goto exit;
    }

    for (int i = 0; i < I2C_MAX_INSTANCE; ++i)
    {
        if (NULL == _i2c_instance[i].cfg)
        {
            idata = &_i2c_instance[i];
            idata->cfg = cfg;
            idata->port_num = (i2c_port_t)cfg->bus_num;
            break;
        }
    }

exit:
    ESP_LOGD(TAG, "validate_and_alloc_instance: bus%d %08x %08x", cfg->bus_num, (uint32_t)cfg, (uint32_t)idata);
    return idata;
}

void _i2c_release_instance(struct i2c_instance_data *idata)
{
    idata->cfg = NULL;
}

static struct i2c_device_data *_i2c_validate_and_alloc_device(struct i2c_instance_data *idata, const struct i2c_device_cfg *cfg)
{
    static struct i2c_device_data *ddata = NULL;

    if (NULL == idata || NULL == cfg)
    {
        goto exit;
    }

    // check clock
    if (cfg->max_freq > 0 && idata->clock_hz > cfg->max_freq)
    {
        ESP_LOGE(TAG, "validate_and_alloc_device: device does not support configured clock speed %d", idata->clock_hz);
        goto exit;
    }

    for (int i = 0; i < I2C_MAX_DEVICE_PER_BUS; ++i)
    {
        if (NULL == idata->list_dev[i].cfg)
        {
            ddata = &idata->list_dev[i];
            ddata->owner = idata;
            ddata->cfg = cfg;
            break;
        }
    }

exit:
    ESP_LOGD(TAG, "validate_and_alloc_device: %08x %08x", (uint32_t)idata, (uint32_t)cfg, (uint32_t)ddata);
    return ddata;
}

static void _i2c_release_device(struct i2c_device_data *ddata)
{
    ddata->cfg = NULL;
}

i2c_instance_t i2c_init(struct i2c_config *cfg)
{
    int ret;
    struct i2c_instance_data *idata;

    idata = _i2c_validate_and_alloc_instance(cfg);
    if (!idata)
    {
        // invalid config or instance full
        return NULL;
    }

    memset(&idata->conf, 0, sizeof(idata->conf));
    idata->conf.scl_io_num = cfg->scl;
    idata->conf.sda_io_num = cfg->sda;
    idata->conf.scl_pullup_en = cfg->flags & I2C_FLAGS_SCL_PULLUP ? 1 : 0;
    idata->conf.sda_pullup_en = cfg->flags & I2C_FLAGS_SDA_PULLUP ? 1 : 0;

    if (cfg->flags & I2C_FLAGS_SLAVE)
    {
        idata->conf.mode = I2C_MODE_SLAVE;
        // TODO
        ESP_LOGE(TAG, "init: slave mode is currently unsupported");
        goto cleanup;
    }
    else
    {
        idata->conf.mode = I2C_MODE_MASTER;
        idata->conf.master.clk_speed = I2C_MAX_CLOCK;

        idata->rx_bufsize = 0;
        idata->tx_bufsize = 0;
    }

    ret = i2c_param_config(idata->port_num, &idata->conf);
    if (ESP_OK != ret)
    {
        ESP_LOGD(TAG, "init: i2c_param_config failed: %d", ret);
        goto cleanup;
    }

    ret = i2c_driver_install(idata->port_num, idata->conf.mode, idata->rx_bufsize, idata->tx_bufsize, 0);
    if (ESP_OK != ret)
    {
        ESP_LOGD(TAG, "init: i2c_driver_install failed: %d", ret);
        goto cleanup;
    }

    ESP_LOGD(TAG, "init: bus%d initialized scl: %2d sda: %2d", cfg->bus_num, cfg->scl, cfg->sda);
    goto exit;

cleanup:
    _i2c_release_instance(idata);
    idata = NULL;
exit:
    return idata;
}

int i2c_deinit(i2c_instance_t in)
{
    ESP_LOGD(TAG, "deinit: %08x", (uint32_t)in);

    int ret = EXIT_SUCCESS;
    struct i2c_instance_data *idata = _i2c_get_instance_data(in);
    if (NULL == idata)
    {
        return -EXIT_FAILURE;
    }
    // TODO

    return ret;
}

i2c_device_t i2c_device_add(i2c_instance_t in, const struct i2c_device_cfg *devcfg)
{
    int ret;
    struct i2c_device_data *ddata;
    struct i2c_instance_data *idata;

    idata = _i2c_get_instance_data(in);
    if (NULL == idata)
        return NULL;

    ddata = _i2c_validate_and_alloc_device(idata, devcfg);
    if (NULL == idata)
        return NULL;

    ESP_LOGD(TAG, "device_add: added device to bus%d", idata->port_num);
    return idata;
}

int i2c_device_remove(i2c_device_t dv)
{
    ESP_LOGD(TAG, "device_remove: %08x", (uint32_t)dv);

    int ret = EXIT_SUCCESS;
    struct i2c_device_data *ddata = _i2c_get_device_data(dv);
    if (NULL == ddata)
    {
        return -EXIT_FAILURE;
    }
    // TODO

    return ret;
}

int i2c_start(i2c_device_t dv)
{
    int ret;
    struct i2c_device_data *ddata = _i2c_get_device_data(dv);
    if (NULL == ddata)
        return -EXIT_FAILURE;

    if (NULL == ddata->cmd_handle)
        return -EBUSY;

    ddata->cmd_handle = i2c_cmd_link_create_static(ddata->cmd_handle_buffer, sizeof(ddata->cmd_handle_buffer));
    if (NULL == ddata->cmd_handle)
        return -ENOMEM;

    return EXIT_SUCCESS;
}

int i2c_end(i2c_device_t dv)
{
    int ret;
    struct i2c_device_data *ddata = _i2c_get_device_data(dv);
    if (NULL == ddata)
        return -EXIT_FAILURE;

    if (NULL != ddata->cmd_handle)
        return -EINVAL;

    ret = i2c_master_cmd_begin(ddata->owner->port_num, ddata->cmd_handle, portMAX_DELAY);

    i2c_cmd_link_delete_static(ddata->cmd_handle);
    ddata->cmd_handle = NULL;
    return ret;
}

int i2c_read(i2c_device_t dv, uint8_t *data, uint32_t len)
{
    int ret = EXIT_SUCCESS;
    struct i2c_device_data *ddata = _i2c_get_device_data(dv);
    if (NULL == ddata)
        return -EXIT_FAILURE;

    if (NULL == ddata->cmd_handle)
    {
        ESP_LOGD(TAG, "i2c_read: device not ready! call i2c_start first", ddata->owner->port_num);
        goto exit;
    }

    if (ddata->owner->conf.mode != I2C_MODE_MASTER)
    {
        ESP_LOGD(TAG, "i2c_read: operation only allowed on master mode");
        ret = -EXIT_FAILURE;
        goto exit;
    }

    ret = i2c_master_read(ddata->cmd_handle, data, len, I2C_MASTER_LAST_NACK);
    if (ESP_OK != ret)
    {
        ret = -EXIT_FAILURE;
    }
exit:
    return ret;
}

int i2c_write(i2c_device_t dv, const uint8_t *data, uint32_t len)
{
    int ret = EXIT_SUCCESS;
    struct i2c_device_data *ddata = _i2c_get_device_data(dv);
    if (NULL == ddata)
        return -EXIT_FAILURE;

    if (NULL == ddata->cmd_handle)
    {
        ESP_LOGD(TAG, "i2c_write: device not ready! call i2c_start first", ddata->owner->port_num);
        goto exit;
    }

    if (ddata->owner->conf.mode != I2C_MODE_MASTER)
    {
        ESP_LOGD(TAG, "i2c_write: operation only allowed on master mode");
        ret = -EXIT_FAILURE;
        goto exit;
    }

    ret = i2c_master_write(ddata->cmd_handle, data, len, true);
    // ret = i2c_master_write_to_device(ddata->owner->port_num, ddata->addr, data, len, I2C_MASTER_TIMEOUT);
    if (ESP_OK != ret)
    {
        ret = -EXIT_FAILURE;
    }
exit:
    return ret;
}
