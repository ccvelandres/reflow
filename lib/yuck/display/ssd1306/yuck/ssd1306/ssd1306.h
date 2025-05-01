// SPDX-License-Identifier: MIT

#ifndef __DISPLAY_SSD1306_H__
#define __DISPLAY_SSD1306_H__

/**
 * @file ssd1306.h
 * @author Cedric Velandres (ccvelandres@gmail.com)
 */

#ifdef __cplusplus
extern "C"
{
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define SSD1306_I2C_ADDRESS                 (0x3C)
#define SSD1306_I2C_CB_CONTINUE             (1 << 7)
#define SSD1306_I2C_CB_COMMAND              (0 << 6)
#define SSD1306_I2C_CB_DATA                 (1 << 6)

/*! @name SSD1306 Charge Pump Commands */
#define SSD1306_CMD_CHARGE_PUMP_CFG         (0x8D) // Charge Pump Setting

/*! @name SSD1306 Fundamental Commands */
#define SSD1306_CMD_CONTRAST_CTRL           (0x81) // Set Contrast Control
#define SSD1306_CMD_DISPLAY_RAM             (0xA4) // Entire Display ON follow RAM
#define SSD1306_CMD_DISPLAY_FORCE           (0xA5) // Force Entire Display ON ignore RAM
#define SSD1306_CMD_DISP_NORMAL             (0xA6) // Set Normal Display
#define SSD1306_CMD_DISP_INVERSE            (0xA7) // Set Inverse Display
#define SSD1306_CMD_DISPLAY_OFF             (0xAE) // Set Display OFF
#define SSD1306_CMD_DISPLAY_ON              (0xAF) // Set Display ON

/*! @name SSD1306 Scrolling Commands */
#define SSD1306_CMD_HOR_SCROLL_RIGHT        (0x26) // Horizontal Scroll Setup (Right)
#define SSD1306_CMD_HOR_SCROLL_LEFT         (0x27) // Horizontal Scroll Setup (Left)
#define SSD1306_CMD_VERT_HOR_SCROLL_RIGHT   (0x29) // Continuous Vertical and Horizontal Scroll Setup (Diagonal Right)
#define SSD1306_CMD_VERT_HOR_SCROLL_LEFT    (0x2A) // Continuous Vertical and Horizontal Scroll Setup (Diagonal Left)
#define SSD1306_CMD_SCROLL_ENABLE           (0x2E) // Deactivate Scroll
#define SSD1306_CMD_SCROLL_DISABLE          (0x2F) // Activate Scroll
#define SSD1306_CMD_VERT_SCROLL_AREA        (0xA3) // Set Vertical Scroll Area

/*! @name SSD1306 Addressing Setting Commands */
#define SSD1306_CMD_LCOLADDR                (0x00) // Set Lower Column Start Address for Page Addressing Mode
#define SSD1306_CMD_HCOLADDR                (0x10) // Set Higher Column Start Address for Page Addressing Mode
#define SSD1306_CMD_MEMADDR_MODE            (0x20) // Set Memory Addressing Mode
#define SSD1306_CMD_COL_ADDR                (0x21) // Set Column Address
#define SSD1306_CMD_PAGE_ADDR               (0x22) // Set Page Address

/*! @name SSD1306 Hardware Configuration Commands */
#define SSD1306_CMD_START_LINE              (0x40) // Set display RAM display start line register from 0-63
#define SSD1306_CMD_SEG_REMAP               (0xA0) // Set Segment Re-map (col addr 0 mapped to SEG0)
#define SSD1306_CMD_SEG_REMAP_127           (0xA1) // Set Segment Re-map (col addr 127 mapped to SEG0)
#define SSD1306_CMD_MULTIPLEX_RATIO         (0xA8) // Set Multiplex Ratio
#define SSD1306_CMD_SCANDIR_POS             (0xC0) // Set COM Output Scan Direction (Positive)
#define SSD1306_CMD_SCANDIR_NEG             (0xC8) // Set COM Output Scan Direction (Negative)
#define SSD1306_CMD_DISP_OFFSET             (0xD3) // Set Display Offset (D3h)
#define SSD1306_CMD_COMPIN_HWCONF           (0xDA) // Set COM Pins Hardware Configuration

/*! @name SSD1306 Timing and Driving Scheme Setting Commands */
#define SSD1306_CMD_DISPCLOCKDIV            (0xD5) // Set Display Clock Divide Ratio/ Oscillator Frequency
#define SSD1306_CMD_PRECHARGE               (0xD9) // Set Pre-charge Period
#define SSD1306_CMD_VCOMLEVEL               (0xDB) // Set VCOMH Deselect Level
#define SSD1306_CMD_NOP                     (0xE3) // NOP

#define SSD1306_COLOR_BLACK                 (0x0000)
#define SSD1306_COLOR_WHITE                 (0xFFFF)

#define SSD1306_FRAMEBUFFER_SIZE            ((128 * 8) + 2)

typedef struct ssd1306_scb ssd1306_scb_t;

typedef void (*ssd1306_delay_fptr_t)(uint32_t ms);
typedef int (*ssd1306_write_fptr_t)(const uint8_t *data, size_t len, bool lfen, bool lfstop, void *user_ctx);
typedef int (*ssd1306_read_fptr_t)(uint8_t *data, size_t len, bool lfen, bool lfstop, void *user_ctx);

struct ssd1306_scb
{
    uint16_t height;
    uint16_t width;

    uint8_t framebuf[SSD1306_FRAMEBUFFER_SIZE];

    void *user_ctx;
    ssd1306_delay_fptr_t f_delay;
    ssd1306_write_fptr_t f_write;
    ssd1306_read_fptr_t f_read;
};

int ssd1306_init(ssd1306_scb_t *scb);
int ssd1306_clear(ssd1306_scb_t *scb, uint16_t color);
int ssd1306_invert(ssd1306_scb_t* scb, bool invert);
int ssd1306_update_screen(ssd1306_scb_t *scb);

int ssd1306_draw_pixel(ssd1306_scb_t *scb, int16_t x1, int16_t y1, uint16_t color);
int ssd1306_draw_line(ssd1306_scb_t *scb, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
int ssd1306_draw_bitmap(ssd1306_scb_t *scb, int16_t x, int16_t y, int16_t w, int16_t h, const uint16_t *colors, bool le);

#ifdef __cplusplus
}
#endif

#endif