
# Yet another collection (of libraries) (YAC --- YUCK)
# The only motivation for this a completely platform-agnostic library

## Common library
add_library(common OBJECT
    ${CMAKE_CURRENT_SOURCE_DIR}/common/list.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/scheduler.c)
target_include_directories(common PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/common)
add_library(yuck::common ALIAS common)


## Sub libraries

### Display libraries
add_subdirectory(display/ili9341)
add_subdirectory(display/ssd1306)

### Motor Drivers
add_subdirectory(motor/drv8825)
