

#include <libopencm3/cm3/systick.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>

#include <FreeRTOS.h>
#include <task.h>
#include <SEGGER_RTT.h>
#include "uart.h"
#include "stdio.h"
#include "pwm.h"

#define ILI9341_SPIBUS (SPI2)

void spi_init()
{

    // SCK  - PB13
    // MISO - PB14
    // MOSI - PB15
    gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLDOWN, GPIO13 | GPIO14 | GPIO15);
    gpio_set_af(GPIOB, GPIO_AF5, GPIO13 | GPIO14 | GPIO15);
    gpio_set_output_options(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, GPIO13 | GPIO15);

    rcc_periph_clock_enable(RCC_SPI2);
    spi_enable(ILI9341_SPIBUS);

    uint32_t reg = SPI_CR1(ILI9341_SPIBUS);
    reg &= SPI_CR1_SPE | SPI_CR1_CRCEN | SPI_CR1_CRCNEXT;
    reg |= SPI_CR1_MSTR;                    // master
    reg |= SPI_CR1_BAUDRATE_FPCLK_DIV_2;    // div 8
    reg |= SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE; // cpol 0
    reg |= SPI_CR1_CPHA_CLK_TRANSITION_1;   // cpha 1 edge
    reg |= SPI_CR1_MSBFIRST;                // msb first
    reg |= SPI_CR1_DFF_8BIT;                // 8 bit frame
    reg |= SPI_CR1_SSI | SPI_CR1_SSM;

    SPI_CR1(ILI9341_SPIBUS) = reg;
}

#include <ili9341.h>

#define ILI9341_PIN_CS (GPIO12)
#define ILI9341_PORT_CS (GPIOB)
#define ILI9341_PIN_DC (GPIO10)
#define ILI9341_PORT_DC (GPIOB)
#define ILI9341_PIN_RST (GPIO2)
#define ILI9341_PORT_RST (GPIOB)
#define ILI9341_PIN_LED (GPIO1)
#define ILI9341_PORT_LED (GPIOB)

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

