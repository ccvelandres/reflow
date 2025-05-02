
add_library(yuck_server_rest_server_lib OBJECT EXCLUDE_FROM_ALL
    ${CMAKE_CURRENT_SOURCE_DIR}/drv8825.c)

target_include_directories(yuck_server_rest_server_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/../../common)

target_link_libraries(yuck_server_rest_server_lib PUBLIC
    yuck::common
    )

add_library(yuck::server::rest_server ALIAS yuck_server_rest_server_lib)