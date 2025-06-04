/****************************************************************************
 * JL Wilkinson 13-Jul-1994 Fixed two bugs in our VMS implementation of
 *			    Gtxt() and catgets() which caused the Gtxt("",222) 
 *			    call setlocale() to return a null string (at least
 *			    on my DEC 7610 AXP machine under VMS v6.1).
 * F.Macrides 13-Jul-1994   Worked in Alan's second set of patches as a
 			     separate section for Unix.  They're buggey,
			     so please make sure none of the for-VMS code
			     is changed when working on the for_Unix code:
	- major changes to allow machines without catgets to use i18n
	  (but these routines are now heavily dependent on the rest of
	  the gopher libraries, unlike their original design)
 * F.Macrides 03-Jul-1994   Fixed up the catopen() emulation for VMS so it
 *			     doesn't ACCVIO on the bad MESSAGE shareable for
 *			     Alphas.  It now puts up "key not found" and the
 *			     traceback, then goes on normally, using the
 *			     the hardcoded message strings.  Since nothing
 *			     "bad" happens and a relevant message is shown
 *			     if someone tries to use a bad shareable on an
 *			     Alpha, I took out the bypass.  Now all we need
 *			     to do is figure out how to put in a UNIVERSAL
 *			     key when building the MESSAGE shareable for
 *			     Alphas.
 * F.Macrides 02-Jul-1994   Added include files for defining return statuses
 *			     and code to check them so we don't hang on
 *			     errors.
 *			    The shareable MESSAGE images can't be built on
 *			     VMS Alphas as presently written for VAXen, and
 *			     I haven't yet figured out how to fix it up. So
 *			     for now, an "#ifdef __ALPHA" bypass precedes
 *			     the LIB$FIND_IMAGE_SYMBOL() call in the catopen()
 *			     emulation for VMS, so it "returns on error"
 *			     instead of actually calling it and causing an
 *			     ACCVIO.  Don't worry, we'll get it right for
 *			     DECC/AXP too, eventually.
 * F.Macrides 01-Jul-1994   Added Lance's internationalization port to VMS:
 *   JL Wilkinson 24-June-1994	Gtxt() and setlocale() functions and support
 *				for VMS.
 *   JL Wilkinson 24-June-1994	Fixed DAnew() call in Gtxtopen() for proper
 *				order of Copy and Destroy functions.
 */
/**********************************************************************
  Changes from Alan Coopersmith's patches:
    - use routines in Locale.c instead of direct calls to
	  catopen()/catgets()
 **********************************************************************/
/*
 * These routines make using the international messages facilities easier.
 * 
 * This interface could be applied to methods other than X/Open coding
 * (VMS etc..)
 *
 * Copyright (c) 1994 University of Minnesota
 */

#ifdef VMS /*** VMS version: ***/

#include "Locale.h"
#include "Stdlib.h"
#include "String.h"
#include "DAarray.h"
#include "STRstring.h"

typedef int nl_catd;

nl_catd	     Gcatd	 = (nl_catd) -1;

/*************** DynArray of messages ***************/
typedef	DynArray GtxtArray;
GtxtArray     *GtxtMsgs	 = (GtxtArray *) NULL;

#define GtxtNew	    STRnew
#define GtxtInit    STRinit
#define GtxtCopy    STRcpy
#define GtxtDestroy STRdestroy

/*************** Public interfaces ***************/

char *
Gtxt (defaultString, msgNumber)
     char *defaultString;
     int msgNumber;
{
     String    *msgString;
     char      *msgText;

     if ( (Gcatd == (nl_catd) -1) )
	  return defaultString;

     if ( GtxtMsgs != NULL ) {
	  if (DAgetNumitems(GtxtMsgs) < msgNumber) {
	       DAgrow(GtxtMsgs, msgNumber);
	  }
	  msgString = (String *) DAgetEntry(GtxtMsgs, msgNumber - 1);
	  if (STRlen(msgString) == 0) {
	       STRset(msgString,
		      catgets(Gcatd, NL_SETD, msgNumber, defaultString));
	  }
	  if (STRget(msgString) != NULL)
	       return (STRget(msgString));
     }

     return (catgets(Gcatd, NL_SETD, msgNumber, defaultString));
}

#ifdef flags
#undef flags
#endif

nl_catd
Gtxtopen (catname, flags)
     char *catname;
     int flags;
{
    int junk;

     if (GtxtMsgs != NULL) {
	  DAdestroy(GtxtMsgs);
     }

     Gcatd = catopen(catname, flags);

     if (Gcatd != (nl_catd) -1) {
	  GtxtMsgs = (GtxtArray *) DAnew(10, GtxtNew, GtxtInit,
					 GtxtDestroy, GtxtCopy);
     }

     return Gcatd;
}

