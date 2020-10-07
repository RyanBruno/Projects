#!/bin/sh

#USAGE: ./node NODEID MERGE_RATE OPERATION_RATE ADD_TO_REM_RATIO PEERS_LEN DURATION EAGER_RATE
demo ()
{
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
    sleep 360
}


read -r -d '' biglist << EOF
demo 1 2 1 1 2 300 1
demo 2 3 1 1 2 300 1
demo 3 4 1 1 2 300 1
demo 4 5 1 1 2 300 1
demo 5 10 1 1 2 300 1
demo 6 15 1 1 2 300 1
demo 7 20 1 1 2 300 1
demo 8 2 3 1 2 300 1
demo 9 3 3 1 2 300 1
demo 10 4 3 1 2 300 1
demo 11 5 3 1 2 300 1
demo 12 10 3 1 2 300 1
demo 13 15 3 1 2 300 1
demo 14 20 3 1 2 300 1
demo 15 2 10 1 2 300 1
demo 16 3 10 1 2 300 1
demo 17 4 10 1 2 300 1
demo 18 5 10 1 2 300 1
demo 19 10 10 1 2 300 1
demo 20 15 10 1 2 300 1
demo 21 20 10 1 2 300 1
demo 22 2 30 1 2 300 1
demo 23 3 30 1 2 300 1
demo 24 4 30 1 2 300 1
demo 25 5 30 1 2 300 1
demo 26 10 30 1 2 300 1
demo 27 15 30 1 2 300 1
demo 28 20 30 1 2 300 1
EOF
echo "$biglist" | while read -r line
do
    $line
