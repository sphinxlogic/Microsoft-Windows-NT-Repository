/*
 *	db.c : Database routines for xkal.
 *	       Handles searching and updating the appointment graph.
 *	       See db.h for a description of the data structure.
 *
 *	George Ferguson, ferguson@cs.rochester.edu, 27 Oct 1990.
 *	Version 1.1 - 27 Feb 1991.
 *
 *	$Id: db.c,v 2.2 91/03/13 13:31:16 ferguson Exp $
 *
 * The following can be used in place of the ones from X11/Intrinsic.h:
 *	#define XtNew(T)	(T *)malloc(sizeof(T))
 *	#define XtFree(X)	if ((X) != NULL) free(X)
 *	#define XtNewString(S)	strcpy(malloc(strlen(S)+1),S)
 *
 */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>			/* for writeDb() backup only */
#include <sys/stat.h>			/*  "	  "	    "	  "  */
#include <X11/Intrinsic.h>
#include "db.h"
#include "util.h"
#include "date-strings.h"
#include "app-resources.h"		/* for levelDelim and daySlashMonth */
#ifdef USE_ALERT
#include "alert.h"
#endif
extern char *program;

/*
 * Functions defined here:
 */
void initDb();
Msgrec *addEntry();
Boolean deleteEntry();
int lookupEntry();
void readDb(), writeDb(), writeAppoint();

/*
 * Data defined here (could be static but we might hack on the DB later,
 * like we do with xkal2xremind and the like).
 */
Yearrec *Db[8];

/*	-	-	-	-	-	-	-	-	*/

/*VARARGS1*/
static void
dprintf(fmt,a1,a2,a3,a4,a5,a6,a7,a8,a9)
char *fmt,*a1,*a2,*a3,*a4,*a5,*a6,*a7,*a8,*a9;
{
#ifdef debug
    printf(fmt,a1,a2,a3,a4,a5,a6,a7,a8,a9);
#endif
}

/*	-	-	-	-	-	-	-	-	*/
/*
 * initDb() : Pretty lame, and unneeded for most UNIX boxes, but I put
 *	it here anyway. We never erase the db, so we don't have to
 *	worry about this.
 */
void
initDb()
{
    int i;

    for (i=0; i < 8; i++)
	Db[i] = NULL;		/* should free them all */
}

/*	-	-	-	-	-	-	-	-	*/
/*
 * addEntry() : Add an entry to the appropriate place in the database
 *	by traversing it and possibly adding nodes. Not much to it.
 */
Msgrec *
addEntry(dow,year,mon,day,hour,mins,text,flag,level)
int dow,year,mon,day,hour,mins;
char *text;
int flag,level;
{
    Yearrec *yp;
    Monrec *mp;
    Dayrec *dp;
    Msgrec *xp;

    if (Db[dow] == NULL) {
	Db[dow] = XtNew(Yearrec);
	Db[dow]->year = year;
	Db[dow]->mons = NULL;
	Db[dow]->next = NULL;
    }
    for (yp = Db[dow]; yp->year != year && yp->next != NULL; yp = yp->next) ;
    if (yp->year != year) {
	yp->next = XtNew(Yearrec);
	yp->next->year = year;
	yp->next->mons = NULL;
	yp->next->next = NULL;
	yp = yp->next;
    }
    if (yp->mons == NULL) {
	yp->mons = XtNew(Monrec);
	yp->mons->mon = mon;
	yp->mons->days = NULL;
	yp->mons->next = NULL;
    }
    for (mp = yp->mons; mp->mon != mon && mp->next != NULL; mp = mp->next) ;
    if (mp->mon != mon) {
	mp->next = XtNew(Monrec);
	mp->next->mon = mon;
	mp->next->days = NULL;
	mp->next->next=NULL;
	mp = mp->next;
    }

    if (mp->days == NULL) {
	mp->days = XtNew(Dayrec);
	mp->days->day = day;
	mp->days->msgs = NULL;
	mp->days->next = NULL;
    }
    for (dp = mp->days; dp->day != day && dp->next != NULL; dp = dp->next) ;
    if (dp->day != day) {
	dp->next = XtNew(Dayrec);
	dp->next->day = day;
	dp->next->msgs = NULL;
	dp->next->next=NULL;
	dp = dp->next;
    }

    if (dp->msgs == NULL) {
	dp->msgs = XtNew(Msgrec);
	dp->msgs->next = NULL;
    } else {
	xp = dp->msgs;
	dp->msgs = XtNew(Msgrec);
	dp->msgs->next = xp;
    }
    dp->msgs->dow = dow;
    dp->msgs->year = year;
    dp->msgs->month = mon;
    dp->msgs->day = day;
    dp->msgs->hour = hour;
    dp->msgs->mins = mins;
    dp->msgs->text = XtNewString(text);
    dp->msgs->system = flag;
    dp->msgs->level = level;
    return(dp->msgs);
}

