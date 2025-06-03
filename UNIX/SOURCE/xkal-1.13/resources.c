/*
 *	resources.c: Defines the application resources used
 *		     in XtAppInitialize().
 *
 *	Also, getResources() is called instead of initGraphics() when
 *	we're in non-interactive mode, to try and get the few resources
 *	we need to avoid opening a display.
 *
 *	Needs XAPPLOADDIR defined during compilation.
 *
 *	George Ferguson, ferguson@cs.rochester.edu, 27 Feb 1991.
 *
 *	$Id: resources.c,v 2.4 91/03/13 13:31:31 ferguson Exp $
 *
 */
#include <stdio.h>
#include <ctype.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include "xkal.h"
#include "app-resources.h"
extern char *getenv();

/*
 * Functions defined here:
 */
void getResources();
static void readResourceValue(),setResourceValue();

/*
 * Data defined here: (check resources.h if these change!!)
 */
/*
 *	Non-widget resources settable on command line.
 */
XrmOptionDescRec xkalOptions[] = {
    { "-appoints",	".personalAppoints",XrmoptionSepArg,	"~/.appoints" },
    { "-date",		".date",	XrmoptionSepArg,	"" },
    { "-numMonths",	".numMonths",	XrmoptionSepArg,	"1" },
    { "-bothShown",	".bothShown",	XrmoptionNoArg,		"True" },
    { "-nobothShown",	".bothShown",	XrmoptionNoArg,		"False" },
    { "-version",	".version",	XrmoptionNoArg,		"True" },
    { "-listOnly",	".listOnly",	XrmoptionNoArg,		"True" },
    { "-silent",	".silent",	XrmoptionNoArg,		"True" },
    { "-exitUsesLevels",".exitUsesLevels",XrmoptionNoArg,	"True" },
    { "-noexitUsesLevels",".exitUsesLevels",XrmoptionNoArg,	"False" },
    { "-opaqueDates",	".opaqueDates",	XrmoptionNoArg,		"True" },
    { "-noopaqueDates",	".opaqueDates",	XrmoptionNoArg,		"False" },
    { "-dowLabels",	".dowLabels",	XrmoptionNoArg,		"True" },
    { "-nodowLabels",	".dowLabels",	XrmoptionNoArg,		"False" },
    { "-titlebar",	".useTitlebar",	XrmoptionNoArg,		"True" },
    { "-notitlebar",	".useTitlebar",	XrmoptionNoArg,		"False" },
    { "-checkpointInterval",".checkpointInterval",XrmoptionSepArg,"5" },
};
/*
 * Non-widget resources obtained from the resource manager
 */
