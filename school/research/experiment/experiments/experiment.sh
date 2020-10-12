#!/bin/sh

#USAGE: ./node NODEID MERGE_RATE OPERATION_RATE ADD_TO_REM_RATIO PEERS_LEN DURATION EAGER_RATE
#usage="Usage ./demo.sh FILE_NAME NUM_NODES MERGE_RATE OPERATION_RATE ADD_TO_REM_RATIO DURATION EAGER_RATE EXP_NAME"
demo ()
{
    [ $# -lt 7 ] && echo $usage && exit
    [ ! -d $8-data ] && mkdir $8-data || exit
    [ -e $8-data/$1.csv ] && echo "Data file already exists!" && exit
    echo '"node_id","duration","items"' > $8-data/$1.csv
    [ -e $8-data/$1.txt ] && echo "Text file already exists!" && exit
    echo -e "MERGE_RATE=$3\nOPERATION_RATE=$4\nADD_TO_REM_RATIO=$5" > $8-data/$1.txt
    echo -e "DURATION=$6\nEAGER_RATE=$7" >> $8-data/$1.txt

    rm -rf tmp/
    mkdir tmp/

    i=0
    while [ $i -lt $2 ];
    do
        ./node $i $3 $4 $5 $2 $6 $7 2>> err.txt > tmp/$1-$i.csv &
        i=$(( i+1 ))
    done
    sleep $(( $6+65 ))
    cat tmp/* >> $8-data/$1.csv
}

[ ! -e $1 ] && echo "Input file does not exists!" && exit
[ -e $1-data ] && echo "Data dir exists!" && exit

cat $1 | while read -r line
do
    $line $1
done

rm -rf tmp/
