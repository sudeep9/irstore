#!/bin/bash

export INC_BUILD=0
export TESTS_ONLY=0

run_ninja_tests() {
    echo "##############################################"
    echo "Running tests"
    echo "##############################################"
    ninja test
    if [ $? -ne 0 ]; then
        exit 1
    fi

    echo "##############################################"
    echo "Generating coverage report"
    echo "##############################################"
    mkdir cov
    cd ..
    gcovr -r . -s
    gcovr -r . -b --html --html-details -o build/cov/report.html
    if [ $? -ne 0 ]; then
        exit 1
    fi
    echo "Coverage at: build/cov/report.html"
}

run_ninja() {
    cd ./build

    if [ $TESTS_ONLY == 0 ]; then 
        ninja
        if [ $? -ne 0 ]; then
            exit 1
        fi
    fi

    run_ninja_tests
}

run_meson() {
    rm -fR build
    mkdir build
    cd build

    meson ..
    if [ $? -ne 0 ]; then
        exit 1
    fi

    cd $ROOTDIR
}

meson_build() {
    export CC=gcc
    export CXX=g++

    if [ $INC_BUILD == 0 ]; then
        run_meson
    else
        if [ ! -d ./build ]; then
            echo "Error: run full build first"
            exit 1
        fi
    fi

    cd $ROOTDIR

    run_ninja
}

if [ $# -gt 0 ]; then
    if [ $1 = "--help" ]; then
        echo "Options: inc test"
        echo "inc  => Run incremental build"
        echo "test => Run only test"
        exit 1
    fi
fi

export ROOTDIR=$(pwd)

for opt in $*
do
    case $opt in
    inc)
        export INC_BUILD=1
    ;;

    test)
        export INC_BUILD=1
        export TESTS_ONLY=1
    ;;

    *)
        echo "Unknown option: [$opt]"
        exit 1
    ;;
    esac
done

meson_build

