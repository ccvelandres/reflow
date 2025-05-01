
include($ENV{IDF_PATH}/tools/cmake/idf.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/boards.cmake)

set(IDF_TARGET esp32 CACHE STRING "")

add_compile_definitions(TARGET=${IDF_TARGET} ESP32)
include($ENV{IDF_PATH}/tools/cmake/idf.cmake)

idf_build_process(${IDF_TARGET} 
    SDKCONFIG_DEFAULTS ${CMAKE_CURRENT_LIST_DIR}/sdkconfig.defaults)

function(__BOARD_POST_PROCESS__ target)
    message(STATUS "esp32 board post process called on ${target}")
    idf_build_executable(${target})

    add_custom_target(flash_app
        COMMAND esptool.py --port /dev/ttyUSB0 write_flash @flash_app_args
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endfunction()

set(__BOARD_POST_PROCESS __BOARD_POST_PROCESS__ CACHE STRING "")