# Use RHEL 8 as the base image
FROM registry.access.redhat.com/ubi8/ubi:latest

# Set environment variables to avoid prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Add a label for the image author
LABEL maintainer="arbaudie.it@gmail.com"

# Install necessary tools and libraries
RUN yum update -y && \
    yum install -y \
    git \
    gcc \
    gcc-c++ \
    cmake \
    libcjson-devel \
    libmicrohttpd-devel \
    curl \
    && yum clean all

# Add MariaDB 10.11 repository and install it
RUN curl -sSL https://downloads.mariadb.com/MariaDB/mariadb_repo_setup | bash && \
    yum install -y MariaDB-server MariaDB-client && \
    yum clean all

# Set the working directory
WORKDIR /app

# Clone your GitHub repository
RUN git clone https://github.com/your-username/your-repo.git /app

# Build your project
RUN mkdir build && cd build && \
    cmake .. && \
    make

# Expose the default MariaDB port
EXPOSE 3306
# Expose the defaul json2sql port
EXPOSE 3000

# Start command for the container (MariaDB will not auto-start)
CMD ["bash"]
