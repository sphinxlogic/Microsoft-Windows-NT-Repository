XCOMM!/bin/sh

XCOMM $XFree86: mit/util/scripts/xon.cpp,v 2.0 1993/09/10 05:50:10 dawes Exp $
XCOMM start up xterm (or any other X command) on the specified host
XCOMM Usage: xon host [arguments] [command]
case $# in
0)
	echo "Usage: $0 <hostname> [-user user] [-name window-name] [-debug]"
	echo "[-screen screen-number] [command ...]"
	exit 1
	;;
esac
target=$1
shift
label=$target
resource=xterm-$label
rsh=RSHCMD
rcmd="$rsh $target"
case $DISPLAY in
unix:*)
	DISPLAY=`echo $DISPLAY | sed 's/unix//'`
	;;
esac
case $DISPLAY in
:*)
	fullname=`hostname`
	hostname=`echo $fullname | sed 's/\..*$//'`
	if [ $hostname = $target -o $fullname = $target ]; then
		DISPLAY=$DISPLAY
		rcmd="sh -c"
	else
		DISPLAY=$fullname$DISPLAY
	fi
	;;
esac
username=
xauth=
case x$XUSERFILESEARCHPATH in
x)
	xpath='HOME=${HOME-`pwd`} '
	;;
*)
	xpath='HOME=${HOME-`pwd`} XUSERFILESEARCHPATH=${XUSERFILESEARCHPATH-"'"$XUSERFILESEARCHPATH"'"} '
	;;
esac
redirect=" < /dev/null > /dev/null 2>&1 &"
command=
ls=-ls
continue=:
while $continue; do
	case $1 in
	-user)
		shift
		username="-l $1"
		label="$target $1"
		rcmd="$rsh $target $username"
		shift
		case x$XAUTHORITY in
		x)
			XAUTHORITY="$HOME/.Xauthority"
			;;
		esac
		case x$XUSERFILESEARCHPATH in
		x)
			;;
		*)
			xpath="XUSERFILESEARCHPATH=$XUSERFILESEARCHPATH "
			;;
		esac
		;;
	-access)
		shift
		xhost +$target
		;;
	-name)
		shift
		label="$1"
		resource="$1"
		shift
		;;
	-nols)
		shift
		ls=
		;;
	-debug)
		shift
		redirect=
		;;
	-screen)
		shift
		DISPLAY=`echo $DISPLAY | sed 's/:\\([0-9][0-9]*\\)\\.[0-9]/:\1/'`.$1
		shift
		;;
	*)
		continue=false
		;;
	esac
done
case x$XAUTHORITY in
x)
	;;
x*)
	xauth="XAUTHORITY=$XAUTHORITY "
	;;
esac
vars="$xpath$xauth"DISPLAY="$DISPLAY"
case $# in
0)
	$rcmd 'sh -c '"'$vars"' xterm '$ls' -name "'"$resource"'" -T "'"$label"'" -n "'"$label"'" '"$redirect'"
	;;
*)
	$rcmd 'sh -c '"'$vars"' '"$*$redirect'"
	;;
esac
