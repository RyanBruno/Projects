#!/bin/bash

## Fix Perms
chmod -R 700 Objects/
find Objects/ -type f | while read -r line; do chmod u-wx "$line"; done

## Encrypt and move Objects
echo Enter Passpharse: 

read -s KEY
openssl aes256 -d -pbkdf2 -in Keyfile.aes -k $KEY -out /dev/null &&
find Objects/ -type f | while read -r line; 
do
    OUT=Archive/$(echo $line | sed -e "s/^Objects\///" -e "s/\//\./g").aes
    [ -e "$OUT" ] || echo $line 
    [ -e "$OUT" ] || openssl aes256 -d -pbkdf2 -in Keyfile.aes -k $KEY | openssl aes256 -pbkdf2 -in "$line" -out $OUT -pass stdin
done

KEY=
KF=

## Fix Perms
chmod -R 700 Archive/
find Archive/ -type f | while read -r line; do chmod u-wx "$line"; done

## Share objects with blackbird
#scp -r 192.168.1.21:Archive/* Archive/
#scp -r 192.168.1.21:Objects/* Objects/
#scp -r Archive/* 192.168.1.21:Archive/
#scp -r Objects/* 192.168.1.21:Objects/

