#if !defined(lint) && !defined(SABER)
static char XRNrcsid[] = "$Header: /net/objy27/wrld/mnt11/ricks/src/master/xrn/compose.c,v 1.9 1993/02/04 18:22:12 ricks Exp $";
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
 * compose.c: routines for composing messages and articles
 */

#include "copyright.h"

#include <ctype.h>
#include <stdio.h>
#include <time.h>
#include "config.h"
#include "utils.h"
#include <unistd.h>
#include <socket.h>

#if defined(sun) 
#if defined(sparc) && !defined(SOLARIS)
#include <vfork.h>
/*
 * Unfortunately, it's necessary to declare fclose on the Sun because
 * stdio.h in the standard Sun C library doesn't, and we need to
 * assign fclose to a function pointer variable below.
 */
extern int fclose _ARGUMENTS((FILE *));
#else /* sun & ! solaris */
#if !defined(SVR4_0) && !defined(hpux)
extern int vfork();
#else /* SVR4 */
extern pid_t vfork(void);
#endif /* SVR4 */
#endif /* sparc & !solaris */
#endif /* sun */

#ifndef i386
extern int getdtablesize();
#endif

#ifndef VMS
#include <sys/wait.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <pwd.h>
#else /* VMS */
#include <stat.h>
#include <file.h>
#define index strchr
#define rindex strrchr
#include <lib$routines.h>
#include <mail$routines.h>
#include <starlet.h>
extern void addTimeOut();
extern void removeTimeOut();
extern char *localHostName();
#endif

#include <signal.h>
#ifdef RESOLVER
#ifndef VMS
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#endif	/* VMS */
#endif	/* RESOLVER */

#if defined(aiws) || defined(DGUX)
struct passwd *getpwuid();
struct passwd *getpwnam();
#endif

#ifndef VMS
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#else
#include <types.h>
#include <decw$include/StringDefs.h>
#include <decw$include/Intrinsic.h>
#include <decw$include/Shell.h>
#endif
#ifdef MOTIF
#include <Xm/PanedW.h>
#include <Xm/TextP.h>
#include <Xm/RowColumn.h>
#include <Xm/PushB.h>
#include <Xm/Label.h>
#include <Xm/Frame.h>
#if (XmVERSION == 1) && (XmREVISION == 0)
#define MOTIF_V10
#endif
extern void PaneSetMinMax _ARGUMENTS((Widget, int, int));
#else
#ifndef DwtNmenuExtendLastRow
#define DwtNmenuExtendLastRow "menuExtendLastRow"
#endif
#ifndef VMS
#include <X11/DECwDwtWidgetProg.h>
#include "Pane.h"
#else
#include <decw$include/DECwDwtWidgetProg.h>
#include "Pane.h"
#endif
#endif

extern char *getinfofromfile _ARGUMENTS((char * ));
#include "avl.h"
#include "xthelper.h"
#include "resources.h"
#include "dialogs.h"
#include "news.h"
#include "server.h"
#include "mesg.h"
#include "internals.h"
#include "xrn.h"
#include "xmisc.h"
#include "error_hnds.h"
#include "butdefs.h"

#ifndef R_OK
#define F_OK            0       /* does file exist */
#define X_OK            1       /* is it executable by caller */
#define W_OK            2       /* writable by caller */
#define R_OK            4       /* readable by caller */
#endif /* R_OK */

/* entire pane */
Widget ComposeTopLevel = (Widget) 0;
Boolean ComposeActive = False;
/* text window */
Widget ComposeText = (Widget) 0;
/* buttons */
static Widget AbortButton;
static Widget SendButton;
static Widget SaveButton;
static Widget IncludeFileButton;
static Widget IncludeArticleButton;
static Widget IncludeCurrentButton;
static Widget WrapTextButton;
static Widget Rot13Button;
static int InsertionPoint = 0;
static int EditorPosition = 0;
Widget HeaderText = (Widget) 0;
#ifdef MOTIF
static Widget headerFrame = (Widget) 0;
static Widget textFrame = (Widget) 0;
#endif

static char *CompositionString = NIL(char);
static char *HeaderString = NIL(char);
static char *TempString = NIL(char);
static int CompositionSize = 0;
static int editMessage();

#define BUFFER_SIZE 1024

#define POST     0
#define FOLLOWUP 1
#define REPLY    2
#define FORWARD  3
#define GRIPE    4
#define FOLLOWUPREPLY 5
static int PostingMode = POST;
static char *PostingModeStrings[] =
    { "post", "followup", "reply", "forward", "gripe", "followup-and-mail" };

static Boolean IncludeCurrentMode = False;

#ifdef VMS
static int spawnStat;
#endif

struct header {
    art_num article;
    char *artText;
    char *newsgroups;
    char *subject;
    char *messageId;
    char *followupTo;
    char *references;
    char *from;
    char *replyTo;
    char *distribution;
    char *keywords;

    char *user;
    char *real_user;
    char *fullname;
    char *host;
    char *real_host;
    char *path;		/* note: this is not used with InterNetNews */
    char *organization;

    char  *date; /* added to header structure....... */
};


/*
 * storage for the header information needed for building, checking,
 * and repairing the header.  Once this is created, the user can go
 * to another article or another group and still use the composition
 * window
 */

static struct header Header = {0, NIL(char), NIL(char), NIL(char), NIL(char),
				  NIL(char), NIL(char), NIL(char), NIL(char),
				  NIL(char), NIL(char), NIL(char), NIL(char),
				  NIL(char), NIL(char), NIL(char)};


#define HOST_NAME_SIZE 1024

static void
sizeCompose(newSize)
int newSize;
{
    newSize += 10000;
    if (newSize > CompositionSize) {
	if (CompositionString == NIL(char)) {
	    CompositionString = XtMalloc(newSize);
	    HeaderString = XtMalloc(newSize);
	    TempString= XtMalloc(newSize);
	} else {
	    CompositionString = XtRealloc(CompositionString, newSize);
	    HeaderString = XtRealloc(HeaderString, newSize);
	    TempString= XtRealloc(TempString, newSize);
	}
	CompositionSize = newSize;
    }
}

/*
 * get the header and other important information for an article
 *
 * if called with article equal to zero, will only set up the non-article
 * specific entries
 *
 */
static void
getHeader(article)
art_num article;
{
    struct newsgroup *newsgroup = CurrentGroup;
#ifndef VMS
    struct passwd *pwd;
#endif
    char host[HOST_NAME_SIZE], *ptr;
    char real_host[HOST_NAME_SIZE];
#ifdef INN
    char *inn_domain = (char *) GetFileConfigValue ("domain");
    char *inn_org;
#else
    char path[HOST_NAME_SIZE];
#endif

#ifndef VMS
#if defined(RESOLVER) && !defined(RETURN_HOST)
    struct hostent *hent;
#endif
#endif
    char buffer[10000];
    char *getenv();
    
    if (article > 0) {
	Header.article = article;
	xhdr(article, "newsgroups", &Header.newsgroups);
	xhdr(article, "subject", &Header.subject);
	xhdr(article, "message-id", &Header.messageId);
	xhdr(article, "followup-to", &Header.followupTo);
	xhdr(article, "references", &Header.references);
	xhdr(article, "from", &Header.from);
	xhdr(article, "reply-to", &Header.replyTo);
	xhdr(article, "distribution", &Header.distribution);
	xhdr(article, "keywords", &Header.keywords);
    } else {
	/* information for 'post' */
	if (newsgroup) {
	    Header.newsgroups = XtNewString(newsgroup->name);
	} else {
	    Header.newsgroups = XtNewString("");
	}
	Header.artText = NULL;
    }

    /*
     * since I'm lazy down below, I'm going to replace NIL pointers with ""
     */
    if (Header.newsgroups == NIL(char)) {
	Header.newsgroups = XtNewString("");
    }
    if (Header.subject == NIL(char)) {
	Header.subject = XtNewString("");
    }
    if (Header.messageId == NIL(char)) {
	Header.messageId = XtNewString("");
    }
    if (Header.followupTo == NIL(char)) {
	Header.followupTo = XtNewString("");
    }
    if (Header.references == NIL(char)) {
	Header.references = XtNewString("");
    }
    if (Header.from == NIL(char)) {
	Header.from = XtNewString("");
    }
    if (Header.replyTo == NIL(char)) {
	Header.replyTo = XtNewString("");
    }
    if (Header.distribution == NIL(char)) {
	Header.distribution = XtNewString("");
    }
    if (Header.keywords == NIL(char)) {
	Header.keywords = XtNewString("");
    }

    real_host[0] = 0;
    host[0] = 0;
#ifdef	UUCPNAME
    {
        FILE * uup;
    
        if ((uup = fopen(UUCPNAME, "r")) != NULL) {
    		char   *p;
    		char    xbuf[BUFSIZ];
    
    		while (fgets(xbuf, sizeof(xbuf), uup) != NULL) {
    		    if (*xbuf <= ' ' || *xbuf == '#')
    			continue;
    		    break;
    		}
    		if (p = index(xbuf, '\n'))
    		    *p = '\0';
    		(void) strncpy(real_host, xbuf, sizeof(real_host) - 1);
		strcpy(host, real_host);
    		(void) fclose(uup);
        }
    }
#endif
    if (real_host[0] == 0) {
#ifdef INN
	/* always let values in inn.cofnf take precedence */
	char *inn_fromhost = (char *) GetConfigValue("fromhost");
	if (inn_fromhost != NIL(char)) {
	    (void) strcpy(real_host, inn_fromhost);
	} else if (inn_domain != NIL(char)) {
	    (void) strcpy(real_host, inn_domain);
	} else {
#endif /* INN */
        if ((ptr = getenv("HIDDENHOST")) != NIL(char)) {
	    (void) strncpy(real_host, ptr, sizeof(real_host)-1);
        } else if ((ptr = getinfofromfile(HIDDEN_FILE)) != NULL) {
	    (void) strncpy(real_host, ptr, sizeof(real_host) - 1);
	} else {
#ifdef RETURN_HOST
	    (void) strcpy(real_host, RETURN_HOST);
#else /* Not RETURN_HOST */
#ifndef VMS
	    (void) gethostname(real_host, sizeof(real_host));
#ifdef RESOLVER
	    hent = gethostbyname(real_host);
	    if (hent != NULL) {
		(void) strcpy(real_host,hent->h_name);
	    }
#endif
#else
	    ptr = localHostName();
	    if (ptr != NIL(char)) {
		(void) strcpy(real_host, ptr);
	    }
	    ptr = index(real_host, '.');
	    if (ptr == NIL(char)) {
		ptr = index(real_host, ':');
		if (ptr != NIL(char)) {
		    *ptr = '\0';
		}
		(void) strcat(real_host, DOMAIN_NAME);
	    }
	    utDowncase(real_host);
#endif
#endif /* Not RETURN_HOST */
	}
#ifdef INN
	}
#endif	/* INN */
    }
    
    if (app_resources.hostName != NIL(char)){
	utLowerCase(host, app_resources.hostName, sizeof(host) - 1);
    } else {
	if ((ptr = getenv("HIDDENHOST")) != NIL(char)) {
	    (void) strncpy(host, ptr, sizeof(host)-1);
	} else {
	    strcpy(host, real_host);
	}
    }

#ifndef INN    
    if ((ptr = getenv("HIDDENPATH")) != NIL(char)) {
	(void) strncpy(path, ptr, sizeof(path)-1);
    } else if ((ptr = getinfofromfile(PATH_FILE)) != NULL) {
	(void) strncpy(path, ptr, sizeof(path)-1);
    } else {
	(void) strncpy(path, host, sizeof(path)-1);
    }
#endif /* INN */

    /* If the host name is not a full domain name, put in the domain */
    if (index (host, '.') == NIL(char)) {
        char   *domain;
    
        if ((domain = getenv ("DOMAIN")) != NIL (char)) {
	    (void) strcat(host, domain);
#ifdef INN
	} else if (inn_domain != NIL(char)) {
	    (void) strcat(host, inn_domain);
#endif /* INN */	    
	} else if ((domain = getinfofromfile(DOMAIN_FILE)) != NULL) {
	    (void) strcat(host, domain);
        } else {
	    (void) strcat (host, DOMAIN_NAME);
	}
    }

    Header.host = XtNewString(host);
    Header.real_host = XtNewString(real_host);
#ifndef INN    
    Header.path = XtNewString(path);
#endif /* INN */

#ifndef VMS
    if ((ptr = getinfofromfile(utTildeExpand("~/.organization"))) != NULL) {
	Header.organization = XtNewString(ptr);
    } else
#endif
    if (app_resources.organization != NIL(char)) {
	Header.organization = XtNewString(app_resources.organization);
#ifndef apollo
    } else if ((ptr = getenv ("ORGANIZATION")) != NIL(char)) {
#else
    } else if ((ptr = getenv ("NEWSORG")) != NIL(char)) {
#endif
	Header.organization = XtNewString(ptr);
#ifdef INN
    } else if ((inn_org = (char *) GetConfigValue("organization")) != NIL(char)) {
	Header.organization = XtNewString(inn_org);
#endif /* INN */    
#ifdef ORG_FILE
    } else if ((ptr = getinfofromfile(ORG_FILE)) != NULL) {
	Header.organization = XtNewString(ptr);
#endif /* ORG_FILE */
    } else {
	Header.organization = XtNewString(ORG_NAME);
    }
#ifndef VMS
    pwd = getpwuid(getuid());
    if (Header.user = getenv("USER")) {
	Header.user = XtNewString(Header.user);
    } else if (Header.user = pwd->pw_name) {
	Header.user = XtNewString(Header.user);
    } else {
	Header.user = XtNewString("");
    }
    if (Header.real_user = pwd->pw_name) {
	Header.real_user = XtNewString(Header.real_user);
    } else {
	Header.real_user = XtNewString("");
    }
    if (Header.fullname = getenv("FULLNAME")) {
	Header.fullname = XtNewString(Header.fullname);
    } else if (Header.fullname = pwd->pw_gecos) {
	Header.fullname = XtNewString(Header.fullname);
    } else {
	Header.fullname = XtNewString("");
    }
#else
    Header.user = XtNewString(getenv("USER"));
    utDowncase(Header.user);
    Header.real_user = XtNewString(Header.user);

    if (app_resources.personalName != NULL) {
	Header.fullname = XtNewString(app_resources.personalName);
    } else {
	if ((Header.fullname = getenv("FULLNAME")) == NIL (char))
	    Header.fullname = XtNewString("");
	else
	    Header.fullname = XtNewString(Header.fullname);
    }
#endif
    ptr = index(Header.fullname, ',');
    if (ptr != NIL(char)) {
	*ptr = '\0';
    }
    
    /* & expansion */
    ptr = index(Header.fullname, '&');
    if (ptr != NIL(char)) {
	char *p = buffer + (ptr - Header.fullname);

	buffer[0] = '\0';
	*ptr = '\0';
	(void) strcpy(buffer, Header.fullname);
	(void) strcat(buffer, Header.user);
	if (isascii(*p)) {
	    *p = toupper(*p);
	}
	ptr++;
	(void) strcat(buffer, ptr);
	FREE(Header.fullname);
	Header.fullname = XtNewString(buffer);
    }
    
    return;
}

