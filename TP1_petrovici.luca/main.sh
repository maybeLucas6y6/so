#!/bin/bash

# nu sunt sigur daca trebuie sa testez asta
# mi se pare normal sa testez, dar nu este 
# specificat in enunt

# if [ $# -ne 1 ] 
# then
#     echo "insufficient number of arguments" 1>&2
#     exit 3
# fi 

origin=$(pwd)
dir=$(dirname $0)
arg=$(realpath $1)
cd $dir 

if [ ! -d "xprocesator" ]
then
    echo "xprocesator folder not found" 1>&2 
    exit 1
fi 

if [ ! -f "xprocesator/procesator.sh" -o ! -x "xprocesator/procesator.sh" ]
then
    echo "procesator.sh script not found or execute permission required" 1>&2
    exit 1
fi 

if [ ! -d "validators" ]
then
    echo "validators folder not found" 1>&2 
    exit 2
fi 

if [ ! -f "validators/validatorASCII.c" -o ! -r "validators/validatorASCII.c" ]
then
    echo "validatorASCII.c script not found or read permission required" 1>&2
    exit 2
fi 

if [ ! -d $arg -o ! -r $arg ]
then
    echo "$arg is not a folder or read permission required" 1>&2
    exit 3
fi 

./xprocesator/procesator.sh $arg

echo $?
