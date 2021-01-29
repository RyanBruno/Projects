#!/bin/bash

if [ $# -lt 1 ]
then
    echo "Usage: ./generate.sh [name]"
    exit
fi

NAME=$(echo ${1} | cut -d'/' -f2); export NAME

if [ ! -d pages/$NAME/ ]
then
    mkdir pages/$NAME/
fi

if [ -f uploads$1 ]
then
    sed -e 's/\r//' -i uploads$1
    mv uploads$1 markdown/$NAME
fi

if [ -f markdown/$NAME/index.md ]
then

    INDEX_SRC=$(cat markdown/$NAME/index.md); export INDEX_SRC

    INDEX=$(pandoc -t html markdown/$NAME/index.md); export INDEX
fi

find templates/ -type f |
    cut -d'/' -f 2 |
    cut -d'.' -f 1 |
while read -r TEMPLATE
do
    export TEMPLATE
    cat templates/${TEMPLATE}.html | envsubst > pages/$NAME/${TEMPLATE}.html
done
