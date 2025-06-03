XCOMM!/bin/sh

XCOMM $XFree86: mit/clients/xinit/startx.cpp,v 2.0 1993/10/13 15:49:15 dawes Exp $
XCOMM $XConsortium: startx.cpp,v 1.4 91/08/22 11:41:29 rws Exp $
XCOMM 
XCOMM This is just a sample implementation of a slightly less primitive 
XCOMM interface than xinit.  It looks for user .xinitrc and .xserverrc
XCOMM files, then system xinitrc and xserverrc files, else lets xinit choose
XCOMM its default.  The system xinitrc should probably do things like check
XCOMM for .Xresources files and merge them in, startup up a window manager,
XCOMM and pop a clock and serveral xterms.
XCOMM
XCOMM Site administrators are STRONGLY urged to write nicer versions.
XCOMM 

userclientrc=$HOME/.xinitrc
userserverrc=$HOME/.xserverrc
if [ x"$XWINHOME" != x ]; then
    if [ x"$XWINHOME" = x/usr ]; then
        BIN_DIR=/usr/bin/X11
    else
        BIN_DIR=$XWINHOME/bin
    fi
    XINIT_DIR=$XWINHOME/lib/X11/xinit
else
    BIN_DIR=BINDIR
    XINIT_DIR=XINITDIR
fi
sysclientrc=$XINIT_DIR/xinitrc
sysserverrc=$XINIT_DIR/xserverrc
clientargs=""
serverargs=""

if [ -f $userclientrc ]; then
    clientargs=$userclientrc
else if [ -f $sysclientrc ]; then
    clientargs=$sysclientrc
fi
fi

if [ -f $userserverrc ]; then
    serverargs=$userserverrc
else if [ -f $sysserverrc ]; then
    serverargs=$sysserverrc
fi
fi

whoseargs="client"
have_server=0
while [ "x$1" != "x" ]; do
    case "$1" in
	/''*|\.*)	if [ "$whoseargs" = "client" ]; then
		    clientargs="$1"
		else
		    serverargs="$1"
		    have_server=1
		fi ;;
	--)	whoseargs="server" ;;
	*)	if [ "$whoseargs" = "client" ]; then
		    clientargs="$clientargs $1"
		else
		    if [ "$have_server" = 0 ]; then
			if [ -x $BIN_DIR/"$1" ]; then
			    serverargs=$BIN_DIR/"$1"
			    have_server=1
			else
			    serverargs="$serverargs $1"
			fi
		    else
		        serverargs="$serverargs $1"
		    fi
		fi ;;
    esac
    shift
done

xinit $clientargs -- $serverargs

/*
 * various machines need special cleaning up
 */
#ifdef macII
Xrepair
screenrestore
#endif

#ifdef sun
kbd_mode -a
#endif

#if defined(__386BSD__) && !defined(__bsdi__)
kbd_mode -a
#endif
