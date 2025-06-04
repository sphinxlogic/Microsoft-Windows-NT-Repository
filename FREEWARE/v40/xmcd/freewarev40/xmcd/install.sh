#!/bin/sh
#
# @(#)install.sh	6.78 98/10/26
#
# Script to install the software binary and support files on
# the target system.
#
# Command line options:
#
# -n	Do not remove clean up files after installing
# -b	Run in non-interactive mode, use default answers for all prompts.
#	The default answers can be overrided with the following environment
#	variables:
#	BATCH_BINDIR		Binary executable installation directory
#	BATCH_LIBDIR		X11 library directory
#	BATCH_APPDEFAULTSDIR	X11 app-defaults directory
#	BATCH_XMCDLIB		Xmcd support files top level directory
#	BATCH_MANDIR		Manual page top level directory
#	BATCH_MANSUFFIX		Manual page file suffix
#
#    xmcd  - Motif(tm) CD Audio Player
#    cda   - Command-line CD Audio Player
#
#    Copyright (C) 1993-1998  Ti Kan
#    E-mail: ti@amb.org
#
#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 2 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#

OPATH="$PATH"
PATH=/bin:/usr/bin:/sbin:/usr/sbin:/etc:/usr/local/bin
export PATH

XMCD_VER=2.4
DEMODB=4102560a
WWWURL=http://sunsite.unc.edu/tkan/xmcd/
DIRPERM=755
SCRPERM=755
FILEPERM=444
BINPERM=711
XBINPERM=4711
OWNER=bin
GROUP=bin
BINOWNER=bin
BINGROUP=bin
XBINOWNER=root
XBINGROUP=bin
ERRFILE=/tmp/xmcd.err
TMPFILE=/tmp/xmcdinst.$$

#
# Utility functions
#

doexit()
{
	if [ $1 -eq 0 ]
	then
	    $ECHO "\nInstallation of xmcd is now complete.  Please read"
	    $ECHO "$XMCDLIB/docs/README for further information."
	    if [ -n "$BATCH" ]
	    then
		$ECHO "\nBefore using xmcd/cda for the first time, you must"
		$ECHO "set up the software by running the following program:"
		$ECHO "\n\t${XMCDLIB}/config/config.sh\n"
		$ECHO "Xmcd/cda will not work until that is done."
	    fi
	else
	    $ECHO "\nErrors have occurred in the installation."
	    if [ $ERRFILE != /dev/null ]
	    then
		$ECHO "See $ERRFILE for an error log."
	    fi
	fi
	exit $1
}

logerr()
{
	if [ "$1" = "-p" ]
	then
		$ECHO "Error: $2"
	fi
	$ECHO "$2" >>$ERRFILE
	ERROR=1
}

getstr()
{
	$ECHO "$* \c"
	read ANS
	if [ -n "$ANS" ]
	then
		return 0
	else
		return 1
	fi
}

getyn()
{
	if [ -z "$YNDEF" ]
	then
		YNDEF=y
	fi

	while :
	do
		$ECHO "$*? [${YNDEF}] \c"
		read ANS
		if [ -n "$ANS" ]
		then
			case $ANS in
			[yY])
				RET=0
				break
				;;
			[nN])
				RET=1
				break
				;;
			*)
				$ECHO "Please answer y or n"
				;;
			esac
		else
			if [ $YNDEF = y ]
			then
				RET=0
			else
				RET=1
			fi
			break
		fi
	done

	YNDEF=
	return $RET
}

dolink()
{
	# Try symlink first
	ln -s $1 $2 2>/dev/null
	if [ $? != 0 ]
	then
		# Use hard link
		ln $1 $2 2>/dev/null
	fi
	RETSTAT=$?
	if [ $RETSTAT != 0 ]
	then
		logerr -p "Cannot link $1 -> $2"
	fi
	return $RETSTAT
}

makedir()
{
	$ECHO "\t$1"
	if [ ! -d $1 ]
	then
		mkdir -p $1
	fi
	if [ $3 != _default_ ]
	then
		chown $3 $1 2>/dev/null
	fi
	if [ $4 != _default_ ]
	then
		chgrp $4 $1 2>/dev/null
	fi
	if [ $2 != _default_ ]
	then
		chmod $2 $1 2>/dev/null
	fi
	return 0
}

