#!/bin/bash

mode=build

if [ $# -gt 0 ]; then
    mode=$1
fi

DOCKER_IMAGE=img.druva.org/ubuntu16:gcc7

echo "Docker image: $DOCKER_IMAGE"

case $mode in 
    build)
        docker pull $DOCKER_IMAGE
        docker run -v $(pwd):/code $DOCKER_IMAGE bash -c 'cd /code; ./build.sh'
    ;;

    bash)
        docker run -it -v $(pwd):/code $DOCKER_IMAGE bash
    ;;

    *)
        echo "Error: unknown mode: [$mode]"
    ;;
esac

