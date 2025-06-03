#if !defined(lint) && !defined(SABER)
static char XRNrcsid[] = "$Header: /net/objy27/wrld/mnt11/ricks/src/master/xrn/utils.c,v 1.8 1993/02/04 18:22:34 ricks Exp $";
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
 * utils.c: random utility functions for xrn
 */

#include "copyright.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "config.h"
#include "utils.h"
#include "news.h"
#ifndef VMS
#include <sys/param.h>
#include <pwd.h>
#else
#define MAXPATHLEN 512
#define index strchr
#endif /* VMS */

#ifdef sco
#define MAXPATHLEN 512
#endif

#ifdef aiws
struct passwd *getpwuid();
struct passwd *getpwnam();
#endif /* aiws */

#if defined(sun)
#include <string.h>
#endif

#if defined(apollo)
#include <string.h>
#include <sys/time.h>
#endif

#include <time.h>
#ifdef __STDC__
#ifndef VMS
#include <unistd.h>
#endif
#endif

#define USER_NAME_SIZE 32

#ifdef SYSV_REGEX
/* 
 * kludge alert:  this is here because (on A/UX 1.1) linking with
 * the PW lib (necessary to get the regular expression routines, 
 * regcmp/regex), causes symbol 'Error' to be undefined.
 * 	glenn@mathcs.emory.edu 10/17/89 
 *
 * SYSV_REGEX may be overkill, need for macII and HPUX
 *    (Andy.Linton@comp.vuw.ac.nz 11/8/89)
 */
int Error; 
#endif

/*
 * trim leading and trailing spaces from a string (and newlines)
 *
 *   NOTE: this function modifies the argument
 *
 *   returns: the modified string
 *
 */
char *
utTrimSpaces(str)
char *str;
{
    char *end = &str[utStrlen(str) - 1];

    while ((*str == ' ') || (*str == '\n') || (*str == '\t')) {
	str++;
    }
    while ((*end == ' ') || (*end == '\n') || (*end == '\t')) {
	*end = '\0';
	end--;
	if (end == str) return (str);
    }
    return(str);
}


/*
 * tilde expand a file name
 *
 *   returns: the expanded name of the file (in a static buffer)
 *            or NIL(char) 
 */
char *
utTildeExpand(filename)
char *filename;    /* file name, possibly with a '~'             */
{
#ifdef aiws
    static char dummy[MAXPATH];
#else
    static char dummy[MAXPATHLEN];
#endif /* aiws */
    char username[USER_NAME_SIZE], *loc;
    struct passwd *pw;
    
    if ((filename == NIL(char)) || STREQ(filename, "")) {
	return(NIL(char));
    }

    if (filename[0] != '~') {
	(void) strcpy(dummy, filename);
	return(dummy);
    }

    /* tilde at the beginning now */
    if (filename[1] == '/' || filename[1] == '\0') {
	/* current user */
	char *home, *getenv _ARGUMENTS((const char *));
	
	if ((home = getenv("HOME")) == NIL(char)) {
#ifndef VMS
	    /* fall back on /etc/passwd */
	    if ((pw = getpwuid(getuid())) == NIL(struct passwd)) {
		return(NIL(char));
	    }
	    (void) sprintf(dummy, "%s%s", pw->pw_dir, &filename[1]);
#else
	    return (NIL(char));
#endif
	} else {
	    (void) sprintf(dummy, "%s%s", home, &filename[1]);
	}
	    
    } else {
	if ((loc = index(filename, '/')) == NIL(char)) {
	    /* error - bad filename */
	    return(NIL(char));
	}
	(void) strncpy(username, &filename[1], loc - &filename[1]);
	username[loc - &filename[1]] = '\0';
#ifndef VMS
	if ((pw = getpwnam(username)) == NIL(struct passwd)) {
	    return(NIL(char));
	}
	(void) sprintf(dummy, "%s%s", pw->pw_dir, loc);
#else
	return(getenv("USER"));
#endif
    }
    return(dummy);
}

char *
utNameExpand(filename)
char *filename;    /* file name, possibly with a '~'             */
/*
 * Use DOTDIR to expand a startup file name only if the filename is a
 * relative pathname. Otherwise call utTildeExpand()
 *
 *   returns: the expanded name of the file (in a static buffer)
 *            or NIL(char) 
 */
{
#ifdef aiws
    static char dummy2[MAXPATH];
#else
    static char dummy2[MAXPATHLEN];
#endif /* aiws */
    char *dotdir, *getenv _ARGUMENTS((const char *));
    
    if ((filename == NIL(char)) || STREQ(filename, "")) {
	return(NIL(char));
    }

    if (filename[0] != '~' && filename[0] != '/')    {
	if ((dotdir = getenv("DOTDIR")) != NIL(char)) {
	    (void) sprintf(dummy2, "%s/%s", dotdir, filename);
	    return(dummy2);
	}
	return (utTildeExpand(filename));
    } else {
	return(utTildeExpand(filename));
    }
}


