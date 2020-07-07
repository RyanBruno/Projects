#!/usr/bin/env sh
# Profile file. Runs on login.

# XDG Paths
export XDG_CONFIG_HOME="$HOME/.config"
export XDG_CACHE_HOME="$HOME/.cache"
export XDG_DATA_HOME="$HOME/.local/share"

# Xauthority
export XAUTHORITY="$XDG_RUNTIME_DIR"/Xauthority

# Adds `~/.local/bin/` and all subdirectories to $PATH
export PATH=$PATH:~/TheRepository/scripts/
export EDITOR="vim"
export TERMINAL="st"
export BROWSER="firefox"
export READER="zathura"
export FILE="vifm"

# less/man colors
export LESS=-R
export LESS_TERMCAP_mb="$(printf '%b' '[1;31m')"; a="${a%_}"
export LESS_TERMCAP_md="$(printf '%b' '[1;36m')"; a="${a%_}"
export LESS_TERMCAP_me="$(printf '%b' '[0m')"; a="${a%_}"
export LESS_TERMCAP_so="$(printf '%b' '[01;44;33m')"; a="${a%_}"
export LESS_TERMCAP_se="$(printf '%b' '[0m')"; a="${a%_}"
export LESS_TERMCAP_us="$(printf '%b' '[1;32m')"; a="${a%_}"
export LESS_TERMCAP_ue="$(printf '%b' '[0m')"; a="${a%_}"

# Wallpaper
if [ -f "$HOME/.config/wallpaper/current" ]
then
    export BACKGROUND="$HOME/.config/wallpaper/current"
else
    [ -f "$HOME/.config/wallpaper/default" ] && export BACKGROUND="$HOME/.config/wallpaper/default"
fi

#!/usr/bin/env bash
stty -ixon # Disable ctrl-s and ctrl-q.
HISTSIZE= HISTFILESIZE= # Infinite history.

# Set the prompt
export PS1="\[$(tput bold)\]\[$(tput setaf 1)\][\[$(tput setaf 3)\]\u\[$(tput setaf 2)\]@\[$(tput setaf 4)\]\h \[$(tput setaf 5)\]\W\[$(tput setaf 1)\]]\[$(tput setaf 7)\]\\$ \[$(tput sgr0)\]"

# Enable vim mode
set -o vi

# If not running interactively, don't do anything
[[ $- != *i* ]] && return

alias mkdir="mkdir -pv" \
    ls="ls -hN --color=auto --group-directories-first" \
    less="less --no-histdups" \
    la="ls -al" \
    ll="ls -l"
