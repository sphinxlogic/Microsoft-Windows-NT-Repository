/*
 * writefil.c - Pcal routines concerned with writing the PostScript output
 *
 * Contents:
 *
 *		def_footstring
 *		expand_fmt
 *		find_daytext
 *		find_holidays
 *		find_noteboxes
 *		print_db_word
 *		print_julian_info
 *		print_month
 *		print_moon_info
 *		print_dates
 *		print_pstext
 *		print_text
 *		print_word
 *		write_calfile
 *		write_psfile
 *
 * Revision history:
 *
 *	4.3	AWR	12/03/91	add support for -s flag (specify
 *					alternate date/fill box shading values)
 *
 *	4.2	AWR	10/08/91	add support for -[kK] flags (change
 *					position of small calendars)
 *
 *			10/03/91	add find_noteboxes(); revise to print
 *					text in multiple notes boxes
 *
 *					add support for -S flag
 *
 *			10/02/91	modify def_footstring() to handle all
 *					types of strings; use it to print notes
 *					header (-N flag)
 *
 *			09/19/91	add write_calfile(), print_dates(),
 *					and new print_text() to generate 
 *					input for Un*x "calendar" utility;
 *					renamed old print_text() as
 *					print_pstext() for clarity; revised
 *					to simplify setting working date
 *
 *	4.11	AWR	08/23/91	revise expand_fmt() to write results
 *					to string instead of stdout; revise
 *					print_word() to avoid writing null
 *					strings
 *
 *		AWR	08/21/91	use ABBR_DAY_LEN and ABBR_MONTH_LEN
 *					(cf. pcallang.h) to print abbreviated
 *					day/month names
 *
 *		AWR	08/21/91	add %u and %w (calculate week number
 *					so that 1/1 is always week 1); support
 *					%[+-]<n>[DWMY] to adjust working date
 *					by +|- <n> days/weeks/months/years
 *
 *	4.1	AWR	08/16/91	Support -G flag (outlined gray dates)
 *
 *	4.02	AWR	07/02/91	Added "%" expansions in text strings
 *					(cf. expand_fmt())
 *
 *	4.0	AWR	01/28/91	Support -B, -w flags and moon file
 *
 *			01/15/91	Extracted from pcal.c
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
#define  WRITEFIL		/* to get ordinal_suffix() from pcallang.h */
#include "pcallang.h"
#include "pcalinit.h"		/* PostScript boilerplate */

/*
 * Macros:
 */

/* make sure PRT() doesn't round "ph" up to 1.0 when printing it */
#define PRT_TWEAK(ph)		((ph) >= 0.9995 ? 0.0 : (ph))

/* advance working date by n days */
#define SET_DATE(n)	do {						\
	MAKE_DATE(date, work_month, work_day + (n), work_year);		\
	normalize(&date);						\
	work_month = date.mm, work_day = date.dd, work_year = date.yy;	\
} while (0)

/* reset working date to original date */
#define RESET_DATE	\
work_month = this_month, work_day = this_day, work_year = this_year

/*
 * Globals:
 */

/* order of following strings must conform to #define's in pcaldefs.h (q.v.) */
static char *cond[3] = {"false", "true", "(some)"};
static char *gray[3] = {"(gray)", "(outline)", "(outline_gray)"};

static int this_day, this_month, this_year;	/* current day */
static int work_day, work_month, work_year;	/* working day (cf. expand_fmt()) */
static char *kw_note, *kw_opt, *kw_year;	/* keywords for -c output */

static int debug_text;				/* generate debug output */


/*
 * write_psfile - write PostScript code
 *
 * The actual output of the PostScript code is straightforward.  This routine
 * writes a PostScript header followed by declarations of all the PostScript
 * variables affected by command-line flags and/or language dependencies.  It
 * the generates the PostScript boilerplate generated from pcalinit.ps, and
 * finally calls print_month() to generate the PostScript code for each
 * requested month.
 */
#ifdef PROTOS
void write_psfile(int month,
		  int year,
		  int nmonths)
#else
void write_psfile(month, year, nmonths)
	int month;			/* starting month   */
	int year;			/* starting year    */
	int nmonths;			/* number of months */
