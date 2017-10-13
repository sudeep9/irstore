#!/bin/bash

mode=build

if [ $# -gt 0 ]; then
    mode=$1
fi

case $mode in 
    build)
        docker run -v $(pwd):/code ubuntu16:gcc7 bash -c 'cd /code; ./build.sh'
    ;;

    bash)
        docker run -it -v $(pwd):/code ubuntu16:gcc7 bash
    ;;

    *)
        echo "Error: unknown mode: [$mode]"
    ;;
esac

