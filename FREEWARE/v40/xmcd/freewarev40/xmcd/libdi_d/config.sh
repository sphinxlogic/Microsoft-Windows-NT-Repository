#!/bin/sh
#
# @(#)config.sh	6.121 98/10/26
#
# Script to set up the device-dependent configuration files.
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

PATH=/bin:/usr/bin:/sbin:/usr/sbin:/etc:/usr/local/bin:/usr/ucb
export PATH

VER=2.4

# Change the following directory to fit your local configuration
BINDIR=/usr/bin/X11
LIBDIR=/usr/lib/X11
XMCDLIB=/usr/lib/X11/xmcd
APPDEFAULTSDIR=/usr/lib/X11/app-defaults

ERRFILE=/tmp/xmcd.err
TMPFILE=/tmp/.xmcdcfg.$$
TMPSITES=/tmp/.sites.$$

CFGDIR=$XMCDLIB/config
SCRDIR=$XMCDLIB/scripts
SITES=$CFGDIR/sites
OWNER=bin
GROUP=bin
CDIRPERM=777
DFLT_CGIPATH='/~cddb/cddb.cgi'
CDDBCATS="rock jazz blues newage classical reggae folk country soundtrack misc data"
BROWSERS="netscape Mosaic"

# Utility functions

doexit()
{
	if [ $1 -eq 0 ]
	then
		$ECHO "\nXmcd set-up is now complete.\n"
		$ECHO "Please read the README file supplied with the xmcd"
		$ECHO "distribution for hardware configuration information"
		$ECHO "about specific drives.\n"
	else
		$ECHO "\nErrors have occurred configuring xmcd."
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
		$ECHO "Error: $2" >&2
	fi
	$ECHO "$2" >>$ERRFILE
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
	if [ $? -ne 0 ]
	then
		# Use hard link
		ln $1 $2 2>/dev/null
	fi
	RETSTAT=$?
	if [ $RETSTAT -ne 0 ]
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


ask_scsi_config()
{
	$ECHO "\n  Since you have an unlisted drive, I will assume"
	$ECHO "  that it is SCSI-2 compliant.  If this is not true then"
	$ECHO "  xmcd will probably not work."

	YNDEF=n
	if getyn "\n  Do you want to continue"
	then
		METHOD=0
		VENDOR=0
		VOLBASE=0
		VOLTAPER=0
		PLAYNOTUR=0
	else
		return 1
	fi

	$ECHO "\n  You will now be asked several technical questions about"
	$ECHO "  your drive.  If you don't know the answer, try accepting"
	$ECHO "  the default values, and if problems occur when using"
	$ECHO "  xmcd, reconfigure the settings by running this script"
	$ECHO "  again, or editing the $CFGDIR/$CONFIGFILE"
	$ECHO "  file."
	$ECHO "\n  If you get an unlisted drive working with xmcd in this"
	$ECHO "  manner, the author of xmcd would like to hear from you"
	$ECHO "  and incorporate the settings into the next xmcd release."
	$ECHO "  Please send e-mail to \"xmcd@amb.org\"."

	while :
	do
	    YNDEF=n
	    if getyn "\n  Is your drive on $XMCD_DEV a multi-disc changer"
	    then
		$ECHO "\n  Select one of the following changer methods:\n"
		$ECHO "  1.\tSCSI LUN addressing method"
		$ECHO "  2.\tSCSI medium changer method"
		$ECHO "  3.\tother"
		$ECHO "  q.\tquit (abort configuration)"

		while :
		do
		    if getstr "\n  Enter choice: [1]"
		    then
			case "$ANS" in
			[1-3])
			    break
			    ;;
			q)
			    return 1
			    ;;
			*)
			    $ECHO "  Please answer 1 to 3."
			    ;;
			esac
		    else
			ANS=1
			break
		    fi
		done	

		if [ "$ANS" -eq 3 ]
		then
		    #
		    # Unsupported changer method: just treat it
		    # as a single-disc drive
		    #
		    $ECHO "\n  Your drive will be treated as a single-disc unit."
		    CHGMETHOD=0
		    NUMDISCS=1
		    MULTIPLAY=0
		else
		    CHGMETHOD="$ANS"
		    while :
		    do
			if getstr \
			    "\n  How many discs does the drive support?:"
			then
			    case "$ANS" in
			    [1-9]*)
				NUMDISCS="$ANS"
				break
				;;
			    *)
				$ECHO "  Invalid input.  Try again."
				;;
			    esac
			else
			    $ECHO "  Invalid input.  Try again."
			fi
		    done

		    if [ "$NUMDISCS" -gt 1 ]
		    then
			MULTIPLAY=True
		    else
			CHGMETHOD=0
			MULTIPLAY=False
		    fi
		fi
	    else
		CHGMETHOD=0
		NUMDISCS=1
		MULTIPLAY=False
	    fi

	    $ECHO "\n  Does your drive on $XMCD_DEV support the following:\n"

	    YNDEF=y
	    if getyn "  - The Play_Audio_MSF SCSI command"
	    then
		    PLAYMSF=True
	    else
		    PLAYMSF=False
	    fi

	    YNDEF=n
	    if getyn "  - The Play_Audio(12) SCSI command"
	    then
		    PLAY12=True
	    else
		    PLAY12=False
	    fi

	    YNDEF=y
	    if getyn "  - The Play_Audio(10) SCSI command"
	    then
		    PLAY10=True
	    else
		    PLAY10=False
	    fi

	    YNDEF=y
	    if getyn "  - The Play_Audio_Track/Index command"
	    then
		    PLAYTI=True
	    else
		    PLAYTI=False
	    fi

	    YNDEF=n
	    if getyn "  - Caddy load via the Start_Stop_Unit SCSI command"
	    then
		    LOAD=True
	    else
		    LOAD=False
	    fi

	    YNDEF=y
	    if getyn "  - Caddy eject via the Start_Stop_Unit SCSI command"
	    then
		    EJECT=True
	    else
		    EJECT=False
	    fi

	    YNDEF=y
	    if getyn \
		"  - Disable block descriptor in the Mode_Sense SCSI command"
	    then
		    MODEDBD=True
	    else
		    MODEDBD=False
	    fi

	    YNDEF=y
	    if getyn \
		"  - Audio volume control via the Mode_Select SCSI command"
	    then
		    YNDEF=y
		    if getyn \
		    "  - Independent SCSI Mode_Select volume control for each channel"
		    then
			    VOLSUPP=True
			    BALSUPP=True
		    else
			    VOLSUPP=True
			    BALSUPP=False
		    fi

		    YNDEF=y
		    if getyn "  - Audio channel routing via SCSI Mode_Select"
		    then
			    CHRSUPP=True
		    else
			    CHRSUPP=False
		    fi
	    else
		    VOLCTL=0
		    VOLSUPP=False
		    BALSUPP=False
		    CHRSUPP=False
	    fi

	    YNDEF=y
	    if getyn "  - The Pause/Resume SCSI command"
	    then
		    PAUSE=True
	    else
		    PAUSE=False
	    fi

	    YNDEF=y
	    if getyn "  - The Prevent/Allow_Medium_Removal SCSI command"
	    then
		    CADDYLOCK=True
	    else
		    CADDYLOCK=False
	    fi

	    YNDEF=n
	    if getyn "  - Data Format 1 of the Read_Subchannel SCSI command"
	    then
		    CURPOSFMT=True
	    else
		    CURPOSFMT=False
	    fi

	    $ECHO "\n  This is the configuration for ${XMCD_DEV}:\n"
	    $ECHO "  logicalDriverNumber:   $DRVNO"
	    $ECHO "  mediumChangeMethod:    $CHGMETHOD"
	    $ECHO "  numDiscs:              $NUMDISCS"
	    $ECHO "  playAudio12Support:    $PLAY12"
	    $ECHO "  playAudioMSFSupport:   $PLAYMSF"
	    $ECHO "  playAudio10Support:    $PLAY10"
	    $ECHO "  playAudioTISupport:    $PLAYTI"
	    $ECHO "  loadSupport:           $LOAD"
	    $ECHO "  ejectSupport:          $EJECT"
	    $ECHO "  modeSenseSetDBD:       $MODEDBD"
	    $ECHO "  volumeControlSupport:  $VOLSUPP"
	    $ECHO "  balanceControlSupport: $BALSUPP"
	    $ECHO "  pauseResumeSupport:    $PAUSE"
	    $ECHO "  caddyLockSupport:      $CADDYLOCK"
	    $ECHO "  curposFormat:          $CURPOSFMT"
	    $ECHO "  multiPlay:             $MULTIPLAY"

	    YNDEF=y
	    if getyn "\n  Is this acceptable"
	    then
		    break
	    fi

	    $ECHO "  Try again..."
	done

	return 0
}


