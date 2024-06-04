#!/bin/bash

base_level=$(dirname $0)

arts_dir="$base_level/legal_arts/"
if [ ! -d $arts_dir -o ! -f "$arts_dir/iwashere.sh" ]
then
    echo "Nu exista folderul $arts_dir sau nu acesta nu contine \"iwashere.sh\"" 1>&2
    exit 1
elif [ ! -x "$arts_dir/iwashere.sh" ]
then
    chmod u+x "$arts_dir/iwashere.sh"
fi 

performances_dir="$arts_dir/performances/"
if [ ! -d $performances_dir -o ! -f "$performances_dir/filegrafitti.c" -o ! -r "$performances_dir/filegrafitti.c" ]
then
    echo "Nu exista folderul $performances_dir sau nu acesta nu contine \"filegrafitti.c\" sau nu aveti drepturi de citire pe \"filegrafitti.c\" " 1>&2
    exit 2
fi 

if [ ! -f "$performances_dir/filegrafitti" ]
then
    gcc "$performances_dir/filegrafitti.c" -o "$performances_dir/filegrafitti" -Wall
    if [ ! $? -eq 0 ]
    then
        echo "Eroare la compilare" 1>&2
        exit 3
    fi 
fi 

if [ ! -d $1 -o ! -r $1 ]
then
    echo "Argumentul dat nu este director sau nu aveti drepturi de citire pe directorul dat" 1>&2
    exit 4
fi 

./"$arts_dir/iwashere.sh" $1 | sort -r -u | tr -d "/"
