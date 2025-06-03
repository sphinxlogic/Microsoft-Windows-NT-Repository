/*
 * pcalutil.c - utility routines for Pcal
 *
 * Contents:
 *
 *		alloc
 *		calc_day
 *		calc_weekday
 *		calc_year_day
 *		ci_strcmp
 *		ci_strncmp
 *		copy_text
 *		cvt_escape
 *		find_executable
 *		getline
 *		is_valid
 *		loadwords
 *		mk_filespec
 *		mk_path
 *		normalize
 *		note_box
 *		note_day
 *		split_date
 *		trnlog
 *
 * Revision history:
 *
 *	4.3	AWR	11/22/91	added special case to loadwords():
 *					split -<flag>"<text>" into two words
 *
 *					removed octal/hex escape functionality
 *					from getline() to new routine
 *					cvt_escape() (for use elsewhere)
 *
 *			10/25/91	added parameters to loadwords() and
 *					getline() to avoid always using same
 *					global data
 *
 *			10/15/91	revised UN*X mk_filespec() to translate
 *					"~/" in path name as well as file name
 *
 *	4.2	AWR	10/08/91	support -[kK] flags (cf. note_box())
 *
 *			10/03/91	added note_box(), note_day()
 *
 *	4.11	AWR	08/20/91	documented find_executable()
 *
 *	4.02	AWR	06/07/91	added find_executable()
 *
 *	4.0	AWR	02/24/91	Revised getline() and copy_text() to
 *					handle C-style escapes of characters
 *					and octal/hex numbers
 *
 *		AWR	02/19/91	Added support for negative ordinals
 *					in calc_day(), calc_year_day()
 *
 *		AWR	02/04/91	Added calc_year_day()
 *
 *		AWR	01/15/91	Extracted from pcal.c
 *
 */


/*
 * Standard headers:
 */

#include <stdio.h>
#include <ctype.h>
#include <string.h>

/*
 * Pcal-specific definitions:
 */

#include "pcaldefs.h"
#include "pcalglob.h"
#include "pcallang.h"

/*
 * Macros:
 */

/* skip over numeric field and subsequent non-numeric characters */
#define SKIP_FIELD(p) \
	do { while (*p && isdigit(*p)) p++; \
	     while (*p && !isdigit(*p)) p++; } while (0)

/* guarantee that a path is terminated by the END_PATH character */
#define TERM_PATH(path) \
	do { char *p;	\
		if ((p = P_LASTCHAR(path)) && *p != END_PATH) \
			*++p = END_PATH, *++p = '\0'; } while (0)


/*
 * General-purpose utility routines
 */


/*
 * alloc - interface to calloc(); terminates if unsuccessful
 */
#ifdef PROTOS
char *alloc(int size)
#else
char *alloc(size)
	int size;
#endif
{
	char *p;

	if (size == 0)		/* not all calloc()s like null requests */
		size = 1;

	if ((p = calloc(1, size)) == NULL) {
		FPR(stderr, E_ALLOC_ERR, progname);
		exit(EXIT_FAILURE);
	}

	return p;
}


/*
 * ci_str{n}cmp - case-insensitive flavors of strcmp(), strncmp()
 */
#ifdef PROTOS
int ci_strcmp(register char *s1,
	      register char *s2)
#else
int ci_strcmp(s1, s2)
	register char *s1, *s2;
#endif
{
	register char c1, c2;

	for ( ; (c1 = TOLOWER(*s1)) == (c2 = TOLOWER(*s2)); s1++, s2++)
		if (c1 == '\0')
			return 0;

	return c1 - c2;
}


#ifdef PROTOS
int ci_strncmp(register char *s1,
	       register char *s2,
	       int n)
#else
int ci_strncmp(s1, s2, n)
	register char *s1, *s2;
	int n;
#endif
{
	register char c1, c2;

	for ( ; --n >= 0 && (c1 = TOLOWER(*s1)) == (c2 = TOLOWER(*s2)); s1++, s2++)
		if (c1 == '\0')
			return 0;

	return n < 0 ? 0 : c1 - c2;
}


/*
 * Date calculation routines (see also macros in pcaldefs.h)
 */


/*
 * normalize - adjust day in case it has crossed month (or year) bounds 
 */
#ifdef PROTOS
void normalize(DATE *pd)
#else
void normalize(pd)
	DATE *pd;		/* pointer to date */