ask_nonscsi_config()
{
	$ECHO "\n  Non-SCSI drives are currently supported only on the BSDI,"
	$ECHO "  Digital UNIX, FreeBSD, IBM AIX, Linux, NetBSD, OpenBSD, QNX,"
	$ECHO "  SCO Open Server and Sun Solaris platforms.  You must have"
	$ECHO "  the appropriate CD-ROM driver configured in your kernel."
	$ECHO "  QNX users must use the QNX ioctl method, even for SCSI"
	$ECHO "  drives."

	YNDEF=y
	getyn "\n  Do you want to continue"
	if [ $? -ne 0 ]
	then
		return 1
	fi

	#
	# Select the ioctl method
	#
	$ECHO "\n  Please select a Device Interface Method:\n"
	$ECHO "  1.\tSunOS/Solaris/Linux/QNX ioctl method"
	$ECHO "  2.\tFreeBSD/NetBSD/OpenBSD ioctl method"
	$ECHO "  3.\tIBM AIX IDE ioctl method"
	$ECHO "  4.\tBSDI BSD/OS ATAPI"
	$ECHO "  5.\tSCO Open Server ATAPI BTLD"
	$ECHO "  6.\tDigital UNIX ATAPI"
	$ECHO "  q.\tquit (abort configuration)"

	while :
	do
		if getstr "\n  Enter choice:"
		then
			case "$ANS" in
			[1-6])
				break
				;;
			q)
				return 1
				;;
			*)
				$ECHO "  Please answer 1 to 6."
				;;
			esac
		else
			$ECHO "  Please answer 1 to 6."
		fi
	done

	IOCMETHOD="$ANS"

	if [ -z "$NUMDISCS" ]
	then
		YNDEF=n
		if getyn "\n  Is your drive on $XMCD_DEV a multi-disc changer"
		then
			while :
			do
				if getstr \
				"\n  How many discs does the drive support?:"
				then
				    case "$ANS" in
				    [1-9]*)
					NUMDISCS="$ANS"
					break
					;;
				    *)
					$ECHO "  Invalid input.  Try again."
					;;
				    esac
				else
				    $ECHO "  Invalid input.  Try again."
				fi
			done

			if [ "$NUMDISCS" -gt 1 ]
			then
				CHGMETHOD=3
				MULTIPLAY=True
			else
				CHGMETHOD=0
				MULTIPLAY=False
			fi
		else
			CHGMETHOD=0
			NUMDISCS=1
			MULTIPLAY=False
		fi
	fi

	# Set the rest of the parameters
	case "$IOCMETHOD" in
	1)	# SunOS/Solaris/Linux/QNX ioctl method
		METHOD=1
		VENDOR=0
		VOLBASE=0
		VOLTAPER=0
		PLAYMSF=True
		PLAY12=False
		PLAY10=False
		PLAYTI=True
		LOAD=True
		EJECT=True
		MODEDBD=False
		VOLSUPP=True
		BALSUPP=True
		CHRSUPP=False
		VOLCTL=3
		PAUSE=True
		CADDYLOCK=False
		CURPOSFMT=False
		PLAYNOTUR=False
		;;
	2)	# FreeBS/NetBSDD ioctl method
		METHOD=2
		VENDOR=0
		VOLBASE=0
		VOLTAPER=0
		PLAYMSF=True
		PLAY12=False
		PLAY10=False
		PLAYTI=True
		LOAD=True
		EJECT=True
		MODEDBD=False
		VOLSUPP=True
		BALSUPP=True
		CHRSUPP=False
		VOLCTL=3
		PAUSE=True
		CADDYLOCK=True
		CURPOSFMT=False
		PLAYNOTUR=False
		;;
	3)	# IBM AIX IDE ioctl method
		METHOD=3
		VENDOR=0
		VOLBASE=0
		VOLTAPER=0
		PLAYMSF=True
		PLAY12=False
		PLAY10=False
		PLAYTI=True
		LOAD=False
		EJECT=True
		MODEDBD=False
		VOLSUPP=True
		BALSUPP=True
		CHRSUPP=False
		VOLCTL=3
		PAUSE=True
		CADDYLOCK=True
		CURPOSFMT=False
		PLAYNOTUR=False
		;;
	4)	# BSDI BSD/OS ATAPI (SCSI emulation)
		# Set up as a "generic" SCSI-2 drive
		METHOD=0
		VENDOR=0
		VOLBASE=0
		VOLTAPER=0
		PLAYMSF=True
		PLAY12=False
		PLAY10=False
		PLAYTI=True
		LOAD=True
		EJECT=True
		MODEDBD=False
		VOLSUPP=False
		BALSUPP=False
		CHRSUPP=False
		VOLCTL=0
		PAUSE=True
		CADDYLOCK=False
		CURPOSFMT=True
		PLAYNOTUR=False
		CHGMETHOD=0
		NUMDISCS=1
		MULTIPLAY=False
		;;
	5)	# SCO Open Server ATAPI BTLD (SCSI emulation)
		# Set up as a "generic" SCSI-2 drive
		METHOD=0
		VENDOR=0
		VOLBASE=0
		VOLTAPER=0
		PLAYMSF=True
		PLAY12=False
		PLAY10=False
		PLAYTI=True
		LOAD=True
		EJECT=True
		MODEDBD=False
		VOLSUPP=True
		BALSUPP=True
		CHRSUPP=False
		VOLCTL=3
		PAUSE=True
		CADDYLOCK=False
		CURPOSFMT=False
		PLAYNOTUR=False
		CHGMETHOD=0
		NUMDISCS=1
		MULTIPLAY=False
		;;
	6)	# Digital UNIX ATAPI (SCSI emulation)
		# Set up as a "generic" SCSI-2 drive
		METHOD=0
		VENDOR=0
		VOLBASE=0
		VOLTAPER=0
		PLAYMSF=True
		PLAY12=False
		PLAY10=False
		PLAYTI=True
		LOAD=True
		EJECT=True
		MODEDBD=False
		VOLSUPP=True
		BALSUPP=True
		CHRSUPP=True
		VOLCTL=7
		PAUSE=True
		CADDYLOCK=True
		CURPOSFMT=True
		PLAYNOTUR=False
		CHGMETHOD=0
		NUMDISCS=1
		MULTIPLAY=False
		;;
	*)	# Should not get here
		$ECHO "  Error: unsupported ioctl method."
		return 1
		;;
	esac

	return 0
}


config_drive()
{
	if [ "$OS_SYS" = QNX ]
	then
		ask_nonscsi_config
		return $?
	fi

	eval `\
	(
		$ECHO "ENTRIES=\""
		cd $CFGDIR/.tbl
		for i in *
		do
			if [ -f $i ]
			then
				if fgrep tblver=3 $CFGDIR/.tbl/$i \
			   		>/dev/null 2>&1
				then
					$ECHO "$i \c"
				else
					logerr -p \
					"$CFGDIR/.tbl/$i version mismatch"
				fi
			fi
		done
		$ECHO "\""
	)`

	j=1
	>$TMPFILE
	if [ -n "$ENTRIES" ]
	then
		for i in $ENTRIES
		do
			brand=`fgrep tblalias= $CFGDIR/.tbl/$i 2>/dev/null | \
				sed 's/^.*tblalias=//'`
			if [ -z "$brand" ]
			then
				brand=$i
			fi
			$ECHO "  $j.\t$brand" >>$TMPFILE
			j=`expr $j + 1`
		done
	fi
	$ECHO "  $j.\tother (SCSI)" >>$TMPFILE
	j=`expr $j + 1`
	$ECHO "  $j.\tother (non-SCSI)" >>$TMPFILE
	$ECHO "  q.\tquit (abort configuration)" >>$TMPFILE

	$ECHO "\n  Device ($XMCD_DEV) configuration"
	$ECHO "  Please select the drive brand:\n"

	PGLEN=`wc -l $TMPFILE | $AWK '{ print $1 + 1 }'`
	if [ $PGLEN -gt 18 ]
	then
		PGLEN=`expr $PGLEN / 2`
		pr -t -2 -w78 -l$PGLEN $TMPFILE
	else
		cat $TMPFILE
	fi

	rm -f $TMPFILE

	while :
	do
		if getstr "\n  Enter choice:"
		then
			if [ "$ANS" = q ]
			then
				return 1
			elif [ "$ANS" -lt 1 -o "$ANS" -gt $j ]
			then
				$ECHO "  Please answer 1 to $j."
			elif [ "$ANS" = "`expr $j - 1`" ]
			then
				ask_scsi_config
				return $?
			elif [ "$ANS" = "$j" ]
			then
				ask_nonscsi_config
				return $?
			else
				k=1
				for i in $ENTRIES
				do
					if [ $k = $ANS ]
					then
						model_sel $i $CFGDIR/.tbl/$i
						return $?
					fi
					k=`expr $k + 1`
				done

				# Should not get here.
				return 1
			fi
		else
			$ECHO "  Please answer 1 to $j."
		fi
	done

	# Should not get here.
	return 1
}