#include "GDgopherDir.h"
#include "GSgopherObj.h"
#include <descrip.h>
#include <ssdef.h>

int GTXT_facility=0;

extern	GopherDirObj	*setlocale_LangDir;

/*
**  Emulate catgets() by securing a VMS message
 */

char *
catgets(junk1, junk2, code, msg_default)
    int junk1;
    int junk2;
    int code;
    char *msg_default;
{
    struct  dsc$descriptor_s buf_;
static
    char    buf[512];
    int	    i;
    int	    x;

    /* Reconfigure code as VMS error condition */
    code = (GTXT_facility<<16) + (code << 3) + 3;
    code |= 0x08008000;

    buf_.dsc$b_dtype = DSC$K_DTYPE_T;
    buf_.dsc$b_class = DSC$K_CLASS_S;
    buf_.dsc$a_pointer = buf;
    buf_.dsc$w_length = sizeof(buf);
    memset(buf,'\0',sizeof(buf));
    i = 0;
    x = SYS$GETMSG(code, &i, &buf_, 1, 0);
    if ((x & 1) && (x != SS$_MSGNOTFND)) {
	buf[i] = '\0';
	return(buf);
    }
    return(msg_default);
}

/*
**	Emulate catopen() by locating and activating a VMS message file
 */
nl_catd
catopen(char *filespec, int junk)
{
    struct  dsc$descriptor_s filespec_;
    struct  dsc$descriptor_s symbol_;
    unsigned long value;
    int	    status;
    $DESCRIPTOR(image_,"GOPHERP_DIR:.EXE");
    char    *language;
    char    *file;
    char    *cp;

    filespec_.dsc$b_dtype = symbol_.dsc$b_dtype = DSC$K_DTYPE_T;
    filespec_.dsc$b_class = symbol_.dsc$b_class = DSC$K_CLASS_S;
    if ((cp=strstr(filespec, "GOPHER_MSG")) == NULL)
	return(SS$_IVLOGNAM);
    file = (char *)malloc(sizeof(char)*(strlen(cp)+1));
    strcpy(file,cp);
    language = strstr(file,"_MSG") + strlen("_MSG");
    cp = strrchr(language,'.');
    if (cp)
        *cp = '\0';
    symbol_.dsc$a_pointer = (char *)malloc(sizeof(char)*(strlen("GOPHER_LANG")
				    +1+strlen(language)));
    strcpy(symbol_.dsc$a_pointer,"GOPHER_LANG");
    strcat(symbol_.dsc$a_pointer,language);
    symbol_.dsc$w_length = strlen(symbol_.dsc$a_pointer);
    filespec_.dsc$a_pointer = file;
    filespec_.dsc$w_length = strlen(filespec_.dsc$a_pointer);
    status = LIB$FIND_IMAGE_SYMBOL(&filespec_,&symbol_,&value,&image_);
    free(symbol_.dsc$a_pointer);
    free(file);
    if (status == SS$_NORMAL) {
	GTXT_facility = value;
	return( (nl_catd) 1);
    }
    else
	return ( (nl_catd) -1 );
}

/*
**  This procedure secures the LC_MESSAGES logical or symbol or SYS$LANGUAGE
**  system logical and inserts it into the GopherP_Dir:gopher_msg_%s.exe
**  filespec.  If not defined, "*" is inserted, triggering a wildcard search
**  of message files.  The filespec is then searched for, and for each
**  matching filespec found, the message number 222 is retrieved and inserted
**  into a menu list.  If no menu list entries are retrieved, Gtxt() is
**  disabled, causing the default message to be returned.  If only one menu
**  list entry is retrieved, the message file it came from is chosen as the
**  process level message file and Gtxt() is enabled to read it.  If more
**  than one menu list entry is retrieved, the menu is offered to the user,
**  and the choice they make is set as the process level message file and
**  Gtxt() is enabled to read it.
*/

#include <rms.h>
#include <rmsdef.h>

void
rsetlocale(int facility)
{
    Gcatd = (nl_catd) 1;
    GTXT_facility = facility;
    if (GtxtMsgs != NULL)
	DAdestroy(GtxtMsgs);
    GtxtMsgs = (GtxtArray *) DAnew(10, GtxtNew, GtxtInit,
					 GtxtDestroy, GtxtCopy);
}

