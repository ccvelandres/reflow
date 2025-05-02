// SPDX-License-Identifier: MIT

/**
 * @file ili9341.cpp
 * @author Cedric Velandres (ccvelandres@gmail.com)
 */

#include "yuck/api.h"
#include "yuck/drv8825/drv8825.h"

#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#define DEFINE_SET_FUNC(rtype, dname)                        \
    int drv8825_set_##dname(drv8825_scb_t *scb, rtype dname) \
    {                                                        \
        YUCK_CHECK_NULL(scb);                              \
        scb->data.dname = dname;                             \
        return YUCK_E_OK;                                  \
    }

#define DEFINE_GET_FUNC(rtype, dname)                         \
    int drv8825_get_##dname(drv8825_scb_t *scb, rtype *dname) \
    {                                                         \
        YUCK_CHECK_NULL(scb);                               \
        YUCK_CHECK_NULL(dname);                             \
        *dname = scb->data.dname;                             \
        return YUCK_E_OK;                                   \
    }

#define DEFINE_GET_SET_FUNC(rtype, dname) \
    DEFINE_SET_FUNC(rtype, dname)         \
    DEFINE_GET_FUNC(rtype, dname)

int drv8825_init(drv8825_scb_t *scb)
{
    YUCK_CHECK_NULL(scb);
    YUCK_CHECK_ARG(0 > scb->pin_dir, YUCK_E_INVALID_ARG);
    YUCK_CHECK_ARG(0 > scb->pin_step, YUCK_E_INVALID_ARG);

    yuck_gpio_set_type(scb->pin_dir, YUCK_GPIO_TYPE_OUTPUT);
    yuck_gpio_set_state(scb->pin_dir, YUCK_GPIO_STATE_LOW);

    yuck_gpio_set_type(scb->pin_step, YUCK_GPIO_TYPE_OUTPUT);
    yuck_gpio_set_state(scb->pin_step, YUCK_GPIO_STATE_LOW);

    if (0 > scb->pin_en)
    {
        yuck_gpio_set_type(scb->pin_en, YUCK_GPIO_TYPE_OUTPUT);
        yuck_gpio_set_state(scb->pin_en, YUCK_GPIO_STATE_LOW);
    }

    // set defaults for data
    memset(&scb->data, 0, YUCK_MEMBER_SIZEOF(drv8825_scb_t, data));
    scb->data.state = 1;
    scb->data.direction = DRV8825_DIR_CLOCKWISE;
    scb->data.position = 0;
    scb->data.rpm = 0;
    scb->data.microstep = 0;

    return YUCK_E_OK;
}

int drv8825_uninit(drv8825_scb_t *scb)
{
    YUCK_CHECK_NULL(scb);
    return YUCK_E_OK;
}

int drv8825_step(drv8825_scb_t *scb)
{
    YUCK_CHECK_NULL(scb);
    
    // prolly make the timer scheduler first
    return YUCK_E_OK;
}

// rpm to interval is
// (((uint32_t) step_per_rev) * ((uint32_t) microstep) * ((uint32_t) rpm) * 2)
// ie for 100 rpm on a 64 step motor on 32 microstep -- pulse interval is 4us

// interval to rpm is:
// (step_interval * 2)

DEFINE_GET_SET_FUNC(bool, state)
DEFINE_GET_SET_FUNC(uint8_t, direction)
DEFINE_GET_SET_FUNC(int, microstep)
DEFINE_GET_SET_FUNC(uint8_t, position)
DEFINE_GET_SET_FUNC(uint16_t, rpm)