#endif
{
	int len;

	/* adjust if day is in previous or following month */

	while (pd->dd < 1) {
		pd->yy = PREV_YEAR(pd->mm, pd->yy);
		pd->mm = PREV_MONTH(pd->mm, pd->yy);
		pd->dd += LENGTH_OF(pd->mm, pd->yy);
	}

	while (pd->dd > (len = LENGTH_OF(pd->mm, pd->yy))) {
		pd->dd -= len;
		pd->yy = NEXT_YEAR(pd->mm, pd->yy);
		pd->mm = NEXT_MONTH(pd->mm, pd->yy);
	}
}


/*
 * calc_day - calculate calendar date from ordinal date (e.g., "first Friday
 * in November", "last day in October"); return calendar date if it exists, 
 * 0 if it does not
 */
#ifdef PROTOS
int calc_day(int ord,
	     int wkd,
	     int mm)
#else
int calc_day(ord, wkd, mm)
	int ord;
	int wkd;
	int mm;
#endif
{
#ifdef PROTOS
	int first, last, day, (*pfcn)(int, int, int);
#else
	int first, last, day, (*pfcn)();
#endif

	if (IS_WILD(wkd)) {	/* "day", "weekday", "workday", or "holiday" */
		pfcn = pdatefcn[wkd - WILD_FIRST];
		last = LENGTH_OF(mm, curr_year);

		if (ord < 0) {			/* search backwards */
			for (day = last; 
			     day >= 1 &&
				!((*pfcn)(mm, day, curr_year) && ++ord == 0);
			     day--)
				;
		} else {			/* search forwards */
			for (day = 1; 
			     day <= last && 
				!((*pfcn)(mm, day, curr_year) && --ord == 0);
			     day++)	
				;
		}
		return is_valid(mm, day, curr_year) ? day : 0; 

	} else {		/* fixed weekday - calculate it */
		first = (wkd - FIRST_OF(mm, curr_year) + 7) % 7 + 1;
		if (ord < 0) {		/* get last (try 5th, then 4th) */
			if (!is_valid(mm, last = first + 28, curr_year))
				last -= 7;
			if (!is_valid(mm, day = last + 7 * (ord + 1),
			    curr_year))
				day = 0;	
		}
		else 
			if (!is_valid(mm, day = first + 7 * (ord - 1),
			    curr_year))
				day = 0;

		return day;
	}

}


/*
 * calc_year_day - calculate calendar date from ordinal date within year
 * (e.g., "last Friday in year", "10th holiday in year"); if date exists,
 * fill in pdate and return TRUE; else return FALSE
 */
#ifdef PROTOS
int calc_year_day(int ord,
		  int wkd,
		  DATE *pdate)
#else
int calc_year_day(ord, wkd, pdate)
	int ord;
	int wkd;
	DATE *pdate;
#endif
{
#ifdef PROTOS
	int incr, (*pfcn)(int, int, int);
#else
	int incr, (*pfcn)();
#endif
	DATE date;

	if (IS_WILD(wkd)) {	/* "day", "weekday", "workday", or "holiday" */
		pfcn = pdatefcn[wkd - WILD_FIRST];

		if (ord < 0) {			/* nth occurrence backwards */
			MAKE_DATE(date, DEC, 31, curr_year);
			ord = -ord;
			incr = -1;
		} else {			/* nth occurrence forwards */
			MAKE_DATE(date, JAN, 1, curr_year);
			incr = 1;
		}

		/* search for selected occurrence of specified wildcard */

		while (date.yy == curr_year &&
		       !((*pfcn)(date.mm, date.dd, date.yy) && --ord == 0)) {
			date.dd += incr;
			normalize(&date);
		}

	} else {		/* fixed weekday - calculate it */
		if (ord < 0)
			MAKE_DATE(date, DEC,
				  calc_day(-1, wkd, DEC) + 7 * (ord + 1),
				  curr_year);
		else
			MAKE_DATE(date, JAN,
				  calc_day(1, wkd, JAN) + 7 * (ord - 1),
				  curr_year);
		normalize(&date);
	}

	return date.yy == curr_year ? (*pdate = date, TRUE) : FALSE;
}


/*
 * calc_weekday - return the weekday (0-6) of mm/dd/yy (mm: 1-12)
 */
#ifdef PROTOS
int calc_weekday(int mm,
		 int dd,
		 int yy)
#else
int calc_weekday(mm, dd, yy)
	int mm;
	int dd;
	int yy;
