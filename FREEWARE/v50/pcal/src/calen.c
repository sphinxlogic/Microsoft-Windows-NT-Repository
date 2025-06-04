/*
 * Calendar program - one month per page
 *
 * Author: AW Rogers
 *
 *	Originally written in FORTRAN-IV for GE Timesharing, 10/65
 *	Re-coded in C for UNIX, 3/83; adapted for VMS 2/85; y2k changes
 *	added 2/99.
 *      12-FEB-2000 Rick Dyson (rick-dyson@uiowa.edu)
 *                  OpenVMS fix-up and "-h" option added
 *
 * Parameters:
 *
 *	calen	generate calendar for current month/year
 *
 *	calen yy	generate calendar for entire year yy
 *
 *	calen mm yy	generate calendar for month mm (1 = January), year yy
 *			(CCyy if yy < 100)
 *
 *	calen mm yy n	as above, for n consecutive months
 *
 * Options:
 *
 *	-b<N>	add N blank lines at top of each page
 *
 *	-f<FILE> write output to FILE (calen.lst if -f alone)
 *
 *	-l	left-justify dates within boxes (default)
 *
 *	-r	right-justify dates within boxes
 *
 *	-o<STR>	use characters in STR as overstrike sequence for
 *		printing large month/year (default: HIX)
 *
 *	-t	print trailing dates (30, 31 in 23/30 and 24/31) in
 *		vacant box on first line
 *
 *
 * Output:
 *
 *	Full-page calendar for each of the specified months, written to
 *	stdout unless -f option used.
 *
 */

#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include <string.h>

#ifndef FALSE
#define	FALSE	0
#define	TRUE	1
#endif

#define	JAN	1		/* significant months/years */
#define	FEB	2
#define	DEC	12
#define	MINYR	1753
#define	MAXYR	9999
#define	CENTURY	1900		/* base year for struct tm */

#define	SOLID	0		/* box styles (cf. box_line) */
#define	OPEN	1

#define	LEFT	0		/* date justification within boxes */
#define	RIGHT	1
#define	DEFAULT_JUST	LEFT

#define	TOP	0		/* trailing date position */
#define	BOTTOM	1
#define	DEFAULT_TRAIL	BOTTOM

#define	TOP_ROW	0		/* top and bottom rows of calendar */
#define	BOTTOM_ROW	5

#define	OVERSTRIKE	"HIX"	/* overstrike sequence for heading */
#define	MAX_OVERSTR	4

#define	OUTFILE	"calen.lst"	/* default output file for -f flag */

#define	NUM_BLANKS	0	/* default blank lines after <FF> */
#define	LEFTMARGIN	"  "	/* left margin */

#define	NUM_MONTHS	1	/* default number of months printed */

#define	MAXARGS	3		/* maximum non-flag command-line args */

#ifndef EXIT_FAILURE
#ifdef VMS
#define	EXIT_FAILURE	0
#define	EXIT_SUCCESS	1
#else
#define	EXIT_FAILURE	1
#define	EXIT_SUCCESS	0
#endif
#endif

#ifdef VMS
#define	END_PATH	']'
#else
#define	END_PATH	'/'
#endif

#define	is_leap(y) ((y) % 4 == 0 && ((y) % 100 != 0 || (y) % 400 == 0))

typedef struct {		/* information about a single month */
    int		mm;
    int		yy;
    char	*mmname;
    char	dates[6][7][3];
} month_rec;

typedef month_rec *p_month;	/* pointer to above struct */

/* globals for defaultable command-line parameters and their default values */

int just = DEFAULT_JUST;	/* justification of dates */
int trail = DEFAULT_TRAIL;	/* format for 23/30, 24/31 */
int nblank = NUM_BLANKS;	/* blank lines after <FF> */
char *seq = OVERSTRIKE;		/* overstrike sequence for heading */
int nmonths = NUM_MONTHS;	/* number of months to print */
char *fname = "";		/* output file name */

int century;			/* current century */

/* forward declarations for local functions */

#ifdef __STDC__
void box_line(int, int);
void date_line(p_month, int, int);
void decode(int, int);
void divider_line(char [7][3]);
void fill_calendar(p_month);
void get_params(int, char *[], p_month);
void header_line(char *, int, char);
void print_calendar(p_month, p_month, p_month);
void small_cal_line(p_month, int);
void usage(char *);
#else
void box_line();
void date_line();
void decode();
void divider_line();
void fill_calendar();
void get_params();
void header_line();
void print_calendar();
void small_cal_line();
void usage();
#endif

