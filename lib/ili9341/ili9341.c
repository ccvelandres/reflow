// SPDX-License-Identifier: MIT

#include <ili9341.h>

/**
 * @file ili9341.cpp
 * @author Cedric Velandres (ccvelandres@gmail.com)
 */

// clang-format off
static const uint8_t ili9341_init_cmds[] = 
{
    ILI9341_CMD_SWRST, 50, 0,
    ILI9341_CMD_DISPLAY_OFF, 2, 0,

    // 0xEF, 0, 3, 0x03, 0x80, 0x02,
    // ILI9341_CMD_PWRCTRLB, 0, 3, 0x00, 0xC1, 0x30,
    // ILI9341_CMD_PWRSEQCTRL, 0, 4, 0x64, 0x03, 0x12, 0x81,
    // ILI9341_CMD_DTCTRLA, 0, 3, 0x85, 0x00, 0x78,
    // ILI9341_CMD_PWRCTRLA, 0, 5, 0x39, 0x2C, 0x00, 0x34, 0x02,
    // ILI9341_CMD_PUMPRCTRL, 0, 1, 0x20,
    // ILI9341_CMD_DTCTRLB, 0, 2, 0x00, 0x00,

    // Power control
    ILI9341_CMD_PWCTRL1, 0, 1, 0x23,
    ILI9341_CMD_PWCTRL2, 0, 1, 0x10,
    ILI9341_CMD_VCCTRL1, 0, 2, 0x3E, 0x28,
    ILI9341_CMD_VCCTRL2, 0, 1, 0x86,
    ILI9341_CMD_MADCTL, 0, 1, 0x48,

    // reset scroll, pixel format
    ILI9341_CMD_VSCRADDR, 0, 1, 0x00,
    ILI9341_CMD_PIXFMT, 0, 1, 0x55,
    ILI9341_CMD_FRMCTRL1, 0, 2, 0x00, 0x1B,
    ILI9341_CMD_DISPFUNC, 0, 3, 0x08, 0x82, 0x27,
    ILI9341_CMD_ENTRY_MODE, 0, 1, 0x07,

    // Gamma
    // ILI9341_CMD_EN3G, 0, 1, 00,
    // ILI9341_CMD_GAMMA_SET, 0, 1, 0x01,
    // ILI9341_CMD_PGMCRP, 0, 15, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00,
    // ILI9341_CMD_NGMCRP, 0, 15, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F,

    ILI9341_CMD_SLEEP_OUT, 150, 0,
    ILI9341_CMD_DISPLAY_ON, 150, 0,
};
// clang-format on

// clang-format off
#define ILI9341_START_TX(scb)   do { if (NULL != scb->f_cs) scb->f_cs(false, scb->user_ctx); } while(0);
#define ILI9341_END_TX(scb)     do { if (NULL != scb->f_cs) scb->f_cs(true, scb->user_ctx); } while(0);
#define ILI9341_START_DC(scb)   do { scb->f_dc(false, scb->user_ctx); } while(0);
#define ILI9341_END_DC(scb)     do { scb->f_dc(true, scb->user_ctx); } while(0);
#define ILI9341_START_RST(scb)  do { scb->f_rst(false, scb->user_ctx); } while(0);
#define ILI9341_END_RST(scb)    do { scb->f_rst(true, scb->user_ctx); } while(0);

#define ILI9341_DFF_B8(scb)     do { if (NULL != scb->f_dff) scb->f_dff(false, scb->user_ctx); } while(0);
#define ILI9341_DFF_B16(scb)    do { if (NULL != scb->f_dff) scb->f_dff(true, scb->user_ctx); } while(0);

// clang-format on

int ili9341_cmd(ili9341_scb_t *scb, uint8_t cmd)
{
    int ret;
    ILI9341_START_DC(scb);
    ret = scb->f_write(&cmd, 1, scb->user_ctx);
    ILI9341_END_DC(scb);

    return (ret) ? ILI9341_E_COMM_FAIL : ret;
}

int ili9341_write_cmd(ili9341_scb_t *scb, uint8_t cmd, const uint8_t *data, size_t len)
{
    ILI9341_START_TX(scb);
    int ret = ili9341_cmd(scb, cmd);
    if (!ret && len > 0)
        ret = scb->f_write(data, len, scb->user_ctx);
    ILI9341_END_TX(scb);

    return (ret) ? ILI9341_E_COMM_FAIL : ret;
}

