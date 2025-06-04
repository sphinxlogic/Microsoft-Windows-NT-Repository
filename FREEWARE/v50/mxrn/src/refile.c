
#if !defined(lint) && !defined(SABER)
static char XRNrcsid[] = "$Header: /net/objy27/wrld/mnt11/ricks/src/master/xrn/refile.c,v 1.6 1993/01/11 02:15:17 ricks Exp $";
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

/* MH and RMAIL refiling: 
 *
 *   MH: Bob Ellison <ellison@sei.cmu.edu>
 *   RMAIL:  Michael Thomas <mike@gordian.com>
 */

#include "config.h"
#include <stdio.h>
#include "utils.h"
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef SYSV
#include <fcntl.h>
#else
#include <sys/file.h>
#endif
#if defined(_XOPEN_SOURCE)
#include <stdlib.h>
#else
    char *getenv();
#endif
#if defined(_OSF_SOURCE)
#include <string.h>
#else
#if defined(ultrix)
    char *index();
#endif
#endif
#include <errno.h>

#include "xrn.h"
#include "dialogs.h"
#include "error_hnds.h"
#include "mesg.h"
#include "resources.h"

extern char *strpbrk();

#ifndef S_ISDIR
#define S_ISDIR(mode) (((mode) & S_IFMT) == S_IFDIR)
#endif

#if defined(hpux) || (defined(sony) && defined(SYSV))		    /* ggh */

/* horrible strcasecmp */

int strcasecmp(const char *str1,const char *str2)
{
    char    tmp1[1024], tmp2[1024];
    int     i,result;

    for (i = 0; str1[i] != '\0'; i++) {
	if (isupper(str1[i])) {
	    tmp1[i] = tolower(str1[i]);
	} else {
	    tmp1[i] = str1[i];
	}
    }
    tmp1[i] = '\0';

    for (i = 0; str2[i] != '\0'; i++) {
	if (isupper(str2[i])) {
	    tmp2[i] = tolower(str2[i]);
	} else {
	    tmp2[i] = str2[i];
	}
    }
    tmp2[i] = '\0';

    return strcmp(tmp1, tmp2);
}
#endif


/* replacement for MH routine */
static char * m_find(p)
char *p;
{
    FILE *F;
    char *home, *mhprofile;
    char fullname [100];
    static char buf[512];
    char *q,*s;
    char *getenv();

    mhprofile = getenv("MH");
    if (!mhprofile) {
	home = getenv("HOME");
	if (!home) {
	    return 0;
	}
	(void) sprintf(fullname, "%s/.mh_profile", home);
    }
    else	/* either an absolute pathname, or relative to current dir */
	(void) strcpy(fullname, mhprofile);

    F = fopen(fullname, "r");
    if(F == NULL) {
	return 0;
    }
    while (fgets(buf, 512, F) != NULL) {
	q = index(buf,':');
        if(q != 0) {
	    *q = '\0';
	    s = ++q;
	    if (strcasecmp(buf, p) == 0) {
	        q += strspn(q," \t");
	        s = strpbrk(q," \n\t");
	        if(s) {
		    *s = '\0';
		}
	        (void) fclose(F);
	        return q;
	    }
        }
    }
    (void) fclose(F);
    return 0;
}


