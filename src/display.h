#pragma once

#include <yuck/ili9341/ili9341.h>

extern ili9341_scb_t *ili9341_scb;

int init_spi_display();


int init_i2c_display();