#endif
{
	int i, ngray, nfsize;
	char *p, **ap, tmp[STRSIZ];

	debug_text = DEBUG(DEBUG_TEXT);		/* debug text output? */

	/*
	 * Write out PostScript prolog
	 */

 	PRT("%%!\n%%\t");
	PRT(VERSION_MSG, progname, version);
	if (*datefile)
		PRT(DATEFILE_MSG, datefile);
	PRT("\n%%\n");

	/* font names and sizes (notes font only in 4.2) */

	nfsize = (p = strrchr(notesfont, '/')) ? *p = '\0', atoi(p + 1) :
				atoi(strrchr(NOTESFONT, '/') + 1);

	PRT("/titlefont /%s def\n/dayfont /%s def\n/notesfont /%s def\n",
	    titlefont, dayfont, notesfont);
	PRT("/notesfontsize %d def\n", nfsize);

	/* month names */

	PRT("/month_names [");
	for (i = JAN; i <= DEC; i++) {
		PRT(i % 6 == 1 ? "\n\t" : " ");
		(void) print_word(months[i-1]);
	}
	PRT(" ] def\n");

	/* day names - abbreviate if printing entire year on page */

	PRT("/day_names [");
	for (i = SUN; i <= SAT; i++) {
		PRT(i % 6 == 0 && ! do_whole_year ? "\n\t" : " ");
		strcpy(tmp, days[(i + first_day_of_week) % 7]);
		if (do_whole_year)
			tmp[ABBR_DAY_LEN] = '\0';
		(void) print_word(tmp);
		}
	PRT(" ] def\n");

	/* line separator */

	PRT("/linesep ");
	print_word(LINE_SEP);
	PRT(" def\n");

	/* colors (black/gray) to print weekdays and holidays */

	PRT("/day_gray [");
	for (ngray = 0, i = SUN; i <= SAT; ngray += day_color[i++] == GRAY)
		PRT(" %s", cond[day_color[(i + first_day_of_week) % 7]]);
	PRT(" ] def\n");
	PRT("/holiday_gray %s def\n", cond[ngray <= 3]);
	PRT("/logical_gray %s def\n", gray[num_style]);

 	/* rotation, scale, and translate values */
 
 	PRT("/rval %d def\n", rotate);
 	PRT("/xsval %s def\n/ysval %s def\n", xsval, ysval);
 	PRT("/xtval %s def\n/ytval %s def\n", xtval, ytval);

	/* moon, Julian date, and box fill flags */

	PRT("/draw-moons %s def\n", cond[draw_moons]);
	PRT("/julian-dates %s def\n", cond[julian_dates]);
	PRT("/fill-boxes %s def\n", cond[! blank_boxes]);

	/* position of small calendars */

	PRT("/prev_small_cal %d def\n", prev_cal_box[small_cal_pos]);
	PRT("/next_small_cal %d def\n", next_cal_box[small_cal_pos]);

	/* date and fill box shading values */

	strcpy(tmp, shading);
	*(p = strchr(tmp, '/')) = '\0';
	PRT("/dategray %s def\n", tmp);
	PRT("/fillgray %s def\n", p + 1);

	/* PostScript boilerplate (part 1 of 1) */

	for (ap = header; *ap; ap++)
		PRT("%s\n", *ap);
	PRT("\n");

	/*
	 * Write out PostScript code to print calendars
	 */

	for (this_month = month, this_year = year; nmonths--; ) {
		print_month(this_month, this_year);
		this_year = NEXT_YEAR(this_month, this_year);
		this_month = NEXT_MONTH(this_month, this_year);
	}

}


/*
 * write_calfile - write dates in format suitable for Un*x "calendar" utility
 * (and subsequent use by Pcal)
 */
#ifdef PROTOS
void write_calfile(int month,
		   int year,
		   int nmonths)
#else
void write_calfile(month, year, nmonths)
	int month;			/* starting month   */
	int year;			/* starting year    */
	int nmonths;			/* number of months */
