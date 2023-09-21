#!/bin/bash

sum=0;
amt=0;
for num in "$@"
do
  sum=$((sum + num));
  amt=$((amt + 1));
done
avg=$((sum / amt));
echo "amount: $amt"
echo "average: $avg"
