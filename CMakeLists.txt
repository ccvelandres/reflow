
cmake_minimum_required(VERSION 3.18)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(reflow)
include(FetchContent)

## stm32f4
include(boards/libopencm3_stmf4.cmake)

set(FREERTOS_EXTRA_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/src/include)
include(boards/freertos_cm4.cmake)

set(SEGGER_RTT_EXTRA_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/src/include)
set(SEGGER_RTT_EXTRA_LIBRARIES libopencm3)
include(boards/segger_rtt.cmake)

#########################################################################
#########################################################################
# main app

set(PROJECT_LINKER_FILE ${CMAKE_CURRENT_LIST_DIR}/ld/stm32f411ce.ld)

set(PROJECT_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/src/main.c
    ${CMAKE_CURRENT_LIST_DIR}/src/freertos_hooks.c

    # ili9341
    ${CMAKE_CURRENT_LIST_DIR}/lib/ili9341/ili9341.c
)

set(PROJECT_INCLUDE_DIRS
    ${CMAKE_CURRENT_LIST_DIR}/src

    # ili9341
    ${CMAKE_CURRENT_LIST_DIR}/lib/ili9341
)

set(PROJECT_LIBRARIES
    libopencm3
    freertos_kernel_cm4
    segger_rtt
)

# Create project target
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
target_link_libraries(${PROJECT_NAME} ${PROJECT_LIBRARIES})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_INCLUDE_DIRS})
target_link_options(${PROJECT_NAME} PRIVATE
    -T${PROJECT_LINKER_FILE}
    -Wl,-print-memory-usage
    -Wl,-Map=$<TARGET_FILE_DIR:${PROJECT_NAME}>/$<TARGET_FILE_BASE_NAME:${PROJECT_NAME}>.map)

# Generate bin, hex, disasm files
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}> $<TARGET_FILE_DIR:${PROJECT_NAME}>/$<TARGET_FILE_BASE_NAME:${PROJECT_NAME}>.bin
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${PROJECT_NAME}> $<TARGET_FILE_DIR:${PROJECT_NAME}>/$<TARGET_FILE_BASE_NAME:${PROJECT_NAME}>.hex
    COMMAND ${CMAKE_OBJDUMP} -D $<TARGET_FILE:${PROJECT_NAME}> > $<TARGET_FILE_DIR:${PROJECT_NAME}>/$<TARGET_FILE_BASE_NAME:${PROJECT_NAME}>.asm)