instfile()
{
	TDIR=`dirname $2`

	if [ -n "$TDIR" -a -d "$TDIR" -a -w "$TDIR" ]
	then
		if [ ! -f $1 ]
		then
			$ECHO "\t$2 NOT installed"
			logerr -n "Cannot install $2: file missing."
			return 1
		fi

		$ECHO "\t$2"
		if [ -f $2 ]
		then
			rm -f $2
		fi

		cp $1 $2
		if [ $? != 0 ]
		then
			logerr -n "Cannot install $2: file copy error."
			return 1
		fi

		if [ -f $2 ]
		then
			if [ $4 != _default_ ]
			then
				chown $4 $2 2>/dev/null
			fi
			if [ $5 != _default_ ]
			then
				chgrp $5 $2 2>/dev/null
			fi
			if [ $3 != _default_ ]
			then
				chmod $3 $2 2>/dev/null
			fi
		fi
		return 0
	else
		$ECHO "\t$2 NOT installed"
		logerr -n "Cannot install $2: target directory not writable."
		return 1
	fi
}

link_prompt()
{
	$ECHO "\nFor security reasons, setuid programs (such as xmcd) search"
	$ECHO "only /usr/lib and/or /usr/ccs/lib for dynamic libraries."
	$ECHO "Some of the dynamic libraries that xmcd needs may not be in"
	$ECHO "the standard locations, thus xmcd may not be able to find"
	$ECHO "them."

	$ECHO "\nSymbolic links can be created now to correct this problem."

	$ECHO "\nDo you want this installation procedure to search your"
	$ECHO "system for needed dynamic libraries, and create symbolic links"
	$ECHO "of these libraries to /usr/lib\c"
	YNDEF=y
	getyn " "
	if [ $? != 0 ]
	then
		$ECHO "\nNo links will be created.\n"
		$ECHO "If you encounter difficulty starting xmcd, see the FAQ"
		$ECHO "file in the xmcd distribution for further information."
		return 1
	fi
	$ECHO ""
	return 0
}

link_dynlibs()
{
	LINKFOUND=False

	#
	# Set LD_LIBRARY_PATH to point to all conceivable places where
	# dynamic libraries can hide
	#
	LD_LIBRARY_PATH=$LIBDIR:/usr/X/lib:/usr/X11/lib:/usr/X386/lib:/usr/X11R5/lib:/usr/X11R6/lib:/usr/openwin/lib:/usr/X/desktop:/usr/Motif/lib:/usr/Motif1.1/lib:/usr/Motif1.2/lib:/usr/Motif2.0/lib:/usr/dt/lib:/usr/lib/X11:/usr/ccs/lib:/usr/lib:/lib
	export LD_LIBRARY_PATH

	# Find the ldd program
	for i in /bin /usr/bin /usr/ccs/bin
	do
		if [ -x $i/ldd ]
		then
			LDD=$i/ldd
		fi
	done

	if [ -z "$LDD" ]
	then
		# Can't locate ldd
		return
	fi

	if [ ! -r xmcd_d/xmcd ]
	then
		# Can't read xmcd binary
		return
	fi

	# Run ldd to determine its dynamic library configuration
	$LDD xmcd_d/xmcd >$TMPFILE 2>/dev/null

	if fgrep '=>' $TMPFILE >/dev/null 2>&1
	then
		# BSD/SunOS/SVR5 style ldd output
		DYNLIBS="`fgrep -v 'not found' $TMPFILE | \
			fgrep -v 'xmcd needs:' | \
			sed -e 's/^.*=> *//' -e 's/(.*)//' | tr '\015' ' '`"
		ERRLIBS="`fgrep 'not found' $TMPFILE | $AWK '{ print $1 }' | \
			tr '\015' ' '`"
	else
		# SVR4 style ldd output
		DYNLIBS="`fgrep 'loaded:' $TMPFILE | sed 's/^.*: //' | \
			tr '\015' ' '`"
		ERRLIBS="`fgrep 'error opening' $TMPFILE | \
			sed 's/^.*opening //' | tr '\015' ' '`"
	fi

	# Remove temp files
	rm -f $TMPFILE

	for i in $ERRLIBS _xoxo_
	do
		if [ "$i" = _xoxo_ ]
		then
			break
		fi

		# A needed library is not found in LD_LIBRARY_PATH
		logerr -p "\nNeeded library $i not found!  See the xmcd FAQ."
	done

	for i in $DYNLIBS _xoxo_
	do
		if [ "$i" = _xoxo_ ]
		then
			# Done processing
			break
		fi

		LIBNAME=`basename $i`
		DIRNAME=`dirname $i`

		if [ "$DIRNAME" = /usr/lib -o "$DIRNAME" = /usr/ccs/lib ]
		then
			# This is the standard library location
			continue
		fi

		if [ -f /usr/lib/$LIBNAME -o -f /usr/ccs/lib/$LIBNAME ]
		then
			# Link already there
			continue
		fi

		if [ $LINKFOUND = False ]
		then
			LINKFOUND=True

			if [ -z "$BATCH" ]
			then
				link_prompt
				if [ $? != 0 ]
				then
					return
				fi
			fi
		fi

		YNDEF=y
		if [ -n "$BATCH" ] || \
			getyn "Link $DIRNAME/$LIBNAME to /usr/lib/$LIBNAME"
		then
			dolink $DIRNAME/$LIBNAME /usr/lib/$LIBNAME
		else
			$ECHO "$DIRNAME/$LIBNAME not linked."
		fi

	done
}