XtResource xkalResources[] = {
    { "systemAppoints", "String", XtRString, sizeof(String),
      XtOffset(AppResources *,systemAppoints), XtRImmediate, "" },
    { "personalAppoints", "String", XtRString, sizeof(String),
      XtOffset(AppResources *,personalAppoints), XtRImmediate, "~/.appoints" },
    { "backupExtension", "String", XtRString, sizeof(String),
      XtOffset(AppResources *,backupExtension), XtRImmediate, "~" },
    { "outputFormat", "String", XtRString, sizeof(String),
      XtOffset(AppResources *,outputFormat), XtRImmediate,
						"%w%~%d%~%m%~%y%~%t%~%l%~" },
    { "daySlashMonth", "Boolean", XtRBoolean, sizeof(Boolean),
      XtOffset(AppResources *,daySlashMonth), XtRImmediate, (XtPointer)False },

    { "date", "String", XtRString, sizeof(String),
      XtOffset(AppResources *,date), XtRImmediate, "" },
    { "numMonths", "Int", XtRInt, sizeof(int),
      XtOffset(AppResources *,numMonths), XtRImmediate, (XtPointer)1 },
    { "bothShown", "Boolean", XtRBoolean, sizeof(Boolean),
      XtOffset(AppResources *,bothShown), XtRImmediate, (XtPointer)True },
    { "useTitlebar", "Boolean", XtRBoolean, sizeof(Boolean),
      XtOffset(AppResources *,useTitlebar), XtRImmediate, (XtPointer)False },
    { "version", "Boolean", XtRBoolean, sizeof(Boolean),
      XtOffset(AppResources *,version), XtRImmediate, (XtPointer)False },
    { "listOnly", "Boolean", XtRBoolean, sizeof(Boolean),
      XtOffset(AppResources *,listOnly), XtRImmediate, (XtPointer)False },
    { "silent", "Boolean", XtRBoolean, sizeof(Boolean),
      XtOffset(AppResources *,silent), XtRImmediate, (XtPointer)False },
    { "exitUsesLevels", "Boolean", XtRBoolean, sizeof(Boolean),
      XtOffset(AppResources *,exitUsesLevels), XtRImmediate, (XtPointer)False },
    { "opaqueDates", "Boolean", XtRBoolean, sizeof(Boolean),
      XtOffset(AppResources *,opaqueDates), XtRImmediate, (XtPointer)False },
    { "checkpointInterval", "Int", XtRInt, sizeof(int),
      XtOffset(AppResources *,checkpointInterval),XtRImmediate,(XtPointer)5 },

    { "dowLabels", "Boolean", XtRBoolean, sizeof(Boolean),
      XtOffset(AppResources *,dowLabels), XtRImmediate, (XtPointer)False },
    { "longDowStrings", "String", XtRString, sizeof(String),
      XtOffset(AppResources *,longDowStrings), XtRImmediate,
			"Sunday Monday Tuesday Wednesday Thursday \
			 Friday Saturday" },
    { "shortDowStrings", "String", XtRString, sizeof(String),
      XtOffset(AppResources *,shortDowStrings), XtRImmediate,
			"Sun Mon Tue Wed Thu Fri Sat" },
    { "longMonthStrings", "String", XtRString, sizeof(String),
      XtOffset(AppResources *,longMonthStrings), XtRImmediate,
			"January February March April May June \
		 	 July August September October November December" },
    { "shortMonthStrings", "String", XtRString, sizeof(String),
      XtOffset(AppResources *,shortMonthStrings), XtRImmediate,
			"Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec" },
    { "dowOffset", "Int", XtRInt, sizeof(int),
      XtOffset(AppResources *,dowOffset), XtRImmediate, (XtPointer)0 },

    { "appsStartTime", "String", XtRString, sizeof(String),
      XtOffset(AppResources *,appsStartTime), XtRImmediate, "8:00" },
    { "appsEndTime", "String", XtRString, sizeof(String),
      XtOffset(AppResources *,appsEndTime), XtRImmediate, "17:00" },
    { "appsIncrement", "String", XtRString, sizeof(String),
      XtOffset(AppResources *,appsIncrement), XtRImmediate, ":30" },
    { "appsUseAmPm", "Boolean", XtRBoolean, sizeof(Boolean),
      XtOffset(AppResources *,appsUseAmPm), XtRImmediate, (XtPointer)False },
    { "numNotes", "Int", XtRInt, sizeof(int),
      XtOffset(AppResources *,numNotes), XtRImmediate, (XtPointer)2 },
    { "notesLabel", "String", XtRString, sizeof(String),
      XtOffset(AppResources *,notesLabel), XtRImmediate, "NOTES" },
    { "rearrangeSilently", "Boolean", XtRBoolean, sizeof(Boolean),
      XtOffset(AppResources *,rearrangeSilently),XtRImmediate,(XtPointer)False},
    { "noonStr", "String", XtRString, sizeof(String),
      XtOffset(AppResources *,noonStr), XtRImmediate, "Noon" },
    { "midnightStr", "String", XtRString, sizeof(String),
      XtOffset(AppResources *,midnightStr), XtRImmediate, "Midnight" },

    { "levelDelim", "String", XtRString, sizeof(String),
      XtOffset(AppResources *,levelDelim),XtRImmediate,"@@" },
    { "maxLevel", "Int", XtRInt, sizeof(int),
      XtOffset(AppResources *,maxLevel), XtRImmediate, (XtPointer)7 },
    { "defaultLevel", "Int", XtRInt, sizeof(int),
      XtOffset(AppResources *,defaultLevel), XtRImmediate, (XtPointer)1 },

    { "dateFont1", "Font", XtRFont, sizeof(Font),
      XtOffset(AppResources *,dateFont1), XtRString, "*fixed*bold*" },
    { "dateFont2", "Font", XtRFont, sizeof(Font),
      XtOffset(AppResources *,dateFont2), XtRString, "*fixed*bold*" },
    { "dateFont3", "Font", XtRFont, sizeof(Font),
      XtOffset(AppResources *,dateFont3), XtRString, "*fixed*bold*" },
    { "dateFont12", "Font", XtRFont, sizeof(Font),
      XtOffset(AppResources *,dateFont12), XtRString, "*fixed*bold*" },

    { "datePosition1", "String", XtRString, sizeof(String),
      XtOffset(AppResources *,datePosition1), XtRImmediate, "+10+10" },
    { "datePosition2", "String", XtRString, sizeof(String),
      XtOffset(AppResources *,datePosition2), XtRImmediate, "+10+10" },
    { "datePosition3", "String", XtRString, sizeof(String),
      XtOffset(AppResources *,datePosition3), XtRImmediate, "+10+10" },
    { "datePosition12", "String", XtRString, sizeof(String),
      XtOffset(AppResources *,datePosition12), XtRImmediate, "+10+10" },

    { "dateWidth1", "Int", XtRInt, sizeof(int),
      XtOffset(AppResources *,dateWidth1), XtRImmediate, (XtPointer)50 },
    { "dateWidth2", "Int", XtRInt, sizeof(int),
      XtOffset(AppResources *,dateWidth2), XtRImmediate, (XtPointer)18 },
    { "dateWidth3", "Int", XtRInt, sizeof(int),
      XtOffset(AppResources *,dateWidth3), XtRImmediate, (XtPointer)18 },
    { "dateWidth12", "Int", XtRInt, sizeof(int),
      XtOffset(AppResources *,dateWidth12), XtRImmediate, (XtPointer)10 },

    { "dateHeight1", "Int", XtRInt, sizeof(int),
      XtOffset(AppResources *,dateHeight1), XtRImmediate, (XtPointer)50 },
    { "dateHeight2", "Int", XtRInt, sizeof(int),
      XtOffset(AppResources *,dateHeight2), XtRImmediate, (XtPointer)18 },
    { "dateHeight3", "Int", XtRInt, sizeof(int),
      XtOffset(AppResources *,dateHeight3), XtRImmediate, (XtPointer)18 },
    { "dateHeight12", "Int", XtRInt, sizeof(int),
      XtOffset(AppResources *,dateHeight12), XtRImmediate, (XtPointer)10 },

    { "noDayShade", "String", XtRString, sizeof(String),
      XtOffset(AppResources *,noDayShade), XtRImmediate, "wide_weave" },
    { "shades", "String", XtRString, sizeof(String),
      XtOffset(AppResources *,shades), XtRImmediate,
		"gray3 gray3 light_gray light_gray gray gray flipped_gray" },
    { "colors", "String", XtRString, sizeof(String),
      XtOffset(AppResources *,colors), XtRImmediate,
		"Green Green Blue Blue Yellow Yellow Red" },

    { "revision", "Revision", XtRString, sizeof(String),
      XtOffset(AppResources *,revision), XtRImmediate, "" },
};