/*
 * deleteEntry() : Removes an entry matching the given parameters from
 *	the database, and tidies up afterwards. Note that everything must
 *	match, the text as well.
 */
Boolean
deleteEntry(dow,year,mon,day,hour,mins,text)
int dow,year,mon,day,hour,mins;
char *text;
{
    Yearrec *yp,*yp2;
    Monrec *mp,*mp2;
    Dayrec *dp,*dp2;
    Msgrec *xp,*xp2;

    for (yp = Db[dow]; yp != NULL && yp->year != year; yp = yp->next) ;
    if (yp == NULL)
	return(False);
    for (mp = yp->mons; mp != NULL && mp->mon != mon; mp = mp->next) ;
    if (mp == NULL)
	return(False);
    for (dp = mp->days; dp != NULL && dp->day != day; dp = dp->next) ;
    if (dp == NULL)
	return(False);
    for (xp = dp->msgs; xp != NULL; xp = xp2) {
	xp2 = xp->next;
	if (xp->hour == hour && xp->mins == mins &&
						strcmp(xp->text, text) == 0) {
	    XtFree(xp->text);		/* free the text */
	    if (dp->msgs == xp)		/* and the msgrec */
		dp->msgs = xp->next;
	    else {
		for (xp2 = dp->msgs; xp2->next != xp; xp2 = xp2->next);
		xp2->next = xp->next;
	    }
	    XtFree(xp);
	    if (dp->msgs == NULL) {	/* if no more entries, free the day */
		if (mp->days == dp)
		    mp->days = dp->next;
		else {
		    for (dp2 = mp->days; dp2->next != dp; dp2 = dp2->next);
		    dp2->next = dp->next;
		}
		XtFree(dp);
	    }
	    if (mp->days == NULL) {	/* if no more days, free the month */
		if (yp->mons == mp)
		    yp->mons = mp->next;
		else {
		    for (mp2 = yp->mons; mp2->next != mp; mp2 = mp2->next);
		    mp2->next = mp->next;
		}
		XtFree(mp);
	    }
	    if (yp->mons == NULL) {	/* if no more months, free the year */
		if (Db[dow] == yp)
		    Db[dow] = Db[dow]->next;
		else {
		    for (yp2 = Db[dow]; yp2->next != yp; yp2 = yp2->next);
		    yp2->next = yp->next;
		}
		XtFree(yp);
	    }
	    return(True);
	}
    }
    return(False);
}

