#!/bin/bash

simtype=--4probe
obsc=$1
track=$2
nfiles=$3

if [[ $nfiles -eq "" ]]; then
    echo "usage: starprobs <nfiles>"
    exit
fi

echo -e "grid\tconfig\twfsmag\tgdrmag\tprob"
echo -e "----\t------\t------\t------\t----"

for wfsmag in 13 14 15 16 17 18 19; do
    for gdrmag in 13 14 15 16 17 18 19; do
        echo -ne "grid\tdgnf\t$wfsmag\t$gdrmag\t"
        ./skycov $simtype $obsc $track --noprint $wfsmag $gdrmag 0 $nfiles
    done
done
