#!/bin/bash

vec=()

if [ $# -eq 0 ]
then
    read -p "Dati lungimea secventei de numere: " n
    for i in $(seq 1 $n)
    do
        read -p "Dati al $i-lea numar: " vec[$i]
    done
    read -p "Dati numarul p: " p
    for i in $(seq 1 $n)
    do
        if ((${vec[$i]}<=$p))
        then
            echo -n "${vec[$i]} "
        fi
    done
else
    pivot=${*: -1}
    for i in $(seq 1 $(($# - 1)))
    do
        if (($1 <= pivot))
        then
            echo -n "$1 "
        fi
        shift
    done
fi