/*	-	-	-	-	-	-	-	-	*/
/*
 * lookupEntry() : Fills in at most max entries in result[] with pointers
 *	to messages for the given date/time. Returns the number of
 *	entries filled or -1 if there were more than max entries (but the
 *	ones that were filled, ie max of them, are valid). As a special
 *	case, if max is -1, then no entries are filled (result can be NULL)
 *	and the number of entries is returned. (If useLevel is True, then
 *	the total of the levels of all applicable appointments is returned
 *	rather than the number of appointments.)
 *
 *	The definition of a match is complicated by the fact that items in
 *	the database may be only partially specified. The approach is as
 *	follows:
 *	(a) First consider all db records which do not specify a dow (ie.
 *	    from Db[0]). Within these, the day, month, and year must match
 *	    if given, otherwise they are assumed to match. The hour and
 *	    minutes must match explicitly.
 *	(b) Then consider all db records specifying the dow of the given date.
 *	    If the record does not also specify a day, then the month and
 *	    year must match if given, as above, and the time must match
 *	    exactly. If the record specifies both dow and day, then the
 *	    interpretation is "the first dow after date", so we have to
 *	    check if the current date is within a week of the date in the
 *	    record.
 *
 *	Note that the strings are not copied.
 */
int
lookupEntry(day,mon,year,hour,mins,max,result,useLevel)
int day,mon,year,hour,mins,max;
Msgrec *result[];
{
    Yearrec *yp;
    Monrec *mp;
    Dayrec *dp;
    int num,n,i;

    dprintf("lookup for %d:%02d, %d %s %d...\n",hour,mins,day,
						shortMonthStr[mon-1],year);
    num = 0;
    dprintf("  checking DB with dow = 0...\n");
    for (yp = Db[0]; yp != NULL; yp = yp->next) {
	dprintf("    year = %d\n", yp->year);
	if (yp->year == year || yp->year == 0)
	    for (mp = yp->mons; mp != NULL; mp = mp->next) {
		dprintf("      mon = %d\n", mp->mon);
		if (mp->mon == mon || mp->mon == 0)
		    for (dp = mp->days; dp != NULL; dp = dp->next) {
			dprintf("\tday = %d\n", dp->day);
			if (dp->day == day || dp->day == 0) {
			    n = lookupEntryDay(dp,hour,mins,max-num,
							result+num,useLevel);
			    if (n < 0)
				return(-1);
			    else
				num += n;
			}
		}
	}
    }
    dprintf("  checking DB with real dow...\n");
    for (yp = Db[computeDOW(day,mon,year)]; yp != NULL; yp = yp->next) {
	dprintf("    year = %d\n", yp->year);
	for (mp = yp->mons; mp != NULL; mp = mp->next) {
	    dprintf("      mon = %d\n", mp->mon);
	    for (dp = mp->days; dp != NULL; dp = dp->next) {
		dprintf("\tday = %d\n", dp->day);
		if (dp->day == 0) {
		    if ((yp->year == 0 || yp->year == year) &&
			(mp->mon == 0 || mp->mon == mon)) {
			n = lookupEntryDay(dp,hour,mins,max-num,
							result+num,useLevel);
			if (n < 0)
			    return(-1);
			else
			    num += n;
		    }
		} else {	/* have dow and day */
		    int d,m,y;

		    d = day;
		    m = mon;
		    y = year;
		    dprintf("\tscanning back from %d %d %d\n",d,m,y);
		    for (i=0; i < 7; i++)
			if (dp->day == d && (yp->year == 0 || yp->year == y) &&
					    (mp->mon == 0 || mp->mon == m))
			    break;
			else
			    prevDay(&d,&m,&y);
		    if (i < 7) {
			n = lookupEntryDay(dp,hour,mins,max-num,
							result+num,useLevel);
			if (n < 0)
			    return(-1);
			else
			    num += n;
		    }
		}
	    }
	}
    }
    dprintf("returning %d\n", num);
    return(num);
}

/*
 * lookupEntryDay() : Scans the list of messages associated with the
 *	given Dayrec and fills in at most max entries of result[] with
 *	pointers to the messages. Returns the number of entries filled in,
 *	or -1 if there were more entries than max (but all that were filled
 *	in, ie. max of them, are valid).
 *
 *	If the given hour or mins is -1, then it's assumed to match, making
 *	it easy to get all the entries for a day.
 */
