#!/bin/sh

GRAB_NAME=$(date +%s)

mkdir -p /tmp/Grabs
ffmpeg -f x11grab -video_size 1920x1080 -i $DISPLAY -vframes 1 /tmp/Grabs/${GRAB_NAME}.png
sxiv /tmp/Grabs/${GRAB_NAME}.png &
xclip -t image/png /tmp/Grabs/${GRAB_NAME}.png 