#endif
{
	return (yy + (yy-1)/4 - (yy-1)/100 + (yy-1)/400 + OFFSET_OF(mm, yy) +
		(dd-1)) % 7;
}


/*
 * note_day - translate n (from "note/<n>" spec) to appropriate note text
 * day for mm/yy; return note text day if in range, 0 if not
 */
#ifdef PROTOS
int note_day(int mm,
	     int n,
	     int yy)
#else
int note_day(mm, n, yy)
	int mm;
	int n;
	int yy;
#endif
{
	int day, lastday;

	if (n == 0)			/* convert 0 to appropriate default */
		n = NOTE_DEFAULT;

	/* lastday depends on month length and presence (but not position) of
	 * small calendars
	 */
	lastday = LAST_NOTE_DAY - (LENGTH_OF(mm, yy) - 28) -
			(small_cal_pos == SC_NONE ? 0 : 2);

	/* count forward if n is positive, backward if negative */
	day = (n > 0) ? FIRST_NOTE_DAY + n - 1 : lastday + n + 1;

	/* make sure result is valid for this month/year */
	return (day >= FIRST_NOTE_DAY && day <= lastday) ? day : 0;
}


/*
 * note_box - translate dd from note text day to box number for mm/yy,
 * adjusting for presence and position of small calendars
 */
#ifdef PROTOS
int note_box(int mm,
	     int dd,
	     int yy)
#else
int note_box(mm, dd, yy)
	int mm;
	int dd;
	int yy;
#endif
{
	int startbox = START_BOX(mm, yy), pc, nc;

	/* move starting box to second row if conflict with small calendars */
	if ((pc = prev_cal_box[small_cal_pos]) == startbox ||
	    (nc = next_cal_box[small_cal_pos]) == startbox)
		startbox += 7;

	dd -= FIRST_NOTE_DAY;		/* convert to note box number 0..13 */
	dd += (pc == 0) + (nc == 1);	/* adjust for small calendars in 0, 1 */

	/* position box after calendar body if no room before */
	return dd < startbox ? dd : dd + LENGTH_OF(mm, yy);
}


/*
 * is_valid - return TRUE if m/d/y is a valid date
 */
#ifdef PROTOS
int is_valid(register int m,
	     register int d,
	     register int y)
#else
int is_valid(m, d, y)
	register int m, d, y;
#endif
{
	return m >= JAN && m <= DEC && 
		d >= 1 && d <= LENGTH_OF(m, y);
}



/*
 * Token parsing/remerging routines:
 */


/*
 * loadwords - tokenize buffer 'buf' into array 'words' and return word count;
 * differs from old loadwords() in that it handles quoted (" or ') strings
 * and removes escaped quotes
 */
#ifdef PROTOS
int loadwords(char **words,
	      char *buf)
#else
int loadwords(words, buf)
	char **words;
	char *buf;
#endif
{
	register char *ptok;
	char *delim, **ap, *p1, *p2, c;
	int nwords;

	for (ptok = buf, ap = words; TRUE; ap++) {

		ptok += strspn(ptok, WHITESPACE); /* find next token */

		if (! *ptok) {			/* end of buf? */
			*ap = NULL;		/* add null ptr at end */
			nwords = ap - words;	/* number of non-null ptrs */
			break;			/* exit loop */
			}

		delim =	*ptok == '"'  ? "\"" :	/* set closing delimiter */
			*ptok == '\'' ? "'"  :
			WHITESPACE;

		/* split flag followed by quoted string into two words */
		if (*ptok == '-' && isalpha(ptok[1]) &&
		    ((c = ptok[2]) == '"' || c == '\'')) {
			delim = c == '"' ? "\"" : "'";
			*ap++ = ptok;
			ptok[2] = '\0';		/* terminate first token */
			ptok += 3;		/* start second token */
		}
		else if (*ptok == *delim)
			ptok++;			/* skip opening quote */

		*ap = ptok;			/* save token ptr */

		/* find first unescaped string delimiter - handle special
		 * case where preceding backslash is itself escaped
		 */
		do {
			ptok += strcspn(ptok, delim);
			if ((c = ptok[-1]) == '\\')
				if (ptok[-2] == '\\')
					break;	/* stop on \\" or \\' */
				else
					ptok++;
		} while (c == '\\');

		if (*ptok)			/* terminate token */
			*ptok++ = '\0';
	}

	/* now reprocess the word list, removing remaining escapes */
	for (ap = words; *ap; ap++)
		for (p1 = p2 = *ap; c = *p2 = *p1++; *p2++)
			if (c == '\\')
				*p2 = *p1++;

	return nwords;				/* return word count */

}


