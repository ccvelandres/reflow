// SPDX-License-Identifier: MIT

#ifndef __YUCK_API_H__
#define __YUCK_API_H__

/**
 * @file error.h
 * @author Cedric Velandres (ccvelandres@gmail.com)
 */

#ifdef __cplusplus
extern "C"
{
#endif

#include "yuck.h"
#include <stdint.h>
#include <stdbool.h>

/*! @name Common Delay Functions */
void yuck_delay_ms(uint32_t ms);
void yuck_delay_ns(uint32_t ns);

/*! @name Common GPIO Functions */
#define YUCK_GPIO_TYPE_INPUT          (0)
#define YUCK_GPIO_TYPE_OUTPUT         (1)

#define YUCK_GPIO_STATE_LOW           (0)
#define YUCK_GPIO_STATE_HIGH          (1)

void yuck_gpio_set_type(uint32_t pin, int type);
void yuck_gpio_set_state(uint32_t pin, int state);

#ifdef __cplusplus
}
#endif
#endif