echo "ceva" 1>&2 # afisare pe iesirea standard de eroare stderr
file_noext="${file%.*}" # get filename without extension
dir=$(dirname $0) # iti da folderul in care se afla scriptul
