#!/bin/sh
#
killall playmus

# Setup PC graphic fonts with a special console font (t)
reset
setfont -v /lib/kbd/consolefonts/iso01.16; setfont -v /lib/kbd/consolefonts/t

rpgclient
