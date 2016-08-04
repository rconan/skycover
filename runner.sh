#!/bin/bash

nfiles=$1

if [[ $nfiles -eq "" ]]; then
    echo "usage: starprobs <nfiles>"
    exit
fi

echo -e "grid\tconfig\twfsmag\tgdrmag\tprob"
echo -e "----\t------\t------\t------\t----"

for wfsmag in 13 14 15 16 17 18 19; do
    for gdrmag in 13 14 15 16 17 18 19; do
        echo -ne "grid\tdgnf\t$wfsmag\t$gdrmag\t"
        ./skycov $wfsmag $gdrmag $nfiles
    done
done