int
utSubstring(string, sub)
char *string;
char *sub;
{
    int i;
    int srcLen = utStrlen(string);
    int subLen = utStrlen(sub);

    if (srcLen < subLen)
	return(0);
    
    for (i = 0; i < srcLen - subLen + 1; i++) {
	if (STREQN(&string[i], sub, utStrlen(sub))) {
	    return(1);
	}
    }
    return(0);
}

/*
 * down case the characters in a string (in place)
 */
void
utDowncase(string)
char *string;
{
    for ( ; *string != '\0'; string++) {
	if (isupper(*string)) {
	    *string = tolower(*string);
	}
    }
    return;
}

/*
 * like strncpy but lowercases too
 */
void
utLowerCase(dst, src, size)
char *dst;
char *src;
{
    for ( ; size > 0 && *src != '\0'; src++, dst++, size--) {
	if (isupper(*src)) {
	    *dst = tolower(*src);
	} else {
	    *dst = *src;
	}
    }
    *dst = '\0';
    return;
}

#ifdef VMS    
int
utGroupToVmsFilename(filename,group)
    char *filename;
    char *group;
{
    char *ptr;
    int i=0;

    for (ptr = group; *ptr != 0; ptr++)
    {
	if (isalnum(*ptr))
	{
	    filename[i++] = *ptr;
	}
	else
	{
	    switch (*ptr)
	    {
		case '.' :
		    filename[i++] = '_';
		    break;
		case '-' :
		    filename[i++] = '-';
		    break;
		default :
		    i = i + sprintf(&filename[i], "$X%X$", *ptr);
		    break;
	    }
	}
    }
    filename[i] = 0;
    return (i);
}
#endif


#ifdef VMS    
extern int delete();
int unlink(foo)
char *foo;
{
    while (!delete(foo));
    return 0;
}
#endif /* VMS */


/* case insensitive comparision for subjects */

int
utSubjectCompare(str1, str2)
#if defined(__STDC__) && __STDC__
const char *str1;
const char *str2;
#else
char *str1, *str2;
#endif
{
    char c1, c2;

    while (True) {
	if (!*str1 && !*str2) {
	    return 0;
	}
	if (!*str1) {
	    return -1;
	}
	if (!*str2) {
	    return 1;
	}
	if (isupper(*str1)) {
	    c1 = tolower(*str1);
	} else {
	    c1 = *str1;
	}
	if (isupper(*str2)) {
	    c2 = tolower(*str2);
	} else {
	    c2 = *str2;
	}
	if (c1 != c2) {
	    return (c1 - c2);
	}
	str1++;
	str2++;
    }
}

void
utCopyFile(old, save)
char *old;   /* name of file to save */
char *save;  /* name of file to save to */
{
    FILE *orig, *new;
    char buf[BUFSIZ];
    char *newFile;
    int num_read;

    if ((orig = fopen(old, "r")) == NULL) {
	return;
    }

    if ((newFile = utNameExpand(save)) == NIL(char)) {
	return;
    }

    /* if .old is a link to new we could have trouble, so unlink it */
    (void) unlink(newFile);
    
    if ((new = fopen(newFile, "w")) == NULL) {
	fclose(orig);
	return;
    }

    while (num_read = fread(buf, sizeof(char), BUFSIZ, orig)) {
	(void) fwrite(buf, sizeof(char), num_read, new);
    }

    (void) fclose(orig);
    (void) fclose(new);

    return;
}

#ifdef NEED_STRNCASECMP

strncasecmp(str1,str2,len)
char  *str1, *str2;
int   len;
{
      char    *tmp1,*tmp2;
      int     i,result;
      if ((tmp1 = (char *)malloc(len+1)) == NULL) {
              perror("strncasecmp");
              exit(1);
      }
      if ((tmp2 = (char *)malloc(len+1)) == NULL) {
              free(tmp1);
              perror("strncasecmp");
              exit(1);
      }
      for (i = 0; i < len && str1[i] != '\0'; i++)
              if (isupper(str1[i]))
                      tmp1[i] = tolower(str1[i]);
              else
                      tmp1[i] = str1[i];
      tmp1[i] = '\0';
      for (i = 0; i < len && str2[i] != '\0'; i++)
              if (isupper(str2[i]))
                      tmp2[i] = tolower(str2[i]);
              else
                      tmp2[i] = str2[i];
      tmp2[i] = '\0';
      result = strncmp(tmp1,tmp2,len);
      free(tmp1);
      free(tmp2);
      return result;
}
#endif /* NEED_STRNCASECMP */


#ifdef REALLY_USE_LOCALTIME
static int days[] = { 31,28,31,30,31,30,31,31,30,31,30,31};