#endif
{
	KWD *k;

	/* look up the Pcal keywords (assumed present) for the -c output file */
	for (k = keywds; k->name; k++) {
		if (k->code == DT_NOTE) kw_note = k->name;
		if (k->code == DT_OPT)  kw_opt  = k->name;
		if (k->code == DT_YEAR) kw_year = k->name;
	}

	/* print the date style for subsequent use by Pcal */
	PRT("%s -%c\n", kw_opt, date_style == USA_DATES ? F_USA_DATES :
							  F_EUR_DATES);

	for (this_month = month, this_year = year; nmonths--; ) {
		print_dates(this_month, this_year);
		this_year = NEXT_YEAR(this_month, this_year);
		this_month = NEXT_MONTH(this_month, this_year);
	}
}

/*
 * low-level utilities for PostScript generation
 */

/*
 * expand_fmt - expand a strftime-like date format specifier; pcal supports
 * %[aAbBdjmUWyY] from strftime() plus %[luwDM] and prefixes [0o+-] (see below);
 * places expanded string in output buffer and returns pointer to character
 * following end of format specifier.  Assumes working date has been initialized
 * (via RESET_DATE macro) prior to first call for a given text string
 */
#ifdef PROTOS
char *expand_fmt(char *buf,
		 char *p)
#else
char *expand_fmt(buf, p)
	char *buf;		/* output buffer (filled in)	    */
	char *p;		/* character following percent sign */
#endif
{
	char c;
	static char *prefixes = "0o+-";
	int firstday, wkday;
	int adjust = 0, print_lz = FALSE, ordinal = FALSE, prev_num = -1;
	int num_present = FALSE, num_value = 0;
	DATE date;

	/* For compatibility with version 4.1, still support %[+-][bBdmY]
	 * (print the next/last month-name/day/month/year).  Version 4.11
	 * introduces %[+-]<n>[DWMY], which adjusts the working date by
	 * [+-]<n> days/weeks/months/years; this method is preferred due
	 * to its greater flexibility.
	 */

	buf[0] = '\0';		/* initialize output to null string */
	
	do {			/* loop until format character found */
		switch (c = *p++) {
		case 'a':	/* %a : abbreviated weekday */
		case 'A':	/* %A : full weekday */
			wkday = calc_weekday(work_month, work_day, work_year);
			strcpy(buf, days[wkday]);
			if (c == 'a')
				buf[ABBR_DAY_LEN] = '\0';
			break;

		case 'b':	/* %b : abbreviated month name */
		case 'B':	/* %B : full month name */
			strcpy(buf, months[(work_month + adjust + 11) % 12]);
			if (c == 'b')
				buf[ABBR_MONTH_LEN] = '\0';
			break;

		case 'd':	/* %d : day of month (01-31) */
			prev_num = work_day;
			sprintf(buf, print_lz ? "%02d" : "%d", prev_num);
			break;

		case 'D':	/* %D : adjust working date by <N> days (NEW) */
			if (!num_present || num_value == 0)
				RESET_DATE;
			else
				SET_DATE(adjust * num_value);
			break;

		case 'j':	/* %j : day of year (001-366) */
			prev_num = DAY_OF_YEAR(work_month, work_day,
					work_year);
			sprintf(buf, print_lz ? "%03d" : "%d", prev_num);
			break;

		case 'l':	/* %l : days left in year (000-365) (NEW) */
			prev_num = YEAR_LEN(work_year) - DAY_OF_YEAR(work_month,
					work_day, work_year);
			sprintf(buf, print_lz ? "%03d" : "%d", prev_num);
			break;

		case 'm':	/* %m : month (01-12) */
			prev_num = (work_month + adjust + 11) % 12 + 1;
			sprintf(buf, print_lz ? "%02d" : "%d", prev_num);
			break;

		case 'M':	/* %M : adjust date by <N> months (NEW) */
			if (!num_present || num_value == 0)
				RESET_DATE;
			else {
				int len;

				work_month += adjust * num_value;
				while (work_month > DEC) {
					work_month -= 12;
					work_year++;
				}
				while (work_month < JAN) {
					work_month += 12;
					work_year--;
				}

				/* make sure day of new month is legal */
				len = LENGTH_OF(work_month, work_year);
				if (work_day > len)
					work_day = len;
			}
			break;

		/* %u considers the week containing 1/1 to be week 1 and
		 * the next "logical Sunday" (the first day of the week
		 * as printed - cf. the -F option) to be the start of week
		 * 2; %U considers the first "logical Sunday" of the year to
		 * be the start of week 1.  %w and %W behave like %u and %U
		 * respectively, but use the first "logical Monday" instead.
		 */
		case 'W':	/* %W : week number (00-53)       */
			/* %W, if prefaced by [+-]N, adjusts the date by
			 * [+-]N weeks (resets if N == 0); check for this
			 * case first
			 */
			if (num_present) {
				if (num_value == 0)	/* N = 0: reset date */
					RESET_DATE;
				else
					SET_DATE(7 * adjust * num_value);
				break;
			}
			/* fall through */
		case 'u':	/* %u : week number (01-54) (NEW) */
		case 'U':	/* %U : week number (00-53)       */
		case 'w':	/* %w : week number (01-54) (NEW) */
			firstday = ((TOLOWER(c) == 'w' ? 15 : 14) -
					START_BOX(JAN, work_year)) % 7 + 1;
			prev_num = (DAY_OF_YEAR(work_month, work_day,
					work_year) - firstday + 7) / 7;
			if (islower(c) && firstday != 1)
				prev_num++;
			sprintf(buf, print_lz ? "%02d" : "%d", prev_num);
			break;

		case 'y':	/* %y : year w/o century (00-99) */
			prev_num = (work_year + adjust) % 100;
			sprintf(buf, "%02d", prev_num);
			break;

		case 'Y':	/* %Y : year w/century */
			/* %Y, if prefaced by [+-]N, adjusts the date by
			 * [+-]N years (resets if N == 0); check for this
			 * case first
			 */
			if (num_present) {
				if (num_value == 0)	/* N = 0: reset date */
					RESET_DATE;
				else {
					int len;

					work_year += adjust * num_value;

					/* make sure day is legal */
					len = LENGTH_OF(work_month, work_year);
					if (work_day > len)
						work_day = len;
				}
			} else {
				prev_num = work_year + adjust;
				sprintf(buf, "%d", prev_num);
			}
			break;

		/* prefix flags [o0+-] : set flags for next pass */

		case 'o':	/* %o : ordinal suffix (NEW) */
			ordinal = TRUE;
			break;

		case '0':	/* %0 : add leading zeroes (NEW) */
			print_lz = TRUE;
			break;

		case '+':	/* %+ : increment next value (NEW) */
		case '-':	/* %- : decrement next value (NEW) */
			adjust = c == '-' ? -1 : 1;
			if (isdigit(*p)) {		/* get the number */
				num_present = TRUE;
				while (isdigit(*p))
					num_value = num_value * 10 +
							(*p++ - '0');
			}
			break;

		case '\0':	/* accidental end-of-string */
		case ' ':
			return p - 1;

		default:	/* other - just copy it to output */
			sprintf(buf, "%c", c);
			break;
		};

	} while (strchr(prefixes, c) != NULL);

	/* append ordinal suffix if requested */
	if (ordinal && prev_num >= 0)
		strcat(buf, ordinal_suffix(prev_num));
	return p;

}


