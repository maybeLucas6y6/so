#!/bin/bash

if [ $# -ne 1 -o ! -d $1 -o ! -r $1 ]
then
    echo "Nr argumente invalid sau $1 nu este director sau $1 nu are perm read"
    exit 2
fi 

dir=$(dirname $0)
> "$dir/forbidden.log"

function rec {
    if [ ! -r $1 ]
    then
        realpath $1 >> "$dir/forbidden.log"
        echo >> "$dir/forbidden.log"
    elif [ -f $1 ]
    then
        programs/alphanumeric $1
    elif [ -d $1 ]
    then
        for file in $(ls $1)
        do 
            rec "$1/$file"
        done
    fi 
}

rec $1

exit 0
