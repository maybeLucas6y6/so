#!/bin/bash

cp ./shared/db_bk ./shared/db

for file in $(ls ./commands/)
do 
    echo "$file:"
    ./executables/bin_to_text_s ./commands/$file
done

echo db
./executables/bin_to_text_s ./shared/db

exit 0
