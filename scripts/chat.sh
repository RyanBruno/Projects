#!/bin/sh

while read -e line
do
    if [[ $line == *"|"* ]]; then continue; fi
    echo $line >> $1
done

