#!/bin/sh
#
# @(#)dp2xmcd.sh	6.1 98/07/17
#
# Script to convert a DiscPlay4 CD database file to xmcd CDDB format.
# DiscPlay is a CD player application for the Windows 95/NT platform
# from Matt Jensen (Obvion Systems).
#
# This script currently does not handle extended disc and track information
# fields.
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

TMPFILE=/tmp/conv.$$
INFILE=$1
OUTFILE=$2

if [ $# -ne 2 ]
then
	echo "Usage: $0 infile outfile"
	exit 1
fi
if [ $INFILE = $OUTFILE ]
then
	echo "Input file and output file are the same."
	exit 2
fi
if [ ! -r $INFILE ]
then
	echo "Cannot open $INFILE"
	exit 3
fi

# Filter out carriage returns
sed -e 's///g' $INFILE >$TMPFILE

# Header
echo "# xmcd CD database file" >$OUTFILE
echo "# Copyright (C) 1993-1998  Ti Kan" >>$OUTFILE
echo "#" >>$OUTFILE
echo "# Track frame offsets:" >>$OUTFILE

# Track frame offsets
grep "^  TO\[[0-9]*\]=[0-9]*$" $TMPFILE | \
	sed -e 's/^  TO\[[0-9]*\]=/#	/' >>$OUTFILE

echo "#" >>$OUTFILE

# Disc length
n=`grep "^  LN=[0-9]*$" $TMPFILE | sed -e 's/^  LN=//'`
echo "# Disc length: $n seconds" >>$OUTFILE

echo "#" >>$OUTFILE

# Revision
n=`grep "^  RV=[0-9]*$" $TMPFILE | sed -e 's/^  RV=//'`
echo "# Revision: $n" >>$OUTFILE >>$OUTFILE
echo "# Submitted via: xmcd 2.3 PL0" >>$OUTFILE

echo "#" >>$OUTFILE

# Disc ID
n=`grep "^  ID=[0-9A-Z]*$" $TMPFILE | sed -e 's/^  ID=//'`
n=`expr substr "$n" 1 8 | tr '[A-Z]' '[a-z]'`
echo "DISCID=$n" >>$OUTFILE

# Disc title
n=`grep "^  AR=.*$" $TMPFILE | sed -e 's/^  AR=//'`
m=`grep "^  TL=.*$" $TMPFILE | sed -e 's/^  TL=//'`
echo "DTITLE=$n / $m" >>$OUTFILE

ntrks=`grep "^  TT\[[0-9]*\]=.*$" $TMPFILE | wc -l`

# Track titles
grep "^  TT\[[0-9]*\]=.*$" $TMPFILE | \
	sed -e 's/^  TT\[[0-9]*\]=//' | \
	awk 'BEGIN { i = 0 } { printf "TTITLE%d=%s\n", i++, $0 }' >>$OUTFILE

# Extended disc info
echo "EXTD=" >>$OUTFILE

# Extended track info
i=0
while [ $i -lt $ntrks ]
do
	echo "EXTT${i}="
	i=`expr $i + 1`
done >>$OUTFILE

rm -f $TMPFILE
exit 0

