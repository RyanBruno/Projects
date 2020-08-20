#!/bin/bash

T=$(mktemp tmp.XXXXXXXXXX)
calcurse -c $T -i $1
calcurse -c $T && calcurse -c $T -xical --export-uid > $1
rm $T
