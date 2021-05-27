#!/bin/bash

# Basic snapshot-style rsync backup script 

# Config
OPT="-aPh"
LINK="--link-dest=/mnt/drive/Snapshots/last/"
SRC="/home/$USER/TheRepository /home/$USER/.Private"
SNAP="/mnt/drive/Snapshots/"
LAST="/mnt/drive/Snapshots/last"
date=`date "+%Y-%b-%d:_%T"`

# Run rsync to create snapshot
#rsync $OPT $LINK $SRC ${SNAP}$date

# Remove symlink to previous snapshot
#rm -f $LAST

# Create new symlink to latest snapshot for the next backup to hardlink
#ln -s ${SNAP}$date $LAST

LINK="--link-dest=/mnt/usb/Snapshots/last/"
SRC="/home/$USER/TheRepository /home/$USER/.Private"
SNAP="/mnt/usb/Snapshots/"
LAST="/mnt/usb/Snapshots/last"

# Create new symlink to latest snapshot for the next backup to hardlink
ln -s ${SNAP}$date $LAST

# Run rsync to create snapshot
rsync $OPT $LINK $SRC ${SNAP}$date

# Remove symlink to previous snapshot
rm -f $LAST

# Create new symlink to latest snapshot for the next backup to hardlink
ln -s ${SNAP}$date $LAST
