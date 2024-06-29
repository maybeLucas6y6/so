#!/bin/bash

if [ $# -eq 0 ]
then
    echo "Use ./starter.sh [p]"
    echo "    where p is 1 or 2"
    exit 1
fi

script_path=$(realpath $0)
script_dir=$(dirname $script_path)

gcc -Wall ${script_dir}/manager/supervisor.c -o ${script_dir}/manager/supervisor
gcc -Wall ${script_dir}/workers/worker1.c -o ${script_dir}/workers/worker1
gcc -Wall ${script_dir}/workers/worker2.c -o ${script_dir}/workers/worker2

if [ $1 == "1" ] 
then 
    ${script_dir}/workers/worker2 &
    sleep 2
    ${script_dir}/manager/supervisor $script_dir/input_data.txt
elif [ $1 == "2" ]
then 
    ${script_dir}/manager/supervisor $script_dir/input_data.txt &
    sleep 3
    ${script_dir}/workers/worker2
fi

wait

# afisare mapare
# cleanup fifo
# cleanup shm

exit 0
