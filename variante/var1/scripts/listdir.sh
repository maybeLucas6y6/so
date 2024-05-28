#!/bin/bash

if [ $# -ne 1 ]
then 
    exit 2
fi 

dir=$(dirname $0)
script=$(basename $0)
arg=$(realpath $1)
cd $dir

../programs/filetypeandsize $arg

if [ $? -eq 1 ]
then
    for file in $(ls $arg)
    do 
        ./$script "$arg/$file"
    done
fi 
