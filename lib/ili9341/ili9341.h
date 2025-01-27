// SPDX-License-Identifier: MIT

#ifndef __ILI9341__
#define __ILI9341__

/**
 * @file ili9341.hpp
 * @author Cedric Velandres (ccvelandres@gmail.com)
 */

#ifdef __cplusplus
extern "C"
{
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/*! @name ILI9341 API Error Codes */
#define ILI9341_E_OK                    (0x00)
#define ILI9341_E_NULLPTR               (0x01)
#define ILI9341_E_COMM_FAIL             (0x02)
#define ILI9341_E_INVALID_ARG           (0x03)

/*! @name ILI9341 Level 1 Command definitions */
#define ILI9341_CMD_NOP                 (0x00) // No Operation
#define ILI9341_CMD_SWRST               (0x01) // Software Reset
#define ILI9341_CMD_RDDID               (0x04) // Read Display Identification Information
#define ILI9341_CMD_RDS                 (0x09) // Read Display Status
#define ILI9341_CMD_RDPM                (0x0A) // Read Display Power Mode
#define ILI9341_CMD_RDMADCTL            (0x0B) // Read Display MADCTL
#define ILI9341_CMD_RDPF                (0x0C) // Read Display Pixel Format
#define ILI9341_CMD_RDIF                (0x0D) // Read Display Image Format
#define ILI9341_CMD_RDSM                (0x0E) // Read Display Signal Mode
#define ILI9341_CMD_RDSDR               (0x0F) // Read Display Self-Diagnostic Result
#define ILI9341_CMD_SLEEP_IN            (0x10) // Enter Sleep Mode
#define ILI9341_CMD_SLEEP_OUT           (0x11) // Sleep Out
#define ILI9341_CMD_PARTIAL             (0x12) // Partial Mode On
#define ILI9341_CMD_NORMAL_DISP         (0x13) // Normal Display Mode On
#define ILI9341_CMD_DISP_INV_ON         (0x20) // Display Inversion Off
#define ILI9341_CMD_DISP_INV_OFF        (0x21) // Display Inversion On
#define ILI9341_CMD_GAMMA_SET           (0x26) // Gamma Set
#define ILI9341_CMD_DISPLAY_OFF         (0x28) // Display Off
#define ILI9341_CMD_DISPLAY_ON          (0x29) // Display On
#define ILI9341_CMD_COL_ADDR            (0x2A) // Column Address Set
#define ILI9341_CMD_PAGE_ADDR           (0x2B) // Page Address Set
#define ILI9341_CMD_MEMWR               (0x2C) // Memory Write
#define ILI9341_CMD_CLR_SET             (0x2D) // Color Set
#define ILI9341_CMD_MEM_READ            (0x2E) // Memory Read
#define ILI9341_CMD_PARTIAL_AREA        (0x30) // Partial Area
#define ILI9341_CMD_VSCRDEF             (0x33) // Vertical Scrolling Definition
#define ILI9341_CMD_TL_OFF              (0x34) // Tearing Effect Line Off
#define ILI9341_CMD_TL_ON               (0x35) // Tearing Effect Line On
#define ILI9341_CMD_MADCTL              (0x36) // Memory Access Control
#define ILI9341_CMD_VSCRADDR            (0x37) // Vertical Scrolling Start Address
#define ILI9341_CMD_IDLE_OFF            (0x38) // Idle Mode Off
#define ILI9341_CMD_IDLE_ON             (0x39) // Idle Mode On
#define ILI9341_CMD_PIXFMT              (0x3A) // Pixel Format Set
#define ILI9341_CMD_WMCON               (0x3C) // Write Memory Continue
#define ILI9341_CMD_RMCON               (0x3E) // Read Memory Continue
#define ILI9341_CMD_SET_TSL             (0x44) // Set Tear Scanline
#define ILI9341_CMD_GET_TSL             (0x45) // Get Scanline
#define ILI9341_CMD_WD_BRIGHT           (0x51) // Write Display Brightness
#define ILI9341_CMD_RD_BRIGHT           (0x52) // Read Display Brightness
#define ILI9341_CMD_WCTRLD              (0x53) // Write CTRL Display
#define ILI9341_CMD_RCTRLD              (0x54) // Read CTRL Display
#define ILI9341_CMD_WCADBRCTL           (0x55) // Write Content Adaptive Brightness Control
#define ILI9341_CMD_RCADBRCTL           (0x56) // Read Content Adaptive Brightness Control
#define ILI9341_CMD_WCABC_MIN           (0x5E) // Write CABC Minimum Brightness
#define ILI9341_CMD_RCABC_MIN           (0x5F) // Read CABC Minimum Brightness
#define ILI9341_CMD_READ_ID1            (0xDA) // Read Id1
#define ILI9341_CMD_READ_ID2            (0xDB) // Read Id2
#define ILI9341_CMD_READ_ID3            (0xDC) // Read Id3

/*! @name ILI9341 Level 2 Command definitions */
#define ILI9341_CMD_RGBIFSC          (0xB0) // RGB Interface Signal Control
#define ILI9341_CMD_FRMCTRL1         (0xB1) // Frame Control (In Normal Mode)
#define ILI9341_CMD_FRMCTRL2         (0xB2) // Frame Control (In Idle Mode)
#define ILI9341_CMD_FRMCTRL3         (0xB3) // Frame Control (In Partial Mode)
#define ILI9341_CMD_DISPINVC         (0xB4) // Display Inversion Control
#define ILI9341_CMD_BLKPC            (0xB5) // Blanking Porch Control
#define ILI9341_CMD_DISPFUNC         (0xB6) // Display Function Control
#define ILI9341_CMD_ENTRY_MODE       (0xB7) // Entry Mode Set
#define ILI9341_CMD_BLCTRL1          (0xB8) // Backlight Control 1
#define ILI9341_CMD_BLCTRL2          (0xB9) // Backlight Control 2
#define ILI9341_CMD_BLCTRL3          (0xBA) // Backlight Control 3
#define ILI9341_CMD_BLCTRL4          (0xBB) // Backlight Control 4
#define ILI9341_CMD_BLCTRL5          (0xBC) // Backlight Control 5
#define ILI9341_CMD_BLCTRL7          (0xBE) // Backlight Control 7
#define ILI9341_CMD_BLCTRL8          (0xBF) // Backlight Control 8
#define ILI9341_CMD_PWCTRL1          (0xC0) // Power Control 1
#define ILI9341_CMD_PWCTRL2          (0xC1) // Power Control 2
#define ILI9341_CMD_VCCTRL1          (0xC5) // VCOM Control 1
#define ILI9341_CMD_VCCTRL2          (0xC7) // VCOM Control 2
#define ILI9341_CMD_NV_MEMW          (0xD0) // NV Memory Write
#define ILI9341_CMD_NV_MEMPK         (0xD1) // NV Memory Protection Key
#define ILI9341_CMD_NV_MEMSR         (0xD2) // NV Memory Status Read
#define ILI9341_CMD_READ_ID4         (0xD3) // Read ID4
#define ILI9341_CMD_PGMCRP           (0xE0) // Positive Gamma Correction
#define ILI9341_CMD_NGMCRP           (0xE1) // Negative Gamma Correction
#define ILI9341_CMD_DIGGMCTRL1       (0xE2) // Digital Gamma Control 1
#define ILI9341_CMD_DIGGMCTRL2       (0xE3) // Digital Gamma Control 2
#define ILI9341_CMD_IFCTRL           (0xF6) // Interface Control

/*! @name ILI9341 Extended Command definitions */
#define ILI9341_CMD_PWRCTRLA      (0xCB) // Power control A
#define ILI9341_CMD_PWRCTRLB      (0xCF) // Power control B
#define ILI9341_CMD_DTCTRLA       (0xE8) // Driver timing control A
#define ILI9341_CMD_DTCTRLB       (0xEA) // Driver timing control B
#define ILI9341_CMD_PWRSEQCTRL    (0xED) // Power on sequence control
#define ILI9341_CMD_EN3G          (0xF2) // Enable 3 Gamma
#define ILI9341_CMD_PUMPRCTRL     (0xF7) // Pump ratio control

#define ILI9341_BLACK       (0x0000)
#define ILI9341_WHITE       (0xFFFF)

typedef struct _ili9341_scb_t ili9341_scb_t;

typedef void (*ili9341_delay_fptr_t)(uint32_t ms);
typedef void (*ili9341_cs_fptr_t)(bool state, void *user_ctx);
typedef void (*ili9341_dc_fptr_t)(bool state, void *user_ctx);
typedef void (*ili9341_rst_fptr_t)(bool state, void *user_ctx);
typedef int (*ili9341_read_fptr_t)(uint8_t *data, size_t len, void *user_ctx);
typedef int (*ili9341_write_fptr_t)(const uint8_t *data, size_t len, void *user_ctx);
typedef int (*ili9341_xfer_fptr_t)(const uint8_t *write, uint8_t *read, size_t len, void *user_ctx);

struct _ili9341_scb_t
{
    size_t height;
    size_t width;

    uint8_t *cache;
    size_t cache_size;

    // Interface data
    void *user_ctx;
    const ili9341_delay_fptr_t f_delay;
    const ili9341_cs_fptr_t f_cs;
    const ili9341_dc_fptr_t f_dc;
    const ili9341_rst_fptr_t f_rst;
    const ili9341_read_fptr_t f_read;
    const ili9341_write_fptr_t f_write;
    const ili9341_xfer_fptr_t f_xfer;
};

int ili9341_init(ili9341_scb_t* scb);
int ili9341_clear(ili9341_scb_t* scb, uint16_t color);

int ili9341_set_addr_window(ili9341_scb_t *scb, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);


#ifdef __cplusplus
}
#endif
#endif