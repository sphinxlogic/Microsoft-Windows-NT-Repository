 9-May-86 20:59:01-MDT,4366;000000000001
Return-Path: <unix-sources-request@BRL.ARPA>
Received: from BRL-SMOKE.ARPA by SIMTEL20.ARPA with TCP; Fri 9 May 86 20:58:43-MDT
Received: from USENET by SMOKE.BRL.ARPA id a015994; 9 May 86 19:41 EDT
From: "Daniel R. Levy" <levy@ttrdc.uucp>
Newsgroups: net.sources
Subject: search -- search for arbitrary string in file
Message-ID: <858@ttrdc.UUCP>
Date: 7 May 86 05:43:19 GMT
To:       unix-sources@brl-smoke.arpa

Here is a little C program to search for an arbitrary string in a file
or files.  Unlike the 'grep' variety of searching commands, it works even
if the string to be searched for contains newline characters or if the
file to be searched is not an ASCII text file.  It reports the byte offset
(byte 1 is the beginning byte) at which the match occurred, and the line
of the file (number of newline characters encountered through the end
of the match, plus one) in which the match occurs.

Code follows below.  Do NOT run through /bin/sh or unshar.  Watch out
for trailing signature.  Flames about gotos > /dev/f77 :-).

#include <stdio.h>
#include <string.h>
main(argc,argv)	/* search string filename[s] */
int argc;
char **argv;
{
	/*
	   search searches for instances of an arbitrary string
	   given as the first argument in the files named as
	   second, third,... arguments.  The search string may
	   be completely arbitrary (e.g., contain newlines,
	   non-ASCII characters) except that it must be null
	   terminated (so a string containing nulls cannot be
	   searched for).  Matches are reported on stdout
	   with byte offset in file and line number in file.
	   Failure to match is reported on stderr and the exit
	   value equals the number of files in which there was
	   no match.
	*/

	char *malloc();
	void perror();
	register char *rdb;	 /* pointer to circular read buffer */
	int irdl;		/* length of read buffer */
	char *rdp;		/* "starting" location in read buffer */
	register char *rde;	/* end of read buffer */
	register char *pat;	/* start of string */
	register char *pp;	/* pointer inside string */
	register char *bp;	/* pointer inside buffer */
	register int i;
	register int ifile;	/* current file arg */
	register int ibyt;
	register int ilin;
	int matched;
	int istex=0;
	FILE *ifp = NULL;	/* input file pointer */
	if (argc <= 2) {
		(void) fprintf(stderr,
			"Usage: %s arbitrary-fixed-string file[s]\n",argv[0]);
		return -1;
	}
	pat = argv[1];
	irdl = strlen(pat);
	if (irdl == 0) {
		(void) fprintf(stderr,
			"%s: zero length string to match\n",argv[0]);
		return -2;
	}
	rdb = malloc((unsigned)irdl);
	if (!rdb) {
		(void) fprintf(stderr,
			"%s: cannot allocate enough memory\n",argv[0]);
		return -3;
	}
	rde = rdb + irdl;
	for (ifile=2; ifile<argc; ifile++) {	/* for each file */
		if (!(ifp = fopen(argv[ifile],"r"))) {
			(void) perror(argv[ifile]);
			istex++;
			continue;
		}
		rdp=bp=rdb;
		matched=ibyt=0;
		ilin=1;
		while(bp<rde) {	/* fill the buffer to begin */
			if ((i=getc(ifp)) != EOF) {
				*bp++ = i;
				if (i == '\n') ilin++;
			} else {
				/* EOF */
				(void) fprintf(stderr,
					"file %s: no string match\n",
					argv[ifile]);
				istex++;
				goto nextfile;
			}
		}
compare:	bp=rdp; pp=pat; ibyt++;
		while (bp<rde) if (*bp++ != *pp++) goto nomatch;
		if (rdb != rdp) {
			bp=rdb;
			while (bp<rdp) if (*bp++ != *pp++) goto nomatch;
		}
		/* match */
		(void) fprintf(stdout,
			"file %s: string match at byte %d (line %d)\n",
			argv[ifile],ibyt,ilin);
		matched=1;
nomatch:	if ((i=getc(ifp)) != EOF) *rdp++ = i;	/* add character */
		else {
			if (!matched) {
				(void) fprintf(stderr,
					"file %s: no string match\n",
					argv[ifile]);
				istex++;
			}
			goto nextfile;
		}
		if (rdp==rde) rdp=rdb;
		if (i=='\n') ilin++;
		goto compare;
nextfile:	if (ifp != NULL) (void) fclose(ifp);
	}
	return istex;
}
-- 
 -------------------------------    Disclaimer:  The views contained herein are
|       dan levy | yvel nad      |  my own and are not at all those of my em-
|         an engihacker @        |  ployer or the administrator of any computer
| at&t computer systems division |  upon which I may hack.
|        skokie, illinois        |
 --------------------------------   Path: ..!{akgua,homxb,ihnp4,ltuxa,mvuxa,
						vax135}!ttrdc!levy
