#!/bin/bash

if [ $# -ne 1 -o ! -d $1 ]
then 
    echo "Arg insf sau not dir" 1>&2
    exit 1
fi 

if [ ! -d "scripts" -o ! -f "scripts/listdir.sh" ]
then 
    echo "Nu este dir sau nu exista sh" 1>&2
    exit 2
fi 

if [ ! -x "scripts/listdir.sh" ]
then
    chmod u+x scripts/listdir.sh
fi 

if [ ! -f "programs/alphanumeric.c" ]
then
    echo "Nu este sursa" 1>&2
    exit 3
fi 

src_mod_time=$(stat programs/alphanumeric.c --printf "%Y")
if [ ! -f "programs/alphanumeric" ]
then 
    gcc programs/alphanumeric.c -o programs/alphanumeric -Wall
    if [ $? -ne 0 ]
    then 
        echo "Eroare la compilare" 1>&2
        exit 3
    fi 
else 
    exe_mod_time=$(stat programs/alphanumeric --printf "%Y")
    if [ $src_mod_time -gt $exe_mod_time ]
    then
        gcc programs/alphanumeric.c -o programs/alphanumeric -Wall
        if [ $? -ne 0 ]
        then 
            echo "Eroare la compilare" 1>&2
            exit 3
        fi 
    fi 
fi

./scripts/listdir.sh $1 | grep ":alphanumeric" | head -n 5 | cut -d":" -f1
