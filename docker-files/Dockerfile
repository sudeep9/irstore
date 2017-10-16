
FROM ubuntu:16.04

#--------------------------------------------------------------
# Install gcc
#--------------------------------------------------------------
RUN apt-get update && \
    apt-get install -y software-properties-common && \
    add-apt-repository ppa:jonathonf/gcc-7.1 && \
    apt-get update && \
    apt-get install -y gcc-7

RUN apt-get install -y make g++-7 && \
    ln -s /usr/bin/gcc-7 /usr/bin/gcc && \
    ln -s /usr/bin/g++-7 /usr/bin/g++ 


#--------------------------------------------------------------
# Install basic utils
#--------------------------------------------------------------
RUN apt-get install -y wget less vim 

#--------------------------------------------------------------
# Install build tools
#--------------------------------------------------------------
RUN cd /root && \ 
    rm -fR cmake* && \
    wget https://cmake.org/files/v3.9/cmake-3.9.3-Linux-x86_64.tar.gz && \
    tar xfz cmake-3.9.3-Linux-x86_64.tar.gz && \
    cp ./cmake-3.9.3-Linux-x86_64/bin/* /usr/bin/. && \
    cp -r ./cmake-3.9.3-Linux-x86_64/share/* /usr/share/.

RUN apt-get update && \
    apt-get install -y git python3-pip python-pip && \
    pip3 install meson && \
    pip install ninja gcovr 

RUN cd /root/ && \
    rm -fR doctest && \
    git clone --depth=1 https://github.com/onqtam/doctest && \
    cd doctest && \
    mkdir build && \
    cd build && \
    cmake .. && \
    make && \
    make install && \
    cd /root && \
    rm -fR doctest

RUN cd /root/ && \
    rm -fR lmdb && \
    git clone --depth=1 https://github.com/LMDB/lmdb.git && \
    cd lmdb/libraries/liblmdb && \
    make && \
    make install && \
    cd /root && \
    rm -fR lmdb

RUN cd /usr/include && wget https://github.com/nlohmann/json/raw/develop/src/json.hpp

RUN cd /root/ && \
    rm -fR lmdb && \
    git clone --depth=1 https://github.com/gabime/spdlog.git &&\
    cd spdlog && \
    mkdir build && \
    cd build && \
    cmake .. && \
    make && \
    make install && \
    cd /root && \
    rm -fR spdlog

RUN mv /usr/bin/gcov /usr/bin/gcov.old && \
    ln -s /usr/bin/gcov-7 /usr/bin/gcov

ENV LC_ALL=C.UTF-8