void mainFunction(void *pxArg)
{
    (void)pxArg;
    uint32_t buffer = 0;

    ili9341_init(&ili9341_scb);

    // vTaskDelay(100);
    // ili9341_clear(&ili9341_scb, ILI9341_COLOR_BLACK);
    // ili9341_fdraw_vline(&ili9341_scb, 10, 10, 200, ILI9341_COLOR_BLUE);
    // ili9341_fdraw_vline(&ili9341_scb, 20, 10, 200, ILI9341_COLOR_BLUE);
    // ili9341_fdraw_vline(&ili9341_scb, 30, 10, 200, ILI9341_COLOR_BLUE);
    // ili9341_fdraw_vline(&ili9341_scb, 40, 10, 200, ILI9341_COLOR_BLUE);
    // ili9341_fdraw_vline(&ili9341_scb, 50, 10, 200, ILI9341_COLOR_BLUE);

    // vTaskDelay(100);
    // ili9341_clear(&ili9341_scb, ILI9341_COLOR_BLACK);
    // ili9341_fdraw_hline(&ili9341_scb, 10, 10, 200, ILI9341_COLOR_BLUE);
    // ili9341_fdraw_hline(&ili9341_scb, 10, 20, 200, ILI9341_COLOR_BLUE);
    // ili9341_fdraw_hline(&ili9341_scb, 10, 30, 200, ILI9341_COLOR_BLUE);
    // ili9341_fdraw_hline(&ili9341_scb, 10, 40, 200, ILI9341_COLOR_BLUE);
    // ili9341_fdraw_hline(&ili9341_scb, 10, 50, 200, ILI9341_COLOR_BLUE);

    // vTaskDelay(100);
    // ili9341_clear(&ili9341_scb, ILI9341_COLOR_BLACK);
    // ili9341_fdraw_rect(&ili9341_scb, 20, 20, 100, 100, ILI9341_COLOR_BLUE);
    // ili9341_fdraw_rect(&ili9341_scb, 30, 30, 100, 100, ILI9341_COLOR_BLUE);
    // ili9341_fdraw_rect(&ili9341_scb, 40, 40, 100, 100, ILI9341_COLOR_BLUE);
    // ili9341_fdraw_rect(&ili9341_scb, 50, 50, 100, 100, ILI9341_COLOR_BLUE);
    // ili9341_fdraw_rect(&ili9341_scb, 60, 60, 100, 100, ILI9341_COLOR_BLUE);

    // vTaskDelay(100);
    // ili9341_clear(&ili9341_scb, ILI9341_COLOR_BLACK);
    // ili9341_fdraw_filled_rect(&ili9341_scb, 20, 20, 100, 100, ILI9341_COLOR_BLUE);
    // ili9341_fdraw_filled_rect(&ili9341_scb, 30, 30, 100, 100, ILI9341_COLOR_BLUE);
    // ili9341_fdraw_filled_rect(&ili9341_scb, 40, 40, 100, 100, ILI9341_COLOR_BLUE);
    // ili9341_fdraw_filled_rect(&ili9341_scb, 50, 50, 100, 100, ILI9341_COLOR_BLUE);
    // ili9341_fdraw_filled_rect(&ili9341_scb, 60, 60, 100, 100, ILI9341_COLOR_BLUE);

    // // negative widths

    // vTaskDelay(100);
    // ili9341_clear(&ili9341_scb, ILI9341_COLOR_BLACK);
    // ili9341_fdraw_vline(&ili9341_scb, 10, 10, -200, ILI9341_COLOR_BLUE);
    // ili9341_fdraw_vline(&ili9341_scb, 20, 10, -200, ILI9341_COLOR_BLUE);
    // ili9341_fdraw_vline(&ili9341_scb, 30, 10, -200, ILI9341_COLOR_BLUE);
    // ili9341_fdraw_vline(&ili9341_scb, 40, 10, -200, ILI9341_COLOR_BLUE);
    // ili9341_fdraw_vline(&ili9341_scb, 50, 10, -200, ILI9341_COLOR_BLUE);

    // vTaskDelay(100);
    // ili9341_clear(&ili9341_scb, ILI9341_COLOR_BLACK);
    // ili9341_fdraw_hline(&ili9341_scb, 10, 10, -200, ILI9341_COLOR_BLUE);
    // ili9341_fdraw_hline(&ili9341_scb, 10, 20, -200, ILI9341_COLOR_BLUE);
    // ili9341_fdraw_hline(&ili9341_scb, 10, 30, -200, ILI9341_COLOR_BLUE);
    // ili9341_fdraw_hline(&ili9341_scb, 10, 40, -200, ILI9341_COLOR_BLUE);
    // ili9341_fdraw_hline(&ili9341_scb, 10, 50, -200, ILI9341_COLOR_BLUE);

    // vTaskDelay(100);
    // ili9341_clear(&ili9341_scb, ILI9341_COLOR_BLACK);
    // ili9341_fdraw_rect(&ili9341_scb, 20, 20, -100, -100, ILI9341_COLOR_BLUE);
    // ili9341_fdraw_rect(&ili9341_scb, 30, 30, -100, -100, ILI9341_COLOR_BLUE);
    // ili9341_fdraw_rect(&ili9341_scb, 40, 40, -100, -100, ILI9341_COLOR_BLUE);
    // ili9341_fdraw_rect(&ili9341_scb, 50, 50, -100, -100, ILI9341_COLOR_BLUE);
    // ili9341_fdraw_rect(&ili9341_scb, 60, 60, -100, -100, ILI9341_COLOR_BLUE);

    // vTaskDelay(100);
    // ili9341_clear(&ili9341_scb, ILI9341_COLOR_BLACK);
    // ili9341_fdraw_filled_rect(&ili9341_scb, 20, 20, -100, -100, ILI9341_COLOR_BLUE);
    // ili9341_fdraw_filled_rect(&ili9341_scb, 30, 30, -100, -100, ILI9341_COLOR_BLUE);
    // ili9341_fdraw_filled_rect(&ili9341_scb, 40, 40, -100, -100, ILI9341_COLOR_BLUE);
    // ili9341_fdraw_filled_rect(&ili9341_scb, 50, 50, -100, -100, ILI9341_COLOR_BLUE);
    // ili9341_fdraw_filled_rect(&ili9341_scb, 60, 60, -100, -100, ILI9341_COLOR_BLUE);

    // vTaskDelay(100);
    // ili9341_clear(&ili9341_scb, ILI9341_COLOR_BLACK);
    // ili9341_draw_line(&ili9341_scb, 20, 20, 220, 120, ILI9341_COLOR_BLUE);
    // ili9341_draw_line(&ili9341_scb, 20, 20, 220, 140, ILI9341_COLOR_BLUE);
    // ili9341_draw_line(&ili9341_scb, 20, 20, 220, 160, ILI9341_COLOR_BLUE);
    // ili9341_draw_line(&ili9341_scb, 20, 20, 220, 180, ILI9341_COLOR_BLUE);
    // ili9341_draw_line(&ili9341_scb, 20, 20, 220, 200, ILI9341_COLOR_BLUE);

    // vTaskDelay(100);
    // ili9341_invert(&ili9341_scb, true);
    // vTaskDelay(100);
    // ili9341_invert(&ili9341_scb, false);
    // vTaskDelay(100);
    // ili9341_invert(&ili9341_scb, true);

    extern const uint16_t aglaea_bmp[];
    ili9341_draw_bitmap(&ili9341_scb, 0, 0, 240, 320, aglaea_bmp, true);
    gpio_toggle(GPIOC, GPIO13);
    vTaskDelay(1000);

    while (1)
    {
        ili9341_clear(&ili9341_scb, ILI9341_COLOR_BLACK);
        gpio_toggle(GPIOC, GPIO13);
        vTaskDelay(500);
        ili9341_clear(&ili9341_scb, ILI9341_COLOR_RED);
        gpio_toggle(GPIOC, GPIO13);
        vTaskDelay(500);
        ili9341_clear(&ili9341_scb, ILI9341_COLOR_WHITE);
        gpio_toggle(GPIOC, GPIO13);
        vTaskDelay(500);
        ili9341_draw_bitmap(&ili9341_scb, 0, 0, 240, 320, aglaea_bmp, true);
        gpio_toggle(GPIOC, GPIO13);
        vTaskDelay(1000);
    }
}

