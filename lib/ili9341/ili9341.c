// SPDX-License-Identifier: MIT

#include <ili9341.h>

/**
 * @file ili9341.cpp
 * @author Cedric Velandres (ccvelandres@gmail.com)
 */

static const uint8_t ili9341_init_cmds[] = 
{
    ILI9341_CMD_SWRST, 50, 0,
    ILI9341_CMD_DISPLAY_OFF, 2, 0,

    0xEF, 0, 3, 0x03, 0x80, 0x02,
    ILI9341_CMD_PWRCTRLB, 0, 3, 0x00, 0xC1, 0x30,
    ILI9341_CMD_PWRSEQCTRL, 0, 4, 0x64, 0x03, 0x12, 0x81,
    ILI9341_CMD_DTCTRLA, 0, 3, 0x85, 0x00, 0x78,
    ILI9341_CMD_PWRCTRLA, 0, 5, 0x39, 0x2C, 0x00, 0x34, 0x02,
    ILI9341_CMD_PUMPRCTRL, 0, 1, 0x20,
    ILI9341_CMD_DTCTRLB, 0, 2, 0x00, 0x00,

    // Power control
    ILI9341_CMD_PWCTRL1, 0, 1, 0x23,
    ILI9341_CMD_PWCTRL2, 0, 1, 0x10,
    ILI9341_CMD_VCCTRL1, 0, 2, 0x3e, 0x28,
    ILI9341_CMD_VCCTRL2, 0, 1, 0x86,
    ILI9341_CMD_MADCTL, 0, 1, 0x48,
    
    // Gamma
    ILI9341_CMD_EN3G, 0, 1, 00,
    ILI9341_CMD_GAMMA_SET, 0, 1, 0x01,
    ILI9341_CMD_PGMCRP, 0, 15, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00,
    ILI9341_CMD_NGMCRP, 0, 15, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F,

    // reset scroll, pixel format
    ILI9341_CMD_VSCRADDR, 0, 1, 0x00,
    ILI9341_CMD_PIXFMT, 0, 1, 0x55,
    ILI9341_CMD_FRMCTRL1, 0, 2, 0x00, 0x18,
    ILI9341_CMD_DISPFUNC, 0, 3, 0x08, 0x82, 0x27,

    ILI9341_CMD_SLEEP_OUT, 150, 0,
    ILI9341_CMD_DISPLAY_ON, 150, 0,
};

#define ILI9341_START_TX(scb)   {if (NULL != scb->f_cs) scb->f_cs(true, scb->user_ctx);}
#define ILI9341_END_TX(scb)     {if (NULL != scb->f_cs) scb->f_cs(false, scb->user_ctx);}

#define ILI9341_START_DC(scb)   {if (NULL != scb->f_dc) scb->f_dc(false, scb->user_ctx);}
#define ILI9341_END_DC(scb)     {if (NULL != scb->f_dc) scb->f_dc(true, scb->user_ctx);}

static int ili9341_send_cmd(ili9341_scb_t* scb, uint8_t cmd)
{
    int ret;
    ILI9341_START_DC(scb);
    ret = scb->f_write(&cmd, 1, scb->user_ctx);
    if (ret) return ILI9341_E_COMM_FAIL;
    ILI9341_END_DC(scb);
    return ret;
}

static int ili9341_write(ili9341_scb_t *scb, const uint8_t *data, size_t len)
{
    int ret;
    ret = scb->f_write(data, len, scb->user_ctx);
    if (ret) return ILI9341_E_COMM_FAIL;
    return ret;
}

// static int ili9341_read(ili9341_scb_t *scb, uint8_t *data, size_t *len)
// {
//     int ret;
//     ret = scb->f_read(data, len, scb->user_ctx);
//     return ret;
// }

int ili9341_init(ili9341_scb_t* scb)
{
    int ret = 0;
    if (NULL == scb)
    {
        return ILI9341_E_NULLPTR;
    }

    const uint8_t* addr = ili9341_init_cmds;
    const uint8_t* cmd_end = ili9341_init_cmds + sizeof(ili9341_init_cmds);
    while((cmd_end > addr) && !ret)
    {
        uint8_t cmd = *addr++;
        uint8_t delay = *addr++;
        uint8_t argc = *addr++;
        const uint8_t *argv = addr;
        if (argc) addr += argc;

        ILI9341_START_TX(scb);
        
        ret = ili9341_send_cmd(scb, cmd);
        ret |= ili9341_write(scb, argv, argc);
        
        ILI9341_END_TX(scb);
        scb->f_delay(delay);
    }

    return ret;
}

int ili9341_clear(ili9341_scb_t* scb, uint16_t color)
{
    int ret = 0;
    if (NULL == scb)
    {
        return ILI9341_E_NULLPTR;
    }

    ILI9341_START_TX(scb);
    ret = ili9341_send_cmd(scb, ILI9341_CMD_MEMWR);

    // send by cache size
    uint32_t pixel_count = scb->height * scb->width;

    while(pixel_count && !ret)
    {
        uint32_t len = (pixel_count > scb->cache_size) ? scb->cache_size : pixel_count;
        for (uint32_t i = 0; i < len; ++len)
            scb->cache[i] = color;

        ret = ili9341_write(scb, scb->cache, len);
    }

    ILI9341_END_TX(scb);
    return ret;
}

int ili9341_set_addr_window(ili9341_scb_t *scb, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    int ret = 0;
    if (NULL == scb)
    {
        return ILI9341_E_NULLPTR;
    }
    
    ILI9341_START_TX(scb);
    ret |= ili9341_send_cmd(scb, ILI9341_CMD_COL_ADDR) ||
          ili9341_write(scb, (uint8_t*) &x1, sizeof(x1)) ||
          ili9341_write(scb, (uint8_t*) &x1, sizeof(x2));
    ILI9341_END_TX(scb);

    ILI9341_START_TX(scb);
    ret |= ili9341_send_cmd(scb, ILI9341_CMD_PAGE_ADDR) ||
          ili9341_write(scb, (uint8_t*) &y1, sizeof(y1)) ||
          ili9341_write(scb, (uint8_t*) &y1, sizeof(y2));
    ILI9341_END_TX(scb);

    return ret;
}