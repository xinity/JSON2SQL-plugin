# Use Ubuntu 20.04 as the base image
FROM ubuntu:20.04

# Add a label for the image author
LABEL maintainer="arbaudie.it@gmail.com"

# Avoid prompts from apt
ENV DEBIAN_FRONTEND=noninteractive

# Install necessary tools and libraries
RUN apt-get update && apt-get install -y \
    git \
    build-essential \
    cmake \
    libmariadb-dev \
    libcjson-dev \
    libmicrohttpd-dev \
    curl \
    gnupg

# Add MariaDB 10.11 repository
RUN curl -o /etc/apt/trusted.gpg.d/mariadb_release_signing_key.asc 'https://mariadb.org/mariadb_release_signing_key.asc'
RUN echo "deb [arch=amd64] https://mirrors.aliyun.com/mariadb/repo/10.11/ubuntu focal main" > /etc/apt/sources.list.d/mariadb.list

# Install MariaDB 10.11
RUN apt-get update && apt-get install -y mariadb-server

# Set the working directory
WORKDIR /app

# Clone your GitHub repository
RUN git clone https://github.com/SylvainA77/JSON2SQL-plugin.git /app

# Build your project
RUN mkdir build && cd build && \
    cmake .. && \
    make

# Set the entry point for your application
CMD ["/bin/bash"]
