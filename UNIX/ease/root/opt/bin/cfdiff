#!/bin/sh
# Bruce Barnett
# usage:
# cfdiff file1.cf file2.cf
# this removes all comments from the cf file
case  $#  in
	0|1)
	echo usage: cfdiff file1 file2;
	exit 1
	;;
esac
[ ! -f $1 ] && {
	echo file $1 does not exist;
	exit 1;
};
[ ! -f $2 ] && {
	echo file $2 does not exist;
	exit 1;
}
# if SunOS, use the -b -w option
DIFF="diff -b"
ARG1=`basename $1`
ARG2=`basename $2`
[ $ARG1 = $ARG2 ] && {
	ARG2=$ARG2-
}
cfstrip <$1>/tmp/$ARG1
cfstrip <$2>/tmp/$ARG2

cd /tmp
${DIFF} $ARG1 $ARG2

/bin/rm /tmp/$ARG1 /tmp/$ARG2
