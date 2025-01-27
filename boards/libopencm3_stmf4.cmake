
#########################################################################
# libopencm3
FetchContent_Declare(libopencm3
    URL https://github.com/libopencm3/libopencm3/archive/cb0661f81de5b1cae52ca99c7b5985b678176db7.tar.gz
    SOURCE_DIR "${CMAKE_BINARY_DIR}/lib/libopencm3"
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND ""
    )
FetchContent_MakeAvailable(libopencm3)
FetchContent_GetProperties(libopencm3)

add_custom_command(OUTPUT "${libopencm3_SOURCE_DIR}/lib/libopencm3_stm32f4.a"
    COMMAND make -j TARGETS=stm32/f4
    WORKING_DIRECTORY ${libopencm3_SOURCE_DIR})
add_custom_target(libopencm3_lib
    DEPENDS "${libopencm3_SOURCE_DIR}/lib/libopencm3_stm32f4.a")

# Setup library and compile flags for stm32f4
add_library(libopencm3_stm32f4 STATIC IMPORTED)
set_target_properties(libopencm3_stm32f4 PROPERTIES 
    IMPORTED_LOCATION "${libopencm3_SOURCE_DIR}/lib/libopencm3_stm32f4.a")
target_include_directories(libopencm3_stm32f4 INTERFACE "${libopencm3_SOURCE_DIR}/include")
target_link_directories(libopencm3_stm32f4 INTERFACE "${libopencm3_SOURCE_DIR}/lib")
target_compile_options(libopencm3_stm32f4 INTERFACE
     -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -MD -DSTM32F4 -nostartfiles
)
target_link_options(libopencm3_stm32f4 INTERFACE
     -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -MD -DSTM32F4 -nostartfiles
)
add_dependencies(libopencm3_stm32f4 libopencm3_lib)

add_library(libopencm3 ALIAS libopencm3_stm32f4)