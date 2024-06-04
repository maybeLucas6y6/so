#!/bin/bash

if [ $# -ne 1 ]
then
    exit 2
fi 

cwd=$(pwd)
dir=$(dirname $0)
cd $dir
script=$(basename $0)

if [ -f $1 ]
then
    ./"performances/filegrafitti" $1 $USER
elif [ -d $1 ]
then
    for file in $(ls $1)
    do 
        ./$script "$1/$file"
    done
fi 

cd $cwd
exit 0
