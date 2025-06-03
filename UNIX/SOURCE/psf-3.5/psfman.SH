#! /bin/sh
# $Id: psfman.SH,v 3.3 1992/02/07 15:10:57 ajf Exp ajf $
#	Print man pages using psf filters -- guess about chapters,
#	even allow Xenix chapter conventions.
#
#	The "-r 3" option in psfbs below removes 3 blank lines between
#	pages (to allow correct printing in the imageable area of the
#	postscript printer).  Different printers may require different
#	number of lines to remove.  (see psfbs.1)

#	If European paper sizes are used, possibly "force" psf to
#	use 63 lines of output.  This may (i.e. untested) work for A4:
#
#   	psfbs -i -r 3 | psf -l63 -c80 -2 | PRINTER $banneropt
#
#	If margin offsets are desired for hole-punching, try:
#
#	psfbs -i -r3 | psf -l63 -c80 -M 0.7,,,,i | PRINTER $banneropt
#
# set the binary name for man:
# MANBIN="/usr/bin/man"
MANBIN=man

# Some man systems require additional options such as -Tlp
# while other consider it an error.
# manopts=-Tlp

# Printing banners. Suit your preferences, check the "psfbs" line below
# for compliance.

case PRINTER in
lpr)	nobanner="-h"
	;;
lp)	nobanner="-o nobanner"
	;;
esac
banneropt=""


usage () {
	echo "Usage:  $0 [chapter] page page ..."
	echo "  e.g.  $0 vi sh"
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

case $1 in
[CFMS1-9] | CP | CT | DOS | HW | LOCAL | UCB  )  CHAPTER=$1
					         shift;;
esac

tempfile=/tmp/PSF$$
trap "rm -f $tempfile ; exit 0" 1 2 15
while test "$1" != ""
do
	if ( $MANBIN $manopts $CHAPTER "$1" > $tempfile ) then
		psfbs -i -r 3  $tempfile | psf -2  | PRINTER $banneropt
		banneropt=$nobanner
	else
		echo "unknown man page: $1"
	fi
	shift
done
rm $tempfile
exit 0