model_sel()
{
	BRAND=$1
	CFGFILE=$2

	$ECHO "\n  Device ($XMCD_DEV) configuration"
	$ECHO "  Please select the $BRAND drive model:\n"

	$AWK -F: '
	BEGIN {
		n = 1
	}
	!/^#/ {
		if ($2 == 0) {
			if ($3 == 1)
				mode = "OS driver ioctl"
			else
				mode = "other"
		}
		else if ($2 == 1)
			mode = "SCSI-1"
		else if ($2 >= 2)
			mode = "SCSI-2"

		printf("  %d.\t%-12s%s\n", n, $1, mode)
		n++
	}
	END {
		printf("  %d.\t%-12s%s\n", n, "other", "SCSI")
		printf("  %d.\t%-12s%s\n", n+1, "other", "non-SCSI")
		printf("  q.\tquit (abort configuration)\n")
	}
	' $CFGFILE >$TMPFILE


	PGLEN=`wc -l $TMPFILE | $AWK '{ print $1 + 1 }'`
	if [ $PGLEN -gt 18 ]
	then
		$ECHO "\tModel       Mode\c"
		$ECHO "                       Model       Mode\n"
		PGLEN=`expr $PGLEN / 2`
		pr -t -2 -w78 -l$PGLEN $TMPFILE
	else
		$ECHO "\tModel       Mode\n"
		cat $TMPFILE
	fi

	rm -f $TMPFILE

	while :
	do
		if getstr "\n  Enter choice:"
		then
			j=`grep -v "^#" $CFGFILE | wc -l | sed 's/^[ 	]*//'`
			j=`expr $j + 2`

			if [ "$ANS" = q ]
			then
				return 1
			elif [ "$ANS" -lt 1 -o "$ANS" -gt $j ]
			then
				$ECHO "  Please answer 1 to $j."
			elif [ "$ANS" = "`expr $j - 1`" ]
			then
				ask_scsi_config
				return $?
			elif [ "$ANS" = "$j" ]
			then
				ask_nonscsi_config
				return $?
			else
				read_config $CFGFILE $ANS
				if [ "$METHOD" != 0 ]
				then
					ask_nonscsi_config
				fi
				return $?
			fi
		else
			$ECHO "  Please answer 1 to $j."
		fi
	done

	# Should not get here.
	return 1
}


read_config()
{
	eval `$AWK -F: '
	BEGIN {
		n = 1
	}
	!/^#/ {
		if (n == sel) {
			if ($2 > 0) {
				print "METHOD=0"
				printf("VENDOR=%d\n", $3)
			}
			else {
				printf("METHOD=%d\n", $3)
				print "VENDOR=0"
			}

			if ($4 == 0)
				print "PLAYMSF=False"
			else
				print "PLAYMSF=True"
			if ($5 == 0)
				print "PLAY12=False"
			else
				print "PLAY12=True"
			if ($6 == 0)
				print "PLAY10=False"
			else
				print "PLAY10=True"
			if ($7 == 0)
				print "PLAYTI=False"
			else
				print "PLAYTI=True"
			if ($8 == 0)
				print "LOAD=False"
			else
				print "LOAD=True"
			if ($9 == 0)
				print "EJECT=False"
			else
				print "EJECT=True"
			if ($10 == 0)
				print "MODEDBD=False"
			else
				print "MODEDBD=True"

			printf("VOLCTL=%d\n", $11)
			printf("VOLBASE=%d\n", $12)
			printf("VOLTAPER=%d\n", $13)

			if ($14 == 0)
				print "PAUSE=False"
			else
				print "PAUSE=True"
			if ($15 == 0)
				print "CADDYLOCK=False"
			else
				print "CADDYLOCK=True"
			if ($16 == 0)
				print "CURPOSFMT=False"
			else
				print "CURPOSFMT=True"
			if ($17 == 0)
				print "PLAYNOTUR=False"
			else
				print "PLAYNOTUR=True"

			printf("CHGMETHOD=%d\n", $18)
			printf("NUMDISCS=%d\n", $19)
			if ($18 == 0)
				print "MULTIPLAY=False"
			else
				print "MULTIPLAY=True"
		}
		n++
	}
	' sel=$2 $1`

	return $?
}


get_protocol()
{
	$ECHO "\nYou should choose HTTP if your system is behind a"
	$ECHO "firewall that only allows web access to the Internet."
	$ECHO "Otherwise, choose CDDBP.  There are more public CDDB"
	$ECHO "servers supporting CDDBP than HTTP."
	while :
	do
		$ECHO "\n  Protocol Selection\n"
		$ECHO "  1.\tCDDBP (CD Database Protocol)"
		$ECHO "  2.\tHTTP  (Hyper-Text Transport Protocol)"
		$ECHO "  q.\tquit (abort configuration)"

		if getstr "\n  Enter Choice: [1]"
		then
			case "$ANS" in
			1)
				PROTOCOL=cddbp
				break
				;;
			2)
				PROTOCOL=http
				break
				;;
			q)
				$ECHO "\nConfiguration aborted." >&2
				logerr -n "Configuration aborted by user"
				doexit 3
				;;
			*)
				$ECHO "\nInvalid selection.  Try again."
				;;
			esac
		else
			PROTOCOL=cddbp
			return
		fi
	done
}


get_proxy()
{
	YNDEF=n
	getyn "\n  Do you want to use a proxy server"
	if [ $? -ne 0 ]
	then
		USEPROXY=False
		PROXYSERVER="yourproxyhost:80"
		return
	fi

	USEPROXY=True
	while :
	do
		if getstr "  Enter the proxy server host name or IP number:"
		then
			if ($ECHO "$ANS" | grep "[ 	]") >/dev/null 2>&1
			then
				$ECHO "  Invalid input.  Try again."
				continue
			else
				PROXYSERVER="$ANS"
			fi

			if getstr "  Enter the proxy server port number: [80]"
			then
				PROXYSERVER="${PROXYSERVER}:${ANS}"
			fi

			break
		else
			$ECHO "  Invalid input.  Try again."
		fi
	done
}


get_useserver()
{
	$ECHO "\nThis configuration procedure can connect to the central"
	$ECHO "CD database server to retrieve the latest list of categories"
	$ECHO "and public server sites.  A built-in list can also be used"
	$ECHO "that is correct at the time this version of xmcd is released."

	USESERVER=0
	while :
	do
		$ECHO "\n  CDDB Information\n"
		$ECHO "  1.\tConnect to CDDB server and get current list"
		$ECHO "  2.\tUse built-in list"
		$ECHO "  q.\tquit (abort configuration)\n"

		getstr "  Enter choice: [1]"
		if [ $? -ne 0 ]
		then
			ANS=1
		fi

		case "$ANS" in
		1)
			if [ -x $BINDIR/cddbcmd ]
			then
				return 0
			else
				$ECHO "\nCannot find the cddbcmd(1) utility."
				$ECHO "This is needed to connect to the server."
			fi
			;;
		2)
			return 1
			;;
		q)
			$ECHO "\nConfiguration aborted." >&2
			doexit 3
			;;
		*)
			$ECHO "\nInvalid input.  Please try again."
			;;
		esac
	done
	return 1
}


