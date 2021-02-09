
cd git-testing

../create-commit.sh $1 $3

../merge-commit.sh

../generate.sh $1
