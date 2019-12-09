#!/bin/bash
read var1

delimiter=' '

s=$var1$delimiter


COUNTER=0
Chance=""
M=""
N=""

temp="${s%%"$delimiter"*}";
s=${s#*"$delimiter"};
Chance=$temp

temp="${s%%"$delimiter"*}";
s=${s#*"$delimiter"};
M=$temp

temp="${s%%"$delimiter"*}";
s=${s#*"$delimiter"};
N=$temp

# echo "$Chance $M $N"

if [ "$M" = "8" ]; then
    ./bot0 $Chance
elif [ "$M" = "10" ]; then
    if [ "$N" = "8" ]; then
        ./bot1 $Chance
    elif [ "$N" = "10" ]; then
        ./bot2 $Chance
    fi
fi