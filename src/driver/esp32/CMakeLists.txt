

add_library(driver_esp32 OBJECT
    ${CMAKE_CURRENT_SOURCE_DIR}/init.c
    ${CMAKE_CURRENT_SOURCE_DIR}/spi.c
    ${CMAKE_CURRENT_SOURCE_DIR}/i2c.c
    ${CMAKE_CURRENT_SOURCE_DIR}/gpio.c)

target_include_directories(driver_esp32 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..)

target_link_libraries(driver_esp32 PUBLIC
    idf::newlib
    idf::freertos
    idf::driver
    idf::spi_flash)

add_library(driver ALIAS driver_esp32)