#
# Main execution starts here
#

# Catch some signals
trap "rm -f $TMPFILE; exit 1" 1 2 3 5 15

#
# Get platform information
#
OS_SYS=`(uname -s) 2>/dev/null`
OS_VER=`(uname -r) 2>/dev/null`

# Use Sysv echo if possible
if [ -x /usr/5bin/echo ]
then
	ECHO=/usr/5bin/echo				# SunOS SysV echo
elif [ -z "`(echo -e a) 2>/dev/null | fgrep e`" ]
then
	ECHO="echo -e"					# GNU bash, etc.
else
	ECHO=echo					# generic SysV
fi
if [ "$OS_SYS" = QNX ]
then
	ECHO=echo
fi

# If awk doesn't work well on your system, try changing the
# following to nawk or gawk.
AWK=awk

# Remove old error log file
ERROR=0
rm -f $ERRFILE
if [ -f $ERRFILE ]
then
	$ECHO "Cannot remove old $ERRFILE: error logging not enabled."
	ERRFILE=/dev/null
fi

# Determine whether to remove distribution files after installation
NOREMOVE=0
BATCH=
if [ $# -gt 1 ]
then
	for i in $*
	do
		case "$i" in
		-n)
			NOREMOVE=1
			;;
		-b)
			BATCH=$i
			;;
		*)
			logerr -p "Unknown option: $i"
			doexit 1
			;;
		esac
	done
elif [ -f docs_d/INSTALL -a -f docs_d/PORTING ]
then
	NOREMOVE=1
fi


# Implement platform-specific features and deal with OS quirks
LINKLIBS=False
SHELL=/bin/sh
SCO=False

if [ "$OS_SYS" = A/UX ]
then
	# Apple A/UX
	SHELL=/bin/ksh
elif [ "$OS_SYS" = SunOS ]
then
	case $OS_VER in
	4.*)
		# SunOS 4.x
		LINKLIBS=True
		;;
	5.*)
		# SunOS 5.x
		LINKLIBS=True
		;;
	*)
		;;
	esac
elif [ "$OS_SYS" = ULTRIX ]
then
	# Digital Ultrix
	SHELL=/bin/sh5
elif [ -x /bin/ftx ] && /bin/ftx
then
	case $OS_VER in
	4.*)
		# Stratus FTX SVR4
		LINKLIBS=True
		;;
	*)
		;;
	esac
