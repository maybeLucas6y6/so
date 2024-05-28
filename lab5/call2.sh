#!/bin/bash

if [ $# -eq 0 ]
then
    read -p "Input a number: " n
else
    n=$1
fi

files=$(find ~/proj/cpp/ -maxdepth $n -mtime -28 -type f)

if [ $? -ne 0 ]
then
    echo "Command find has failed!" 1>&2
    exit 1
fi

IFS=$'\n'
for f in $files
do 
    res=$(file $f | grep "C source")
    if [ ${#res} -ne 0 ]
    then
        filepath=$(echo $res | cut -d: -f1)
        filename=${filepath##*/}
        filename_noext=${filename%%.*}
        gcc $filepath -o ~/facultate3/so/lab5/c-dump/$filename_noext.out > /dev/null 2>&1
        if [ $? -eq 0 ]
        then
            echo "$filepath -> Compilat ok"
        else
            echo "$filepath -> Eroare la compilare!"
        fi
    else
        filepath=$(echo $f | cut -d: -f1)
        echo $filepath $(stat $f --print "%y")$
    fi
done

exit 0