static int
lookupEntryDay(dp,hour,mins,max,result,useLevel)
Dayrec *dp;
int hour,mins,max;
Msgrec *result[];
Boolean useLevel;
{
    Msgrec *xp;
    int num;

    num = 0;
    for (xp = dp->msgs; xp != NULL; xp = xp->next) {
	dprintf("\t  h:m = %d:%02d\n",xp->hour,xp->mins);
	if ((hour == -1 || xp->hour == -1 || xp->hour == hour) &&
	    (mins == -1 || xp->mins == -1 || xp->mins == mins)) {
	    dprintf("\t    YES\n");
	    if (max >= 0 && num >= max)
		return (-1);
	    else if (max > 0)
		result[num] = xp;
	    if (useLevel)
		num += xp->level;
	    else
		num += 1;
	}
    }
    return(num);
}

/*	-	-	-	-	-	-	-	-	*/
/*
 * readDb() : Reads the given file into the database, setting the flag
 *	bit on new entries appropriately. This function understands
 *	comments starting with #, and the #include capability. It uses
 *	parseLine to actually decode the date spec.
 */
void
readDb(name,systemFlag)
char *name;
int systemFlag;
{
    FILE *fp;
    char *fname,buf[256];
    int line,i,n,c;
    int dow,y,m,d,h,mins,lev;
    char *t;

    fname = expandFilename(name);
    if ((fp=fopen(fname,"r")) == NULL) {
	if (errno != ENOENT)	/* appointment file may not exist */
	    perror(fname);
	return;
    }
    c = line = 0;
    while (c != EOF) {
	n = 0;
	line += 1;
	while (n < 255 && (c=getc(fp)) != EOF && c != '\n')
	    buf[n++] = c;
	buf[n] = '\0';
	if (n == 0) {
	    continue;
	} else if (c != EOF && c != '\n') {
	    fprintf(stderr,"%s: line %d too long, file %s\n",program,
								line,fname);
	    while ((c=getc(fp)) != EOF && c != '\n') ;
	}
	i = 0;
	while (isspace(buf[i]))
	    i += 1;
	if (buf[i] == '#') {
	    if (strncmp(buf+i+1,"include",7) == 0) {
		i += 8;
		while (isspace(buf[i]))
		    i += 1;
		readDb(buf+i,systemFlag);
	    }
	    continue;
	}
	parseLine(buf+i,&dow,&y,&m,&d,&h,&mins,&t,&lev);
#ifdef debug
	if (dow != 0)
	    printf("%s ",shortDowStr[dow-1]);
	if (d != 0)
	    printf("%2d ",d);
	if (m != 0)
	    printf("%s ",shortMonthStr[m-1]);
	if (y != 0)
	    printf("%4d ",y);
	if (h != -1)
	    printf("%2d:%02d ",h,mins);
	if (lev != 0)
	    printf("@%d@ ",lev);
	printf("%s\n",t);
#endif
	addEntry(dow,y,m,d,h,mins,t,systemFlag,lev);
    }
    fclose(fp);
}

/*
 * writeDb() : Writes any non-system database entries to the given file
 *	in the canonical format. If writeAll is True, then system entries are
 *	also written.
 */
void
writeDb(name,writeAll)
char *name;
Boolean writeAll;
{
    FILE *fp;
    char *fname,*backup;
    struct stat stbuf;
    int dow;
    Yearrec *yp;
    Monrec *mp;
    Dayrec *dp;
    Msgrec *xp;

    fname = expandFilename(name);
    if (*appResources.backupExtension && stat(fname,&stbuf) == 0) {
	backup = XtMalloc(strlen(fname)+strlen(appResources.backupExtension)+1);
	strcpy(backup,fname);
	strcat(backup,appResources.backupExtension);
	(void)unlink(backup);
	if (link(fname,backup) < 0) {
#ifdef USE_ALERT
	    alert("Error: Can't backup \"%s\"; save aborted.",fname);
#else
	    fprintf(stderr,"\007%s: can't backup \"%s\"; save aborted\n",program,fname);
#endif
	    XtFree(backup);
	    return;
	}
	(void)unlink(fname);
	XtFree(backup);
    }
    if ((fp=fopen(fname,"w")) == NULL) {
	perror(fname);
	return;
    }
    for (dow = 0; dow < 8; dow++)
      for (yp = Db[dow]; yp != NULL; yp = yp->next)
	for (mp = yp->mons; mp != NULL; mp = mp->next)
	    for (dp = mp->days; dp != NULL; dp = dp->next)
		for (xp = dp->msgs; xp != NULL; xp = xp->next)
		    if (!xp->system || writeAll)
			writeAppoint(fp,xp);

    fclose(fp);
}