elif [ -x /bin/i386 -o -x /sbin/i386 ] && i386
then
	case $OS_VER in
	3.*)
		if (uname -X | fgrep "Release = 3.2") >/dev/null 2>&1
		then
			# SCO UNIX/ODT
			SCO=True
		fi
		;;
	4.*)
		# UNIX SVR4.x
		LINKLIBS=True
		;;
	4*MP)
		# UNIX SVR4 MP
		LINKLIBS=True
		;;
	5)
		# UNIX SVR5
		LINKLIBS=True
		;;
	5.*)
		# UNIX SVR5.x
		LINKLIBS=True
		;;
	*)
		;;
	esac
elif [ -x /bin/m88k ] && m88k
then
	case $OS_VER in
	4.*)
		# Motorola SVR4/m88k
		LINKLIBS=True
		;;
	*)
		;;
	esac
fi


$ECHO "\nInstalling \"xmcd\" Motif CD Player version $XMCD_VER by Ti Kan"
$ECHO "-------------------------------------------------------"
$ECHO "\nThis is free software and comes with no warranty."
$ECHO "See the GNU General Public License in the COPYING file"
$ECHO "for details."
$ECHO "\nPlease visit the xmcd web site:"
$ECHO "\n\t$WWWURL"

# Check privilege
(id | fgrep 'uid=0(root)') >/dev/null 2>&1
if [ $? != 0 ]
then
	$ECHO "\n\nYou should be the super user to install xmcd."

	YNDEF=n
	if [ -z "$BATCH" ] && getyn "\n  Proceed with installation anyway"
	then
		$ECHO "\nWARNING: Without super-user privilege, some files may"
		$ECHO "not be properly installed, or they may be installed"
		$ECHO "with incorrect permissions."

		XBINPERM=711
		XBINOWNER=_default_
		BINOWNER=_default_
		OWNER=_default_
		GROUP=_default_
	else
		logerr -p "Not super user: installation aborted by user."
		doexit 1
	fi
fi


# Check existence of binaries

MISSING=
for i in xmcd_d/xmcd cda_d/cda util_d/cddbcmd util_d/dp2xmcd.sh util_d/wm2xmcd
do
	if [ ! -f $i ]
	then
		MISSING="$MISSING $i"
	fi
done

if [ -n "$MISSING" ]
then
	$ECHO "\n\nThe following executable binaries are missing:\n"
	for i in $MISSING
	do
		$ECHO "\t$i"
	done
	$ECHO "\nIf you have the xmcd source code distribution, make sure"
	$ECHO "you compile the source code to generate the binaries first."
	$ECHO "See the INSTALL file for details."
	$ECHO "\nIf you have the xmcd binary distribution, it is probably"
	$ECHO "corrupt."

	YNDEF=n
	if [ -z "$BATCH" ] && getyn "\n  Proceed with installation anyway"
	then
		$ECHO "\nThe missing files will not be installed."
	else
		logerr -p "Missing binaries: installation aborted by user."
		doexit 1
	fi
fi


# Determine BINDIR

BINTRYDIRS="\
	/usr/bin/X11 \
	/usr/X/bin \
	/usr/X11/bin \
	/usr/X11R6/bin \
	/usr/X11R5/bin \
	/usr/X386/bin \
	/usr/openwin/bin \
	/usr/local/bin/X11 \
	/usr/local/bin \
	/usr/lbin \
"

if [ -z "$BINDIR" ]
then
	for i in $BINTRYDIRS
	do
		if [ -d $i ]
		then
			BINDIR=$i
			break
		fi
	done

	if [ -z "$BINDIR" ]
	then
		BINDIR=/usr/bin/X11
	fi
else
	BINDIR=`echo $BINDIR | sed 's/\/\//\//g'`
fi


$ECHO "\n\nThe X binary directory is where the executable binary files"
$ECHO "will be installed."

if [ -n "$BATCH" ]
then
	if [ -n "$BATCH_BINDIR" ]
	then
		BINDIR=$BATCH_BINDIR
	fi
	$ECHO "\nUsing $BINDIR"
else
	while :
	do
		if getstr "\n  Enter X binary directory\n  [${BINDIR}]:"
		then
			if [ -d "$ANS" ]
			then
				BINDIR=$ANS
				break
			else
				$ECHO "  Error: $ANS does not exist."
			fi
		else
			break
		fi
	done
