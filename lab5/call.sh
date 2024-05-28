#!/bin/bash

if [ $# -eq 0 ]
then
    read -p "Input a number: " n
else
    n=$1
fi

files=$(find ~/.config/ -mindepth $n -perm -u=rw -type f)

if [ $? -ne 0 ]
then
    echo "Command find has failed!" 1>&2
    exit 2
fi

for f in $files
do 
    res=$(file $f | grep "Bourne-Again shell script")
    if [ ${#res} -ne 0 ]
    then
        filepath=$(echo $res | cut -d: -f1)
        chmod a+x $filepath
        echo $filepath $(stat $filepath --print "%a")
    else
        filepath=$(echo $f | cut -d: -f1)
        echo $filepath $(stat $f --print "%A")
    fi
done

exit 0
