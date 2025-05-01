#include "spi.h"
#include "ili9341.h"



void ili9341_cs(bool state, void *user_ctx);
void ili9341_dc(bool state, void *user_ctx);
void ili9341_rst(bool state, void *user_ctx);
int ili9341_read(uint8_t *data, size_t len, void *user_ctx);
int ili9341_write(const uint8_t *data, size_t len, void *user_ctx);
int ili9341_write16(const uint16_t *data, size_t len, void *user_ctx);
int ili9341_xfer(const uint8_t *write, uint8_t *read, size_t len, void *user_ctx);

static uint8_t ili9341_cache[256];
static ili9341_scb_t ili9341_scb = {
    .height = 320,
    .width = 240,
    .cache_ptr = ili9341_cache,
    .cache_size = sizeof(ili9341_cache),
    .user_ctx = (void *)&ili9341_scb,
    .f_delay = vTaskDelay,
    .f_dc = ili9341_dc,
    .f_rst = ili9341_rst,
    .f_cs = ili9341_cs,
    .f_read = ili9341_read,
    .f_write = ili9341_write,
    .f_write16 = ili9341_write16,
    .f_xfer = ili9341_xfer};

#if defined(ESP32)

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// #define ILI9341_PIN_CS (GPIO12)
// #define ILI9341_PORT_CS (GPIOB)
// #define ILI9341_PIN_DC (GPIO10)
// #define ILI9341_PORT_DC (GPIOB)
// #define ILI9341_PIN_RST (GPIO2)
// #define ILI9341_PORT_RST (GPIOB)
// #define ILI9341_PIN_LED (GPIO1)
// #define ILI9341_PORT_LED (GPIOB)



void ili9341_cs(bool state, void *user_ctx)
{

}

void ili9341_dc(bool state, void *user_ctx)
{

}

void ili9341_rst(bool state, void *user_ctx)
{

}

int ili9341_read(uint8_t *data, size_t len, void *user_ctx)
{

}

int ili9341_write(const uint8_t *data, size_t len, void *user_ctx)
{

}

int ili9341_write16(const uint16_t *data, size_t len, void *user_ctx)
{

}

int ili9341_xfer(const uint8_t *write, uint8_t *read, size_t len, void *user_ctx)
{

}


#else

#include <FreeRTOS.h>
#include <task.h>

#define ILI9341_PIN_CS (GPIO12)
#define ILI9341_PORT_CS (GPIOB)
#define ILI9341_PIN_DC (GPIO10)
#define ILI9341_PORT_DC (GPIOB)
#define ILI9341_PIN_RST (GPIO2)
#define ILI9341_PORT_RST (GPIOB)
#define ILI9341_PIN_LED (GPIO1)
#define ILI9341_PORT_LED (GPIOB)


void ili9341_dff(bool b16, void *user_ctx)
{
    uint32_t reg = SPI_CR1(ILI9341_SPIBUS);
    if (b16)
    {
        reg = (reg & ~SPI_CR1_DFF) | SPI_CR1_DFF_16BIT;
        SPI_CR1(ILI9341_SPIBUS) = reg;
    }
    else
    {
        reg = (reg & ~SPI_CR1_DFF) | SPI_CR1_DFF_8BIT;
        SPI_CR1(ILI9341_SPIBUS) = reg;
    }
}

void ili9341_cs(bool state, void *user_ctx)
{
    typedef void (*callback_t)(uint32_t, uint16_t);
    const callback_t cb = state ? gpio_set : gpio_clear;

    if (user_ctx == &ili9341_scb)
        cb(ILI9341_PORT_CS, ILI9341_PIN_CS);
}

void ili9341_dc(bool state, void *user_ctx)
{
    typedef void (*callback_t)(uint32_t, uint16_t);
    const callback_t cb = state ? gpio_set : gpio_clear;

    if (user_ctx == &ili9341_scb)
        cb(ILI9341_PORT_DC, ILI9341_PIN_DC);
}

void ili9341_rst(bool state, void *user_ctx)
{
    typedef void (*callback_t)(uint32_t, uint16_t);
    const callback_t cb = state ? gpio_set : gpio_clear;

    if (user_ctx == &ili9341_scb)
        cb(ILI9341_PORT_RST, ILI9341_PIN_RST);
}

int ili9341_read(uint8_t *data, size_t len, void *user_ctx)
{
    int xfer_len = 0;
    if (user_ctx == &ili9341_scb)
    {
        while (len > xfer_len)
        {
            data[xfer_len] = spi_xfer(ILI9341_SPIBUS, 0) & 0xFF;
            xfer_len++;
        }
    }
    return EXIT_SUCCESS;
}

int ili9341_write(const uint8_t *data, size_t len, void *user_ctx)
{
    const uint8_t *end = data + len;
    if (user_ctx == &ili9341_scb)
    {
        while (end > data)
        {
            (void)spi_xfer(ILI9341_SPIBUS, *data++);
        }
    }
    return EXIT_SUCCESS;
}

int ili9341_write16(const uint16_t *data, size_t len, void *user_ctx)
{
    const uint16_t *end = data + len;
    spi_set_dff_16bit(ILI9341_SPIBUS);
    if (user_ctx == &ili9341_scb)
    {
        while (end > data)
        {
            (void) spi_xfer(ILI9341_SPIBUS, *((uint16_t*) data));
            data++;
        }
    }
    spi_set_dff_8bit(ILI9341_SPIBUS);
    return EXIT_SUCCESS;
}

int ili9341_xfer(const uint8_t *write, uint8_t *read, size_t len, void *user_ctx)
{
    int xfer_len = 0;
    if (user_ctx == &ili9341_scb)
    {
        while (len > xfer_len)
        {
            uint8_t rd = spi_xfer(ILI9341_SPIBUS, write[xfer_len]) & 0xFF;
            read[xfer_len] = rd;
            xfer_len++;
        }
    }
    return EXIT_SUCCESS;
}
#endif

int display_init()
{
    int err = ili9341_init(&ili9341_scb);
    if (ILI9341_E_OK != err)
    {
        return EXIT_FAILURE;
    }
}