fi


# Determine LIBDIR

LIBTRYDIRS="\
	/usr/lib/X11 \
	/usr/X/lib \
	/usr/X11/lib \
	/usr/X11R6/lib/X11 \
	/usr/X11R5/lib/X11 \
	/usr/X386/lib \
	/usr/openwin/lib \
	/usr/local/lib/X11 \
"

if [ -z "$LIBDIR" ]
then
	for i in $LIBTRYDIRS
	do
		if [ -d $i ]
		then
			LIBDIR=$i
			break
		fi
	done

	if [ -z "$LIBDIR" ]
	then
		LIBDIR=/usr/lib/X11
	fi
else
	LIBDIR=`echo $LIBDIR | sed 's/\/\//\//g'`
fi

$ECHO "\n\nThe X library directory is where the X window system"
$ECHO "configuration files are located."

if [ -n "$BATCH" ]
then
	if [ -n "$BATCH_LIBDIR" ]
	then
		LIBDIR=$BATCH_LIBDIR
	fi
	$ECHO "\nUsing $LIBDIR"
else
	while :
	do
		if getstr "\n  Enter X library directory\n  [${LIBDIR}]:"
		then
			if [ -d "$ANS" ]
			then
				LIBDIR=$ANS
				break
			else
				$ECHO "  Error: $ANS does not exist."
			fi
		else
			break
		fi
	done
fi


# Determine APPDEFAULTSDIR

$ECHO "\n\nThe X app-defaults directory is where the X resource file"
$ECHO "for xmcd will be installed."
$ECHO "\n  NOTE: If you specify a non-standard location, you will need to"
$ECHO "  add it to your XFILESEARCHPATH environment variable in order for"
$ECHO "  xmcd to work.  See X(1)."

APPDEFAULTSDIR=$LIBDIR/app-defaults

if [ -n "$BATCH" ]
then
	if [ -n "$BATCH_APPDEFAULTSDIR" ]
	then
		APPDEFAULTSDIR=$BATCH_APPDEFAULTSDIR
	fi
	$ECHO "\nUsing $APPDEFAULTSDIR"
else
	while :
	do
		if getstr \
		    "\n  Enter X app-defaults directory\n  [${APPDEFAULTSDIR}]:"
		then
			if [ -d "$ANS" ]
			then
				APPDEFAULTSDIR=$ANS
				break
			else
				$ECHO "  Error: $ANS does not exist."
			fi
		else
			break
		fi
	done
fi


# Determine xmcd libdir

$ECHO "\n\nThe xmcd library directory is where xmcd/cda support files"
$ECHO "will be installed."

XMCDLIB=$LIBDIR/xmcd

if [ -n "$BATCH" ]
then
	if [ -n "$BATCH_XMCDLIB" ]
	then
		XMCDLIB=$BATCH_XMCDLIB
	fi
	$ECHO "\nUsing $XMCDLIB"
else
	while :
	do
		if getstr "\n  Enter xmcd library directory\n  [${XMCDLIB}]:"
		then
			UDIR="$ANS"
			if [ -d "$UDIR" ]
			then
				XMCDLIB=$UDIR
				break
			else
				if getyn \
				"  Directory $UDIR does not exist.  Create it"
				then
					XMCDLIB="$UDIR"
					break
				else
					$ECHO \
					"  ERROR: Cannot install in ${UDIR}."
				fi
			fi
		else
			break
		fi
	done
fi


# Determine MANDIR

if [ -z "$MANDIR" ]
then
	for i in	/usr/man/man.LOCAL \
			/usr/share/man/man1 \
			/usr/X11/man/man1 \
			/usr/X/man/man1 \
			/usr/X11R6/man/man1 \
			/usr/X11R5/man/man1 \
			/usr/X386/man/man1 \
			/usr/local/man/man1
	do
		if [ -d $i ]
		then
			MANDIR=$i
			break
		fi
	done

	if [ -z "$MANDIR" ]
	then
		MANDIR=/usr/man/man1
	fi

else
	MANDIR=`echo $MANDIR | sed 's/\/\//\//g'`
fi