int
#ifdef __STDC__
main(int argc, char *argv[])
#else
main(argc, argv)
    int argc;
    char *argv[];
#endif
{
    month_rec mRec[3];		/* main and small calendars */
    p_month prev = &mRec[0], curr = &mRec[1], next = &mRec[2], temp;

    /* get/validate command-line parameters and flags */
    get_params(argc, argv, curr);

    /* fill in calendars for previous and current month */
    prev->mm = curr->mm == JAN ? DEC : curr->mm - 1;
    prev->yy = curr->mm == JAN ? curr->yy - 1 : curr->yy;

    fill_calendar(prev);
    fill_calendar(curr);

    /*
     * main loop: print each month of the calendar (with small calendars
     * for the previous and next months in the upper corners); reuse the
     * current and next month's calendars on the subsequent pass
     */

    while (nmonths-- > 0 && curr->yy <= MAXYR) {
	next->mm = curr->mm == DEC ? JAN : curr->mm + 1;
	next->yy = curr->mm == DEC ? curr->yy + 1 : curr->yy;
	fill_calendar(next);	/* fill in following month */

	print_calendar(prev, curr, next);

	temp = prev;		/* swap pointers to months */
	prev = curr;
	curr = next;
	next = temp;
    }

    if (*fname)			/* report output file name */
	fprintf(stderr, "Output is in file %s\n", fname);

    exit(EXIT_SUCCESS);
}

/*
 * Get and validate command-line parameters and flags.	If month/year not
 * specified on command line, generate calendar for current month/year.
 * Exit program if month or year out of range; forgive illegal flags.
 */

void
#ifdef __STDC__
get_params(int argc, char *argv[], p_month curr)
#else
get_params(argc, argv, curr)
    int argc;		/* argument count/vector passed in from main() */
    char *argv[];
    p_month curr;	/* current month record (fill in month/year) */
#endif
{
    int badopt = FALSE;		/* flag set if bad option	*/
    int badpar = FALSE;		/* flag set if bad param	*/
    int nargs = 0;		/* non-flag argument count	*/
    int numargs[MAXARGS];	/* non-flag (numeric) arguments	*/
    char *parg;			/* generic argument pointer	*/
    time_t tmp;			/* temp for system clock	*/
    struct tm *p_tm;		/* ptr to date/time struct	*/
    char *progname;		/* program name (argv[0])	*/
    char *p;

    extern int atoi();

    /* isolate root program name (for use in error messages) */

    progname = **argv ? *argv : "calen";
#ifdef VMS
    if ((p = strrchr(progname, END_PATH)) != NULL)
	progname = ++p;
    if ((p = strchr(progname, '.')) != NULL)
	*p = '\0';
#endif

    /* walk command-line argument list */

    while (--argc) {
	parg = *++argv;
	if (*parg == '-') {
	    switch (*++parg) {
	    case 'b':
		nblank = atoi(++parg);
		break;
	    case 'f':
		fname = *++parg ? parg : OUTFILE;
		if (freopen(fname, "w", stdout) == (FILE *) NULL) {
		    fprintf(stderr, "%s: can't open %s\n", progname, fname);
		    exit(EXIT_FAILURE);
		}
		break;
	    case 'l':
		just = LEFT;
		break;
	    case 'r':
		just = RIGHT;
		break;
	    case 'o':
		if (*++parg)
		    seq = parg;
		break;
	    case 't':
		trail = TOP;
		break;
	    case 'h':
		usage(progname);
                exit(EXIT_SUCCESS);
	    default:
		fprintf(stderr, "%s: invalid flag %s\n", progname, *argv);
		badopt = TRUE;
		break;
	    }
	} else {
	    if (nargs < MAXARGS)	/* non-flag - add to list */
		numargs[nargs++] = atoi(parg);
	}
    }

    /* get and validate non-flag (numeric) arguments */

    time(&tmp);			/* get current date for defaults */
    p_tm = localtime(&tmp);
    century = ((CENTURY + p_tm->tm_year) / 100);	/* for usage message */

    switch (nargs) {
    case 0:			/* no arguments - print current month/year */
	curr->mm = p_tm->tm_mon + 1;
	curr->yy = CENTURY + p_tm->tm_year;
	break;
    case 1:			/* one argument - print entire year */
	curr->mm = JAN;
	curr->yy = numargs[0];
	nmonths = 12;
	break;
    default:			/* 2-3 args - print one or more months */
	curr->mm = numargs[0];
	curr->yy = numargs[1];
	nmonths = nargs > 2 ? numargs[2] : NUM_MONTHS;
	break;
    }

    if (curr->yy >= 0 && curr->yy < 100)	/* treat nn as CCnn */
	curr->yy += 100 * ((CENTURY + p_tm->tm_year) / 100);

    if (nmonths < 1)		/* ensure at least one month */
	nmonths = 1;

    if (curr->mm < JAN || curr->mm > DEC) {	/* validate month */
	fprintf(stderr, "%s: month %d not in range %d..%d\n", progname,
	curr->mm, JAN, DEC);
	badpar = TRUE;
    }

    if (curr->yy < MINYR || curr->yy > MAXYR) {	/* validate year */
	fprintf(stderr, "%s: year %d not in range %d..%d\n", progname,
	curr->yy, MINYR, MAXYR);
	badpar = TRUE;
    }

    if (badpar || badopt)
	usage(progname);

    if (badpar || badopt)
	exit(EXIT_FAILURE);
}