done
exit
demo 1 2 60 1 2 300 1
demo 2 3 60 1 2 300 1
demo 3 4 60 1 2 300 1
demo 4 5 60 1 2 300 1
demo 5 10 60 1 2 300 1
demo 6 15 60 1 2 300 1
demo 7 20 60 1 2 300 1
demo 1 2 1 1 2 300 3
demo 2 3 1 1 2 300 3
demo 3 4 1 1 2 300 3
demo 4 5 1 1 2 300 3
demo 5 10 1 1 2 300 3
demo 6 15 1 1 2 300 3
demo 7 20 1 1 2 300 3
demo 1 2 3 1 2 300 3
demo 2 3 3 1 2 300 3
demo 3 4 3 1 2 300 3
demo 4 5 3 1 2 300 3
demo 5 10 3 1 2 300 3
demo 6 15 3 1 2 300 3
demo 7 20 3 1 2 300 3
demo 1 2 10 1 2 300 3
demo 2 3 10 1 2 300 3
demo 3 4 10 1 2 300 3
demo 4 5 10 1 2 300 3
demo 5 10 10 1 2 300 3
demo 6 15 10 1 2 300 3
demo 7 20 10 1 2 300 3
demo 1 2 30 1 2 300 3
demo 2 3 30 1 2 300 3
demo 3 4 30 1 2 300 3
demo 4 5 30 1 2 300 3
demo 5 10 30 1 2 300 3
demo 6 15 30 1 2 300 3
demo 7 20 30 1 2 300 3
demo 1 2 60 1 2 300 3
demo 2 3 60 1 2 300 3
demo 3 4 60 1 2 300 3
demo 4 5 60 1 2 300 3
demo 5 10 60 1 2 300 3
demo 6 15 60 1 2 300 3
demo 7 20 60 1 2 300 3
demo 1 2 1 1 2 300 10
demo 2 3 1 1 2 300 10
demo 3 4 1 1 2 300 10
demo 4 5 1 1 2 300 10
demo 5 10 1 1 2 300 10
demo 6 15 1 1 2 300 10
demo 7 20 1 1 2 300 10
demo 1 2 3 1 2 300 10
demo 2 3 3 1 2 300 10
demo 3 4 3 1 2 300 10
demo 4 5 3 1 2 300 10
demo 5 10 3 1 2 300 10
demo 6 15 3 1 2 300 10
demo 7 20 3 1 2 300 10
demo 1 2 10 1 2 300 10
demo 2 3 10 1 2 300 10
demo 3 4 10 1 2 300 10
demo 4 5 10 1 2 300 10
demo 5 10 10 1 2 300 10
demo 6 15 10 1 2 300 10
demo 7 20 10 1 2 300 10
demo 1 2 30 1 2 300 10
demo 2 3 30 1 2 300 10
demo 3 4 30 1 2 300 10
demo 4 5 30 1 2 300 10
demo 5 10 30 1 2 300 10
demo 6 15 30 1 2 300 10
demo 7 20 30 1 2 300 10
demo 1 2 60 1 2 300 10
demo 2 3 60 1 2 300 10
demo 3 4 60 1 2 300 10
demo 4 5 60 1 2 300 10
demo 5 10 60 1 2 300 10
demo 6 15 60 1 2 300 10
demo 7 20 60 1 2 300 10
demo 1 2 1 1 2 300 15
demo 2 3 1 1 2 300 15
demo 3 4 1 1 2 300 15
demo 4 5 1 1 2 300 15
demo 5 10 1 1 2 300 15
demo 6 15 1 1 2 300 15
demo 7 20 1 1 2 300 15
demo 1 2 3 1 2 300 15
demo 2 3 3 1 2 300 15
demo 3 4 3 1 2 300 15
demo 4 5 3 1 2 300 15
demo 5 10 3 1 2 300 15
demo 6 15 3 1 2 300 15
demo 7 20 3 1 2 300 15
demo 1 2 10 1 2 300 15
demo 2 3 10 1 2 300 15
demo 3 4 10 1 2 300 15
demo 4 5 10 1 2 300 15
demo 5 10 10 1 2 300 15
demo 6 15 10 1 2 300 15
demo 7 20 10 1 2 300 15
demo 1 2 30 1 2 300 15
demo 2 3 30 1 2 300 15
demo 3 4 30 1 2 300 15
demo 4 5 30 1 2 300 15
demo 5 10 30 1 2 300 15
demo 6 15 30 1 2 300 15
demo 7 20 30 1 2 300 15
demo 1 2 60 1 2 300 15
demo 2 3 60 1 2 300 15
demo 3 4 60 1 2 300 15
demo 4 5 60 1 2 300 15
demo 5 10 60 1 2 300 15
demo 6 15 60 1 2 300 15
demo 7 20 60 1 2 300 15
demo 1 2 1 1 2 300 20
demo 2 3 1 1 2 300 20
demo 3 4 1 1 2 300 20
demo 4 5 1 1 2 300 20
demo 5 10 1 1 2 300 20
demo 6 15 1 1 2 300 20
demo 7 20 1 1 2 300 20
demo 1 2 3 1 2 300 20
demo 2 3 3 1 2 300 20
demo 3 4 3 1 2 300 20
demo 4 5 3 1 2 300 20
demo 5 10 3 1 2 300 20
demo 6 15 3 1 2 300 20
demo 7 20 3 1 2 300 20
demo 1 2 10 1 2 300 20
demo 2 3 10 1 2 300 20
demo 3 4 10 1 2 300 20
demo 4 5 10 1 2 300 20
demo 5 10 10 1 2 300 20
demo 6 15 10 1 2 300 20
demo 7 20 10 1 2 300 20
demo 1 2 30 1 2 300 20
demo 2 3 30 1 2 300 20
demo 3 4 30 1 2 300 20
demo 4 5 30 1 2 300 20
demo 5 10 30 1 2 300 20
demo 6 15 30 1 2 300 20
demo 7 20 30 1 2 300 20
demo 1 2 60 1 2 300 20
demo 2 3 60 1 2 300 20
demo 3 4 60 1 2 300 20
demo 4 5 60 1 2 300 20
demo 5 10 60 1 2 300 20
demo 6 15 60 1 2 300 20
demo 7 20 60 1 2 300 20
EOF
