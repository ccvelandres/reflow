#include "i2c.h"
#include "gpio.h"
#include "board.h"
#include "yuck/ssd1306/ssd1306.h"
#include "display.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

int ssd1306_write(const uint8_t *data, size_t len, bool lfen, bool lfstop, void *user_ctx);
int ssd1306_read(uint8_t *data, size_t len, bool lfen, bool lfstop, void *user_ctx);

static ssd1306_scb_t ssd1306_scb =
    {
        .width = 128,
        .height = 64,
        .f_delay = vTaskDelay,
        .f_write = ssd1306_write,
        .f_read = ssd1306_read};

static i2c_instance_t ssd1306_i2c;
static struct i2c_config ssd1306_i2c_cfg =
    {
        .name = "i2c",
        .bus_num = 0,
        .scl = PIN_SCL2,
        .sda = PIN_SDA2};

static struct i2c_device_cfg ssd1306_devcfg =
    {
        .max_freq = 400000};

int ssd1306_write(const uint8_t *data, size_t len, bool lfen, bool lfstop, void *user_ctx)
{
    int ret;
    i2c_device_t *dv = (i2c_device_t *)user_ctx;

    if (!lfen || (lfen && !lfstop))
    {
        ret = i2c_start(dv);
        if (ret)
            return ret;
    }

    ret = i2c_write(dv, data, len);
    if (!lfen || (lfen && lfstop))
    {
        ret = i2c_end(dv);
    }
    return ret;
}

int ssd1306_read(uint8_t *data, size_t len, bool lfen, bool lfstop, void *user_ctx)
{
    int ret;
    i2c_device_t *dv = (i2c_device_t *)user_ctx;
    if (!lfen || (lfen && !lfstop))
    {
        ret = i2c_start(dv);
        if (ret)
            return ret;
    }

    ret = i2c_read(dv, data, len);
    if (!lfen || (lfen && lfstop))
    {
        ret = i2c_end(dv);
    }
    return ret;
}

int init_i2c_display()
{
    int ret;
    i2c_device_t ssd1306_device;

    printf("initializing i2c bus for display\n");

    ssd1306_i2c = i2c_init(&ssd1306_i2c_cfg);
    if (NULL == ssd1306_i2c)
    {
        printf("i2c_init failed at %d\n", __LINE__);
        vTaskDelay(10);
        return -EXIT_FAILURE;
    }

    ssd1306_device = i2c_device_add(ssd1306_i2c, &ssd1306_devcfg);
    if (NULL == ssd1306_device)
    {
        printf("i2c_device_add failed at %d\n", __LINE__);
        return -EXIT_FAILURE;
    }

    ssd1306_scb.user_ctx = ssd1306_device;
    ret = ssd1306_init(&ssd1306_scb);
    if (0 != ret)
    {
        printf("ssd1306 init failed at %d\n", __LINE__);
        return EXIT_FAILURE;
    }

    while(1)
    {
        ssd1306_clear(&ssd1306_scb, SSD1306_COLOR_WHITE);
        ssd1306_update_screen(&ssd1306_scb);
        vTaskDelay(50);

        ssd1306_clear(&ssd1306_scb, SSD1306_COLOR_BLACK);
        ssd1306_update_screen(&ssd1306_scb);
        vTaskDelay(50);

        ssd1306_invert(&ssd1306_scb, true);
        ssd1306_update_screen(&ssd1306_scb);
        vTaskDelay(50);

        ssd1306_invert(&ssd1306_scb, false);
        ssd1306_update_screen(&ssd1306_scb);
        vTaskDelay(50);
    }
}