#!/bin/bash
# Eitan Kerzhner
# 205697139

cd $1
n=1

for d in $(ls -1v);
do
    if [[ $d = *".txt"* ]]
        then
            echo $n "$d is a file"
            n=$[$n+1]
    fi
done

for d in $(ls -1v);
do
    if [[ $d != *"."* ]]
        then
            echo $n "$d is a directory"
            n=$[$n+1]
    fi
done