int ili9341_read_cmd(ili9341_scb_t *scb, uint8_t cmd, uint8_t *data, size_t len)
{
    ILI9341_START_TX(scb);
    int ret = ili9341_cmd(scb, cmd);
    if (!ret && len > 0)
        ret = scb->f_read(data, len, scb->user_ctx);
    ILI9341_END_TX(scb);

    return (ret) ? ILI9341_E_COMM_FAIL : ret;
}

int ili9341_init(ili9341_scb_t *scb)
{
    int ret = 0;
    if (NULL == scb)
    {
        return ILI9341_E_NULLPTR;
    }

    if (NULL == scb->cache_ptr ||
        4 > scb->cache_size ||
        NULL == scb->f_dc ||
        NULL == scb->f_delay ||
        NULL == scb->f_read ||
        NULL == scb->f_write ||
        NULL == scb->f_xfer)
    {
        return ILI9341_E_INVALID_ARG;
    }

    if (scb->f_rst)
    {
        ILI9341_END_RST(scb);
        scb->f_delay(100);
        ILI9341_START_RST(scb);
        scb->f_delay(100);
        ILI9341_END_RST(scb);
    }
    ILI9341_END_DC(scb);
    ILI9341_END_TX(scb);

    const uint8_t *addr = ili9341_init_cmds;
    const uint8_t *cmd_end = ili9341_init_cmds + sizeof(ili9341_init_cmds);
    while ((cmd_end > addr) && !ret)
    {
        uint8_t cmd = *addr++;
        uint8_t delay = *addr++;
        uint8_t argc = *addr++;
        const uint8_t *argv = addr;
        if (argc)
            addr += argc;

        ret = ili9341_write_cmd(scb, cmd, argv, argc);
        if (ret)
            goto end;
        scb->f_delay(delay);
    }

end:
    return ret;
}

int ili9341_clear(ili9341_scb_t *scb, uint16_t color)
{
    int ret = 0;
    if (NULL == scb)
    {
        return ILI9341_E_NULLPTR;
    }

    ret = ili9341_set_addr_window(scb, 0, 0, scb->width - 1, scb->height - 1);
    if (ret)
        goto end;

    ret = ili9341_write_color(scb, scb->height * scb->width, color);
    if (ret)
        goto end;

end:
    return ret;
}

int ili9341_invert(ili9341_scb_t *scb, bool invert)
{
    if (NULL == scb)
    {
        return ILI9341_E_NULLPTR;
    }

    char cmd = invert ? ILI9341_CMD_DISP_INV_ON : ILI9341_CMD_DISP_INV_OFF;
    return ili9341_write_cmd(scb, cmd, NULL, 0);
}

int ili9341_update_screen(ili9341_scb_t *scb)
{
    if (NULL == scb)
    {
        return ILI9341_E_NULLPTR;
    }

    return ili9341_write_cmd(scb, ILI9341_CMD_DISPLAY_ON, NULL, 0);
}

int ili9341_write_color(ili9341_scb_t *scb, uint32_t count, uint16_t color)
{
    int ret = 0;
    if (NULL == scb)
    {
        return ILI9341_E_NULLPTR;
    }

    size_t pixel_count = count * sizeof(uint16_t);
    // clamp cache if setting less pixels
    size_t pixel_cache_size = scb->cache_size / sizeof(uint16_t);
    pixel_cache_size = (pixel_cache_size > count) ? count : pixel_cache_size;

    // if spi supports dff to 16 bit mode, dont byte swap
    uint16_t color_data = (NULL == scb->f_write16) ? (color >> 8) | (color << 8) : color;

    for (uint32_t i = 0; i < pixel_cache_size; ++i)
        ((uint16_t *)scb->cache_ptr)[i] = color_data;

    ILI9341_START_TX(scb);
    ret = ili9341_cmd(scb, ILI9341_CMD_MEMWR);
    if (ret)
        goto end;

    while (pixel_count > 0 && !ret)
    {
        uint32_t len = (pixel_count > pixel_cache_size) ? pixel_cache_size : pixel_count;

        // prefer 16 bit mode available
        if (NULL != scb->f_write16)
            ret = scb->f_write16((uint16_t *)scb->cache_ptr, len, scb->user_ctx);
        else
            ret = scb->f_write(scb->cache_ptr, len, scb->user_ctx);

        ret = ret ? ILI9341_E_COMM_FAIL : ILI9341_E_OK;
        if (ret)
            goto end;
        pixel_count -= len;
    }

end:
    ILI9341_END_TX(scb);
    return ret;
}

