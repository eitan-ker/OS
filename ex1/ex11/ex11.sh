#!/bin/bash
input=$1
numLine=0
while IFS= read -r line
do
    words=0
    counter=0
    numLine=$[$numLine+1]
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