/**********************************************************************
This function performs the function of mktime as declared in time.h,
but which has no definition.  It's the inverse of gmtime.
**********************************************************************/

time_t
makeTime(tmp)
struct tm *tmp;
{
  time_t ret;
  int i;

  if (tmp->tm_year < 70) return 0;
  ret = ((tmp->tm_year-70) / 4)*(366+365+365+365);
  switch ((tmp->tm_year-70) % 4) {
  case 1:
    ret += 365;
    break;
  case 2:
    ret += 365+365;
    if (tmp->tm_mon > 1) {
      ret += 1;
    }
    break;
  case 3:
    ret += 365+366+365;
  }
  for (i=0; i<tmp->tm_mon; i++) {
    ret += days[i];
  }
  ret += tmp->tm_mday-1;
  ret = ret*24+tmp->tm_hour;
  if (tmp->tm_isdst) {
    ret -= 1;
  }
  ret = ret*60+tmp->tm_min;
  ret = ret*60+tmp->tm_sec;
  return ret;
}

/**********************************************************************
This (ugly) function takes a source of the form "31 Aug 90 16:47:06 GMT"
and writes into dest the equivalent in local time.  If an invalid
source is given, the dest is a copy of the source.

Optionally, there may be a "XXX, " prepending the source where XXX is
a weekday name.
**********************************************************************/

char *getzonename();

tconvert(dest, source)
char *dest, *source;
{
  char *p, *fmt;
  int h, m, s, day, mon, year;
  struct tm *tmp, t;
  time_t then;
  int doWeekDay;
#if defined(apollo)
  int daylight;
#endif

  strcpy(dest, source);

  /* Parse date */
  p = source;
  if (!strncasecmp(p, "mon, ", 5) ||
      !strncasecmp(p, "tue, ", 5) ||
      !strncasecmp(p, "wed, ", 5) ||
      !strncasecmp(p, "thu, ", 5) ||
      !strncasecmp(p, "fri, ", 5) ||
      !strncasecmp(p, "sat, ", 5) ||
      !strncasecmp(p, "sun, ", 5)) {
    p += 5;
    doWeekDay = 1;
  } else {
/*    doWeekDay = 0; */
    doWeekDay = 1;		/* Let's put the weekday in all postings */
  }
  while (*p == ' ') {
    p++;
  }
  if (!sscanf(p, "%d", &day)) {
    return;
  }
  while (*p != ' ' && *p != '\0') {
    p++;
  }
  if (*p == '\0') {
    return;
  }
  while (*p == ' ' && *p != '\0') {
    p++;
  }
  if (*p == '\0') {
    return;
  }
  if (!strncasecmp(p, "jan", 3)) {
    mon = 0;
  } else if (!strncasecmp(p, "feb", 3)) {
    mon = 1;
  } else if (!strncasecmp(p, "mar", 3)) {
    mon = 2;
  } else if (!strncasecmp(p, "apr", 3)) {
    mon = 3;
  } else if (!strncasecmp(p, "may", 3)) {
    mon = 4;
  } else if (!strncasecmp(p, "jun", 3)) {
    mon = 5;
  } else if (!strncasecmp(p, "jul", 3)) {
    mon = 6;
  } else if (!strncasecmp(p, "aug", 3)) {
    mon = 7;
  } else if (!strncasecmp(p, "sep", 3)) {
    mon = 8;
  } else if (!strncasecmp(p, "oct", 3)) {
    mon = 9;
  } else if (!strncasecmp(p, "nov", 3)) {
    mon = 10;
  } else if (!strncasecmp(p, "dec", 3)) {
    mon = 11;
  } else {
    return;
  }
  while (*p != ' ' && *p != '\0') {
    p++;
  }
  if (*p == '\0') {
    return;
  }
  if (!sscanf(p, "%d", &year)) {
    return;
  }
  year = year % 100;

  /* Parse time */
  p = strrchr(source, ':');
  if (!p) {
    return;
  }
  p--;
  while (p > source && *p != ':') {
    p--;
  }
  while (p > source && *p != ' ') {
    p--;
  }
  if (!p) {
    return;
  }
  sscanf(p, "%d", &h);
  p = strchr(p, ':');
  if (!p++) {
    return;
  }
  sscanf(p, "%d", &m);
  p = strchr(p, ':');
  if (!p++) {
    return;
  }
  sscanf(p, "%d", &s);
  p = strchr(p, ' ');
  if (!p++) {
    return;
  }

  /* Confirm GMT */
  if (strcmp(p, "GMT")) {
    return;
  }

  t.tm_sec = s;
  t.tm_min = m;
  t.tm_hour = h;
  t.tm_mday = day;
  t.tm_mon = mon;
  t.tm_year = year;
  t.tm_isdst = 0;
  then = makeTime(&t);
  tmp = localtime(&then);
  
/* ascftime is non-standard, sigh.
  ascftime(dest, "%e %b %y %H:%M:%S %Z", tmp);
*/
  fmt = asctime(tmp);
  /* Make this look like the original format */
  p = dest;
  if (doWeekDay) {
    switch (tmp->tm_wday) {
    case 0:
      strcpy(dest, "Sun, ");
      break;
    case 1:
      strcpy(dest, "Mon, ");
      break;
    case 2:
      strcpy(dest, "Tue, ");
      break;
    case 3:
      strcpy(dest, "Wed, ");
      break;
    case 4:
      strcpy(dest, "Thu, ");
      break;
    case 5:
      strcpy(dest, "Fri, ");
      break;
    case 6:
      strcpy(dest, "Sat, ");
      break;
    }
    p += 5;
  }
  if (*(fmt+8) == ' ') {
    strncpy(p, fmt+9, 2);
    p += 2;
  } else {
    strncpy(p, fmt+8, 3);
    p += 3;
  }
  strncpy(p, fmt+4, 4);
  p += 4;
  if ((tmp->tm_year % 100) < 10) {
    sprintf(p, "0%d", tmp->tm_year % 100);
  } else {
    sprintf(p, "%d", tmp->tm_year % 100);
  }
  strcat(dest, fmt+10);
  p = strrchr(dest, ' ');
#if defined(sun) && !defined(SYSV)				    /* ggh */
  strcpy(p+1, tmp->tm_zone);
#else
#if defined(ultrix)
  (void) strcpy(p+1, getzonename(tmp->tm_isdst));
#else
#if defined(apollo)
  daylight = tmp->tm_isdst;
#endif
  if (daylight) {
    strcpy(p+1, tzname[1]);
  } else {
    strcpy(p+1, tzname[0]);
  }
#endif
#endif

  if (*dest == ' ') {
    p = dest;
    while (*p != '\0') {
      *p = *(p+1);
      p++;
    }
  }
}

