#!/bin/bash

for i in $(seq 1 $1)
do
    read -p "Nume utilizator: " u
    echo $u
done
