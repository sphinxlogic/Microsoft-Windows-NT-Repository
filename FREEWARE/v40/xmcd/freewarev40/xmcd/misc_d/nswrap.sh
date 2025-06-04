#!/bin/sh
#
# @(#)nswrap.sh	6.3 98/03/23
#
# Shell script wrapper to allow xmcd to invoke the Netscape browser that
# is located on a remote host.
#
# Usage: nswrap.sh [-remote | -direct] [-auth type] host netscape_path args...
#    where,
#    -remote		Control a running Netscape session
#    -direct		Start a new Netscape session (default)
#    -auth type		"xhost", "xhost-xterminal", "xauth", "environment"
#			or "none" (the default is "xhost").
#    host		The remote host where Netscape is to be run.
#    netscape_path	the full path to the Netscape executable
#    args		The URL to go to
#
# The DISPLAY environment variable must be set appropriately for your
# X display.
#
#    xmcd  - Motif(tm) CD Audio Player
#    cda   - Command-line CD Audio Player
#    libdi - CD Audio Player Device Interface Library
#
#    Copyright (C) 1993-1998  Ti Kan
#    E-mail: ti@amb.org
#
#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 2 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#
#    This file contains some code adapted from the xrsh script originally
#    by James J. Dempsey (Copyright 1991 by James J. Dempsey <jjd@bbn.com>).
#

localhost=`uname -n`

#
# Fix up DISPLAY environment variable if necessary
#
if [ -z "$DISPLAY" ]
then
	echo "The DISPLAY environment variable is not set." >&2
	exit 1
elif [ `expr "$DISPLAY" : ':[0-9]*\.*[0-9]*'` -gt 0 ]
then
       DISPLAY="${localhost}${DISPLAY}"
elif [ `expr "$DISPLAY" : 'unix:[0-9]*\.*[0-9]*'` -gt 0 ]
then
	DISPLAY=`echo $DISPLAY | sed "s/unix/$localhost/"`
elif [ `expr "$DISPLAY" : 'localhost:[0-9]*\.*[0-9]*'` -gt 0 ]
then
	DISPLAY=`echo $DISPLAY | sed "s/localhost/$localhost/"`
fi

# Some platforms have rsh as "restricted shell" and ucb rsh is remsh or rcmd
if [ -r /usr/bin/remsh ]
then
	RSH=remsh
elif [ -f /usr/bin/rcmd ]	# SCO Unix uses "rcmd" instead of rsh
then
	RSH=rcmd
else
	RSH=rsh
fi

default_auth_type=xhost	# sites might want to change this
authenv=
xhostvar=		# used by auth type xhost-xterminal
mode=dir

# process command line arguments
foundarg=
until [ "$foundarg" = "no" ]
do
	foundarg=no
	case $1 in
	-remote)
		mode=rmt; foundarg=yes; shift;
		;;
	-direct)
		mode=dir; foundarg=yes; shift;
		;;
	-auth)
		shift; XRSH_AUTH_TYPE=$1; foundarg=yes; shift;
		;;
	esac
done

clienthost="$1"; shift	# The full remote host name
command=$1
if [ -z "$command" ]	# default command to xterm if none specified
then
	return 1
else
	shift
fi

# Grab the arguments to the command here so that we don't have to worry
# about restoring them
xcmdargs=$@
if [ -n "$xcmdargs" ]
then
	if [ $mode = rmt ]
	then
		command="$command -remote 'openURL($xcmdargs)'"
	else
		command="$command $xcmdargs"
	fi
fi

# Use $XRSH_AUTH_TYPE to determine whether to run xhost, xauth, 
# propagate $XAUTHORITY to the remote host, or do nothing
case ${XRSH_AUTH_TYPE-$default_auth_type} in
xhost)      
	xhost +$clienthost >/dev/null 2>&1
	;;
xhost-xterminal)
	# If run on an X terminal, rsh to the XDMCP host to run xhost
	# if this is the first time, we are on the XDMCP host and 
	# we can just run xhost.
	if [ "$XHOST" = "" ]
	then
		xhostvar="XHOST=$localhost; export XHOST;"
		xhost +$clienthost >/dev/null 2>&1 
	else
		xhostvar="XHOST=$XHOST; export XHOST;"
		xhostcmd="DISPLAY=$DISPLAY; export DISPLAY; \
			exec /usr/bin/X11/xhost +$clienthost"
		$RSH "$XHOST" exec /bin/sh -cf "\"$xhostcmd\"" </dev/null
	fi
;;
xauth)
	# the "xauth remove" is theoretically unnecessary below,
	# but due to a bug in the initial X11R5 (fixed in fix-09) xauth,
	# entries for MIT-MAGIC-COOKIE-1 get lost if
	# you do merge twice without the remove.  The remove
	# command can be removed when xauth gets fixed.
	xauth extract - $DISPLAY | \
	$RSH $clienthost xauth remove $DISPLAY \;xauth merge - >/dev/null 2>&1
	;;
environment)
	authenv="XAUTHORITY=$XAUTHORITY; export XAUTHORITY;"
	;;
none)
	;;
*)
	return 1
	;;
esac

#
# Run Netscape on the remote host
#
if [ $mode = rmt ]
then
	msg=`echo "DISPLAY=$DISPLAY; export DISPLAY; $authenv $xhostvar $command" ' 2>&1 </dev/null' | $RSH $clienthost /bin/sh`
	if echo $msg | grep "not running on display" >/dev/null 2>&1
	then
		exit 1
	fi
	exit 0
fi

echo "DISPLAY=$DISPLAY; export DISPLAY; $authenv $xhostvar $command" ' >/dev/null 2>&1 </dev/null &' | $RSH $clienthost /bin/sh &

exit $?

