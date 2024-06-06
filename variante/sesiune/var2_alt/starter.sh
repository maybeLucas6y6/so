#!/bin/bash

if [ $# -eq 0 ]
then
    echo "Usage: ./sharter.sh path"
    exit 1
fi

file_path=$(realpath $1)

gcc -Wall manager/supervisor.c -o manager/supervisor
gcc -Wall workers/worker1.c -o workers/worker1
gcc -Wall workers/worker2.c -o workers/worker2

./workers/worker2 &

sleep 3

./manager/supervisor $file_path

exit 0
