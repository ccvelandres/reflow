
#include "stdint.h"

#include "libopencm3/stm32/timer.h"

void timer_pwm_init(const uint32_t timer);
void timer_pwm_channel_init(const uint32_t timer, const uint32_t ch, float value);
void timer_pwn_channel_set(const uint32_t timer, const uint32_t ch, float value);