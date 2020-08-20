#!/bin/bash

qemu-system-x86_64 \
    -drive file=~/VMs/windows/windows-base.img,format=qcow2 \
    -audiodev id=pa,driver=pa \
    -soundhw hda \
    -usb -device usb-tablet \
    -device usb-ehci,id=ehci \
    -device usb-host,bus=ehci.0,vendorid=0x05c8,productid=0x038e \
    -cpu host \
    -snapshot \
    -smp 3 \
    -m 4G \
    -enable-kvm 

    #-cdrom ~/Torrents/Win10_1909_English_x64.iso \
    #-boot menu=on \