get_host()
{
	if getstr "    Server host name or IP number:"
	then
		HOST="$ANS"
		return 0
	else
		$ECHO "    No host specified."
		return 1
	fi
}


get_port()
{
	if [ "$PROTOCOL" = http ]
	then
		DFLT_PORT=80
	else
		DFLT_PORT=888
	fi

	if getstr "    TCP port number: [$DFLT_PORT]"
	then
		PORT="$ANS"
	else
		PORT=$DFLT_PORT
	fi
}


get_cgipath()
{
	if getstr "    Enter CGI path: [$DFLT_CGIPATH]"
	then
		CGIPATH="$ANS"
	else
		CGIPATH="$DFLT_CGIPATH"
	fi
}


print_servers()
{
	$ECHO "\n  CDDB server sites selection"
	$ECHO "\n\tServer host               Proto Port   Location\n"
	$AWK '
	BEGIN	{
		n = 1
	}
	{
		printf("  %d.\t%-25s %-5s %-6s ", n, $1, $2, $3)
		for (i = 7; i <= NF; i++)
			printf("%s ", $i);
		printf("\n")
		n++;
	}
	END	{
		printf("  %d.\tother site not listed\n", n++)
		printf("  ?.\tshow this list again\n")
		printf("  q.\tquit (abort configuration)\n")
		printf("  c.\tdone adding servers\n\n")
	}
	' $TMPFILE
}


set_server_entry()
{
	if [ ! $1 -eq 0 ]
	then
		eval `\
		(
			$AWK '
			BEGIN	{
				n = 1
			}
			{
				if (n == i) {
					printf("HOST=%s\n", $1)
					printf("PORT=%s\n", $3)
					printf("CGIPATH=%s\n", $4)
				}
				n++;
			}
			' i=$1 $TMPFILE
		)`
	fi

	if [ -z "$HOST" -o -z "$PORT" ]
	then
		logerr -p "Internal error: undefined HOST or PORT!"
		doexit 1
	fi

	case "$PROTOCOL" in
	cddbp)
		if [ $PORT -eq 888 ]
		then
			ENTRY="cddbp://${HOST}"
		else
			ENTRY="cddbp://${HOST}:${PORT}"
		fi
		;;
	http)
		if [ $PORT -eq 80 ]
		then
			ENTRY="http://${HOST}${CGIPATH}"
		else
			ENTRY="http://${HOST}:${PORT}${CGIPATH}"
		fi
		;;
	*)
		return
		;;
	esac

	CDDBPATH="$CDDBPATH;$ENTRY"
	$ECHO "    Added \"$ENTRY\" to cddbPath."
}


prompt_server()
{
	get_host
	if [ $? -ne 0 ]
	then
		return 1
	fi

	get_port

	if [ "$PROTOCOL" = http ]
	then
		get_cgipath
	fi

	set_server_entry 0
	return 0
}


sel_servers()
{
	$AWK '
	/^#/ || /^$/ { next }
	{ if ($2 == protocol) print }
	' protocol="$PROTOCOL" $SITES >$TMPFILE

	NUMSERVERS=`wc -l $TMPFILE | $AWK '{ print $1 }'`
	USRSPEC=`expr $NUMSERVERS + 1`

	$ECHO "\nYou may configure more than one server site.  Choose the"
	$ECHO "servers closest to you.  Enter the selections one at a time."
	$ECHO "Type 'c' when done."

	cnt=0
	print_servers
	while :
	do
		if getstr "  Enter CDDB server selection:"
		then
			case "$ANS" in
			[0-9]*)
			    if [ "$ANS" -lt 1 -o "$ANS" -gt $NUMSERVERS ]
			    then
				if [ "$ANS" -eq $USRSPEC ]
				then
				    if prompt_server
				    then
					cnt=`expr $cnt + 1`
				    fi
				else
				    $ECHO "\nInvalid selection.  Try again."
				fi
			    else
				set_server_entry $ANS
				cnt=`expr $cnt + 1`
			    fi
			    ;;
			'?')
			    print_servers
			    ;;
			c)
			    $ECHO "\nTotal CDDB servers configured: $cnt"
			    break
			    ;;
			q)
			    $ECHO "\nConfiguration aborted." >&2
			    logerr -n "Configuration aborted by user"
			    doexit 3
			    ;;
			*)
			    $ECHO "  Please enter 1-${USRSPEC}, c, ? or q."
			    ;;
			esac
		else
			$ECHO "  Please enter 1-${USRSPEC}, c, ? or q."
		fi
	done
	rm -f $TMPSITES $TMPFILE
}


check_local_cddb()
{
	$ECHO "\nChecking local CD database...\n"
	if [ -z "$CDDBCATS" ]
	then
		$ECHO "\nNo CD database categories defined!"
		return
	fi

	for i in $CDDBCATS
	do
		makedir $XMCDLIB/cddb/$i $CDIRPERM $OWNER $GROUP
	done
}


cddb_config()
{
	# Set up default cddbPath
	cnt=0
	for i in $CDDBCATS
	do
		if [ $cnt -eq 0 ]
		then
			CDDBPATH="$i"
		else
			CDDBPATH="${CDDBPATH};$i"
		fi
		cnt=`expr $cnt + 1`
	done

	$ECHO "\nIf your system has Internet connectivity and functional"
	$ECHO "domain name service (DNS), you should answer 'y' to the next"
	$ECHO "question.  If you have a dial-up Internet connection that"
	$ECHO "needs to be manually started, you should start up the link"
	$ECHO "now before you continue with this configuration procedure."
	$ECHO "If this system is not linked to the Internet, then answer 'n'."

	YNDEF=y
	getyn "\n  Would you like to use Internet CD database servers"
	if [ $? -ne 0 ]
	then
		$ECHO "\nNo Internet CD database servers are configured."

		# Check and set up local CDDB directories
		check_local_cddb
		return
	fi

	# Get which protocol to use
	get_protocol
	if [ "$PROTOCOL" = http ]
	then
		get_proxy
	fi

	# Get current CDDB categories and servers list
	if get_useserver
	then
	    $ECHO "\nConnecting to the central CDDB server."

	    OPTS=""
	    if [ "$PROTOCOL" = http ]
	    then
		OPTS="$OPTS -m http"
		if [ $USEPROXY = True ]
		then
		    OPTS="$OPTS -p $PROXYSERVER"
		fi
	    fi

	    # Categories
	    $ECHO "Getting CDDB categories... \c"
	    cnt=0
	    while [ $cnt -lt 5 ]
	    do
		CATS=`$BINDIR/cddbcmd $OPTS cddb lscat 2>/dev/null`
		stat=$?
		if [ $stat -eq 0 ]
		then
		    break
		fi
		sleep 5
		$ECHO "retrying \c"
		cnt=`expr $cnt + 1`
	    done
	    if [ $stat -ne 0 -o -z "$CATS" ]
	    then
		$ECHO "Failed"
		$ECHO "Cannot obtain CDDB categories list from central server."
		$ECHO "Using built-in list."
	    else
		$ECHO "Success"
		CDDBCATS="$CATS"

		# Set up cddbPath
		cnt=0
		for i in $CDDBCATS
		do
		    if [ $cnt -eq 0 ]
		    then
			CDDBPATH="$i"
		    else
			CDDBPATH="${CDDBPATH};$i"
		    fi
		    cnt=`expr $cnt + 1`
		done
	    fi

	    # Sites
	    $ECHO "Getting CDDB public server sites list... \c"
	    cnt=0
	    while [ $cnt -lt 5 ]
	    do
		$BINDIR/cddbcmd $OPTS sites 2>/dev/null | \
		    grep -v unofficial >$TMPSITES
		stat=$?
		if [ $stat -eq 0 ]
		then
		    break
		fi
		sleep 5
		$ECHO "retrying \c"
		cnt=`expr $cnt + 1`
	    done
	    if [ $stat -ne 0 -o ! -s $TMPSITES ]
	    then
		$ECHO "Failed"
		$ECHO "Cannot obtain server sites list from central server."
		$ECHO "Using built-in list."
	    else
		$ECHO "Success"
		SITES=$TMPSITES
	    fi
	fi

	if [ ! -r $SITES ]
	then
		$ECHO "Warning: Cannot read $SITES."
		SITES=/dev/null
	fi

	$ECHO "\nPress <RETURN> to continue. \c"
	read ANS

	# Let user choose the CDDB servers
	sel_servers

	# Check and set up local CDDB directories
	check_local_cddb
}


