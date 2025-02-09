#pragma once

#include <stdint.h>

void uart_init(void);
uint32_t uart_write(const char *ptr, uint32_t len);
uint32_t uart_read(char *ptr, uint32_t buflen);