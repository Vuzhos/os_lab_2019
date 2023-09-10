#! /bin/bash
echo "Amount of numbers: $#"
total=0
for var in $@
do
    (( total += $var ))
done
echo "scale=3 ; $total / $#" | bc