/*
 * copy_text - retrieve remaining text in pbuf and copy to output string,
 * separating tokens by a single blank and condensing runs of blanks (all
 * other whitespace has been converted to blanks by now) to one blank
 */
#ifdef PROTOS
void copy_text(char *pbuf,
	       char **ptext)
#else
void copy_text(pbuf, ptext)
	char *pbuf;		/* output buffer - can be pbuf itself */
	char **ptext;		/* pointer to first text word in "words" */
#endif
{
	char *p, *pb;

	/* copy words to pbuf, separating by one blank */

	for (*(pb = pbuf) = '\0'; p = *ptext; *pb++ = *++ptext ? ' ' : '\0') {
		for ( ; *p; *p++)
			if (! (*p == ' ' && (pb == pbuf || pb[-1] == ' ')))
				*pb++ = *p;
		if (pb > pbuf && pb[-1] == ' ')
			pb--;
	}
}


/*
 * split_date - extract 1-3 numeric fields (separated by one or more
 * non-numeric characters) from date string; return number of fields
 */
#ifdef PROTOS
int split_date(char *pstr,
	       int *pn1,
	       int *pn2,
	       int *pn3)
#else
int split_date(pstr, pn1, pn2, pn3)
	char *pstr;			/* input string */
	int *pn1, *pn2, *pn3;		/* output numbers */
#endif
{
	int i, n, *pn;

	/* attempt to extract up to three numeric fields */
	for (n = 0, i = 1; i <= 3; i++) {
		pn = i == 1 ? pn1 : i == 2 ? pn2 : pn3;	/* crude but portable */
		if (pn)
			*pn = *pstr ? (n++, atoi(pstr)) : 0;
		SKIP_FIELD(pstr);		/* go to next field */
	}

	return n;
}



/*
 * Escape sequence conversion routines:
 */


/*
 * octal_esc - read up to 3 octal digits from character string; fill in
 * value of octal constant and return pointer to last character
 */
#ifdef PROTOS
static char *octal_esc(char *buf,
		       char *val)
#else
static char *octal_esc(buf, val)
	char *buf;
	char *val;
#endif
{
	int i, n, c;

	for (n = 0, i = 0; i < 3 && (c = buf[i]) && isodigit(c); i++) {
		n = n * 8 + (c - '0');
	}

	*val = n & 0377;	/* fill in the value */
	return buf + i - 1;	/* return pointer to last character */
}


/*
 * hex_esc - read 'x' or 'X' followed by 1 or 2 hex digits from character
 * string; fill in value of hexadecimal constant (or letter if no hex digits
 * follow) and return pointer to last character
 */
#ifdef PROTOS
static char *hex_esc(char *buf,
		     char *val)
#else
static char *hex_esc(buf, val)
	char *buf;
	char *val;
#endif
{
	int i, n, c;

	/* assume leading character is known to be 'x' or 'X'; skip it */
	buf++;

	for (n = 0, i = 0; i < 2 && (c = buf[i]) && isxdigit(c); i++) {
		n = n * 16 + (isupper(c) ? c - 'A' + 10 :
			      islower(c) ? c - 'a' + 10 :
			      c - '0');
	}

	*val = i == 0 ? buf[-1] : n & 0377; /* fill in the value */
	return buf + i - 1;		/* return pointer to last character */
}


/*
 * cvt_escape - copy string ibuf to string obuf, converting octal/hex/whitespace
 * escape sequences to the appropriate equivalents.  Escaped quotes and
 * backslashes are not converted here; they need to be preserved so that
 * loadwords() can parse quoted strings correctly
 */
#ifdef PROTOS
void cvt_escape(char *obuf,
		char *ibuf)
#else
void cvt_escape(obuf, ibuf)
	char *obuf, *ibuf;
#endif
{
	char c, c2, *po, *pi;
	static char whitespace[] = "abfnrtv";	/* cf. ANSI spec, 2.2.2 */
	static char no_cvt[] = "\"'\\";

	for (po = obuf, pi = ibuf; c = *pi; *po++ = c, pi++) {
		/* handle escape sequences here: escaped whitespace
		 * and ANSI escapes are all converted to a space;
		 * octal and hex constants are converted in place;
		 * escaped single/double quotes are left escaped for
		 * loadwords() to handle later on
		 */
		if (c == '\\') {
			c2 = *++pi;
			if (isspace(c2) || strchr(whitespace, c2)) {
				c = ' ';
			}
			else if (isodigit(c2)) {	/* octal */	
				pi = octal_esc(pi, &c);
			}
			else if (TOLOWER(c2) == 'x') {	/* hex */
				pi = hex_esc(pi, &c);
			}
			else {
				if (strchr(no_cvt, c2))	/* leave escaped */
					*po++ = c;
				c = c2;
			}
		}
	}

	*po = '\0';
}



