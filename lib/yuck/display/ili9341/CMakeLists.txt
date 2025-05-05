
add_library(yuck_display_ili9341_lib OBJECT EXCLUDE_FROM_ALL
    ${CMAKE_CURRENT_SOURCE_DIR}/ili9341.c)

target_include_directories(yuck_display_ili9341_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(yuck_display_ili9341_lib PUBLIC
    yuck::yuck
    )

add_library(yuck::display::ili9341 ALIAS yuck_display_ili9341_lib)