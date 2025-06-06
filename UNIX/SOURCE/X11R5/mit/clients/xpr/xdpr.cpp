XCOMM! /bin/sh
XCOMM Copyright 1985,1988 Massacusetts Institute of Technology.
XCOMM $XFree86: mit/clients/xpr/xdpr.cpp,v 2.0 1993/10/06 14:52:41 dawes Exp $
XCOMM $XConsortium: xdpr.script,v 1.9 91/06/30 19:20:16 rws Exp $
XCOMM origin: William Kucharski, Solbourne Computer, Inc. 3/24/90
XCOMM         translated from csh script xdpr.script "paul 4/12/88"

XCOMM initialize variables

display="$DISPLAY"
header=
bsdlprv=
lprv=
out=
svlprv=
trailer=
xprv=
xwdv=

usage="Usage: xdpr [filename] [-out filename ] \
[-display host:display] [[-Pprinter] | [-dprinter]] [-device devtype] \
[{-root | -id <id> | -name <name>}] [-nobdrs] [-xy] \
[-scale scale] [-height inches] [-width inches] [-left inches] \
[-top inches] [-split n] [-header string] [-trailer string] \
[-landscape] [-portrait] [-rv] [-compact] [-noff] [-frame] \
[-plane number] [-gray number] [-psfig] [-density dpi] \
[-cutoff level] [-noposition] [-gamma correction] [-render algorithm] \
[-slide] [-add value] [-help]"

XCOMM Guess if we are BSD or System V

if [ -x /usr/ucb/lpr -o -x /usr/bin/lpr -o -x /bin/lpr -o -x /usr/bsd/lpr ]
then
	LP=lpr
	BSD=1
elif [ -x /usr/bin/lp -o -x /bin/lp ]
then
	LP=lp
	BSD=0
else
	LP=lpr
	BSD=1
fi

if [ x"$XWINHOME" != x ]; then
	if [ x"$XWINHOME" = x/usr ]; then
		BIN_DIR=/usr/bin/X11
	else
		BIN_DIR=$XWINHOME/bin
	fi
else
	BIN_DIR=BINDIR
fi

XCOMM parse arguments...

while [ $1 ]; do
	case "$1" in

XCOMM ...arguments interpreted by xdpr itself...

	-help)
		echo $usage;
		exit 0;;

XCOMM ...arguments to xwd...

	-nobdrs|-root|-xy|-frame)
		xwdv="$xwdv $1";;
	-display)
		display=$2
		xwdv="$xwdv $1 $2";
		shift;;
	-id|-name)
		xwdv="$xwdv $1 $2";
		shift;;
	-out|-add)
		out=true
		xwdv="$xwdv $1 $2";
		shift;;

XCOMM ...arguments to xpr...

	-scale|-height|-width|-left|-top|-split|-device)
		xprv="$xprv $1 $2";
		shift;;
	-plane|-gray|-density|-cutoff|-gamma|-render)
		xprv="$xprv $1 $2";
		shift;;
	-header)
		shift;
		header="$1";;
	-trailer)
		shift;
		trailer="$1";;
	-landscape|-portrait|-rv|-compact|-noff|-psfig|-noposition|-slide)
		xprv="$xprv $1";;

XCOMM ...arguments to lp[r]...

	-P*|-#?*|-C?*|-J?*|-h|-m)
		bsdlprv="$lprv $1";;

	-d*|-H*|-q*|-n*|-o*|-w)
		svlprv="$svlprv $1";;

XCOMM ...disallow other arguments; print usage message

	-*)
		echo "xdpr: Unknown option $1";
		echo $usage;
		exit 1;;

XCOMM ...input filename...

	*)
		if [ ! "$infile" ]; then
			infile=true
			xprv="$xprv $1"
		else
			echo "xdpr: Invalid argument "$1""
			echo $usage			
			exit 1
		fi
	esac
	shift
done

XCOMM quit if there is no DISPLAY specified

if [ ! "$display" ]; then
	echo "xdpr: DISPLAY variable must be set or a display specified."
	exit
fi

XCOMM Command lines:

XCOMM Set up lp[r] options...

if [ $BSD -eq 0 ]
then
	lprv=$svlprv
else
	lprv=$bsdlprv
fi

XCOMM disallow concurrent input and  -out arguments
if [ "$out" -a "$infile" ]; then
	echo "xdpr: -out <filename> cannot be used if an input file is also specified."
	exit 0
fi

XCOMM dump only
if [ "$out" ]; then
	if [ "$xprv" -o "$lprv" ]; then
		echo "xdpr: The following arguments will be ignored:"
		echo $xprv $lprv
	fi
	$BIN_DIR/xwd $xwdv
	exit 0
fi

XCOMM print only 
if [ "$infile" ]; then
	if [ "$xwdv" ]; then
		echo "xdpr: The following arguments will be ignored:"
		echo $xwdv
	fi
	$BIN_DIR/xpr -header "$header" -trailer "$trailer" $xprv | $LP $lprv
	exit 0
fi

XCOMM dump & print (default)
$BIN_DIR/xwd $xwdv | $BIN_DIR/xpr -header "$header" -trailer "$trailer" $xprv | $LP $lprv
exit 0	

XCOMM EOF
