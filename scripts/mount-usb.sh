#!/bin/sh

cryptsetup open /dev/sdc1 usb && mount /dev/mapper/usb /mnt
