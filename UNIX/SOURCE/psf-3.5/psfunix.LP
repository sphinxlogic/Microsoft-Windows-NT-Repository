:
# $Id: psfunix.LP,v 3.5 1992/06/02 08:16:01 ajf Exp ajf $
#  Postscript serial or parallel printer using psf filters
#
#	"make scripts" will use sed to replace "BINARY" with
#		the binary library directory name such as /local/bin.

# If page accounting is desired, modify the code at the end
# of this script.

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

# get the banner option from the configuration file.
# see if the user is allowed to override the banner spec.
# The "Banner: on" and "Banner: on:Always" can be specified by lpadmin.
# The "Banner: off" must be manually edited into the configuration file.

banner="yes"
banopt=`sed -n 's/^Banner: *\(.*\).*$/\1/p' /usr/spool/lp/admins/lp/printers/$printer/configuration`
mustbanner="no"
for i in ${banopt}
do
	case "${i}" in
	
	on)		banner="yes"
			mustbanner="no"
			;;
	on:Always)	banner="yes"
			mustbanner="yes"
			;;
	off)		banner="no"
			mustbanner="no"
			;;
	*)		banner="yes"
			mustbanner="no"
			;;
	esac
done

option_list=$options
options=
for i in ${option_list}
do
	case "${i}" in

	*@*)	rname="${i}"
		;;
	nobanner)
		case "${mustbanner}" in
		yes)	;;
		*)	banner="no"
			;;
		esac
		;;
	banner)
		banner="yes"
		;;
	*)
		options="${options} ${i}"
		;;
	esac;
done

today=`date`

[ "$title " = " " ] && {
	title=`basename $1`
}

[ "$banner" = "yes" ] && {

#	get the machine name uname.
	sysid=`uname -n`

#	get the user name from the GCOS field in /etc/passwd
	user=`sed -n "s/^$name:.*:.*:.*:\(.*\):.*:.*$/\1/p" /etc/passwd`

#	user is on remote system
	case $rname in
	[0-z]*) name=$rname
		;;
	esac

	BINARY/psfbanner "$name" "$user" "$request" "$printer" "$options" "$today" "$sysid" "$title" -g PAGETYPE -u DEFFILE
}

#	If the file looks like it is in postscript, then psffilter
#	copies the file to the printer.  (unless -z is specified)
#	The "$options" are passed by lp with  -o"opions"

#	To ensure that file names do not push off the date stamp,
#	you might wish to use:
#
#		fname=`basename $file`
#		BINARY/psffilter $options "$fname" <$file 2>&1

files="$*"
while	[ "$copies" -gt 0 ]
do
	for file in $files
	do
		BINARY/psffilter -g PAGETYPE $options "$file" <$file 2>&1
#
# 		OPTIONAL: enable accounting and read/write access to serial printer
#
#		BINARY/psffilter -g PAGETYPE -y "$user" -Y "$sysid" -a -A /usr/spool/lp/logs/psfacct $options "$file" <$file 2>/usr/spool/lp/logs/psferror
	done
	copies=`expr $copies - 1`
done
exit 0
