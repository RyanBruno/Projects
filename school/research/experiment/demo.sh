#!/bin/sh

./node 0 >> out.txt &
./node 1 >> out.txt &
./node 2 >> out.txt &
./node 3 >> out.txt &

#./node 0 &
#./node 1 &
#./node 2 &
#./node 3 &
