#!/bin/bash

rm dist/*
make
FILE=$(echo dist/*.html)
firefox http://127.0.0.1:8080/${FILE}#/nexus/ &
