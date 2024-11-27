# Use Rocky Linux as the base image
FROM rockylinux/rockylinux:8

# Set environment variables to avoid prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Add a label for the image author
LABEL maintainer="arbaudie.it@gmail.com"

# Install necessary tools and libraries
RUN dnf update -y && yum install -y epel-release
RUN dnf install -y git gcc gcc-c++ cmake curl cjson-devel
RUN dnf --enablerepo=devel install -y libmicrohttpd-devel
#housekeeping time
RUN yum clean all


# Add MariaDB 10.11 repository and install it + devel headers + housekeeping
RUN curl -sSL https://downloads.mariadb.com/MariaDB/mariadb_repo_setup | bash -s -- --mariadb-server-version="mariadb-10.11" && \
    yum install -y MariaDB-server MariaDB-client MariaDB-devel && yum clean all

# Set the working directory
WORKDIR /app

# Clone your GitHub repository
RUN git clone https://github.com/SylvainA77/json2sql-plugin.git .

# Build your project
RUN mkdir build && cd build && cmake .. && make

# Expose the default MariaDB port
EXPOSE 3306
# Expose the defaul json2sql port
EXPOSE 3000

# Start command for the container (MariaDB will not auto-start)
CMD ["/bin/bash"]
