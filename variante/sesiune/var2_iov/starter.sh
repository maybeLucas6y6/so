#!/bin/bash

if [ $# -eq 0 ]
then
    echo "Usage: ./starter.sh [path]"
    exit 2
fi

gcc workers/worker1.c -o workers/worker1
gcc workers/worker2.c -o workers/worker2
gcc manager/supervisor.c -o manager/supervisor

./workers/worker2 &

fis=$(realpath $1)

sleep 3

./manager/supervisor $fis

exit 0
