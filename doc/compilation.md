## To compile your MariaDB plugin as an external library without compiling the entire MariaDB server, follow these steps:

1. Install the MariaDB development files:
   - For Ubuntu/Debian: `sudo apt-get install -y libmariadb-dev`
   - For Fedora/CentOS: `sudo dnf install -y mariadb-devel`

2. Install cJSON development files :
   - For Ubuntu/Debian: `sudo apt-get install -y libmariadb-dev` (TBD)
   - For Fedora/CentOS: `sudo dnf install -y epel-release; dnf install -y cjson-devel`
  
3. Install microphttpd developmeent files :
   - For Ubuntu/Debian: `sudo apt-get install -y libmariadb-dev` (TBD)
   - For Fedora/CentOS: `sudo dnf --enablerepo=devel install -y libmicrohttpd-devel`   

5. Clone this repo in your work directory
```bash
git clone https://github.com/SylvainA77/json2sql-plugin.git .
```
   
5. Create a build directory and navigate to it:

```bash
mkdir build
cd build
```

6. Run CMake to configure the build:

```bash
cmake ..
```

7. Compile your plugin:

```bash
make
```

The compiled plugin will be in the build directory as `json2sql.so`.

To use the plugin, copy it to the MariaDB plugin directory (usually `/usr/lib/mysql/plugin/`) and load it using SQL commands[1].

## Alternatively you can duild+run the dockerfile from [HERE](https://github.com/SylvainA77/JSON2SQL-plugin/tree/main/docker) 

## Sources :  
[1] https://mariadb.org/installing-plugins-in-the-mariadb-docker-library-container/  
[2] https://docs.tiledb.com/mariadb/installation-from-source  
[3] https://github.com/pluots/sql-udf  
[4] https://mariadb.com/kb/en/generic-build-instructions/  
[5] https://mariadb.com/kb/en/compiling-mariadb-from-source/  
[6] https://stackoverflow.com/questions/61047894/building-mariadb-connector-c-library-as-a-submodule  
[7] https://mariadb.com/kb/en/development-writing-plugins-for-mariadb/  
[8] https://www.jetify.com/docs/devbox/devbox_examples/databases/mariadb/  
