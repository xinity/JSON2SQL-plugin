## To compile your MariaDB plugin as an external library without compiling the entire MariaDB server, follow these steps:

1. Install the MariaDB development files:
   - For Ubuntu/Debian: `sudo apt-get install libmariadb-dev`
   - For Fedora/CentOS: `sudo dnf install mariadb-devel`

2. Create a CMakeLists.txt file in your plugin directory with the following content:

```cmake
cmake_minimum_required(VERSION 3.0)
project(your_plugin_name)

find_package(MySQL REQUIRED)
include_directories(${MYSQL_INCLUDE_DIR})

add_library(your_plugin_name MODULE your_plugin_source.c)
target_link_libraries(your_plugin_name ${MYSQL_LIBRARY})
```

3. Create a build directory and navigate to it:

```bash
mkdir build
cd build
```

4. Run CMake to configure the build:

```bash
cmake ..
```

5. Compile your plugin:

```bash
make
```

The compiled plugin will be in the build directory as `your_plugin_name.so`.

To use the plugin, copy it to the MariaDB plugin directory (usually `/usr/lib/mysql/plugin/`) and load it using SQL commands[1].

## Sources :  
[1] https://mariadb.org/installing-plugins-in-the-mariadb-docker-library-container/  
[2] https://docs.tiledb.com/mariadb/installation-from-source  
[3] https://github.com/pluots/sql-udf  
[4] https://mariadb.com/kb/en/generic-build-instructions/  
[5] https://mariadb.com/kb/en/compiling-mariadb-from-source/  
[6] https://stackoverflow.com/questions/61047894/building-mariadb-connector-c-library-as-a-submodule  
[7] https://mariadb.com/kb/en/development-writing-plugins-for-mariadb/  
[8] https://www.jetify.com/docs/devbox/devbox_examples/databases/mariadb/  
