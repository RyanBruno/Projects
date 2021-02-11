
git merge temp-branch

if [ ! $? -eq 0 ]
then
    git commit -am "Merge"
fi

git branch -d temp-branch

## TODO broadcast commit(s)
