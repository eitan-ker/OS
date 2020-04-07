#!/bin/bash

for d in $(ls $1 | sort);
do
    if [[ $d = $2 ]]
        then
            cat $1/$d;
            echo
    else
        # recursive call
        if [[ $d != *"."* ]]
            then
                for n in $(ls $1/$d | sort);
                do
                    if [[ $n = $2 ]]
                    then
                        cat $1/$d/$n;
                        echo
                    fi    
                done
        fi
    fi
done

