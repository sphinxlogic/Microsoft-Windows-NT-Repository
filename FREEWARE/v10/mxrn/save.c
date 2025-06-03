
#if !defined(lint) && !defined(SABER)
static char XRNrcsid[] = "$Header: /net/objy27/wrld/mnt11/ricks/src/master/xrn/save.c,v 1.6 1993/01/11 02:15:23 ricks Exp $";
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
 * save.c: routines for saving articles and sending articles to processes
 */

#include "copyright.h"
#include <ctype.h>
#include <stdio.h>
#include "config.h"
#include "utils.h"
#ifndef VMS
#include <sys/param.h>
#include <sys/stat.h>
#else
#define MAXPATHLEN 512
#define index strchr
#include <stat.h>
#ifdef __STDC__
#include <unixlib.h>
extern int time();
#endif
#endif /* VMS */
#include <errno.h>

#ifdef sco
#define MAXPATHLEN 512
#endif

#include "news.h"
#include "resources.h"
#include "dialogs.h"
#include "error_hnds.h"
#include "internals.h"
#include "server.h"
#include "mesg.h"
#include "save.h"
#include "xmisc.h"
#include "xrn.h"

extern int errno;
#ifndef VMS
extern int refile();
#endif

#define BUFFER_SIZE 1024


#ifndef VMS
/*
 * send the current article to 'command'
 *
 *   returns: the exit status of the command
 *
 */
static int
processArticle(command, artfile)
char *command;
char *artfile;
{
    FILE *process;
    extern FILE *popen();	/* sequent */
    char *artPtr;
    int c, status;

    if ((process = popen(command, "w")) == NULL) {
	mesgPane(XRN_SERIOUS, "can not start `%s'", command);
	return(0);
    }

    artPtr = artfile;
    while ((c = *artPtr) != '\0') {
	(void) putc((char) c, process);
	artPtr++;
    }

    status = pclose(process);

    return(status);
}
#endif /* VMS */

/*
 * make sure that the news directory exists before trying to update it
 *
 *   returns: 0 for failure, 1 for okay
 */
int
createNewsDir()
{
    static int done = 0;
    char *newdir;

    if (done) {
	return(1);
    }
    if ((newdir = utTildeExpand(app_resources.saveDir)) == NIL(char)) {
	mesgPane(XRN_SERIOUS, "Cannot create save directory `%s'",
		 app_resources.saveDir);
	return(0);
    }
    if ((mkdir(newdir, 0777) == -1) && (errno != EEXIST)) {
	mesgPane(XRN_SERIOUS, "Cannot create save directory `%s': %s",
		       app_resources.saveDir, errmsg(errno));
	return(0);
    }
    done = 1;
    if (app_resources.expandedSaveDir)
	XtFree(app_resources.expandedSaveDir);
    app_resources.expandedSaveDir = (char *) XtNewString(newdir);

    return(1);
}


#ifdef VMS
static void
VmsMakeDirName(dummy, savedir, Group)
    char *dummy;
    char *savedir;
    char *Group;
{
    int temp;
    (void) strcpy(dummy, savedir);	/* Copy save directory name */
    temp = strlen(dummy);		/* Fetch the length */
    if (dummy[temp-1] == ']') {		/* If a directory spec, */
        dummy[temp-1] = '.';		/*  Convert to a .  */
    } else {
	(void) strcat(dummy, "[.");	/* Else, start directory */
    }
    (void) strcat(dummy, Group);	/* Add the group name */
    (void) strcat(dummy, "]");		/* Now terminate the string */
}
#endif /* VMS */

#ifndef VMS
/*
 * expand a file name for 'saving' a file
 *
 *   returns: the name of the file or NIL(char) if the filename is bad
 *            (i.e. ~user/xx where 'user' is not a valid user name)
 *            the area returned is static
 *
 */
