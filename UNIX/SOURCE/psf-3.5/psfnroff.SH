#! /bin/sh
# $Id: psfnroff.SH,v 3.3 1992/02/07 15:10:57 ajf Exp ajf $
#	Print nroff'ed pages using psf filters
#
#	The "-r 3" option in psfbs below removes 3 blank lines between
#	pages (to allow correct printing in the imageable area of the
#	postscript printer).  Different printers may require different
#	number of lines to remove.  (see psfbs.1)

#	If double sided printing is desired, use the "-d" option
#	in psf below.

#	If European paper sizes are used, possibly "force" psf to
#	use 63 lines of output.  This may (i.e. untested) work:
#
#   	nroff $OPTIONS $i | psfbs -i -r 3 | psf -l63 -c80 -2 | PRINTER $banneropt
#
#	If margin offsets are desired for hole-punching, try:
#
#   	nroff $OPTIONS $i | psfbs -i -r 3 | psf -M 0.7,,,,i -l63 -c80 -2 | PRINTER $banneropt
#

usage () {
	echo "Usage:  $0 [nroff options] doc.nroff doc.nroff ..."
	echo "  e.g.  $0 -ms document.ms"
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
-*)	;;
*)	usage
	;;
esac

case PRINTER in
lpr)	nobanner="-h"
	;;
lp)	nobanner="-o nobanner"
	;;
esac

banneropt=""

for i in $*
do
    case $i in
    -*)      OPTIONS=`echo $OPTIONS $i`
	     shift;;
	     
    [1-z]*)  if test -f $i
    	     then
   		  nroff $OPTIONS $i | psfbs -i -r 3 | psf -2 | PRINTER $banneropt
		  banneropt=$nobanner
    	     else
    	     	  echo "$0: file $i does not exist."
    	     fi
    	     ;;
    esac
done
exit 0
