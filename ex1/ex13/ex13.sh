#!/bin/bash
num=0
Factorial()
{

for d in $(ls $1 | sort);
do
echo $1
    if [[ $d = $2 ]]
        then
            # return
            echo $d "________________"
            break
    else
        # recursive call
        if [[ $d != *"."* ]]
            then
                Factorial $1/$d $2
        fi
    fi
done
}
Factorial $1 $2