/*
 * print_word - print a single word, representing punctuation and non-ASCII
 * characters as octal literals and expanding format specifiers; return pointer
 * to character following word (NULL if no word follows)
 */
#ifdef PROTOS
char *print_word(char *p)
#else
char *print_word(p)
	char *p;
#endif
{
	char c, buf[20];
	int first = TRUE;	/* flag to avoid printing null strings */

	if (*p == '\0' || *(p += strspn(p, WHITESPACE)) == '\0')
		return NULL;

	while ((c = *p) && !isspace(c)) {
		if (c == '%' && p[1] != '\0') {
			p = expand_fmt(buf, p + 1);
			if (*buf && first) {
				PRT("(");
				first = FALSE;
			}	
			PRT("%s", buf);
		} else {
			if (first)
				PRT("(");
			first = FALSE;
			PUTCHAR(c);
			p++;
		}
	}

	if (!first)
		PRT(")");

	return p;
}


/*
 * print_db_word - debug version of print_word; omits parentheses, does not
 * convert punctuation to escape sequences, and writes results to stderr
 * (not stdout)
 */
#ifdef PROTOS
char *print_db_word(char *p)
#else
char *print_db_word(p)
	char *p;
#endif
{
	char c, buf[20];

	if (*p == '\0' || *(p += strspn(p, WHITESPACE)) == '\0')
		return NULL;

	while ((c = *p) && !isspace(c)) {
		if (c == '%' && p[1] != '\0') {
			p = expand_fmt(buf, p + 1);
			FPR(stderr, "%s", buf);
		} else {
			FPR(stderr, isprint(c) ? "%c" : "\\%03o", c & 0377);
			p++;
		}
	}

	return p;
}


