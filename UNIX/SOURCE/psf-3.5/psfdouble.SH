#! /bin/sh
# $Id: psfdouble.SH,v 3.3 1992/02/07 15:10:57 ajf Exp ajf $
#	An example of setting up a script for doublesided printing
#
#	usage:		psfdouble file
#		where	  "file" is the file to be printed
#			  	 double sided, two-up.
#
#	The -d option for psf causes file "psfbook.psd" to be created
#	which is then processed by psfdoub to actually print double sided.

usage () {
	echo "Usage:  $0 file.nam"
	echo "  where   file.nam = name of file to be printed"
	echo "                     double sided, two-up"
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


# begin double sided printing.

psf -2d $1

echo Press ENTER to send side 1 to the printer.
read X

psfdoub -1 psfbook.psd | PRINTER

echo Refeed the paper for the 2nd side and press ENTER.
read X

psfdoub -2 psfbook.psd | PRINTER

rm psfbook.psd
