#!/bin/sh

GRAB_NAME=$(date +%s)

ffmpeg -f x11grab -video_size 1920x1080 -i $DISPLAY -vframes 1 ~/Objects/Grabs/${GRAB_NAME}.png
sxiv ~/Objects/Grabs/${GRAB_NAME}.png &
xclip -t image/png ~/Objects/Grabs/${GRAB_NAME}.png 
