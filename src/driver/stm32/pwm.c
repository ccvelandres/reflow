#include "libopencm3/cm3/nvic.h"
#include "libopencm3/stm32/gpio.h"
#include "libopencm3/stm32/timer.h"
#include "libopencm3/stm32/rcc.h"

// TIM2-TIM5 -- APB1
// TIM9-TIM11 -- APB2

void timer_pwm_init(const uint32_t timer)
{
    uint32_t timer_clock = 1000000;
    uint32_t timer_freq = 200;
    uint32_t timer_abp1_psc = (rcc_apb1_frequency / timer_clock) - 1;
    uint32_t timer_abp2_psc = (rcc_apb2_frequency / timer_clock) - 1;
    volatile uint32_t timer_period = timer_clock / timer_freq;

    timer_set_prescaler(timer, timer_abp1_psc);
    timer_set_period(timer_period, timer_period);
    timer_set_counter(timer, 0);
    timer_enable_preload(timer);
    timer_continuous_mode(timer);
    timer_enable_counter(timer);
}

void timer_pwm_channel_init(const uint32_t timer, const uint32_t ch, float value)
{
    uint32_t timer_period = 5000;
    timer_set_oc_value(timer, ch, (uint32_t) (timer_period * value));
    timer_enable_oc_preload(timer, ch);
    timer_set_oc_mode(timer, ch, TIM_OCM_PWM1);
    timer_enable_oc_output(timer, ch);
    timer_generate_event(timer, TIM_EGR_UG);
}

void timer_pwn_channel_set(const uint32_t timer, const uint32_t ch, float value)
{
    uint32_t timer_period = 5000;
    timer_set_oc_value(timer, ch, (uint32_t) (timer_period * value));
    
}