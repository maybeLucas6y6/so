#!/bin/bash

function euclid()
{
    a=$1
    b=$2
    if ((a > b)) then
        temp=$a
        a=$b
        b=$temp
    fi
    if ((a != b)) then
        if ((a > b)) then
            euclid $(($a - $b)) $b
            return $?
        else
            euclid $a $(($b - $a))
            return $?
        fi
    else
        return $a
    fi
}

if [ $# -lt 2 ]
then
    read -p "Dati cele doua numere: " x y
    euclid $x $y
else 
    euclid $1 $2
fi

echo $?
