
cd git-testing

../create-commit.sh $1 $2

../merge-commit.sh

../generate.sh $1
