
FROM ubuntu:16.04

RUN apt-get update && \
    apt-get install -y software-properties-common && \
    add-apt-repository ppa:jonathonf/gcc-7.1 && \
    apt-get update && \
    apt-get install -y gcc-7

RUN apt-get install -y make g++-7 && \
    ln -s /usr/bin/gcc-7 /usr/bin/gcc && \
    ln -s /usr/bin/g++-7 /usr/bin/g++

RUN apt-get install -y wget less vim 

RUN cd /root && \ 
    rm -fR cmake* && \
    wget https://cmake.org/files/v3.9/cmake-3.9.3-Linux-x86_64.tar.gz && \
    tar xfz cmake-3.9.3-Linux-x86_64.tar.gz && \
    cp ./cmake-3.9.3-Linux-x86_64/bin/* /usr/bin/. && \
    cp -r ./cmake-3.9.3-Linux-x86_64/share/* /usr/share/.


