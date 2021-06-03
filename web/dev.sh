#!/bin/bash

rm dist/*
make
FILE=$(echo ./dist/*.html)
firefox ${FILE}#/nexus/ &
