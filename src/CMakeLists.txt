
include(${CMAKE_SOURCE_DIR}/cmake/boards/esp32.cmake)

set(BOARD_LIBRARIES )
add_subdirectory(driver/esp32)
include_directories(board/esp32)

# Create project target
add_executable(reflow
    ${CMAKE_CURRENT_LIST_DIR}/main.c
    ${CMAKE_CURRENT_LIST_DIR}/spi_display.c
    ${CMAKE_CURRENT_LIST_DIR}/i2c_display.c
    ${CMAKE_CURRENT_LIST_DIR}/aglaea_bmp.c

    ${CMAKE_CURRENT_LIST_DIR}/board/esp32/yuck_common.c)

target_link_libraries(reflow
    ${BOARD_LIBRARIES} driver
    yuck::display::ili9341
    yuck::display::ssd1306
    yuck::motor::drv8825)


target_include_directories(reflow
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/include
        ${CMAKE_CURRENT_LIST_DIR}/include/driver)

# Call board post process functions
BOARD_POST_PROCESS(reflow)