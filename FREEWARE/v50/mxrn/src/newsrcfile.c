
#if !defined(lint) && !defined(SABER)
static char XRNrcsid[] = "$Header: /net/objy27/wrld/mnt11/ricks/src/master/xrn/newsrcfile.c,v 1.5 1993/01/11 02:15:13 ricks Exp $";
#endif

/*
 * xrn - an X-based NNTP news reader
 *
 * Copyright (c) 1988-1993, Ellen M. Sentovich and Rick L. Spickelmier.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the University of California not
 * be used in advertising or publicity pertaining to distribution of 
 * the software without specific, written prior permission.  The University
 * of California makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express or
 * implied warranty.
 *
 * THE UNIVERSITY OF CALIFORNIA DISCLAIMS ALL WARRANTIES WITH REGARD TO 
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND 
 * FITNESS, IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE FOR
 * ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * newsrcfile.c: routines for reading and updating the newsrc file
 *
 */

#include "copyright.h"
#include <stdio.h>
#include "config.h"
#include "utils.h"
#include <errno.h>
#ifndef VMS
#include <sys/stat.h>
#else
#include <stat.h>
#define index strchr
#endif
#ifdef __STDC__
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#endif /* __STDC__ */
#include "news.h"
#include "error_hnds.h"
#include "mesg.h"
#include "xrn.h"
#include "dialogs.h"
#include "resources.h"
#include "newsrcfile.h"

#ifndef R_OK
#define R_OK            4       /* readable by caller */
#endif /* R_OK */

extern int yyparse();

char *NewsrcFile;    /* newsrc file name    */
FILE *Newsrcfp;      /* newsrc file pointer */
char *optionsLine;   /*`options' line       */
struct newsgroup **Newsrc;   /* sequence list for .newsrc file */
struct stat fbuf;

#define OKAY  1
#define FATAL 0


static void
reportWriteError(name)
char *name;
{
    mesgPane(XRN_SERIOUS,
	 "Cannot write the .newsrc file (%s): %s", name,
	 errmsg(errno));
}

/*
 * copy newsrc file to .oldnewsrc file
 */
int
copyNewsrcFile(old, save)
char *old;   /* name of file to save */
char *save;  /* name of file to save to */
{
    FILE *orig, *new;
    char buf[BUFSIZ];
    char *newFile;
    int num_read;

    if ((orig = fopen(old, "r")) == NULL) {
	mesgPane(XRN_SERIOUS,
		 "Cannot open the .newsrc file (%s) for copying: %s", old,
		 errmsg(errno));
	return FATAL;
    }

    if ((newFile = utNameExpand(save)) == NIL(char)) {
	mesgPane(XRN_SERIOUS,
		 "Cannot expand the .newsrc save file name (%s)", save);
	return FATAL;
    }

    if (STREQ(newFile, "")) {
	 mesgPane(XRN_SERIOUS, "hmmm.... file name is \"\"\n");
	 return FATAL;
    }

    /* if .oldnewsrc is a link to .newsrc we could have trouble, so unlink it */
    (void) unlink(newFile);
    
    if ((new = fopen(newFile, "w")) == NULL) {
	mesgPane(XRN_SERIOUS, "Cannot open the .newsrc save file (%s): %s",
		 save, errmsg(errno));
	return FATAL;
    }

    while (num_read = fread(buf, sizeof(char), BUFSIZ, orig)) {
	 if (! fwrite(buf, sizeof(char), num_read, new)) {
	      mesgPane(XRN_SERIOUS,
		       "Error writing to .newsrc save file (%s): %s",
		       save, errmsg(errno));
	      return FATAL;
	 }
    }

    (void) fclose(orig);
    if (fclose(new) == EOF) {
	 mesgPane(XRN_SERIOUS, "Error writing to .newsrc save file (%s): %s",
		  save, errmsg(errno));
	 return FATAL;
    }

    return OKAY;
}

