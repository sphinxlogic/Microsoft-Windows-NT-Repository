#! /bin/sh
#
# This accepts bsd-style install arguments and executes them
#

die()
{
    echo "$*" 1>&2
    exit 1
}

dest=""
src=""
strip="false"
owner=""
group=""
mode="755"

while [ -n "$1" ]
do
    case $1 in 
    -c)	;;

    -m)	mode="$2"
		shift
		;;

    -o) owner="$2"
		shift
		;;

    -g) group="$2"
		shift
		;;

    -s) strip="true"
		;;

    -*)	die "Illegal option"
		;;

    *)	if [ -z "$2" ]
		then
			dest="$1"
			break
		fi

    	src="$src $1"

    	if [ ! -f "$1" -o ! -r "$1" ]
		then
			die "$1: file does not exist or is not readable"
		fi
	;;
    esac

    shift
done

[ -n "$dest" ] || die "No destination specified"

[ -n "$src" ] || die "No file specified"

if [ ! -d "$dest" ]
then
    count=0
    for i in $src
    do
		count=`expr $count + 1`
    done

    if [ "$count" -eq 1 ]
    then
		parent=`dirname $dest`
		if [ -d "$parent" ]
		then
			newname=`basename $dest`
			dest=$parent
		fi
    fi

    [ -n "$newname" ] || die "$dest: No such directory"
fi

# Here's where the real work happens.  Note that on some systems, chown
# clears SUID and SGID bits for non-superusers.  Thus, the chmod has to
# follow chown.  However, under System V, you can not chmod SUID or SGID
# permissions unless you are the owner or superuser.
# If you are in doubt, "su" first!

for i in $src
do
    set -e

    ofile=$dest/${newname:-$i}

    rm -f $ofile

    cp $i $ofile

    if $strip
    then
		strip $ofile
		if i386
		then
			mcs -d $ofile
		fi
    fi

    if [ -n "$group" ]
    then
		chgrp $group $ofile
    fi

    if [ -n "$owner" ]
    then
		chown $owner $ofile
    fi

    chmod $mode $ofile

    set +e
done

exit 0
