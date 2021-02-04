#!/bin/bash

if [ $# -lt 1 ]
then
    echo "Usage: ./generate.sh [name]"
    exit
fi

NAME=$(echo ${1} | cut -d'/' -f1); export NAME

mkdir -p pages/$NAME/

if [ -f $NAME/index.md ]
then

    INDEX_SRC=$(cat $NAME/index.md); export INDEX_SRC

    INDEX=$(pandoc -t html $NAME/index.md); export INDEX
fi

find templates/ -type f |
    cut -d'/' -f 2 |
    cut -d'.' -f 1 |
while read -r TEMPLATE
do
    export TEMPLATE
    cat templates/${TEMPLATE}.html | envsubst > pages/$NAME/${TEMPLATE}.html
done
