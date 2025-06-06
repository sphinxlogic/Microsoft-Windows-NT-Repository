 4-Jan-86 04:15:17-MST,5433;000000000001
Return-Path: <unix-sources-request@BRL.ARPA>
Received: from BRL-TGR.ARPA by SIMTEL20.ARPA with TCP; Sat 4 Jan 86 04:15:10-MST
Received: from usenet by TGR.BRL.ARPA id a007212; 4 Jan 86 5:42 EST
From: "Col. G. L. Sicherman" <colonel@sunybcs.uucp>
Newsgroups: net.sources
Subject: prnews
Message-ID: <2696@sunybcs.UUCP>
Date: 3 Jan 86 15:32:42 GMT
To:       unix-sources@BRL-TGR.ARPA

Do you ever get tired of sitting at the terminal and hitting the space
bar?  Well, you should.  You look silly doing it.  The following script
saves you the trouble by _printing_ the news.  If you have troff or
ditroff, it will print in columns in 8-point Roman condensed, so you
can pretend you're reading the New York Herald-Tribune.  It will even
unrot jokes that need it, if you have a suitable rot-detector.

Warnings: it's klugy.  You'll probably have to tweak it.  And don't start
it with the -uu option and then read news interactively; you'll clobber
your .newsrc.  And syshats hate it because it's a hog.

I think you'll like it.