int init()
{
    /** Clock Configuration -> STM32F411 HSE 25MHz
     * SYSCLK     -> 84MHz
     * USB Clock  -> 48MHz
     * AHB Clock  -> 84MHz
     * APB1 Clock -> 42MHz --> low speed domain
     * APB2 Clock -> 84MHz --> high speed domain
     */
    rcc_clock_setup_pll(&rcc_hse_25mhz_3v3[RCC_CLOCK_3V3_84MHZ]);

    SEGGER_RTT_Init();

    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOB);
    rcc_periph_clock_enable(RCC_GPIOC);

    spi_init();
    uart_init();
    printf("starting\r\n");

    gpio_mode_setup(ILI9341_PORT_CS, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, ILI9341_PIN_CS);
    gpio_set(ILI9341_PORT_CS, ILI9341_PIN_CS);

    gpio_mode_setup(ILI9341_PORT_DC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, ILI9341_PIN_DC);
    gpio_set(ILI9341_PORT_DC, ILI9341_PIN_DC);

    gpio_mode_setup(ILI9341_PORT_LED, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, ILI9341_PIN_LED);
    gpio_set(ILI9341_PORT_LED, ILI9341_PIN_LED);

    gpio_mode_setup(ILI9341_PORT_RST, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, ILI9341_PIN_RST);
    gpio_set(ILI9341_PORT_RST, ILI9341_PIN_RST);

    gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO13);
    gpio_set(GPIOC, GPIO13);

    gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO0);

    static TaskHandle_t mainTaskHandle;
    static StaticTask_t mainTaskBuffer;
    static StackType_t mainStack[1024];

    mainTaskHandle = xTaskCreateStatic(mainFunction,
                                       "mainTask",
                                       256,
                                       NULL,
                                       1,
                                       mainStack,
                                       &mainTaskBuffer);
    (void)mainTaskHandle;

    vTaskStartScheduler();

    while (1)
    {
    }
}
