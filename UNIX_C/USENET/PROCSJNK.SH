23-Nov-85 06:10:04-MST,9005;000000000001
Return-Path: <unix-sources-request@BRL.ARPA>
Received: from BRL-TGR.ARPA by SIMTEL20.ARPA with TCP; Sat 23 Nov 85 06:09:52-MST
Received: from usenet by TGR.BRL.ARPA id a003838; 23 Nov 85 7:40 EST
From: Charlie Perkins <charliep@polaris.uucp>
Newsgroups: net.sources
Subject: The (nearly) Compleat junk news processor
Message-ID: <298@polaris.UUCP>
Date: 22 Nov 85 01:24:20 GMT
To:       unix-sources@BRL-TGR.ARPA

=========
Are you tired of just tossing all the articles in your
junk repository just because it's too much hassle to
clean 'em up?  Do you really want to make sure that
there is EVEN MORE news available to your users?

Well, then here is just the thing you need.  I have
been working on this program very sporadically for
a long time, and it seems to be pretty reliable.
You will probably want to modify certain things
about it, not least the shell variables at the tops
of the programs like $LIB, etc.  You may want to
modify it so that junk is kept around less often
than I keep it.  I have some other (less useful)
processing I do as a further stage.

The "main" program is "process_junk", which
calls "modactive" and "getnewsgps".  The latter
two programs are much less useful by themselves,
and could reasonably become C programs if speed
becomes an issue.  One general area that could
be improved is to add locking so that these scripts
could be run without disabling the news.

I don't have a shar program handy, so just separate
the programs which all three are preceded by lines
containing '='s.

I will be happy to distribute bug fixes and/or
improvements if they are sent to me.

===================================================
process_junk
===================================================
SPOOL=/usr/spool/news
NEWSBIN=$SPOOL/bin
LIB=/usr/local/lib/news
ACTIVE=$LIB/active
LOG=$LIB/log
JUNK=$SPOOL/junk
GRPCHARS=",-a-z0-9."
LOCALGP=ibm

PATH=/usr/local:/bin:/usr/bin:$NEWSBIN
count=
export PATH count
#
#	NOTE: You should make sure that no new articles can be posted while
#	      this script is running, otherwise $ACTIVE might be screwed up
#	      a little bit.
#	It assumes that:
#	      1. It is OK to create any necessary newsgroups,
#	      2. The active file is in news 2.10.2 format,
#	      3. Posting to fa.*, mod.* is disallowed,
#	      4. Posting to na.*, net.* $LOCALGP.* is allowed, and
#	      5. It is OK to leave unprocessed articles in $JUNK.
#	      6. There are commas between newsgroups when there an
#		 article is posted to multiple newsgroups.
#	      7. No "parent" newsgroup directories need to be created.
#		 (This assumption is often violated).
#
#	NOTE: A check is performed to make sure that all the current copies 
#		of the junk article are links to the same file.	
#
#	NOTE: If a junk article does not need to be posted to ANY groups,
#		this script doesn't recognize it and leaves the article in $JUNK
#		I do have additional (less interesting) processing that I do in
#		this case.
#
#	Because of #1, a "grep '^Newsgroups: ' $JUNK/*" might be advisable
#	before running this script.
#
#	Last note: don't shoot me if some comment or another is outdated.
#
cd $JUNK
junkcount=`sed -n "/^junk /s/junk \([0-9]*\).*/\1/p" $ACTIVE`
case "$junkcount" in
"")	exit
esac
newsgrps=`while test "$junkcount" -ge ${i=0}000
	do
		case "$i" in
		0)	getnewsgps [1-9] [1-9]? [1-9]??;;
		*)	getnewsgps ${i}???
		esac
		i=\`expr $i + 1\`
	done | tr ' ' '\012' | sort -u`
