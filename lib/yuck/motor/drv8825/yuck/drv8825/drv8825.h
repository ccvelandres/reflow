// SPDX-License-Identifier: MIT

#ifndef __MOTOR_DRV8825_H__
#define __MOTOR_DRV8825_H__

/**
 * @file drv8825.h
 * @author Cedric Velandres (ccvelandres@gmail.com)
 */

#ifdef __cplusplus
extern "C"
{
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define DRV8825_DIR_CLOCKWISE   (0)
#define DRV8825_DIR_COUNTER   (1)

typedef struct drv8825_scb drv8825_scb_t;

struct drv8825_scb
{
    int pin_en;
    int pin_dir;
    int pin_step;
    int step_per_rev;

    yuck_gpio_set_type_fptr_t fptr_gpio_set_type;
    yuck_gpio_set_state_fptr_t fptr_gpio_set_state;

    struct {
        uint8_t state;
        uint8_t direction;
        uint8_t position;
        uint16_t rpm;
        uint16_t microstep;

        uint32_t pulse_interval;
    } data;
};

int drv8825_init(drv8825_scb_t *scb);
int drv8825_uninit(drv8825_scb_t *scb);

int drv8825_step(drv8825_scb_t *scb);

int drv8825_set_state(drv8825_scb_t *scb, bool state);
int drv8825_get_state(drv8825_scb_t *scb, bool *state);

int drv8825_set_direction(drv8825_scb_t *scb, uint8_t direction);
int drv8825_get_direction(drv8825_scb_t *scb, uint8_t *direction);

int drv8825_set_microstep(drv8825_scb_t *scb, int microstep);
int drv8825_get_microstep(drv8825_scb_t *scb, int *microstep);

int drv8825_set_position(drv8825_scb_t *scb, uint8_t position);
int drv8825_get_position(drv8825_scb_t *scb, uint8_t *position);

int drv8825_set_rpm(drv8825_scb_t *scb, uint16_t rpm);
int drv8825_get_rpm(drv8825_scb_t *scb, uint16_t *rpm);


#ifdef __cplusplus
}
#endif

#endif