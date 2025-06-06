24-Dec-85 06:28:33-MST,4388;000000000001
Return-Path: <unix-sources-request@BRL.ARPA>
Received: from BRL-TGR.ARPA by SIMTEL20.ARPA with TCP; Tue 24 Dec 85 06:28:24-MST
Received: from usenet by TGR.BRL.ARPA id a022731; 24 Dec 85 7:45 EST
From: sources-request@panda.uucp
Newsgroups: mod.sources
Subject: agelog - trim log files
Message-ID: <1222@panda.UUCP>
Date: 23 Dec 85 16:39:28 GMT
Sender: jpn@panda.uucp
Approved: jpn@panda.UUCP
To:       unix-sources@BRL-TGR.ARPA

Mod.sources:  Volume 3, Issue 66
Submitted by: ucbvax!hpda!hpdsa!davel (Dave Lennert)


The agelog script performs a simple, yet critical function in
system maintenance.  Many log files in unix will grow without bound.
A common practice is to periodically clear a log file's contents 
when it has grown large enough to gobble too much disk space.  This
unfortunately loses all your recent log history.  Agelog strikes
a happy medium by automatically (via cron(1)) trimming log files
while retaining recent information.  An explanation of its usage is
given in the comments at the front of the script along with examples
for a couple common unix log files.  Note that by controlling how many
history files are maintained and how often the trimming is done one 
has control over how much information is lost during each agelog.

This is an enhanced version of the `syslogswap' script shown in Wizard's 
Grabbag in the January 1986 Unix/World; it supports an optional
directory where the archived log files are stashed.

The original script is by Dave Lennert.  The enhancements are by
Bob Designer.  Both of Hewlett-Packard.

    Dave Lennert		ihnp4!hplabs!hpda!davel         [UUCP]
    Bob Designer		ihnp4!hplabs!hpda!bd            [UUCP]

    Hewlett-Packard - 47UX
    19447 Pruneridge Ave.
    Cupertino, CA  95014

---------cut here--------------------------------------
#! /bin/sh
: agelog -- age log files, by Dave Lennert as told to Bob Desinger
#
# Usage:  $0 logFile howMany [stashDir]
# The most recent <howMany> logs are kept around by renaming logFile to
# logFile.0, after similarly rolling logFile.0 => logFile.1 => logFile.2 => ...
# If <stashDir> is named, the old logs will be kept in that directory.  If not
# given, the old logs are left in the same directory as the original logFile.
#
# Example:
# `agelog /usr/adm/sulog 2' will, if run weekly, keep 3 files around:
#	/usr/adm/sulog    containing this week's log info
#	/usr/adm/sulog.0  containing last week's log info
#	/usr/adm/sulog.1  containing the week before last's log info
#
# A typical crontab entry:
# #	Keep the most recent 2 weeks worth of uucp logs around in
# #	/tmp/Oldlogs/*, one per day, so that old LOGFILEs will be in
# #	/tmp/Oldlogs/LOGFILE.{0,1,2,...}
# 00 1 * * * /usr/local/agelog /usr/spool/uucp/LOGFILE 14 /tmp/Oldlogs


# Initialize:

PATH=/usr/ucb:/usr/bin:/bin:/etc:/usr/lib  # BSD systems have /usr/ucb
export PATH

# traps:  0=exit  1=hangup  2=interrupt  3=quit  15=terminate
trap 'echo 1>&2 "$0: Ow!"; exit 15' 1 2 3 15


# Digest arguments:

if [ $# -lt 2 -o $# -gt 3 ]
then	# we can use only 2 args or 3 args
	echo 1>&2 "Usage:  $0 logFileName howMany [stashDir]"
	exit 1
else
	log="$1"	# logFileName
	max="$2"	# howMany
	if [ -z "$3" ]
	then	# no directory to stash them in; use log's directory
		head=`expr $log : '\(.*/\).*'`	# /a/b/x => /a/b/
	else	# user specified a directory
		if [ ! -d "$3" ]
		then
			echo 1>&2 "$0: $3 is not a directory"
			exit 2
		else
			head="$3/"
		fi
	fi
fi


# Rename log.$max-1 => ... => log.3 => log.2 => log.1

arch="${head}`basename $log`"	# name of archive files, sans {.0, .1, ...}

older=`expr $max - 1`	# ensure we had a number in $2
if [ $? -eq 2 ]
then	# not a number, or some problem
	echo 1>&2 "$0: cannot decrement $max"
	exit 2
fi

while [ $older -gt 0 ]
do	# age the logfiles in the stashdir
	old=`expr $older - 1`
	if [ -f $arch.$old ]
	then
		mv $arch.$old $arch.$older
	fi
	older=`expr $older - 1`
done


# Old logfiles are all rolled over; now move the current log to log.0

# Use cp instead of mv to retain owner & permissions for the original file,
# and to avoid prematurely aging any info going into the file right now.
if [ -f $log ]
then
	# don't create an old log if $2 was 0
	test $max -gt 0 && cp $log $arch.0
	cp /dev/null $log	# clear out log
fi

exit 0
