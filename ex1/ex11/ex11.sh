#!/bin/bash
# Eitan Kerzhner
# 205697139

input=$1
# line counter
numLine=0
# line reader
while IFS= read -r line
do
# num of words
    words=0
    counter=0
    numLine=$[$numLine+1]
# words reader
    for word in $line; do
        words=$[$words+1]
        if [ $word = $2 ]
        then
            counter=$[$counter+1]
        fi
    done
    if [ $counter != 0 ]
    then
        echo $numLine $words
    fi
done < "$input"