/*
 * File input routines:
 */


/*
 * getline - read next non-null line of input file into buf; return 0 on EOF.
 * Strip leading whitespace and trailing comments; handle escaped newlines
 * and call cvt_escape() to translate other escape sequences
 */
#ifdef PROTOS
int getline(FILE *fp,
	    char *buf,
	    int *pline)
#else
int getline(fp, buf, pline)
	FILE *fp;
	char *buf;
	int *pline;
#endif
{
	register char *cp;
	register int c, c2;
	int in_comment;		/* comments: from '#' to end-of-line */
	char tmpbuf[LINSIZ];	/* temporary buffer to accumulate line */

	cp = tmpbuf;
	*buf = '\0';		/* in case of premature EOF */

	do {
		in_comment = FALSE;
		while ((c = getc(fp)) != '\n' && c != EOF) {
			if (c == '#')
				in_comment = TRUE;

			if (isspace(c))		/* whitespace => blank */
				c = ' ';

			/* ignore comments and leading white space */
			if (in_comment || (cp == tmpbuf && c == ' '))
				continue;

			/* only handle escape newlines here; other escapes are
			 * now handled by cvt_escape() or loadwords() (q.v.)
			 */
			if (c == '\\') {
				if ((c2 = getc(fp)) == EOF)
					return FALSE;

				if (c2 == '\n') {
					c = ' ';
					(*pline)++;
				}
				else
					ungetc(c2, fp);
			}
			*cp++ = c;
		}

		if (c == EOF)			/* no more input lines */
			return FALSE;

		(*pline)++;			/* bump line number */

	} while (cp == tmpbuf);			/* ignore empty lines */

	*cp = '\0';
	cvt_escape(buf, tmpbuf);		/* convert escape sequences */
	return TRUE;
}


/*
 * Routines dealing with translation of file specifications (VMS, Un*x)
 */

#ifdef VMS
/*
 * mk_path - extract the path component from VMS file spec
 */
#ifdef PROTOS
char *mk_path(char *path,
	      char *filespec)
#else
char *mk_path(path, filespec)
	char *path;		/* output path */
	char *filespec;		/* input filespec */
#endif
{
	char *p;

	strcpy(path, filespec);
	if (!(p = strchr(path, ']')) && !(p = strchr(path, ':')))
		p = path - 1;	/* return null string if no path */
	*++p = '\0';

	return path;
}


/*
 * mk_filespec - merge VMS path and file names, where latter can be relative
 */
#ifdef PROTOS
char *mk_filespec(char *filespec,
		  char *path,
		  char *name)
#else
char *mk_filespec(filespec, path, name)
	char *filespec;		/* output filespec */
	char *path;		/* input path */
	char *name;		/* input file name */
#endif
{
	char *p;

	*filespec = '\0';

	/* copy name intact if absolute; else merge path and relative name */
	if (!strchr(name, ':')) {
		strcpy(filespec, path);
		if ((p = P_LASTCHAR(filespec)) && *p == END_PATH &&
		    name[0] == START_PATH && strchr(".-", name[1]))
			*p = *++name == '-' ? '.' : '\0';
	}

	return strcat(filespec, name);
}


/*
 * trnlog - return translation of VMS logical name (null if missing)
 */
#ifdef PROTOS
char *trnlog(char *logname)
#else
char *trnlog(logname)	/* look up logical name */
	char *logname;
#endif
{
	static char trnbuf[STRSIZ];
	
	$DESCRIPTOR(src, logname);
	$DESCRIPTOR(dst, trnbuf);
	short len;
	int ret;
	
	src.dsc$w_length  = strlen(logname);
	ret = LIB$SYS_TRNLOG(&src, &len, &dst);
	return ret == SS$_NORMAL ? (trnbuf[len] = '\0', trnbuf) : NULL;
}

#else		/* apparently DOS and Amiga can use the Un*x flavors */

/*
 * mk_path - extract the path component from a Un*x file spec
 */
