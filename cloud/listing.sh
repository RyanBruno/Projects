

#$1 = dir

echo $1
LS=$(ls -hlF --group-directories-first $1); export LS
DIR=$1; export DIR


cat ../template.html | envsubst > $1index.html
