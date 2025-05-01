#include "gpio.h"
#include <string.h>

#include <esp_log.h>
#include <driver/gpio.h>

static const char *TAG = "driver/gpio";

static const gpio_mode_t _gpio_mode_map[] = {
    GPIO_MODE_INPUT,
    GPIO_MODE_OUTPUT,
};

int gpio_set_mode(uint32_t pin, enum gpio_mode_cfg mode)
{
    int ret = EXIT_SUCCESS;

    ESP_LOGD(TAG, "gpio_set_mode %2u %4x", pin, mode);
    if (GPIO_INPUT == mode)
        ret = gpio_set_direction(pin, GPIO_MODE_INPUT);
    else if (GPIO_OUTPUT == mode)
        ret = gpio_set_direction(pin, GPIO_MODE_OUTPUT);
    else
        ret = -EXIT_FAILURE;

    return ret;
}

int gpio_set_pull(uint32_t pin, enum gpio_pull_cfg pull)
{
    int ret = EXIT_SUCCESS;

    ESP_LOGD(TAG, "gpio_set_pull %2u %4x", pin, pull);
    if (GPIO_PULL_NONE == pull)
        ret = gpio_set_pull_mode(pin, GPIO_FLOATING);
    else if (GPIO_PULL_DOWN == pull)
        ret = gpio_set_pull_mode(pin, GPIO_PULLDOWN_ONLY);
    else if (GPIO_PULL_UP == pull)
        ret = gpio_set_pull_mode(pin, GPIO_PULLUP_ONLY);
    else
        ret = -EXIT_FAILURE;

    return ret;
}

int gpio_set(uint32_t pin)
{
    gpio_set_level(pin, 1);
    return EXIT_SUCCESS;
}

int gpio_clear(uint32_t pin)
{
    gpio_set_level(pin, 0);
    return EXIT_SUCCESS;
}

int gpio_toggle(uint32_t pin)
{
    
}