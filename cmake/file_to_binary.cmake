

# Convert arbitrary file into a c file suitable for the compiler
# As replacement for objcopy to allow any architecture

if(NOT BINARY_FILE)
    message(FATAL_ERROR "EMBED_FILE not defined")
endif()

if(NOT SOURCE_FILE)
    message(FATAL_ERROR "SOURCE_FILE not defined")
endif()

if(NOT SYMBOL_NAME)
    get_filename_component(SYMBOL_NAME ${BINARY_FILE} NAME)
    string(REGEX REPLACE "\\." "_" SYMBOL_NAME "${SYMBOL_NAME}")
    string(TOLOWER SYMBOL_NAME ${SYMBOL_NAME})
    message(STATUS "SYMBOL_NAME not defined, setting to ${SYMBOL_NAME}")
endif()

message(STATUS "Running script for ${BINARY_FILE} ${SOURCE_FILE}")

file(READ "${BINARY_FILE}" _binary_data HEX)

# Split hex into 8 bytes per line
string(REGEX REPLACE "................" "\\0\n" _binary_data "${_binary_data}")
# Add 0x and comma
string(REGEX REPLACE "[0-9a-f][0-9a-f]" "0x\\0, " _binary_data "${_binary_data}")
# Remove last comma
string(REGEX REPLACE ", $" "" _binary_data "${_binary_data}")

set(_symbol_name_data __binary_${SYMBOL_NAME}_data)
set(_symbol_name_start __binary_${SYMBOL_NAME}_start)
set(_symbol_name_end __binary_${SYMBOL_NAME}_end)
set(_symbol_name_size __binary_${SYMBOL_NAME}_size)

file(REMOVE "${SOURCE_FILE}")
file(APPEND "${SOURCE_FILE}" "const unsigned char ${_symbol_name_data}[] = {\n${_binary_data}\n};\n")
file(APPEND "${SOURCE_FILE}" "const unsigned ${_symbol_name_size} = sizeof(${_symbol_name_data});\n")
file(APPEND "${SOURCE_FILE}" "const void* ${_symbol_name_start} = &${_symbol_name_data};\n")
file(APPEND "${SOURCE_FILE}" "const void* ${_symbol_name_end} = &${_symbol_name_data} + sizeof(${_symbol_name_data});\n")