/*
 * Print message explaining correct usage of the command-line arguments and
 * flags.
 */

void
#ifdef __STDC__
usage(char *prog)
#else
usage(prog)
    char *prog;
#endif
{
    fprintf(stderr, "\nUsage:\n\n");
    fprintf(stderr, "\t%s [-bN] [-fFILE] [-t | -r] [-oSTR] [-t]\n", prog);
    fprintf(stderr, "\t\t[ [ [mm] yy] | [mm yy n] ]\n\n");
    fprintf(stderr, "\nValid flags are:\n\n");
    fprintf(stderr,
	"\t-bN\t\tadd N blank lines after each <FF> (default: %d)\n\n",
	NUM_BLANKS);
    fprintf(stderr,
	"\t-fFILE\t\twrite output to file FILE (%s if -f alone)\n\n", OUTFILE);
    fprintf(stderr, "\t-l\t\tleft-justify dates within boxes");
    fprintf(stderr, "%s\n\n", DEFAULT_JUST == LEFT ? " (default)" : "");
    fprintf(stderr, "\t-r\t\tright-justify dates within boxes");
    fprintf(stderr, "%s\n\n", DEFAULT_JUST == RIGHT ? " (default)" : "");
    fprintf(stderr,
	"\t-oSTR\t\tuse characters in STR as overstrike sequence for\n");
    fprintf(stderr,
	"\t\t\tprinting large month/year (default: %s)\n\n", OVERSTRIKE);
    fprintf(stderr,
	"\t-t\t\tmove trailing 30 and 31 to vacant box on top line\n\n");
    fprintf(stderr,
	"\t%s [opts]\t\tgenerate calendar for current month/year\n\n", prog);
    fprintf(stderr,
	"\t%s [opts] yy\t\tgenerate calendar for each month in year yy\n",
	prog);
    fprintf(stderr,
	"\t\t\t\t(%dyy if yy < 100)\n\n", century);
    fprintf(stderr,
	"\t%s [opts] mm yy\tgenerate calendar for month mm\n", prog);
    fprintf(stderr, "\t\t\t\t(Jan = 1), year yy (%dyy if yy < 100)\n\n",
	century);
    fprintf(stderr,
	"\t%s [opts] mm yy n\tas above, for n consecutive months\n\n", prog);
}

/*
 * Print the calendar for the current month, generating small calendars for
 * the previous and following months in the upper corners and the month/year
 * (in 5x9 dot-matrix characters) centered at the top.
 */

void
#ifdef __STDC__
print_calendar(p_month prev, p_month curr, p_month next)
#else
print_calendar(prev, curr, next)
    p_month prev;	/* previous month (upper-left corner)	*/
    p_month curr;	/* current month (main calendar)	*/
    p_month next;	/* following month (upper-right corner)	*/