browser_config()
{
	$ECHO "\nSearching for installed web browsers..."
	BROWSER=
	WORD=A
	for j in $BROWSERS
	do
		for i in	/usr/bin/X11 \
				/usr/X/bin \
				/usr/X11/bin \
				/usr/X11R6/bin \
				/usr/X11R5/bin \
				/usr/X386/bin \
				/usr/openwin/bin \
				/usr/local/bin/X11 \
				/usr/local/bin \
				/usr/lbin
		do
		    if [ -x $i/$j ]
		    then
			$ECHO "\nThe following is found on your system:"
			$ECHO "    $i/$j"
			YNDEF=y
			if getyn "\n  Do you want xmcd to use this browser"
			then
				BDIR=$i
				BROWSER=$j
				break
			else
				WORD=Another
			fi
		    fi
		done
		if [ -n "$BROWSER" ]
		then
		    break
		fi
	done

	if [ -z "$BROWSER" ]
	then
		$ECHO "\n$WORD web browser cannot be found on your system."
		$ECHO "You must have Netscape Navigator/Communicator or"
		$ECHO "or NCSA Mosaic in order to use the wwwWarp feature"
		$ECHO "in xmcd.  If you have one of these browsers, please"
		$ECHO "enter its full path name."

		while :
		do
		    BDIR=/usr/bin/X11
		    i=netscape

		    $ECHO "\n  Enter web browser path, type \"c\" to \c"
		    $ECHO "continue without\n  specifying a browser path, \c"
		    $ECHO "or type \"q\" to abort."
		    if getstr "  [$BDIR/$i]:"
		    then
			    if [ "$ANS" = c ]
			    then
				break
			    elif [ "$ANS" = q ]
			    then
				$ECHO "\nConfiguration aborted." >&2
				logerr -n "Configuration aborted by user"
				doexit 3
			    fi

			    if [ -x "$ANS" ]
			    then
				BDIR=`dirname $ANS`
				i=`basename $ANS`
				for j in $BROWSERS
				do
					if [ $j = $i ]
					then
						BROWSER=$i
						break
					fi
				done
				if [ -z "$BROWSER" ]
				then
					$ECHO "\n$i is not a supported browser."
				else
					break
				fi
			    else
				$ECHO "\n$ANS does not exist!"
			    fi
			else
			    BROWSER=$i
			    break
			fi
		done
	fi

	#
	# Make netscape the default setting
	#
	BROWSER_RMT="netscape -remote 'openURL(%U)' >/dev/null 2>&1"
	BROWSER_DIR="netscape '%U' >/dev/null 2>&1 &"

	if [ -n "$BROWSER" ]
	then
	    #
	    # Set up browser configuration
	    #
	    case $BROWSER in
	    netscape)
		    BROWSER_RMT="$BDIR/$BROWSER_RMT"
		    BROWSER_DIR="$BDIR/$BROWSER_DIR"
		    ;;
	    Mosaic)
		    BROWSER_RMT="$SCRDIR/ncsarmt '%U'"
		    BROWSER_DIR="$BDIR/Mosaic '%U' >/dev/null 2>&1 &"
		    ;;
	    esac

	    $ECHO "\nXmcd has been configured to use the $BROWSER browser."
	else
	    $ECHO "\nNo web browser specified."
	    $ECHO "\n  Do you have a supported web browser that can be run"
	    YNDEF=n
	    if getyn "  from a remote system"
	    then
		while :
		do
		    $ECHO "\n  Enter the remote system host name, \c"
		    $ECHO "type \"c\" to continue without\n  specifying \c"
		    $ECHO "a host name, \c"
		    if getstr "or type \"q\" to abort:"
		    then
			case "$ANS" in
			c)
			    break
			    ;;
			q)
			    $ECHO "\nConfiguration aborted." >&2
			    logerr -n "Configuration aborted by user"
			    doexit 3
			    ;;
			*)
			    RMTSYS="$ANS"
			    break
			esac
		    fi
		done

		if [ -n "$RMTSYS" ]
		then
		    while :
		    do
			BDIR=/usr/bin/X11
			i=netscape

			$ECHO "\n  Enter web browser path on \"$RMTSYS\", \c"
			$ECHO "type \"c\" to\n  continue without specifying \c"
			$ECHO "a path, or type \"q\" to abort."
			if getstr "  [$BDIR/$i]: "
			then
			    if [ "$ANS" = c ]
			    then
				break
			    elif [ "$ANS" = q ]
			    then
				$ECHO "\nConfiguration aborted." >&2
				logerr -n "Configuration aborted by user"
				doexit 3
			    fi

			    BDIR=`dirname $ANS`
			    i=`basename $ANS`
			    for j in $BROWSERS
			    do
				if [ $j = $i ]
				then
				    BROWSER=$i
				    break
				fi
			    done

			    if [ -z "$BROWSER" ]
			    then
				$ECHO "\n$i is not a supported browser."
			    else
				break
			    fi
			else
			    BROWSER=$i
			    break
			fi
		    done
		fi
	    fi

	    if [ -n "$BROWSER" ]
	    then
		case $BROWSER in
		netscape)
			SCR=$SCRDIR/nswrap
			;;
		Mosaic)
			SCR=$SCRDIR/ncsawrap
			;;
		esac
		BROWSER_RMT="$SCR -remote $RMTSYS $BDIR/$BROWSER '%U'"
		BROWSER_DIR="$SCR -direct $RMTSYS $BDIR/$BROWSER '%U' &"

		$ECHO "\nXmcd has been configured to use the $BROWSER"
		$ECHO "browser remotely on host \"$RMTSYS\".  Xmcd"
		$ECHO "users must have rsh(1) permissions on $RMTSYS"
		$ECHO "in order to use the wwwWarp feature.  See"
		$ECHO "rhosts(4) and hosts.equiv(4) for details."
	    else
		$ECHO "\nNo web browser is configured."
		$ECHO "The xmcd wwwWarp feature will not function."
	    fi
	fi
}


#
# Main starts here
#

# Catch some signals
trap "rm -f $TMPFILE $TMPSITES; exit 1" 1 2 3 5 15

# Get platform information
OS_SYS=`(uname -s) 2>/dev/null`
OS_VER=`(uname -r) 2>/dev/null`
OS_NODE=`(uname -n) 2>/dev/null`

# Use Sysv echo if possible
if [ -x /usr/5bin/echo ]				# SunOS SysV echo
then
	ECHO=/usr/5bin/echo
elif [ -z "`(echo -e a) 2>/dev/null | fgrep e`" ]	# GNU bash, etc.
then
	ECHO="echo -e"
else							# generic SysV
	ECHO=echo
fi
if [ "$OS_SYS" = QNX ]
then
	ECHO=echo
fi

# If awk doesn't work well on your system, try changing the
# following to nawk or gawk.
AWK=awk

# Error log file handling
if [ -f $ERRFILE -a ! -w $ERRFILE ]
then
	ERRFILE=/dev/null
fi

$ECHO "\nXmcd version $VER Configuration Program"
$ECHO "Setting up for host: ${OS_NODE}"
$ECHO "--------------------------------------"

# Sanity check

if [ ! -w $CFGDIR ]
then
	logerr -p "No write permission in $CFGDIR"
	doexit 1
fi

if [ ! -r $CFGDIR/device.cfg ]
then
	logerr -p "Cannot find $CFGDIR/device.cfg"
	doexit 2
fi

fgrep cfgver=1 $CFGDIR/device.cfg >/dev/null 2>&1
if [ $? -ne 0 ]
then
	logerr -p "$CFGDIR/device.cfg version mismatch"
	doexit 2
fi

if [ ! -d $CFGDIR/.tbl ]
then
	logerr -p "The directory $CFGDIR/.tbl is missing"
	doexit 2
fi

# Configure platform-dependent and device-dependent parameters