$ECHO "\n\nThe on-line manual directory is where the man pages in"
$ECHO "in the xmcd package will be installed."

if [ -n "$BATCH" ]
then
	if [ -n "$BATCH_MANDIR" ]
	then
		MANDIR=$BATCH_MANDIR
	fi
	$ECHO "\nUsing $MANDIR"
else
	if getstr "\n  Enter on-line manual directory\n  [${MANDIR}]:"
	then
		MANDIR=$ANS
	fi
fi

if [ ! -d $MANDIR -a -z "$BATCH" ]
then
	YNDEF=y
	getyn "  Directory $MANDIR does not exist.  Create it"
	if [ $? -ne 0 ]
	then
		$ECHO "  The xmcd on-line manual will not be installed."
		MANDIR=
	fi
fi

# Determine MANSUFFIX

if [ -n "$MANDIR" ]
then
	if [ -z "$MANSUFFIX" ]
	then
		MANSUFFIX=1
	fi

	if [ -n "$BATCH" ]
	then
		if [ -n "$BATCH_MANSUFFIX" ]
		then
			MANSUFFIX=$BATCH_MANSUFFIX
		fi
		$ECHO "Using ManSuffix $MANSUFFIX"
	else
		if getstr \
		"\n  Enter on-line manual file name suffix\n  [${MANSUFFIX}]:"
		then
			MANSUFFIX=$ANS
		fi
	fi
fi

# Remove old xmcd binaries
dirs=`echo "$OPATH" | $AWK -F: '{ for (i = 1; i <= NF; i++) print $i }'`
for i in $BINTRYDIRS
do
	dirs=`$ECHO "$dirs\n$i"`
done
dirs=`($ECHO "$dirs" | \
       sed -e 's/^[ 	]*//' -e '/^$/d' | \
       sort | uniq) 2>/dev/null`

first=1
if [ -n "$dirs" ]
then
	for i in $dirs
	do
	    if [ "$i" = "$BINDIR" -o "$i" = "." ]
	    then
		    continue
	    fi

	    for j in xmcd cda cddbcmd wm2xmcd dp2xmcd
	    do
		if [ -f ${i}/${j} -a -x ${i}/${j} ]
		then
		    if [ -z "$BATCH" ]
		    then
			if [ $first = 1 ]
			then
			    first=0
			    $ECHO "\n"
			fi
			$ECHO "An old ${j} executable is found in ${i}.  \c"
			YNDEF=y
			if getyn "Remove"
			then
			    rm -f ${i}/${j}
			    if [ $? -ne 0 ]
			    then
				$ECHO "Cannot remove ${i}/${j}."
			    fi
			fi
		    else
			rm -f ${i}/${j}
		    fi
		fi
	    done
	done
fi

# Make all necessary directories

$ECHO "\n\nMaking directories..."

makedir $XMCDLIB $DIRPERM $OWNER $GROUP
makedir $XMCDLIB/cddb $DIRPERM $OWNER $GROUP
makedir $XMCDLIB/config $DIRPERM $OWNER $GROUP
makedir $XMCDLIB/config/.tbl $DIRPERM $OWNER $GROUP
makedir $XMCDLIB/docs $DIRPERM $OWNER $GROUP
makedir $XMCDLIB/help $DIRPERM $OWNER $GROUP
makedir $XMCDLIB/pixmaps $DIRPERM $OWNER $GROUP
makedir $XMCDLIB/scripts $DIRPERM $OWNER $GROUP
if [ -n "$MANDIR" ]
then
	makedir $MANDIR $DIRPERM $OWNER $GROUP
fi


# Install files
$ECHO "\nInstalling xmcd files..."

# Binaries
instfile xmcd_d/xmcd $BINDIR/xmcd $XBINPERM $XBINOWNER $GROUP
instfile cda_d/cda $BINDIR/cda $XBINPERM $XBINOWNER $GROUP
instfile util_d/cddbcmd $BINDIR/cddbcmd $BINPERM $BINOWNER $GROUP
instfile util_d/dp2xmcd.sh $BINDIR/dp2xmcd $SCRPERM $OWNER $GROUP
instfile util_d/wm2xmcd $BINDIR/wm2xmcd $BINPERM $BINOWNER $GROUP