#endif
{
    static char *wkday[] = {
	" Sunday  ", " Monday  ", " Tuesday ", "Wednesday", "Thursday ",
	" Friday  ", "Saturday "
    };

    int nchars, line, week, day;
    char *blanks = "                     ";	/* 21 blanks for centering */
    char *padding;				/* pointer into 'blanks'   */
    char month_and_year[20];			/* work area		   */
    char *ovr;					/* overstrike sequence	   */

    /* set up month/year heading and appropriate padding to center it */
    nchars = strlen(curr->mmname);
    padding = blanks + (3 * (nchars - 3));
    sprintf(month_and_year, "%s%5d", curr->mmname, curr->yy);

    /* print top-of-form and leading blank lines, if any */
    printf("\f");
    for (line = 0; line < nblank; line++)
	printf("\n");

    /* print month and year in large letters, with small calendars */
    for (line = 0; line < 9; line++) {
	/* print overstruck lines first */
	for (ovr = seq; ovr < seq + MAX_OVERSTR - 1 && ovr[1]; ovr++) {
	    printf("%20s%s", "", padding);
	    header_line(month_and_year, line, *ovr);
	    printf(" %s", padding);
	    printf("\r");
	}
	/* print small calendars and non-overstruck lines */
	small_cal_line(prev, line);
	printf("%s", padding);
	header_line(month_and_year, line, *ovr);
	printf(" %s", padding);
	small_cal_line(next, line);
	printf("\n");
    }

    /* print the weekday names */
    printf("\n");
    box_line(1, SOLID);
    box_line(1, OPEN);
    printf(LEFTMARGIN);
    for (day = 0; day < 7; day++)
	printf("|%13.9s    ", wkday[day]);
    printf("|\n");
    box_line(1, OPEN);

    /* print the first four weeks of the calendar */
    for (week = TOP_ROW; week < BOTTOM_ROW - 1; week++) {
	box_line(1, SOLID);
	date_line(curr, week, just);
	box_line(7, OPEN);
    }

    /* print the fifth week of the calendar */
    box_line(1, SOLID);		/* fifth week */
    date_line(curr, BOTTOM_ROW - 1, just);
    box_line(2, OPEN);

    /* print divider for 23/30 and/or 24/31 if necessary */
    divider_line(curr->dates[BOTTOM_ROW]);
    box_line(3, OPEN);

    /* print the final week (trailing 31 or 30 31) */
    date_line(curr, BOTTOM_ROW, !just);
    box_line(1, SOLID);
}	

/*
 * Fill in the month name and date fields of a calendar record according to its
 * month and year fields.
 */

void
#ifdef __STDC__
fill_calendar(p_month month)
#else
fill_calendar(month)
    p_month month;		/* record to be filled in */
#endif
{
    typedef struct {		/* local info about months of year	 */
	char *name;		/* month name				 */
	int offset[2];		/* offset of m/1 from 1/1 (common, leap) */
	int length[2];		/* length of month (common, leap)	 */
    } month_info;

    static month_info info[12] = {
	{ "January",	{ 0, 0 }, { 31, 31 } },
	{ "February",	{ 3, 3 }, { 28, 29 } },
	{ "March",	{ 3, 4 }, { 31, 31 } },
	{ "April",	{ 6, 0 }, { 30, 30 } },
	{ "May",	{ 1, 2 }, { 31, 31 } },
	{ "June",	{ 4, 5 }, { 30, 30 } },
	{ "July",	{ 6, 0 }, { 31, 31 } },
	{ "August",	{ 2, 3 }, { 31, 31 } },
	{ "September",	{ 5, 6 }, { 30, 30 } },
	{ "October",	{ 0, 1 }, { 31, 31 } },
	{ "November",	{ 3, 4 }, { 30, 30 } },
	{ "December",	{ 5, 6 }, { 31, 31 } }
    };

    int i, first, last, date = 0, y = month->yy, m = month->mm - 1;
    int leap = is_leap(y);

    /* determine start/end of month */

    first = (y + (y-1)/4 - (y-1)/100 + (y-1)/400 + info[m].offset[leap]) % 7;
    last = first + info[m].length[leap] - 1;

    /* fill in 7x6 matrix of dates */
    for (i = 0; i < 42; i++)
	if (i < first || i > last)
	    month->dates[i/7][i%7][0] = '\0';
	else
	    sprintf(month->dates[i/7][i%7], "%2d", ++date);

    /* move trailing 30/31 to top row if requested */
    if (trail == TOP)
	for (i = 0; month->dates[BOTTOM_ROW][i][0]; i++) {
	    strcpy(month->dates[TOP_ROW][i], month->dates[BOTTOM_ROW][i]);
	    month->dates[BOTTOM_ROW][i][0] = '\0';
	}

    /* fill in pointer to name of month */
    month->mmname = info[m].name;
}