#if defined(apollo) || defined(ultrix) || defined(SYSV)
#if !defined(_POSIX_SOURCE) && !defined(__STDC__)

extern char *timezone();

#if defined(ultrix) && defined(mips)
char * getzonename(isdst)
int isdst;
{
    static char *name[2];
    time_t then = 0;
    struct tm *tm;

    if (isdst)
	isdst = 1;

    if (name[isdst] != NULL)
	return name[isdst];

    tm = localtime(&then);
    if (tm->tm_isdst == isdst) {
	name[isdst] = XtNewString(tm->tm_zone);
    } else {
	name[isdst] = XtNewString(timezone(tm->tm_gmtoff/60, isdst));
    }
    return name[isdst];
}
#else /* ultrix && mips */
char *
getzonename(isdst)
int isdst;
{
    static char *name[2];
    struct timezone tz;

    if (isdst)
	isdst = 1;

    if (name[isdst] != NULL)
	return name[isdst];

    gettimeofday(NULL, &tz);
    name[isdst] = XtNewString(timezone(tz.tz_minuteswest, isdst));
    return name[isdst];
}
#endif /* ultrix  && mips */
#else /* POSIX */
#include <sys/time.h>
char *
getzonename(isdst)
int isdst;
{
    if (isdst)
	isdst = 1;

    return tzname[isdst];
}
#endif /* POSIX or STDC */
#endif /* apollo or ultrix or SYSV */
#endif /* REALLY_USE_LOCALTIME */

char *
string_pool(key)
char *key;
{
    char *tmp;

    if (StringPool == NULL)
	StringPool = avl_init_table(strcmp);

    if (!avl_lookup(StringPool, key, &tmp)) {
	avl_insert_str(StringPool, key);
	avl_lookup(StringPool, key, &tmp);
    }
    return tmp;
}

#ifdef XLATE

#define UC(x)	(unsigned char)(x)
/* translate a character string in place */

static Boolean	inited = 0;
static char	xlate[ 256 ];
static char	unxlate[ 256 ];

static void
XlateInit()
{
    char	*to = XLATE_TO, *from = XLATE_FROM;
    int		i;

    for ( i = 256; --i >= 0; )
	xlate[ i ] = unxlate[ i ] = i;

    while (*to && *from) {
	unxlate[ UC(*to) ] = *from;
	xlate[ UC(*from++) ] = *to++;
    }
    inited = 1;
}

void
utXlate(s)
char *s;
{
    if (!inited) XlateInit();

    do {
	*s = xlate[ UC(*s) ];
    } while(*++s);
}

void
utUnXlate(s)
char *s;
{
    if (!inited) XlateInit();

    do {
	*s = unxlate[ UC(*s) ];
    } while(*++s);
}
#endif /* XLATE */
