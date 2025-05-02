
# Yet another collection (of libraries) (YAC --- YUCK)
# The only motivation for this a completely platform-agnostic library

## Build dependencies
include(FetchContent)
# include(${CMAKE_CURRENT_LIST_DIR}/deps/kconfig-cmake/Kconfig.cmake)

# add_custom_target(__kconfig_bin
#     # OUTPUT
#     #     ${CMAKE_CURRENT_LIST_DIR}/bin/conf
#     #     ${CMAKE_CURRENT_LIST_DIR}/bin/mconf
#     COMMAND ${CMAKE_COMMAND}
#         -DOUTPUT_DIR=${CMAKE_CURRENT_LIST_DIR}/bin
#         -DBUILD_DIR=${CMAKE_BINARY_DIR}/kbuild
#         -P ${CMAKE_CURRENT_LIST_DIR}/deps/kconfig-cmake/kconfig-build.cmake
#     WORKING_DIRECTORY ${CMAKE_BINARY_DIR})


# find_package(Curses REQUIRED)

include(FetchContent)
FetchContent_Declare(
    kbuild_standalone
    URL https://github.com/WangNan0/kbuild-standalone/archive/refs/tags/v6.9.tar.gz
    SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/kconfiglib-src
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND ""
)
FetchContent_MakeAvailable(kbuild_standalone)
FetchContent_GetProperties(kbuild_standalone)


# add_custom_target(__kconfig_bin
#     # OUTPUT
#     #     ${CMAKE_CURRENT_LIST_DIR}/bin/conf
#     #     ${CMAKE_CURRENT_LIST_DIR}/bin/mconf
#     COMMAND ${CMAKE_COMMAND}
#         -DOUTPUT_DIR=${CMAKE_CURRENT_LIST_DIR}/bin
#         -DBUILD_DIR=${CMAKE_BINARY_DIR}/kbuild
#         -P ${CMAKE_CURRENT_LIST_DIR}/deps/kconfig-cmake/kconfig-build.cmake
#     WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
#     DEPENDS kbuild_standalone)


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
