#!/bin/sh

#USAGE: ./node NODEID MERGE_RATE OPERATION_RATE ADD_TO_REM_RATIO PEERS_LEN DURATION EAGER_RATE
usage="Usage ./demo.sh FILE_NAME NUM_NODES MERGE_RATE OPERATION_RATE ADD_TO_REM_RATIO DURATION EAGER_RATE"
[ $# -lt 7 ] && echo $usage && exit
[ -e data/$1.csv ] && echo "Data file already exists!" && exit
[ -e data/$1.txt ] && echo "Text file already exists!" && exit
echo '"node_id","duration","items"' > data/$1.csv
echo -e "MERGE_RATE=$3\nOPERATION_RATE=$4\nADD_TO_REM_RATIO=$5\nDURATION=$6\nEAGER_RATE=$7" > data/$1.txt

i=0
while [ $i -lt $2 ];
do
    ./node $i $3 $4 $5 $2 $6 $7 >> data/$1.csv &
    i=$(( i+1 ))
done

