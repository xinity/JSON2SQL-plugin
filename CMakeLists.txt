cmake_minimum_required(VERSION 3.10)
project(json2sql VERSION 1.0)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

# Include directories
include_directories(/usr/include/)
include_directories(/usr/include/cjson/)
include_directories(/usr/include/mysql/server/)

# Add the plugin library
add_library(json2sql MODULE code/json-api.c)

target_link_libraries(json2sql cjson microhttpd)
# Set output directory for the plugin
set_target_properties(json2sql PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "/app/plugin" )

# Installation rules
install(TARGETS json2sql
    LIBRARY DESTINATION "/usr/lib64/mysql/plugin/" 
    )
