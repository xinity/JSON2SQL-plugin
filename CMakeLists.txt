cmake_minimum_required(VERSION 3.10)
project(json2sql VERSION 1.0)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

# Find required packages
# find_package(MariaDB REQUIRED)
# find_package(MySQL REQUIRED)
find_package(CJSON REQUIRED)
find_package(MHD REQUIRED)

# Include directories
#include_directories(${MYSQL_INCLUDE_DIR})
include_directories(${CJSON_INCLUDE_DIR})
include_directories(${MHD_INCLUDE_DIR})
include_directories(/usr/lib64/cmake/mariadb/) 
include_directories(/usr/lib/cmake/mariadb/)
# include_directories(/usr/lib64/cmake/mysql/)
# include_directories(/usr/lib/cmake/mysql/)

# Add the plugin library
add_library(json2sql MODULE code/json-api.c)

# Link libraries
target_link_libraries(json2sql 
#    ${MYSQL_LIBRARY}
    ${CJSON_LIBRARIES}
    ${MHD_LIBRARIES} 
    /usr/lib64/cmake/mariadb/
    /usr/lib/cmake/mariadb/
)

# Set output directory for the plugin
set_target_properties(json2sql PROPERTIES 
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugin"
)

# Installation rules
install(TARGETS json2sql
    LIBRARY DESTINATION ${MYSQL_PLUGIN_DIR}
)
