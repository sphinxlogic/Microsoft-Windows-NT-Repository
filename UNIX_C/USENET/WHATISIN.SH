14-Dec-85 12:07:18-MST,2564;000000000001
Return-Path: <unix-sources-request@BRL.ARPA>
Received: from BRL-TGR.ARPA by SIMTEL20.ARPA with TCP; Sat 14 Dec 85 12:07:14-MST
Received: from usenet by TGR.BRL.ARPA id a001691; 14 Dec 85 13:23 EST
From: Greg Earle <earle@smeagol.uucp>
Newsgroups: net.sources
Subject: whatisin - Script for quickly identifying contents of news articles
Message-ID: <518@smeagol.UUCP>
Date: 12 Dec 85 03:28:29 GMT
To:       unix-sources@BRL-TGR.ARPA

Here's a handy little script I use to at-a-glance see what is in
the news articles that are currently available in a newsgroup or
groups.  For some groups (like this one), I keep archives for a ways
back, so even though I've read the articles, I might want to go
back to one and build the tool that was in it.  I use this script
to peruse the contents of a group or groups, to see which articles
have what I'm looking for (or not looking for) in it.  The number of
the article is given, along with a summary taken from the header
'Subject:' line.  The output of this script is suitable for page or more.

	Greg Earle
	Jet Propulsion Laboratory, Spacecraft Data Systems group
	UUCP: ..!sdcrdcf!smeagol!earle
	ARPA: ia-sun2!smeagol!earle@cit-vax.arpa

------------ Cut Here ----- Cut Here ------------------
#! /bin/sh
#
# whatisin : Bourne Shell script to tell you what is the contents
# of a particular news group.  Uses the Subject: header line (presumed
# to be the best indicator).
# Called via 'whatisin <newsgroup> <newsgroup2> ...', 
# e.g. 'whatisin net.sources.bugs'

# Edit next line as appropriate for your system
NEWSDIR="/usr/spool/news"
for i in $*
do
#	There are newlines inserted here via ^V^J
	echo "
        Contents of" $i ":
"
	DESTDIR=`echo $i | sed 's?\.?/?g'`
	cd ${NEWSDIR}/$DESTDIR
# We look at all the files in the directory the slow way, because
# if you keep news archives for certain directories around for a while,
# (like I do at this site), you may have too many files, and a
# grep * will blow up on "too many arguments"
	for a in 1 2 3 4 5 6 7 8 9
	do
		if [ -f ./${a}* ]
		then
#			Note that this next line is broken by postings that contain
#			Mail headers from previous posts that contain a ^Subject:
#			line in the included mail message.  In some cases this is
#			not necessarily a bad thing, try running this on a digested
#			news group like mod.computers.sun/Sun-Spots
			grep "^Subject:" ${a}* /dev/null | sed 's/Subject://g' 
		fi
	done | sort -n 
	echo "--------------------------------------------------------------"
done
