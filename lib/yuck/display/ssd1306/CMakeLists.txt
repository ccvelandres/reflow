
add_library(yuck_display_ssd1306_lib OBJECT EXCLUDE_FROM_ALL
    ${CMAKE_CURRENT_SOURCE_DIR}/ssd1306.c)

target_include_directories(yuck_display_ssd1306_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(yuck_display_ssd1306_lib PUBLIC
    yuck::common
    )

add_library(yuck::display::ssd1306 ALIAS yuck_display_ssd1306_lib)