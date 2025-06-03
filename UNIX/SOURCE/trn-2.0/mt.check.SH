case $CONFIG in
    '') . ./config.sh ;;
esac
echo "Extracting mt.check (with variable substitutions)"
version=`cat patchlevel`
$spitshell >mt.check <<!GROK!THIS!
$startsh
# $Id: mt.check.SH,v 1.2 92/01/11 16:05:20 usenet Exp $
#
# $Log:	mt.check.SH,v $
# Revision 1.2  92/01/11  16:05:20  usenet
# header twiddling, bug fixes
# 
# Revision 4.4.3.1  1991/11/22  04:11:10  davison
# Trn Release 2.0
# 
# mt.check - daily maintenance for mt.log
# 
# Check mt.log for earth-shattering errors, and mail them to \$gurus if found.
# Then move the mt.log file into a week-long history chain.
#
# Usage: mt.check
#

gurus="$newsadmin"
tmp="/tmp/mt.c\$\$"

PATH=/bin:/usr/bin
export PATH

umask 002

trap "rm -f \$tmp ; exit 0" 0 1 2 15

cd $mtlib

$egrep " \\*\\*\$" mt.log >\$tmp

if test -s \$tmp ; then
	(cat <<EOT
To: \$gurus
Subject: mthreads error!

The following errors were reported in mt.log.  If the error pertains to
the active file,  you should lock down your news system and fix it.  If
it is a thread-releated bug and it persists, please report this fact to
Wayne Davison (davison@borland.com).  Mention version "$version".

EOT
	cat \$tmp) | mail \$gurus
fi

test -f mt.log.6 && mv mt.log.6 mt.log.7
test -f mt.log.5 && mv mt.log.5 mt.log.6
test -f mt.log.4 && mv mt.log.4 mt.log.5
test -f mt.log.3 && mv mt.log.3 mt.log.4
test -f mt.log.2 && mv mt.log.2 mt.log.3
test -f mt.log   && mv mt.log   mt.log.2
touch mt.log

exit 0
!GROK!THIS!
