
if [ $# -lt 2 ]
then
    echo "Usage: ./create-commit.sh <path> <parent commit hash>"
    exit
fi

#### Begin: NEEDS to be atomic

# Checkout
git checkout -b temp-branch $2

# Add
OBJECT_HASH=$(sed -e 's/\r//' - | git hash-object --stdin -w)
git update-index --add --cacheinfo 100644 \
    ${OBJECT_HASH} $1

# Commit
git commit -m "Commit"

# Checkout
git checkout -f master

## TODO broadcast commit

#../merge-commit.sh

#### END: NEEDS to be atomic

