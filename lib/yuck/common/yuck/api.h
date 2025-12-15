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
#include "yuck_impl.h"
#include <stdint.h>
#include <stdbool.h>

/*! @name Common Delay Functions */
void yuck_delay_ms(uint32_t ms);
void yuck_delay_ns(uint32_t ns);

/*****************************************************************************/
/**
 * @defgroup Yuck Interrupt API
 * @{
 */

typedef struct yuck_mutex_api_s yuck_mutex_api_t;

typedef int (*yuck_isr_enable)();
typedef int (*yuck_isr_disable)();

struct yuck_isr_api_s
{
    yuck_isr_enable enable; 
    yuck_isr_disable disable; 
};

extern yuck_mutex_api_t yuck_mutex_api;

/** @} */

/*****************************************************************************/
/**
 * @defgroup Yuck Mutex API
 * @{
 */

typedef void* yuck_mutex_t;
typedef struct yuck_mutex_api_s yuck_mutex_api_t;

typedef int (*yuck_mutex_init)(yuck_mutex_t);
typedef int (*yuck_mutex_lock)(yuck_mutex_t);
typedef int (*yuck_mutex_unlock)(yuck_mutex_t);

struct yuck_mutex_api_s
{
    yuck_mutex_init init_fptr;
    yuck_mutex_lock lock_fptr;
    yuck_mutex_unlock unlock_fptr;
};
struct yuck_mutex_api_s

/** @} */

/*****************************************************************************/
/**
 * @defgroup Yuck Common GPIO API
 * @{
 */
#define YUCK_GPIO_TYPE_INPUT          (0)
#define YUCK_GPIO_TYPE_OUTPUT         (1)

#define YUCK_GPIO_STATE_LOW           (0)
#define YUCK_GPIO_STATE_HIGH          (1)

typedef int (*yuck_gpio_set_type_fptr_t)(uint32_t pin, int type);
typedef int (*yuck_gpio_set_state_fptr_t)(uint32_t pin, int state);

/** @} */

/*****************************************************************************/
/**
 * @defgroup Yuck Common Timer API
 * @{
 */


typedef int (*yuck_xx_fptr_t)(uint32_t pin, int type);
typedef int (*yuck_xx_fptr_t)(uint32_t pin, int state);

/** @} */


#ifdef __cplusplus
}
#endif
#endif