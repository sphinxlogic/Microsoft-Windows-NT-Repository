#!/bin/sh
#
# @(#)makesrc.sh	6.12 98/04/29
#
# Make software source code release
#
#    xmcd  - Motif(tm) CD Audio Player
#    cda   - Command-line CD Audio Player
#    libdi - CD Audio Player Device Interface Library
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

SRCLIST=misc_d/SRCLIST
TMPDIR=/tmp/_makesrc_d
ZFILE=xmcdsrc.tar.gz
UUEFILE=xmcdsrc.uue
COMPRESS=gzip
UNCOMPRESS=gunzip
ENCODE=uuencode
DECODE=uudecode
TAR=/usr/local/bin/scotar

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

# Select tar program
if [ ! -x $TAR ]
then
	TAR=tar
fi

CURDIR=`pwd`
if [ `basename "$CURDIR"` = misc_d ]
then
	cd ..
elif [ ! -f install.sh ]
then
	$ECHO "You must run the makesrc.sh script while in the xmcd"
	$ECHO "source code distribution top-level directory or in the"
	$ECHO "misc_d subdirectory."
	exit 1
fi

if [ ! -r $SRCLIST ]
then
	$ECHO "Error: Cannot open $SRCLIST"
	exit 2
fi

VERS=`grep "VERSION=" $SRCLIST | sed 's/^.*VERSION=//'`

$ECHO "Creating xmcd/cda version $VERS source code release...\n"

trap "$ECHO 'Interrupted!'; rm -rf $TMPDIR $ZFILE $UUEFILE; exit 1" 1 2 3 5 15

# Make temp directory and copy release files to it
rm -rf $TMPDIR
mkdir -p $TMPDIR/xmcd-$VERS
for i in `awk '!/^#/ { print $1 }' $SRCLIST`
do
	$ECHO "\t$i"
	DEST=`dirname $TMPDIR/xmcd-$VERS/$i`
	mkdir -p $DEST >/dev/null 2>&1
	cp $i $DEST
	if [ $? -ne 0 ]
	then
		$ECHO "Error: cannot copy $i"
		rm -rf $TMPDIR
		exit 1
	fi
done

# Make a link to the README file
(cd $TMPDIR/xmcd-$VERS; ln docs_d/README README)

$ECHO "\nFixing permissions..."
(cd $TMPDIR; find xmcd-$VERS -type f -print | xargs chmod 444)

$ECHO "\nCreating \"$COMPRESS\"ed tar archive..."
# Create tar archive and compress it
(cd $TMPDIR; $TAR cf - xmcd-$VERS) | $COMPRESS >$ZFILE

$ECHO "\n\"$ENCODE\"ing..."
$ECHO '

Instructions to unpack xmcd v_VERS_ source code release
----------------------------------------------------

The following is a "_COMPRESS_"ed and "_ENCODE_"ed tar archive
containing the xmcd, cda and libdi source code files.  To extract,
make a suitable source code directory for xmcd and save this
message in a file "_UUEFILE_" in that directory.  Then, change
to the directory and do the following:

	_DECODE_ _UUEFILE_
	_UNCOMPRESS_ < _ZFILE_ | tar xvf -

Read the README file for further instructions.

' | sed	-e "s/_VERS_/$VERS/g" \
	-e "s/_UUEFILE_/$UUEFILE/g" \
	-e "s/_ZFILE_/$ZFILE/g" \
	-e "s/_COMPRESS_/$COMPRESS/g" \
	-e "s/_UNCOMPRESS_/$UNCOMPRESS/g" \
	-e "s/_ENCODE_/$ENCODE/g" \
	-e "s/_DECODE_/$DECODE/g" >$UUEFILE

# Uuencode
$ENCODE $ZFILE $ZFILE >>$UUEFILE
$ECHO "\n\n\n" >>$UUEFILE

$ECHO ""

ls -l $ZFILE $UUEFILE

rm -rf $TMPDIR
exit 0

