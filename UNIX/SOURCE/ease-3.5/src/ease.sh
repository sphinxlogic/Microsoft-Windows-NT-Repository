#! /bin/sh
#
#  ease - front end for the ease translator (et).
#
#  This sh script simplifies the task of calling "et" with the proper flags.
#
#  It also defines a VERSION macro, known to cpp when preprocessing the
#  input to et.  This allows you to automatically embed the RCS version
#  number of your config file into the generated sendmail.cf. 
#
#  For example, if your ease input file contains the RCS version string
#	$Revision: 1.7 $
#  and the lines
#	define ("Received:",
#	    "by ${m_oname} (${VERSION})"
#	    ifset (m_shostname, " from ${m_shostname} ") "for ${m_ruser}"
#  then your sendmail.cf will define the "Received:" header line so that
#  it contains the RCS version number of the ease input file.  This version
#  number will ultimately be stamped into the header of every message which
#  flows though this sendmail, thus allowing you to see at a glance whether
#  some problem was due to an out-of-date sendmail.cf.
#
#  At our site, the "Received:" header lines thus have the following form:
#
#	Received: by isis.tc.fluke.COM (version 2.46)
#	    from argv.tc.fluke.COM for jeff
#	    id AA10285; Wed, 21 Feb 90 17:28:43 PST
#	Received: by argv.tc.fluke.COM (version 2.46)
#	    for jeff@isis
#	    id AA06739; Wed, 21 Feb 90 17:28:39 PST
#
#  This makes it a little easier to track down problems in networks comprising
#  dozens or hundreds of machines.
#
# $Source: /home/kreskin/u0/barnett/Src/ease/src/RCS/ease.sh,v $
# $Locker:  $
#
# $Revision: 1.7 $
# Check-in $Date: 1991/05/16 10:45:25 $
# $State: Exp $
#
# $Author: barnett $
#
# $Log: ease.sh,v $
# Revision 1.7  1991/05/16  10:45:25  barnett
# Better support for System V machines
# Support for machines with read only text segments
#
# Revision 1.6  1990/05/07  11:15:04  jeff
# Add support for the "-q" flag added to ease.
#
# Version 1.5  90/02/22  15:51:12  jeff
# Improved the comments in preparation for netwide release.
# 
# Version 1.4  88/11/18  11:24:12  jeff
# RCS mangled the previous change; try it again.  (It saw something that
# looked like a keyword to it, so it expanded the token.  Yuck.)
# 
# Version 1.3  88/11/18  11:20:52  jeff
# Change the VERSION macro from the date to the RCS revision of the
# config.ease file.
# 
# Version 1.2  87/04/13  16:56:29  jeff
# Change argument parsing to accomodate the new -C flag.
# 
# Version 1.1  87/04/08  12:20:58  jeff
# Initial version
# 
#
# @(#)FLUKE source file: $Header: /home/kreskin/u0/barnett/Src/ease/src/RCS/ease.sh,v 1.7 1991/05/16 10:45:25 barnett Exp $

PATH=.:/bin:/usr/bin:/usr/ucb:/usr/local/bin
export PATH

echo    "#"
echo    "#   Compiled via: $0 $*"
echo	"# From directory: `pwd`"
echo    "#           Date: `date`"
echo    "#"
echo    "# This file was produced by the \"ease\" translator."
echo    "# You probably shouldn't edit it, since changes will be lost"
echo	"# the next time that ease is run.  Instead, edit the source file"
echo	"# located in the directory named above."
echo    "#"

cppflags=
etflags='-q'
CPP='cc -E'


for i in ${1+"$@"} ;do
    case "$1" in
	'')	break;;
	-C)	etflags="${etflags-} $1";;
	-D*)	cppflags="$cppflags $1";;
	*)	file="$1";;
    esac
    shift
done

#
#  Extract the RCS "Revision" string from the ease input file, and
#  use it to define the VERSION symbol to cpp.
#
#  If you maintain your ease input file with SCCS, the appropriate change
#  should be simple.
#
Rev=`fgrep 'Revision:' $file | sed -e 's/^.*Revision:[	 ]*\([^	 ]*\).*/\1/'`

# The sed commands delete empty comment lines and those preprocessor output
# lines which indicate the linenumber and filename.
$CPP -DVERSION=\"version\ $Rev\" $cppflags $file |
    et $etflags |
    sed -e '/^# *$/d' \
	-e '/^#[ 	]*[0123456789][0123456789]*[ 	]*".*"[	 ]*$/d'
