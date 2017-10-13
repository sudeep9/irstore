#!/bin/bash

build_system=meson

if [ $# -gt 0 ]; then
    build_system=$1
fi

meson_build() {
    rm -fR build
    mkdir build
    cd build

    export CC=gcc
    export CXX=g++

    meson ..
    if [ $? -ne 0 ]; then
        exit 1
    fi

    ninja
    if [ $? -ne 0 ]; then
        exit 1
    fi
}



case $build_system in 
    meson)
        meson_build
    ;;

    *)
        echo "Unknown build system: $build_system"
    ;;
esac

