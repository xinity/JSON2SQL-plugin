## To compile, install and launch your MariaDB plugin as an external library without compiling the entire MariaDB server, follow these steps:

* download [Dockerfile](https://github.com/SylvainA77/JSON2SQL-plugin/blob/main/docker/rockylinux/Dockerfile) 
* build once : _docker build --no-cache -t json2sql:latest ._   
  *  Container is built with all the tools and scripts
  *  To target a different branch, edit Dockerfile and change _ENV GITBRANCH=yourtargetbranch_ then (re)build 
* run many : _docker run -it json2sql_    
  *  first _chmod +x /opt/*_
  *  To clone your branch target, call _clone_
  *  to compile, _cd /app/build; cmake --build . --verbose_
  *  To forcefully deploy the lib and start mariadb daemon, call _deploy_
  *  To test, _curl -X method http://127.0.0.1:3000/v1/resourceyouwanttotest_

## Sources :  
[1] https://mariadb.org/installing-plugins-in-the-mariadb-docker-library-container/  
[2] https://docs.tiledb.com/mariadb/installation-from-source  
[3] https://github.com/pluots/sql-udf  
[4] https://mariadb.com/kb/en/generic-build-instructions/  
[5] https://mariadb.com/kb/en/compiling-mariadb-from-source/  
[6] https://stackoverflow.com/questions/61047894/building-mariadb-connector-c-library-as-a-submodule  
[7] https://mariadb.com/kb/en/development-writing-plugins-for-mariadb/  
[8] https://www.jetify.com/docs/devbox/devbox_examples/databases/mariadb/  
