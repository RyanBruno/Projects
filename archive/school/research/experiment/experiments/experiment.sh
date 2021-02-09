#!/bin/sh

#USAGE: ./node NODEID MERGE_RATE PEERS_LEN DURATION EAGER_RATE
usage="Usage ./demo.sh FILE_NAME MERGE_RATE NUM_NODES DURATION EAGER_RATE EXP_NAME"
demo ()
{
    [ $# -lt 6 ] && echo $usage && exit
    [ ! -d $6-data ] && ( mkdir $6-data || exit )
    [ -e $6-data/$1.csv ] && echo "Data file already exists!" && exit
    echo '"node_id","duration","items"' > $6-data/$1.csv
    [ -e $6-data/$1.txt ] && echo "Text file already exists!" && exit
    echo -e "MERGE_RATE=$2\nPEERS_LEN=$3" > $6-data/$1.txt
    echo -e "DURATION=$4\nEAGER_RATE=$5" >> $6-data/$1.txt

    rm -rf tmp/
    mkdir tmp/

    i=0
    while [ $i -lt $3 ];
    do
        ./node $i $2 $3 $4 $5 2>> err.txt > tmp/$1-$i.csv &
        i=$(( i+1 ))
    done
    sleep $(( $4+65 ))
    cat tmp/* >> $6-data/$1.csv
}

[ ! -e $1 ] && echo "Input file does not exists!" && exit
#[ -e $1-data ] && echo "Data dir exists!" && exit

cat $1 | while read -r line
do
    $line $1
done

rm -rf tmp/
