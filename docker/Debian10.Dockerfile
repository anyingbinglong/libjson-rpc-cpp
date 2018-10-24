FROM debian:buster
MAINTAINER Peter Spiess-Knafl <dev@spiessknafl.at>

RUN apt-get update && apt-get install -y \
    wget \
    build-essential \
    cmake \
    libjsoncpp-dev \
    libargtable2-dev \
    libcurl4-openssl-dev \
    libmicrohttpd-dev \
    git \
    libhiredis-dev \
    redis-server

RUN mkdir /app
COPY docker/build_test_install.sh /app
COPY . /app
RUN chmod a+x /app/build_test_install.sh
RUN cd /app && ./build_test_install.sh
