24-Oct-85 14:27:30-MDT,5112;000000000001
Return-Path: <unix-sources-request@BRL.ARPA>
Received: from BRL-TGR.ARPA by SIMTEL20.ARPA with TCP; Thu 24 Oct 85 14:27:14-MDT
Received: from usenet by TGR.BRL.ARPA id a009999; 24 Oct 85 14:41 EDT
From: Jim Scardelis <frodo@wcom.uucp>
Newsgroups: net.sources
Subject: revised mvjunk script
Message-ID: <837@wcom.UUCP>
Date: 23 Oct 85 03:24:38 GMT
To:       unix-sources@BRL-TGR.ARPA


	A couple of days ago, right after rmgroup'ing net.bizarre, and
net.misc.coke, a script that I had taken off of the net awhile ago went
and recreated them NET-WIDE. The script is mvjunk, which takes articles
that end up in the junk newsgroup, and puts them back into the groups 
they came from. I use it because wcom seems to get alot of articles that
it thinks are too old for it, or stuff posted to new newsgroups that we
haven't gotten newgroup control messages for yet, or groups that are
in regional distributions, but are so rarely posted to that we don't
know about them. Unfortunately, the script as originally distributed 
just did an inews -C, which results in the group being created net-wide.
I have modified it so that it only does it locally.

Thanks go to my fellow news administrators that pointed out the bug to me,
and sincere apologies to anyone whom the extraneous control messages have
inconvenienced.

The modified version of mvjunk follows:
---CUT HERE---CUT HERE---CUT HERE----------------------------------------

#! /bin/sh
# @(#)mvjunk.sh   (TRW Advanced Technology Facility) STA 2-Jun-1985
#
# Modified 10/22/85-James M. Scardelis, SA, Warner Computer Systems
#       to create newsgroups locally only.
#
# Move news files out of junk and into their proper directories,
# updating the active list and localgroups as we go.  Unknown newsgroups
# are created LOCALLY and the article installed.
#
# Be CAREFULL with that active list!  Although we try to write out
# buffers as quickly as possible, conflicts can occur if someone else
# tries to use it at the same time this script is running!
#
# Strange occurences are dribbled into mvjunk-log.
#
# TRW DSG EDS SISD SDL ATF STA

JUNK=/usr/spool/news/junk
NEWSDIR=/usr/spool/news
LIB=/usr/lib/news		: See line 88

chmod u+wr $LIB/active

cd $JUNK

if /bin/test -z "`ls`" ; then exit
fi
chmod u+wr *

for j in $JUNK/*
do
	for group in `egrep '^Newsgroups:' $j | head -1 | \
		sed 's;Newsgroups:[ 	];;' | \
		sed 's;,; ;g'`
	do
		l=`egrep '^Control:' $j`
		if /bin/test -n "$l" ; then
			GROUPDIR="control"
			group="control"
		else
			GROUPDIR=`echo $group | sed 's;\.;\/;g'`
		fi
# Sick Lex... really sick!
		cd $NEWSDIR
		if /bin/test -d $GROUPDIR ; then
			cd $GROUPDIR
			l=`ls`
			if /bin/test -n "$l" ; then
				a=`ls | sort -r -n`
				for f in $a
				do
					if /bin/test -f "$f" ; then
						article=`echo $f "+1" | bc`
						break
					fi
				done
			else
				article=1
			fi
		else
			mkdir $NEWSDIR/$GROUPDIR
			article=1
		fi
		cd $NEWSDIR/$GROUPDIR
		if /bin/test ! -d "$article" ; then

			cp $j $NEWSDIR/$GROUPDIR/$article

			chmod ugo+wrx $NEWSDIR/$GROUPDIR/$article
			chown news $NEWSDIR/$GROUPDIR/$article
			chgrp news $NEWSDIR/$GROUPDIR/$article
			entry=`egrep "^$group " $LIB/active`
			if /bin/test -z "$entry" ; then
				echo "Distribution: local \nControl: newgroup $group" | \
				/usr/lib/news/inews -h -n control -t cmsg
				the_date=`date`
				echo "Created group $group at $date" >> $LIB/mvjunk-log
				entry="$group 00001 00001 y" >> $LIB/localgroups
			fi
			entry="^$entry "
			the_date=`date`
#
# There should be a way of passing shell variables into awk, but since
# we don't know how to do that we just hardwire it.  Kludgy... no?
#
			echo $article $entry $the_date | \
				awk 'BEGIN { FS = " " } \
			{ if ( NF != 11 ) printf "Munged active entry: %s\n",$0 >"/usr/lib/news/mvjunk-log" ; \
			if ( length($1) > 5 ) printf "Field too large: %s\n",$0 >"/usr/lib/news/mvjunk-log" ; \
			if ( length($3) > 5 ) printf "Field too large: %s\n",$0 >"/usr/lib/news/mvjunk-log" ; \
			if ( length($4) > 5 ) printf "Field too large: %s\n",$0 >"/usr/lib/news/mvjunk-log" ; \
			printf "/%s/s;%s;",$2,$3 ; \
			for ( i = 5 - length($1) ; i > 0 ; i-- ) printf "0" ; \
			print $1 ";" ; \
			if ( length($4) < 5 ) { \
				printf "s;%s;",$4 ; \
				for ( i = 5 - length($1) ; i > 0 ; i-- ) printf "0" ; \
				print $4 ";" \
				} \
			if ( $5 != "y" && $5 != "n" ) print "s;.$;y;" } \

					END { printf "w\nq\n" }' | ed - $LIB/active
		else
			echo "Hey, there's a directory called $NEWSDIR/$GROUPDIR/$article!"
			echo "Hey, there's a directory called $NEWSDIR/$GROUPDIR/$article!" >> $LIB/mvjunk-log
		fi
	done
	if /bin/test -f $NEWSDIR/$GROUPDIR/$article ; then
		/bin/rm -f $j		# But my MOTHER lives in Hackensack!
	else
		echo "Couldn't find the article I just installed!"
		echo "Couldn't find the article I just installed!" >> $LIB/mvjunk-log
	fi
done
-- 

UUCP: ...{decvax,ihnp4,allegra}!seismo!trwatf!root	- Lord Frith
ARPA: trwatf!root@SEISMO

"Money from da Buddah"
