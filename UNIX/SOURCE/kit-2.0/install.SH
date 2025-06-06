case $CONFIG in
'')
	if test ! -f config.sh; then
		ln ../config.sh . || \
		ln ../../config.sh . || \
		ln ../../../config.sh . || \
		(echo "Can't find config.sh."; exit 1)
	fi 2>/dev/null
	. config.sh
	;;
esac
case "$0" in
*/*) cd `expr X$0 : 'X\(.*\)/'` ;;
esac
echo "Extracting install (with variable substitutions)"
$spitshell >install <<!GROK!THIS!
$startsh
# @(#) Installing script accepting bsd-style arguments
# Derived from a script found in the X11R4 distribution.

# $Id: install.SH,v 2.0.1.3 92/05/02 14:12:31 ram Exp $
#
# $Log:	install.SH,v $
# Revision 2.0.1.3  92/05/02  14:12:31  ram
# patch16: minor typo fix in mkdir_p
# 
# Revision 2.0.1.2  91/07/30  17:20:35  ram
# patch6: new install.SH script
# 
# Revision 2.0.1.1  91/05/27  14:20:35  ram
# patch5: mkdir_p is now eval'ed instead of being a function
# patch5: explicitely exit with a zero status when all is done
# 
# Revision 2.0  91/02/19  15:49:42  ram
# Baseline for first official release.
# 

chown='$chown'
chmod='$chmod'
chgrp='$chgrp'
rm='$rm'
mv='$mv'
test='$test'
sed='$sed'
!GROK!THIS!
$spitshell >>install <<'!NO!SUBS!'

mode=""
dst=""
src=""
dostrip=""
newdir=""
uid=""
gid=""

# simulates mkdir -p
mkdir_p='
name=$1;
create="";
while $test $name; do
	if $test ! -d "$name"; then
		create="$name $create";
		name=`echo $name | $sed -e "s|^[^/]*$||"`;
		name=`echo $name | $sed -e "s|\(.*\)/.*|\1|"`;
	else
		name="";
	fi;
done;
for file in $create; do
	mkdir $file && $test $verbose &&
	echo "install: created directory $file" >&2;
done
'

verbose=''

while $test x$1 != x
do
	case $1 in 
	-c) shift
		continue
		;;
	-m) mode="$2 "
		shift
		shift
		continue
		;;
	-o) uid="$2 "
		shift
		shift
		continue
		;;
	-g) gid="$2 "
		shift
		shift
		continue
		;;
	-s) dostrip="strip"
		shift
		continue
		;;
	-d) newdir="$newdir$2 "
		shift
		shift
		continue
		;;
	-v) verbose='true'
		shift
		;;
	*) if $test x$src = x
		then
			src=$1
		else
			dst=$1
		fi
		shift
		continue
		;;
	esac
done

# if -d option is used, we have to create the path given
if $test ! x$newdir = x
then
	for i in $newdir
	do
		set x $i
		shift
		eval $mkdir_p
	done
	exit 0		# -d is the only action
fi

if $test x$src = x
then
	echo "install: no input file specified" >&2
	exit 1
fi

if $test x$dst = x
then
	echo "install: no destination specified" >&2
	exit 1
fi

srcbase=`basename $src`
dstbase=`basename $dst`

# If the destination is a directory, the target name is srcbase...
if $test -d $dst; then
	dstbase=$srcbase
else
	dst="`echo $dst | sed 's,^\(.*\)/.*$,\1,'`"
	if $test ! -d $dst; then
		dstbase=$dst
		dst="."
	fi
fi

# If the src has a directory, extract the dir name...
if $test "$src" != "$srcbase" -a "$src" != "./$srcbase"; then
	src="`echo $src | sed 's,^\(.*\)/.*$,\1,'`"
else
	src="."
fi

# dst is the destination directory and dstbase the base name.
# srcbase is the base name of source and src the source dir.

srcpth=`(cd $src; pwd)`/$srcbase
destpth=`(cd $dst; pwd)`/$dstbase
if $test x$srcpth = x$destpth; then
	$test $verbose && \
	echo "install: destination and source are identical"
	exit 0
fi

# Do the install
(
	cd $src

	if $test -f $dst/$dstbase; then
		$rm -f $dst/$dstbase && $test $verbose &&
		echo "install: $dst/$dstbase removed"
	fi
	if $test -f $dst/$dstbase; then
		$mv $dst/$dstbase $dst/OLD$dstbase && $test $verbose &&
		echo "install: $dst/$dstbase renamed as OLD$dstbase"
	fi

	cp $srcbase $dst/$dstbase && $test $verbose &&
	echo "install: $srcbase installed as $dst/$dstbase"

	if $test ! x$dostrip = x; then
		strip $dst/$dstbase 2>/dev/null && $test $verbose &&
		echo "install: stripped $dst/$dstbase"
	fi

	if $test ! x$uid = x; then
		$chown $uid $dst/$dstbase
	fi
	if $test ! x$gid = x; then
		$chgrp $gid $dst/$dstbase
	fi
	if $test ! x$mode = x
	then
		$chmod $mode $dst/$dstbase
	fi
)

exit 0
!NO!SUBS!
chmod 755 install
$eunicefix install