DEVPREF=/dev/rcdrom
DEVSUFF=
FIRST=0
BLKDEV=0
VOLMGT=False
STOPONLOAD=True
EJECTONEXIT=False
STOPONEXIT=True
EXITONEJECT=False
CLOSEONEJECT=False
MAILCMD="mailx -s '%S' %A <%F >/dev/null 2>&1"
USEPROXY=False
PROXYSERVER="yourproxyhost:80"
CDDBPATH=

# Determine what platform we are running on

if [ "$OS_SYS" = AIX ]
then
	# IBM AIX
	DEVPREF=/dev/rcd
	DEVSUFF=
	FIRST=0
elif [ "$OS_SYS" = A/UX ]
then
	# Apple A/UX
	DEVPREF=/dev/scsi/
	DEVSUFF=
	FIRST=3
	MAILCMD="mush -s '%S' %A <%F >/dev/null 2>&1"
elif [ "$OS_SYS" = BSD/OS ]
then
	# BSDI BSD/OS
	DEVPREF=/dev/rsd
	DEVSUFF=c
	FIRST=2
	MAILCMD="Mail -s '%S' %A <%F >/dev/null 2>&1"
elif [ "$OS_SYS" = dgux ]
then
	# Data General DG/UX
	DEVPREF="/dev/scsi/scsi(ncsc@7(FFFB0000,7),"
	DEVSUFF=",0)"
	FIRST=2
elif [ "$OS_SYS" = FreeBSD ]
then
	# FreeBSD
	DEVPREF=/dev/rcd
	DEVSUFF=c
	FIRST=0
	MAILCMD="Mail -s '%S' %A <%F >/dev/null 2>&1"
elif [ "$OS_SYS" = HP-UX ]
then
	case $OS_VER in
	[AB].09*)	# HP-UX 9.x
		DEVPREF=/dev/rdsk/c201d
		DEVSUFF=s0
		FIRST=4
		;;
	B.10*)		# HP-UX 10.x
		DEVPREF=/dev/rdsk/c0t
		DEVSUFF=s0
		FIRST=4
		;;
	B.11*)		# HP-UX 11.x
		DEVPREF=/dev/rdsk/c0t
		DEVSUFF=s0
		FIRST=4
		;;
	*)
		OS_VER=unknown
		;;
	esac
elif [ "$OS_SYS" = IRIX -o "$OS_SYS" = IRIX64 ]
then
	# SGI IRIX
	DEVPREF=`hinv | grep CDROM | line | \
		sed 's/^.*controller \([0-9]*\).*$/\/dev\/scsi\/sc\1d/'`
	DEVSUFF=l0
	FIRST=`hinv | grep CDROM | line | sed 's/^.*unit \([0-9]*\).*$/\1/'`
elif [ "$OS_SYS" = Linux ]
then
	# Linux
	DEVPREF=/dev/scd
	DEVSUFF=
	FIRST=0
	BLKDEV=1
	MAILCMD="mail -s '%S' %A <%F >/dev/null 2>&1"
elif [ "$OS_SYS" = NetBSD ]
then
	# NetBSD
	DEVPREF=/dev/rcd
	case "`uname -m`" in
	*86)
		DEVSUFF=d
		;;
	*)
		DEVSUFF=c
		;;
	esac
	FIRST=0
	MAILCMD="Mail -s '%S' %A <%F >/dev/null 2>&1"
elif [ "$OS_SYS" = OpenBSD ]
then
	# OpenBSD
	DEVPREF=/dev/rcd
	case "`uname -m`" in
	*86)
		DEVSUFF=d
		;;
	*)
		DEVSUFF=c
		;;
	esac
	FIRST=0
	MAILCMD="Mail -s '%S' %A <%F >/dev/null 2>&1"
elif [ "$OS_SYS" = OSF1 ]
then
	case "`uname -m`" in
	alpha)	# Digital OSF/1
		DEVPREF=/dev/rrz
		DEVSUFF=c
		FIRST=4
		;;
	*)
		OS_VER=unknown
		;;
	esac
elif [ "$OS_SYS" = QNX ]
then
	# QNX
	DEVPREF=/dev/cd
	DEVSUFF=
	FIRST=0
	BLKDEV=1
elif [ "$OS_SYS" = SINIX-N ]
then
	# SNI SINIX-N
	DEVPREF=/dev/ios0/rsdisk
	DEVSUFF=s0
	FIRST=005
elif [ "$OS_SYS" = SINIX-P ]
then
	# SNI SINIX-P
	DEVPREF=/dev/ios0/rsdisk
	DEVSUFF=s0
	FIRST=006
elif [ "$OS_SYS" = SunOS ]
then
	case $OS_VER in
	4.*)	# SunOS 4.x
		case `arch -k` in
		sun4[cm])
			DEVPREF=/dev/rsr
			DEVSUFF=
			FIRST=0
			MAILCMD="Mail -s '%S' %A <%F >/dev/null 2>&1"
			;;
		*)
			OS_VER=unknown
			;;
		esac
		;;
	5.*)	# SunOS 5.x
		YNDEF=y
		if getyn \
		"Does your system support the Volume Manager (/usr/sbin/vold)"
		then
			DEVPREF=/vol/dev/aliases/cdrom
			DEVSUFF=
			FIRST=0
			VOLMGT=True
			CLOSEONEJECT=True
		else
			DEVPREF=/dev/rdsk/c0t
			DEVSUFF=d0s0
			FIRST=6
		fi
		;;
	*)
		OS_VER=unknown
		;;
	esac
elif [ "$OS_SYS" = ULTRIX ]
then
	case "`uname -m`" in
	RISC)	# Digital Ultrix
		DEVPREF=/dev/rrz
		DEVSUFF=c
		FIRST=4
		MAILCMD="Mail -s '%S' %A <%F >/dev/null 2>&1"
		;;
	*)
		OS_VER=unknown
		;;
	esac
elif [ -x /bin/ftx ] && ftx
then
	case $OS_VER in
	4.*)	
		if [ -x /bin/hppa ] && hppa
		then
			# Stratus FTX SVR4/PA-RISC
			DEVPREF=/dev/rcdrom/c0a2d
			DEVSUFF=l0
			FIRST=0
		else
			# On non-supported FTX variants
			OS_VER=unknown
		fi
		;;
	*)
		OS_VER=unknown
		;;
	esac
elif [ -x /bin/i386 -o -x /sbin/i386 ] && i386
then
	case $OS_VER in
	3.2)	# SCO UNIX/ODT/Open Server
		DEVPREF=/dev/rcd
		DEVSUFF=
		FIRST=0
		;;
	4.0)	# UNIX SVR4.0/x86
		DEVPREF=/dev/rcdrom/cd
		DEVSUFF=
		FIRST=0
		;;
	4.1)	# UNIX SVR4.1/x86
		DEVPREF=/dev/rcdrom/cdrom
		DEVSUFF=
		FIRST=1
		;;
	4.2)	# UNIX SVR4.2/x86 (UnixWare 1.x)
		DEVPREF=/dev/rcdrom/cdrom
		DEVSUFF=
		FIRST=1
		;;
	4*MP)	# UNIX SVR4.2MP/x86 (UnixWare 2.x)
		DEVPREF=/dev/rcdrom/cdrom
		DEVSUFF=
		FIRST=1
		;;
	5)	# UNIX SVR5/x86 (UnixWare 7)
		DEVPREF=/dev/rcdrom/cdrom
		DEVSUFF=
		FIRST=1
		;;
	5.*)	# UNIX SVR5.*/x86 (UnixWare 7.x)
		DEVPREF=/dev/rcdrom/cdrom
		DEVSUFF=
		FIRST=1
		;;
	*)
		OS_VER=unknown
		;;
	esac
elif [ -x /bin/m88k ] && m88k
then
	case $OS_VER in
	4.0)	# UNIX SVR4.0/88k
		DEVPREF=/dev/rdsk/m187_c0d
		DEVSUFF=s7
		FIRST=3
		;;
	*)
		OS_VER=unknown
		;;
	esac
elif [ -r /vmunix ] && (strings /vmunix | fgrep NEWS-OS) >/dev/null 2>&1
then
	# Sony NEWS-OS
	DEVPREF=/dev/rsd
	DEVSUFF=c
	FIRST=06
else
	OS_VER=unknown
fi

NOT_SUPPORTED="Error: You are not running an operating system that's\n\
currently supported by xmcd."

