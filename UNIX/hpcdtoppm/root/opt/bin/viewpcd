#!/bin/sh
# This is a simple Photo-CD viewer. 
# Modified by Greg Lehey, LEMIS, 15 May 1993
#
#
if [ "$1" != "" ] ; then	# explicit image names
  images=$*			# take them
else
  images="img* *.pcd"		# take all files in this directory which start with img or end with .pcd
fi
for i in $images ; do
  if [ -f $i ] ; then		# really there
    echo viewing $i
    hpcdtoppm -2 -a $i | xv -quick24 -
  else				# doesn't exist
    echo $i does not exist
  fi
done
