/*
 *	util.c : Some miscellaneous routines for handling dates.
 *
 *	George Ferguson, ferguson@cs.rochester.edu, 27 Oct 1990.
 *	Version 1.1 - 27 Feb 1991.
 *
 *      $Id: util.c,v 2.2 91/03/13 13:31:41 ferguson Exp $
 *
 */

#include <sys/time.h>
#include <ctype.h>
#include "date-strings.h"		/* for parseDate() */
#include "app-resources.h"		/* for appsUseAmPm, etc. */
extern char *getenv();			/* for $HOME */
extern char *program;			/* for error messages */

/*
 * Functions defined here:
 */
int computeDOW(), firstDOW(), lastDay();
void nextDay(),prevDay(),getCurrentDate();
int parseDate();
void parseLine();
char *expandFilename();
int strtotime();
char *timetostr();

/*
 * Data defined here:
 */
static int mon_max[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
static char filename[1024];

/*	-	-	-	-	-	-	-	-	*/
/*
 *	computeDOW(day,mon,year) : Returns the day of the week for the
 *		requested date (1=Sunday, 2=Monday, etc).
 *		This newer, faster, method is courtesy findeis@alberta.
 */
int
computeDOW(d,m,y)
int d,m,y;
{
    static int dp [12] = {0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5};
    register int y4, s;

    y = y - 1901;
    s = (y/4) * 5;
    y4 = y % 4;
    if ((y4 == 3) && (m>2)) s++;
    s = s + y4 + dp[m-1] + d + 1;
    return (s % 7 + 1);
}

/*
 *	firstDOW(mon,year) : Returns the day of the week for the first
 *		day of the given month and year.
 */
int
firstDOW(mon,year)
int mon,year;
{
    return(computeDOW(1,mon,year));
}

/*
 *	lastDay(mon,year) : Returns the last day of the requested
 *		month (and handles leap years, etc).
 */
int
lastDay(mon,year)
int mon,year;
{
    if ((mon == 1)&&(dysize(year) == 366))
	    return(29);
    else return(mon_max[mon-1]);
}

/*
 *	nextDay(d,m,y) : Increment d,m,y to the next day
 */
void
nextDay(dp,mp,yp)
int *dp,*mp,*yp;
{
    if (*dp != lastDay(*mp,*yp))
	*dp += 1;
    else {
	*dp = 1;
	if (*mp != 12)
	    *mp += 1;
	else {
	    *yp += 1;
	    *mp = 1;
	}
    }
}

/*
 *	prevDay(d,m,y) : Decrement d,m,y to the previous day
 */
void
prevDay(dp,mp,yp)
int *dp,*mp,*yp;
{
    if (*dp != 1)
	*dp -= 1;
    else {
	if (*mp != 1)
	    *mp -= 1;
	else {
	    *yp -= 1;
	    *mp = 12;
	}
	*dp = lastDay(*mp,*yp);
    }
}

/*	-	-	-	-	-	-	-	-	*/
/*
 *	getCurrentDate() : Day is 1-31, Month is 1-12, Year is 1900-.
 */
void
getCurrentDate(dp,mp,yp)
int *dp,*mp,*yp;
{
  struct timeval t;
  struct timezone tz;
  struct tm *r;

  gettimeofday(&t,&tz);
  r = localtime(&(t.tv_sec));
  *dp = r->tm_mday;
  *mp = r->tm_mon+1;
  *yp = r->tm_year+1900;
}

/*	-	-	-	-	-	-	-	-	*/
/*
 *	parseDate(text,dp,mp,yp) : Parse the string text into a date which is
 *		either an absolute or a relative date as follows:
 *
 *	rel date = [+-]{<num>[dmy]}*
 *	abs date = [<day><mon><year>]* where <day> = <num> less than 32
 *					     <mon> = <string>
 *					     <year>= <num> greater than 32
 *	The variables pointed to by dp,mp,yp should contain the current date
 *	and are filled in with the new date.
 *	Whitespace is skipped.
 *	Returns -1 if the date was garbled, else 0.
 */
int
parseDate(text,dp,mp,yp)
char *text;
int *dp,*mp,*yp;
{
    int day,mon,year,last,num,sign;

    day = mon = year = 0;
    last = lastDay(*mp,*yp);
    if (*text == '+' || *text == '-') {	/* relative date */
	sign = *text++;			/* save sign */
	while (*text) {			/* parse string... */
	    while (isspace(*text))			/* skip white space */
		text += 1;
	    num = 0;
	    while (*text >= '0' && *text <= '9') {	/* get a number */
		num = num * 10 + *text - '0';
		text += 1;
	    }
	    switch(*text) {				/* and a specifier */
		case '\0':		/* no specifier => days */
		case 'D' :
		case 'd' : day = num;
			   break;
		case 'M' :
		case 'm' : mon = num;
			   break;
		case 'Y' :
		case 'y' : year = num;
			   break;
		default: return(-1);
	    }
	    if (*text != '\0')			/* continue unless at end */
		text += 1;
	}
	if (sign == '+') {		/* now set the `current' date */
	    *dp += day;
	    if (*dp > last) {
		*dp -= last;
		*mp += 1;
	    }
	    *mp += mon;
	    if (*mp > 12) {
		*mp -= 12;
		*yp += 1;
	    }
	    *yp += year;
	} else {
	    *yp -= year;
	    *mp -= mon;
	    if (*mp < 1) {
		*mp += 12;
		*yp -= 1;
	    }
	    *dp -= day;
	    if (*dp < 1) {
		*mp -= 1;
		if (*mp < 1) {
		    *mp = 12;
		    *yp -= 1;
		}
		*dp += last;
	    }
	}
    } else {				/* absolute date, use parser */
	int dow,y,m,d,h,mins,l;
	char *t;

	parseLine(text,&dow,&y,&m,&d,&h,&mins,&t,&l);
	if (y != 0)
	    *yp = y;
	if (m != 0)
	    *mp = m;
	if (d != 0)
	    *dp = d;
    }
    return(0);
}

/*
 * parseLine() : Given a text buffer presumed to be a (possibly incomplete)
 *	date spec followed by the text of the reminder, this function
 *	parses it as best it can and sets the various parameters. The
 *	following are understood (where # is any consecutive run of digits):
 *		#:#	=	hours:mins (24hr)
 *		#:#am	=	hours:mins
 *		#:#pm	=	hours+12:mins
 *		#/#	=	month/day (depends on daySlashMonth)
 *		#/#/#	=	month/day/year ("   "        "     )
 *		#am	=	hours
 *		#pm	=	hours (+12)
 *		# <= 31	=	day
 *		# > 31	=	year
 *		@#@	=	level (depends on levelDelim)
 *	Also, either the long or short forms of months and days of the week
 *	set the appropriate parameter. Parsing stops (and the text is assumed
 *	to start) at the first non-numerical string which cannot be
 *	interpreted as one of these.
 *
 *	If parameters weren't set, they are set to 0, except hours and mins
 *	which are set to -1 (since 0 is a valid value).
 */
void
parseLine(buf,dowp,yp,mp,dp,hp,minsp,tp,lp)
char buf[];
int *dowp,*yp,*mp,*dp,*hp,*minsp;
char **tp;
int *lp;
{
    int i,j,n;
    char word[32];

    *dowp = *yp = *mp = *dp = *lp = 0;
    *hp = *minsp = -1;
    *tp = NULL;
    i = 0;
    while (buf[i]) {
	while (isspace(buf[i]))
	    i += 1;
	if (!buf[i]) {
	    break;
	} else if (isdigit(buf[i])) {
	    n = 0;
	    while (isdigit(buf[i]))
		n = n * 10 + buf[i++] - '0';
	    if (buf[i] == ':') {
		*hp = n;
		i += 1;
		*minsp = 0;
		while (isdigit(buf[i]))
		    *minsp = *minsp * 10 + buf[i++] - '0';
		if (buf[i] == 'p' && buf[i+1] == 'm' && *hp < 12) {
		    *hp += 12;
		    i += 2;
		} else if (buf[i] == 'a' && buf[i+1] == 'm')
		    i += 2;
	    } else if (buf[i] == 'p' && buf[i+1] == 'm') {
		*hp = n + 12;
		*minsp = 0;
		i += 2;
	    } else if (buf[i] == 'a' && buf[i+1] == 'm') {
		*hp = n;
		*minsp = 0;
		i += 2;
	    } else if (buf[i] == '/') {
		*mp = n;
		i += 1;
		*dp = 0;
		while (isdigit(buf[i]))
		    *dp = *dp * 10 + buf[i++] - '0';
		if (buf[i] == '/') {
		    i += 1;
		    *yp = 0;
		    while (isdigit(buf[i]))
			*yp = *yp * 10 + buf[i++] - '0';
		}
		if (appResources.daySlashMonth && *dp <= 12) {
		    n = *mp;
		    *mp = *dp;
		    *dp = n;
		}
	    } else if (n <= 31) {
		*dp = n;
	    } else {
		*yp = n;
	    }
	} else if (buf[i] == *(appResources.levelDelim)) {
	    i += 1;
	    *lp = 0;
	    while (isdigit(buf[i]))
		*lp = *lp * 10 + buf[i++] - '0';
	    i += 1;
	} else {		/* !isdigit(buf[i]) */
	    j = 0;
	    do {
		word[j++] = buf[i++];
	    } while (isalpha(buf[i]) && j < 32);
	    word[j] = '\0';
	    if (strcasecmp(word, appResources.noonStr) == 0) {
		*hp = 12;
		*minsp = 0;
		continue;
	    } else if (strcasecmp(word, appResources.midnightStr) == 0) {
		*hp = *minsp = 0;
		continue;
	    }
	    for (j = 0; j < 7; j++)
		if (strcasecmp(word, longDowStr[j]) == 0)
		    break;
	    if (j != 7) {
		*dowp = j + 1;
		continue;
	    }
	    for (j = 0; j < 12; j++)
		if (strcasecmp(word, longMonthStr[j]) == 0)
		    break;
	    if (j != 12) {
		*mp = j + 1;
		continue;
	    }
	    for (j = 0; j < 7; j++)
		if (strcasecmp(word, shortDowStr[j]) == 0)
		    break;
	    if (j != 7) {
		*dowp = j + 1;
		if (buf[i] == '.')
		    i += 1;
		continue;
	    }
	    for (j = 0; j < 12; j++)
		if (strcasecmp(word, shortMonthStr[j]) == 0)
		    break;
	    if (j != 12) {
		*mp = j + 1;
		if (buf[i] == '.')
		    i += 1;
		continue;
	    }
	    *tp = buf + i - strlen(word);
	    break;		/* start of text, apparently */
	}
    }
}


/*	-	-	-	-	-	-	-	-	*/
/*
 * expandFilename() : Replaces a leading tilde by the value of $HOME,
 *	and returns a pointer to the resulting filename.
 */
char *
expandFilename(name)
char *name;
{
    char *s;

    if (*name == '~') {
	if ((s=getenv("HOME")) == NULL) {
	    strcpy(filename,name);
	} else {
	    strcpy(filename,s);
	    strcat(filename,name+1);
	}
    } else {
	strcpy(filename,name);
    }
    return(filename);
}

/*	-	-	-	-	-	-	-	-	*/
/* Minutes to/from ascii conversions */

/*
 * strtotime(s) : Returns number of minutes in s, allowing "#:#" and am/pm,
 *	"noon", and "midnight". Skips leading white space, doesn't errorcheck
 *	at all.
 */
int
strtotime(s)
char *s;
{
    int h,m;
    char *t;

    while (isspace(*s))
	s += 1;
    if (strncasecmp(s,appResources.noonStr,
				strlen(appResources.noonStr)) == 0)
	return(12*60);
    else if (strncasecmp(s,appResources.midnightStr,
				strlen(appResources.midnightStr)) == 0)
	return(0);
    t = s;
    h = (int)strtol(s,&s,10);
    if (*s == ':') {
	s += 1;
	m = (int)strtol(s,&s,10);
    } else
	m = 0;
    if (s == t)		/* no numbers, some trash */
	return(-1);
    if (*s == 'p')	/* otherwise it's 'a' or '\0' so okay */
	h += 12;
    return(h*60+m);
}

/*
 * timetostr(t) : Formats t minutes into hh:mm, possibly with am/pm, and
 *	possibly with noonStr or midnightStr.
 *	Returns a pointer to the static string.
 */
char *
timetostr(t)
int t;
{
    static char *s = "XX:XXxxyy";
    int h,m;

    h = t / 60;
    m = t % 60;
    if (appResources.appsUseAmPm)
	if (h == 12)
	    if (m == 0)
		sprintf(s,appResources.noonStr);
	    else
		sprintf(s,"12:%02dpm",m);
	else if (h == 0)
	    if (m == 0)
		sprintf(s,appResources.midnightStr);
	    else
		sprintf(s,"00:%02dam",m);
	else if (h > 12)
	    sprintf(s,"%2d:%02dpm",h-12,m);
	else
	    sprintf(s,"%2d:%02dam",h,m);
    else
	sprintf(s,"%2d:%02d",h,m);
    return(s);
}
