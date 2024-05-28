#!/bin/bash

if [ $# -ne 1 ] 
then
    echo "insufficient number of arguments" 1>&2
    exit 2
fi 

origin=$(pwd)
dir=$(dirname $0)
script=$(basename $0)
arg=$(realpath $1)
cd $dir 

if [ ! -f "../validators/default.sh" -o ! -x "../validators/default.sh" ]
then
    echo "default.sh not found or execute permission required" 1>&2
    exit 1
fi 

if [ ! -d $arg -o ! -r $arg -o ! -x $arg ]
then
    echo "$arg is not a folder or read AND execute permission required" 1>&2
    exit 3
fi 

for file in $(ls $arg)
do 
    if [ -f $arg/$file ]
    then
        ../validators/default.sh $arg/$file >> results.txt
    elif [ -d $arg/$file ]
    then 
        ./$script $arg/$file
    fi 
done
