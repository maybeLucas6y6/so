#!/bin/bash

if [ $# -eq 0 ]
then
    read -p "Introduceti numele fisierului: " fisier
else 
    fisier=$1
fi

# dam wipe la fisier
echo > $fisier # sau truncate -s 0 fisier 

nr_operatie=0

until false
do 
    read a b op 

    let nr_operatie=nr_operatie+1

    case $op in 
        "+" ) let rez=a+b ;;
        "-" ) let rez=a-b ;;
        "*" ) let rez=a*b ;;
        "/" ) rez=$(echo "$a / $b" | bc -l) ;; # sau rez=$("scale=5; $a / $b | bc")
        "q" ) break 
    esac

    echo "$nr_operatie: $a $op $b = $rez" >> $fisier 
done

echo "Nr total de operatii: $nr_operatie" >> $fisier
