#!/bin/sh
# usage:
#	cfstrip <infile >outfile
# Bruce Barnett
#
# this file strips out all comments from a sendmail.cf file
# it also converts 
#	<tab>[whitespace] 	to 	<tab>
#	<space>[whitespace]	to	<space>
#	<space>$		to	$
#	<space><		to	<
#	<space>@		to	@
case  $#  in
	0)
		;;
	*)
		echo no arguments are allowed;
		exit 1
		;;
esac
sed -e '
s/^#.*//
s/^\(R[^	]*[	][	]*[^	]*\)[	]*.*$/\1/
s/^\(R[^	]*[	][	]*[^	]*\)[	]*$/\1/
s/^\(R[^	]*[	][	]*[^	]*\)$/\1/
s/	[	 ]*/	/g
s/ [	 ]*/ /g
s/ \$/$/g
s/ </</g
s/ @/@/g
# convert multiple tabs into one tab
s/		*/	/g
# this one will really remove most spaces
s/^R\([^ ]*\) \([^ ]*\)/R\1\2/g
# remove a space in "T root"
s/^T */T/
/^[ 	]*$/d
' 