/*
 * Print one line of a small calendar (for upper left and right corners);
 * always prints exactly 20 characters.
 */

void
#ifdef __STDC
small_cal_line(p_month month, int line)
#else
small_cal_line(month, line)
    p_month month;	/* information for month to print */
    int line;		/* line to print (0-8; see below) */
#endif
{
    int day;
    char tmp1[10], tmp2[30];

    switch (line) {
    case 0:		/* month and year (centered) */
	strcpy(tmp1, "      ");
	tmp1[(15 - strlen(month->mmname)) / 2] = '\0';
	sprintf(tmp2, "%s %s %4d      ", tmp1, month->mmname, month->yy);
	printf("%-20.20s", tmp2);
	break;
    case 1:		/* blank line */
	printf("%-20.20s", "");
	break;
    case 2:		/* weekday names */
	printf("Su Mo Tu We Th Fr Sa");
	break;
    default:	/* line of calendar (3 = first) */
	for (day = 0; day < 6; day++)
	    printf("%2s ", month->dates[line-3][day]);
	printf("%2s", month->dates[line-3][6]);
	break;
    }
}

/*
 * Print n calendar box lines in selected style
 */

void
#ifdef __STDC__
box_line(int n, int style)
#else
box_line(n, style)
    int n;		/* number of lines to print */
    int style;		/* SOLID or OPEN	    */
#endif
{
    int day;
    char *fmt = style == SOLID ? "+-----------------" : "|                 ";

    for (; n > 0; n--) {
	printf(LEFTMARGIN);
	for (day = 0; day < 7; day++)
	    printf(fmt);
	printf("%c\n", *fmt);
    }
}


/*
 * Print one week of dates, left- or right- justified
 */

void
#ifdef __STDC__
date_line(p_month month, int week, int just)
#else
date_line(month, week, just)
    p_month month;	/* pointer to month data	 */
    int week;		/* week to print (0 = first)	 */
    int just;		/* justification (LEFT or RIGHT) */
#endif
{
    int day;
    char *fmt = just == LEFT ? "| %-16s" : "|%16s ";

    printf(LEFTMARGIN);
    for (day = 0; day < 7; day++)
	printf(fmt, month->dates[week][day]);
    printf("|\n");
}


/*
 * Print the divider separating 23/30 and/or 24/31 as needed
 */

void
#ifdef __STDC__
divider_line(char last_row[7][3])
#else
divider_line(last_row)
    char last_row[7][3];	/* row containing any trailing 30 and/or 31 */
#endif
{
    int day;

    printf(LEFTMARGIN);
    for (day = 0; day < 7; day++)
	printf(last_row[day][0] ? "|_________________" : "|                 ");
    printf("|\n");
}


/*
 * Print least-significant 6 bits of n (0 = ' '; 1 = c)
 */

void
#ifdef __STDC__
decode(int n, int c)
#else
decode(n, c)
    int n;
    char c;
#endif
{
    int msk;

    for (msk = 1 << 5; msk; msk >>= 1)
	printf("%c", n & msk ? c : ' ');
}


/*
 * print one line of string 'str' in large (5x9) characters
 */

void
#ifdef __STDC__
header_line(char *str, int line, char c)
#else
header_line(str, line, c)
    char *str;		/* string to print	*/
    int line;		/* line (0 - 8)		*/
    char c;		/* output character	*/
