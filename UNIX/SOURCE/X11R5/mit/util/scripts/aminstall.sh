#!/bin/sh
#
# $XFree86: mit/util/scripts/aminstall.sh,v 1.1 1993/03/20 02:36:18 dawes Exp $
#
# Usage: aminstall binary-directory unix-source amoeba-dest
#

#
# Default soap mask for files
#
SPMASK=0xFF:2:2
export SPMASK

#
# Argument check
#
case $# in
3)	;;
*)	echo "Usage: $0 binary-directory unix-source amoeba-dest" >&2
	exit 1
	;;
esac

#
# Change /public .... into /super (just for installation)
#
dest=$3
stripped=`echo $dest | sed 's:^/public::'`
if [ X$dest != X$stripped ]; then
    dest=/super$stripped
fi

#
# If the file already exists, then delete it
#
INFO=`$1/std_info $dest 2>&1`
case $INFO in
*"not found"*)	;;
*failed*)	;;
*bytes*)	$1/del -f $dest
		;;
/??????)	echo $0: cannot install over directory 1>&2
		exit
		;;
*)		$1/del -d $dest
		;;
esac

#
# Transfer the file to Amoeba
#
$1/tob $2 $dest
