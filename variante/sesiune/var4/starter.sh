#!/bin/bash

if [ $# -eq 0 ]
then
    echo "Use: ./starter.sh [path]"
    exit 1
fi

script_path=$(realpath $0)
script_dir=$(dirname $script_path)
file_path=$(realpath $1)

gcc -Wall ${script_dir}/app/components/worker2.c -o ${script_dir}/app/components/worker2
gcc -Wall ${script_dir}/app/worker1.c -o ${script_dir}/app/worker1
gcc -Wall ${script_dir}/app/supervisor.c -o ${script_dir}/app/supervisor

${script_dir}/app/components/worker2 &

sleep 2

${script_dir}/app/worker1 $file_path
