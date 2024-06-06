#!/bin/bash

if [ $# -eq 0 ]
then
    echo "Usage: ./sharter.sh path"
    exit 1
fi

file_path=$(realpath $1)

gcc -Wall master/supervisor.c -o master/supervisor
gcc -Wall slaves/worker1.c -o slaves/worker1
gcc -Wall slaves/worker2.c -o slaves/worker2

./slaves/worker2 &

sleep 3

./master/supervisor $file_path

exit 0
