#! /bin/sh
# mkinstalldirs --- make directory hierarchy
# Author: Noah Friedman <friedman@prep.ai.mit.edu>
# Created: 1993-05-16
# Last modified: 1994-03-25
# Public domain
#
# $XFree86: xc/programs/xterm/mkdirs.sh,v 3.5 1999/09/27 14:59:29 dawes Exp $

errstatus=0
umask 022

for file in ${1+"$@"} ; do
   set fnord `echo ":$file" | sed -ne 's/^:\//#/;s/^://;s/\// /g;s/^#/\//;p'`
   shift

   pathcomp=
   for d in ${1+"$@"} ; do
     pathcomp="$pathcomp$d"
     case "$pathcomp" in
       -* ) pathcomp=./$pathcomp ;;
     esac

     if test ! -d "$pathcomp"; then
        echo "mkdir $pathcomp" 1>&2
        mkdir "$pathcomp" || errstatus=$?
     fi

     pathcomp="$pathcomp/"
   done
done

exit $errstatus

# mkinstalldirs ends here