int ili9341_write_pixels(ili9341_scb_t *scb, const uint16_t *pixels, uint32_t pcnt, bool le)
{
    int ret = 0;
    if (NULL == scb)
    {
        return ILI9341_E_NULLPTR;
    }

    ILI9341_START_TX(scb);
    ret = ili9341_cmd(scb, ILI9341_CMD_MEMWR);
    if (ret)
        goto end;

    // clamp cache if setting less pixels
    size_t pixel_cache_size = scb->cache_size / sizeof(uint16_t);
    pixel_cache_size = (pixel_cache_size > pcnt) ? pcnt : pixel_cache_size;

    if (le && NULL == scb->f_write16)
    {
        // if spi does not support dff to 16 bit mode and
        // if pixel data is little-endian, we need to do byte swap
        while (pcnt > 0 && !ret)
        {
            uint32_t len = (pcnt > pixel_cache_size) ? pixel_cache_size : pcnt;
            for (uint32_t i = 0; i < len; ++i)
            {
                ((uint16_t *)scb->cache_ptr)[i] = (*pixels >> 8) | (*pixels << 8);
                pixels++;
            }

            ret = scb->f_write((uint8_t *)scb->cache_ptr, len * 2, scb->user_ctx);
            if (ret)
                goto end;

            pcnt -= len;
        }
    }
    else
    {

        // prefer 16 bit mode available
        if (NULL != scb->f_write16)
            ret = scb->f_write16(pixels, pcnt, scb->user_ctx);
        else
            ret = scb->f_write((uint8_t*) pixels, pcnt * 2, scb->user_ctx);
        if (ret)
            goto end;
    }

end:
    ILI9341_END_TX(scb);
    return ret;
}

int ili9341_set_addr_window(ili9341_scb_t *scb, int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
    int ret = 0;
    uint32_t buf;
    if (NULL == scb)
    {
        return ILI9341_E_NULLPTR;
    }

    buf = ((x2 >> 8) | (x2 << 8)) << 16 | (x1 >> 8) | (x1 << 8);
    ret = ili9341_write_cmd(scb, ILI9341_CMD_COL_ADDR, (uint8_t *)&buf, sizeof(uint32_t));
    if (ret)
        goto end;

    buf = ((y2 >> 8) | (y2 << 8)) << 16 | (y1 >> 8) | (y1 << 8);
    ret = ili9341_write_cmd(scb, ILI9341_CMD_PAGE_ADDR, (uint8_t *)&buf, sizeof(uint32_t));
    if (ret)
        goto end;

end:
    return ret;
}

int ili9341_draw_pixel(ili9341_scb_t *scb, int16_t x1, int16_t y1, uint16_t color)
{
    int ret = 0;
    if (NULL == scb)
    {
        return ILI9341_E_NULLPTR;
    }

    ret = ili9341_set_addr_window(scb, x1, y1, x1, y1);
    if (ret)
        goto end;

    // send single color data
    ILI9341_START_TX(scb);
    ret = ili9341_cmd(scb, ILI9341_CMD_MEMWR);
    if (ret)
        goto end;

    *((uint16_t *)scb->cache_ptr) = (color >> 8) | (color << 8);
    ret = scb->f_write(scb->cache_ptr, sizeof(uint16_t), scb->user_ctx);
    if (ret)
        goto end;

end:
    ILI9341_END_TX(scb);
    return ret;
}

int ili9341_draw_line(ili9341_scb_t *scb, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
    int ret = 0;
    int16_t dx, dy, sx, sy, e2, error;
    if (NULL == scb)
    {
        return ILI9341_E_NULLPTR;
    }

    // bresenham, all hail wikipedia
    dx = ((int16_t)x2) - x1;
    sx = (x1 < x2) ? 1 : -1;
    dx = dx < 0 ? -dx : dx;
    dy = ((int16_t)y2) - y1;
    sy = (y1 < y2) ? 1 : -1;
    dy = dy < 0 ? dy : -dy;
    error = dy + dx;

    while (!ret && x1 != x2 && y1 != y2)
    {
        e2 = 2 * error;
        ret = ili9341_draw_pixel(scb, x1, y1, color);
        if (ret)
            goto end;

        if (e2 >= dy)
        {
            error += dy;
            x1 += sx;
        }
        if (e2 <= dx)
        {
            error += dx;
            y1 += sy;
        }
    }

end:
    return ret;
}

