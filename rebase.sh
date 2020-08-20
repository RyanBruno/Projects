#!/bin/bash

## Rebase TheRepository
COMMIT=$(git log --pretty=oneline | cut -d' ' -f1 | head -n 1)
git checkout --orphan temp $COMMIT
git commit -m "Release $(date +%x)"
git rebase --onto temp $COMMIT master
git branch -D temp