#ifdef PROTOS
char *mk_path(char *path,
	      char *filespec)
#else
char *mk_path(path, filespec)
	char *path;		/* output path */
	char *filespec;		/* input filespec */
#endif
{
	char *p;

	strcpy(path, filespec);
	if (! (p = strrchr(path, END_PATH)) )
		p = path - 1;	/* return null string if no path */

	*++p = '\0';
	return path;
}


/*
 * mk_filespec - merge Un*x path and file names, where latter can be relative
 */
#ifdef PROTOS
char *mk_filespec(char *filespec,
		  char *path,
		  char *name)
#else
char *mk_filespec(filespec, path, name)
	char *filespec;		/* output filespec */
	char *path;		/* input path */
	char *name;		/* input file name */
#endif
{
	char *p;

	*filespec = '\0';

	/* copy name intact if absolute; else merge path and relative name */

	/* if name starts with "~/", translate it for user */
	if (strncmp(name, "~/", 2) == 0 && (p = trnlog(HOME_DIR)) != NULL) {
		strcpy(filespec, p);
		TERM_PATH(filespec);
		name += 2;		/* skip "~/" */
	}
	else if (*name != START_PATH) {		/* relative path */
		/* if path starts with "~/", translate it for user */
		if (strncmp(path, "~/", 2) == 0 &&
		    (p = trnlog(HOME_DIR)) != NULL) {
			strcpy(filespec, p);
			TERM_PATH(filespec);
			path += 2;	/* skip "~/"; append rest below */
		}
		strcat(filespec, path);
		TERM_PATH(filespec);
	}

	return strcat(filespec, name);
}


/*
 * trnlog - return translation of Un*x environment variable
 */
#ifdef PROTOS
char *trnlog(char *logname)
#else
char *trnlog(logname)	/* look up logical name */
	char *logname;
#endif
{
	return getenv(logname);
}

#endif

#ifdef UNIXX	/* highly Un*x-dependent; probably nobody else can use it */

#include <sys/types.h>
#include <sys/stat.h>

/* X_OK is a #define'd constant used by access() to determine whether or not
 * the specified file is executable by the user.  Investigation of several
 * Un*x systems reveals that 01 appears to be the standard value, but it would
 * be best to #include the appropriate system .h file instead of hard-coding
 * the value (as below).  (The hard-coded approach was adopted reluctantly
 * because there does not appear to be a standardized location for X_OK: some
 * systems put it in <unistd.h>, others in <access.h>, and others elsewhere
 * (if at all).  The access() man page may be helpful.)  (AWR 08/20/91)
 */
#define X_OK	01		/* does user have execute permission? */

/*
 * find_executable - return full path name of executable
 */
#ifdef PROTOS
char *find_executable(char *prog)
#else
char *find_executable(prog)
	char *prog;
#endif
{
	char pathvar[1000], *p, *pnext;
	struct stat st;
	static char filepath[STRSIZ];
	
	/* if 'prog' is an absolute/relative path name or environment
	 * variable 'PATH' does not exist, return 'prog'; otherwise,
	 * search the directories specified in 'PATH' for the executable
	 */
	if (strchr(prog, END_PATH) == 0 && (p = getenv(PATH_ENV_VAR)) != 0) {
		strcpy(pathvar, p);

		/* assumes PATH is of form "dir1:dir2:...:dirN"; strtok()
		 * would be handy here, but not everybody has it yet
		 */
		for (p = pathvar; *p; p = pnext) {
			if ((pnext = strchr(p, ':')) != NULL)
				*pnext++ = '\0';
			else
				pnext = p + strlen(p);

			/* assume the executable lives in the path, and
			 * construct its complete file name
			 */
			filepath[0] = '\0';
			if (strcmp(p, ".") != 0) {
				strcat(filepath, p);
				strcat(filepath, "/");
			}
			strcat(filepath, prog);

			/* check that file a) exists, b) is a "normal" file,
			 * and c) is executable - if so, return its full path
			 */
			if (stat(filepath, &st) >= 0 &&
			   (st.st_mode & S_IFMT) == S_IFREG &&
		            access(filepath, X_OK) == 0)
				return filepath;
		}
	}

	return prog;
}
#else


/*
 * find_executable - return full path name of executable
 */
#ifdef PROTOS
char *find_executable(char *prog)
#else
char *find_executable(prog)
	char *prog;
#endif
{
	return prog;	/* non-Un*x flavor always returns its argument */
}
#endif
