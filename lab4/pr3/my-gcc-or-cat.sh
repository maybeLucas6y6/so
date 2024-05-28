#!/bin/bash

if [ $# -ne 1 ]
then
    echo "Use: <cmd> <dir>"
    exit 1
else
    dir=$1
fi

cd $dir

if [ ! -d $dir -o ! -r $dir ]
then 
    echo "Eroare: Primul argument, $dir, nu este director, sau nu aveti drept de citire a acestuia!"
fi 

for file in $(ls -A $1)
do 
    case $file in 
        *.c ) echo "fisier sursa C: $dir/$file"
            gcc $file -o ${file%.c} -Wall ;;
        *.txt ) echo "fisier text: $dir/$file"
            cat $file ;;
    esac
done

exit 0
