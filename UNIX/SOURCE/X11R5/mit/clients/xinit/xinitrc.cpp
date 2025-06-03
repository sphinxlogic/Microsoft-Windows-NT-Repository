XCOMM!/bin/sh
XCOMM $XFree86: mit/clients/xinit/xinitrc.cpp,v 1.3 1993/04/11 13:50:35 dawes Exp $
XCOMM $XConsortium: xinitrc.cpp,v 1.4 91/08/22 11:41:34 rws Exp $

userresources=$HOME/.Xresources
usermodmap=$HOME/.Xmodmap
if [ x"$XWINHOME" != x ]; then
    XINIT_DIR=$XWINHOME/lib/X11/xinit
else
    XINIT_DIR=XINITDIR
fi
sysresources=$XINIT_DIR/.Xresources
sysmodmap=$XINIT_DIR/.Xmodmap

XCOMM merge in defaults and keymaps

if [ -f $sysresources ]; then
    xrdb -merge $sysresources
fi

if [ -f $sysmodmap ]; then
    xmodmap $sysmodmap
fi

if [ -f $userresources ]; then
    xrdb -merge $userresources
fi

if [ -f $usermodmap ]; then
    xmodmap $usermodmap
fi

XCOMM start some nice programs

twm &
xclock -geometry 50x50-1+1 &
xterm -geometry 80x50+494+51 &
xterm -geometry 80x20+494-0 &
exec xterm -geometry 80x66+0+0 -name login
