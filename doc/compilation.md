## To compile, install and launch your MariaDB plugin as an external library without compiling the entire MariaDB server, follow these steps:

* download [Dockerfile](https://github.com/SylvainA77/JSON2SQL-plugin/blob/main/docker/rockylinux/Dockerfile) 
* docker build --no-cache -t json2sql:latest .
* docker run -it json2sql
* cp /app/plugin/* /usr/lib64/mysql/plugin/
* mariadbd-safe --defaults-extra-file=/etc/load_jsonplugin.cnf &

## Sources :  
[1] https://mariadb.org/installing-plugins-in-the-mariadb-docker-library-container/  
[2] https://docs.tiledb.com/mariadb/installation-from-source  
[3] https://github.com/pluots/sql-udf  
[4] https://mariadb.com/kb/en/generic-build-instructions/  
[5] https://mariadb.com/kb/en/compiling-mariadb-from-source/  
[6] https://stackoverflow.com/questions/61047894/building-mariadb-connector-c-library-as-a-submodule  
[7] https://mariadb.com/kb/en/development-writing-plugins-for-mariadb/  
[8] https://www.jetify.com/docs/devbox/devbox_examples/databases/mariadb/  
