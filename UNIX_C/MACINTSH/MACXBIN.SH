#! /bin/sh
# Shellscript for xbin'ing new net.sources.mac posts to one directory for macput
# 	Gets files to xbin from comm'ing the file '.lastls' in the download 
#	directory and the new net.sources.mac directory.
# 
# This shellscript will ignore directories and files that are unreadable,
# 	as well as any files without the line "(This file must be converted...)"
#
# Upon termination of the operation, the Macintosh bell will ring
#	two times.  This is your clue to wake up and see what is waiting.
# 	This might be deleted if you plan to do this in the background and 
#	don't want to be bothered.
#
# Done on 1 June 1985 by 
# Chris Borton, UC San Diego Undergraduate CS
#{ucbvax,decvax,akgua,dcdwest}!sdcsvax!sdcc6!ix924	
#
# User modifications necessary:
#
#  DLDIR:   replace "Download directory" with the full path to the 
#	    directory you wish to put the files to download.
#  SOURCES: replace "/usr/spool/news/net/sources/mac" with the path to your 
#  	    net.sources.mac if it is different.

DLDIR="Download directory"
SOURCES="/usr/spool/news/net/sources/mac"

ls $SOURCES > $DLDIR/.thisls
mesg n
for f in `comm -23 $DLDIR/.thisls $DLDIR/.lastls`
do
	if [ -f $SOURCES/$f ] && [ -r $SOURCES/$f ]
	then
		fgrep "(This file must be " $SOURCES/$f > /tmp/BinHextest
		if [ -s /tmp/BinHextest ]
		then
			rm /tmp/BinHextest
			cp $SOURCES/$f $DLDIR
			cd $DLDIR
			xbin $f
			rm $f
			cd $SOURCES
		else
			rm /tmp/BinHextest
		fi
	fi
 done
 mv $DLDIR/.thisls $DLDIR/.lastls
 rm $DLDIR/.thisls
 echo "Downloads ready!"
 echo -n 
 echo -n 
 mesg y
