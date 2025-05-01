
if (${CMAKE_VERSION} VERSION_LESS 3.17)
    set(CMAKE_CURRENT_FUNCTION_LIST_DIR "${CMAKE_CURRENT_LIST_DIR}")
endif()

function(target_embed_file target file)
    get_filename_component(embed_filename ${file} NAME)
    get_filename_component(embed_absfile ${file} ABSOLUTE)

    set(embed_object_name _binary_${embed_filename})
    set(embed_object_filepath ${CMAKE_CURRENT_BINARY_DIR}/${embed_object_name}.c)

    add_custom_command(OUTPUT ${embed_object_filepath}
        COMMAND ${CMAKE_COMMAND}
            -DBINARY_FILE="${embed_absfile}"
            -DSOURCE_FILE="${embed_object_filepath}"
            -P "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/file_to_binary.cmake"
        MAIN_DEPENDENCY "${file}"
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/file_to_binary.cmake "${file}")

    add_library(${embed_object_name} OBJECT ${embed_object_filepath})
    target_link_libraries(${target} ${embed_object_name})
endfunction()