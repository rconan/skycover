#!/bin/bash

nfiles=$1

if [[ $nfiles -eq "" ]]; then
    echo "usage: starprobs <nfiles>"
    exit
fi

for wfsmag in 13 14 15 16 17 18 19; do
    echo -ne "$wfsmag\t"
    ./skycov $wfsmag 0 $nfiles
done
