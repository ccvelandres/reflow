
cmake_minimum_required(VERSION 3.18)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(PROJECT_NAME reflow)
project(${PROJECT_NAME})
include(FetchContent)

## stm32f4
# set(PROJECT_LINKER_FILE ${CMAKE_CURRENT_LIST_DIR}/ld/stm32f411ce.ld)
# include(boards/boards.cmake)

# set(FREERTOS_EXTRA_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/src/include)
# include(boards/freertos_cm4.cmake)

# set(SEGGER_RTT_EXTRA_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/src/include)
# set(SEGGER_RTT_EXTRA_LIBRARIES libopencm3)
# include(boards/segger_rtt.cmake)

# set(BOARD_LIBRARIES libopencm3 freertos_kernel_cm4 segger_rtt)

# add_subdirectory(${CMAKE_SOURCE_DIR}/lib/yuck)

#########################################################################
#########################################################################
# main app

# add_subdirectory(${CMAKE_SOURCE_DIR}/examples)
# add_subdirectory(src)

set(BOARD_LIBRARIES )
include(${CMAKE_SOURCE_DIR}/cmake/boards/esp32.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/utilities.cmake)
add_subdirectory(src/driver/esp32)
include_directories(board/esp32)

set(PROJECT_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/src/main.c
    ${CMAKE_CURRENT_LIST_DIR}/src/spi_display.c
    ${CMAKE_CURRENT_LIST_DIR}/src/i2c_display.c
    ${CMAKE_CURRENT_LIST_DIR}/src/aglaea_bmp.c


    ${CMAKE_CURRENT_LIST_DIR}/src/board/esp32/yuck_common.c
)

set(PROJECT_INCLUDE_DIRS
    ${CMAKE_CURRENT_LIST_DIR}/src/
    ${CMAKE_CURRENT_LIST_DIR}/src/include
    ${CMAKE_CURRENT_LIST_DIR}/src/include/driver
)

set(PROJECT_LIBRARIES
    ${BOARD_LIBRARIES} driver
    # yuck::display::ili9341
    # yuck::display::ssd1306
    # yuck::motor::drv8825
)

# Create project target
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
target_compile_options(${PROJECT_NAME} PUBLIC -Og -O0)
target_link_libraries(${PROJECT_NAME} ${PROJECT_LIBRARIES})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_INCLUDE_DIRS})

# Call board post process functions
BOARD_POST_PROCESS(${PROJECT_NAME})