/*
 * print_pstext - print tokens in text (assumed separated by single blank)
 * in PostScript format and as debugging information if requested
 */
#ifdef PROTOS
void print_pstext(char *p)
#else
void print_pstext(p)
	char *p;
#endif
{
	char *s = p;		/* save for possible second pass */
	
	while (p = print_word(p))
		PRT(" ");

	/* repeat to generate debugging info if requested */
	if (debug_text)
		while (s = print_db_word(s))
			FPR(stderr, " ");

}


/*
 * print_text - print text as supplied; expand format specifiers but
 * do not tokenize into words or translate punctuation to escape sequences
 */
#ifdef PROTOS
void print_text(char *p)
#else
void print_text(p)
	char *p;
#endif
{
	char c, buf[20];

	while (c = *p)
		if (c == '%' && p[1] != '\0') {
			p = expand_fmt(buf, p + 1);
			PRT("%s", buf);
		} else {
			PRT(isprint(c) ? "%c" : "\\%03o" , (c) & 0377);
			p++;
		}
}


/*
 * def_footstring - print definition for foot string, again converting 
 * all characters other than letters, digits, or space to octal escape
 * and expanding format specifiers
 */
#ifdef PROTOS
void def_footstring(char *p,
		    char *str)
#else
void def_footstring(p, str)
	char *p;			/* definition */
	char *str;			/* name of string */
#endif
{
	char c, buf[20];

	this_day = 1;			/* set default day in foot string */
	RESET_DATE;			/* reset working date */

	PRT("/%s (", str);
	while (c = *p)
		if (c == '%' && p[1] != '\0') {
			p = expand_fmt(buf, p + 1);
			PRT("%s", buf);
		} else {
			PUTCHAR(c);
			p++;
		}
	PRT(") def\n");
}


/*
 * Routines to extract and print data
 */


/*
 * find_daytext - find and print the day (including notes) or holiday text
 * for the specified month/year
 */
#ifdef PROTOS
void find_daytext(int month,
		  int year,
		  int is_holiday)
#else
void find_daytext(month, year, is_holiday)
	int month, year;
	int is_holiday;		/* TRUE: print holiday text */
#endif
{
	register int day;
	year_info *py;
	month_info *pm;
	register day_info *pd;
	int first;
	char *fcn = is_holiday ? "holidaytext" : "daytext";
	char hol = is_holiday ? '*' : ' ';

	/* if no text for this year and month, return */

	if ((py = find_year(year, FALSE)) == NULL ||
	    (pm = py->month[month-1]) == NULL)
		return;

	/* walk array of day text pointers and linked lists of text */

	for (day = 1; day <= LAST_NOTE_DAY; day++) {
		for (pd = pm->day[day-1], first = TRUE;
		     pd;
		     pd = pd->next) {
			if (pd->is_holiday != is_holiday)
				continue;
			if (first) {
				PRT("%d [ \n", day >= FIRST_NOTE_DAY ?
					note_box(month, day, year) : day);
			}
			else {
				PRT("\n");
				print_word(LINE_SEP);	/* separate lines */
				PRT("\n");
			}
			this_day = day >= FIRST_NOTE_DAY ? 1 : day;
			RESET_DATE;		/* reset working date */
			if (debug_text)
				if (day < FIRST_NOTE_DAY)
					FPR(stderr, "%02d/%02d/%d%c ", month,
						day, year, hol);
				else
					FPR(stderr, "%02d[%02d]%d  ", month,
						day - FIRST_NOTE_DAY + 1, year);
			print_pstext(pd->text);
			if (debug_text)
				FPR(stderr, "\n");
			first = FALSE;
		}
		if (! first) {		/* wrap up call (if one made) */
			PRT("\n] %s\n", day >= FIRST_NOTE_DAY ? "notetext" :
								fcn);
		}
	}
}


/*
 * find_holidays - find and print the holidays for specified month/year
 */
