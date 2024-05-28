#!/bin/bash

if [ $# -ne 2 ]
then
    read -p "Dati n si m: " n m 
else
    n=$1
    m=$2
fi 

for i in $(seq 1 $m)
do 
    let p=p+n
done

echo $p 
exit 0
