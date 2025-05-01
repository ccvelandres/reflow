#pragma once

#ifdef cplusplus
extern "C" {
#endif

#include <stdint.h>

enum gpio_mode_cfg
{
    GPIO_INPUT,
    GPIO_OUTPUT
};

enum gpio_pull_cfg
{
    GPIO_PULL_NONE,
    GPIO_PULL_DOWN,
    GPIO_PULL_UP
};

typedef void (*gpio_intr_cb)(uint32_t pin);

int gpio_set_mode(uint32_t pin, enum gpio_mode_cfg mode);
int gpio_set_pull(uint32_t pin, enum gpio_pull_cfg pull);
int gpio_set(uint32_t pin);
int gpio_clear(uint32_t pin);
int gpio_toggle(uint32_t pin);

// int gpio_set_intr(uint32_t pin, gpio_intr_cb cb);

#ifdef cplusplus
}
#endif