#ifdef PROTOS
void find_holidays(int month,
		   int year)
#else
void find_holidays(month, year)
	int month, year;
#endif
{
	register int day;
	register unsigned long holidays;
	year_info *py;
	month_info *pm;

	pm = (py = find_year(year, FALSE)) ? py->month[month-1] : NULL;
	PRT("/holidays [");	/* start definition of list */

	for (holidays = pm ? pm->holidays : 0, day = 1;
	     holidays;
	     holidays >>= 1, day++)
		if (holidays & 01)
			PRT(" %d", day);

	PRT(" ] def\n");

}


/*
 * find_noteboxes - find and print the note box numbers for specified
 * month/year
 */
#ifdef PROTOS
void find_noteboxes(int month,
		    int year)
#else
void find_noteboxes(month, year)
	int month, year;
#endif
{
	register int day;
	year_info *py;
	month_info *pm;

	/* if no text for this year and month, print empty list and return */

	if ((py = find_year(year, FALSE)) == NULL ||
	    (pm = py->month[month-1]) == NULL) {
		PRT("/noteboxes [ ] def\n");
		return;
	}

	PRT("/noteboxes [");	/* start definition of list */

	/* walk array of note text pointers, converting days to box numbers */

	for (day = FIRST_NOTE_DAY; day <= LAST_NOTE_DAY; day++)
		if (pm->day[day-1])
			PRT(" %d", note_box(month, day, year));
	PRT(" ] def\n");

}


/*
 * print_dates - print the dates (in "calendar" format) for the specified
 * month and year
 */
#ifdef PROTOS
void print_dates(int month,
		 int year)
#else
void print_dates(month, year)
	int month, year;
#endif
{
	register int day;
	year_info *py;
	month_info *pm;
	register day_info *pd;
	unsigned long holidays;
	int has_holiday_text;
	static int first = TRUE;
	static int save_year = 0;

	/* if no text for this year and month, return */

	if ((py = find_year(year, FALSE)) == NULL ||
	    (pm = py->month[month-1]) == NULL)
		return;

	/* print the year if it has changed */

	if (year != save_year)
		PRT("%s %d\n", kw_year, save_year = year);

	/* walk array of day text pointers and linked lists of text */

	for (holidays = pm->holidays, day = 1;
	     day < FIRST_NOTE_DAY;
	     holidays >>= 1, day++) {
		has_holiday_text = FALSE;
		for (pd = pm->day[day-1]; pd; pd = pd->next) {
			if (date_style == USA_DATES)
				PRT("%02d/%02d", month, day);
			else
				PRT("%02d/%02d", day, month);
			PRT(pd->is_holiday ? "* " : " ");
			this_day = day;
			RESET_DATE;	/* reset working date */
			print_text(pd->text);
			PRT("\n");
			has_holiday_text |= pd->is_holiday;
		}
		/* was date flagged as holiday w/o associated text? */
		if ((holidays & 01) && !has_holiday_text) {
			if (date_style == USA_DATES)
				PRT("%02d/%02d*\n", month, day);
			else
				PRT("%02d/%02d*\n", day, month);
		}
	}
}



/*
 * print_moon_info - print the information necessary to draw moons.  If
 * printing moons on all days, print the phase for each day; if printing
 * only quarter moons, tweak the phase to an exact quarter (so the icon
 * is printed correctly) and generate a list of the quarter-moon dates
 */
#ifdef PROTOS
void print_moon_info(int month,
		     int year)
#else
void print_moon_info(month, year)
	int month, year;
