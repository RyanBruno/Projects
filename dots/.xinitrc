#!/usr/bin/env sh

# Screen layout config
# Only link the relevant script
[ -f "$HOME/.config/arandr/$HOSTNAME.sh" ] && . ~/.config/arandr/$HOSTNAME.sh

xhost +

# Status Bar
while true; do
    BAT=""
    [ -e /sys/class/power_supply/BAT0/capacity ] && BAT="Bat: $(cat /sys/class/power_supply/BAT0/capacity) |"
    xsetroot -name "$BAT $( date +"%F %R" )"
    sleep 1m
done &

# Wallpaper
xwallpaper --zoom .config/wallpaper/default

#exec /usr/bin/i3
exec /usr/local/bin/dwm
