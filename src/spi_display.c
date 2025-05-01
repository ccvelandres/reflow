#include "spi.h"
#include "gpio.h"
#include "board.h"
#include "yuck/ili9341/ili9341.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void ili9341_cs(bool state, void *user_ctx);
void ili9341_dc(bool state, void *user_ctx);
void ili9341_rst(bool state, void *user_ctx);
int ili9341_read(uint8_t *data, size_t len, void *user_ctx);
int ili9341_write(const uint8_t *data, size_t len, void *user_ctx);
int ili9341_write16(const uint16_t *data, size_t len, void *user_ctx);
int ili9341_xfer(const uint8_t *write, uint8_t *read, size_t len, void *user_ctx);

// static uint8_t ili9341_cache[256];
static ili9341_scb_t ili9341_scb = {
    .height = 320,
    .width = 240,
    .f_delay = vTaskDelay,
    .f_dc = ili9341_dc,
    .f_rst = ili9341_rst,
    .f_cs = ili9341_cs,
    .f_read = ili9341_read,
    .f_write = ili9341_write,
    .f_write16 = ili9341_write16,
    .f_xfer = ili9341_xfer};

static spi_instance_t ili9341_spi;
static struct spi_config ili9341_spi_cfg = {
    .name = "spi2",
    .bus_num = SPI_BUS_NUM,
    .miso = PIN_SPI_MISO,
    .mosi = PIN_SPI_MOSI,
    .sck = PIN_SPI_SCK,
};

static struct spi_device_cfg ili9341_devcfg = {
    .cs = -1,
    .freq = 80000000, // 80Mhz on iomux, 40Mhz on matrix
    .mode = 0
};

void ili9341_cs(bool state, void *user_ctx)
{
    (void) user_ctx;

    if (state)
        gpio_set(PIN_ILI9341_CS);
    else
        gpio_clear(PIN_ILI9341_CS);
}

void ili9341_dc(bool state, void *user_ctx)
{
    (void) user_ctx;

    if (state)
        gpio_set(PIN_ILI9341_DC);
    else
        gpio_clear(PIN_ILI9341_DC);
}

void ili9341_rst(bool state, void *user_ctx)
{
    (void) state;
    (void) user_ctx;
}

int ili9341_read(uint8_t *data, size_t len, void *user_ctx)
{
    int ret;
    spi_device_t *spi_device = (spi_device_t *) user_ctx;
    ret = spi_receive(spi_device, data, len);
    return ret;
}

int ili9341_write(const uint8_t *data, size_t len, void *user_ctx)
{
    int ret;
    spi_device_t *spi_device = (spi_device_t *) user_ctx;
    ret = spi_transmit(spi_device, data, len);
    return ret;
}

int ili9341_write16(const uint16_t *data, size_t len, void *user_ctx)
{
    int ret;
    spi_device_t *spi_device = (spi_device_t *) user_ctx;
    uint16_t txdata = *data >> 8 | (*data & 0xFF) << 8;
    ret = spi_transmit(spi_device, (const uint8_t*) &txdata, len);
    return ret;
}

int ili9341_xfer(const uint8_t *write, uint8_t *read, size_t len, void *user_ctx)
{
    int ret;
    spi_device_t *spi_device = (spi_device_t *) user_ctx;
    ret = spi_transfer(spi_device, write, read, len);
    return ret;
}

