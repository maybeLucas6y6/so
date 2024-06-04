#!/bin/bash

# se apeleaza cu
# luca ~ : ./facultate/so/variante/var1/test.sh facultate/so/tips.sh

# pwd # /home/luca
origin=$(pwd)
dir=$(dirname $0) # ./facultate/so/variante/var1
# la apel argumentul e relativ la cwd (adica facultate/so/tips.sh)
# asa ca inainte sa dam cd $dir, vrem sa luam calea absoluta de la
# argument (o sa devina /home/luca/facultate/so/tips.sh)
file=$(realpath $1)
cd $dir 
# pwd # /home/luca/facultate/so/variante/var1

# basically aici poti sa faci orice operatie
# ca si cum ai fi in folderul cu scriptului
# defapt chiar esti, ptc ai dat cd

# cat $file # afiseaza bine

cd $origin

# pwd