# X resource defaults file
if [ $LIBDIR = /usr/openwin/lib -a ! -d /usr/lib/X11 ]
then
	ln -s /usr/openwin/lib /usr/lib/X11 2>/dev/null
fi

if instfile xmcd_d/XMcd.ad $APPDEFAULTSDIR/XMcd $FILEPERM $OWNER $GROUP &&
   [ $LIBDIR != "/usr/lib/X11" -a -d /usr/lib/X11/app-defaults ]
then
	# Test APPDEFAULTSDIR
	rm -f /usr/lib/X11/app-defaults/._junk_

	>$APPDEFAULTSDIR/._junk_

	if [ ! -f /usr/lib/X11/app-defaults/._junk_ ]
	then
		rm -f /usr/lib/X11/app-defaults/XMcd
		dolink $APPDEFAULTSDIR/XMcd /usr/lib/X11/app-defaults/XMcd
	fi

	rm -f $APPDEFAULTSDIR/._junk_
fi

# Special hack for some Solaris 2.x versions which have two
# app-defaults directories
if [ $APPDEFAULTSDIR = "/usr/openwin/lib/app-defaults" -a \
     -d /usr/openwin/lib/X11/app-defaults ]
then
	# Test APPDEFAULTSDIR
	rm -f $APPDEFAULTSDIR/._junk_

	>/usr/openwin/lib/X11/app-defaults/._junk_

	if [ ! -f $APPDEFAULTSDIR/._junk_ ]
	then
		rm -f /usr/openwin/lib/X11/app-defaults/XMcd
		dolink $APPDEFAULTSDIR/XMcd \
			/usr/openwin/lib/X11/app-defaults/XMcd
	fi

	rm -f /usr/openwin/lib/X11/app-defaults/._junk_
fi

# Documentation
rm -f $XMCDLIB/docs/*
(cd docs_d ;\
for i in * ;\
do \
	case $i in
	SCCS)
		;;
	RCS)
		;;
	CVS)
		;;
	*)
		instfile $i $XMCDLIB/docs/$i $FILEPERM $OWNER $GROUP ;\
		;;
	esac
done)

# Help files
rm -f $XMCDLIB/help/*
for i in xmcd_d/hlpfiles/*.btn xmcd_d/hlpfiles/*.lbl xmcd_d/hlpfiles/*.txw \
	 xmcd_d/hlpfiles/*.scl xmcd_d/hlpfiles/*.lsw xmcd_d/hlpfiles/*.rbx \
	 xmcd_d/hlpfiles/*.cbx xmcd_d/hlpfiles/*.opt
do
	j=`echo $i | sed 's/xmcd\_d\/hlpfiles\///'`
	instfile $i $XMCDLIB/help/$j $FILEPERM $OWNER $GROUP
done

# Icon/pixmap files
for i in xmcd.icon xmcd_a.px xmcd_b.px xmcd.xpm
do
	instfile misc_d/$i $XMCDLIB/pixmaps/$i $FILEPERM $OWNER $GROUP
done

# Configuration files
instfile libdi_d/common.cfg $XMCDLIB/config/common.cfg \
	$FILEPERM $OWNER $GROUP
instfile libdi_d/device.cfg $XMCDLIB/config/device.cfg \
	$FILEPERM $OWNER $GROUP
instfile misc_d/sites $XMCDLIB/config/sites \
	$FILEPERM $OWNER $GROUP

rm -f $XMCDLIB/config/.tbl/*
ENTRIES=`(cd libdi_d/cfgtbl; echo * | \
	sed -e 's/Imakefile//' -e 's/Makefile//' -e 's/SCCS//' -e 's/RCS//')`
for i in $ENTRIES
do
	if (fgrep "tblver=" libdi_d/cfgtbl/$i) >/dev/null 2>&1
	then
		instfile libdi_d/cfgtbl/$i $XMCDLIB/config/.tbl/$i \
			$FILEPERM $OWNER $GROUP
	fi
done

# Configuration script
SHELL_S=`echo $SHELL | sed 's/\//\\\\\//g'`
BINDIR_S=`echo $BINDIR | sed 's/\//\\\\\//g'`
LIBDIR_S=`echo $LIBDIR | sed 's/\//\\\\\//g'`
XMCDLIB_S=`echo $XMCDLIB | sed 's/\//\\\\\//g'`
APPDEFAULTSDIR_S=`echo $APPDEFAULTSDIR | sed 's/\//\\\\\//g'`
sed -e "s/^#!\/bin\/sh.*/#!$SHELL_S/" \
    -e "s/^BINDIR=.*/BINDIR=$BINDIR_S/" \
    -e "s/^LIBDIR=.*/LIBDIR=$LIBDIR_S/" \
    -e "s/^XMCDLIB=.*/XMCDLIB=$XMCDLIB_S/" \
    -e "s/^APPDEFAULTSDIR=.*/APPDEFAULTSDIR=$APPDEFAULTSDIR_S/" \
    <libdi_d/config.sh >/tmp/xmcdcfg.$$

