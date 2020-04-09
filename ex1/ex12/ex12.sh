#!/bin/bash
# Eitan Kerzhner
# 205697139

# line counter
n=1
# printing all files
for d in $(ls $1 -1v);
do
    if [[ $d = *".txt"* ]]
        then
            echo $n "$d is a file"
            n=$[$n+1]
    fi
done
# printing all directories
for d in $(ls $1 -1v);
do
    if [[ $d != *"."* ]]
        then
            echo $n "$d is a directory"
            n=$[$n+1]
    fi
done