int MHrefile(folder, arttext)
char *folder;
char *arttext;
{
    char *p,*q;
    char  *userpath;
    char *getenv();
    char *m_find();
    char tmp[512];
    char fullpath[512];
    char fullpath2[512];
    char newfolders[512];
    char msg[512];
    struct stat st;
    char *artfile = NULL;
    FILE *fpsave;
   
    p = getenv("HOME");
    if(p == 0) {
	return 0;
    }
    userpath = m_find("path");
    if(userpath == 0) {
	(void) strcpy(fullpath, p);
    } else if (userpath[0] == '/') {
	(void) strcpy(fullpath, userpath);
    } else {
	if (userpath[0] == '/')
	    (void) strcpy(fullpath, userpath);
	else
	    (void) sprintf(fullpath, "%s/%s", p, userpath);
    }
    if((stat(fullpath,&st) == -1) || !S_ISDIR(st.st_mode)) {
	mesgPane(XRN_SERIOUS, "No Mail directory `%s'", fullpath);      
	return 0;
    }
    (void) sprintf(fullpath2, "%s/%s", fullpath, (folder+1));
    if (stat(fullpath2,&st) == -1 && errno == ENOENT) {
	(void) sprintf(msg,"No such folder - %s - Create it?",fullpath2);
	if (ConfirmationBox(TopLevel, msg)  == XRN_CB_ABORT) {
	    return 0;
	}
	(void) strcpy(newfolders, (folder+1));
	q = strtok(newfolders, "/");
	while (q) {
	    (void) strcat(fullpath, "/");
	    (void) strcat(fullpath, q);
	    if(stat(fullpath, &st) == -1) {
	        if(errno == ENOENT){
		    mkdir(fullpath, 0777);
	        } else {
		    mesgPane(XRN_SERIOUS, 
		      "Can not create folder `%s. Errno: %d'", fullpath, errno);
		    return 0;
	        }
	     } else if (!S_ISDIR(st.st_mode)) {
	        mesgPane(XRN_SERIOUS, 
	 	    "Can not create folder.  Not a directory `%s'", fullpath);
	        return 0;
	    }
	    q = strtok(0, "/");
        }
    } else if (!S_ISDIR(st.st_mode)) {
        mesgPane(XRN_SERIOUS, "Not a folder %s'", fullpath2);
        return 0;
    }
    artfile = (char *) utTempnam(p,"XRN");
    if ((fpsave = fopen(artfile, "w")) == NULL) {
	(void) sprintf(error_buffer, "Cannot create temp file: %s",
			errmsg(errno));
	mesgPane(XRN_SERIOUS, error_buffer);
	free(artfile);
	return 0;
    }

    if (fputs(arttext, fpsave) == EOF) {
	(void) sprintf(error_buffer, "Cannot write temp file: %s",
			errmsg(errno));
	mesgPane(XRN_SERIOUS, error_buffer);
	free(artfile);
	return 0;
    }
    (void) fclose(fpsave);

    if (stat(artfile, &st) == -1) {
	(void) sprintf(error_buffer, "Cannot stat temp file: %s",
			errmsg(errno));
	mesgPane(XRN_SERIOUS, error_buffer);
	free(artfile);
        return 0;
    }

    if (app_resources.mhPath)
	(void) sprintf(tmp, "%s/refile -f %s %s", app_resources.mhPath,
		       artfile, folder);
    else
	(void) sprintf(tmp, "refile -f %s %s", artfile, folder);

    (void) system(tmp);
    unlink(artfile);
    free(artfile);
    return 1;
}

/* 
 * Refile an article in EMACS RMAIL format
 */

/*ARGSUSED*/
int RMAILrefile
#ifndef _NO_PROTO
(char *fullpath, char *folder, char *artfile, long pos)
#else
(fullpath, folder, artfile, pos)
char *fullpath, *folder, *artfile;
long pos;
#endif
{
    char msg[512];
    struct stat st;
    extern int errno;
    FILE * fp;
    int artfd, rv, n;

    if (stat(fullpath,&st) == -1 && errno == ENOENT) {
	(void) sprintf(msg,"No such folder - %s - Create it?",fullpath);
	if (ConfirmationBox (TopLevel, msg)  == XRN_CB_ABORT) {
	    return 0;
	}
	if ((fp = fopen (fullpath, "w")) == NULL) {
	    mesgPane(XRN_SERIOUS, 
		     "Can not create folder `%s. Errno: %d'", fullpath, errno);
	    return 0;
	}
	/* Produce the header */
	fprintf (fp, "BABYL OPTIONS:\n");
	fprintf (fp, "Version: 5\n");
	fprintf (fp, "Labels:\n");
	fprintf (fp, "Note: This is RMAIL file produced by XRN:\n");
	fprintf (fp, "Note: if your are seeing this it means the file\n");
	fprintf (fp, "Note: has no messages in it.\n\037");
    } else {
	if ((fp = fopen (fullpath, "a")) == NULL) {
	    mesgPane(XRN_SERIOUS, 
		     "Can not open folder `%s. Errno: %d'", fullpath, errno);
	    return 0;
	}
    }
    if ((artfd = open(artfile, 0)) < 0) {
	mesgPane(XRN_SERIOUS, "Cannot open the article temporary file: %s",
		 errmsg(errno));
	return(0);
    }
    /* Format the header */
    fprintf (fp, "\014\n1,,\n");
    /* insert from 0 to pos (from getarticle) for the header */
    n = 0;
    while ((rv = read (artfd, msg, n + 512 > pos ? pos - n : 512)) > 0) {
	if (! fwrite (msg, n+512 > pos ? pos - n : 512, 1, fp)) {
	    mesgPane(XRN_SERIOUS, "Cannot write RMAIL file: %s",
		     errmsg(errno));
	    break;
	}
	n += rv;
    }
    /* reseek start of file */
    lseek (artfd, (off_t) 0, 0);
    fprintf (fp, "\n*** EOOH ***\n");
    /* insert the article */
    while ((rv = read (artfd, msg, 512)) > 0)
	if (! fwrite (msg, rv, 1, fp)) {
	    mesgPane(XRN_SERIOUS, "Cannot write RMAIL file: %s",
		     errmsg(errno));
	    break;
	}
    close (artfd);
    /* insert the article end mark */
    fprintf (fp, "\037");
    fclose (fp);
    return 1;
}

