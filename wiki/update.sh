
if [ $# -lt 1 ]
then
    echo "Usage: ./update.sh <path> [parent commit hash]"
fi

#### NEEDS to be atomic
if [ ! $# -lt 2 ]
then
    git checkout -b temp-branch $2
fi

OBJECT_HASH=$(git hash-object --stdin -w)

git update-index --add --cacheinfo 100644 \
    ${OBJECT_HASH} $1

git commit -m "Commit"

if [ ! $# -lt 2 ]
then
    git checkout -f master
    git merge temp-branch
    if [ ! $? -eq 0 ]
    then
        git commit -am "Merge"
    fi
    git branch -d temp-branch
fi

#### END: NEEDS to be atomic
