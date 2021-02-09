
#./signal <path> <filename> <key>=<value>

echo $1$2
cat > data/$1$2

cd "data/"
../listing.sh $1