/*
 * Resources that we need for non-interactive mode.
 * See also setResourceValue() if you change the order or content of these.
 */
#define NUM_RESOURCES 17
static char *strings[NUM_RESOURCES] = {
	"systemAppoints",
	"personalAppoints",
	"date",
	"listOnly",
	"silent",
	"exitUsesLevels",
	"longDowStrings",
	"shortDowStrings",
	"longMonthStrings",
	"shortMonthStrings",
	"daySlashMonth",
	"levelDelim",
	"backupExtension",
	"version",
	"notesLabel",		/* needed for listAppoints() */
	"noonStr",
	"midnightStr",
};
static int strpos[NUM_RESOURCES];

static char className[] = "Xkal";

#define SKIPSPACE(FP,C)	while (C == ' ' || C == '\t') C = getc(FP);
#define SKIPWHITE(FP,C)	while (C == ' ' || C == '\t' || C == '\n') C = getc(FP);
#define SKIPTOEOL(FP,C)	while (C != EOF && C != '\n') C = getc(FP);

/*	-	-	-	-	-	-	-	-	*/
/*
 *	Uses the array xkalResources[] for the defaults, looks for an
 *	app-defaults file in either $XAPPLRESDIR or XAPPLOADDIR, and
 *	checks command line, in that order (doesn't check ~/.Xdefaults,
 *	yet). Modifies *argcp and argv to remove parsed options.
 *
 *	This allows us to get resources without opening the connection
 *	to the X server.
 *
 */