rm -f $XMCDLIB/config/configure.sh
instfile /tmp/xmcdcfg.$$ $XMCDLIB/config/config.sh $SCRPERM $OWNER $GROUP
rm -f /tmp/xmcdcfg.$$

# Shell scripts
instfile misc_d/ncsarmt.sh $XMCDLIB/scripts/ncsarmt $SCRPERM $OWNER $GROUP
instfile misc_d/ncsawrap.sh $XMCDLIB/scripts/ncsawrap $SCRPERM $OWNER $GROUP
instfile misc_d/nswrap.sh $XMCDLIB/scripts/nswrap $SCRPERM $OWNER $GROUP

# Convenience link to config.sh
if [ "$SCO" = True ]
then
	if [ -w /usr/lib/mkdev ]
	then
		$ECHO "\t/usr/lib/mkdev/xmcd"
		rm -f /usr/lib/mkdev/xmcd
		dolink $XMCDLIB/config/config.sh /usr/lib/mkdev/xmcd
	fi
fi

# Motif XKeysymDB file
if [ ! -f $LIBDIR/XKeysymDB ]
then
	instfile xmcd_d/XKeysymDB $LIBDIR/XKeysymDB $FILEPERM $OWNER $GROUP
fi

# Manual page files
if [ -n "$MANDIR" -a -n "$MANSUFFIX" ]
then
	instfile xmcd_d/xmcd.man $MANDIR/xmcd.$MANSUFFIX \
		$FILEPERM $OWNER $GROUP
	instfile cda_d/cda.man $MANDIR/cda.$MANSUFFIX \
		$FILEPERM $OWNER $GROUP
	instfile util_d/cddbcmd.man $MANDIR/cddbcmd.$MANSUFFIX \
		$FILEPERM $OWNER $GROUP
	instfile util_d/dp2xmcd.man $MANDIR/dp2xmcd.$MANSUFFIX \
		$FILEPERM $OWNER $GROUP
	instfile util_d/wm2xmcd.man $MANDIR/wm2xmcd.$MANSUFFIX \
		$FILEPERM $OWNER $GROUP
fi

if [ -z "$BATCH" ]
then
	# Run device-dependent config script
	if [ -r $XMCDLIB/config/config.sh ]
	then
		$SHELL $XMCDLIB/config/config.sh
		if [ $? != 0 ]
		then
			logerr -n "$XMCDLIB/config/config.sh failed."
		fi
	else
		logerr -p "Cannot execute $XMCDLIB/config/config.sh"
	fi
fi

# Demo cddb file
if [ -d $XMCDLIB/cddb/misc ]
then
	$ECHO "\nInstalling demo mode CDDB file..."
	instfile misc_d/demo.db $XMCDLIB/cddb/misc/$DEMODB \
		$FILEPERM $OWNER $GROUP
fi

# Set up dynamic library links
if [ $LINKLIBS = True ]
then
	link_dynlibs
fi

# Clean up
if [ $NOREMOVE = 0 ]
then
	rm -rf common_d libdi_d misc_d xmcd_d cda_d util_d docs_d install.sh \
		xmcd.tar
fi

doexit $ERROR

