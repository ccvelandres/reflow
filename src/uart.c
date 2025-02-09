#include <stdbool.h>
#include "uart.h"

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/dma.h>
#include <libopencm3/stm32/usart.h>

// Remove to profile functions
#if defined(ENABLE_PROFILING)
#define PF_INS __attribute__((no_instrument_function))
#else
#define PF_INS
#endif

#define UART_USE_DMA 1

PF_INS void uart_init(void)
{
    // Read register to see if enabled
    if(USART_CR1(USART1) & USART_CR1_UE)
    {
        return;
    }

    // using PB6 - tx, PB7 - rx
    rcc_periph_clock_enable(RCC_GPIOB);
    gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO9 | GPIO10);
    gpio_set_af(GPIOA, GPIO_AF7, GPIO9 | GPIO10);

    rcc_periph_clock_enable(RCC_USART1);
    usart_set_baudrate(USART1, 115200);
    usart_set_databits(USART1, 8);
    usart_set_stopbits(USART1, USART_STOPBITS_1);
    usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);
    usart_set_mode(USART1, USART_MODE_TX);
    usart_set_parity(USART1, USART_PARITY_NONE);
    usart_enable(USART1);

#if UART_USE_DMA
    rcc_periph_clock_enable(RCC_DMA2);
    dma_stream_reset(DMA2, DMA_STREAM7);
    dma_set_peripheral_address(DMA2, DMA_STREAM7, (uint32_t)&USART1_DR);
    dma_set_priority(DMA2, DMA_STREAM7, DMA_SxCR_PL_HIGH);
    dma_set_memory_size(DMA2, DMA_STREAM7, DMA_SxCR_MSIZE_8BIT);
    dma_set_peripheral_size(DMA2, DMA_STREAM7, DMA_SxCR_PSIZE_8BIT);
    dma_enable_memory_increment_mode(DMA2, DMA_STREAM7);
    dma_set_transfer_mode(DMA2, DMA_STREAM7, DMA_SxCR_DIR_MEM_TO_PERIPHERAL);
    dma_channel_select(DMA2, DMA_STREAM7, DMA_SxCR_CHSEL_4);
    // usart1_tx -> dma2_channel7
    nvic_set_priority(NVIC_DMA2_STREAM7_IRQ, 0);
    nvic_enable_irq(NVIC_DMA2_STREAM7_IRQ);
#endif
    // flush some empty lines
    static const char flush_line[] = "\r\nusart_init\r\n\n";
    uart_write(flush_line, sizeof(flush_line));
}

PF_INS uint32_t uart_write(const char *ptr, uint32_t len)
{
#if UART_USE_DMA
    while (DMA2_S7CR & DMA_SxCR_EN);
    dma_set_memory_address(DMA2, DMA_STREAM7, (uint32_t)ptr);
    dma_set_number_of_data(DMA2, DMA_STREAM7, len);
    dma_enable_stream(DMA2, DMA_STREAM7);
    dma_enable_transfer_complete_interrupt(DMA2, DMA_STREAM7);
    usart_enable_tx_dma(USART1);
#else
    const char *end = ptr + len;

    while(end > ptr)
    {
        usart_wait_send_ready(USART1);
        usart_send(USART1, (uint16_t) *ptr++);
    }
#endif
    return len;
}

PF_INS uint32_t uart_read(char *ptr, uint32_t buflen)
{
    // unimplmented
    return 0;
}

#if UART_USE_DMA
PF_INS void dma2_stream7_isr(void)
{
    if (dma_get_interrupt_flag(DMA2, DMA_STREAM7, DMA_TCIF))
    {
        dma_clear_interrupt_flags(DMA2, DMA_STREAM7, DMA_TCIF);
        usart_disable_tx_dma(USART1);
        dma_disable_transfer_complete_interrupt(DMA2, DMA_STREAM7);
        dma_disable_stream(DMA2, DMA_STREAM7);
    }
}
#endif

#if defined(LOG_STREAM_UART)
#pragma weak log_flush = uart_write
#endif