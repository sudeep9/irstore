#!/bin/bash


cd /code
rm -fR build
mkdir build
cd build
cmake ..
if [ $? -ne 0 ]; then
    echo "cmake failed"
    exit 1
fi

make
if [ $? -ne 0 ]; then
    echo "make failed"
    exit 1
fi