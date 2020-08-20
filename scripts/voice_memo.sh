#!/bin/sh

#*.mkv works

ffmpeg -f alsa -ac 2 -i pulse -acodec pcm_s16le -preset ultrafast $1
