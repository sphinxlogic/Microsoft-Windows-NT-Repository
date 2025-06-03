#!	Postscript serial or parallel printer using psf filters
#
#	NOTE: the above line is displayed on the "mkdev lp" printer
#	^^^^^ selection list.  If you create multiple printer models,
#	      edit the above to signify the options invoked with psf.
#
#	This print model works with Xenix 2.2.3, 2.3.2.  It is the printer
#	back-end that is called by lp for physical printing.
#
#	"make installxenix" will use sed to replace "BINARY" with
#		the binary library directory name.

# The following parameters are passed from "lp" at print time.

printer=`basename $0`
request=$1
name=$2
title=$3
copies=$4
options=$5
shift; shift; shift; shift; shift

# Modify the following for required stty settings.

stty onlcr ixon ixoff 0<&1

banner=yes
today=`date`
for i in $options; do
	case $i in
	b)	banner=no ;;
	esac
done

[ "$title " = " " ] && {
	title=`basename $1`
}

[ "$banner" = yes ] && {

#	get the machine name

	if test -r /etc/systemid; then
		sysid=`sed 1q /etc/systemid`
	else
		sysid=`uname -n`
	fi

#	get the user name from the GCOS field in /etc/passwd

	user=`sed -n "s/^$name:.*:.*:.*:\(.*\):.*:.*$/\1/p" /etc/passwd`

#	get the count of the number of banner pages from /etc/default/lpd

	bannercount=`sed -n 's/^BANNERS=//p' /etc/default/lpd`
	[ "$bannercount" -ge 0 -a "$bannercount" -le 5 ] || bannercount=1
	while	[ "$bannercount" -gt 0 ]
	do
		BINARY/psfbanner "$name" "$user" "$request" "$printer" "$options" "$today" "$sysid" "$title" -g PAGETYPE -u DEFFILE
		bannercount=`expr $bannercount - 1`
	done
}

#	psffilter filters the file unless it already is in postscript
#
#	The "$options" are passed by lp with  -o"opions"

files="$*"
while	[ "$copies" -gt 0 ]
do
	for file in $files
	do
		BINARY/psffilter $options "$file" 2>&1
	done
	copies=`expr $copies - 1`
done
exit 0
