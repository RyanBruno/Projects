#!/bin/bash

rm /tmp/lock.png
ffmpeg -f x11grab -video_size 1920x1080 -i $DISPLAY -vframes 1 -vf "boxblur=8:1" /tmp/lock.png
i3lock -ti /tmp/lock.png
