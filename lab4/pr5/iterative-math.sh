#!/bin/bash

if [ $# -eq 0 ]
then
    read -p "Introduceti n: " n
    declare -a v
    for i in $(seq 1 $n)
    do 
        read -n v[$i]
    done

    for i in $(seq 1 $n)
    do 
        echo ${v[$i]}
    done
fi 
