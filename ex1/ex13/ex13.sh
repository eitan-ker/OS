#!/bin/bash
# Eitan Kerzhner
# 205697139

# running on all files in the first level folder
for d in $(ls $1 | sort);
do
# if there is a file in the first folder
    if [[ $d = $2 ]]
        then
            cat $1/$d;
            echo
    else
        if [[ $d != *"."* ]]
            then
# running on all files in the second level folder
                for n in $(ls $1/$d | sort);
                do
# if ther's a file in the second level folders
                    if [[ $n = $2 ]]
                    then
                        cat $1/$d/$n;
                        echo
                    fi    
                done
        fi
    fi
done

