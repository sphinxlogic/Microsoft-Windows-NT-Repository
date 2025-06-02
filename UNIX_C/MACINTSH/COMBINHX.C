22-Dec-86 00:50:17-MST,7048;000000000000
Return-Path: <INFO-MAC-REQUEST@SUMEX-AIM.STANFORD.EDU>
Received: from SUMEX-AIM.STANFORD.EDU by SIMTEL20.ARPA with TCP; Mon 22 Dec 86 00:49:59-MST
Date: Sun 21 Dec 86 23:49:13-PST
From: David Gelphman... <INFO-MAC-REQUEST@SUMEX-AIM.STANFORD.EDU>
Subject: UNIX-COMB.C
To: info-mac-redist: ;
Message-ID: <12264754603.20.INFO-MAC-REQUEST@SUMEX-AIM.STANFORD.EDU>

/*
    comb - combine split binhex files
    usage:
	comb file1 .. fileN > result ; xbin result
	comb -m file > result ; xbin result
    compile: cc -o comb comb.c

    binhex files often come across the net split into multiple parts.  I
got sick of re-combining them by hand.  comb is a quick hack that takes 
as its arguments a set of files which are the parts
of a binhex file.  It splits out the binhex file to standard output.
If the parts have been combined into a single file already, with mail
headers and such in between, the "-m" flag can be used.  The text that
doesn't look like binhex goes to standard output, unless the "-q" (for
quiet) flag is specified.
    It's real ugly (even has gotos!), but hey, it works.
    Author: Greg Dudek
    Usenet:	{linus, ihnp4, allegra, decvax, floyd}!utcsri!dudek
*/

#include <stdio.h>
#define min(a,b)	(a<b?a:b)

#ifdef DEBUG
#define EBUG
#endif

#ifdef EBUG
#define debug(x)	x
#else
#define debug(x)	/**/
#endif

int verbose=1;
int mailfile = 0;	/* file contains multiple parts */
int gotend = 0;		/* found apparent end of binhex data */
int eof = 0;		/* found at least 1 eof (only used if mailfile) */

main(argc,argv)
char **argv;
{
    int l,i,fnum, curline;
    char inline[256];
    char inline2[256];
    FILE *fd;

    if (argc <= 1) {
	fprintf(stderr,"Usage: %s [-q] [-m] file1 [..fileN]\n",argv[0]);
	exit(2);
	}

    fnum=1;
    /* get flags */
    while (argv[fnum][0]=='-') {
        switch (argv[fnum][1]) {
        case 'm':
            mailfile=1;
            break;
        case 'q':
            verbose=0;
            break;
        case 'v':
            verbose=1;
            break;
        default:
            fprintf(stderr,"Unknown option -%c ignored\n",argv[fnum][1]);
            break;
            }
        fnum++;
        }

    fd = fopen(argv[fnum],"r"); 
    if (fd==0) {
	perror(argv[fnum]);
	exit(3);
	}
    argc--;
    fnum++;
    for ( ;; ) {
        if (fgets(inline,80,fd)<=0) break;
        if (strncmp(inline,"(This file",10)==0) {
            printf("%s",inline);

            fgets(inline,80,fd);
            printf("%s",inline);

            fgets(inline,80,fd);
            printf("%s",inline);
            l = strlen(inline);
            for ( ; (fgets(inline,80,fd)>0) && (strlen(inline)==l); ) 
                printf("%s",inline);
            break;
            }
        else {
            if (verbose) fprintf(stderr,"%s",inline);
            }
        }
    if (!mailfile) {
        /* throw away rest of file */
        while (verbose && (fgets(inline,80,fd)>0)) {
            fprintf(stderr,"%s",inline);
            }
        }

    for ( ; (mailfile && !eof) || (argc-->1);  ) {
        if (!mailfile) fd = freopen(argv[fnum++],"r",fd);

	/* hack, hack.  jump here when in mailfile mode */
	/* instead of opening a new file		*/
fakenew:
        /* only consider first 500 lines of each file for data start */
        for (curline=1;curline<500;curline++) {
            if (fgets(inline,90,fd)<=0) {
                debug(printf("eof (argc=%d)\n",argc));
                eof = 1;
                break;
                }

	/* jump here if we already read 2 lines thinking the first was
	   the start of the binhex data, but that hypothesis failed.
	   We make the extra line look like we just read it, & jump here.
	*/
got1:

	    /* is the line the right length? */
            if (strlen(inline)!=l) {
                if (verbose) fprintf(stderr,"%s",inline);
                debug(printf("Bad length %d != %d\n",strlen(inline),l));
                continue;
                }

	    /* we don't expect binhex to contain these lines, just in case
		we fluked out on line length 
	    */
            if ((substr(inline,"here"))  ||
                (substr(inline,"From")) ||
                (substr(inline,"CUT")) ||
                (substr(inline,"end")) ||
                (substr(inline,"Path")) ||
                (substr(inline,"cut")) ) {
                if (verbose) fprintf(stderr,"%s",inline);
                debug(printf("Has English\n"));
                continue;
                }

            /* get another line, see if lengths match */
            if (fgets(inline2,80,fd)<=0) {
                eof = 1;
                break;
                }
	    /* check it for the keywords too */
            if ((strlen(inline)==strlen(inline2)) &&
                ( ! ((substr(inline2,"here"))  ||
                (substr(inline2,"From")) ||
                (substr(inline2,"CUT")) ||
                (substr(inline2,"end")) ||
                (substr(inline2,"Path")) ||
                (substr(inline2,"cut")) ))) {

                /* Okay, we're convinced 	*/
		/* spit the 2 lines		*/
                printf("%s",inline);
                printf("%s",inline2);
		/* Take any more lines of the right length */
                for ( ; ; ) {
                    if (fgets(inline,80,fd)<=0) {
                        eof = 1;
                        break;
                        }
                    if (strlen(inline)!=l) {
                        i = strlen(inline);
                        while (inline[i-1]=='\n') i--;
                        if (inline[i-1]==':') {
                            /* end of stuff */
                            gotend = 1;
                            printf("%s",inline);
                            }
                        break;
                        }
                    printf("%s",inline);
                    }
                }
            else {
                debug(printf("Second line is bad\n"));
                strcpy(inline,inline2);
                goto got1;
                }
            break;
            }
        if (mailfile && !eof) goto fakenew;
        while (verbose & (fgets(inline,80,fd)>0)) {
            fprintf(stderr,"%s",inline);
            }
        }
    if (!gotend) {
	fprintf(stderr,"File didn't seem to end properly\n");
	exit(1);
	}
    exit(0);
    }

substr(s,t)
char *s, *t;
{
    extern char *index();
    while ((s=index(s,*t))!=0) {
        if (strncmp(s,t,(min(strlen(s),strlen(t))))==0) return(1);
        s++;
        }
    return(0);
    }
-- 
	    Dept. of Computer Science (vision group)    University of Toronto
	    Usenet:	{linus, ihnp4, allegra, decvax, floyd}!utcsri!dudek
	    CSNET:	dudek@ai.toronto.edu
	    ARPA:	dudek%ai.toronto.edu@csnet-relay
	    DELPHI:     GDUDEK
	    Paper mail: 10 King's College Circle, Toronto, Canada 

-------
