ARG ROCKYVER=8
ARG MARIAVERS=11.4
# Use Rocky Linux as the base image
FROM rockylinux/rockylinux:${ROCKYVERS} AS build

# Add a label for the image author
LABEL maintainer="arbaudie.it@gmail.com"

# Install necessary tools and libraries and housekeeping time
RUN dnf update -y && yum install -y epel-release \ 
    && dnf install -y git gcc gcc-c++ cmake curl net-tools cjson-devel \
    && dnf --enablerepo=devel install -y libmicrohttpd-devel \
    && yum clean all

# Add MariaDB 11.4 repository and install it + devel headers + housekeeping
RUN curl -sSL https://downloads.mariadb.com/MariaDB/mariadb_repo_setup | bash -s -- --mariadb-server-version=mariadb-${MARIAVERS} \
    && yum install -y MariaDB-server MariaDB-client MariaDB-devel \ 
    && yum clean all

# Set the working directory
WORKDIR /opt

# Adding /opt to the $PATH for easy calling from inside the container
ENV PATH="${PATH}:/opt"

# creating the basic plugin integration config
RUN printf "[mariadb]\nplugin-load-add=libjson2sql\nplugin_maturity=unknown\n" > /etc/load_jsonplugin.cnf

# creating clone & deploy scripts
RUN printf "#!/bin/bash \n cd /app \n git clone --depth 1 https://github.com/SylvainA77/json2sql-plugin.git . \n mkdir build \n cd build \n cmake .. \n" > clone
RUN printf "#!/bin/bash \n rm /usr/lib64/mysql/plugin/libjson2sql.so \n cp -f /app/plugin/libjson2sql.so /usr/lib64/mysql/plugin/ \n mariadbd-safe --defaults-extra-file=/etc/load_jsonplugin.cnf &" > deploy
# dont forget to chmod the scripts !! 

WORKDIR /app

# Expose the default json2sql port
EXPOSE 3000

# Start command for the container (MariaDB will not auto-start)
CMD ["/bin/bash"]
