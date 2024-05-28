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

if [ ! -f "validatorASCII" ]
then
    gcc validatorASCII.c -o validatorASCII
    if [ $? -ne 0 ]
    then
        echo "Eroare la compilare" 1>&2
        exit 3
    fi 
fi 

./validatorASCII $1 | tr ":" ">"