void
setlocale(char *x, char *y)
{
    int	 status;
    char fname[256];
    char command[256];
    char iso_language[33];
    GopherObj *tmpgs = NULL;
    static struct
	dsc$descriptor_s language_ = {0,DSC$K_DTYPE_T, DSC$K_CLASS_S,0};
    static struct FAB	 wild_fab;  
    static struct NAM	 wild_nam;  
    static char fullname[256];
    static char expanded[256];
    static char result[256];

    iso_language[0] = '\0';

    if (getenv("LC_MESSAGES") != NULL) 
	strcpy(iso_language,getenv("LC_MESSAGES"));
    else {	/*  Test SYS$LANGUAGE using LIB$GET_USERS_LANGUAGE() */
	language_.dsc$w_length = sizeof(iso_language)-1;
	language_.dsc$a_pointer = iso_language;
	if (SS$_NORMAL == (status = LIB$GET_USERS_LANGUAGE(&language_)))
	    iso_language[language_.dsc$w_length] = '\0';
	else
	    iso_language[0] = '\0';
    }

    while (iso_language[0] != '\0' &&
    	   iso_language[strlen(iso_language)-1] == ' ')
	iso_language[strlen(iso_language)-1] = '\0';

    if (iso_language[0] == '\0')
	strcpy(iso_language,"*");

    sprintf(fname, "GOPHERP_DIR:gopher_msg_%s.exe", iso_language);

    /*	Scan for fname; on a hit, activate the image and get menu message */
    wild_fab = cc$rms_fab;
    wild_nam = cc$rms_nam;
    wild_fab.fab$b_fac = FAB$M_GET;
    wild_fab.fab$l_fop = FAB$V_NAM;
    wild_fab.fab$l_nam = &wild_nam;
    wild_fab.fab$l_dna = fname;
    wild_fab.fab$b_dns = strlen(wild_fab.fab$l_dna);
    wild_nam.nam$l_esa = expanded;
    wild_nam.nam$l_rsa = result;
    wild_nam.nam$b_ess = wild_nam.nam$b_rss = 255;
    wild_fab.fab$l_fna = fullname;
    wild_fab.fab$b_fns = fullname[0] = expanded[0] = result[0] = 0;
    if ((status = SYS$PARSE(&wild_fab)) != RMS$_NORMAL)
        return;  /* The while loop will hang if wild_fab is invalid */

    Gcatd = (nl_catd) 1;		        /* Always try to get the msg here   */

    while (status!=RMS$_NMF && status!=RMS$_FNF) {
	if ((( status = SYS$SEARCH(&wild_fab)) &1) != 1)
	    continue;
	result[wild_nam.nam$b_rsl] = '\0';
	if ((nl_catd)1 != catopen(result,0))
	    continue;
	strcpy(command,Gtxt("",222));
	if (strlen(command)) {			/* An available language ...	    */
	    if (setlocale_LangDir == NULL) {
		setlocale_LangDir = GDnew(32);    /*	1st language		    */
		GDsetTitle(setlocale_LangDir,"");
	    }
	    tmpgs = GSnew();			/*	Store language menu item    */
	    GSsetTitle(tmpgs, command);
	    strcpy(command+1,result);
	    GSsetType(tmpgs, command[0] = A_LANGUAGE);
	    GSsetPath(tmpgs, command);
	    GSsetHost(tmpgs,"0.0.0.0");
	    GSsetPort(tmpgs,GTXT_facility);
	    GDaddGS(setlocale_LangDir, tmpgs);
	    GSdestroy(tmpgs);
	}
    }
    Gcatd = (nl_catd) -1;	/* By default, never bother trying a message file.. */
    if (setlocale_LangDir) {	
			    /* We have one or more available languages ...	    */
	if (GDgetNumitems(setlocale_LangDir)==1) {
	    status = 0;					    /*	Only one choice	    */
	    rsetlocale(GSgetPort(GDgetEntry(setlocale_LangDir,0)));
	    GDdestroy(setlocale_LangDir);
	    setlocale_LangDir = NULL;
	}
	else {
	    status = setlocale_screen();
	    rsetlocale(GSgetPort(GDgetEntry(setlocale_LangDir,status)));
	}
    }
}

#else /*** Unix version: ***/

#include "Locale.h"

#ifdef GINTERNATIONAL

#include "Stdlib.h"
#include "String.h"
#include "DAarray.h"
#include "STRstring.h"
#include "fileio.h"
#include <ctype.h>

nl_catd	     Gcatd	 = (nl_catd) -1;

#ifdef NO_XPGCAT
/** if we're not using the X/Open message catalogs, we have to keep
     track of the locale **/
String *msgLocale = NULL;
#endif /* NO_XPGCAT */

/*************** DynArray of messages ***************/
typedef	DynArray GtxtArray;
GtxtArray     *GtxtMsgs	 = (GtxtArray *) NULL;

#define GtxtNew	    STRnew
#define GtxtInit    STRinit
#define GtxtCopy    STRcpy
#define GtxtDestroy STRdestroy

/*************** Public interfaces ***************/

