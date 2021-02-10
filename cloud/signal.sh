
#./signal <path> <filename> <key>=<value>

echo $1$2
cat > bills/$1$2

cd "bills/"
../listing.sh $1
