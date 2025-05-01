#include "spi.h"
#include <string.h>

#include <esp_log.h>
#include <driver/spi_common.h>
#include <driver/spi_master.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "driver/spi";

#define SPI_MAX_INSTANCE (2)
#define SPI_MAX_DEVICE_PER_BUS (2)

struct spi_device_data;
struct spi_instance_data;

struct spi_device_data
{
    const struct spi_device_cfg *cfg;
    struct spi_instance_data *owner;

    spi_device_handle_t handle;
    spi_device_interface_config_t devcfg;
};

struct spi_instance_data
{
    const struct spi_config *cfg;
    struct spi_device_data list_dev[SPI_MAX_DEVICE_PER_BUS];

    spi_host_device_t hdev;
    spi_bus_config_t buscfg;
};

static struct spi_instance_data _spi_instance[SPI_MAX_INSTANCE];

static struct spi_instance_data *_spi_get_instance_data(spi_instance_t in)
{
    return (struct spi_instance_data *)in;
}

static struct spi_device_data *_spi_get_device_data(spi_device_t dv)
{
    return (struct spi_device_data *)dv;
}

static void _spi_pre_cb(spi_transaction_t *t)
{
}

static struct spi_instance_data *_spi_validate_and_alloc_instance(const struct spi_config *cfg)
{
    struct spi_instance_data *idata = NULL;

    if (NULL == cfg ||
        -1 == cfg->miso ||
        -1 == cfg->mosi ||
        -1 == cfg->sck)
    {
        goto exit;
    }

    // only support host 2 for now
    if (cfg->bus_num > SPI_HOST_MAX || cfg->bus_num < 0)
    {
        return NULL;
    }

    for (int i = 0; i < SPI_MAX_INSTANCE; ++i)
    {
        if (_spi_instance[i].cfg == NULL)
        {
            idata = &_spi_instance[i];
            idata->cfg = cfg;
            idata->hdev = (spi_host_device_t)cfg->bus_num; // only support HOST2 for now
            break;
        }
    }

exit:
    ESP_LOGD(TAG, "validate_and_alloc_instance: bus%d %08x %08x", cfg->bus_num, (uint32_t)cfg, (uint32_t)idata);
    return idata;
}

static void _spi_release_instance(struct spi_instance_data *idata)
{
    idata->cfg = NULL;
}

static struct spi_device_data *_spi_validate_and_alloc_device(struct spi_instance_data *idata, const struct spi_device_cfg *cfg)
{
    struct spi_device_data *ddata = NULL;

    if (NULL == cfg ||
        NULL == idata ||
        0 >= cfg->freq)
    {
        goto exit;
    }

    for (int i = 0; i < SPI_MAX_INSTANCE; ++i)
    {
        if (idata->list_dev[i].cfg == NULL)
        {
            ddata = &idata->list_dev[i];
            ddata->owner = idata;
            ddata->cfg = cfg;
            break;
        }
    }

exit:
    ESP_LOGD(TAG, "validate_and_alloc_instance: %08x %08x", (uint32_t)idata, (uint32_t)cfg, (uint32_t)ddata);
    return ddata;
}

static void _spi_release_device(struct spi_device_data *ddata)
{
    ddata->cfg = NULL;
}

spi_instance_t spi_init(const struct spi_config *cfg)
{
    int ret;
    struct spi_instance_data *idata;

    idata = _spi_validate_and_alloc_instance(cfg);
    if (!idata)
    {
        // invalid config or instance full
        return NULL;
    }

    memset(&idata->buscfg, 0, sizeof(idata->buscfg));
    idata->buscfg.mosi_io_num = cfg->mosi;
    idata->buscfg.miso_io_num = cfg->miso;
    idata->buscfg.sclk_io_num = cfg->sck;
    idata->buscfg.quadwp_io_num = -1;
    idata->buscfg.quadhd_io_num = -1;
    idata->buscfg.max_transfer_sz = 2048;

    ret = spi_bus_initialize(idata->hdev, &idata->buscfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK)
    {
        ESP_LOGD(TAG, "init: spi_bus_initialize failed: %d", ret);
        goto cleanup;
    }

    ESP_LOGD(TAG, "init: bus%d initialized miso: %2d mosi: %2d sck: %2d", cfg->bus_num, cfg->miso, cfg->mosi, cfg->sck);
    goto exit;

cleanup:
    _spi_release_instance(idata);
    idata = NULL;
exit:
    return idata;
}

int spi_deinit(spi_instance_t in)
{
    ESP_LOGD(TAG, "deinit: %08x", (uint32_t)in);

    int ret = EXIT_SUCCESS;
    struct spi_instance_data *idata = _spi_get_instance_data(in);
    if (NULL == idata)
    {
        return -EXIT_FAILURE;
    }
    // TODO

    return ret;
}

