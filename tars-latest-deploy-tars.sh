#!/bin/bash

if (( $# < 1 ))
then
    echo $#
    echo "$0 push(false/true)"
    exit 1
fi

push=$1

if [ "$push" == "true" ]; then
	push="true"
else
	push="false"
fi

cd docker/tars
./build-docker.sh master master latest ${push}
