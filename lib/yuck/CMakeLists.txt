
# Yet another collection (of libraries) (YAC --- YUCK)
# The only motivation for this a completely platform-agnostic library

if (NOT YUCK_EXTRA_INCLUDE_DIR)
    set(YUCK_EXTRA_INCLUDE_DIR )
endif()

## Common library
add_library(yuck_common OBJECT
    ${CMAKE_CURRENT_SOURCE_DIR}/common/list.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/scheduler.c)
target_include_directories(yuck_common PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/common
    ${YUCK_EXTRA_INCLUDE_DIR})
add_library(yuck::yuck ALIAS yuck_common)

## Sub libraries

### Display libraries
add_subdirectory(display/ili9341)
add_subdirectory(display/ssd1306)

### Motor Drivers
add_subdirectory(motor/drv8825)