/*
 * see if a field exists in the header that begins with `fieldName'
 *
 * returns 0 for no, non-zero for yes
 */
static int
fieldExists(string, fieldName)
char *string;
char *fieldName;
{
    char *ptr;

    for (ptr = string; (ptr != NIL(char)) && (*ptr != '\0'); ptr++) {
	if ((ptr[0] == '\n') && (ptr[1] == '\0')) {
	    /* end of message */
	    return(0);
	}
	if ((ptr[0] == '\n') && (ptr[1] == '\n')) {
	    /* end of header */
	    return(0);
	}

	/*
	 * see if the field name is in the string and at the beginning
	 * of the line and that the field name ends with a ':'
	 */
	if (((ptr == string) || (*(ptr-1) == '\n')) &&
	    STREQN(fieldName, ptr, utStrlen(fieldName)) &&
	    (ptr[utStrlen(fieldName)] == ':')) {
	    return(1);
	}
    }
    return(0);
}

/*
 * add a header field to a message.
 * this is a destructive operation.
 */
static void
addField(string, field)
char *string;
char *field;
{
    char buffer[10000];

    (void) strcpy(buffer, string);
    string[0] = '\0';
    (void) strcat(string, field);
    (void) strcat(string, buffer);

    return;
}    

/*
 * remove all fields from a header that begin with `fieldName'.
 * this is a destructive operation.
 */
static void
stripField(string, fieldName)
char *string;
char *fieldName;
{
    char *ptr, *nl;


    for (ptr = string; (ptr != NIL(char)) && (*ptr != '\0'); ptr = index(ptr, '\n')) {
	if (ptr != string) {
	    /* character after the newline */
	    ptr++;
	}
	
	if ((ptr[0] == '\n') && (ptr[1] == '\0')) {
	    /* end of message */
	    return;
	}
	if ((ptr[0] == '\n') && (ptr[1] == '\n')) {
	    /* end of header */
	    return;
	}

	/*
	 * make sure a ':' is at the end of the field name
	 */
	if (STREQN(fieldName, ptr, utStrlen(fieldName)) && (ptr[utStrlen(fieldName)] == ':')) {
	    nl = index(ptr, '\n');
	    (void) strcpy(ptr, ++nl);
	    if (*ptr == '\n') {
		return;
	    }
	    ptr--;
	}
    }
    return;
}

/*
 * remove fields from a header that begin with `fieldName'
 * and return the removed characters in `removed'.
 * this is a destructive operation.
 */
static void
returnField(string, fieldName, removed)
char *string;
char *fieldName;
char *removed;
{
    char *ptr, *nl, *copyPtr;

    *removed = '\0';
    for (ptr = string; (ptr != NIL(char)) && (*ptr != '\0');  ptr = index(++ptr, '\n')) {
	if (ptr != string) {
	    /* character after the newline */
	    ptr++;
	}
	
	if ((ptr[0] == '\n') && (ptr[1] == '\0')) {
	    /* end of message */
	    return;
	}
	if ((ptr[0] == '\n') && (ptr[1] == '\n')) {
	    /* end of header */
	    return;
	}

	/*
	 * make sure a ':' is at the end of the field name
	 */
	if (STREQN(fieldName, ptr, utStrlen(fieldName)) &&
	    (ptr[utStrlen(fieldName)] == ':')) {
	    nl = index(ptr, '\n');
	    copyPtr = ptr + utStrlen(fieldName) + 1;
	    while (*copyPtr == ' ')		/* strip leading spaces */
		copyPtr++;
	    for (; copyPtr < nl; copyPtr++)
	         *removed++ = *copyPtr;
	    *removed = '\0';
	    (void) strcpy(ptr, ++nl);
	    return;
	}
    }
    return;
}

static void
PopdownCompositionTopLevel()
{
    XtSetSensitive(SendButton, False);
    XtSetSensitive(SaveButton, False);
    XtSetSensitive(IncludeFileButton, False);
    XtSetSensitive(IncludeArticleButton, False);
    XtSetSensitive(IncludeCurrentButton, False);
    XtSetSensitive(WrapTextButton, False);
    XtSetSensitive(Rot13Button, False);
    if (ComposeTopLevel != (Widget) 0) {
	xthUnmapWidget(ComposeTopLevel);
	XtPopdown(ComposeTopLevel);
    }
    ComposeActive = False;
    return;
}

static void
freeHeader()
{
    if (PostingMode != GRIPE) {
	FREE(Header.newsgroups);
	FREE(Header.subject);
	FREE(Header.messageId);
	FREE(Header.followupTo);
	FREE(Header.references);
	FREE(Header.from);
	FREE(Header.replyTo);
	FREE(Header.distribution);
	FREE(Header.keywords);
	FREE(Header.user);
	FREE(Header.real_user);
	FREE(Header.fullname);
	FREE(Header.host);
	FREE(Header.real_host);
#ifndef INN	
	FREE(Header.path);
#endif /* INN */	
	FREE(Header.organization);
    }
    return;
}

/*
 * add a subject field to the header of a message.
 * deal with supressing multiple '[rR][eE]: ' strings
 */
static void
buildSubject(message)
char *message;
{
    if (STREQN(Header.subject, "Re: ", 4) ||
	STREQN(Header.subject, "RE: ", 4) ||
	STREQN(Header.subject, "re: ", 4)) {
	(void) strcat(message, "Subject: ");
    } else {
	(void) strcat(message, "Subject: Re: ");
    }
    (void) strcat(message, Header.subject);
    (void) strcat(message, "\n");

    return;
}

/*ARGSUSED*/
static void
compAbortFunction(widget, event, string, count)
Widget widget;
XEvent *event;
String *string;
Cardinal *count;
{
    char *ptr;
    PopdownCompositionTopLevel();
    freeHeader();
    switch (PostingMode) {
	case POST:
	case FOLLOWUP:
	case FOLLOWUPREPLY:
	    ptr = "Article";
	    break;
	default:
	    ptr = "Message";
	    break;
    }

    mesgPane(XRN_INFO, "%s aborted", ptr);

    return;
}

