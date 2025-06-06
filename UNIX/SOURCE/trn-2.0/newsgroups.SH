case $CONFIG in
    '') . ./config.sh ;;
esac
echo "Extracting newsgroups (with variable substitutions)"
$spitshell >newsgroups <<!GROK!THIS!
$startsh
# $Id: newsgroups.SH,v 1.2 92/01/13 14:48:55 usenet Exp $
# 
# $Log:	newsgroups.SH,v $
# Revision 1.2  92/01/13  14:48:55  usenet
# Make all three respect $PAGER, and default to more if it isn't set.
# 
# Revision 1.1  92/01/11  15:51:36  usenet
# USENET News Software
# 
# Revision 4.4.1.1  1991/09/25  19:38:08  sob
# various fixed
#
# Revision 4.4  1991/09/09  20:23:31  sob
# release 4.4
#
# 
# This software is Copyright 1991 by Stan Barber. 
#
# Permission is hereby granted to copy, reproduce, redistribute or otherwise
# use this software as long as: there is no monetary profit gained
# specifically from the use or reproduction of this software, it is not
# sold, rented, traded or otherwise marketed, and this copyright notice is
# included prominently in any copy made. 
#
# The author make no claims as to the fitness or correctness of this software
# for any use whatsoever, and it is provided as is. Any use of this software
# is at the user's own risk. 

export PATH || (echo "OOPS, this isn't sh.  Desperation time.  I will feed myself to sh."; sh \$0; kill \$\$)

: syntax: newsgroups [pattern] [pipeflag]

: System Dependencies

: You might want to change pager to a "make column" program if you have one.
: On the other hand, if your kernel does paging, cat would probably do.
pager="${PAGER-more}"
#NORMALactive="${active-/usr/lib/news/active}"

#NORMALcase \$active in
#NORMAL~*) active=\`$filexp \$active\` ;;
#NORMALesac

#NNTPactive="/tmp/active.\$\$"

#NNTPcase \$rnlib in
#NNTP~*) rnlib=\`$filexp \$rnlib\` ;;
#NNTPesac

: End of system dependencies, hopefully

#NNTPif $rnlib/getactive ACTIVE \$active; then
#NNTP    true;
#NNTPelse
#NNTP    exit 1;
#NNTPfi

if $test \$# -ge 2 ; then
    pager=$cat
else
    $echo "Completely unsubscribed newsgroups:"
fi

dotdir=\${DOTDIR-\${HOME-\$LOGDIR}}
newsrc=\${NEWSRC-\$dotdir/.newsrc}

: Throwing .newsrc into the pot twice is a lovely hack to prevent
: bogus newsgroups from showing up as unsubscribed.

$cat \$newsrc \$newsrc \$active | \\
$sed -n	-e '/^options/d' \\
	-e '/^[	 ]/d' \\
	-e '/^control/d' \\
	-e '/^to\./d' \\
 	-e '/ x\$/d' \\
	-e 's/^\([^ !:]*\)[ !:].*\$/\1/' \\
	-e "/.*\$1/p" | \\
$sort | $uniq -u | \$pager
#NNTP$rm -f \$active
if $test \$# -ge 2 ; then
    exit
fi
$echo $n "[Type return to continue] $c"
read tmp
$echo ""
$echo "Unsubscribed but mentioned in \$newsrc:"
$sed -n < \$newsrc \\
	-e "/\$1.*!/"'s/^\([^!]*\)!.*\$/\1/p' | \\
$sort | \$pager
!GROK!THIS!
case "$isrrn" in
define)  sed < newsgroups -e '/^#NNTP/s/^#NNTP//' -e '/^#NORMAL/d' > newsgroups.new ;;
*) sed < newsgroups -e '/^#NNTP/d' -e '/^#NORMAL/s/^#NORMAL//' > newsgroups.new ;;
esac
mv newsgroups.new newsgroups
$eunicefix newsgroups
chmod 755 newsgroups
