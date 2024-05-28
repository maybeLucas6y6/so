#!/bin/bash

if [ $# -eq 0 ]
then
    read -p "No. of instances: " n
    read -p "Mode: " mode 
elif [ $# -eq 1 ]
then
    n=$1
    read -p "Mode: " mode 
else 
    n=$1
    mode=$2
fi 

if [ $n -lt 1 ]
then
    echo -e "Invalid no. of instances"
    exit 1
fi 

exec="executables"
if [ ! -d $exec -o ! -a $exec ] 
then
    echo "$exec is not a dir or you don't have access perm to $exec" 1>&2
    exit 2
fi

src="sources"
if [ ! -d $src -o ! -a $src ] 
then
    echo "$src is not a dir or you don't have access perm to $src" 1>&2
    exit 2
fi

without_locks="without_locks"
if [ ! -f "$exec/$without_locks" ]
then
    gcc "$src/$without_locks.c" -o "$exec/$without_locks"
    if [ $? -ne 0 ]
    then
        echo "Eroare la compilare $without_locks" 1>&2 
        exit 2
    fi 
fi

with_locks="with_locks"
if [ ! -f "$exec/$with_locks" ]
then
    gcc "$src/$with_locks.c" -o "$exec/$with_locks"
    if [ $? -ne 0 ]
    then
        echo "Eroare la compilare $with_locks" 1>&2 
        exit 2
    fi 
fi

cmds="commands"
if [ ! -d $cmds -o ! -a $cmds ] 
then
    echo "$cmds is not a dir or you don't have access perm to $cmds" 1>&2
    exit 3
fi

for i in $(seq 1 $n)
do 
    if [ ! -f "$cmds/instr$i" -o ! -r "$cmds/instr$i" ]
    then
        echo "instr$i: not a file or no read perm" 1>&2
        exit 3
    fi
done

shr="shared"
if [ ! -d $shr -o ! -a $shr ] 
then
    echo "$shr is not a dir or you don't have access perm to $shr" 1>&2
    exit 4
fi

if [ ! -f "$shr/db" ]
then
    if [ ! -r "$shr/db" -o ! -w "$shr/db" ]
    then
        echo "kaboom" 1>&2
        exit 4
    fi
    echo "kaboom" 1>&2
    exit 4
fi

for i in $(seq 1 $n)
do 
    case $mode in
        "lock" ) ./$exec/with_locks $shr/db $cmds/instr$i & ;;
        "no-lock" ) ./$exec/without_locks $shr/db $cmds/instr$i & ;;
        * ) echo -e "Invalid mode" ; exit 2 ;;
    esac
done

wait

exit 0
