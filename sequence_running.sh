#!/bin/bash

parallel_level=$1 # parallelism degree
instances=($(ls -1 ./instances/Solomon/*))
num_instances=$(expr ${#instances[@]} - 1)
cur_paralel=1

for i in $(seq 0 $num_instances); do
  echo ${instances[$i]}
  echo $cur_paralel
  python main.py --instance ${instances[$i]} &
  if [ "$cur_paralel" -gt "$parallel_level" ]; then
    cur_paralel=1
    wait
  else
    cur_paralel=$((cur_paralel + 1))
  fi
done
wait