int init_spi_display()
{
    int err;
    spi_device_t ili9341_device;

    printf("initializing spi for display\n");


    ili9341_spi = spi_init(&ili9341_spi_cfg);
    if (NULL == ili9341_spi)
    {
        printf("spi_init failed at %d\n", __LINE__);
        vTaskDelay(10);
        return -EXIT_FAILURE;
    }

    ili9341_device = spi_device_add(ili9341_spi, &ili9341_devcfg);
    if (NULL == ili9341_device)
    {
        printf("spi_device_add failed at %d\n", __LINE__);
        return -EXIT_FAILURE;
    }
    
    gpio_set_mode(PIN_ILI9341_CS, GPIO_OUTPUT);
    gpio_set_pull(PIN_ILI9341_CS, GPIO_PULL_NONE);
    gpio_set(PIN_ILI9341_CS);
    gpio_set_mode(PIN_ILI9341_DC, GPIO_OUTPUT);
    gpio_set_pull(PIN_ILI9341_DC, GPIO_PULL_NONE);
    gpio_set(PIN_ILI9341_DC);

    ili9341_scb.cache_size = 1024;
    ili9341_scb.cache_ptr = heap_caps_malloc(ili9341_scb.cache_size, MALLOC_CAP_DMA);
    ili9341_scb.user_ctx = ili9341_device;
    err = ili9341_init(&ili9341_scb);
    if (ILI9341_E_OK != err)
    {
        printf("ili9341 init failed at %d\n", __LINE__);
        return EXIT_FAILURE;
    }

    vTaskDelay(100);
    ili9341_clear(&ili9341_scb, ILI9341_COLOR_BLACK);
    ili9341_fdraw_vline(&ili9341_scb, 10, 10, 200, ILI9341_COLOR_BLUE);
    ili9341_fdraw_vline(&ili9341_scb, 20, 10, 200, ILI9341_COLOR_BLUE);
    ili9341_fdraw_vline(&ili9341_scb, 30, 10, 200, ILI9341_COLOR_BLUE);
    ili9341_fdraw_vline(&ili9341_scb, 40, 10, 200, ILI9341_COLOR_BLUE);
    ili9341_fdraw_vline(&ili9341_scb, 50, 10, 200, ILI9341_COLOR_BLUE);

    vTaskDelay(100);
    ili9341_clear(&ili9341_scb, ILI9341_COLOR_BLACK);
    ili9341_fdraw_hline(&ili9341_scb, 10, 10, 200, ILI9341_COLOR_BLUE);
    ili9341_fdraw_hline(&ili9341_scb, 10, 20, 200, ILI9341_COLOR_BLUE);
    ili9341_fdraw_hline(&ili9341_scb, 10, 30, 200, ILI9341_COLOR_BLUE);
    ili9341_fdraw_hline(&ili9341_scb, 10, 40, 200, ILI9341_COLOR_BLUE);
    ili9341_fdraw_hline(&ili9341_scb, 10, 50, 200, ILI9341_COLOR_BLUE);

    vTaskDelay(100);
    ili9341_clear(&ili9341_scb, ILI9341_COLOR_BLACK);
    ili9341_fdraw_rect(&ili9341_scb, 20, 20, 100, 100, ILI9341_COLOR_BLUE);
    ili9341_fdraw_rect(&ili9341_scb, 30, 30, 100, 100, ILI9341_COLOR_BLUE);
    ili9341_fdraw_rect(&ili9341_scb, 40, 40, 100, 100, ILI9341_COLOR_BLUE);
    ili9341_fdraw_rect(&ili9341_scb, 50, 50, 100, 100, ILI9341_COLOR_BLUE);
    ili9341_fdraw_rect(&ili9341_scb, 60, 60, 100, 100, ILI9341_COLOR_BLUE);

    vTaskDelay(100);
    ili9341_clear(&ili9341_scb, ILI9341_COLOR_BLACK);
    ili9341_fdraw_filled_rect(&ili9341_scb, 20, 20, 100, 100, ILI9341_COLOR_BLUE);
    ili9341_fdraw_filled_rect(&ili9341_scb, 30, 30, 100, 100, ILI9341_COLOR_BLUE);
    ili9341_fdraw_filled_rect(&ili9341_scb, 40, 40, 100, 100, ILI9341_COLOR_BLUE);
    ili9341_fdraw_filled_rect(&ili9341_scb, 50, 50, 100, 100, ILI9341_COLOR_BLUE);
    ili9341_fdraw_filled_rect(&ili9341_scb, 60, 60, 100, 100, ILI9341_COLOR_BLUE);

    // negative widths

    vTaskDelay(100);
    ili9341_clear(&ili9341_scb, ILI9341_COLOR_BLACK);
    ili9341_fdraw_vline(&ili9341_scb, 10, 10, -200, ILI9341_COLOR_BLUE);
    ili9341_fdraw_vline(&ili9341_scb, 20, 10, -200, ILI9341_COLOR_BLUE);
    ili9341_fdraw_vline(&ili9341_scb, 30, 10, -200, ILI9341_COLOR_BLUE);
    ili9341_fdraw_vline(&ili9341_scb, 40, 10, -200, ILI9341_COLOR_BLUE);
    ili9341_fdraw_vline(&ili9341_scb, 50, 10, -200, ILI9341_COLOR_BLUE);

    vTaskDelay(100);
    ili9341_clear(&ili9341_scb, ILI9341_COLOR_BLACK);
    ili9341_fdraw_hline(&ili9341_scb, 10, 10, -200, ILI9341_COLOR_BLUE);
    ili9341_fdraw_hline(&ili9341_scb, 10, 20, -200, ILI9341_COLOR_BLUE);
    ili9341_fdraw_hline(&ili9341_scb, 10, 30, -200, ILI9341_COLOR_BLUE);
    ili9341_fdraw_hline(&ili9341_scb, 10, 40, -200, ILI9341_COLOR_BLUE);
    ili9341_fdraw_hline(&ili9341_scb, 10, 50, -200, ILI9341_COLOR_BLUE);

    vTaskDelay(100);
    ili9341_clear(&ili9341_scb, ILI9341_COLOR_BLACK);
    ili9341_fdraw_rect(&ili9341_scb, 20, 20, -100, -100, ILI9341_COLOR_BLUE);
    ili9341_fdraw_rect(&ili9341_scb, 30, 30, -100, -100, ILI9341_COLOR_BLUE);
    ili9341_fdraw_rect(&ili9341_scb, 40, 40, -100, -100, ILI9341_COLOR_BLUE);
    ili9341_fdraw_rect(&ili9341_scb, 50, 50, -100, -100, ILI9341_COLOR_BLUE);
    ili9341_fdraw_rect(&ili9341_scb, 60, 60, -100, -100, ILI9341_COLOR_BLUE);

    vTaskDelay(100);
    ili9341_clear(&ili9341_scb, ILI9341_COLOR_BLACK);
    ili9341_fdraw_filled_rect(&ili9341_scb, 20, 20, -100, -100, ILI9341_COLOR_BLUE);
    ili9341_fdraw_filled_rect(&ili9341_scb, 30, 30, -100, -100, ILI9341_COLOR_BLUE);
    ili9341_fdraw_filled_rect(&ili9341_scb, 40, 40, -100, -100, ILI9341_COLOR_BLUE);
    ili9341_fdraw_filled_rect(&ili9341_scb, 50, 50, -100, -100, ILI9341_COLOR_BLUE);
    ili9341_fdraw_filled_rect(&ili9341_scb, 60, 60, -100, -100, ILI9341_COLOR_BLUE);

    vTaskDelay(100);
    ili9341_clear(&ili9341_scb, ILI9341_COLOR_BLACK);
    ili9341_draw_line(&ili9341_scb, 20, 20, 220, 120, ILI9341_COLOR_BLUE);
    ili9341_draw_line(&ili9341_scb, 20, 20, 220, 140, ILI9341_COLOR_BLUE);
    ili9341_draw_line(&ili9341_scb, 20, 20, 220, 160, ILI9341_COLOR_BLUE);
    ili9341_draw_line(&ili9341_scb, 20, 20, 220, 180, ILI9341_COLOR_BLUE);
    ili9341_draw_line(&ili9341_scb, 20, 20, 220, 200, ILI9341_COLOR_BLUE);

    vTaskDelay(100);
    ili9341_invert(&ili9341_scb, true);
    vTaskDelay(100);
    ili9341_invert(&ili9341_scb, false);
    vTaskDelay(100);
    ili9341_invert(&ili9341_scb, true);

    extern const uint16_t aglaea_bmp[];
    ili9341_draw_bitmap(&ili9341_scb, 0, 0, 240, 320, aglaea_bmp, true);
}