/*
 * read, parse, and process the .newsrc file
 *
 *   returns: 0 for fatal error, non-zero for okay
 *
 */
int
readnewsrc(newsrcfile, savenewsrcfile)
char *newsrcfile;
char *savenewsrcfile;
{
    struct stat buf;
    char *name;
    char *nntp;
    int nameLth;
#ifdef VMS
    int nntp_len;
    char *nntp_end;
#endif
    
    /* create the Newsrc array structure */
    Newsrc = ARRAYALLOC(struct newsgroup *, ActiveGroupsCount);

    optionsLine = NIL(char);

    if ((NewsrcFile = utNameExpand(newsrcfile)) == NIL(char)) {
	mesgPane(XRN_SERIOUS,
		 "Cannot expand the .newsrc file name (%s)", newsrcfile);
	return FATAL;
    }

    /* check for .newsrc-NNTPSERVER */

    if (app_resources.nntpServer) {
	nntp = app_resources.nntpServer;
    } else {
        nntp = getenv("NNTPSERVER");
    }

    if (nntp) {
#ifdef VMS
	if (nntp_end = index(nntp,':')) {  /* Look for the : in the name */
	    nntp_len = nntp_end-nntp;	   /* Get the length of the first
					      part */
	} else nntp_len = strlen(nntp);	   /* If no colon, use the whole
					      string */
	nameLth = strlen(NewsrcFile) + nntp_len;
#else
        nameLth = strlen(NewsrcFile) + strlen(nntp);
#endif
        name = ARRAYALLOC(char, nameLth + 20);
        (void) strcpy(name, NewsrcFile);
        (void) strcat(name, "-");
#ifdef VMS
	(void) strncat(name, nntp, nntp_len);
#else
        (void) strcat(name, nntp);
#endif
      
        if (access(name, R_OK) != 0) {
	    NewsrcFile = (char *) XtNewString(NewsrcFile);
	} else {
	    NewsrcFile = (char *) XtNewString(name);
        }
	FREE(name);
    } else {
        NewsrcFile = (char *) XtNewString(NewsrcFile);
    }

    if (access(NewsrcFile, R_OK) != 0) {
	if (errno != ENOENT) {
	    mesgPane(XRN_SERIOUS, "Cannot read newsrc file (%s): %s",
		     NewsrcFile, errmsg(errno));
	    return FATAL;
	}
	mesgPane(XRN_INFO, "Creating a newsrc (%s) file for you", NewsrcFile);
	if ((Newsrcfp = fopen(NewsrcFile, "w")) == NULL) {
	    mesgPane(XRN_SERIOUS, "Cannot create a newsrc file (%s): %s",
		     NewsrcFile, errmsg(errno));
	    return(FATAL);
	}
	(void) fprintf(Newsrcfp, "news.announce.newusers:\n");
#if !defined(VMS) && !defined(sco)
	(void) fstat((int) fileno(Newsrcfp), &fbuf);
	(void) fchmod((int) fileno(Newsrcfp), fbuf.st_mode);
#endif
	(void) fclose(Newsrcfp);
    }

    if (stat(NewsrcFile, &buf) == -1) {
	mesgPane(XRN_SERIOUS, "Cannot stat %s: %s", NewsrcFile, errmsg(errno));
	return FATAL;
    }

    if (buf.st_size == 0) {
	mesgPane(XRN_SERIOUS, "%s is zero length, aborting", NewsrcFile);
	return FATAL;
    }

    if ((Newsrcfp = fopen(NewsrcFile, "r")) == NULL) {
	mesgPane(XRN_SERIOUS,
		 "Cannot open the newsrc file for reading, %s: %s",
		 NewsrcFile, errmsg(errno));
	return FATAL;
    }

    if (yyparse() != 0) {
	mesgPane(XRN_SERIOUS,
		 "Cannot parse the newsrc file (%s) -- error on line %ld",
		 NewsrcFile, MaxGroupNumber + 1);
	return FATAL;
    }

#if !defined(VMS) && !defined(sco)
    (void) fstat((int) fileno(Newsrcfp), &fbuf);
    (void) fchmod((int) fileno(Newsrcfp), fbuf.st_mode);
#endif

    (void) fclose(Newsrcfp);
    if (!copyNewsrcFile(NewsrcFile, savenewsrcfile))
	 return FATAL;

    Newsrcfp = NIL(FILE);

    return(OKAY);
}

