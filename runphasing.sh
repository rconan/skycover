#!/bin/bash

nfiles=$1

if [[ $nfiles -eq "" ]]; then
    echo "usage: starprobs <nfiles>"
    exit
fi

for wfsmag in 12 12.5 13 13.5 14 14.5 15 16 17 18 19; do
    echo -ne "$wfsmag\t"
    ./skycov --phasing --m3 --notrack --noprint 18 18 $wfsmag $nfiles
done
