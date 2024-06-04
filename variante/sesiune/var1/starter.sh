#!/bin/bash

if [ $# -eq 0 ]
then
    echo "Usage: ./sharter.sh path"
    exit 1
fi

file_path=$(realpath $1)

./subordinates/worker1 &

sleep 2

./coordonator/supervisor $file_path

exit 0
