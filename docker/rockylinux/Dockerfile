# Use Rocky Linux as the base image
FROM rockylinux/rockylinux:8 as build

# Add a label for the image author
LABEL maintainer="arbaudie.it@gmail.com"

# Install necessary tools and libraries
RUN dnf update -y && yum install -y epel-release
RUN dnf install -y git gcc gcc-c++ cmake curl cjson-devel
RUN dnf --enablerepo=devel install -y libmicrohttpd-devel
#housekeeping time
RUN yum clean all


# Add MariaDB 10.11 repository and install it + devel headers + housekeeping
RUN curl -sSL https://downloads.mariadb.com/MariaDB/mariadb_repo_setup | bash -s -- --mariadb-server-version="mariadb-11.4" && \
    yum install -y MariaDB-server MariaDB-client MariaDB-devel && yum clean all

# Set the working directory
WORKDIR /app

# Clone your GitHub repository
RUN git clone --depth 1 https://github.com/SylvainA77/json2sql-plugin.git .

# Alternately build from the top level directory of this repo
#COPY . .

# Build your project
RUN mkdir build && cd build && cmake .. && cmake --build . --verbose

FROM mariadb:11.4

RUN apt-get update && apt-get install -y libcjson1 libmicrohttpd12t64 && rm -rf /var/lib/apt/lists/*
COPY --from=build /app/plugin/libjson2sql.so /usr/lib/mysql/plugin
RUN printf "[mariadb]\nplugin-load-add=libjson2sql\n" > /etc/mysql/mariadb.conf.d/load_jsonplugin.cnf
# Expose the default json2sql port
EXPOSE 3000
