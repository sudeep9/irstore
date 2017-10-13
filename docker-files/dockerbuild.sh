#!/bin/bash


export CC=gcc
export CXX=g++

cd /code
rm -fR build
mkdir build
cd build
meson ..
if [ $? -ne 0 ]; then
    echo "meson failed"
    exit 1
fi

ninja
if [ $? -ne 0 ]; then
    echo "ninja failed"
    exit 1
fi