static void
saveMessage(fname)
char *fname;
{
    FILE *fp;
    char *file = utNameExpand(fname);
    time_t clock;
    char *msg;

    if (file == NIL(char)) {
	mesgPane(XRN_SERIOUS, "Cannot save the article/message (%s)",
		 errmsg(errno));
	return;
    }
    
    (void) sprintf(error_buffer, "Saving in %s", file);
    infoNow(error_buffer);
    
#ifndef VMS
    if ((fp = fopen(file, "a")) != NULL) {
#else
    if ((fp = fopen(file, "w")) != NULL) {
#endif
	/* Provide initial 'From' line (note ctime() provides a newline) */

	if (Header.user == NULL) getHeader((art_num) 0);
	(void) time(&clock);
	if (fprintf(fp, "From %s %s", Header.user, ctime(&clock)) == EOF) {
	    goto finished;
	}

	/* copy body of message, protecting any embedded 'From' lines */

	msg = HeaderString;
	while (*msg) {
	    if (STREQN(msg, "From ", 5)) {
		if (fputc('>', fp) == EOF) {
		    goto hdrfinished;
		}
	    }
	    while (*msg) {
		if (fputc(*msg, fp) == EOF) {
		    goto hdrfinished;
		}
		++msg;
		if (*(msg-1) == '\n') break;
	    }
	}

hdrfinished:
	
	if (fputs("\n\n", fp) == EOF) {
	    goto finished;
	}

	msg = CompositionString;
	while (*msg) {
	    if (STREQN(msg, "From ", 5)) {
		if (fputc('>', fp) == EOF) {
		    goto finished;
		}
	    }
	    while (*msg) {
		if (fputc(*msg, fp) == EOF) {
		    goto finished;
		}
		++msg;
		if (*(msg-1) == '\n') break;
	    }
	}
	
	/* ensure there is an empty line at the end */

	if (fputs("\n\n", fp) == EOF) {
	    goto finished;
	}

finished:
	(void) fclose(fp);

    } else {
	mesgPane(XRN_SERIOUS, "Can not save the article/message (%s)",
	 errmsg(errno));
    }
    return;
}

/*ARGSUSED*/
static void
compSaveFunction(widget, event, string, count)
Widget widget;
XEvent *event;
String *string;
Cardinal *count;
{
    char *textString;
    
#ifdef MOTIF
    textString = xmWrapTextWidget(ComposeText);
#else
    textString = DwtSTextGetString(ComposeText);
#endif
    sizeCompose(utStrlen(textString));
    strcpy(CompositionString, textString);
    XtFree(textString);
#ifdef MOTIF
    textString = XmTextGetString(HeaderText);
#else
    textString = DwtSTextGetString(HeaderText);
#endif
    strcpy(HeaderString, textString);
    XtFree(textString);
    saveMessage(app_resources.savePostings);
}

static void
saveDeadLetter(textString)
char	*textString;
{
    FILE *fp;

#ifndef VMS
    if ((fp = fopen(utNameExpand(app_resources.deadLetters), "a")) != NULL) {
#else
    if ((fp = fopen(utNameExpand(app_resources.deadLetters), "w")) != NULL) {
#endif
	(void) fwrite(textString ,sizeof(char), utStrlen(textString), fp);
	(void) putc('\n', fp);
	(void) putc('\n', fp);
	(void) fclose(fp);
    } else {
	mesgPane(XRN_SERIOUS, "Can not save the dead article/message (%s)",
		 errmsg(errno));
    }
    return;
}


/*
 * go through the string and look for fields that have not
 * been filled in and remove them
 */
/*ARGSUSED*/
static void
stripBlankFields(string)
char *string;
{
    char *ptr, *nl, *end;

    ptr = string + utStrlen(string) - 1;
/*
 * make sure it ends with a newline
 */
    if (*ptr != '\n') {
	ptr++;
	*ptr++ = '\n';
	*ptr++ = '\0';
    }
    for (ptr = string; (ptr != NIL(char)) && (*ptr != '\0'); ptr = index(ptr, '\n')) {
	if ((ptr[0] == '\n') && (ptr[1] == '\0')) {
	    ptr[1] = '\n';		/* put in the missing newline */
	    ptr[2] = '\0';
	    /* end of message */
	    return;
	}
	if ((ptr[0] == '\n') && (ptr[1] == '\n')) {
	    /* end of header */
	    return;
	}

	if (ptr != string) {
	    /* character after the newline */
	    ptr++;
	}
	
	/* get the line */
	if ((nl = index(ptr, '\n')) == NIL(char)) {
	    /* weird */
	    return;
	}

	if (strncmp(ptr, "-=-=", 4) == 0 || strncmp(ptr, "=-=-", 4) == 0) {
	    /* end of message banner */
	    *ptr++ = '\n';
	    strcpy(ptr, ++nl);
	    return;
	}
	/* ignore trailing spaces */
	end = nl;
	end--;
	for ( ; *end == ' '; end--) ;
	
	if ((*end == ':') && (end == index(ptr, ':'))) {
	    /* blank field */
	    (void) strcpy(ptr, ++nl);
	    if (*ptr == '\n') {
		return;
	    }
	    ptr--;
	}
    }
    return;
}    

/*
 * Remote the 'start of text' header
 */
static void
stripHeaderField(string)
char *string;
{
    char *ptr, *nl;

    ptr = string + utStrlen(string) - 1;
    for (ptr = string; (ptr != NIL(char)) && (*ptr != '\0'); ptr = index(ptr, '\n')) {
	if ((ptr[0] == '\n') && (ptr[1] == '\0')) {
	    ptr[1] = '\n';		/* put in the missing newline */
	    ptr[2] = '\0';
	    /* end of message */
	    break;
	}

	if (ptr != string) {
	    /* character after the newline */
	    ptr++;
	}
	
	/* get the line */
	if ((nl = index(ptr, '\n')) == NIL(char)) {
	    /* weird */
	    break;
	}

	if (strncmp(ptr, "-=-=", 4) == 0 || strncmp(ptr, "=-=-", 4) == 0) {
	    /* end of message banner */
	    *ptr++ = '\n';
	    strcpy(ptr, ++nl);
	    break;
	}
    }
    ptr = string + utStrlen(string) - 1;
    while (*ptr == '\n') ptr--;
    ptr++;
    strcpy(ptr, "\n\n");
    return;
}    

#ifdef VMS
#include <ssdef.h>
#include <descrip.h>
#include <maildef.h>
static Widget	mailErrorDialog = NULL;
static char	VMSmessage[255];
static char	TXTmessage[255];
static char	*VMSmessPtr;
static int	VMSmessSize;
static int
grabMessage(descr)
struct	dsc$descriptor_s *descr;
{
    char *p;
    short s;
    s = (short)descr->dsc$w_length;
    p = (char *)descr->dsc$a_pointer;
    strncpy(VMSmessPtr, p, s);    
    VMSmessSize += s;
    VMSmessage[VMSmessSize] = '\n';
    VMSmessSize++;
    VMSmessage[VMSmessSize] = '\0';
    VMSmessPtr += VMSmessSize;
    return 0;
}
static unsigned int
handleMailSignal(sig, mech)
int sig[];
int mech[];
{
    sig[0] = sig[0] - 2;
    VMSmessPtr = VMSmessage;
    VMSmessSize = 0;
    sys$putmsg(sig, grabMessage, 0, 0);
    sig[0] = sig[0] + 2;
    return SS$_CONTINUE;
}
static void
popDownMail(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
{
    PopDownDialog(mailErrorDialog);
    return;
}
static void
mailError()
{
    static struct DialogArg args[] = {
	{"Click To Continue", popDownMail, (caddr_t) -1},
    };

    (void) strcpy(TXTmessage,"Error sending mail:");
    (void) strcat(TXTmessage,VMSmessage);
    mailErrorDialog = CreateDialog(TopLevel, "Mail Error",
				   TXTmessage, DIALOG_NOTEXT,
				   args, XtNumber(args));
    PopUpDialog(mailErrorDialog);
    return;
}
#endif /* VMS */

static void
sendFollowupMail(str)
char *str;
{
    char *message = NULL;
    char pathFormatString[100];

    message = XtMalloc(strlen(str) + 1024);

    if (app_resources.replyPath != NIL(char)) {
	if (app_resources.ccForward) {
	    sprintf(pathFormatString, "To: %s\nCc: %s\n",
		app_resources.replyPath, Header.user);
	} else {
	    if (app_resources.cc == True) {
		sprintf(pathFormatString, "To: %s\nCc: \n",
			app_resources.replyPath);
	    } else {
		sprintf(pathFormatString, "To: %s\n",app_resources.replyPath);
	    }
	}
    } else {
	if (app_resources.ccForward) {
	    sprintf(pathFormatString, "To: %s\nCc: %s\n", "%s", Header.user);
	} else {
	    if (app_resources.cc == True) {
		strcpy(pathFormatString, "To: %s\nCc: \n");
	    } else {
		strcpy(pathFormatString, "To: %s\n");
	    }
	}
    }
    (void) sprintf(message, pathFormatString,
		   (*Header.replyTo != '\0') ? Header.replyTo : Header.from);

    if (app_resources.replyTo != NIL(char)) {
	 (void) strcat(message, "Reply-To: ");
	 (void) strcat(message, app_resources.replyTo);
	 (void) strcat(message, "\n");
    }

    buildSubject(message);
    strcat(message, "\n");
    strcat(message, str);
    switch (postArticle(message, XRN_MAIL)) {
	case POST_TRYAGAIN:
	case POST_FAILED:
	    XBell(XtDisplay(TopLevel), 50);
	    mesgPane(XRN_SERIOUS, "Could not send reply.");
	    break;
	case POST_OKAY:
	    mesgPane(XRN_INFO, "Mail Message Sent");
	    break;
    }
    if (message) XtFree(message);
}
/*ARGSUSED*/
static void
compSendFunction(widget, event, string, count)
Widget widget;
XEvent *event;
String *string;
Cardinal *count;
{
    char buffer[10000];
    char *textString;
    /*
     * I am loathe to use buffers that are 1024 bytes long for the old
     * from line, new from line and sender line, since they are almost
     * certainly going to be much shorter than this, but this seems to
     * be the way things are done elsewhere in this file, so I'll
     * stick with it.
     */
    char oldNgString[BUFFER_SIZE];
    char newNgString[BUFFER_SIZE];
    char oldFromString[BUFFER_SIZE], tempFromString[BUFFER_SIZE];
    char newFromString[BUFFER_SIZE];
    char senderString[BUFFER_SIZE];
    int mode, i,j, len, comma;
#if defined(INN)
    char subjString[BUFFER_SIZE];
#endif

    if (app_resources.editorCommand == NIL(char)) {
#ifdef MOTIF
	textString = xmWrapTextWidget(ComposeText);
#else
	textString = DwtSTextGetString(ComposeText);
#endif
	sizeCompose(utStrlen(textString));
	strcpy(CompositionString, textString);
	XtFree(textString);
#ifdef MOTIF
	textString = XmTextGetString(HeaderText);
#else
	textString = DwtSTextGetString(HeaderText);
#endif
	strcpy(HeaderString, textString);
	XtFree(textString);
	stripBlankFields(HeaderString);
	stripHeaderField(HeaderString);
    } else {
	strcpy(HeaderString, CompositionString);
	CompositionString[0] = '\0';
	stripBlankFields(HeaderString);
	stripHeaderField(HeaderString);
    }
    
    if ((PostingMode == POST) || (PostingMode == FOLLOWUP) ||
	(PostingMode == FOLLOWUPREPLY)) {
	mode = XRN_NEWS;
	if (app_resources.editorCommand == NIL(char)) {
	    if (!fieldExists(HeaderString, "Subject")) {
		if (PostingMode == POST) {
		    XBell(XtDisplay(TopLevel), 50);
		    (void) sprintf(buffer, "Subject: ********\n");
		    mesgPane(XRN_SERIOUS,
			 "The Subject field is missing in your message!");
		    addField(HeaderString, buffer);
#ifdef MOTIF
		    XmTextSetString(HeaderText, HeaderString);
#ifndef MOTIF_V10
		    XmTextSetCursorPosition(HeaderText, (XmTextPosition) 0);
#endif
		    XmTextSetInsertionPosition(HeaderText, (XmTextPosition) 0);
#else
		    DwtSTextSetString(HeaderText, HeaderString);
		    DwtTextSetInsertionPosition(HeaderText, 0);
#endif
		    return;
		}
		buffer[0] = '\0';
		buildSubject(buffer);
		addField(HeaderString, buffer);
	    }
#ifdef INN
	    /* Let's be more strict, since inews doesn't see empty headers.  */
	    returnField(HeaderString, "Subject", subjString);
	    if (subjString[0] == '\0') {
		XBell(XtDisplay(TopLevel), 0);
		mesgPane(XRN_SERIOUS, "The Subject field is empty in your message!");
		return;
	    }
	    sprintf(buffer, "Subject: %s\n", subjString);
	    addField(HeaderString, buffer);
#endif /* INN */	
	}
	if (PostingMode == FOLLOWUP || PostingMode == FOLLOWUPREPLY) {
	    if (!fieldExists(HeaderString, "References")) {
		(void) sprintf(buffer, "References: %s\n", Header.messageId);
		addField(HeaderString, buffer);
	    }
	}

#ifdef INEWS
	/*
	 * Extract any From: field currently in the message, and store it
	 * in the newFromString.
	 */
	returnField(HeaderString, "From", newFromString);
	/*
	 * This is the default that was displayed in the Composition pane
	 */
	(void) sprintf(oldFromString, "%s@%s (%s)",
			    Header.user, Header.host, Header.fullname);
	/*
	 * If we're using INEWS we pass on any From: header the user
	 * may have set explicitly.
	 */
	if (strcmp(oldFromString, newFromString)) {
	    (void) sprintf(buffer, "From: %s\n", newFromString);
	    addField(HeaderString, buffer);
	}
	/*
	 * Get rid of any Sender: field currently in the mesage --
	 * the Sender: field should not ever be inserted by the user.
	 */
	stripField("Sender:");
#else
	/*
	 * Strip any From: field currently in the message, and store
	 * it in oldFromString.
	 */
	returnField(HeaderString, "From", oldFromString);
	/*
	 * If there was no From: field in the message, create a
	 * template one.
	 */
	if (*oldFromString == '\0')
	     (void) sprintf(oldFromString, "%s@%s (%s)",
			    Header.user, Header.host, Header.fullname);
	/*
	 * Now add into the message either the From: field that was
	 * pulled out or the one that was just created.
	 */
	(void) sprintf(buffer, "From: %s\n", oldFromString);
	addField(HeaderString, buffer);
	/*
	 * Now figure out what the default From: field should look
	 * like, with the real username in it (in case the user has
	 * specified a different username in the USER environment
	 * variable).
	 */
	(void) sprintf(newFromString, "%s@%s (%s)",
		       Header.real_user, Header.real_host, Header.fullname);
	/*
	 * Get rid of any Sender: field currently in the message --
	 * the Sender: field should not ever be inserted by the user.
	 */
	returnField(HeaderString, "Sender", senderString);
	/*
	 * If the default From: field is different from what's
	 * actually in the message, then insert a Sender: field with
	 * the default information in it.
	 */
	if (strcmp(oldFromString, newFromString)) {
	     (void) sprintf(senderString, "Sender: %s@%s (%s)\n",
			    Header.real_user, Header.real_host,
			    Header.fullname);
	     addField(HeaderString, senderString);
	}
#endif /* INEWS */	

	if (!fieldExists(HeaderString, "Newsgroups")) {
	    (void) sprintf(buffer, "Newsgroups: %s\n", Header.newsgroups);
	    addField(HeaderString, buffer);
	} else {
	    /*
	     * fix up the Newsgroups: field - inews can not handle spaces
	     * between group names
	     */
	    returnField(HeaderString, "Newsgroups", oldNgString);
	    len = strlen(oldNgString);
	    j = 0;
	    comma = 0;
	    for (i = 0; i < len; i++) {
		if (comma && (oldNgString[i] == ' ')) continue;
		comma = 0;
		newNgString[j++] = oldNgString[i];
		if (oldNgString[i] == ',') {
		    comma = 1;
		}
	    }
	    newNgString[j] = '\0';
	    sprintf(oldNgString, "Newsgroups: %s\n", newNgString);
	    addField(HeaderString, oldNgString);
	}

#ifndef INN
	/* The inews in INN adds the Path: header for us, and has its own
	 * idea of policy.  We don't bother creating it, freeing it, or
	 * anything else in compose.c that has to do with it
	 */
	if (!fieldExists(HeaderString, "Path")) {
#if defined(INEWS) || defined(HIDE_PATH)
	    (void) sprintf(buffer, "Path: %s\n", Header.user);
#else
	    (void) sprintf(buffer, "Path: %s!%s\n", Header.path, Header.user);
#endif
	    addField(HeaderString, buffer);
	}
#else
	returnField(HeaderString, "Sender", senderString);
#endif /* INN */

/*	stripField("Message-ID:");  .............we need this !!! */
	stripField(HeaderString, "Relay-Version");
	stripField(HeaderString, "Posting-Version");

	(void) strcat (HeaderString, CompositionString);
    } else {
	(void) strcat (HeaderString, CompositionString);
	mode = XRN_MAIL;
    }

    switch (postArticle(HeaderString,mode)) {
	case POST_TRYAGAIN:
	    return;

	case POST_FAILED:
	    XBell(XtDisplay(TopLevel), 50);
	    mesgPane(XRN_SERIOUS, "Could not post, saving to %s",
		app_resources.deadLetters);
	    saveDeadLetter(HeaderString);
	    break;

	case POST_NOTALLOWED:
	    mesgPane(XRN_SERIOUS,
		"Posting not allowed from this machine, saving in %s",
		 app_resources.deadLetters);
	    break;
	    
	case POST_OKAY:
	    if (PostingMode == FOLLOWUPREPLY) {
		sendFollowupMail(HeaderString);
	    }
	    switch (mode) {
		case XRN_NEWS:
		    mesgPane(XRN_INFO, "Article Posted");
		    break;

		case XRN_MAIL:
		    mesgPane(XRN_INFO, "Mail Message Sent");
		    break;
	    }
	    break;
    }

    if (app_resources.editorCommand == NIL(char)) {
	PopdownCompositionTopLevel();
    }
    freeHeader();

    return;
}
#ifdef VMS
int
mailArticle(article)
char *article;
{
    struct _itemList {
	short item_size;
	short item_code;
	char* item_ptr;
	int   item_rtn_size;
	int   end_list;
    } itemList;
	 
    struct _ccList {
	short item1_size;
	short item1_code;
	char* item1_ptr;
	int   item1_rtn_size;
	short item2_size;
	short item2_code;
	char* item2_ptr;
	int   item2_rtn_size;
	int   end_list;
    } ccList;
	 
    FILE *fp;
    char toString[BUFFER_SIZE];
    char ccString[BUFFER_SIZE];
    char subjString[BUFFER_SIZE];
    char listFileName[512];
    char listName[512];
    int  listNameSize;
    int	context, status;
    char *textPtr, *nl;
    char *toElement, *tokPtr;
    char *p;
    char *ccElement;
    int mailCC = MAIL$_CC;
    char **wrappedLines;
    char **lines;
    long zero = 0;
    int lineCount;

    VAXC$ESTABLISH(handleMailSignal);
    sizeCompose(strlen(article) + 500);
    strcpy(HeaderString, article);
    returnField(HeaderString, "To", toString);

    if (strlen(toString) == 0) {
	XBell(XtDisplay(TopLevel), 0);
	mesgPane(XRN_SERIOUS, "You must enter a non-Null TO field");
	return POST_TRYAGAIN;
	}

    returnField(HeaderString, "Subject", subjString);
    if (strlen(subjString) == 0) {
	XBell(XtDisplay(TopLevel), 0);
	mesgPane(XRN_SERIOUS, "You must enter a non-Null subject");
	return POST_TRYAGAIN;
    }
    returnField(HeaderString,"Cc", ccString);
    context = 0;
    status = mail$send_begin(&context, &zero, &zero);
    if ((status&1) != 1) {
	mailError();
	return POST_TRYAGAIN;
    }
    if (index(toString,'"') == NULL) {
	for (p = toString; *p != '\0'; p++) {
	    *p = toupper(*p);
	}
    }
    if (index(ccString,'"') == NULL) {
	for (p = ccString; *p != '\0'; p++) {
	    *p = toupper(*p);
	}
    }
    itemList.item_code = MAIL$_SEND_TO_LINE;
    itemList.item_ptr = toString;
    itemList.item_size = strlen(toString);
    itemList.item_rtn_size = 0;
    itemList.end_list = 0;
    status = mail$send_add_attribute(&context, &itemList, &zero);
    if ((status&1) != 1) {
	mailError();
        mail$send_abort(&context, &zero, &zero);
	return POST_TRYAGAIN;
    }
    itemList.item_code = MAIL$_SEND_CC_LINE;
    itemList.item_ptr = ccString;
    itemList.item_size = strlen(ccString);
    itemList.item_rtn_size = 0;
    itemList.end_list = 0;
    status = mail$send_add_attribute(&context, &itemList, &zero);
    if ((status&1) != 1) {
	mailError();
        mail$send_abort(&context, &zero, &zero);
	return POST_TRYAGAIN;
    }
    tokPtr = toString;
    while ((toElement = strtok(tokPtr, ",")) != NULL) {
	itemList.item_code = MAIL$_SEND_USERNAME;
	if ((itemList.item_ptr = getenv(toElement)) == NULL) {
	    itemList.item_ptr = toElement;
	}
	itemList.item_size = strlen(itemList.item_ptr);
	for (p = itemList.item_ptr; *p == ' '; p++);
	if (*p == '@') {
	    p++;
	    if ((fp = fopen(p, "r","dna=.dis")) == NULL) {
		sprintf(VMSmessage, "Error opening distribution file %s",p);
		mailError();
		mail$send_abort(&context, &zero, &zero);
		return POST_TRYAGAIN;
	    } else {
		while (fgets(listName, 128, fp) != NULL) {
		    listNameSize = strlen(listName);
		    listNameSize--;			/* strip the newline */
		    listName[listNameSize] = '\0';
		    itemList.item_code = MAIL$_SEND_USERNAME;
		    if ((itemList.item_ptr = getenv(listName)) == NULL) {
			itemList.item_ptr = listName;
		    }
		    for (p = itemList.item_ptr; *p != '\0'; p++) {
			if (*p == '!')
			    *p = '\0';
			if (*p == '\t')
			    *p = ' ';
		    }
		    p = itemList.item_ptr;
		    if ((itemList.item_ptr = getenv(p)) == NULL) {
			itemList.item_ptr = p;
		    }
		    itemList.item_size = strlen(itemList.item_ptr);
		    itemList.item_rtn_size = 0;
		    itemList.end_list = 0;
		    status = mail$send_add_address(&context, &itemList, &zero);
		    if ((status&1) != 1) {
			fclose(fp);
			mailError();
			mail$send_abort(&context, &zero, &zero);
			return POST_TRYAGAIN;
		    }
		}
		fclose(fp);
	    }
	} else {
	    itemList.item_rtn_size = 0;
	    itemList.end_list = 0;
	    status = mail$send_add_address(&context, &itemList, &zero);
	    if ((status&1) != 1) {
		mailError();
		mail$send_abort(&context, &zero, &zero);
		return POST_TRYAGAIN;
	    }
	}
	tokPtr = NULL;
    }
    tokPtr = ccString;
    while ((toElement = strtok(tokPtr, ",")) != NULL) {
	ccList.item1_code = MAIL$_SEND_USERNAME;
	if ((ccList.item1_ptr = getenv(toElement)) == NULL) {
	    ccList.item1_ptr = toElement;
	}
	ccList.item1_size = strlen(ccList.item1_ptr);
	for (p = ccList.item1_ptr; *p == ' '; p++);
	if (*p == '@') {
	    p++;
	    if ((fp = fopen(p, "r","dna=.dis")) == NULL) {
		sprintf(VMSmessage, "Error opening distribution file %s",p);
		mailError();
		mail$send_abort(&context, &zero, &zero);
		return POST_TRYAGAIN;
	    } else {
		while (fgets(listName, 128, fp) != NULL) {
		    listNameSize = strlen(listName);
		    listNameSize--;			/* strip the newline */
		    listName[listNameSize] = '\0';
		    ccList.item1_code = MAIL$_SEND_USERNAME;
		    if ((ccList.item1_ptr = getenv(listName)) == NULL) {
			ccList.item1_ptr = listName;
		    }
		    for (p = ccList.item1_ptr; *p != '\0'; p++) {
			if (*p == '!')
			    *p = '\0';
			if (*p == '\t')
			    *p = ' ';
		    }
		    p = ccList.item1_ptr;
		    if ((ccList.item1_ptr = getenv(p)) == NULL) {
			ccList.item1_ptr = p;
		    }
		    ccList.item1_size = strlen(ccList.item1_ptr);
		    ccList.item1_rtn_size = 0;
		    ccList.item2_code = MAIL$_SEND_USERNAME_TYPE;
		    ccList.item2_ptr = (char *) &mailCC;
		    ccList.item2_size = 2;
		    ccList.item2_rtn_size = 0;
		    ccList.end_list = 0;
		    status = mail$send_add_address(&context, &ccList, &zero);
		    if ((status&1) != 1) {
			fclose(fp);
			mailError();
			mail$send_abort(&context, &zero, &zero);
			return POST_TRYAGAIN;
		    }
		}
		fclose(fp);
	    }
	} else {
	    ccList.item1_rtn_size = 0;
	    ccList.item2_code = MAIL$_SEND_USERNAME_TYPE;
	    ccList.item2_ptr = (char *) &mailCC;
	    ccList.item2_size = 2;
	    ccList.item2_rtn_size = 0;
	    ccList.end_list = 0;
	    status = mail$send_add_address(&context, &ccList, &zero);
	    if ((status&1) != 1) {
		mailError();
		mail$send_abort(&context, &zero, &zero);
		return POST_TRYAGAIN;
	    }
	}
	tokPtr = NULL;
    }
    itemList.item_code = MAIL$_SEND_SUBJECT;
    itemList.item_size = strlen(subjString);
    itemList.item_ptr = subjString;
    itemList.item_rtn_size = 0;
    itemList.end_list = 0;
    status = mail$send_add_attribute(&context, &itemList, &zero);
    if ((status&1) != 1) {
	mailError();
        mail$send_abort(&context, &zero, &zero);
	return POST_TRYAGAIN;
    }
/*
 * Iterate over the composition string, extracting records
 * delimited by newlines and sending each as a record
 *
 */
    textPtr = HeaderString;
    while (*textPtr == '\n')
	textPtr++;
    lines = wrappedLines = wrapText(textPtr, &lineCount);

    while (*lines) {
	itemList.item_code = MAIL$_SEND_RECORD;
	itemList.item_size = strlen(*lines);
	itemList.item_ptr = *lines;
	itemList.item_rtn_size = 0;
	itemList.end_list = 0;
	status = mail$send_add_bodypart(&context, &itemList, &zero);
	if ((status&1) != 1) {
	    mailError();
	    mail$send_abort(&context, &zero, &zero);
	    return POST_FAILED;
	}
	
	FREE(*lines);
	lines++;
    }
    FREE(wrappedLines);
    status = mail$send_message(&context, &zero, &zero);
    if ((status&1) != 1) {
	mailError();
        mail$send_end(&context, &zero, &zero);
	return POST_FAILED;
    }
    mail$send_end(&context, &zero, &zero);
    return POST_OKAY;
}
#endif

static void
includeArticleText()
{
    char line[256], *end;
    char *art, *art1;

    sizeCompose(utStrlen(CompositionString) + utStrlen(Header.artText));
    end = CompositionString + InsertionPoint;
    (void) strcpy(TempString, end);
    *end = '\0';

    if (Header.artText == NULL) {
	XBell(XtDisplay(TopLevel), 0);
	mesgPane(XRN_SERIOUS, "No article to include");
	return;
    }
    if (PostingMode == REPLY && !IncludeCurrentMode) {
       (void) sprintf(line, "In article %s you write:\n",
		Header.messageId );
    } else {
       (void) sprintf(line, "In article %s, %s writes:\n",
		   Header.messageId, Header.from);
    }

    (void) strcat(CompositionString, line);
    end = CompositionString + utStrlen(CompositionString);
    *end = '\0';

    /* copy the article */

    art = Header.artText;
    if (!app_resources.includeHeader) {
	for (art1 = art + 1; *art1 != '\0'; art++, art1++) {
	    if (*art == '\n' && *art1 == '\n') {
		art = ++art1;
		break;
	    }
	}
    }
	
    if (app_resources.includeSep == TRUE && PostingMode != FORWARD) {
        (void) strcat(end, app_resources.includePrefix);
	end += utStrlen(app_resources.includePrefix);
    }
    for (; *art != '\0'; art++) {
	*end++ = *art;
    	*end = '\0';
	if (*art == '\n')
	    if (app_resources.includeSep == TRUE && PostingMode != FORWARD) {
		(void) strcat(end, app_resources.includePrefix);
		end += utStrlen(app_resources.includePrefix);
	    }
    }
    *end++ = '\n';
    *end = '\0';
    /* copy back the text after the insertion point */
    (void) strcat(end, TempString);
    if (ComposeText != (Widget) 0) {
#ifdef MOTIF
	XmTextSetString(ComposeText,CompositionString);
#else
	DwtSTextSetString(ComposeText,CompositionString);
#endif
    }
    return;
}

/*ARGSUSED*/
static void
compIncludeArticleFunction(widget, event, string, count)
Widget widget;
XEvent *event;
String *string;
Cardinal *count;
{
    char *art;
#ifndef MOTIF
    Arg textPos[1];
#endif
    long beforeSize, afterSize;
    
#ifdef MOTIF
    art = xmWrapTextWidget(ComposeText);
#else
    art = DwtSTextGetString(ComposeText);
#endif
    sizeCompose(utStrlen(art));
    strcpy(CompositionString, art);
    XtFree(art);
#ifdef MOTIF
#ifdef MOTIF_V10
    InsertionPoint = XmTextGetInsertionPosition(ComposeText);
#else
    InsertionPoint = XmTextGetCursorPosition(ComposeText);
#endif
#else
    XtSetArg(textPos[0], DwtNinsertionPosition, (XtArgVal) &InsertionPoint);
    XtGetValues(ComposeText, textPos, XtNumber(textPos));
#endif

    beforeSize = utStrlen(CompositionString);
    includeArticleText();
    afterSize = utStrlen(CompositionString);

    InsertionPoint = InsertionPoint + afterSize - beforeSize;

#ifdef MOTIF
    XmTextSetString(ComposeText,CompositionString);
#ifndef MOTIF_V10
    XmTextSetCursorPosition(ComposeText, (XmTextPosition) InsertionPoint);
#endif
    XmTextSetInsertionPosition(ComposeText, (XmTextPosition) InsertionPoint);
#else
    DwtSTextSetString(ComposeText,CompositionString);
    XtSetArg(textPos[0], DwtNinsertionPosition,	(XtArgVal) InsertionPoint);
    XtSetValues(ComposeText, textPos, XtNumber(textPos));
#endif

    return;
}

/*ARGSUSED*/
static void
compIncludeCurrentFunction(widget, event, string, count)
Widget widget;
XEvent *event;
String *string;
Cardinal *count;
{
    struct newsgroup *newsgroup = CurrentGroup;

    if (newsgroup != NULL) {
	Header.artText = getText(newsgroup->current);
	getHeader(newsgroup->current);
    } else {
	Header.artText = NULL;
    }
    IncludeCurrentMode = True;
    compIncludeArticleFunction(widget, NULL, NULL, NULL);
    IncludeCurrentMode = False;
}

#define XRNinclude_ABORT          0
#define XRNinclude_DOIT           1

static Widget IncludeBox = (Widget) 0;  /* box for typing in the name of a file */

/*ARGSUSED*/
static void
includeHandler(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
/*
 * handler for the include box
 */
{
#ifndef MOTIF
    Arg textPos[1];
#endif
    char *end, *file;
    int i;
    FILE *filefp;
    struct stat fileStat;

    PopDownDialog(IncludeBox);

    if ((int) client_data != XRNinclude_ABORT) {
	end = CompositionString + InsertionPoint;
	(void) strcpy(TempString, end);
	*end = '\0';

	/* copy the file */
	file = XtNewString(GetDialogValue(IncludeBox));
	if ((stat(file, &fileStat) != -1) && 
	    ((filefp = fopen(file, "r")) != NULL)) {
	    sizeCompose(CompositionSize + utStrlen(TempString) + 
			fileStat.st_size + 10000);
	    end = CompositionString + InsertionPoint;
	    while ((i = getc(filefp)) != EOF) {
		*end = i;
		end++;
	    }
	    *end = '\0';
	    (void) fclose(filefp);
	} else {
	    mesgPane(XRN_SERIOUS, "Can not open `%s' (%s)",
		     file, errmsg(errno));
	}
	XtFree(file);
	/* new insertion point */
	InsertionPoint = end - CompositionString;

	/* copy back the text after the insertion point */
	(void) strcat(end, TempString);

#ifdef MOTIF
	XmTextSetString(ComposeText, CompositionString);
#ifndef MOTIF_V10
	XmTextSetCursorPosition(ComposeText, InsertionPoint);
#endif
	XmTextSetInsertionPosition(ComposeText, InsertionPoint);
#else
	DwtSTextSetString(ComposeText, CompositionString);
	XtSetArg(textPos[0], DwtNinsertionPosition,
	    (XtArgVal) utStrlen(CompositionString));
	XtSetValues(ComposeText, textPos, XtNumber(textPos));
#endif
    }

    return;
}    

/*ARGSUSED*/
static void
compIncludeFileFunction(widget, event, string, count)
Widget widget;
XEvent *event;
String *string;
Cardinal *count;
{
#ifndef MOTIF
    Arg textPos[1];
#endif
    char *art;
    static struct DialogArg args[] = {
	{"Include",	includeHandler, (caddr_t) XRNinclude_DOIT},
	{"Cancel",	includeHandler, (caddr_t) XRNinclude_ABORT},
    };

#ifdef MOTIF
    art = xmWrapTextWidget(ComposeText);
#else
    art = DwtSTextGetString(ComposeText);
#endif
    sizeCompose(utStrlen(art));
    strcpy(CompositionString, art);
    XtFree(art);
#ifdef MOTIF
#ifdef MOTIF_V10
    InsertionPoint = XmTextGetInsertionPosition(ComposeText);
#else
    InsertionPoint = XmTextGetCursorPosition(ComposeText);
#endif
#else
    XtSetArg(textPos[0], DwtNinsertionPosition, (XtArgVal) &InsertionPoint);
    XtGetValues(ComposeText, textPos, XtNumber(textPos));
#endif
    
    if (IncludeBox == (Widget) 0) {
	IncludeBox = CreateDialog(ComposeTopLevel,
				 "Include File Select",
				 "    File Name?    ",
				  DIALOG_TEXT, args, XtNumber(args));
    }
    PopUpDialog(IncludeBox);

    return;
}
/*ARGSUSED*/
static void
compWrapTextFunction(widget, event, string, count)
Widget widget;
XEvent *event;
String *string;
Cardinal *count;
{
    char *textString, *ptr;
    char **wrappedLines;
    char **lines;
    int size;
    int oldBreak, oldWrap;
#ifndef MOTIF
    Arg textPos[1];
#endif
    int lineCount;

    oldBreak = app_resources.breakLength;
    if (oldBreak == 0) app_resources.breakLength = 80;
    oldWrap = app_resources.lineLength;
    if (oldWrap == 0) app_resources.lineLength = 72;

#ifdef MOTIF
    textString = xmWrapTextWidget(ComposeText);
#ifdef MOTIF_V10
    InsertionPoint = XmTextGetInsertionPosition(ComposeText);
#else
    InsertionPoint = XmTextGetCursorPosition(ComposeText);
#endif
#else
    textString = DwtSTextGetString(ComposeText);
    XtSetArg(textPos[0], DwtNinsertionPosition, (XtArgVal) &InsertionPoint);
    XtGetValues(ComposeText, textPos, XtNumber(textPos));
#endif
    lines = wrappedLines = wrapText(textString, &lineCount);

    size = 0;
    while (*lines) {
	size += strlen(*lines) + 1;
	lines++;
    }
    sizeCompose(size);
    ptr = CompositionString;
    strcpy(CompositionString, textString);
    XtFree(textString);

    lines = wrappedLines;
    while (*lines) {
	strcpy(ptr, *lines);
	ptr += strlen(*lines);
	strcpy(ptr, "\n");
	ptr++;
	FREE(*lines);
	lines++;
    }
    FREE(wrappedLines);

#ifdef MOTIF
    XmTextSetString(ComposeText,CompositionString);
#ifndef MOTIF_V10
    XmTextSetCursorPosition(ComposeText, (XmTextPosition) InsertionPoint);
#endif
    XmTextSetInsertionPosition(ComposeText, (XmTextPosition) InsertionPoint);
#else
    DwtSTextSetString(ComposeText,CompositionString);
    XtSetArg(textPos[0], DwtNinsertionPosition,
	(XtArgVal) InsertionPoint);
    XtSetValues(ComposeText, textPos, XtNumber(textPos));
#endif
    app_resources.breakLength = oldBreak;
    app_resources.lineLength = oldWrap;
}

/*ARGSUSED*/
static void
compRot13Function(widget, event, string, count)
Widget widget;
XEvent *event;
String *string;
Cardinal *count;
{
    char *textString, *ptr;
#ifndef MOTIF
    Arg textPos[1];
#endif

#ifdef MOTIF
    textString = XmTextGetString(ComposeText);
#ifdef MOTIF_V10
    InsertionPoint = XmTextGetInsertionPosition(ComposeText);
#else
    InsertionPoint = XmTextGetCursorPosition(ComposeText);
#endif
#else
    textString = DwtSTextGetString(ComposeText);
    XtSetArg(textPos[0], DwtNinsertionPosition, (XtArgVal) &InsertionPoint);
    XtGetValues(ComposeText, textPos, XtNumber(textPos));
#endif
    for (ptr = textString; *ptr != '\0'; ptr++) {
	if (isalpha(*ptr)) {
	    if ((*ptr & 31) <= 13) {
		*ptr = *ptr + 13;
	    } else {
		*ptr = *ptr - 13;
	    }
	}
    }

#ifdef MOTIF
    XmTextSetString(ComposeText,textString);
#ifndef MOTIF_V10
    XmTextSetCursorPosition(ComposeText, (XmTextPosition) InsertionPoint);
#endif
    XmTextSetInsertionPosition(ComposeText, (XmTextPosition) InsertionPoint);
#else
    DwtSTextSetString(ComposeText,textString);
    XtSetArg(textPos[0], DwtNinsertionPosition,
	(XtArgVal) InsertionPoint);
    XtSetValues(ComposeText, textPos, XtNumber(textPos));
#endif
    XtFree(textString);
}

static struct stat originalBuf;
static char FileName[512];

/* Must reset ComposeActive here, unless we call editMessage() again - ggh */
/* ARGSUSED */
static Boolean
processMessage()
{
    FILE *filefp;
    struct stat buf;
    char *ptr, *msg_type, *confirm1, *confirm2;
    int mode;

    switch (PostingMode) {
	case POST:
	case FOLLOWUP:
	case FOLLOWUPREPLY:
	    msg_type = "article";
	    confirm1 = "Post the article?";
	    confirm2 = "Re-edit the article?";
	    break;
	default:
	    msg_type= "message";
	    confirm1 = "Send the message?";
	    confirm2 = "Re-edit the message?";
	    break;
    }
    if ((filefp = fopen(FileName, "r")) == NULL) {
	mesgPane(XRN_SERIOUS, "Cannot open the temp file (%s)", errmsg(errno));
	freeHeader();
	ComposeActive = False;					    /* ggh */
	return True;
    }

    if (fstat(fileno(filefp), &buf) == -1) {
	mesgPane(XRN_SERIOUS, "Cannot stat the temp file (%s)", errmsg(errno));
	(void) fclose(filefp);
	(void) unlink(FileName);
	freeHeader();
	ComposeActive = False;					    /* ggh */
	return True;
    }

    if (originalBuf.st_mtime == buf.st_mtime) {
	mesgPane(XRN_INFO, "No change, %s aborted.", msg_type);
	(void) fclose(filefp);
	(void) unlink(FileName);
	freeHeader();
	ComposeActive = False;					    /* ggh */
	return True;
    }

    if (buf.st_size == 0) {
	mesgPane(XRN_INFO, "Zero size, %s aborted", msg_type);
	(void) fclose(filefp);
	(void) unlink(FileName);
	freeHeader();
	ComposeActive = False;					    /* ggh */
	return True;
    }

    sizeCompose(buf.st_size+1);
    ptr = ARRAYALLOC(char, (buf.st_size + 100));
    (void) fread(ptr, sizeof(char), buf.st_size, filefp);
    ptr[buf.st_size] = '\0';
    (void) fclose(filefp);
    stripBlankFields(ptr);
    stripHeaderField(ptr);
    strcpy(HeaderString, ptr);

    /* pop up a confirm box */
    if (((app_resources.confirmMode & COMP_POST) == 0) &&
	ConfirmationBox(TopLevel, confirm1) == XRN_CB_ABORT) {
	if (ConfirmationBox(TopLevel, confirm2) == XRN_CB_ABORT) {
	    mesgPane(XRN_SERIOUS, "Aborting the %s, saving to %s",
		     msg_type, app_resources.deadLetters);
	    saveDeadLetter(ptr);
	    FREE(ptr);
	    (void) unlink(FileName);				    /* ggh */
	    freeHeader();
	    ComposeActive = False;				    /* ggh */
	} else {
	    FREE(ptr);
	    editMessage(True, NULL);
	}
	return True;
    }
	
    /* send to the proper processor */
    if ((PostingMode == GRIPE) ||
	(PostingMode == FORWARD) ||
	(PostingMode == REPLY)) {
	mode = XRN_MAIL;
    } else {
	mode = XRN_NEWS;
    }
    
    switch (postArticle(ptr, mode)) {
	case POST_FAILED:
	    XBell(XtDisplay(TopLevel), 0);
	    mesgPane(XRN_SERIOUS, "Could not send, saving in %s",
		 app_resources.deadLetters);
	    saveDeadLetter(ptr);
	    FREE(ptr);
	    (void) unlink(FileName);
	    freeHeader();
	    ComposeActive = False;
	    return True;
    
	case POST_NOTALLOWED:
	    mesgPane(XRN_SERIOUS,
		"Posting not allowed from this machine, saving in %s",
		     app_resources.deadLetters);
	    (void) unlink(FileName);
	    freeHeader();
	    ComposeActive = False;
	    break;
	
	case POST_TRYAGAIN:
	    if (ConfirmationBox(TopLevel, confirm2) == XRN_CB_ABORT) {
		mesgPane(XRN_SERIOUS, "Aborting the %s, saving to %s",
		     msg_type, app_resources.deadLetters);
		saveDeadLetter(ptr);
		(void) unlink(FileName);
		freeHeader();
		ComposeActive = False;
	    } else {
		if (editMessage(True, NULL) == -1) {
		    FREE(ptr);
		    (void) unlink(FileName);
		    freeHeader();
		    ComposeActive = False;
		    return True;
		}
	    }
	    break;
	
	case POST_OKAY:
	    if (PostingMode == FOLLOWUPREPLY) {
		sendFollowupMail(ptr);
	    }
	    switch (mode) {
		case XRN_NEWS:
		    mesgPane(XRN_INFO, "Article Posted");
		    (void) unlink(FileName);
		    freeHeader();
		    ComposeActive = False;
		    break;

		case XRN_MAIL:
		    mesgPane(XRN_INFO, "Mail Message Sent");
		    (void) unlink(FileName);
		    ComposeActive = False;
		    break;
	    }
	    break;
    }
    FREE(ptr);
    return True;
}


static int forkpid; 

#ifndef VMS
#ifdef __STDC__
static void
catch_sigchld(signo)
int signo;
{
    if (signo != SIGCHLD) {
	/* whoops! */
	return;
    }
    (void) signal(SIGCHLD, SIG_DFL);
    if (forkpid != wait(0)) {
	/* whoops! */
	return;
    }
    XtAddWorkProc((XtWorkProc)processMessage, NULL);
    /* Don't clobber file name - user might want to re-edit the file.  ggh */
    /* FileName[0] = '\0'; 					       ggh */
    return;
}
#else
static int
catch_sigchld(signo)
int signo;
{
    if (signo != SIGCHLD) {
	/* whoops! */
	return 1;
    }
    (void) signal(SIGCHLD, SIG_DFL);
    if (forkpid != wait(0)) {
	/* whoops! */
	return 1;
    }
    XtAddWorkProc((XtWorkProc)processMessage, NULL);
    /* Don't clobber file name - user might want to re-edit the file.  ggh */
    /* FileName[0] = '\0'; 					       ggh */
    return 1;
}
#endif /* __STDC__ */
#else /* VMS */
static void
spawnDone() {

    if ((spawnStat & 1) != 1) {
	mesgPane(XRN_SERIOUS, "Error return from editor command - %d",
		spawnStat);
    }
    XtAddWorkProc((XtWorkProc)processMessage, NULL);
}
#endif

/*
 * definition of the buttons
 */
extern int inCommand;

#ifndef MOTIF
#define XmNlabelString DwtNlabel
#define XmNactivateCallback DwtNactivateCallback
#define XmNborderWidth DwtNborderWidth
#endif

BUTTON(compAbort)
BUTTON(compSend)
BUTTON(compSave)
BUTTON(compIncludeArticle)
BUTTON(compIncludeCurrent)
BUTTON(compIncludeFile)
BUTTON(compWrapText)
BUTTON(compRot13)

static XtActionsRec ComposeActions[] = {
    {"composeAbort", (XtActionProc) compAbortAction},
    {"composeSend",  (XtActionProc) compSendAction},
    {"composeSave",   (XtActionProc) compSaveAction},
    {"composeIncludeCurrent", (XtActionProc) compIncludeCurrentAction},
    {"composeIncludeArticle", (XtActionProc) compIncludeArticleAction},
    {"composeIncludeFile", (XtActionProc) compIncludeFileAction},
    {"composeWrapText", (XtActionProc) compWrapTextAction},
    {"composeRot13", (XtActionProc) compRot13Action},
};

/* Used to fork editors when the widget editor is not being used. */

static int editMessage(reEdit, header)
Boolean reEdit;
char *header;
{
    char buffer[1024];
    FILE *filefp;
    int sysStat;
    int maxdesc;
#ifdef VMS
    struct dsc$descriptor_s cmd_desc;
    int one = 1;		/* cli$m_nowait */
#endif

    if (!reEdit) {
	(void) sprintf(buffer, "%sxrnXXXXXXX", app_resources.tmpDir);
	(void) mktemp(buffer);
	(void) sprintf(FileName, "%s.tmp", buffer);
	
	if ((filefp = fopen(FileName, "w")) == NULL) {
	    mesgPane(XRN_SERIOUS, "Can not open the temp file (%s)", errmsg(errno));
	    return(-1);
	}

	sizeCompose(utStrlen(header) + 100);
	(void) strcpy(CompositionString, HeaderString);
	(void) strcat(CompositionString, header);

	if ((PostingMode != FORWARD) && (PostingMode != POST) &&
	    (PostingMode != GRIPE)) {
	    includeArticleText();
	}	    
    
	(void) fwrite(CompositionString, sizeof(char), utStrlen(CompositionString), filefp);
	(void) fclose(filefp);
	/* Stat the file only at first edit, not at re-edits.	       ggh */
	if (stat(FileName, &originalBuf) == -1) {		    /* ggh */
	    mesgPane(XRN_SERIOUS, "Can not stat the temp file");    /* ggh */
	    return(-1);						    /* ggh */
	}							    /* ggh */
    }
	
    /*
     * app_resources.editorCommand is a sprintf'able string with a %s where the
     * file name should be placed.  The result should be a command that
     * handles all editing and windowing.
     *
     * Examples are:
     *
     *   emacsclient %s
     *   xterm -e vi %s
     *   xterm -e microEmacs %s
     *
     */
     (void) sprintf(buffer, app_resources.editorCommand, FileName);
#ifndef VMS
#ifdef VFORK_SUPPORTED
    if ((forkpid = vfork()) == 0) {
#else
    if ((forkpid = fork()) == 0) {
#endif
	int i;

#ifdef hpux
	maxdesc = _NFILE;
#else
#ifdef SVR4
#include <ulimit.h>
	maxdesc = ulimit(UL_GDESLIM);
#else
	maxdesc = getdtablesize();
#endif
#endif
	for (i = 3; i < maxdesc; i++) {
	    (void) close(i);
	}
	(void) execl("/bin/csh", "csh", "-f", "-c", buffer, 0);
	(void) fprintf(stderr, "execl of %s failed\n", buffer);
	(void) _exit(127);
    }
    if (forkpid < 0) {
	sprintf(error_buffer, "Can not execute editor (%s)", errmsg(errno));
	infoNow(error_buffer);
	/* ComposeActive = False; /* Done in composePane()	       ggh */
	unlink (FileName);	  /* instead of FileName[0] = '\0'     ggh */
	return -1;		  /* Tell composePane that we failed   ggh */
    } else {
#ifdef __STDC__
	signal(SIGCHLD, catch_sigchld);
#else
	signal(SIGCHLD, (SIG_PF0) catch_sigchld);
#endif
    }
#else
    cmd_desc.dsc$b_class = DSC$K_CLASS_S;
    cmd_desc.dsc$b_dtype = DSC$K_DTYPE_T;
    cmd_desc.dsc$w_length = strlen(buffer);
    cmd_desc.dsc$a_pointer = buffer;

    sysStat = lib$spawn(&cmd_desc,0,0,&one,0,0, &spawnStat,0,spawnDone);
    if ((sysStat & 1) != 1) {
	mesgPane(XRN_SERIOUS, "Error return from spawn - %d, aborting",
		sysStat);
	return(-1);
    }
#endif
    return 1;
}

/*
 * brings up a new vertical pane, not moded, but maintains
 * enough state so that the current group and/or current
 * article can be changed
 *
 * only one compose pane at a time
 *
 * the pane consists of 4 parts: title bar, header window,
 * scrollable text window,  button box
 *
 * seven functions:
 *    post article
 *    followup article
 *    reply to author
 *    include the text of the article (followup and reply)
 *    include a file
 *    send a gripe
 *    forward a message
 */
static int
composePane(titleString, header)
char *titleString;
char *header;
{
    Widget pane, buttonBox;
    int i;
    Arg paneArgs[15];
    Dimension paneWidth;
    static Arg layoutArgs[] = {			/* main window description */
						/* args set from geometry and */
	{XtNx,			(XtArgVal) 10},	/* must stay in x/y/w/h order */
	{XtNy, 			(XtArgVal) 10},
#ifdef MOTIF
	{XtNwidth, 		(XtArgVal) 10},
	{XtNheight, 		(XtArgVal) 10},
#else
	{DwtNwidth,		(XtArgVal) 600},
	{DwtNheight,		(XtArgVal) 700},
#endif
    };
#ifdef MOTIF
    static Arg boxArgs[] = {
	{XmNresizeHeight,	(XtArgVal) True},
	{XmNresizeWidth,	(XtArgVal) False},
	{XmNadjustLast, 	(XtArgVal) False},
	{XmNorientation,	(XtArgVal) XmHORIZONTAL},
	{XmNpacking,		(XtArgVal) XmPACK_TIGHT},
    };
    static Arg headerArgs[] = {
	{XmNcolumns,		(XtArgVal) 80},
	{XmNrows,		(XtArgVal) 10},
	{XmNscrollVertical, 	(XtArgVal) TRUE},
	{XmNeditMode,  		(XtArgVal) XmMULTI_LINE_EDIT},
	{XmNpendingDelete,	(XtArgVal) True},
    };
    static Arg textArgs[] = {
	{XmNcolumns,		(XtArgVal) 80},
	{XmNrows,		(XtArgVal) 30},
	{XmNscrollVertical, 	(XtArgVal) TRUE},
	{XmNeditMode,  		(XtArgVal) XmMULTI_LINE_EDIT},
	{XmNpendingDelete,	(XtArgVal) True},
    };

    static Arg frameArgs[] = {
	{XmNshadowType,		(XtArgVal) XmSHADOW_OUT},
    };
#else /* MOTIF */
    Arg textPos[1];

    
    static Arg boxArgs[] = {
	{DwtNresizeHeight,	(XtArgVal) True},
	{DwtNresizeWidth,	(XtArgVal) False},
	{DwtNorientation,	(XtArgVal) DwtOrientationHorizontal},
	{DwtNmenuPacking,	(XtArgVal) DwtMenuPackingTight},
	{DwtNmenuExtendLastRow, (XtArgVal) False},
	{DwtNborderWidth,	(XtArgVal) 3},
	{DwtNentryBorder,	(XtArgVal) 2},
	{DwtNspacing,		(XtArgVal) 2},
	{DwtNchildOverlap,	(XtArgVal) False},
	{DwtNmenuNumColumns,	(XtArgVal) 1},
	{DwtNmin,		(XtArgVal) 30},
	{DwtNmenuType,		(XtArgVal) DwtWorkarea},
    };
    static Arg headerArgs[] = {
	{DwtNcols,		(XtArgVal) 80},
	{DwtNrows,		(XtArgVal) 10},
	{DwtNwordWrap,		(XtArgVal) TRUE},
	{DwtNscrollVertical,	(XtArgVal) TRUE},
    };
    static Arg textArgs[] = {
	{DwtNcols,		(XtArgVal) 80},
	{DwtNrows,		(XtArgVal) 30},
	{DwtNwordWrap, 		(XtArgVal) TRUE},
	{DwtNscrollVertical,	(XtArgVal) TRUE},
    };

#endif /* MOTIF */
    if (ComposeActive) {
	mesgPane(XRN_SERIOUS, "Only one composition pane at a time");
	return(-1);
    }

    if (app_resources.editorCommand != NIL(char)) {
	ComposeActive = True;
	if (editMessage(False, header) == -1){
	    ComposeActive = False;
	    return -1;
	}
	/* We aren't done composing until we get a SIGCHLD -	       ggh */
	/* ComposeActive = False;				       ggh */
	return(0);
    }
    
    ComposeActive = True;
    if (ComposeTopLevel != (Widget) 0) {
	i = 0;
#ifdef MOTIF
	XtSetArg(paneArgs[i], XmNtitle,		(XtArgVal) titleString);i++;
#else
	XtSetArg(paneArgs[i], DwtNtitle,	(XtArgVal) titleString);i++;
#endif
	XtSetValues(ComposeTopLevel, paneArgs, i);
    } else {
	i = 0;
#ifdef MOTIF
	XtSetArg(paneArgs[i], XtNinput, 	(XtArgVal) True); i++;
	XtSetArg(paneArgs[i], XmNiconName, 	(XtArgVal) "XRN Compose"); i++;
	XtSetArg(paneArgs[i], XmNtransient,	(XtArgVal) False); i++;
	XtSetArg(paneArgs[i], XmNtitle,		(XtArgVal) titleString); i++;
	XtSetArg(paneArgs[i], XmNcursorPositionVisible, (XtArgVal) True); i++;
#else
	XtSetArg(paneArgs[i], XtNinput, 	(XtArgVal) True); i++;
	XtSetArg(paneArgs[i], XtNallowShellResize, (XtArgVal) True); i++;
	XtSetArg(paneArgs[i], DwtNtitle,	(XtArgVal) titleString);i++;
#endif

	ComposeTopLevel = XtCreatePopupShell("composeTop", 
					 topLevelShellWidgetClass,
					 TopLevel,
					 paneArgs, i);

	if (app_resources.compLayout) {
	    xthTranslateGeometry(app_resources.compLayout, layoutArgs);
	    XtSetValues(ComposeTopLevel, layoutArgs, XtNumber(layoutArgs));
	}

#ifdef MOTIF
	pane = XtCreateManagedWidget("pane", xmPanedWindowWidgetClass,
					ComposeTopLevel, NULL, 0);
#else
	i = 0;
	XtSetArg(paneArgs[i], DwtNwidth,	(XtArgVal) 680);i++;
	XtSetArg(paneArgs[i], DwtNheight,	(XtArgVal) 600);i++;
	pane = XtCreateManagedWidget("pane", panewidgetclass,
					ComposeTopLevel, paneArgs, i);
#endif

    /* XXX how to make a 80 column text window with scroll bar */
	if (app_resources.breakLength > 39) {
            headerArgs[0].value = (XtArgVal) app_resources.breakLength; /* set cols */
	    textArgs[0].value = (XtArgVal) app_resources.breakLength;
	} else {
	    headerArgs[0].value = (XtArgVal) 80;
	    textArgs[0].value = (XtArgVal) 80;
	}
	if (app_resources.composeRows > 0) {
	    textArgs[1].value = (XtArgVal) app_resources.composeRows;
	}
#ifdef MOTIF
	headerFrame = XtCreateManagedWidget("headerFrame", xmFrameWidgetClass,
					pane, frameArgs,XtNumber(frameArgs));
	HeaderText = XmCreateScrolledText(headerFrame, "headerText",
					headerArgs, XtNumber(headerArgs));
	fitFrameToContents(headerFrame, HeaderText);
#else
	HeaderText = DwtSTextCreate(pane, "headerText",
					headerArgs, XtNumber(headerArgs));
#endif
	XtManageChild(HeaderText);
#ifdef MOTIF
	textFrame = XtCreateManagedWidget("textFrame", xmFrameWidgetClass,
					pane, frameArgs,XtNumber(frameArgs));
	ComposeText = XmCreateScrolledText(textFrame, "composeText", 
					textArgs, XtNumber(textArgs));
	fitFrameToContents(textFrame, ComposeText);
#else
	ComposeText = DwtSTextCreate(pane, "composeText", 
					textArgs, XtNumber(textArgs));
#endif
	XtManageChild(ComposeText);
#ifdef MOTIF
	buttonBox = XtCreateManagedWidget("box", xmRowColumnWidgetClass,
		pane, boxArgs, XtNumber(boxArgs));
#else
	buttonBox = DwtMenuCreate(pane, "box", boxArgs, XtNumber(boxArgs));
	XtManageChild(buttonBox);

	paneWidth = 0;
	XtSetArg(paneArgs[0], DwtNwidth, &paneWidth);
	XtGetValues(ComposeText, paneArgs, 1);
	XtSetArg(paneArgs[0], DwtNwidth, paneWidth);
	XtSetValues(pane, paneArgs, 1);
#endif

	if (compAbortArgs[1].value == (XtArgVal) NULL) {
#ifdef MOTIF
	    compAbortArgs[1].value = (XtArgVal)XmStringLtoRCreate("Cancel",
			XmSTRING_DEFAULT_CHARSET);
#else
	    compAbortArgs[1].value = (XtArgVal)DwtLatin1String("Cancel");
#endif
	}
#ifdef MOTIF
	AbortButton = XtCreateManagedWidget("abort", xmPushButtonWidgetClass,
			buttonBox, compAbortArgs, XtNumber(compAbortArgs));
    
	paneWidth = 0;
	XtSetArg(paneArgs[0], XmNheight, &paneWidth);
	XtGetValues(AbortButton, paneArgs, 1);
	PaneSetMinMax(buttonBox, paneWidth, paneWidth+5);
#else
	AbortButton = XtCreateManagedWidget("abort", pushbuttonwidgetclass,
			buttonBox, compAbortArgs, XtNumber(compAbortArgs));
#endif
	if (compSendArgs[1].value == (XtArgVal) NULL) {
#ifdef MOTIF
	    compSendArgs[1].value = (XtArgVal)XmStringLtoRCreate("Send",
			XmSTRING_DEFAULT_CHARSET);
#else
	    compSendArgs[1].value = (XtArgVal)DwtLatin1String("Send");
#endif
	}
#ifdef MOTIF
	SendButton = XtCreateManagedWidget("send", xmPushButtonWidgetClass,
			buttonBox, compSendArgs, XtNumber(compSendArgs));
#else
	SendButton = XtCreateManagedWidget("send", pushbuttonwidgetclass,
			buttonBox, compSendArgs, XtNumber(compSendArgs));
#endif
    
	if (compSaveArgs[1].value == (XtArgVal) NULL) {
#ifdef MOTIF
	    compSaveArgs[1].value = (XtArgVal)XmStringLtoRCreate("Save",
			XmSTRING_DEFAULT_CHARSET);
#else
	    compSaveArgs[1].value = (XtArgVal)DwtLatin1String("Save");
#endif
	}
#ifdef MOTIF
	SaveButton = XtCreateManagedWidget("save", xmPushButtonWidgetClass,
			buttonBox, compSaveArgs, XtNumber(compSaveArgs));
#else
	SaveButton = XtCreateManagedWidget("save", pushbuttonwidgetclass,
			buttonBox, compSaveArgs, XtNumber(compSaveArgs));
#endif

	if (compIncludeFileArgs[1].value == (XtArgVal) NULL) {
#ifdef MOTIF
	    compIncludeFileArgs[1].value = (XtArgVal) XmStringLtoRCreate(
			"Include File",
			XmSTRING_DEFAULT_CHARSET);
#else
	    compIncludeFileArgs[1].value = (XtArgVal)DwtLatin1String(
			"Include File");
#endif
	}
#ifdef MOTIF
	IncludeFileButton = XtCreateManagedWidget("includeFile",
			xmPushButtonWidgetClass, buttonBox,
			compIncludeFileArgs, XtNumber(compIncludeFileArgs));
#else
	IncludeFileButton = XtCreateManagedWidget("includeFile",
			pushbuttonwidgetclass, buttonBox,
			compIncludeFileArgs, XtNumber(compIncludeFileArgs));
#endif

	if (compIncludeArticleArgs[1].value == (XtArgVal) NULL) {
#ifdef MOTIF
	    compIncludeArticleArgs[1].value = (XtArgVal)XmStringLtoRCreate(
			"Include Article",
			XmSTRING_DEFAULT_CHARSET);
#else
	    compIncludeArticleArgs[1].value = (XtArgVal)DwtLatin1String(
		 	"Include Article");
#endif
	}
#ifdef MOTIF
	IncludeArticleButton = XtCreateManagedWidget("includeArticle",
			xmPushButtonWidgetClass, buttonBox,
			compIncludeArticleArgs, XtNumber(compIncludeArticleArgs));
#else
	IncludeArticleButton = XtCreateManagedWidget("includeArticle",
			pushbuttonwidgetclass, buttonBox,
			compIncludeArticleArgs, XtNumber(compIncludeArticleArgs));
#endif
	if (compIncludeCurrentArgs[1].value == (XtArgVal) NULL) {
#ifdef MOTIF
	    compIncludeCurrentArgs[1].value = (XtArgVal)XmStringLtoRCreate(
			"Include Current",
			XmSTRING_DEFAULT_CHARSET);
#else
	    compIncludeCurrentArgs[1].value = (XtArgVal)DwtLatin1String(
			"Include Current");
#endif
	}
#ifdef MOTIF
	IncludeCurrentButton = XtCreateManagedWidget("includeCurrent",
			xmPushButtonWidgetClass, buttonBox,
			compIncludeCurrentArgs, XtNumber(compIncludeCurrentArgs));
#else
	IncludeCurrentButton = XtCreateManagedWidget("includeCurrent",
			pushbuttonwidgetclass, buttonBox,
			compIncludeCurrentArgs, XtNumber(compIncludeCurrentArgs));
#endif
	if (compWrapTextArgs[1].value == (XtArgVal) NULL) {
#ifdef MOTIF
	    compWrapTextArgs[1].value = (XtArgVal)XmStringLtoRCreate(
			"Word Wrap Text",
			XmSTRING_DEFAULT_CHARSET);
#else
	    compWrapTextArgs[1].value = (XtArgVal)DwtLatin1String(
			"Word Wrap Text");
#endif
	}
#ifdef MOTIF
	WrapTextButton = XtCreateManagedWidget("wrapText",
			xmPushButtonWidgetClass, buttonBox,
			compWrapTextArgs, XtNumber(compWrapTextArgs));
#else
	WrapTextButton = XtCreateManagedWidget("wrapText",
			pushbuttonwidgetclass, buttonBox,
			compWrapTextArgs, XtNumber(compWrapTextArgs));
#endif
	if (compRot13Args[1].value == (XtArgVal) NULL) {
#ifdef MOTIF
	    compRot13Args[1].value = (XtArgVal)XmStringLtoRCreate(
			"Rot-13",
			XmSTRING_DEFAULT_CHARSET);
#else
	    compWrapTextArgs[1].value = (XtArgVal)DwtLatin1String(
			"Rot-13");
#endif
	}
#ifdef MOTIF
	Rot13Button = XtCreateManagedWidget("rot13",
			xmPushButtonWidgetClass, buttonBox,
			compRot13Args, XtNumber(compRot13Args));
#else
	Rot13Button = XtCreateManagedWidget("rot13",
			pushbuttonwidgetclass, buttonBox,
			compRot13Args, XtNumber(compRot13Args));
#endif
	xmIconCreate(ComposeTopLevel);
	XtAddActions(ComposeActions, XtNumber(ComposeActions));
#ifdef MOTIF
	xmAddWMHandler(ComposeTopLevel, (XtCallbackProc) compAbortFunction);
#endif
    }

    sizeCompose(utStrlen(header));
    (void) strcpy(CompositionString, header);
#ifdef MOTIF
    XmTextSetString(HeaderText,HeaderString);
    XmTextSetString(ComposeText,CompositionString);
#ifndef MOTIF_V10
    if (EditorPosition < 0) {
	XmTextSetCursorPosition(HeaderText, -EditorPosition);
	XmProcessTraversal(HeaderText, XmTRAVERSE_CURRENT);
	XmTextSetCursorPosition(ComposeText, 0);
    } else {
	XmTextSetCursorPosition(ComposeText, (XmTextPosition) EditorPosition);
	XmTextSetCursorPosition(HeaderText, 0);
	XmProcessTraversal(ComposeText, XmTRAVERSE_CURRENT);
    }
#endif /* MOTIF_V10 */
#else /* MOTIF */
    DwtSTextSetString(HeaderText,HeaderString);
    DwtSTextSetString(ComposeText,CompositionString);
    if (EditorPosition < 0) {
	XtSetArg(textPos[0], DwtNinsertionPosition, (XtArgVal) -EditorPosition);
	XtSetValues(HeaderText, textPos, XtNumber(textPos));
    } else {
	XtSetArg(textPos[0], DwtNinsertionPosition, (XtArgVal) EditorPosition);
	XtSetValues(ComposeText, textPos, XtNumber(textPos));
    }
#endif /* MOTIF */

    XtSetSensitive(SendButton, True);
    XtSetSensitive(SaveButton, True);
    XtSetSensitive(IncludeFileButton, True);
    XtSetSensitive(IncludeArticleButton, (PostingMode != POST));
    XtSetSensitive(IncludeCurrentButton, True);
    XtSetSensitive(WrapTextButton, True);
    XtSetSensitive(Rot13Button, True);
    XtRealizeWidget(ComposeTopLevel);
    

    XtPopup(ComposeTopLevel, XtGrabNone);

    return(0);
}

/*
 * return a string containing the contents of the users signature file
 *   (in a static buffer)
 *
 * if the signature file is bigger than MAX_SIGNATURE_SIZE, return NIL(char).
 */
static char *signatureFile()
{
    char *file;
    FILE *infofp;
    int (*close_func)() = 0;
    long count;
    static char info[MAX_SIGNATURE_SIZE+5];
    static char *retinfo;
    static char *sigfile = NULL;
    char *artfile = NULL;
    FILE *fpsave;
    Boolean execSig;

#if defined(INEWS_READS_SIG)
    /* these posting modes do not go through INEWS, so include the signature */
    if ((PostingMode != REPLY) && (PostingMode != GRIPE)) {
	return 0;
    }
#endif /* INEWS_READS_SIG */

    if (sigfile != NULL) {
	FREE(sigfile);
    }

    if ((file = utTildeExpand(app_resources.signatureFile)) == NIL(char)) {
	return NIL(char);
    }

    /* handle multiple signatures */
    while (1) {
	/* find an appropriate sig */
        struct newsgroup *newsgroup = CurrentGroup;
	char *psigfile = NIL(char);
	char *ptr;

	if (!CurrentGroup) {
	    sigfile = XtMalloc(strlen(file) + 10);
	} else {
	    /* signature according to group or hierarchy */
	    sigfile = XtMalloc(strlen(file) + 10 + strlen(newsgroup->name));
	    if (app_resources.localSignatures) {
		ptr = localKillFile(newsgroup, FALSE);
		if (!ptr) 
		    ptr = file;
		(void) strcpy(sigfile, ptr);
		ptr = rindex(sigfile, '/');
		ptr[1] = 0;
		ptr = (char *) (sigfile + strlen(app_resources.expandedSaveDir));
		while ((psigfile = rindex(sigfile, '/')) != NULL) {
		    if (psigfile < ptr) {
			psigfile = NULL;
			break;
		    }
		    psigfile[0] = 0;
		    (void) strcat(sigfile, "/SIGNATURE");
		    if (! access(sigfile, F_OK)) {
			/* FOUND */
			break;
		    }
		    psigfile[0] = 0;
		}
	    } else {
		(void) strcpy(sigfile, file);
		(void) strcat(sigfile, "-");
		ptr = rindex(sigfile, '-');
		(void) strcat(sigfile, newsgroup->name);
		(void) strcat(sigfile, ".");
		while ((psigfile = rindex(sigfile, '.')) != NIL(char)) {
		    if (psigfile < ptr) {
			psigfile = NIL(char);
			break;
		    }
		    psigfile[0] = 0;
		    if (! access(sigfile, F_OK)) {
			/*FOUND*/ 
			break;
		    }
		}
	    }
	}

	if (psigfile != NIL(char)) {
	    /*FOUND*/
	    break;
	}

	/* signature according to posting mode. */
	(void) strcpy(sigfile, file);
	(void) strcat(sigfile, ".");
	(void) strcat(sigfile, PostingModeStrings[PostingMode]);

	if (! access(sigfile, F_OK)) {
	    /*FOUND*/
	    break;
	}

	(void) strcpy(sigfile, file);
	if (! access(sigfile, F_OK)) {
	    break;
	} else {
	    return NIL(char);
	}
    }

    {
	char cmdbuf[1024];
	char *p = rindex(sigfile, '/');

	infofp = NIL(FILE);

	if (! p) {
	     p = sigfile;
	} else {
	     p++;
	}

#ifndef VMS
	if (Header.artText) {
	    artfile = (char *) utTempnam(NULL, "XRN");
	    fpsave = fopen(artfile,"w");
	    if (fpsave) {
		fputs(Header.artText, fpsave);
		(void) fclose(fpsave);
	    } else {
		artfile = NULL;
	    }
	}

	if (app_resources.executableSignatures && (! access(sigfile, X_OK))) {
	    if (app_resources.signatureNotify) {
		mesgPane(XRN_INFO, "Executing signature command %s.", sigfile);
	    }
	    (void) sprintf(cmdbuf, "%s %s %s %s",
			    sigfile,
			    (CurrentGroup ? CurrentGroup->name : "NIL"),
			    PostingModeStrings[PostingMode],
			    (artfile ? artfile : "NIL"));
	    infofp = popen(cmdbuf, "r");
	    close_func = pclose;
	    if (!infofp) {
		mesgPane(XRN_SERIOUS, 
			 "Cannot execute signature file %s.  Reading instead.",
			 sigfile);
	    }
	} else {
#endif
	    if (app_resources.signatureNotify) {
		mesgPane(XRN_INFO,
			 "Reading signature file %s.", sigfile);
	    }
#ifndef VMS
	}
#endif

	if (!infofp) {
	    infofp = fopen(sigfile, "r");
	    close_func = fclose;
	    if (! infofp) {
		mesgPane(XRN_SERIOUS, "Cannot read signature file %s.",
			 sigfile);
		return NIL(char);
	    }
	}
    }

    (void) strcpy(info, "-- \n");
    count = fread(&info[4], sizeof(char), sizeof(info) - 4, infofp);
    info[count + 4] = '\0';

    if (! feof(infofp)) {
	/* Signature file is too big */
	retinfo = NIL(char);
    }
    else if (strncmp(info + 4, "--\n", 3) == 0 ||
	     strncmp(info + 4, "-- \n", 4) == 0) {
	retinfo = info + 4;
    } else {
	retinfo = info;
    }

    if (artfile) {
	unlink(artfile);
	FREE(artfile);
    }
    (void) (*close_func)(infofp);

    return retinfo;
}

/* public functions 'reply', 'gripe', 'forward', 'followup', and 'post' */

void
reply()
{
    struct newsgroup *newsgroup = CurrentGroup;
    art_num current = newsgroup->current;
    char title[LABEL_SIZE];
    char pathFormatString[100];
    char *message, *signature;
    int OldPostingMode = PostingMode;

    if (ComposeActive) {
	mesgPane(XRN_SERIOUS, "Only one composition pane at a time");
	return;
    }
    getHeader(current);
    Header.artText = getText(current);

    PostingMode = REPLY;
    message = XtMalloc(MAX_SIGNATURE_SIZE + BUFFER_SIZE + 
			utStrlen(Header.artText));
    sizeCompose(utStrlen(Header.artText) + MAX_SIGNATURE_SIZE + BUFFER_SIZE);

    (void) sprintf(title, "Reply to article %ld in %s", current,
		newsgroup->name);
    
    if (app_resources.replyPath != NIL(char)) {
	if (app_resources.ccForward) {
	    sprintf(pathFormatString, "To: %s\nCc: %s\n",
		app_resources.replyPath, Header.user);
	} else {
	    if (app_resources.cc == True) {
		sprintf(pathFormatString, "To: %s\nCc: \n",
			app_resources.replyPath);
	    } else {
		sprintf(pathFormatString, "To: %s\n",app_resources.replyPath);
	    }
	}
    } else {
	if (app_resources.ccForward) {
	    sprintf(pathFormatString, "To: %s\nCc: %s\n", "%s", Header.user);
	} else {
	    if (app_resources.cc == True) {
		strcpy(pathFormatString, "To: %s\nCc: \n");
	    } else {
		strcpy(pathFormatString, "To: %s\n");
	    }
	}
    }
    (void) sprintf(message, pathFormatString,
		   (*Header.replyTo != '\0') ? Header.replyTo : Header.from);

    if (app_resources.replyTo != NIL(char)) {
	 (void) strcat(message, "Reply-To: ");
	 (void) strcat(message, app_resources.replyTo);
	 (void) strcat(message, "\n");
    }

    if (app_resources.extraMailHeaders) {
	 (void) sprintf(&message[strlen(message)],
			"X-Newsgroups: %s\nIn-reply-to: %s\n",
			Header.newsgroups, Header.messageId);
    }

    buildSubject(message);
    if (app_resources.editorCommand != NULL) {
	(void) strcat(message,
  		"-=-=-=-= Enter your text below this line ");
	(void) strcat(message, "(don't delete this line) =-=-=-=-\n");
    }
    (void) strcat(message, "\n");
    InsertionPoint = utStrlen(message);
    (void) strcat(message, "\n");
    strcpy(HeaderString, message);
    message[0] = '\0';
    EditorPosition = 0;
    if ((signature = signatureFile()) != NIL(char)) {
	(void) strcpy(message, "\n");
	(void) strcat(message, signature);
    }
    if (composePane(title, message))
	 PostingMode = OldPostingMode;

    FREE(message);
    return;
}


void
gripe()
{
    char title[LABEL_SIZE];
    char message[MAX_SIGNATURE_SIZE + 10000], *signature;
    int OldPostingMode = PostingMode;
    char gripeTo[200];
    
    if (ComposeActive) {
	mesgPane(XRN_SERIOUS, "Only one composition pane at a time");
	return;
    }
    sizeCompose(1000);
#ifdef VMS
    if (app_resources.replyPath != NIL(char)) {
	sprintf(gripeTo, app_resources.replyPath, GRIPES);
    } else {
	strcpy(gripeTo, GRIPES);
    }
#else
    strcpy(gripeTo, GRIPES);
#endif
    (void) strcpy(title, "Gripe");
    (void) sprintf(message, "To: %s\nSubject: GRIPE about XRN %s\n\n",
		   gripeTo, XRN_VERSION);
    if (app_resources.editorCommand != NULL) {
	(void) strcat(message,
  		"-=-=-=-= Enter your text below this line ");
	(void) strcat(message, "(don't delete this line) =-=-=-=-\n");
    }
    InsertionPoint = utStrlen(message);
    (void) strcat(message, "\n");
    (void) strcpy(HeaderString, message);

    EditorPosition = 0;
    message[0] = '\0';
    if ((signature = signatureFile()) != NIL(char)) {
	(void) strcpy(message, "\n");
	(void) strcat(message, signature);
    }

    Header.artText = NULL;
    PostingMode = GRIPE;
    if (composePane(title, message))
	 PostingMode = OldPostingMode;

    return;
}


void
forward()
{
    char title[LABEL_SIZE];
    char *signature;
    struct newsgroup *newsgroup = CurrentGroup;
    art_num current = newsgroup->current;
    int OldPostingMode = PostingMode;
    
    if (ComposeActive) {
	mesgPane(XRN_SERIOUS, "Only one composition pane at a time");
	return;
    }
    getHeader(current);
    Header.artText = getText(current);
    if (Header.artText == NULL) {
	XBell(XtDisplay(TopLevel), 0);
	mesgPane(XRN_SERIOUS, "No article to include");
	freeHeader();
	return;
    }

    sizeCompose(MAX_SIGNATURE_SIZE + utStrlen(Header.artText) + 1000);
    (void) sprintf(title, "Forward article %ld in %s to a User", current,
		   newsgroup->name);
    if (app_resources.ccForward == True) {
	(void) sprintf(HeaderString, "To: \nCc: %s\nSubject: %s - %s #%ld\n",
		       Header.user,
		       Header.subject, newsgroup->name, current);
    } else {
	if (app_resources.cc == True) {
	    (void) sprintf(HeaderString, 
		"To: \nCc: \nSubject: %s - %s #%ld\n" ,
		Header.subject, newsgroup->name, current);
	} else {
	    (void) sprintf(HeaderString, "To: \nSubject: %s - %s #%ld\n" ,
		Header.subject, newsgroup->name, current);
	}
    }
    if (app_resources.editorCommand != NULL) {
	(void) strcat(HeaderString,
  		"-=-=-=-= Enter your text below this line ");
	(void) strcat(HeaderString, "(don't delete this line) =-=-=-=-\n");
    }
    InsertionPoint = 0;
    if ((signature = signatureFile()) != NIL(char)) {
	(void) strcpy(TempString, signature);
    } else {
	*TempString = '\0';
    }

    strcpy(CompositionString, TempString);
    EditorPosition = -4;
    InsertionPoint = 0;
    PostingMode = FORWARD;
    includeArticleText();
    strcpy(TempString, CompositionString);
    if (composePane(title, TempString))
	 PostingMode = OldPostingMode;
    return;
}

#ifdef GENERATE_EXTRA_FIELDS
/*
 *  generate a message id
 */
static char genid[132];

static char *gen_id()
{
    char *timestr, *cp;
    time_t cur_time;

    time(&cur_time);
    timestr = ctime(&cur_time);

    (void) sprintf(genid, "<%.4s%.3s%.2s.%.2s%.2s%.2s@%s>",
		    &timestr[20], &timestr[4], &timestr[8],
		    &timestr[11], &timestr[14], &timestr[17],
		    Header.host);
    cp = &genid[8];

    if (*cp == ' ') {
	do {
	    *cp = *(cp + 1); 
	} while (*cp++);
    }

    return(genid);
}
/*
 * generate a valid RFC822 date-time
 */
static char gendate[40];

static char *gen_date()
{

    time_t clockTime;
    char *atime, *tz;
    struct tm *cur_time;

    (void) time(&clockTime);
    cur_time = gmtime(&clockTime);
    if (cur_time) {
	atime = ctime(&clockTime);
	tz = "GMT";
    } else {
	atime = asctime(localtime(&clockTime));
	tz = getenv(TIMEZONE_ENV);
	if (tz == NULL) {
	    fprintf(stderr, "xrn: TIMEZONE_ENV not set\n"); 
	    tz = "GMT";
	}
    }
    sprintf(gendate, "%3.3s, %2.2s %3.3s %2.2s %8.8s %s",
	atime, atime+8, atime+4, atime+22, atime+11, tz);
    return(gendate);
}
#endif

static void
doFollowup(newMode)
int newMode;
{
    struct newsgroup *newsgroup = CurrentGroup;
    art_num current = newsgroup->current;
    char buffer[10000], title[LABEL_SIZE];
    char *signature;
    int OldPostingMode = PostingMode;

    if (ComposeActive) {
	mesgPane(XRN_SERIOUS, "Only one composition pane at a time");
	return;
    }

    getHeader(current);

    if (! strcmp(Header.followupTo, "poster")) {
	 freeHeader();
	 mesgPane(XRN_INFO, "Message says to followup to poster; composing reply instead of followup.");
	 reply();
	 return;
    }

    if ((newsgroup->status & NG_UNPOSTABLE) == NG_UNPOSTABLE) {
	freeHeader();
	mesgPane(XRN_SERIOUS, "Can not post articles to this group");
	return;
    }

    Header.artText = getText(current);
    sizeCompose(utStrlen(Header.artText) + MAX_SIGNATURE_SIZE + 1000);
    
    PostingMode = newMode;
    (void) sprintf(title, "Followup to article %ld in %s",
		   current, newsgroup->name);

#if defined(INEWS) || defined(HIDE_PATH)
    (void) sprintf(TempString, "Path: %s\n", Header.user);
#else
    (void) sprintf(TempString, "Path: %s!%s\n", Header.path, Header.user);
#endif
    if ((Header.followupTo != NIL(char)) && (*Header.followupTo != '\0')) {
	Header.newsgroups = XtNewString(Header.followupTo);
    }
    (void) sprintf(buffer, "Newsgroups: %s\n", Header.newsgroups);
    (void) strcpy(TempString, buffer);
    (void) strcat(TempString, "Distribution: ");
    if ((Header.distribution != NIL(char)) && (*Header.distribution != '\0')) {
	(void) strcat(TempString, Header.distribution);
    } else if (app_resources.distribution) {
	(void) strcat(TempString, app_resources.distribution);
    } else {	
	(void) strcat(TempString, DISTRIBUTION);
    }
    (void) strcat(TempString, "\n");
    (void) sprintf(buffer, "X-Newsreader: %s\n", TITLE);
    (void) strcat(TempString, buffer);
    (void) strcat(TempString, "Followup-To: \n");
    (void) sprintf(buffer, "References: %s %s\n",
	Header.references, Header.messageId);
    (void) strcat(TempString, buffer);
    (void) sprintf(buffer, "From: %s@%s (%s)\n",
		   Header.user, Header.host, Header.fullname);
    (void) strcat(TempString, buffer);

#ifdef GENERATE_EXTRA_FIELDS
    /* stuff to generate Message-ID and Date... */
    (void) sprintf(buffer, "Date: %s\n", gen_date());
    (void) strcat(TempString, buffer);
    (void) sprintf(buffer, "Message-ID: %s\n", gen_id());
    (void) strcat(TempString, buffer);
#endif

    (void) strcat(TempString, "Reply-To: ");
    if (app_resources.replyTo != NIL(char)) {
	(void) strcat(TempString, app_resources.replyTo);
    } else {
	(void) sprintf(buffer, "%s@%s (%s)",
		       Header.user, Header.host, Header.fullname);
	(void) strcat(TempString, buffer);
    }
    (void) strcat(TempString, "\n");

    (void) strcat(TempString, "Organization: ");
    if (app_resources.organization != NIL(char)) {
	(void) strcat(TempString, app_resources.organization);
    } else {
	if (Header.organization) {
	    (void) strcat(TempString, Header.organization);
	} else {
	    (void) strcat(TempString, ORG_NAME);
	}
    }
    (void) strcat(TempString, "\n");

    buildSubject(TempString);
    
    (void) strcat(TempString, "Keywords: ");
    if ((Header.keywords != NIL(char)) && (*Header.keywords != '\0')) {
	(void) strcat(TempString, Header.keywords);
    }
    (void) strcat(TempString, "\n");
    if (app_resources.editorCommand != NULL) {
	(void) strcat(TempString,
  		"-=-=-=-= Enter your text below this line ");
	(void) strcat(TempString, "(don't delete this line) =-=-=-=-\n");
    }
    (void) strcpy(HeaderString, TempString);

    EditorPosition = InsertionPoint = utStrlen(TempString);
    
#if !defined(INEWS_READS_SIG)
    if ((signature = signatureFile()) != NIL(char)) {
	(void) strcpy(TempString, "\n");
	(void) strcat(TempString, signature);
    } else {
	*TempString = '\0';
    }
#else
    *TempString = '\0';
#endif

    if (composePane(title, TempString))
	 PostingMode = OldPostingMode;

    return;
}
void
followup()
{
    doFollowup(FOLLOWUP);
}

void
followupAndReply()
{
    doFollowup(FOLLOWUPREPLY);
}

void
post()
{
    struct newsgroup *newsgroup = CurrentGroup;
    char title[LABEL_SIZE], buffer[10000];
    char *signature;
    int OldPostingMode = PostingMode;

    if (ComposeActive) {
	mesgPane(XRN_SERIOUS, "Only one composition pane at a time");
	return;
    }
    if (newsgroup) {
	if ((newsgroup->status & NG_UNPOSTABLE) == NG_UNPOSTABLE) {
	    mesgPane(XRN_SERIOUS, "Can not post articles to this group");
	    return;
	}
    }

    sizeCompose(10000);
    getHeader((art_num) 0);

    if (!newsgroup) {
	FREE(Header.newsgroups);
	Header.newsgroups = XtNewString("");
	(void) strcpy(title, "Post article (no current group)");
    } else {
	(void) sprintf(title, "Post article to `%s'", newsgroup->name);
	Header.newsgroups = XtNewString(newsgroup->name);
    }

#if defined(INEWS) || defined(HIDE_PATH)
    (void) sprintf(TempString, "Path: %s\n", Header.user);
#else
    (void) sprintf(TempString, "Path: %s!%s\n", Header.path, Header.user);
#endif
    (void) sprintf(buffer, "Newsgroups: %s\n", Header.newsgroups);
    (void) strcat(TempString, buffer);
    (void) strcat(TempString, "Distribution: ");
    if (app_resources.distribution) {
	(void) strcat(TempString, app_resources.distribution);
    } else {	
	(void) strcat(TempString, DISTRIBUTION);
    }
    (void) strcat(TempString, "\n");
    (void) sprintf(buffer, "X-Newsreader: %s\n", TITLE);
    (void) strcat(TempString, buffer);

#ifdef GENERATE_EXTRA_FIELDS
    /* stuff to generate Message-ID and Date... */
    (void) sprintf(buffer, "Date: %s\n", gen_date());
    (void) strcat(TempString, buffer);
    (void) sprintf(buffer, "Message-ID: %s\n", gen_id());
    (void) strcat(TempString, buffer);
#endif

    (void) strcat(TempString, "Followup-To: \n");
    (void) sprintf(buffer, "From: %s@%s (%s)\n",
		   Header.user, Header.host, Header.fullname);
    (void) strcat(TempString, buffer);
    (void) strcat(TempString, "Reply-To: ");
    if (app_resources.replyTo != NIL(char)) {
	(void) strcat(TempString, app_resources.replyTo);
    } else {
	(void) sprintf(buffer, "%s@%s (%s)", Header.user, Header.host, Header.fullname);
	(void) strcat(TempString, buffer);
    }
    (void) strcat(TempString, "\n");

    (void) strcat(TempString, "Organization: ");
    if (app_resources.organization != NIL(char)) {
	(void) strcat(TempString, app_resources.organization);
    } else {
	if (Header.organization) {
	    (void) strcat(TempString, Header.organization);
	} else {
	    (void) strcat(TempString, ORG_NAME);
	}
    }
    (void) strcat(TempString, "\n");
    (void) strcat(TempString, "Subject: \n");
    (void) strcat(TempString, "Keywords: \n");
    if (app_resources.editorCommand != NULL) {
	(void) strcat(TempString,
  		"-=-=-=-= Enter your text below this line ");
	(void) strcat(TempString, "(don't delete this line) =-=-=-=-\n");
    }
    (void) strcpy(HeaderString, TempString);

    EditorPosition = InsertionPoint = 0;
#if !defined(INEWS_READS_SIG)
    if ((signature = signatureFile()) != NIL(char)) {
	(void) strcpy(TempString, "\n");
	(void) strcat(TempString, signature);
    } else {
	*TempString = '\0';
    }
#else
    *TempString = '\0';
#endif

    PostingMode = POST;
    if (composePane(title, TempString))
	 PostingMode = OldPostingMode;

    return;
}


void
cancelArticle()
{
    struct newsgroup *newsgroup = CurrentGroup;
    art_num current = newsgroup->current;
    char buffer[10000];
    char *bufptr;

    if (ComposeActive) {
	mesgPane(XRN_SERIOUS, "Only one composition pane at a time");
	return;
    }
    if ((newsgroup->status & NG_UNPOSTABLE) == NG_UNPOSTABLE) {
	mesgPane(XRN_SERIOUS, "Can not post articles to this group");
	return;
    }

    getHeader(current);

    sizeCompose(10000);
    /* verify that the user can cancel the article */
    bufptr = index(Header.from, '@');
    if (bufptr != NIL(char)) {
	bufptr++;
	(void) strcpy(buffer, bufptr);
	if ((bufptr = index(buffer, ' ')) != NIL(char)) {
	    *bufptr = '\0';
	}
	if (strncmp(Header.host, buffer, utStrlen(Header.host))
	   || (strncmp(Header.user, Header.from, utStrlen(Header.user)) 
	      && strcmp(Header.user, "root"))) {
	    mesgPane(XRN_SERIOUS, "Not entitled to cancel the article");
	    freeHeader();
	    return;
        }
    }

#if defined(INEWS) || defined(HIDE_PATH)
    (void) sprintf(TempString, "Path: %s\n", Header.user);
#else
    (void) sprintf(TempString, "Path: %s!%s\n", Header.path, Header.user);
#endif
    (void) sprintf(buffer, "From: %s@%s (%s)\n", Header.user, Header.host, Header.fullname);
    (void) strcat(TempString, buffer);
    (void) sprintf(buffer, "Subject: cancel %s\n", Header.messageId);
    (void) strcat(TempString, buffer);
    if (*Header.followupTo != '\0') {
	Header.newsgroups = XtNewString(Header.followupTo);
    }
    (void) sprintf(buffer, "Newsgroups: %s\n", Header.newsgroups);
    (void) strcat(TempString, buffer);
    (void) sprintf(buffer, "References: %s %s\n",
		   Header.references, Header.messageId);
    (void) strcat(TempString, buffer);

    (void) strcat(TempString, "Distribution: ");
    if ((Header.distribution != NIL(char)) && (*Header.distribution != '\0')) {
	(void) strcat(TempString, Header.distribution);
    } else if (app_resources.distribution) {
	(void) strcat(TempString, app_resources.distribution);
    } else {
	(void) strcat(TempString, DISTRIBUTION);
    }
    (void) strcat(TempString, "\n");
    (void) sprintf(buffer, "Control: cancel %s\n", Header.messageId);
    (void) strcat(TempString, buffer);
#ifdef GENERATE_EXTRA_FIELDS
    /* stuff to generate Message-ID and Date... */
    (void) sprintf(buffer, "Date: %s\n", gen_date());
    (void) strcat(TempString, buffer);
    (void) sprintf(buffer, "Message-ID: %s\n", gen_id());
    (void) strcat(TempString, buffer);
#endif

    freeHeader();

    switch (postArticle(TempString,XRN_NEWS)) {
	case POST_FAILED:
	mesgPane(XRN_SERIOUS, "Could not cancel the article");
	break;

	case POST_NOTALLOWED:
	mesgPane(XRN_SERIOUS, "Posting not allowed from this machine");
	break;
	    
	case POST_OKAY:
	mesgPane(XRN_INFO, "Canceled the article");
	break;
    }

    return;
}
