23-Nov-85 06:12:36-MST,1960;000000000001
Return-Path: <unix-sources-request@BRL.ARPA>
Received: from BRL-TGR.ARPA by SIMTEL20.ARPA with TCP; Sat 23 Nov 85 06:12:32-MST
Received: from usenet by TGR.BRL.ARPA id aa03850; 23 Nov 85 7:40 EST
From: Charlie Perkins <charliep@polaris.uucp>
Newsgroups: net.sources
Subject: Archiving /usr/spool/oldnews
Message-ID: <299@polaris.UUCP>
Date: 22 Nov 85 01:49:49 GMT
To:       unix-sources@BRL-TGR.ARPA

=========
Arch_oldnews creates "nice" (but BIG) output
suitable for printing out on a line printer.
I use it to keep hardcopy listings of the
news articles when I expire them, so that
conceivably people could search through the
listings to find interesting articles and
retrieve them from the tapes.  Anyway, it is
useful.  This program grew to about 5 times
its original size and could very reasonably
become a C program now.

=================================================
arch_oldnews
=================================================
/bin/rm -f /tmp/arch_oldnews
cat <<'arch_oldnews' > /tmp/arch_oldnews
newsgroup=
export newsgroup
if test -d $1
then
	newsgroup=`echo $1 | sed "s;/usr/spool/oldnews/*;;"`
	(echo  ; echo $newsgroup ; echo " "; echo " ") > /tmp/directoryxyzq
elif test -f $1
then
	subject=`grep '^Subject:' $1 | sed 's/Subject: //
					1q'`
	case "$subject" in
	"")	exit
	esac
	messageID=`grep '^Message-ID:' $1 | sed 's/^Message-ID: <\(.*\)>/\1/`
	msgsize=`expr "$messageID" : '.*'`
	if test "$msgsize" -ge 16
	then
		tabs="	"
	else
		tabs="		"
	fi
	if test -s /tmp/directoryxyzq
	then
		cat /tmp/directoryxyzq
		cat /dev/null > /tmp/directoryxyzq
	fi
	echo `basename $1`":	$messageID$tabs$subject"
fi
arch_oldnews
chmod 755 /tmp/arch_oldnews
cd /usr/spool/oldnews
find fa net mod na nj -exec /tmp/arch_oldnews {} \;
rm /tmp/arch_oldnews
-- 

Charlie Perkins, IBM T.J. Watson Research	philabs!polaris!charliep,
		perk%YKTVMX.BITNET@berkeley,  perk.yktvmx.ibm@csnet-relay
