
# Optional extra arguments
# SEGGER_RTT_EXTRA_SOURCES
# SEGGER_RTT_EXTRA_INCLUDE_DIR
# SEGGER_RTT_EXTRA_LIBRARIES

#########################################################################
# Segger RTT Library
FetchContent_Declare(segger_rtt
    URL https://github.com/SEGGERMicro/RTT/archive/refs/tags/V7.54.tar.gz
    SOURCE_DIR "${CMAKE_BINARY_DIR}/lib/segger_rtt"
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND "")
FetchContent_MakeAvailable(segger_rtt)
FetchContent_GetProperties(segger_rtt)

add_library(segger_rtt STATIC
    ${segger_rtt_SOURCE_DIR}/RTT/SEGGER_RTT.c
    ${segger_rtt_SOURCE_DIR}/RTT/SEGGER_RTT_printf.c
    ${segger_rtt_SOURCE_DIR}/Syscalls/SEGGER_RTT_Syscalls_GCC.c
    ${SEGGER_RTT_EXTRA_SOURCES})

target_include_directories(segger_rtt PUBLIC
    ${segger_rtt_SOURCE_DIR}/RTT
    ${SEGGER_RTT_EXTRA_INCLUDE_DIR})

    target_link_libraries(segger_rtt PRIVATE
    ${SEGGER_RTT_EXTRA_LIBRARIES})