#endif
{
	int n, ndays, day, quarter;
	char *p;
	unsigned long qdays;
	double phase;
	static char *q[4] = {"NM", "1Q", "FM", "3Q"};

	if (draw_moons == NO_MOONS)
		return;

	/* print the phase of the moon for each day of the month */

	PRT("/moon_phases [\t\t%% from %s\n\t",
		(p = find_moonfile(year)) ? p : "algorithm");

	for (n = 0, qdays = 0L, day = 1, ndays = LENGTH_OF(month, year);
	     day <= ndays;
	     day++) {
		phase = find_phase(month, day, year, &quarter);
		if (DEBUG(DEBUG_MOON))
			FPR(stderr, "%02d/%02d/%d: %.3f %s\n", month, day,
				year, phase, quarter != MOON_OTHER ?
				q[quarter] : "");
		/* adjust phase to exact quarter if printing only quarters */
		if (draw_moons == SOME_MOONS && quarter != MOON_OTHER)
			phase = 0.25 * quarter;
		if (draw_moons == ALL_MOONS || quarter != MOON_OTHER)
			PRT("%.3f%s", PRT_TWEAK(phase), ++n % 10 == 0 ?
			    "\n\t" : " ");
		if (quarter != MOON_OTHER)
			qdays |= 1L << (day - 1);
		}
	PRT("] def\n");

	/* if drawing only quarter moons, print days when they occur */

	if (draw_moons == SOME_MOONS) {
		PRT("/quarter_moons [ ");
		for (day = 1; qdays; day++, qdays >>= 1)
			if (qdays & 01)
				PRT("%d ", day);
		PRT("] def\n");
	}
}


/*
 * print_julian_info - print the information necessary to print Julian dates
 */
#ifdef PROTOS
void print_julian_info(int month,
		       int year)
#else
void print_julian_info(month, year)
	int month, year;
#endif
{

	if (julian_dates != NO_JULIANS)
		PRT("/jdstart %d def\n", DAY_OF_YEAR(month, 1, year));
	if (julian_dates == ALL_JULIANS)
		PRT("/yearlen %d def\n", YEAR_LEN(year));
}


/*
 * print_month - generate calendar for specified month/year
 */
#ifdef PROTOS
void print_month(int month,
		 int year)
#else
void print_month(month, year)
	int month, year;
#endif
{
	static int nmonths = 0;
	int startbox;

	PRT("/year %d def\n", year);		/* set up year and month */
	PRT("/month %d def\n", month);

	/* move starting box to second row if conflict with small calendars */
	startbox = START_BOX(month, year);
	if (!do_whole_year &&
	    (prev_cal_box[small_cal_pos] == startbox ||
	     next_cal_box[small_cal_pos] == startbox) )
		startbox += 7;
	PRT("/startbox %d def\n", startbox);
	PRT("/ndays %d def\n", LENGTH_OF(month, year));

	find_noteboxes(month, year);		/* make list of note boxes */
	find_holidays(month, year);		/* make list of holidays */

	/* are we printing 12 months per page or only one? */

	if (do_whole_year) {
		/* reset foot strings at start of each page */
		if (nmonths % 12 == 0) {
			def_footstring(lfoot, "Lfootstring");
			def_footstring(cfoot, "Cfootstring");
			def_footstring(rfoot, "Rfootstring");
			def_footstring(notes_hdr, "notesheading");
		}

		PRT("/posn %d def\n", nmonths % 12);	/* location on page */
		PRT("printmonth_%c\n", rotate == LANDSCAPE ? 'l' : 'p');
		if (++nmonths % 12 == 0)
			PRT("showpage\n");
	}
	else {
		/* reset foot strings each month (may change) */
		def_footstring(lfoot, "Lfootstring");
		def_footstring(cfoot, "Cfootstring");
		def_footstring(rfoot, "Rfootstring");
		def_footstring(notes_hdr, "notesheading");

		/* generate information necessary for small calendars */

		if (small_cal_pos != SC_NONE) {
			int m, y;

			PRT("/p_year %d def\n", y = PREV_YEAR(month, year));
			PRT("/p_month %d def\n", m = PREV_MONTH(month, year));
			PRT("/p_startbox %d def\n", START_BOX(m, y));
			PRT("/p_ndays %d def\n", LENGTH_OF(m, y));

			PRT("/n_year %d def\n", y = NEXT_YEAR(month, year));
			PRT("/n_month %d def\n", m = NEXT_MONTH(month, year));
			PRT("/n_startbox %d def\n", START_BOX(m, y));
			PRT("/n_ndays %d def\n", LENGTH_OF(m, y));
		}

		print_julian_info(month, year);		/* Julian date info */
		print_moon_info(month, year);		/* moon info */

		PRT("printmonth\n");
		find_daytext(month, year, TRUE);	/* holiday text */
		find_daytext(month, year, FALSE);	/* day and note text */
		PRT("showpage\n");
	}
}
