#!/bin/sh
# @(#)get_version.sh	1.1 (13 Oct 1993)
#
# Derive a version name from a file containing lines like the following:
#	#define VERSION "1.2BETA"
#	#define PATCHLEVEL 5
# The resulting name for this example woudl be 1.2BETAp5.  The p<patchlevel> is
# omitted if the patchlevel is 0 or 1.
sed -n -e '
:L
N
s/\n/ /' -e 's/\([ 	]PATCHLEVEL.*\)\([ 	]VERSION.*\)/\2 \1/
/[ 	]VERSION.*[ 	]PATCHLEVEL/!b L
s/.*[ 	]VERSION[^0-9;]*\([0-9][0-9A-Z_a-z.]*\).*[ 	]PATCHLEVEL[ 	]*\([0-9]*\).*/\1p\2/
s/p[01]$//
p
q' < ${1:-patchlevel.h}
