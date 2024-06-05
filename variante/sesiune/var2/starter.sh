#!/bin/bash

if [ $# -eq 0 ]
then
    echo "Usage: ./sharter.sh path"
    exit 1
fi

file_path=$(realpath $1)

gcc workers/worker1.c -o workers/worker1
gcc workers/worker2.c -o workers/worker2
gcc manager/supervisor.c -o manager/supervisor

./workers/worker2 &

sleep 3

./manager/supervisor $file_path

exit 0