------- DETACH CAREFULLY ALONG THIS LINE ----------------------------
:	prnews [options] newsgroup ...
:	print all articles in one or more newsgroups
:
:	-y	print with DITROFF, in columns
:	-u	print only unread articles
:	-uu	print only unread articles, mark articles read in .newsrc
: debugging options:
:	-d	divert to standard output
:	-t	divert DITROFF output to standard output
::::::::::::::::
: ISROT=false
ISROT=/cs/staff/colonel/bin/isrot
TMP=/usr/tmp
NNSED=$TMP/prn$$n
NNTMP=$TMP/prn$$x
NUNU=$TMP/prn$$u
GREPT=$TMP/prn$$g
SED2=$TMP/prn$$s
AWK2=$TMP/prn$$a
PRINP=$TMP/prn$$i
trap "rm -f $TMP/prn$$? ; exit" 0 1 2 13 15
while
test "$*"
do
case $1 in
-y)	YFLAG=YFLAG; shift;;
-uu)	UUFLAG=UUFLAG; UFLAG=UFLAG; shift;;
-u)	UFLAG=UFLAG; shift;;
-d)	DFLAG=DFLAG; shift;;
-t)	TFLAG=TFLAG; shift;;
-)	echo "$0: bad switch $1" >&2;exit;;
*)	break;;
esac
done
:	set up 2 sed scripts and 1 awk script.
cat << \NNSED > $NNSED
s/^[^ ]* /,/
s/$/,/
s/,\([0-9][0-9]*\),/,\1-\1,/g
s/\([0-9][0-9]*\)-\([0-9][0-9]*\)/\1 l\
\2 u/g
s/^,//
s/,$//
s/,/\
/g
NNSED
cat << \XSED >$SED2
s/[ 	][ 	]*/ /g
s/[ 	][ 	]*$//
s/\\/&e/g
s/====*/==/g
s/\~\~\~\~*/\~\~/g
s/<><>[<>]*/<>/g
s/___*/__/g
s/|/|\\|/g
s/->/\\(&/g
s/<-/\\(&/g
s/---*/\\(em/g
s/- - [- ]*-/\\-/g
s/-/\\-/g
s/\([ ([]\)"/\1``/g
s/^"/``/
s/ '\([^ ]\)/ `\1/g
s/ '/ \\'/g
s/^'/\\'/
s/"/''/g
s/^\./\\\&./
s/^Subject: \(.*\)$/.SU "\1"/
s/^From: \(.*\)$/.FR "\1"/
s/^Date: \(.*\)$/.DA "\1"/
XSED
cat << \XAWK >$AWK2
$1=="**********" {body=0; slash=index($2,"/"); g=substr($2,1,slash-1);
	if (g!=g0) print ".NG",g; g0=g; print ".NA",substr($2,slash+1);next}
length($1)>20 {print substr($1,1,20),"\&" substr($1,21,40),substr($1,41);
	for (i=2; i<=NF; i++) print $i; next}
/^\./ {print; next}
/^[ 	]*$/ {if (body++) print ".sp .6"; else print ".HE"; next}
body<1 {next}
{print}
XAWK
:	now print some NEWS!
if test "$UUFLAG"
then
cp /dev/null $NUNU
fi
if test "$YFLAG"
then
cat << \XFORMAT >$PRINP
.\"	font for news text
.ds NF RC
.nr PO 0.5i
.nr TM 0.5i
.po \n(POu
.ll 1.7i
.ps 8
.vs 1.1m
.de tm	\" top margin
'sp |\\n(TMu
.nr co 0 1	\" which column
..
.de en	\" end-macro
.\"	remove yo-yo trap
.	wh -.6i
.	wh -.6i pn
..
.de pn	\" page-number
.	if \\n(co<1 \{\
'	sp 1
.	tl '''\fBPage %\fP' \}
..
.em en
.de bm	\" bot margin
.	pn
.	ie \\n+(co<4 \{\
.	po +1.9i
'	sp |\\n(TMu
.	ns \}
.	el \{\
.	po \\n(POu
'	bp \}
..
.wh -.6i bm
.de NG
.	ce 3
\l'1.5i'
.ft B
\\$1
.ft \*(NF
.	sp -.5
\l'1.5i'
..
.de NA
.ds N@ \l'1.5i-\w'\fB\\$1\f\*(NF'u/2u'
.ce 1
.ft B
\u\\*(N@\d\\$1\u\\*(N@\d
.ft \*(NF
..
.\"	SU - subject line(s)
.de SU
.	ds S@ "\\$1
..
.\"	DA - date
.de DA
.	ds D@ "\\$1
..
.\"	FR - from whom
.de FR
.	ds F@ "\\$1
..
.\"	HE - dump heading
.de HE
.	ft B
\\*(S@
.	ft R
.	br
\\*(D@
.	br
.	ft I
\\*(F@:
.	ft \*(NF
.	br
..
.wh 0 tm
XFORMAT
fi
for i
do
	d=/usr/spool/news/`echo $i | tr . /`
	if test ! -d $d
	then
		echo "prnews: $i not found" >&2
		continue
	fi
	cd $d
	if test "$UFLAG"
	then
		grep "^$i[:!]" $HOME/.newsrc | sed -f $NNSED > $NNTMP
		range=`ls -F | grep -v '[*/]' | sed -e 's/$/ n/' \
		| sort -n $NNTMP - | awk '$2!="n"{x=$2;next} x=="u"{print $1}'`
	else
	range=`ls -F | grep -v '[*/]' | sort -n`
	fi
	for j in $range
	do
		last=$j
		echo '**********' $i/$j '**********'
		if test $i = net.jokes && $ISROT $j
		then
		tr A-Za-z N-ZA-Mn-za-m
		else
		cat
		fi < $j
	done
:
:	build new .newsrc records.
:
	if test "$UUFLAG"
	then
		cat -n $HOME/.newsrc |\
		if grep "	$i[:!]" > $GREPT
		then
			awk '{printf "n %s\t%s", $1, $2}' < $GREPT >> $NUNU
		else
			echo -n n "0	$i:" >> $NUNU
		fi
		echo " 1-$last" >> $NUNU
	fi
done | if test "$YFLAG"
then
sed -f $SED2
else
cat
fi | if test "$YFLAG"
then
awk -f $AWK2
else
cat
fi >> $PRINP
if test "$UUFLAG"
then
	cp $HOME/.newsrc $HOME/.newsrc.bak
	cat -n $HOME/.newsrc.bak | \
	sed -e 's/^/o /' | sort +1n -2 +0 -1 +2 -3 $NUNU - | \
	sort -um +2 -3 | sed 's/^[^	]*	//' > $HOME/.newsrc
fi &
if test "$DFLAG"
then
	cat $PRINP
elif test "$YFLAG"
then
	if test "$TFLAG"
	then
		ditroff -t $PRINP
	else
		ditroff $PRINP
	fi
else
lpr $PRINP
fi
-- 
Col. G. L. Sicherman
UU: ...{rocksvax|decvax}!sunybcs!colonel
CS: colonel@buffalo-cs
BI: csdsicher@sunyabva
