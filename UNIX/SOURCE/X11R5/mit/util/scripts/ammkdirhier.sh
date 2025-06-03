#!/bin/sh
#
# $XFree86: mit/util/scripts/ammkdirhier.sh,v 1.1 1993/03/20 02:36:20 dawes Exp $
#
# Create a hierarchy of directories
#
# Usage: ammkdirhier binary-directory directories ...
#
abin=$1
shift

#
# Default soap mask for directories
#
SPMASK=0xFF:2:4
export SPMASK

#
# All the references to /public... are changed in /super...
#
for f in $*; do
    parts=`echo $f | sed 's,\(.\)/\(.\),\1 \2,g' | sed 's,/$,,'`;
    path="";
    for p in $parts; do
	if [ x"$path" = x ]; then
	    if [ x$p = x/public ]; then
		dir=/super
	    else
		dir=$p;
	    fi
	else
	    dir=$path/$p;
	fi;
	if $abin/std_info $dir >/dev/null 2>&1; then
	    : nothing
	else
	    echo + mkd $dir; 
	    $abin/mkd $dir;
	fi
	path=$dir;
    done;
done