/*
 * writeAppoint(fp,msg) : Formats the msg onto the given stream.
 *
 * The following escapes are possible in appResources.outputFormat:
 *	%w	short form of day of the week
 *	%W	long form of day of the week
 *	%d	day
 *	%m	short form of month
 *	%M	long form of month
 *	%n	numeric form of month
 *	%y	year
 *	%Y	year modulo 100
 *	%t	time
 *	%l	criticality level
 *	%~	space iff previous pattern was printed
 *	%/	slash iff previous pattern was printed
 * Any other character is simply printed. The text of the appointment follows,
 * then a newline.
 */
void
writeAppoint(fp,msg)
FILE *fp;
Msgrec *msg;
{
    char *s;
    Boolean flag;

    s = appResources.outputFormat;
    flag = False;
    while (*s) {
	if (*s == '%') {
	    s += 1;
	    switch (*s) {
		case '\0': fprintf(fp,"%");
			   break;
		case '~': if (flag)
			      fprintf(fp," ");
			   break;
		case '/': if (flag)
			      fprintf(fp,"/");
			   break;
		case 'w': if (msg->dow > 0)
			      fprintf(fp,"%s",shortDowStr[msg->dow-1]);
			  flag = (msg->dow > 0);
			  break;
		case 'W': if (msg->dow > 0)
			      fprintf(fp,"%s",longDowStr[msg->dow-1]);
			  flag = (msg->dow > 0);
			  break;
		case 'd': if (msg->day > 0)
			      fprintf(fp,"%d",msg->day);
			  flag = (msg->day > 0);
			  break;
		case 'm': if (msg->month > 0)
			      fprintf(fp,"%s",shortMonthStr[msg->month-1]);
			  flag = (msg->month > 0);
			  break;
		case 'M': if (msg->month > 0)
			      fprintf(fp,"%s",longMonthStr[msg->month-1]);
			  flag = (msg->month > 0);
			  break;
		case 'n': if (msg->month > 0)
			      fprintf(fp,"%d",msg->month);
			  flag = (msg->month > 0);
			  break;
		case 'y': if (msg->year > 0)
			      fprintf(fp,"%d",msg->year);
			  flag = (msg->year > 0);
			  break;
		case 'Y': if (msg->year > 0)
			      fprintf(fp,"%d",msg->year%100);
			  flag = (msg->year > 0);
			  break;
		case 't': if (msg->hour != -1)
			     fprintf(fp,"%s",timetostr(msg->hour*60+msg->mins));
			  flag = (msg->hour != -1);
			  break;
		case 'l': if (msg->level > 0) {
			      fprintf(fp,"%c",*appResources.levelDelim);
			      fprintf(fp,"%d",msg->level);
			      if (*(appResources.levelDelim+1))
				  fprintf(fp,"%c",*(appResources.levelDelim+1));
			      else
				  fprintf(fp,"%c",*appResources.levelDelim);
			  }
			  flag = (msg->level > 0);
			  break;
		default: fprintf(fp,"%c",*s);
	    } /* switch */
	} else {
	   fprintf(fp,"%c",*s);
	}
	if (*s)
	    s += 1;
    } /* while */
    fprintf(fp,"%s\n",msg->text);
}
