#! /bin/sh
# $Id: psfmbox.SH,v 3.3 1992/02/07 15:10:57 ajf Exp ajf $
#	Print multiple mail-box files using psf filters

#	This prints a file name header with date at the top of 
#	each mailbox page for identification purposes.
#	However, each mailbox print becomes a separate print job...
#
#	The mail may be printed 1-up, 2-up, or 4-up as you see fit.
#	Change the paramter ----+
#                               |
#		psfmail "$1" | psf -4x -H "$1" | PRINTER

usage () {
	echo "Usage:  $0 mbox mbox ... "
	echo "  e.g.  $0 /usr/mail/tony"
	exit 0
}

case $# in
0)	usage
	;;
esac
case $1 in
-)	usage
	;;
--)	usage
	;;
esac

while test "$1" != ""
do
	if test -f $1
	then
		psfmail "$1" | psf -c 80 -4xE -H "$1" | PRINTER
	else
		echo "$0: mail box $1 does not exist."
	fi
	shift
done
exit 0
