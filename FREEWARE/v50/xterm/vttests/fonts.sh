#!/bin/sh
# $XFree86: xc/programs/xterm/vttests/fonts.sh,v 1.1 1999/04/11 13:11:43 dawes Exp $
#
# -- Thomas Dickey (1999/3/27)
# Demonstrate the use of dynamic colors by setting the background successively
# to different values.

ESC=""
CMD='echo'
OPT='-n'
SUF=''
TMP=/tmp/xterm$$
for verb in print printf ; do
    rm -f $TMP
    eval '$verb "\c" >$TMP || echo fail >$TMP' 2>/dev/null
    if test -f $TMP ; then
	if test ! -s $TMP ; then
	    CMD="$verb"
	    OPT=
	    SUF='\c'
	    break
	fi
    fi
done
rm -f $TMP

exec </dev/tty
old=`stty -g`
stty raw -echo min 0  time 5

$CMD $OPT "${ESC}]50;?${SUF}" > /dev/tty
read original

stty $old
original="${original}${SUF}"

trap '$CMD $OPT "$original" >/dev/tty; exit' 0 1 2 5 15
F=1
D=1
T=6
while true
do
    $CMD $OPT "${ESC}]50;#$F${SUF}" >/dev/tty
    #sleep 1
    if test .$D = .1 ; then
	test $F = $T && D=-1
    else
	test $F = 1 && D=1
    fi
    F=`expr $F + $D`
done
