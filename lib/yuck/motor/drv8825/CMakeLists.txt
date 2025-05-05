
add_library(yuck_motor_drv8825_lib OBJECT EXCLUDE_FROM_ALL
    ${CMAKE_CURRENT_SOURCE_DIR}/drv8825.c)

target_include_directories(yuck_motor_drv8825_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/../../common)

target_link_libraries(yuck_motor_drv8825_lib PUBLIC
    yuck::yuck
    )

add_library(yuck::motor::drv8825 ALIAS yuck_motor_drv8825_lib)