int
ngEntryFprintf(newsrcfp, newsgroup)
FILE *newsrcfp;
struct newsgroup *newsgroup;
{
    int first = 1;
    struct list *item;

    /* process the .newsrc line */

    for (item = newsgroup->nglist; item != NIL(struct list); item = item->next) {
	if (first)
	    first = 0;
	else
	    if (fprintf(newsrcfp, ",") == EOF) {
		return 0;
	    }

	switch (item->type) {
	    case SINGLE:
	    if (fprintf(newsrcfp, "%d", item->contents.single) == EOF) {
		return 0;
	    }
	    break;

	    case RANGE:
	    if (item->contents.range.start != item->contents.range.end) {
		if (fprintf(newsrcfp, "%d-%d",
				       item->contents.range.start,
				       item->contents.range.end) == EOF) {
		    return 0;
		}
	    } else {
		if (fprintf(newsrcfp, "%d", item->contents.range.start) == EOF) {
		    return 0;
		}
	    }
	    break;
	}
    }
    return 1;
}

/*
 * write out an up to date copy of the .newsrc file
 *
 *   returns: 0 for fatal error, non-zero for okay
 *
 */
int
updatenewsrc()
{
    ng_num indx;

    static FILE *newsrcfp;       /* file pointer for the newsc file      */
    static struct stat lastStat; /* last stat done on the file           */
    struct stat currentStat;     /* current stat                         */
    static int done = 0;
    static char tempfile[4096];

    if (!done) {
        (void) stat(NewsrcFile, &lastStat);
	/* must be in the same filesystem so `rename' will work */
#ifndef VMS
	(void) sprintf(tempfile, "%s.temp", NewsrcFile);
#else
	(void) sprintf(tempfile, "%s-temp", NewsrcFile);
#endif /* VMS */
	done = 1;
    }

    (void) stat(NewsrcFile, &currentStat);

    if (currentStat.st_mtime > lastStat.st_mtime) {
	if (ConfirmationBox(TopLevel, ".newsrc file updated by another program, continue?") == XRN_CB_ABORT) {
	    ehNoUpdateExitXRN();
	}
    }

    if ((newsrcfp = fopen(tempfile, "w")) == NULL) {
	mesgPane(XRN_SERIOUS,
		 "Cannot open the .newsrc temporary file (%s): %s", tempfile,
		 errmsg(errno));
	return(FATAL);
    }
    /* set access mode of the temp file to be the same as the original file */
    chmod(tempfile, currentStat.st_mode);

    /*
     * handle outputing the options line
     */
    if (optionsLine != NIL(char)) {
	if (fprintf(newsrcfp, "%s\n", optionsLine) == EOF) {
	    reportWriteError(NewsrcFile);
	    return(FATAL);
	}
    }

    for (indx = 0; indx < MaxGroupNumber; indx++) {
	struct newsgroup *newsgroup = Newsrc[indx];
	int nocomma = 1, inrange = 1;
	art_num lastread = 1, j;

	if (IS_ALIAS(newsgroup) && IS_NOENTRY(newsgroup)) {
	    continue;
	}
	if (fprintf(newsrcfp, "%s%c", newsgroup->name,
	       (IS_SUBSCRIBED(newsgroup) ? ':' : '!')) == EOF) {
	    reportWriteError(NewsrcFile);
	    return(FATAL);
	}

	if (newsgroup->last == 0) {
	    if (fprintf(newsrcfp, "\n") == EOF) {
		reportWriteError(NewsrcFile);
		return(FATAL);
	    }
	    continue;
	}

	if (newsgroup->artStatus == NIL(struct artStat)) {
	    if (newsgroup->nglist) {
		if (fprintf(newsrcfp, " ") == EOF) {
		    reportWriteError(NewsrcFile);
		    return(FATAL);
		}
		if (!ngEntryFprintf(newsrcfp, newsgroup)) {
		    reportWriteError(NewsrcFile);
		    return(FATAL);
		}
		if (fprintf(newsrcfp, "\n") == EOF) {
		    reportWriteError(NewsrcFile);
		    return(FATAL);
		}
	    } else {
		if (fprintf(newsrcfp, " 1-%d\n", newsgroup->last) == EOF) {
		    reportWriteError(NewsrcFile);
		    return(FATAL);
		}
	    }
	    continue;
	}
	
	if (newsgroup->last >= newsgroup->first) {

	    for (j = newsgroup->first; j <= newsgroup->last; j++) {
		if (inrange && IS_UNREAD(newsgroup->artStatus[INDEX(j)]) &&
		    !IS_UNAVAIL(newsgroup->artStatus[INDEX(j)])) {
		    if (lastread == j - 1) {
			if (fprintf(newsrcfp, "%c%ld",
			       (nocomma ? ' ' : ','), lastread) == EOF) {
			    reportWriteError(NewsrcFile);
			    return(FATAL);
			}
			nocomma = 0;
		    } else {
			if ((j - 1) > 0) {
			    if (fprintf(newsrcfp, "%c%ld-%ld",
				   (nocomma ? ' ' : ','), lastread,
				   j - 1) == EOF) {
				reportWriteError(NewsrcFile);
				return(FATAL);
			    }
			    nocomma = 0;
			}
		    }

		    inrange = 0;
		} else if (!inrange && IS_READ(newsgroup->artStatus[INDEX(j)])) {
		    inrange = 1;
		    lastread = j;
		}
	    }
	    
	    if (inrange) {
		if (lastread == newsgroup->last) {
		    if (fprintf(newsrcfp, "%c%ld",
			   (nocomma ? ' ' : ','), lastread) == EOF) {
			reportWriteError(NewsrcFile);
			return(FATAL);
		    }
		} else {
		    if (fprintf(newsrcfp, "%c%ld-%ld",
			   (nocomma ? ' ' : ','), lastread,
			   newsgroup->last) == EOF) {
			reportWriteError(NewsrcFile);
			return(FATAL);
		    }
		}
	    }
	} else {
	    if (newsgroup->last > 1) {
		if (fprintf(newsrcfp, " 1-%ld", newsgroup->last) == EOF) {
		    reportWriteError(NewsrcFile);
		    return(FATAL);
		}
	    }
	}
	    
	if (fprintf(newsrcfp, "\n") == EOF) {
	    reportWriteError(NewsrcFile);
	    return(FATAL);
	}
    }
#if !defined(VMS) && !defined(sco)
    (void) fchmod((int) fileno(newsrcfp), fbuf.st_mode);
#endif

    if (fclose(newsrcfp) == EOF) {
	reportWriteError(NewsrcFile);
	return(FATAL);
    }

#if defined(ISC_TCP) || defined(VMS)
    /* the following added to fix a bug in ISC TCP/IP rename() - jrh@dell.dell.com (James Howard) */
    if (unlink(NewsrcFile) != 0) {
	mesgPane(XRN_SERIOUS, "Error unlinking %s: %s", NewsrcFile,
		 errmsg(errno));
    }
#endif /* ISC_TCP or VMS */

    if (rename(tempfile, NewsrcFile) != 0) {
	mesgPane(XRN_SERIOUS, "Rename of %s to %s failed: %s", tempfile,
		       NewsrcFile, errmsg(errno));
	return(FATAL);
    }

    (void) stat(NewsrcFile, &lastStat);

    return(OKAY);
}