char *
Gtxt (defaultString, msgNumber)
     char *defaultString;
     int msgNumber;
{
     String    *msgString;
     char      *msgText;

     if ( (Gcatd == (nl_catd) -1) )
	  return defaultString;

     if ( GtxtMsgs != NULL ) {
	  if (DAgetNumitems(GtxtMsgs) < msgNumber) {
	       DAgrow(GtxtMsgs, msgNumber);
	  }
	  msgString = (String *) DAgetEntry(GtxtMsgs, msgNumber - 1);
#ifndef NO_XPGCAT
	  if (STRlen(msgString) == 0) {
	       STRset(msgString,
		      catgets(Gcatd, NL_SETD, msgNumber, defaultString));
	  }
#endif
	  if (STRget(msgString) != NULL)
	       return (STRget(msgString));
     }

#ifdef NO_XPGCAT
     return defaultString;
#else
     return (catgets(Gcatd, NL_SETD, msgNumber, defaultString));
#endif
}

nl_catd
Gtxtopen (catname, flags)
     char *catname;
     int   flags;	    /* not used */
{
     FileIO    *fio = NULL;
     char       quotechar = '\0';
     char       inputline[1024];

     if (GtxtMsgs != NULL) {
	  DAdestroy(GtxtMsgs);
     }

#ifdef NO_XPGCAT
     if (strchr(catname,'/') != NULL) { /* file path name */
	  fio = FIOopenUFS(catname, O_RDONLY, 0);
     } else if ( (msgLocale != NULL) && (STRlen(msgLocale) > 0) ) {
	  sprintf(inputline, "%s/%s.msg", LOCALEDIR, STRget(msgLocale));
	  fio = FIOopenUFS(inputline, O_RDONLY, 0);
     }

     if (fio != NULL) {
	  GtxtMsgs = (GtxtArray *) DAnew(10, GtxtNew, GtxtInit,
					 GtxtCopy, GtxtDestroy);

	  if (GtxtMsgs == NULL) {
	       FIOclose(fio);
	       Gcatd = -1;
	       return Gcatd;
	  }

	  while (FIOreadline(fio, inputline, sizeof(inputline))) {
	       if (*inputline == '$') {
		    if (strncmp(inputline + 1, "quote ", 6)==0)
			 quotechar = *(inputline + 7);
	       } else if (isdigit(*inputline)) {
		    char *cp, *cp2;
		    int  msgNumber;

		    msgNumber = atoi(inputline);
		    if (msgNumber != 0) {
			 if (quotechar != '\0') {
			      cp = strchr(inputline, quotechar);
			      if (cp != NULL) {
				   cp2 = cp++;
				   do {
					cp2 = strchr(cp2 + 1, quotechar);
				   } while ((cp2 != NULL) && (*(cp2-1) == '\\'));
				   if (cp2 != NULL)
					*cp2 = '\0';
			      }
			 } else {
			      cp = inputline;
			      while (isdigit(*cp))
				   cp++;
			      cp++; /* skip exactly one char after number */

			      cp2 = cp + strlen(cp);
			      if (*cp2 == '\n')  /* strip trailing return */
				   *cp2 = '\0';
			 }
			 if (cp != NULL) {
			      if (DAgetNumitems(GtxtMsgs) < msgNumber) {
				   DAgrow(GtxtMsgs, msgNumber);
			      }
			      STRset((String *) 
				     DAgetEntry(GtxtMsgs, msgNumber - 1), cp);
			 }
		    }
	       }
	  }
	  FIOclose(fio);
	  Gcatd = 1;	 /* random positive number */
     }
     else
	  Gcatd = -1;
#else
     Gcatd = catopen(catname, flags);

     if (Gcatd != (nl_catd) -1) {
	  GtxtMsgs = (GtxtArray *) DAnew(10, GtxtNew, GtxtInit,
					 GtxtCopy, GtxtDestroy);
     }
#endif /* NO_XPGCAT */

     return Gcatd;
}

char *
Gtxtlocale(type, str)
     int type;
     char *str;
{
#ifdef NO_XPGCAT
# ifdef LC_MESSAGES
     if ( (type == LC_ALL) || (type == LC_MESSAGES) ) {
# else
     if (type == LC_ALL) {
# endif
	  if (*str == '\0') { /* load from LC_MESSAGES or LANG env var */
	       char *cp;

	       cp = getenv("LC_MESSAGES");
	       if (cp == NULL)
		    cp = getenv("LANG");

	       if (cp != NULL) {
		    if (msgLocale == NULL)
			 msgLocale = STRnew();
		    STRset(msgLocale, str);
	       }
	  
	  } else {
	       if (msgLocale == NULL) 
		    msgLocale = STRnew();
	       STRset(msgLocale, str);
	  }
     }
#endif /* NO_XPGCAT */

#ifndef NO_LOCALE
     return (setlocale(type,str));
#else
     if (msgLocale == NULL)
	  return NULL;
     else
	  return STRget(msgLocale);	  
#endif /* NO_LOCALE */
}

#endif /* GINTERNATIONAL */

#endif