static char *
buildFileName(filename, savedir, group)
char *filename;    /* file name, possibly with a '~' */
char *savedir;     /* save directory                 */
char *group;       /* name of the news group         */
{
    char Group[GROUP_NAME_SIZE];
#ifdef aiws
    static char dummy[MAXPATH];
#else
    static char dummy[MAXPATHLEN];
#endif /* aiws */

    /* Make a local copy of the group name for modification */
    (void) strncpy(Group, group, sizeof(Group));
    /* upcase the first letter of the group name (same as 'rn') */
    if (islower(Group[0])) {
	Group[0] = toupper(Group[0]);
    }

    if ((filename == NIL(char)) || (*filename == '\0')) {
	if (app_resources.saveMode & ONEDIR_SAVE){
	    (void) sprintf(dummy, "%s/%s", savedir, Group);
	} else {
	    /* use "saveDir/group" */
	    (void) sprintf(dummy, "%s/%s", savedir, group);
	    (void) mkdir(utTildeExpand(dummy), 0777);
	    (void) strcat(dummy, "/");
	    (void) strcat(dummy, Group);
	}
	return(utTildeExpand(dummy));
    }

    if ((filename[0] == '/') || (filename[0] == '~')) {
	return(utTildeExpand(filename));
    }

    if (app_resources.saveMode & ONEDIR_SAVE) {
	(void) sprintf(dummy, "%s/%s", savedir, filename);
    } else {
	/* use "saveDir/group/filename" */
	(void) sprintf(dummy, "%s/%s", savedir, group);
	(void) mkdir(utTildeExpand(dummy), 0777);
	(void) strcat(dummy, "/");
	(void) strcat(dummy, filename);
    }
    return(utTildeExpand(dummy));
}
#else /* VMS */
static char *
buildFileName(filename, savedir, group)
char *filename;    /* file name, possibly with a '~' */
char *savedir;     /* save directory                 */
char *group;       /* name of the news group         */
/*
 * expand a file name for 'saving' a file
 *
 *   returns: the name of the file or NIL(char) if the filename is bad
 *            (i.e. ~user/xx where 'user' is not a valid user name)
 *            the area returned is static
 *
 */
{
    char Group[GROUP_NAME_SIZE];
    static char dummy[MAXPATHLEN];
    char *dot;	   /* Pointer to dots to convert to underscores */

    /* First, use the generic group to filename conversion routine and
       convert the group name to a filename */
    (void) utGroupToVmsFilename(Group, group);

    /* If we are in ONEDIR_SAVE mode, find the first underscore character
       and convert it (back) to a dot.  This is done for compatibility with
       VNEWS. */
    if (app_resources.saveMode & ONEDIR_SAVE) {
	if (dot = index(Group, '_')) {
	    *dot = '.';
	}
    }

    if ((filename == NIL(char)) || (*filename == '\0')) {
	if (app_resources.saveMode & ONEDIR_SAVE){
	    (void) sprintf(dummy, "%s%s", savedir, Group);
	} else {
	    /* use "saveDir/group" */
	    VmsMakeDirName(dummy, savedir, Group);
	    (void) mkdir(utTildeExpand(dummy), 0777);
	    (void) strcat(dummy, Group);
	    if (!(app_resources.saveMode & ONEDIR_SAVE)) {
		(void) strcat(dummy, ".ART");
	    }
	}
	return(utTildeExpand(dummy));
    }

    if ((index(filename, ':')) || (index(filename, '['))
		|| (index(filename, '<'))) {
	return(utTildeExpand(filename));
    }

    if (app_resources.saveMode & ONEDIR_SAVE) {
	(void) sprintf(dummy, "%s%s", savedir, filename);
    } else {
	/* use "saveDir/group/filename" */
	VmsMakeDirName(dummy, savedir, Group);
	(void) mkdir(utTildeExpand(dummy), 0777);
	(void) strcat(dummy, filename);
    }
    return(utTildeExpand(dummy));
}
#endif /* VMS */

int
saveArticleByNumber(filename, art)
char *filename;
art_num art;
{
    return saveArticle(filename, CurrentGroup, art);
}


int
saveRangeOfArticles(filenameTemplate, start, end)
char *filenameTemplate;
art_num start;
art_num end;
/*
 * save the a range of articles from start to end,
 *
 * filenameTemplate is a sprintf compatible string
 *
 *   examples:  "mg.%d", "| save-it"
 *
 *     %d will be filled in with the article number
 *
 *   returns: 1 for OKAY, 0 for FAILURE
 *
 */
{
    struct newsgroup *newsgroup = CurrentGroup;
    /* art_num save_current = newsgroup->current; */
    art_num i;
    char buffer[1024];
    int status;

    for (i = start; i <= end; i++) {
	(void) sprintf(buffer, filenameTemplate, i);
	if (!(status = saveArticle(buffer, newsgroup, i)))
	    break;
    }
    return(status);
}


#define XRNsave_OVERWRITE 1
#define XRNsave_APPEND 2
#define XRNsave_ABORT 3
static Widget OverBox = (Widget) 0;	/* for overwrite dialog */
static int OverVal;
static Boolean useSavedValue = False;