(cd $LIB; tar cf - active ) | (cd /usr/tmp; tar xpvf - )	# Save a $ACTIVE
#
#	Crunch, crunch -- do a newsgroup at a time...
#
for newsgrp in $newsgrps
do
	ngline='^Newsgroups:.*( |,)'$newsgrp'(,|$)'
	articles=`2> /dev/null egrep -l "$ngline" [1-9] [1-9]? [1-9]??
		i=1
		while test ${i}000 -le $junkcount
		do
			2> /dev/null egrep -l "$ngline" ${i}???
			i=\`expr $i + 1\`
		done`
	newsdir=$SPOOL/`echo $newsgrp | sed "s;\.;/;g"`
	count=`sed -n "/^$newsgrp /s/$newsgrp \([0-9]*\).*/\1/p" $ACTIVE | sed 1q`
	if test -z "$count"   -a   ! -d $newsdir
	then
	#
	#	Directories will exist for non-existent
	#	newsgroups, when the newsgroup has a sub-group
	#	that was created first (e.g, "mod.std.c" came
	#	before "mod.std").
	#
	#	NOTE that this does not handle the case when
	#	parent directories also need to be created.
	#
		date_field=`date '+%h %d %H:%M'`
		echo "$date_field	local.prjunk	Junk processing: creating newsgrp $newsdir."
		mkdir $newsdir
	fi | tee -a $LOG
	count=`expr 0$count + 0`		# Toss leading zeroes.
	if test ! -d $newsdir
	then
		date_field=`date '+%h %d %H:%M'`
		echo "$date_field	local.prjunk	Junk processing: Bad directory $newsdir!" |
			tee -a $LOG
		continue
	fi
	prevcount=$count
	chown news $newsdir
	INDEX=$newsdir/IndeX
	#
	#	Create indexes for each newsgroup
	#	-- save the time needed for continually
	#	   grepping each article.
	#
	( cd $newsdir
	  2> /dev/null grep '^Message-ID: <' [1-9] [1-9]? [1-9]??
  	  i=1
	  while test ${i}000 -le $count
	  do
	  	2> /dev/null grep '^Message-ID: <' ${i}???
	  	i=`expr $i + 1`
	  done ) | sed "s/\([0-9]*\):\(.*\)/\2 \1/" | uniq -2 > $INDEX
	for article in $articles
	do
		articleID=`sed -n "/^Message-ID: </s///p
					10q" $article | sed 1q`
		#
		#	This new strategy will fail in the following case:
		#	    the article needs to be posted to some of the
		#	    newsgroups in the Newsgroup line, but not all
		#	    of them.  I have never seen this case.
		#	    I have an old version that handles it, though.
		#	Also, it removes articles after they have been
		#	    processed.  That may be undesirable.
		#	    (our "rm" secretly saves them for a day...)
		#
		oldartnum=`sed -n "/^Message-ID: <$articleID/s/.* //p" $INDEX`
		case "$oldartnum" in
		"")	count=`expr 0"$count" + 1`
			newsart=$newsdir/$count
			date_field=`date '+%h %d %H:%M'`
			if test ! -f $newsart    &&    ln $article $newsart
			then
				echo "$date_field	local.prjunk	Linked $article to $newsart."
				echo "Message-ID: <$articleID $count" >> $INDEX
				oldartnum=$count
				linegrps=`getnewsgps $article |
						tr ' ' '\012' | sort -u`
				case "$linegrps" in
				*$newsgrp)	rm $article
				esac	# If it was $article's last newsgroup.
			else
				echo "$date_field	local.prjunk	Link problem; $article $newsart"
			fi | tee -a $LOG
		esac
		echo article="$article", oldartpath="$newsdir/$oldartnum"
	done
	#
	# 	All articles belonging to $newsgrp have been processed.
	#	Adjust the count field in $ACTIVE.
	#
	case "$prevcount" in
	"$count")	;;
	*)		modactive $newsgrp $count
	esac
done
#
#	Final cleanup; modify entry for junk newsgroup in $ACTIVE.
#
date_field=`date '+%h %d %H:%M'`
oldarts=`ls [1-9]* | sort -n`	||
	{ echo "$date_field	local.prjunk	ls problem during $JUNK cleanup." |
					tee -a $LOG
	  exit ; }
count=0
for article in $oldarts
do
	count=`expr $count + 1`
	if test ! -f $count   &&   ln $article $count
	then
		rm $article
	else
		date_field=`date '+%h %d %H:%M'`
		echo "$date_field	local.prjunk	mv $article $count fails in $JUNK cleanup." |
			tee -a $LOG
	fi
done
modactive junk $count
===================================================
modactive
===================================================
SPOOL=/usr/spool/news
NEWSBIN=$SPOOL/bin
LIB=/usr/local/lib/news
ACTIVE=$LIB/active
LOG=$LIB/log
JUNK=$SPOOL/junk
LOCALGP=ibm
STATE=ny
PATH=/bin

newsgrp=$1
count=$2

case $count in
[1-9])		count=0000$count ;;
[1-9]?)		count=000$count ;;
[1-9]??)	count=00$count ;;
[1-9]???)	count=0$count ;;
[1-9]????)	;;
*)		
		date_field=`date '+%h %d %H:%M'`
		echo "$date_field	local.modact	Problem in $newsgrp, count=$count." >> $LOG
		continue
esac

therenow=`grep "^$newsgrp " $ACTIVE`
case "$therenow" in	# if empty, it's a new newsgroup.
"")	parent=`expr "$newsgrp" : "\([^.]*\)."`
	case "$parent" in
	mod|fa|ba)		postable=n ;;
	$LOCALGP|$STATE|net|na)	postable=y ;;
	*)			echo "Problem with $newsgrp; parent=$parent..."
				continue
	esac
	echo "$newsgrp $count 00001 $postable" >> $ACTIVE ;;
*)	sed "/^$newsgrp /s/ [0-9]*/ $count/" $ACTIVE > /tmp/junkactive$$
	mv /tmp/junkactive$$ $ACTIVE
esac
===================================================
getnewsgps
===================================================
#!/bin/sh

PATH=/bin:/usr/bin
GRPCHARS=",-a-z0-9."

case $# in
0|1|2|3|4|5|6|7) # For only a few arguments, it's wasteful to have big pipelines.
	for i
	do
		2> /dev/null sed -n "/^Newsgroups: [$GRPCHARS]*$/s/,/ /g
			   /^Newsgroups: \([ $GRPCHARS]*\)$/s//\1/p
			   10q" $i
	done ;;
*)	# For lots of arguments, the for loop is wasteful.
	2> /dev/null grep '^Newsgroups: ' $* |
		sed "s/\([0-9]*\):Newsgroups: \(.*\)/\2 \1/" |
		uniq -1  | sed "s/ [0-9]*$//
			s/,/ /g"
esac
-- 

Charlie Perkins, IBM T.J. Watson Research	philabs!polaris!charliep,
		perk%YKTVMX.BITNET@berkeley,  perk.yktvmx.ibm@csnet-relay
