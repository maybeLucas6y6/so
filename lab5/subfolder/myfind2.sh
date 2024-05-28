#!/bin/bash

function parcurgere_director ()
{
    for fis in $(ls -A $1)
    do
        cale=$1/$fis
        if [ -d $cale ] ; then
            parcurgere_director $cale
        elif [ -f $cale ] ; then
            res=$(file $cale | grep "Bourne-Again shell script")
            if [ ${#res} -ne 0 ]
            then
                let nr_bash++
                echo $cale $(md5sum $cale | cut -d" " -f1) $(wc -m $cale | cut -d" " -f1)
            else
                echo $cale $(wc -m $cale | cut -d" " -f1) 1>&2
            fi
        fi
    done
}

if [ $# -eq 0 ]
then
    read numedir
    [ -z $numedir ] && numedir="."
else
    numedir=$1
fi

parcurgere_director $numedir

if ((nr_bash<=0))
then
    exit -1
fi

exit 0 
