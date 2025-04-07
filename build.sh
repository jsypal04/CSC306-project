#!/bin/bash

src_ext=".c"

for file in `ls`; do
    rest=${file#*$src_ext}
    index=$(( ${#file} - ${#rest} - ${#src_ext} ))

    if [ $index -lt 0 ]; then
        continue
    fi 

    name=${file:0:$index}

    gcc -o "bin/$name" $file
done
