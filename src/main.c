#include "init.h"

#include <display.h>

#if defined(ESP32)
void app_main(void)
#else
int main()
#endif
{
    init();
    init_i2c_display();
    // init_spi_display();
}