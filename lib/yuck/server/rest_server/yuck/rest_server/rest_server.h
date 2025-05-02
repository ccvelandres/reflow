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

    struct {
        uint8_t state;
        uint8_t direction;
        uint8_t position;
        uint16_t rpm;
        uint16_t microstep;

        uint32_t pulse_interval;
    } data;
};

int rest_server_init(drv8825_scb_t *scb);
int rest_server_uninit(drv8825_scb_t *scb);


#ifdef __cplusplus
}
#endif

#endif