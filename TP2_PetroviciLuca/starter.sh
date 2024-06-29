#!/bin/bash

if [ $# -eq 0 ]
then
    echo "Use: ./starter.sh [0/1]"
    exit 1
fi

script_path=$(realpath $0)
script_dir=$(dirname $script_path)
file_path=$(realpath "data/input.txt")

gcc -Wall ${script_dir}/main_app/supervisor.c -o ${script_dir}/main_app/supervisor
gcc -Wall ${script_dir}/main_app/procesators/worker1.c -o ${script_dir}/main_app/procesators/worker1
gcc -Wall ${script_dir}/main_app/procesators/worker2.c -o ${script_dir}/main_app/procesators/worker2

if [ $1 == "0" ]
then
    ${script_dir}/main_app/procesators/worker2 &
    sleep 1
    ${script_dir}/main_app/supervisor $file_path
elif [ $1 == "1" ]
then
    ${script_dir}/main_app/supervisor $file_path &
    sleep 2
    ${script_dir}/main_app/procesators/worker2 &
fi

cat /dev/shm/sup_and_w1

rm /dev/shm/sup_and_w1
rm w1_and_w2

wait
exit 0
