#!/bin/sh
######################################################################
# chkimports.sh 1.4                                                  #
# ------------------------------------------------------------------ #
# Checking for incorrect differences of imported symbols between     #
# the archive and shared version of a library                        #
# Copyright (c) 1992 by Thomas Wolfram, Berlin, Germany              #
# (thomas@aeon.in-berlin.de, wolf@prz.tu-berlin.de                   #
######################################################################

if [ "$1" = "" -o "`echo $1 | cut -f2 -d_`" != "s.a" ] ; then
        echo $0: no host shared library name specified. >&2
        exit 1
fi
if [ ! -f $1 ] ; then
        echo $0: named host shared library doesn\'t exist. >&2
        exit 1
fi

version="1.4"
LIB=`basename $1 | cut -f1 -d_` ; export LIB
TMPDIR=${TMPDIR:=/usr/tmp} ; export TMPDIR

if [ ! -f ${LIB}/exports.out ] ; then
        echo $0: ${LIB}/exports.out doesn\'t exist. Run mkshtmpl.sh first! >&2
        exit 1
fi
if [ ! -f ${LIB}/imports.out ] ; then
        echo $0: ${LIB}/imports.out doesn\'t exist. Run mkshtmpl.sh first! >&2
        exit 1
fi

echo Checking for incorrect differences of imported symbols between the
echo archive and shared version of ${LIB}...

echo -n "Collecting all imported symbols of archive library... "

# creating modified file with imported symbols
nawk '{ print "IMPORTED 0 " $2 }' LIB=$LIB \
 ${LIB}/imports.out >${TMPDIR}/${LIB}.imp2

# merge and sort by name and type
sort +2 +1 ${LIB}/exports.out ${TMPDIR}/${LIB}.imp2 >${TMPDIR}/${LIB}.oi

# create file of imported symbols with object names of all library members
# for archive version

nawk '{ if ($3 == IMP)  print ; \
	else if ($1 == "IMPORTED") IMP=$3 ; \
      }' \
 ${TMPDIR}/${LIB}.oi >${TMPDIR}/${LIB}.aimp

# sort by object and function
sort +0 +2 -o${TMPDIR}/${LIB}.aimp ${TMPDIR}/${LIB}.aimp
echo Done.


echo -n "Collecting all imported symbols of shared library... "

# create file of imported symbols with object names of all library members
# for shared version

nm -ep $1 | \
nawk "\$1 ~ /lib.*\.a/ { OBJ=substr(\$1, index(\$1,\"[\")+1, \
index(\$1,\"]\")-index(\$1,\"[\")-1)  } ; \
\$3 ~ /^_${LIB}_.*/ { print OBJ, \"0\", substr(\$3, length(\"_${LIB}_\")+1) }"\
  >${TMPDIR}/${LIB}.simp

# sort by object and function
sort +0 +2 -o${TMPDIR}/${LIB}.simp ${TMPDIR}/${LIB}.simp
echo Done.

echo "Comparing... "

# merge and sort, then remove adjacent lines
sort +2 +0 +1 ${TMPDIR}/${LIB}.aimp ${TMPDIR}/${LIB}.simp | \
nawk '{ print $2, $1, $3 }' | \
uniq -c -1 | \
nawk '$1 == 1 { if ($2 == "0") \
                   print "WARNING: shared \"" $3 "\" imports \"" $4 \
                         "\" not used in archive version" ; \
                else \
                   print "ERROR: shared \"" $3 "\" DOES NOT import \"" $4 \
                         "\" used in archive version" ; \
}' >${LIB}/check.out
cat ${LIB}/check.out
echo "
These messages are recorded in ${LIB}/check.out. Check the sources and recompile
if necessary!"
echo READY.

