#!/bin/sh
#
# @(#)ncsarmt.sh	6.3 98/03/22
#
# Remote control a running NCSA Mosaic session owned by the same user
# Usage: ncsarmt http://some.host.com/foo.html
#
# If no running Mosaic session is found, ncsarmt will exit with status 1.
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

# Use Sysv echo if possible
if [ -x /usr/5bin/echo ]				# SunOS SysV echo
then
	ECHO=/usr/5bin/echo
elif [ -z "`(echo -e a) 2>/dev/null | fgrep e`" ]	# GNU bash, etc.
then
	ECHO="echo -e"
else							# generic SysV
	ECHO=echo
fi

# Check command line args
if [ $# -ne 1 ]
then
	$ECHO "Usage: $0 URL"
	exit 2
fi

# Who am i?
myname=`id 2>/dev/null | sed -e 's/^uid=[0-9]*(//' -e 's/).*$//'`

# Determine if a Mosaic process owned by me is running on the system

# Try BSD-style ps
mpid=`/bin/ps x 2>/dev/null | grep Mosaic | \
	grep -v grep | head -1 | awk '{ print $1 }'`

if [ -z "$mpid" ]
then
	# Try SysV-style ps
	mpid=`/bin/ps -fu $myname 2>/dev/null | grep Mosaic | \
		grep -v grep | head -1 | awk '{ print $2 }'`
fi

if [ -z "$mpid" ]
then
	# Not found
	exit 1
fi

rm -f /tmp/Mosaic.$mpid
$ECHO "goto\n$1" >/tmp/Mosaic.$mpid 2>/dev/null
if [ $? -ne 0 ]
then
	# Cannot write remote control file
	exit 3
fi

# Remote control Mosaic
kill -USR1 $mpid 2>/dev/null
if [ $? -ne 0 ]
then
	# Cannot signal Mosaic
	rm -f /tmp/Mosaic.$mpid
	exit 4
fi

sleep 5
rm -f /tmp/Mosaic.$mpid
exit 0

