
cmake_minimum_required(VERSION 3.18)

include(${CMAKE_SOURCE_DIR}/cmake/boards/esp32.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/utilities.cmake)

add_executable(esp32_rest_server
    ${CMAKE_CURRENT_LIST_DIR}/main.c)

target_include_directories(esp32_rest_server
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/)

target_link_libraries(esp32_rest_server
    idf::newlib
    idf::freertos
    idf::driver
    idf::spi_flash
    idf::esp_wifi
    idf::nvs_flash
    idf::esp_http_server
    idf::esp_netif
)

target_embed_file(esp32_rest_server CMakeLists.txt)

# Call board post process functions
BOARD_POST_PROCESS(esp32_rest_server)
