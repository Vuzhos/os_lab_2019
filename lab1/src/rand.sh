#!/bin/bash

for i in {1..150}
do
  printf "$RANDOM\n" >> numbers.txt;
done