if [ "$OS_VER" = unknown ]
then
	$ECHO "$NOT_SUPPORTED"
	YNDEF=n
	getyn "Would you like to proceed anyway"
	if [ $? -ne 0 ]
	then
		$ECHO "\nConfiguration aborted." >&2
		logerr -n "Configuration aborted by user"
		doexit 3
	fi
fi

#
# Configure CD database server access
#
$ECHO "\n*** CDDB CONFIGURATION ***"
cddb_config

#
# Web browser setup
#
$ECHO "\n\n*** WEB BROWSER CONFIGURATION ***"
browser_config

#
# Drive configuration
#
$ECHO "\n\n*** DRIVE CONFIGURATION ***"
$ECHO "\nNote that the default device shown is usually a SCSI CD-ROM."
$ECHO "On some platforms, you may need to specify a difference device"
$ECHO "if your CD-ROM drive is not SCSI."
EXITSTAT=0
DRVNO=0
DEVNO=$FIRST
SEDLINE=
while :
do
	$ECHO "\nConfiguring drive $DRVNO..."
	NUMDISCS=

	DEFAULT_DEV="${DEVPREF}${DEVNO}${DEVSUFF}"

	while :
	do
		if getstr "\n  Enter device path: [$DEFAULT_DEV]"
		then
			XMCD_DEV=$ANS
		else
			XMCD_DEV=$DEFAULT_DEV
		fi

		if [ $VOLMGT = True ]
		then
			break
		fi
		if [ $BLKDEV = 0 -a -c $XMCD_DEV ]
		then
			break
		fi
		if [ $BLKDEV = 1 -a -b $XMCD_DEV ]
		then
			break
		fi

		$ECHO "  $XMCD_DEV is an invalid device."
	done

	if [ $DRVNO -eq 0 ]
	then
		#
		# Configure app-defaults/XMcd file
		#
		chmod 644 $APPDEFAULTSDIR/XMcd 2>/dev/null
		if [ -w $APPDEFAULTSDIR/XMcd ]
		then
			$AWK '
			/^XMcd\.libdir:/ {
				printf("XMcd.libdir:\t\t\t%s\n", xmcdlib)
				next
			}
			/^XMcd\.cddbMailCmd:/ {
				printf("XMcd.cddbMailCmd:\t\t%s\n", mailcmd)
				next
			}
			{
				print $0
			}' xmcdlib="$XMCDLIB" mailcmd="$MAILCMD" \
				$APPDEFAULTSDIR/XMcd > /tmp/xmcd.$$

			cp /tmp/xmcd.$$ $APPDEFAULTSDIR/XMcd
			rm -f /tmp/xmcd.$$

			if [ $LIBDIR != "/usr/lib/X11" -a \
			     -d /usr/lib/X11/app-defaults ]
			then
				rm -f /usr/lib/X11/app-defaults/._trash_
				>$APPDEFAULTSDIR/._trash_

				if [ ! -f /usr/lib/X11/app-defaults/._trash_ ]
				then
					rm -f /usr/lib/X11/app-defaults/XMcd
					dolink $APPDEFAULTSDIR/XMcd \
						/usr/lib/X11/app-defaults/XMcd
				fi

				rm -f $APPDEFAULTSDIR/._trash_
			fi
		else
			logerr -p "Cannot configure $APPDEFAULTSDIR/XMcd"
			EXITSTAT=1
		fi

		#
		# Configure common.cfg file
		#
		chmod 644 $CFGDIR/common.cfg 2>/dev/null
		if [ -w $CFGDIR/common.cfg ]
		then
			$AWK '
			/^device:/	{
				printf("device:\t\t\t%s\n", device)
				next
			}
			/^solaris2VolumeManager:/ {
				printf("solaris2VolumeManager:\t%s\n", volmgt)
				next
			}
			/^cddbPath:/ {
				printf("cddbPath:\t%s\n", cddbpath)
				next
			}
			/^cddbUseHttpProxy:/ {
				printf("cddbUseHttpProxy:\t%s\n", useproxy)
				next
			}
			/^proxyServer:/ {
				printf("proxyServer:\t\t%s\n", proxyserver)
				next
			}
			/^browserRemote:/ {
				printf("browserRemote:\t%s\n", browserrmt)
				next
			}
			/^browserDirect:/ {
				printf("browserDirect:\t%s\n", browserdir)
				next
			}
			{
				print $0
			}' \
				device="$XMCD_DEV" \
				volmgt="$VOLMGT" \
				cddbpath="$CDDBPATH" \
				useproxy="$USEPROXY" \
				proxyserver="$PROXYSERVER" \
				browserrmt="$BROWSER_RMT" \
				browserdir="$BROWSER_DIR" \
				$CFGDIR/common.cfg > /tmp/xmcd.$$

			cp /tmp/xmcd.$$ $CFGDIR/common.cfg
			rm -f /tmp/xmcd.$$
			(cd $CFGDIR ;\
			 rm -f common.cfg-${OS_NODE} ;\
			 dolink common.cfg common.cfg-${OS_NODE})
		else
			logerr -p \
			    "Cannot configure $CFGDIR/common.cfg"
			EXITSTAT=1 
		fi
	fi

	CONFIGFILE=`basename $XMCD_DEV`

	config_drive
	CFGSTAT=$?
	if [ $CFGSTAT -ne 0 ]
	then
		$ECHO "\nConfiguration aborted." >&2
		logerr -n "Configuration aborted by user."
		doexit $CFGSTAT
	fi

	if [ $NUMDISCS -gt 1 ]
	then
		$ECHO "\n  This drive is a ${NUMDISCS}-disc changer."

		case $CHGMETHOD in
		1)
			# SCSI LUN addressing method
			$ECHO "\n  In order to change discs, your system must support separate device"
			$ECHO "  nodes for each LUN of the multi-disc changer."

			YNDEF=y
			if getyn "  Does your OS platforms support this?"
			then
				$ECHO "\n  Please enter the device nodes now."

				n=1
				DEVLIST=
				LUNDEV=$XMCD_DEV
				while [ $n -le $NUMDISCS ]
				do
					#
					# Construct a default device
					#
					if [ "$OS_SYS" != HP-UX ] && \
					   (echo $LUNDEV | grep "s[0-9]*$") \
					   >/dev/null 2>&1
					then
					    LUNPREF=`echo $LUNDEV | \
						sed 's/\(.*\)[0-9]s[0-9]*/\1/'`
					    LUNNUM=`echo $LUNDEV | \
						sed 's/.*\([0-9]\)s[0-9]*/\1/'`
					    LUNSUFF=`echo $LUNDEV | \
						sed 's/.*[0-9]\(s[0-9]*\)/\1/'`
					else
					    LUNPREF=`echo $LUNDEV | \
						sed 's/\(.*\)[0-9][^0-9]*/\1/'`
					    LUNNUM=`echo $LUNDEV | \
						sed 's/.*\([0-9]\)[^0-9]*/\1/'`
					    LUNSUFF=`echo $LUNDEV | \
						sed 's/.*[0-9]\([^0-9]*\)/\1/'`
					fi

					if [ -n "$LUNNUM" ]
					then
						if [ $n -gt 1 ]
						then
						    LUNNUM=`expr $LUNNUM + 1`
						fi
					else
						LUNNUM=`expr $n - 1`
					fi

					SAVDEV=$LUNDEV
					LUNDEV="${LUNPREF}${LUNNUM}${LUNSUFF}"

					if getstr "  Disc $n: [$LUNDEV]"
					then
						LUNDEV=$ANS
					fi

					if [ $BLKDEV = 0 -a ! -c $LUNDEV ]
					then
						$ECHO "  $LUNDEV is invalid.  Try again."
						LUNDEV=$SAVDEV
						continue
					fi
					if [ $BLKDEV = 1 -a ! -b $LUNDEV ]
					then
						$ECHO "  $LUNDEV is invalid.  Try again."
						LUNDEV=$SAVDEV
						continue
					fi

					if [ -z "$DEVLIST" ]
					then
						DEVLIST=$LUNDEV
					else
						DEVLIST="${DEVLIST};${LUNDEV}"
					fi

					n=`expr $n + 1`
				done
			else
				$ECHO "  It will be treated as a single disc player."
				CHGMETHOD=0
				NUMDISCS=1
				MULTIPLAY=False
				DEVLIST=$XMCD_DEV
			fi
			;;
		2)
			# SCSI medium changer method
			$ECHO "\n  In order to change discs, your system must support a separate device"
			$ECHO "  node for the medium changer mechanism."

			DEVLIST=$XMCD_DEV

			YNDEF=y
			if getyn "  Does your OS platforms support this?"
			then
			    while :
			    do
				LUNDEV=/dev/changer
				if getstr \
				"\n  Enter the medium changer device: [$LUNDEV]"
				then
					LUNDEV=$ANS
				fi

				if [ ! -c $LUNDEV ]
				then
				    $ECHO "  $LUNDEV is invalid.  Try again."
				    continue
				fi

				DEVLIST="${DEVLIST};${LUNDEV}"
				break
			    done
			else
			    $ECHO \
			    "\n  The drive will be treated as a single disc player."
			    CHGMETHOD=0
			    NUMDISCS=1
			    MULTIPLAY=False
			    DEVLIST=$XMCD_DEV
			fi
			;;
		3)
			# OS ioctl method
			DEVLIST=$XMCD_DEV
			;;
		*)
			# Unsupported changer method
			DEVLIST=$XMCD_DEV
			;;
		esac
	else
		DEVLIST=$XMCD_DEV
	fi

	DRVNOTICE="\n  NOTE: This drive does not support these features:"

	if [ $PLAYTI = False ]
	then
		$ECHO "$DRVNOTICE"
		DRVNOTICE="\c"
		$ECHO "  - The Previous Index and Next Index buttons."
	fi

	if [ $CADDYLOCK = False ]
	then
		$ECHO "$DRVNOTICE"
		DRVNOTICE="\c"
		$ECHO "  - The caddy lock."
	fi

	if [ $LOAD = False ]
	then
		$ECHO "$DRVNOTICE"
		DRVNOTICE="\c"
		$ECHO "  - Software-controlled caddy load."
	fi

	if [ $PAUSE = False -a $VENDOR = 0 ]
	then
		$ECHO "$DRVNOTICE"
		DRVNOTICE="\c"
		$ECHO "  - Audio pause/resume function."
	fi

	case "$VOLCTL" in
	0)
		$ECHO "$DRVNOTICE"
		DRVNOTICE="\c"
		$ECHO "  - The volume, balance and channel routing controls."
		VOLSUPP=False
		BALSUPP=False
		CHRSUPP=False
		;;
	1)
		$ECHO "$DRVNOTICE"
		DRVNOTICE="\c"
		$ECHO "  - The balance and channel routing controls."
		VOLSUPP=True
		BALSUPP=False
		CHRSUPP=False
		;;
	2)
		$ECHO "$DRVNOTICE"
		DRVNOTICE="\c"
		$ECHO "  - The volume and channel routing controls."
		VOLSUPP=False
		BALSUPP=True
		CHRSUPP=False
		;;
	3)
		$ECHO "$DRVNOTICE"
		DRVNOTICE="\c"
		$ECHO "  - The channel routing control."
		VOLSUPP=True
		BALSUPP=True
		CHRSUPP=False
		;;
	4)
		$ECHO "$DRVNOTICE"
		DRVNOTICE="\c"
		$ECHO "  - The volume and balance controls."
		VOLSUPP=False
		BALSUPP=False
		CHRSUPP=True
		;;
	5)
		$ECHO "$DRVNOTICE"
		DRVNOTICE="\c"
		$ECHO "  - The balance control."
		VOLSUPP=True
		BALSUPP=False
		CHRSUPP=True
		;;
	6)
		$ECHO "$DRVNOTICE"
		DRVNOTICE="\c"
		$ECHO "  - The volume control."
		VOLSUPP=False
		BALSUPP=True
		CHRSUPP=True
		;;
	7)
		VOLSUPP=True
		BALSUPP=True
		CHRSUPP=True
		;;
	*)
		;;
	esac

	$ECHO "\n  Creating the $CFGDIR/$CONFIGFILE file..."
	GDEVLIST=`echo $DEVLIST | sed 's/\//\\\\\//g'`

	sed \
	-e "s/^!.*DO NOT MODIFY.*$/! DEVICE CONFIGURATION FILE/" \
	-e "s/^logicalDriveNumber:.*/logicalDriveNumber:	$DRVNO/" \
	-e "s/^deviceInterfaceMethod:.*/deviceInterfaceMethod:	$METHOD/" \
	-e "s/^mediumChangeMethod:.*/mediumChangeMethod:	$CHGMETHOD/" \
	-e "s/^numDiscs:.*/numDiscs:		$NUMDISCS/" \
	-e "s/^deviceList:.*/deviceList:	$GDEVLIST/" \
	-e "s/^driveVendorCode:.*/driveVendorCode:	$VENDOR/" \
	-e "s/^playAudio12Support:.*/playAudio12Support:	$PLAY12/" \
	-e "s/^playAudioMSFSupport:.*/playAudioMSFSupport:	$PLAYMSF/" \
	-e "s/^playAudio10Support:.*/playAudio10Support:	$PLAY10/" \
	-e "s/^playAudioTISupport:.*/playAudioTISupport:	$PLAYTI/" \
	-e "s/^loadSupport:.*/loadSupport:		$LOAD/" \
	-e "s/^ejectSupport:.*/ejectSupport:		$EJECT/" \
	-e "s/^modeSenseSetDBD:.*/modeSenseSetDBD:	$MODEDBD/" \
	-e "s/^volumeControlSupport:.*/volumeControlSupport:	$VOLSUPP/" \
	-e "s/^balanceControlSupport:.*/balanceControlSupport:	$BALSUPP/" \
	-e "s/^channelRouteSupport:.*/channelRouteSupport:	$CHRSUPP/" \
	-e "s/^volumeControlTaper:.*/volumeControlTaper:	$VOLTAPER/" \
	-e "s/^scsiAudioVolumeBase:.*/scsiAudioVolumeBase:	$VOLBASE/" \
	-e "s/^pauseResumeSupport:.*/pauseResumeSupport:	$PAUSE/" \
	-e "s/^caddyLockSupport:.*/caddyLockSupport:	$CADDYLOCK/" \
	-e "s/^curposFormat:.*/curposFormat:		$CURPOSFMT/" \
	-e "s/^noTURWhenPlaying:.*/noTURWhenPlaying:	$PLAYNOTUR/" \
	-e "s/^spinDownOnLoad:.*/spinDownOnLoad:		$STOPONLOAD/" \
	-e "s/^ejectOnExit:.*/ejectOnExit:		$EJECTONEXIT/" \
	-e "s/^stopOnExit:.*/stopOnExit:		$STOPONEXIT/" \
	-e "s/^exitOnEject:.*/exitOnEject:		$EXITONEJECT/" \
	-e "s/^closeOnEject:.*/closeOnEject:		$CLOSEONEJECT/" \
	-e "s/^multiPlay:.*/multiPlay:		$MULTIPLAY/" \
	   < $CFGDIR/device.cfg > $CFGDIR/$CONFIGFILE
	chmod 644 $CFGDIR/$CONFIGFILE 2>/dev/null
	chown bin $CFGDIR/$CONFIGFILE 2>/dev/null
	chgrp bin $CFGDIR/$CONFIGFILE 2>/dev/null
	(cd $CFGDIR ;\
	 rm -f ${CONFIGFILE}-${OS_NODE} ;\
	 dolink $CONFIGFILE ${CONFIGFILE}-${OS_NODE})

	YNDEF=n
	if getyn "\n  Do you have more CD-ROM or CD-R drives on your system"
	then
		DRVNO=`expr $DRVNO + 1`

		case `expr $DEVNO : '.*'` in
		0)
			;;
		1)
			DEVNO=`expr $DEVNO + 1`
			;;
		2)
			DEVNO=`echo $DEVNO | \
				$AWK '{ printf("%02d\n", $1 + 1) }'`
			;;
		3)
			DEVNO=`echo $DEVNO | \
				$AWK '{ printf("%03d\n", $1 + 1) }'`
			;;
		4)
			DEVNO=`echo $DEVNO | \
				$AWK '{ printf("%04d\n", $1 + 1) }'`
			;;
		*)
			DEVNO=`expr $DEVNO + 1`
			;;
		esac
	else
		break
	fi
done

doexit $EXITSTAT