int ili9341_draw_bitmap(ili9341_scb_t *scb, int16_t x, int16_t y, int16_t w, int16_t h, const uint16_t *colors, bool le)
{
    int ret = 0;
    if (NULL == scb)
    {
        return ILI9341_E_NULLPTR;
    }
    else if (0 > y || y + h > scb->height || 0 > x || x + w > scb->width)
        return ILI9341_E_OUT_OF_BOUNDS;

    ret = ili9341_set_addr_window(scb, x, y, x + w - 1, y + h - 1);
    if (ret)
        goto end;

    ret = ili9341_write_pixels(scb, colors, w * h, le);
    if (ret)
        goto end;

end:
    return ret;
}

int ili9341_fdraw_hline(ili9341_scb_t *scb, int16_t x, int16_t y, int16_t l, uint16_t color)
{
    int ret = 0;
    int16_t x2;

    if (NULL == scb)
        return ILI9341_E_NULLPTR;
    if (0 > y || y > scb->height)
        return ILI9341_E_OUT_OF_BOUNDS;

    if (0 > l)
    {
        x2 = x;
        x += l + 1;
    }
    else
    {
        x2 = x + l;
    }

    // clamp to screen boundaries
    x = (0 > x) ? 0 : x;
    x2 = (x2 > scb->width) ? scb->width : x2;
    ret = ili9341_set_addr_window(scb, x, y, x2, y);
    if (ret)
        goto end;

    ret = ili9341_write_color(scb, x2 - x, color);
    if (ret)
        goto end;

end:
    return ret;
}

int ili9341_fdraw_vline(ili9341_scb_t *scb, int16_t x, int16_t y, int16_t l, uint16_t color)
{
    int ret = 0;
    int16_t y2;

    if (NULL == scb)
        return ILI9341_E_NULLPTR;
    if (0 > x || x > scb->width)
        return ILI9341_E_OUT_OF_BOUNDS;

    if (0 > l)
    {
        y2 = y;
        y += l + 1;
    }
    else
    {
        y2 = y + l;
    }

    // clamp to screen boundaries
    y = (0 > y) ? 0 : y;
    y2 = (y2 > scb->width) ? scb->width : y2;
    ret = ili9341_set_addr_window(scb, x, y, x, y2);
    if (ret)
        goto end;

    ret = ili9341_write_color(scb, y2 - y, color);
    if (ret)
        goto end;

end:
    return ret;
}

int ili9341_fdraw_rect(ili9341_scb_t *scb, int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    int ret = 0;
    if (NULL == scb)
    {
        return ILI9341_E_NULLPTR;
    }

    int16_t x1 = (w > 0) ? x : x + w + 1;
    int16_t x2 = (w > 0) ? x + w : x;
    int16_t y1 = (h > 0) ? y : y + h + 1;
    int16_t y2 = (h > 0) ? y + h : y;

    // clamp to screen boundaries
    x1 = (0 > x1) ? 0 : x1;
    x2 = (x2 > scb->width) ? scb->width : x2;
    y1 = (0 > y1) ? 0 : y1;
    y2 = (y2 > scb->height) ? scb->height : y2;

    ret = ili9341_fdraw_hline(scb, x1, y1, (x2 - x1), color);
    if (ret)
        goto end;
    ret = ili9341_fdraw_vline(scb, x1, y1, (y2 - y1), color);
    if (ret)
        goto end;
    ret = ili9341_fdraw_hline(scb, x1, y2, (x2 - x1), color);
    if (ret)
        goto end;
    ret = ili9341_fdraw_vline(scb, x2, y1, (y2 - y1), color);
    if (ret)
        goto end;

end:
    return (ret);
}

int ili9341_fdraw_filled_rect(ili9341_scb_t *scb, int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    int ret = 0;
    if (NULL == scb)
    {
        return ILI9341_E_NULLPTR;
    }

    int16_t x1 = (w > 0) ? x : x + w + 1;
    int16_t x2 = (w > 0) ? x + w : x;
    int16_t y1 = (h > 0) ? y : y + h + 1;
    int16_t y2 = (h > 0) ? y + h : y;

    // clamp to screen boundaries
    x1 = (0 > x1) ? 0 : x1;
    x2 = (x2 > scb->width) ? scb->width : x2;
    y1 = (0 > y1) ? 0 : y1;
    y2 = (y2 > scb->height) ? scb->height : y2;

    ret = ili9341_set_addr_window(scb, x1, y1, x2, y2);
    if (ret)
        goto end;

    ret = ili9341_write_color(scb, (x2 - x1) * (y2 - y1), color);
    if (ret)
        goto end;

end:
    return ret;
}
