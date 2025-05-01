// SPDX-License-Identifier: MIT

/**
 * @file ili9341.cpp
 * @author Cedric Velandres (ccvelandres@gmail.com)
 */

#include "yuck/ssd1306/ssd1306.h"

#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#define I2C_MASTER_WRITE (0x0)
#define I2C_MASTER_READ (0x1)

// clang-format off
static const uint8_t ssd1306_init_cmds[] = 
{
    // adafruit init sequence from adafruit/Adafruit_SSD1306
    50, 0, SSD1306_CMD_DISPLAY_OFF,
    0,  1, SSD1306_CMD_DISPCLOCKDIV, 0x80,
    0,  1, SSD1306_CMD_MULTIPLEX_RATIO, 0x1F,
    0,  1, SSD1306_CMD_DISP_OFFSET, 0,
    0,  0, SSD1306_CMD_START_LINE,
    0,  1, SSD1306_CMD_CHARGE_PUMP_CFG, 0x14,
    0,  1, SSD1306_CMD_MEMADDR_MODE, 0x00,
    0,  0, SSD1306_CMD_SEG_REMAP_127,
    0,  0, SSD1306_CMD_SCANDIR_NEG,
    0,  1, SSD1306_CMD_COMPIN_HWCONF, 0x02,
    0,  1, SSD1306_CMD_CONTRAST_CTRL, 0x8F,
    0,  1, SSD1306_CMD_PRECHARGE, 0xC2,
    0,  1, SSD1306_CMD_VCOMLEVEL, 0x40,
    0,  0, SSD1306_CMD_DISPLAY_RAM,
    0,  0, SSD1306_CMD_DISP_NORMAL,
    0,  0, SSD1306_CMD_SCROLL_DISABLE,
    0,  0, SSD1306_CMD_DISPLAY_ON,
};
// clang-format on
        // 0x02 for 128x32, 96x16
        // 0x12 for 128x64

static int _ssd1306_validate_config(const ssd1306_scb_t *scb)
{
    if (NULL == scb ||
        NULL == scb->framebuf ||
        NULL == scb->f_delay ||
        NULL == scb->f_read ||
        NULL == scb->f_write)
    {
        return -EINVAL;
    }
}

int ssd1306_init(ssd1306_scb_t *scb)
{
    int ret;
    uint8_t wbuf[2];

    ret = _ssd1306_validate_config(scb);
    if (ret)
        return ret;

    // set page addr and col address
    wbuf[0] = SSD1306_I2C_ADDRESS << 1 | I2C_MASTER_WRITE;
    wbuf[1] = SSD1306_I2C_CB_COMMAND;

    const uint8_t *addr = ssd1306_init_cmds;
    const uint8_t *cmd_end = ssd1306_init_cmds + sizeof(ssd1306_init_cmds);
    printf("ssd1306_init: starting init\n");
    while ((cmd_end > addr) && !ret)
    {
        uint8_t delay = *addr++;
        uint8_t argc = *addr++ + 1;
        uint8_t *cmd = addr++;
        if (argc)
            addr += argc;

        printf("ssd1306_init: cmd: %02x argc: %02x\n", cmd, argc - 1);
        ret = scb->f_write(wbuf, sizeof(wbuf), true, false, scb->user_ctx);
        if (ret)
            goto exit;

        ret = scb->f_write(cmd, argc, true, true, scb->user_ctx);
        if (ret)
            goto exit;
        scb->f_delay(delay);
    }

exit:
    return ret;
}

int ssd1306_clear(ssd1306_scb_t *scb, uint16_t color)
{
    if (NULL == scb)
    {
        return -EINVAL;
    }

    switch (color)
    {
    case SSD1306_COLOR_BLACK:
        memset(scb->framebuf, 0, sizeof(scb->framebuf));
        break;
    case SSD1306_COLOR_WHITE:
        memset(scb->framebuf, 0xFF, sizeof(scb->framebuf));
        break;
    default:
        break;
    }

    return EXIT_SUCCESS;
}

int ssd1306_invert(ssd1306_scb_t* scb, bool invert)
{
    int ret;
    uint8_t wbuf[3];
    if (NULL == scb)
    {
        return -EINVAL;
    }

    wbuf[0] = SSD1306_I2C_ADDRESS << 1 | I2C_MASTER_WRITE;
    wbuf[1] = SSD1306_I2C_CB_COMMAND;
    wbuf[2] = invert ? SSD1306_CMD_DISP_INVERSE : SSD1306_CMD_DISP_NORMAL;

    ret = scb->f_write(wbuf, sizeof(wbuf), false, false, scb->user_ctx);
    return ret;
}

int ssd1306_update_screen(ssd1306_scb_t *scb)
{
    int ret;
    uint8_t wbuf[2];

    if (NULL == scb)
    {
        return -EINVAL;
    }

    // set page addr and col address
    wbuf[0] = SSD1306_I2C_ADDRESS << 1 | I2C_MASTER_WRITE;
    wbuf[1] = SSD1306_I2C_CB_DATA;
    ret = scb->f_write(wbuf, sizeof(wbuf), true, false, scb->user_ctx);
    if (ret)
        goto exit;

    ret = scb->f_write(scb->framebuf, sizeof(scb->framebuf), true, true, scb->user_ctx);
    if (ret)
        goto exit;
exit:
    return ret;
}

int ssd1306_draw_pixel(ssd1306_scb_t *scb, int16_t x1, int16_t y1, uint16_t color)
{
}

int ssd1306_draw_line(ssd1306_scb_t *scb, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
}

int ssd1306_draw_bitmap(ssd1306_scb_t *scb, int16_t x, int16_t y, int16_t w, int16_t h, const uint16_t *colors, bool le)
{
}
