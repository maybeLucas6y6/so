#!/bin/bash

if (( $# > 0 )); then
    n=$1
else
    read -p "Type a value for n: " n
fi

let val=1
let k=0
while (($k <= $n))
do 
    echo "$val" 1>&2
    let val*=2
    let k+=1
done