#endif
{
    /*
     * 5x7 representations of A-Z, 0-9; 5x9 representations of a-z
     */

    static char uppers[26][7] = {
	{14, 17, 17, 31, 17, 17, 17},	{30, 17, 17, 30, 17, 17, 30},	/* AB */
	{14, 17, 16, 16, 16, 17, 14},	{30, 17, 17, 17, 17, 17, 30},	/* CD */
	{31, 16, 16, 30, 16, 16, 31},	{31, 16, 16, 30, 16, 16, 16},	/* EF */
	{14, 17, 16, 23, 17, 17, 14},	{17, 17, 17, 31, 17, 17, 17},	/* GH */
	{31,  4,  4,  4,  4,  4, 31},	{ 1,  1,  1,  1,  1, 17, 14},	/* IJ */
	{17, 18, 20, 24, 20, 18, 17},	{16, 16, 16, 16, 16, 16, 31},	/* KL */
	{17, 27, 21, 21, 17, 17, 17},	{17, 17, 25, 21, 19, 17, 17},	/* MN */
	{14, 17, 17, 17, 17, 17, 14},	{30, 17, 17, 30, 16, 16, 16},	/* OP */
	{14, 17, 17, 17, 21, 18, 13},	{30, 17, 17, 30, 20, 18, 17},	/* QR */
	{14, 17, 16, 14,  1, 17, 14},	{31,  4,  4,  4,  4,  4,  4},	/* ST */
	{17, 17, 17, 17, 17, 17, 14},	{17, 17, 17, 17, 17, 10,  4},	/* UV */
	{17, 17, 17, 21, 21, 21, 10},	{17, 17, 10,  4, 10, 17, 17},	/* WX */
	{17, 17, 17, 14,  4,  4,  4},	{31,  1,  2,  4,  8, 16, 31}	/* YZ */
    };

    static char lowers[26][9] = {
	{ 0,  0, 14,  1, 15, 17, 15,  0,  0},	/* a */
	{16, 16, 30, 17, 17, 17, 30,  0,  0},	/* b */
	{ 0,  0, 15, 16, 16, 16, 15,  0,  0},	/* c */
	{ 1,  1, 15, 17, 17, 17, 15,  0,  0},	/* d */
	{ 0,  0, 14, 17, 31, 16, 14,  0,  0},	/* e */
	{ 7,  8, 30,  8,  8,  8,  8,  0,  0},	/* f */
	{ 0,  0, 14, 17, 17, 17, 15,  1, 14},	/* g */
	{16, 16, 30, 17, 17, 17, 17,  0,  0},	/* h */
	{ 4,  0, 12,  4,  4,  4, 31,  0,  0},	/* i */
	{ 1,  0,  3,  1,  1,  1,  1, 17, 14},	/* j */
	{16, 16, 17, 18, 28, 18, 17,  0,  0},	/* k */
	{12,  4,  4,  4,  4,  4, 31,  0,  0},	/* l */
	{ 0,  0, 30, 21, 21, 21, 21,  0,  0},	/* m */
	{ 0,  0, 30, 17, 17, 17, 17,  0,  0},	/* n */
	{ 0,  0, 14, 17, 17, 17, 14,  0,  0},	/* o */
	{ 0,  0, 30, 17, 17, 17, 30, 16, 16},	/* p */
	{ 0,  0, 15, 17, 17, 17, 15,  1,  1},	/* q */
	{ 0,  0, 30, 17, 16, 16, 16,  0,  0},	/* r */
	{ 0,  0, 15, 16, 14,  1, 30,  0,  0},	/* s */
	{ 8,  8, 30,  8,  8,  9,  6,  0,  0},	/* t */
	{ 0,  0, 17, 17, 17, 17, 15,  0,  0},	/* u */
	{ 0,  0, 17, 17, 17, 10,  4,  0,  0},	/* v */
	{ 0,  0, 17, 21, 21, 21, 10,  0,  0},	/* w */
	{ 0,  0, 17, 10,  4, 10, 17,  0,  0},	/* x */
	{ 0,  0, 17, 17, 17, 17, 15,  1, 14},	/* y */
	{ 0,  0, 31,  2,  4,  8, 31,  0,  0}	/* z */
    };

    static char digits[10][7] = {
	{14, 17, 17, 17, 17, 17, 14},	{ 2,  6, 10,  2,  2,  2, 31},	/* 01 */
	{14, 17,  2,  4,  8, 16, 31},	{14, 17,  1, 14,  1, 17, 14},	/* 23 */
	{ 2,  6, 10, 31,  2,  2,  2},	{31, 16, 16, 30,  1, 17, 14},	/* 45 */
	{14, 17, 16, 30, 17, 17, 14},	{31,  1,  2,  4,  8, 16, 16},	/* 67 */
	{14, 17, 17, 14, 17, 17, 14},	{14, 17, 17, 15,  1, 17, 14}	/* 89 */
    };

    char ch;

    /* convert each character of str to dot-matrix representation for line */

    for (; (ch = *str); str++) {
	if (isupper(ch))
	    decode(line < 7 ? uppers[ch - 'A'][line] : 0, c);
	else if (islower(ch))
	    decode(line < 9 ? lowers[ch - 'a'][line] : 0, c);
	else if (isdigit(ch))
	    decode(line < 7 ? digits[ch - '0'][line] : 0, c);
	else
	    decode(0, c);
    }
}
