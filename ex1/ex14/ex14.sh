#!/bin/bash
# Eitan Kerzhner
# 205697139

# variables initiation
input=$2
name=0
sum=0
# lines
while IFS= read -r line
do
    count=0
# words in lines
    for word in $line; do
# finding the line we need
        if [ $count = 0 ]
        then    
	    if [ $word = $1 ]
	    then
                name=1    
	        echo $line
	    fi
        fi
# sum the balance 
        if [ $name = 1 ]
        then
            if [ $count = 1 ]
            then
                sum=$[$sum+$word]
                name=0
            fi
        fi
    count=$[$count+1]
    done
done < "$input"
echo "Total balance:" $sum
