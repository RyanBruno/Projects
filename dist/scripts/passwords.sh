#!/bin/bash

#st -i -g '60x12' -e sh -c ""
passphrase=""

while true
do
    temp=$(cat ~/TheRepository/scripts/eff_large_wordlist.txt | fzf) &&

    if [ $temp != "quit" ]
    then
        passphrase="${passphrase} ${temp}"
        continue
    fi
    break;
done &&
find ~/Private/passwords | fzf | xargs cat | openssl aes256 -a -d -pbkdf2 -k $passphrase | head -n1 | xclip && passphrase= && sh
