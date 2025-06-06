23-Nov-85 07:10:23-MST,9009;000000000001
Return-Path: <unix-sources-request@BRL.ARPA>
Received: from BRL-TGR.ARPA by SIMTEL20.ARPA with TCP; Sat 23 Nov 85 07:10:05-MST
Received: from usenet by TGR.BRL.ARPA id a004133; 23 Nov 85 8:40 EST
From: Charlie Perkins <charliep@polaris.uucp>
Newsgroups: net.sources
Subject: Retrieving data from broken UUCP transfers
Message-ID: <300@polaris.UUCP>
Date: 22 Nov 85 02:18:42 GMT
To:       unix-sources@BRL-TGR.ARPA

=========
Many, many times I have discovered TM files remaining in
our uucp spool directory resulting from incomplete uucp
sessions.  Moreover, a significant minority of those files
contained information that was never retransmitted, for
reasons that I cannot undestand.  I am a fanatic about not
losing data, so I wrote the following programs to make
maximum use of the incomplete TM files.  This has the
effect of sometimes duplicating mail that WAS eventually
retransmitted correctly, but my users don't mind that at
all.  Duplicate news articles are later recognized
as such.  When all the articles are retrieved from the TM
files, they can be moved to the junk directory and
processed using "process_junk" which I have posted
separately.

The files are deTM, deTMunbatch.c, and mv2junk.
deTM does not autmatically invoke mv2junk so that it can
work without having to lock the news files.  mv2junk just
has the effect of transferring deTM's results into the
junk news directory so that process_junk can get to it.
I would recommend at least reading the comments in
deTM before trying to run it.

deTM_unbatch is a C program, it was too crappy to do as a
shell script.  I would be willing to install #define's for
portability if anybody wants to contribute them.  deTM_unbatch
has the effect of breaking down a batched news file into its
constituent articles.

============================================================
deTM
============================================================
SPOOL=/usr/spool/news
LIB=/usr/local/lib/news
ACTIVE=$LIB/active
LOG=$LIB/log
JUNK=$SPOOL/junk
NEWSBIN=$SPOOL/bin
UUCPDIR=$SPOOL/uucp
PATH=/usr/local:/bin:/usr/bin:$NEWSBIN
FOO=trash	# Only needed to prevent "set" headaches.
#
#	Process TM files.  Three cases are known:
#		1. Empty file (that is easy!)
#		2. Mail file -- send it to the intended
#			recipient.
#		3. Compressed mail -- this is the case
#			that takes all the code.
#
#	This script assumes that the TM files have been moved into the
#	directory $UUCPDIR/TM.  It is unwise to process the TM
#	files in /usr/spool/uucp without somehow turning off all the
#	uucp stuff, and moving the files is easy enough to do.
#
cd $UUCPDIR/TM
for i in TM.?????.???
do
	filetype=`file $i | sed "s/.*:	//"`
	case "$filetype" in
	empty)		rm -f $i ;;
	"ascii text")	receiver=`sed "s/^To: //p/" $i`
			case "$receiver" in
			"")	;;
			*)	mail $receiver < $i
			esac ;;
	data)		testing=`( /usr/local/lib/news/compress -d < $i 2>&1 ) |
							sed 1q`
			case "$testing" in
			"stdin: not in compressed format"*)
				testing=`( tar tvf $i 2>&1 ) | sed 1q`
				case "$testing" in
				"tar: errno returned 0, Can't read input"*) ;;
				"directory checksum error"*) ;;
				*)	echo $i may be a tar-format data file. 1>&2
					continue
				esac
				testing=`( cpio -itv < $i 2>&1 ) | sed 1q`
				case "$testing" in
				"Out of phase--get help")
					echo $i is a data file of undetermined format ;;
				*)	echo $i may be a cpio-format data file. 1>&2
				esac
				continue
			esac
			(mkdir $UUCPDIR/$i 2>&1) > /dev/null
			cd $UUCPDIR/$i
			/usr/local/lib/news/compress -d < $UUCPDIR/TM/$i |
				deTM_unbatch $i
			cd $UUCPDIR/TM ;;
	*)		echo "filetype is $filetype!!" 1>&2 ;;
	esac
done

#
#	Make a directory containing all the news articles, without
#	duplication.  "deTM_unbatch" creates files named after their article ID.
#	"deTM_unbatch" also creates in each of the "TM" directories containing
#	the news articles, a file called "index" that is a list of all the
#	article ID's (== filenames) in the TM directory.
#
cd $UUCPDIR
zerodirs=`find $UUCPDIR/*/index -size 0 -print`
rm -f `echo $zerodirs`
rmdir `echo $zerodirs | sed "s/.index//g"`
mkdir all_articles
for i in TM.?????.???
do
	if test ! -d $UUCPDIR/$i
	then
		echo Oh, how terrible -- $UUCPDIR/$i not a directory! 1>&2
		exit
	fi
	cd $UUCPDIR/$i
	for j in *
	do
		case "$j" in
		index)	continue ;;
		[1-9]*)	destination=$UUCPDIR/all_articles/$j
			if test ! -f "$destination"
			then
				ln $j "$destination"
			else
				set $FOO `ls -l "$destination"`
				oldsize=$6
				set $FOO `ls -l $UUCPDIR/$i/$j`
				newsize=$6
				if test "$oldsize" -lt "$newsize"
				then
					rm -f "$destination"
					ln $UUCPDIR/$i/$j $destination
				fi
			fi ;;
		*)	echo Oooh, what a weird file -- $UUCPDIR/$i/$j 1>&2
		esac
	done
