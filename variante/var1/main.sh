#!/bin/bash

base_level=$(dirname $0)

scripts_dir="$base_level/scripts/"
if [ ! -d $scripts_dir -o ! -f "$scripts_dir/listdir.sh" -o ! -x "$scripts_dir/listdir.sh" ]
then
    echo "Nu exista folderul $scripts_dir sau nu acesta nu contine \"listdir.sh\" sau nu aveti drepturi de executie pe \"listdir.sh\" " 1>&2
    exit 1
fi 

programs_dir="$base_level/programs/"
if [ ! -d $programs_dir -o ! -f "$programs_dir/filetypeandsize.c" -o ! -r "$programs_dir/filetypeandsize.c" ]
then
    echo "Nu exista folderul $programs_dir sau nu acesta nu contine \"filetypeandsize.c\" sau nu aveti drepturi de citire pe \"filetypeandsize.c\" " 1>&2
    exit 2
fi 

if [ ! -f "$programs_dir/filetypeandsize" ]
then
    gcc "$programs_dir/filetypeandsize.c" -o "$programs_dir/filetypeandsize" -Wall
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

./"$scripts_dir/listdir.sh" $1 | tr ':' '---' | head -n 20 | tail -n +5 # sau cu sed "s/:/---"