void
getResources(argcp,argv)
int *argcp;
char **argv;
{
    FILE *fp;
    char *s,**av,**retargv,buf[1024];
    int i,j,c,argc;

    for (i=0 ; i < XtNumber(xkalResources); i++)
	for (j=0; j < NUM_RESOURCES; j++)
	    if (strcmp(strings[j],xkalResources[i].resource_name) == 0)
		setResourceValue(j,xkalResources[i].default_addr);
    if ((s=getenv("XAPPLRESDIR")) != NULL)
	strcpy(buf,s);
    else
	strcpy(buf,XAPPLOADDIR);
    strcat(buf,"/");
    strcat(buf,className);
    if ((fp=fopen(buf,"r")) != NULL) {
	while ((c=getc(fp)) != EOF) {
	    /* skip leading whitespace */
	    SKIPWHITE(fp,c);
	    /* got a comment then skip to EOL */
	    if (c == '!') {
		SKIPTOEOL(fp,c);
		continue;
	    }
	    /* skip class name and . or * after it if they are present */
	    i = 0;
	    while (c == className[i++])
		c = getc(fp);
	    if (c == '.' || c == '*')
		c = getc(fp);
	    /* all strings can potentially match so reset pos array */
	    for (i=0; i < NUM_RESOURCES; i++)
		strpos[i] = 0;
	    /* gather characters in resource name and match strings */
	    while (c != EOF && c != '\n' && c != '.' && c != '*') {
		for (i=0; i < NUM_RESOURCES; i++) {
		    /* if already mismatched then skip this possibility */
		    if (strpos[i] == -1)
			continue;
		    /* if matched resource name then set it */
		    if (c == ':' && strings[i][strpos[i]] == '\0') {
			readResourceValue(fp,buf);
			setResourceValue(i,XtNewString(buf));
			/* done with this line */
			break;
		    } else if (c == strings[i][strpos[i]]) {
			/* haven't matched yet, but still ok */
			strpos[i] += 1;
		    } else {
			/* can't be this resource since mismatch */
			strpos[i] = -1;
		    }
		} /* for i = 0 to NUM_RESOURCES */
		c = getc(fp);
	    } /* while c != '\n' */
	} /* while !feof(fp)) */
	fclose(fp);
    }
    retargv = argv + 1;
    for (argc = *argcp-1, av = argv+1; argc; argc--,av++) {
	if (strncmp(*av,"-a",2) == 0) {
	    setResourceValue(1,*(av+1));
	    av += 1;
	    argc -= 1;
	    *argcp -= 2;
	} else if (strncmp(*av,"-d",2) == 0) {
	    setResourceValue(2,*(av+1));
	    av += 1;
	    argc -= 1;
	    *argcp -= 2;
	} else if (strncmp(*av,"-l",2) == 0) {
	    setResourceValue(3,(char *)True);
	    *argcp -= 1;
	} else if (strncmp(*av,"-s",2) == 0) {
	    setResourceValue(4,(char *)True);
	    *argcp -= 1;
	} else if (strncmp(*av,"-e",2) == 0) {
	    setResourceValue(5,(char *)True);
	    *argcp -= 1;
	} else if (strncmp(*av,"-v",2) == 0) {
	    setResourceValue(13,(char *)True);
	    *argcp -= 1;
	} else {
	    *retargv++ = *av;
	}
    }
}

/*
 * readResourceValue(fp): Assumes that fp just read `:'. Fills in buf with
 * the resource value and eats the final newline. Newlines can be escaped with
 * backslashes.
 */
static void
readResourceValue(fp,buf)
FILE *fp;
char buf[];
{
    int c,n;
    Boolean slash;

    c = getc(fp);
    SKIPSPACE(fp,c);
    if (c == EOF || c == '\n') {
	buf[0] = '\0';
	return;
    }
    n = 0;
    slash = False;
    do {
	/* got newline */
	if (c == '\n') {
	    if (slash) {
	    /* it was escaped with backslash */
		slash = False;
		continue;
	    } else {
		/* otherwise its end of the resource */
		break;
	    }
	}
	if (slash)
	    /* last backslash wasn't escaping \n so add it */
	    buf[n++] = '\\';
	if (c == '\\') {
	    /* we just had a backslash (maybe an escaped one) */
	    slash = !slash;
	} else {
	    /* otherwise we want this char */
	    buf[n++] = c;
	    slash = False;
	}
    } while ((c=getc(fp)) != EOF && n < 1022);
    buf[n] = '\0';
    if (n == 1023 && c != EOF)
	fprintf(stderr,"%s: resource line too long for dumb parser: \"%s...\"\n",program,buf);
}

/*
 * setResourceValue() : Decodes which into the actual resource entry to set,
 *	and sets it.
 */
static void
setResourceValue(which,value)
int which;
char *value;
{
    Boolean bool;

    if (which == 3 || which == 4 || which == 5 || which == 10 || which == 13)
	if ((int)value > 1)
	    bool = (strcasecmp(value,"True") == 0 || strcmp(value,"1") == 0);
	else
	    bool = (Boolean)value;
    switch (which) {
	case 0: appResources.systemAppoints = value;
		break;
	case 1: appResources.personalAppoints = value;
		break;
	case 2: appResources.date = value;
		break;
	case 3: appResources.listOnly = bool;
		break;
	case 4: appResources.silent = bool;
		break;
	case 5: appResources.exitUsesLevels = bool;
		break;
	case 6: appResources.longDowStrings = value;
		break;
	case 7: appResources.shortDowStrings = value;
		break;
	case 8: appResources.longMonthStrings = value;
		break;
	case 9: appResources.shortMonthStrings = value;
		break;
	case 10: appResources.daySlashMonth = bool;
		 break;
	case 11: appResources.levelDelim = value;
		 break;
	case 12: appResources.backupExtension = value;
		 break;
	case 13: appResources.version = bool;
		 break;
	case 14: appResources.notesLabel = value;
		 break;
	case 15: appResources.noonStr = value;
		 break;
	case 16: appResources.midnightStr = value;
		 break;
    }
}
