#!/bin/zsh

/bin/zsh ~/.dwm/scripts/dwm_refresh.sh &

fcitx &

picom -b

oneko &

screenkey &

#redshift &

feh --bg-fill --randomize --no-fehbg ~/.dwm/wallpaper/18.jpg

~/.dwm/scripts/autostart_wait.sh

