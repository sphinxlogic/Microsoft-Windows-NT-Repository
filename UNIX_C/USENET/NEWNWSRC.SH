 8-Aug-85 13:49:10-MDT,1938;000000000001
Return-Path: <unix-sources-request@BRL.ARPA>
Received: from BRL-TGR.ARPA by SIMTEL20.ARPA with TCP; Thu 8 Aug 85 13:48:45-MDT
Received: from usenet by TGR.BRL.ARPA id aa00827; 8 Aug 85 7:48 EDT
From: "Col. G. L. Sicherman" <colonel@ellie.uucp>
Newsgroups: net.sources
Subject: moving a .newsrc
Message-ID: <486@ellie.UUCP>
Date: 7 Aug 85 20:11:42 GMT
To:       unix-sources@BRL-TGR.ARPA

:	newnewsrc - make a new .newsrc conform to an old one.
:	Col. G. L. Sicherman.  7 Aug 1985.
:
:	Have you ever moved from one system to another and found that
:	you must customize a whole new .newsrc, because the old system
:	numbered its news articles differently?
:	This script imposes as much of an old .newsrc as possible on
:	a new .newsrc.  Specifically, it disables groups that were
:	disabled on the old system, and arranges groups into the old
:	order.
:
:	usage:	newnewsrc [-e] [-b] [-o] OLD NEW
:
:	-e	Enable disabled groups on NEW that were enabled on OLD.
:	-b	Place groups not found on OLD at beginning of NEW
:			rather than at end.
:	-o	Do not import "option" lines from OLD.
:
:	New file is written to standard output.
:
bflag='10000'
eflag='~'
oflag='=='
while (true) do
case $1 in
-b*)	bflag='70000'; shift;;
-e*)	eflag='!~'; shift;;
-o*)	oflag='!='; shift;;
*) break;
esac
done
if (test 2 != $#) then
echo 'usage: newnewsrc [-e] [-b] [-o] old new' >&2
exit 1
fi
awk '{print "O",NR+'$bflag',$0}' < $1 > /usr/tmp/$$
awk '{print "N",NR+40000,$0}' < $2 | sort +2 -3 +0 - /usr/tmp/$$ |\
awk '$3 !~ /[:!]$/ {if ($1'$oflag'"O") print; next}\
substr($3,0,length($3)-1)==substr(LSTNM,0,length(LSTNM)-1) \
{if ($3'$eflag'/!$/) LSTNM=$3; print $1, $2, LSTNM, LSTRG; F=0; next}\
0 < F {print LAST; F=0}\
{if ("O"==$1) LSTRG=""; else LSTRG=$4; LSTNM=$3; \
LAST=$1 " " $2 " " LSTNM " " LSTRG; F=1}\
END {if (0<F) print LAST}' | sort -n +1 | sed -e 's/^..[0-9]* *//'
rm /usr/tmp/$$
exit 0
