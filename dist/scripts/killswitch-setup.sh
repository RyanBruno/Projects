#!/bin/bash

ufw --force reset
ufw default deny incoming
ufw default deny outgoing
ufw allow out on tun0
ufw allow out to any port 53,1194 proto udp
ufw status verbose
ufw enable
