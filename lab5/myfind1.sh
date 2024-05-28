#!/bin/bash

if [ $# -eq 0 ]
then
    exit 0
fi

if [ ! -d $1 -o ! -r $1 -o ! -x $1 ]
then
    echo "Argumentul dat, $1, nu este director, sau nu aveti drept de citire sau scanare asupra lui!" 1>&2
fi

./subfolder/myfind2.sh $1
