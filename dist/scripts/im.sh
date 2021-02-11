#!/bin/sh

mkfifo $1
tail -f $1
rm $1
