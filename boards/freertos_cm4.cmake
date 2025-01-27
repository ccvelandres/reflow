

#########################################################################
# FreeRTOS-Kernel
FetchContent_Declare(freertos_kernel
    URL https://github.com/FreeRTOS/FreeRTOS-Kernel/archive/refs/tags/V10.4.4.tar.gz
    SOURCE_DIR "${CMAKE_BINARY_DIR}/lib/freertos_kernel"
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND "")
FetchContent_MakeAvailable(freertos_kernel)
FetchContent_GetProperties(freertos_kernel)

# FreeRTOS doesnt have cmake scripts yet
add_library(freertos_kernel_cm4 STATIC
    ${freertos_kernel_SOURCE_DIR}/croutine.c
    ${freertos_kernel_SOURCE_DIR}/tasks.c
    ${freertos_kernel_SOURCE_DIR}/list.c
    ${freertos_kernel_SOURCE_DIR}/queue.c
    ${freertos_kernel_SOURCE_DIR}/event_groups.c
    ${freertos_kernel_SOURCE_DIR}/stream_buffer.c
    ${freertos_kernel_SOURCE_DIR}/timers.c
    ${freertos_kernel_SOURCE_DIR}/portable/GCC/ARM_CM4F/port.c
    ${freertos_kernel_SOURCE_DIR}/portable/MemMang/heap_4.c)
target_include_directories(freertos_kernel_cm4 PUBLIC
    ${freertos_kernel_SOURCE_DIR}/include
    ${freertos_kernel_SOURCE_DIR}/portable/GCC/ARM_CM4F)
target_include_directories(freertos_kernel_cm4 PRIVATE
    ${FREERTOS_EXTRA_INCLUDE_DIR})
target_link_libraries(freertos_kernel_cm4 PRIVATE libopencm3)