void
resetSaveFlag()
{
    useSavedValue = False;
}

static void
overwriteHandler(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
{
    OverVal = (int) client_data;
    if (OverVal == XRNsave_OVERWRITE || OverVal == XRNsave_APPEND) {
	useSavedValue = True;
    }
    return;
}

static int overwriteOK()
{
    XEvent ev;
    static struct DialogArg args[] = {
	{"Overwrite", overwriteHandler, (caddr_t) XRNsave_OVERWRITE},
	{"Append",    overwriteHandler, (caddr_t) XRNsave_APPEND},
	{"Abort",     overwriteHandler, (caddr_t) XRNsave_ABORT},
    };

    if (useSavedValue)
	return OverVal;

    if (OverBox == (Widget) 0) {
	OverBox = CreateDialog(TopLevel,
		"Overwrite Confirm",
		"File already exists. Overwrite or append?",
		DIALOG_NOTEXT, args, XtNumber(args));
    }
    OverVal = -1;
    PopUpDialog(OverBox);
    for (;;) {
	XtNextEvent(&ev);
	(void) XtDispatchEvent(&ev);
	if (OverVal != -1) {
	    PopDownDialog(OverBox);
	    return(OverVal);
	}
    }
}
int
saveArticle(filename, newsgroup, art)
char *filename;
struct newsgroup *newsgroup;
art_num art;
{
    char timeString[BUFFER_SIZE];
    char fromString[BUFFER_SIZE];
    extern char *ctime();
    int error = 0;
    time_t clockTime;
    int pos;
    char *artfile, *fullName;
    char *artPtr, *strPtr, *endPtr, saveEnd;
    FILE *fpsave;
#ifdef XLATE
    int xlation;
#endif
    int rotation;
    Boolean needRefetch = False;
    char mode[2];
    Boolean appending;
    struct stat buf;
    char *defDir;
#ifdef VMS
    char *p;
    Boolean decnet = False;
#endif
    char rname[200];
    char *type;
    XrmValue value;

#ifdef MOTIF
    sprintf(rname, "mxrn.%s.saveDir", newsgroup->name);
#else
    sprintf(rname, "dxrn.%s.saveDir", newsgroup->name);
#endif

    if (XrmGetResource(XtDatabase(XtDisplay(TopLevel)),
		rname, "SaveDir", &type, &value)) {
	defDir = value.addr;
    } else {
	defDir = app_resources.saveDir;
    }

    if ((filename != NIL(char)) && (*filename != '\0')) {
	filename = utTrimSpaces(filename);
    }

    /* get the FULL article */

    rotation = (IS_ROTATED(newsgroup->artStatus[INDEX(art)]) ? ROTATED : NOT_ROTATED);
#ifdef XLATE
    xlation = (IS_XLATED(newsgroup->artStatus[INDEX(art)]) ? XLATED : NOT_XLATED);
#endif
/*
 * Try to get the article out of the cache
 * but only if --
 * Unrotated and
 *   No strip/leave headers or
 *   Headers not being saved
 */
    needRefetch = False;
    if (rotation == ROTATED)
	needRefetch = True;
#ifdef XLATE
    if (xlation == XLATED)
	needRefetch = True;
#endif
    if ((app_resources.saveMode & HEADERS_SAVE) == HEADERS_SAVE) {
	if (app_resources.leaveHeaders != NULL ||
	    app_resources.stripHeaders != NULL) {
	    needRefetch = True;
	}
    }
    if (needRefetch) {
#ifdef XLATE
	artfile = getarticle(art, &pos, FULL_HEADER, rotation, xlation);
#else
	artfile = getarticle(art, &pos, FULL_HEADER, rotation);
#endif
    } else {
        if ((artfile = getText(art)) == NULL) {
#ifdef XLATE
	    artfile = getarticle(art, &pos, FULL_HEADER, rotation, xlation);
#else
	    artfile = getarticle(art, &pos, FULL_HEADER, rotation);
#endif
	} else {
	    artfile = (char *) XtNewString(artfile);
	    for (pos = 0, strPtr = artfile; *strPtr; pos++, strPtr++) {
		if (artfile[pos] == '\n' && artfile[pos+1] == '\n') {
		    pos = pos + 2;
		    break;
		}
	    }
	}
    }

    if (artfile == NULL) {
	mesgPane(XRN_SERIOUS, "Could not get article %d to save",art);
        return(0);
    }
    
#ifndef VMS
    /* 
     * check a few special cases before actually saving the article
     * to a plain text file:
     *
     * - pipe it to a command
     * - file it to an mh folder
     * - file it to an RMAIL folder
     */
    if ((filename != NIL(char)) && (filename[0] == '|')) {
	int status;
	(void) sprintf(error_buffer, "Piping article %ld into command `%s'...     ",
		       art, &filename[1]);
	infoNow(error_buffer);
    	status = processArticle(utTrimSpaces(&filename[1]), artfile);
	FREE(artfile);
	if (status) {
	    (void) sprintf(error_buffer, "`%s' exited with status %d",
				&filename[1], status);
	} else {
	    (void) strcpy(&error_buffer[strlen(error_buffer) - 4], "done");
	}
	info(error_buffer);
	return(status == 0);
    }

    if ((filename != NIL(char)) && (filename[0] == '+')) {
	int status = MHrefile(filename, artfile);
	FREE(artfile);
	(void) sprintf(error_buffer, "MH refile to folder %s done", filename);
	infoNow(error_buffer);
	return(status);
    }
#endif

    /* XXX not quite right, don't want to try to create it if not used... */
    if (!createNewsDir()) {
	FREE(artfile);
	return(0);
    }
    
    if ((fullName = buildFileName(filename, defDir, newsgroup->name)) == NIL(char)) {
	mesgPane(XRN_SERIOUS, "Cannot figure out file name `%s': %s",
		filename, errmsg(errno));
	FREE(artfile);
	return(0);
    }


#ifndef VMS
    if (filename != NIL(char) && filename [0] == '@') {
	int status;
	if ((fullName = buildFileName(filename+1, app_resources.saveDir,
				      newsgroup->name)) == NIL(char)) {
	    mesgPane(XRN_SERIOUS, "Cannot figure out file name `%s'",
			filename+1);
	    (void) unlink(artfile);
	    FREE(artfile);
	    return(0);
	}
	status = RMAILrefile(fullName, filename+1, artfile, pos);
	(void) sprintf(error_buffer, "RMAIL refile to folder %s done",
			filename+1);
	infoNow(error_buffer);
	(void) unlink(artfile);
	FREE(artfile);
	return(status);
    }
#endif

#ifdef VMS
    for (p = fullName; *p != '\0'; p++) {
	if (*p == ':' && *(p+1) == ':') {
	    decnet = True;
	}
    }
#endif
    if (stat(fullName, &buf) == 0) {
	if (app_resources.confirmMode & ART_SAVE) {
	    switch (overwriteOK()) {
		case XRNsave_ABORT:
		return(0);

		case XRNsave_OVERWRITE:
		(void) strcpy(mode, "w");
		appending = 0;
		busyCursor();
		break;
	    
		case XRNsave_APPEND:
		(void) strcpy(mode, "a");
		appending = 1;
		busyCursor();
		break;
	    }
	} else {
	    appending = 1;
	    (void) strcpy(mode, "a");
	}
    } else {
	appending = 0;
	(void) strcpy(mode, "a");
    }
    
#ifdef VMS
    /*
     * Always do append over DECnet
     */

    if (decnet) (void) strcpy(mode, "a");
#endif

    if ((fpsave = fopen(fullName, mode)) == NULL) {
	if (appending) {
	    (void) sprintf(error_buffer, "Cannot append to file `%s': %s",
		fullName, errmsg(errno));
	} else {
	    (void) sprintf(error_buffer, "Cannot create file `%s': %s",
		fullName, errmsg(errno));
	}
	mesgPane(XRN_SERIOUS, error_buffer);
	FREE(artfile);
	return(0);
    }

    if (!appending) {
	(void) sprintf(error_buffer, "Saving article %ld in file `%s'...     ",
			   art, fullName);
    } else {
	(void) sprintf(error_buffer, "Appending article %ld to file `%s'...     ",
		       art, fullName);
	if ((app_resources.saveMode & MAILBOX_SAVE) != MAILBOX_SAVE) {
#ifdef VMS
	    if (!decnet) {
		fprintf(fpsave, "\014\n");
	    }
#else
	    fprintf(fpsave,"\014\n");
#endif
	}
    }	
     
    infoNow(error_buffer);

    artPtr = artfile;
    if ((app_resources.saveMode & MAILBOX_SAVE) == MAILBOX_SAVE) {
	int found = 0;

	strcpy(fromString, "Unknown");
	/*
	 * Temporarily terminate at the end of the header so the
	 * scans below don't have to read the entire article.
	 */
	endPtr = artPtr + pos;
	saveEnd = *endPtr;
	*endPtr = '\0';
	(void) time(&clockTime);
	(void) strcpy(timeString, ctime(&clockTime));
	timeString[strlen(timeString) - 1] = '\0';  /* get rid of the newline */

	/*
	 * Scan the header lines, looking for 'Reply-To', 'From', and 'Path'
	 * headers, looking for a usable 'From' header to be fabricated.
	 */
        artPtr = artfile;
	while (artPtr && *artPtr != '\0') {
	    if (STREQN(artPtr, "Reply-To:", 9)) {
		found = 1;
		if (strPtr = index(artPtr, '\n'))
		    *strPtr = '\0';
		(void) sscanf(artPtr + 10, "%s", fromString);
		if (strPtr) *strPtr = '\n';
		break;
	    }
	    artPtr = index(artPtr, '\n');
	    if (artPtr) artPtr++;
	}
	if (!found) {
	    artPtr = artfile;
	    while (artPtr && *artPtr != '\0') {
		if (STREQN(artPtr, "From:", 5)) {
		    found = 1;
		    if (strPtr = index(artPtr, '\n'))
			*strPtr = '\0';
		    (void) sscanf(artPtr + 6, "%s", fromString);
		    if (strPtr) *strPtr = '\n';
		    break;
		}
		artPtr = index(artPtr, '\n');
		if (artPtr) artPtr++;
	    }
	}
	if (!found) {
	    artPtr = artfile;
	    while (artPtr && *artPtr != '\0') {
		if (STREQN(artPtr, "Path:", 5)) {
		    found = 1;
		    if (strPtr = index(artPtr, '\n'))
			*strPtr = '\0';
		    (void) sscanf(artPtr + 6, "%s", fromString);
		    if (strPtr) *strPtr = '\n';
		    break;
		}
		artPtr = index(artPtr, '\n');
		if (artPtr) artPtr++;
	    }
	}
	if (fprintf(fpsave, "From %s %s\n", fromString, timeString) == EOF) {
	    error++;
	    goto finished;
	}
        *endPtr = saveEnd;
    }

    if ((app_resources.saveMode & MAILBOX_SAVE) == MAILBOX_SAVE) {
	if (fprintf(fpsave, "Article: %ld of %s\n",
			art, newsgroup->name) == EOF) {;
	    error++;
	    goto finished;
	}
    } else {
	if (fprintf(fpsave, "Article %ld of %s:\n",
		art, newsgroup->name) == EOF) {;
	    error++;
	    goto finished;
	}
    }

    artPtr = artfile;
    if ((app_resources.saveMode & HEADERS_SAVE) != HEADERS_SAVE) {
	artPtr += pos;
    }

    while (*artPtr != '\0') {
	strPtr = index(artPtr, '\n');
	if (strPtr) *strPtr = '\0';
	if (fputs(artPtr, fpsave) == EOF) {
	    error++;
	    goto finished;
	}
	if (fprintf(fpsave, "\n") == EOF) {
	    error++;
	    goto finished;
	}
	artPtr = artPtr + strlen(artPtr) + 1;
    }

    if (fprintf(fpsave, "\n\n") == EOF) {
	error++;
	goto finished;
    }
finished:
    if (fclose(fpsave) == EOF) {
	error++;
    }

    FREE(artfile);

    (void) strcpy(&error_buffer[utStrlen(error_buffer) - 4], 
	error ? "aborted" : "done");
    if (error) {
	info(error_buffer);
	(void) sprintf(error_buffer, "Error writing to file `%s': %s",
			fullName, errmsg(errno));
	mesgPane(XRN_SERIOUS, error_buffer);
	return 0;
    } else {
	info(error_buffer);
	SET_SAVED(newsgroup->artStatus[INDEX(art)]);
	useSavedValue = True;
	OverVal = XRNsave_APPEND;
	return 1;
    }
}

/*
 * save the current article in 'filename' (if it begins with '|', send it to
 *   a process)
 *
 *   returns: 1 for OKAY, 0 for FAILURE
 *
 */
int
saveCurrentArticle(filename)
char *filename;
{
    struct newsgroup *newsgroup = CurrentGroup;
    char formattedName[1000];

    sprintf(formattedName, filename, newsgroup->current);
    return saveArticle(formattedName, newsgroup, newsgroup->current);
}