spi_device_t spi_device_add(spi_instance_t in, const struct spi_device_cfg *devcfg)
{
    int ret;
    struct spi_device_data *ddata;
    struct spi_instance_data *idata;

    idata = _spi_get_instance_data(in);
    if (NULL == idata)
        return NULL;

    ddata = _spi_validate_and_alloc_device(idata, devcfg);
    if (NULL == ddata)
        return NULL;

    memset(&ddata->devcfg, 0, sizeof(ddata->devcfg));
    ddata->devcfg.clock_speed_hz = devcfg->freq;
    ddata->devcfg.mode = devcfg->mode;
    ddata->devcfg.spics_io_num = devcfg->cs;
    ddata->devcfg.queue_size = 7;
    ddata->devcfg.pre_cb = _spi_pre_cb;

    ret = spi_bus_add_device(idata->hdev, &ddata->devcfg, &ddata->handle);
    if (ESP_OK != ret)
    {
        ESP_LOGD(TAG, "device_add: spi_bus_add_device failed: %d", ret);
        goto cleanup;
    }
    
    ESP_LOGD(TAG, "device_add: added device to bus%d cs: %2d freq: %2d mode: %2d", idata->hdev, devcfg->cs, devcfg->freq, devcfg->mode);
    goto exit;

cleanup:
    _spi_release_device(ddata);
    ddata = NULL;
exit:
    return ddata;
}

int spi_device_remove(spi_device_t dv)
{

    return -EXIT_FAILURE;
}

int spi_transmit(spi_device_t dv, const uint8_t *data, uint32_t len)
{
    int ret;
    spi_transaction_t ts;
    struct spi_device_data *ddata = _spi_get_device_data(dv);
    if (NULL == ddata)
    {
        return -EXIT_FAILURE;
    }

    memset(&ts, 0, sizeof(ts));
    ts.length = len * 8;

    if (len < 4)
    {
        memcpy(&ts.tx_data, data, len);
        ts.flags = SPI_TRANS_USE_TXDATA;
    }
    else
    {
        ts.tx_buffer = data;
    }

    ESP_LOGD(TAG, "transmit %u bytes", len);
    ret = spi_device_polling_transmit(ddata->handle, &ts);
    if (ESP_OK != ret)
    {
        return -EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int spi_receive(spi_device_t dv, uint8_t *data, uint32_t len)
{
    int ret;
    spi_transaction_t ts;
    struct spi_device_data *ddata = _spi_get_device_data(dv);
    if (NULL == ddata)
    {
        return -EXIT_FAILURE;
    }

    memset(&ts, 0, sizeof(ts));
    ts.rxlength = len * 8;
    if (len < 4)
    {
        memcpy(&ts.rx_data, data, len);
        ts.flags = SPI_TRANS_USE_RXDATA;
    }
    else
    {
        ts.rx_buffer = data;
    }

    ESP_LOGD(TAG, "receive %u bytes", len);
    ret = spi_device_polling_transmit(ddata->handle, &ts);
    if (ESP_OK != ret)
    {
        return -EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int spi_transfer(spi_device_t dv, const uint8_t *txdata, uint8_t *rxdata, uint32_t len)
{
    int ret;
    spi_transaction_t ts;
    struct spi_device_data *ddata = _spi_get_device_data(dv);
    if (NULL == ddata)
    {
        return -EXIT_FAILURE;
    }

    memset(&ts, 0, sizeof(ts));
    ts.rx_buffer = rxdata;
    ts.tx_buffer = txdata;
    ts.length = len * 8;
    ts.rxlength = len * 8;
    if (len < 4)
        ts.flags = SPI_TRANS_USE_RXDATA | SPI_TRANS_USE_TXDATA;

    ESP_LOGD(TAG, "transfer %u bytes", len);
    ret = spi_device_polling_transmit(ddata->handle, &ts);
    if (ESP_OK != ret)
    {
        return -EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int spi_transmit_async(spi_device_t dv, const uint8_t *data, uint32_t len)
{
    int ret;
    spi_transaction_t ts;
    struct spi_device_data *ddata = _spi_get_device_data(dv);
    if (NULL == ddata)
    {
        return -EXIT_FAILURE;
    }

    memset(&ts, 0, sizeof(ts));

    return EXIT_SUCCESS;
}

int spi_receive_async(spi_device_t dv, uint8_t *data, uint32_t len)
{
    int ret;
    spi_transaction_t ts;
    struct spi_device_data *ddata = _spi_get_device_data(dv);
    if (NULL == ddata)
    {
        return -EXIT_FAILURE;
    }

    memset(&ts, 0, sizeof(ts));

    return EXIT_SUCCESS;
}

int spi_transfer_async(spi_device_t dv, const uint8_t *txdata, uint8_t *rxdata, uint32_t len)
{
    int ret;
    spi_transaction_t ts;
    struct spi_device_data *ddata = _spi_get_device_data(dv);
    if (NULL == ddata)
    {
        return -EXIT_FAILURE;
    }

    memset(&ts, 0, sizeof(ts));

    return EXIT_SUCCESS;
}