done
#
#	Now run "mv2junk", and then "process_junk".  Those two programs should
#	be run with normal news processing disabled, to avoid having two
#	programs simultaneously trying to update $ACTIVE.
#
============================================================
de_TMunbatch.c
============================================================
/*
 * unbatchnews: extract news in batched format and process it one article
 * at a time.  The format looks like
 *	#! rnews 1234
 *	article containing 1234 characters
 *	#! rnews 4321
 *	article containing 4321 characters
 *
 *	Special purpose processing: Create new files in
 *		~news/uucp/TM* using the article ID as the filename.
 */

#ifndef lint
static char	*SccsId = "@(#)unbatch.c	1.8	9/3/84";
#endif !lint

# include <stdio.h>

char buf[BUFSIZ];
char hdr[BUFSIZ];

static char RNEWS_STR[] = "#! rnews ";
static char MSG_STR[] = "Message-ID: <";
static char TMPNAME[(sizeof "/usr/spool/news/uucp/") + (sizeof "TM.xxxxx.xxx")] =
	"/usr/spool/news/uucp/";

main(argc, argv)
int argc;
char **argv;
{
	register int length, x;
	register FILE *ndx_pfn, *pfn = NULL;
	register long size;
	register char *angle_brack, *idptr;
	char filename[BUFSIZ];
	char *index(), *gets();
	long atol();

	if ((argc != 2) ||
	    (strlen (argv[1]) != 12) ||
	    (0 != strncmp ("TM.", argv[1], 3)))
	{
		fprintf (stderr, "Argument error, argv[1]=%s.\n", argv[1]);
		exit (1);
	}

	strcat (TMPNAME, argv[1]);
	sprintf (filename, "%s/index", TMPNAME);
	if (0 == (ndx_pfn = fopen (filename, "w")))
		perror();
	gets (buf);
	while (0 == (x = strncmp (buf, RNEWS_STR, (sizeof RNEWS_STR) - 1)))
	{
		size = atol (buf - 1 + (sizeof RNEWS_STR));
		if(size <= 0)
		{
			fprintf (stderr, "Bad size=%d.\n", size);
			exit (3);
		}
		*hdr = NULL;
		pfn = NULL;
		while ((pfn == NULL)  &&  (size > 0)  &&  !feof(stdin))
		{
			fgets (buf, BUFSIZ, stdin);
			/* Save header info until article ID arrives. */
			strcat (hdr, buf);
			if (0 == strncmp (MSG_STR, buf, (sizeof MSG_STR) - 1))
			{
				idptr = buf + (sizeof MSG_STR) - 1;
				if (angle_brack = index (idptr, '>'))
				{
					*angle_brack = NULL;
					sprintf(filename,"%s/%s",TMPNAME,idptr);
					pfn = fopen(filename, "w");
					fprintf (ndx_pfn, "%s\n", idptr);
					fwrite (hdr,length = strlen(hdr),1,pfn);
					if ( 0 >= (size -= length))
					{
						fprintf (stderr,
						"Oops, header size too big.\n");
						exit (4);
					}
				}
				else
				{
					fprintf (stderr,
						"Oops, bad Message-ID.\n");
					exit (5);
				}
			}
		}
		if (pfn == NULL)	/* Article ID was not found... */
			exit();
		while (fgets (buf, BUFSIZ, stdin)	&&	(0 < size))
		{
			fwrite (buf, length=strlen(buf), 1, pfn);
			size -= length;
		}
		if (size != 0)
		{
			fprintf (stderr, "Article size != %d, filename=%s.\n",
					size, filename);
			exit (6);
		}
		fclose(pfn);
	}
	if (x)	/* x != 0   implies that the RNEWS strncmp test failed above. */
	{
		fprintf(stderr, "out of sync, skipping %s\n", buf);
		exit (2);
	}
}
============================================================
mv2junk
============================================================
SPOOL=/usr/spool/news
NEWSBIN=$SPOOL/bin
LIB=/usr/local/lib/news
ACTIVE=$LIB/active
JUNK=$SPOOL/junk

PATH=/usr/local:/bin:/usr/bin:$NEWSBIN
(cd $LIB; tar cf - active ) | (cd /usr/tmp; tar xpvf - )	# Save a $ACTIVE
count=`awk '$1 == "junk" {print $2}' $ACTIVE`
case "$count" in
"")	exit
esac
curdir=`pwd | sed "s/ //"`
for artdir in $*
do
	cd $curdir/$artdir
	oldarts=`ls | sort +n`  ||
	{ echo Something went wrong during $JUNK cleanup.
	  exit ; }
	for article in $oldarts
	do
		count=`expr $count + 1`
		if test -f $count
		then
			echo ln $article $count fails during $JUNK cleanup.
		else
			ln $article $JUNK/$count
		fi
	done
done
modactive junk $count
echo diff $LIB/active /usr/tmp/active
diff $LIB/active /usr/tmp/active
-- 

Charlie Perkins, IBM T.J. Watson Research	philabs!polaris!charliep,
		perk%YKTVMX.BITNET@berkeley,  perk.yktvmx.ibm@csnet-relay
