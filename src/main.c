

#include <libopencm3/cm3/systick.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>

#include <FreeRTOS.h>
#include <task.h>

static void heartbeatFunction(void *pxArg)
{
    while (1)
    {
        gpio_toggle(GPIOC, GPIO13);
        vTaskDelay(200);
    }
}

void board_setup()
{
    /** Clock Configuration -> STM32F411 HSE 25MHz
     * SYSCLK     -> 84MHz
     * USB Clock  -> 48MHz
     * AHB Clock  -> 84MHz
     * APB1 Clock -> 42MHz --> low speed domain 
     * APB2 Clock -> 84MHz --> high speed domain
     */
    rcc_clock_setup_pll(&rcc_hse_25mhz_3v3[RCC_CLOCK_3V3_84MHZ]);
}

void spi_init()
{
    rcc_periph_clock_enable(RCC_GPIOB);

    // SCK  - PB3
    // MISO - PB4
    // MOSI - PB5
    // CS   - PB6
    // DC   - PB7
    gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLDOWN, GPIO3 | GPIO4 | GPIO5);
    gpio_set_af(GPIOB, GPIO_AF5, GPIO3 | GPIO4 | GPIO5);
    gpio_set_output_options(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, GPIO4 | GPIO5);

    gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO6 | GPIO7);
    gpio_set(GPIOB, GPIO6 | GPIO7);

    rcc_periph_clock_enable(RCC_SPI1);
    spi_enable(SPI1);
    spi_init_master(SPI1,
                    SPI_CR1_BR_FPCLK_DIV_8,
                    SPI_CR1_CPOL_CLK_TO_1_WHEN_IDLE,
                    SPI_CR1_CPHA,
                    SPI_CR1_DFF_8BIT,
                    SPI_CR1_MSBFIRST);
}

#include <ili9341.h>

void ili9341_cs(bool state, void *user_ctx);
void ili9341_dc(bool state, void *user_ctx);
void ili9341_rst(bool state, void *user_ctx);
int ili9341_read(uint8_t *data, size_t len, void *user_ctx);
int ili9341_write(const uint8_t *data, size_t len, void *user_ctx);
int ili9341_xfer(const uint8_t *write, uint8_t *read, size_t len, void *user_ctx);


static uint8_t ili9341_cache[128];
static ili9341_scb_t ili9341_scb = {
    .height = 320,
    .width = 240,
    .cache = ili9341_cache,
    .cache_size = sizeof(ili9341_cache),
    .user_ctx = (void*) &ili9341_scb,
    .f_delay = vTaskDelay,
    .f_dc = ili9341_cs,
    .f_rst = NULL, // ili9341_dc,
    .f_cs = ili9341_rst,
    .f_read = ili9341_read,
    .f_write = ili9341_write,
    .f_xfer = ili9341_xfer};

void ili9341_cs(bool state, void *user_ctx)
{
    typedef void (*callback_t)(uint32_t, uint16_t);
    const callback_t cb = state ? gpio_set : gpio_clear;

    if (user_ctx == &ili9341_scb)
        cb(GPIOB, GPIO6);
}

void ili9341_dc(bool state, void *user_ctx)
{
    typedef void (*callback_t)(uint32_t, uint16_t);
    const callback_t cb = state ? gpio_set : gpio_clear;

    if (user_ctx == &ili9341_scb)
        cb(GPIOB, GPIO7);
}

void ili9341_rst(bool state, void *user_ctx)
{
}

int ili9341_read(uint8_t *data, size_t len, void *user_ctx)
{
    int xfer_len = 0;
    if (user_ctx == &ili9341_scb)
    {
        while(len > xfer_len)
        {
            data[xfer_len] = spi_read(SPI1) & 0xFF;
            xfer_len++;
        }
    }
    return xfer_len;
}

int ili9341_write(const uint8_t *data, size_t len, void *user_ctx)
{
    int xfer_len = 0;
    if (user_ctx == &ili9341_scb)
    {
        while(len > xfer_len)
        {
            (void) spi_xfer(SPI1, data[xfer_len]);
            xfer_len++;
        }
    }
    return xfer_len;
}

int ili9341_xfer(const uint8_t *write, uint8_t *read, size_t len, void *user_ctx)
{
    int xfer_len = 0;
    if (user_ctx == &ili9341_scb)
    {
        while(len > xfer_len)
        {
            uint8_t rd = spi_xfer(SPI1, write[xfer_len]) & 0xFF;
            read[xfer_len] = rd;
            xfer_len++;
        }
    }
    return xfer_len;
}

void mainFunction(void *pxArg)
{
    (void) pxArg;

    ili9341_init(&ili9341_scb);
    while(1)
    {
        ili9341_clear(&ili9341_scb, ILI9341_BLACK);
        vTaskDelay(100);
        ili9341_clear(&ili9341_scb, ILI9341_WHITE);
        vTaskDelay(100);
    }
}

int main()
{
    board_setup();
    spi_init();

    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOB);
    rcc_periph_clock_enable(RCC_GPIOC);

    gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO13);
    gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO0);
    gpio_clear(GPIOC, GPIO13);
    
    static TaskHandle_t heartbeatTaskHandle;
    static StaticTask_t heartbeatTaskBuffer;
    static StackType_t  heartbeatStack[256];
    heartbeatTaskHandle = xTaskCreateStatic(heartbeatFunction,
                                            "heartbeatTask",
                                            256,
                                            NULL,
                                            1,
                                            heartbeatStack,
                                            &heartbeatTaskBuffer);
    (void)heartbeatTaskHandle;
    
    static TaskHandle_t mainTaskHandle;
    static StaticTask_t mainTaskBuffer;
    static StackType_t  mainStack[1024];
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
