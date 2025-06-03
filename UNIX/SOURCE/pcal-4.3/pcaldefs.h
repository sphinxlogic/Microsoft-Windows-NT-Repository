/*
 * pcaldefs.h - definitions for Pcal program
 *
 * Revision history:
 *
 *	4.3	AWR	12/05/91	Revise moonfile name templates for
 *					consistency with current standards
 *					for substituting year in strings
 *
 *			12/03/91	Add support for -s flag
 *
 *			10/17/91	Add support for -Z flag and subflags;
 *					removed obsolete PUTSTR macro
 *
 *	4.2	AWR	10/08/91	Add support for -[kK] flags; renamed
 *					START_DAY as START_BOX for clarity
 *
 *			10/03/91	Add various definitions relating to
 *					"note/<n>"
 *
 *			09/30/91	Add MAX_IF_NESTING (related to "elif";
 *					cf. read_datefile() in readfile.c)
 *
 *	4.11	AWR	08/20/91	Add "nearest" keyword (as per Andy
 *					Fyfe)
 *
 *	4.1	AWR	08/16/91	Add support for -G flag
 *
 *	4.0	AWR	03/01/91	Add STDLIB macro for systems which
 *					support <stdlib.h>
 *
 *		AWR	02/22/91	add definitions for MS-DOS support (as
 *					per Floyd Miller)
 *
 *		AWR	02/19/91	revise ORD_XXX to support negative
 *					ordinals
 *
 *					add definitions for Amiga support (as
 *					per Bill Hogsett)
 *
 *		AWR	02/06/91	additional defs for expression
 *					processing
 *
 *		AWR	02/04/91	support "year" as additional month
 *					name; use negative numbers for
 *					special ordinal codes (reserve
 *					positive numbers for ordinals)
 *
 *			01/28/91	miscellaneous new constants/macros
 *
 *			01/07/91	add FEB_29_OK (cf. enter_day_info())
 *
 *	3.0	AWR	12/10/90	support "weekday", "workday",
 *					"holiday", et. al.
 *
 *		AWR	11/13/90	extracted from pcal.c; added scale and
 *					translation support (-x, -y, -X, -Y);
 *					added -j, -J flags
 *
 */

/*
 * System dependencies:
 */

#ifdef VMS		/* VMS oddities isolated here */

#include <ssdef.h>	/* required for trnlog() */
#include <descrip.h>

#define HOME_DIR	"SYS$LOGIN"
#define DATEFILE	"calendar.dat"
#define MOONFILE	"moon%y.dat"	/* '%y' replaced with year */
#define DEFAULT_OUTFILE	"calendar.ps"
#define START_PATH	'['
#define END_PATH	']'

#define EXIT_SUCCESS	1		/* VMS exit() parameters */
#define EXIT_FAILURE	3

#else
#ifdef AMIGA		/* more oddities for Amiga */

#include <string.h>

#define PROTOS				/* compiler accepts prototypes */
#define STDLIB				/* system has <stdlib.h> */
#define HOME_DIR	"RAM:"
#define DATEFILE	"s:calendar.dat"
#define MOONFILE	"s:.moon%y"	/* '%y' replaced with year */
#define DEFAULT_OUTFILE	"RAM:calendar.ps"
#define START_PATH	'/'
#define END_PATH	'/'

#else
#ifdef DOS		/* even more oddities for MS-DOS */

#define DATEFILE	"pcal.dat"
#define MOONFILE	"moon%y.dat"	/* '%y' replaced with year */
#define START_PATH	'\\'
#define END_PATH	'\\'
#define HOME_DIR	"HOME"
#define ALT_DATEFILE	"calendar"	/* for backward compatibility */

#else			/* neither VMS, Amiga, nor MS-DOS - assume Un*x */

#define UNIXX		/* to distinguish Un*x from others */

#define HOME_DIR	"HOME"
#define DATEFILE	".calendar"
#define ALT_DATEFILE	"calendar"	/* for backward compatibility */
#define MOONFILE	".moon%y"	/* '%y' replaced with year */
#define ALT_MOONFILE	"moon%y"	/* analogous to ALT_DATEFILE */
#define START_PATH	'/'
#define END_PATH	'/'

#endif
#endif
#endif

/* define OUTFILE to DEFAULT_OUTFILE if defined, otherwise to "" (stdout) */

#ifdef DEFAULT_OUTFILE
#define OUTFILE		DEFAULT_OUTFILE
#else
#define OUTFILE		""
#endif

/* PROTOS may be defined independently of __STDC__ for compilers which
 * support function prototypes but are not fully ANSI-compliant
 */

#if defined(__STDC__) && ! defined(PROTOS)
#define PROTOS	
#endif

/* STDLIB may be defined independently of __STDC__ for systems which
 * support <stdlib.h> but are not fully ANSI-compliant
 */

#if defined(__STDC__) && ! defined(STDLIB)
#define STDLIB			/* cf. {no}protos.h */
#endif

#ifdef STDLIB			/* include <stdlib.h> if known to exist */
#include <stdlib.h>
#endif

/* EXIT_SUCCESS and EXIT_FAILURE should be defined in <stdlib.h>
 * (or above if non-standard); define here if necessary
 */

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS	0
#endif
#ifndef EXIT_FAILURE
#define EXIT_FAILURE	1
#endif

/* definition of NULL (if needed) */
 
#ifndef NULL
#define NULL	0
#endif

/*
 * Environment variables (global symbol, logical name on VMS):
 */

#define PCAL_OPTS	"PCAL_OPTS"	/* command-line flags */
#define PCAL_DIR	"PCAL_DIR"	/* calendar file directory */
#define PATH_ENV_VAR	"PATH"		/* cf. find_executable() (pcalutil.c) */

/*
 * Function-like macros:
 */

#define FPR		(void)fprintf
#define PRT		(void)printf

#define PUTCHAR(_c) \
	PRT((_c) == ' ' || isalnum(_c) ? "%c" : "\\%03o" , (_c) & 0377)

#define IS_LEAP(y)	((y) % 4 == 0 && ((y) % 100 != 0 || (y) % 400 == 0))
#define LENGTH_OF(m, y) (month_len[(m)-1] + ((m) == FEB && IS_LEAP(y)))
#define YEAR_LEN(y)	(IS_LEAP(y) ? 366 : 365)
#define DAY_OF_YEAR(m, d, y) ((month_off[(m)-1] + ((m) > FEB && IS_LEAP(y))) + d)
#define OFFSET_OF(m, y) ((month_off[(m)-1] + ((m) > FEB && IS_LEAP(y))) % 7)
#define FIRST_OF(m, y)	calc_weekday(m, 1, y)
#define START_BOX(m, y)	((FIRST_OF(m, y) - first_day_of_week + 7) % 7)

#define PREV_MONTH(m, y) ((m) == JAN ? DEC : (m) - 1)
#define PREV_YEAR(m, y)  ((m) == JAN ? (y) - 1 : (y))
#define NEXT_MONTH(m, y) ((m) == DEC ? JAN : (m) + 1)
#define NEXT_YEAR(m, y)  ((m) == DEC ? (y) + 1 : (y))

#define INIT_COLORS	memcpy(day_color, default_color, sizeof(day_color))

#define P_LASTCHAR(p)	((p) && *(p) ? (p) + strlen(p) - 1 : NULL)
#define LASTCHAR(p)	(p)[strlen(p) - 1]

#define IS_NUMERIC(p)	((p)[strspn((p), DIGITS)] == '\0')
#define IS_WILD(w)	((w) >= WILD_FIRST && (w) <= WILD_LAST)

#define MAKE_DATE(dt, m, d, y) \
	if (1) { (dt).mm = m; (dt).dd = d; (dt).yy = y; } else

#define ERR(errmsg) \
	FPR(stderr, E_ILL_LINE, progname, errmsg, filename, line);

#define DEBUG(f)	((debug_flags & f) != 0)

#ifdef __STDC__
#define TOLOWER(c)	tolower(c)
#else
#define TOLOWER(c)	(isupper(c) ? tolower(c) : (c))
#endif

#ifndef isodigit			/* rare */
#define isodigit(c)	((c) >= '0' && (c) <= '7')
#endif
#ifndef isxdigit			/* ANSI standard */
#define isxdigit(c) \
	(isdigit(c) || ((c) >= 'a' && (c) <= 'f') || ((c) >= 'A' && (c) <= 'F'))
#endif

/* debug subflag codes (must be distinct) - cf. pcallang.h */

#define DEBUG_DATES	(1 << 1)
#define DEBUG_MOON	(1 << 2)
#define DEBUG_PATHS	(1 << 3)
#define DEBUG_OPTS	(1 << 4)
#define DEBUG_PP	(1 << 5)
#define DEBUG_TEXT	(1 << 6)

/* preprocessor token codes - cf. get_token(), pcallang.h */

#define PP_DEFINE	 0
#define PP_ELIF		 1
#define PP_ELSE		 2
#define PP_ENDIF	 3
#define PP_IFDEF	 4
#define PP_IFNDEF	 5
#define PP_INCLUDE	 6
#define PP_UNDEF	 7
#define PP_OTHER	-1	/* not pp token */

/* ordinal number codes - cf. get_ordinal(), pcallang.h */

#define ORD_NEGNUM	-1	/* negative ordinal (-2nd == next to last) */
#define ORD_POSNUM	 1	/* positive ordinal */
#define ORD_ODD		 2	/* special codes for "odd" and "even" */
#define ORD_EVEN	 3
#define ORD_ALL		 4	/* special code for "all" used as ordinal */
#define ORD_OTHER	 0	/* not ordinal token */

/* moon phase codes - cf. pcallang.h and moonphas.c; these must take the
 * values 0 (NM) .. 3 (3Q) since they are used in phase calculations
 */

#define MOON_NM		 0	/* new moon */
#define MOON_1Q		 1	/* first quarter */
#define MOON_FM	 	 2	/* full moon */
#define MOON_3Q		 3	/* last quarter */
#define MOON_OTHER	-1	/* unrecognizable */

/* date type codes - cf. date_type(), get_keywd(), and pcallang.h */

#define DT_ALL		 0	/* "all" keyword" */
#define DT_NOTE		 1	/* "note" keyword */
#define DT_OPT		 2	/* "opt" keyword */
#define DT_YEAR		 3	/* "year" keyword */
#define DT_MONTH	 4	/* name of month */
#define DT_DATE		 5	/* date of form dd/mm{/yy} or mm/dd{/yy} */
#define DT_EURDATE	 6	/* European date of form dd <month> */
#define DT_ORDINAL	 7	/* ordinal (first, 1st, ... last) */
#define DT_WEEKDAY	 8	/* weekday name */
#define DT_OTHER	-1	/* unrecognizable first token */

/* preposition token codes - cf. get_prep(), pcallang.h */

#define PR_BEFORE	 0
#define PR_ON_BEFORE	 1
#define PR_AFTER	 2
#define PR_ON_AFTER	 3
#define PR_NEAREST	 4
#define PR_OTHER	-1	/* not a preposition */


/*
 * Miscellaneous other constants:
 */

#define FALSE		0	/* pseudo-Booleans */
#define TRUE		1

#define MAX_FILE_NESTING 10	/* maximum nesting level for file inclusion */
#define MAX_IF_NESTING	20	/* maximum nesting level for if{n}def */

#define MAX_PP_SYMS	100	/* number of definable preprocessor symbols */
#define PP_SYM_UNDEF     -1	/* flag for undefined symbol */

#define EXPR_ERR	-1	/* expression parsing error */

#define MIN_YR		1900	/* significant years (calendar limits) */
#define MAX_YR		9999

#define CENTURY		1900	/* default century for two-digit years */

#define JAN		 1	/* significant months */
#define FEB		 2
#define DEC		12
#define NOT_MONTH	-1	/* not valid month */
#define ALL_MONTHS	 0	/* select all months */
#define ENTIRE_YEAR	13	/* select entire year as one entity */

#define SUN		 0	/* significant weekdays */
#define SAT		 6
#define NOT_WEEKDAY	-1	/* not valid weekday */

#define SCREENWIDTH	72	/* command-line message in usage() */

#define FEB_29_OK	 1	/* if != 0, ignore 2/29 of common year */

#define NEAREST_INCR	 1	/* if 1, disambiguate "nearest" as later    */
				/* date; if -1, to earlier (cf. readfile.c) */

/* wildcards - cf. days[] in pcallang.h and pdatefcn[] in pcal.c */

#define ANY_DAY		 7	/* special - matches any day */
#define ANY_WEEKDAY	 8	/* matches any weekday (including holidays) */
#define ANY_WORKDAY	 9	/* matches any weekday (excluding holidays) */
#define ANY_HOLIDAY	10	/* matches any holiday */
#define ANY_NONWEEKDAY	11	/* converses of above three */
#define ANY_NONWORKDAY	12
#define ANY_NONHOLIDAY	13
/* moon phase wildcards - must be in same order as MOON_* below */
#define ANY_NM		14	/* matches any new moon, etc. */
#define ANY_1Q		15
#define ANY_FM		16
#define ANY_3Q		17

#define WILD_FIRST	ANY_DAY
#define WILD_FIRST_WKD	ANY_DAY
#define WILD_LAST_WKD	ANY_NONHOLIDAY
#define WILD_FIRST_MOON ANY_NM 
#define WILD_LAST_MOON	ANY_3Q
#define WILD_LAST	ANY_3Q

#define DAY_TEXT	0	/* types of text in data structure */
#define HOLIDAY_TEXT	1
#define NOTE_TEXT	2

#define NOTE_DEFAULT	-1	/* default note box number (-1 = last box) */
#define FIRST_NOTE_BOX	 0	/* first box position for notes text */
#define LAST_NOTE_BOX	41	/* last position (w/o small calendars) */
#define NULL_NOTE_BOX	-1	/* negative number for nonexistent box */

/* number and range of dummy days for notes text */
#define NUM_NOTE_DAYS	(LAST_NOTE_BOX - 28 + 1)	
#define FIRST_NOTE_DAY	32
#define LAST_NOTE_DAY	(FIRST_NOTE_DAY + NUM_NOTE_DAYS - 1)

#define MAX_DATES	366	/* maximum "wildcard" dates */

#define MAXWORD		100	/* maximum words in date file line */
#define STRSIZ		200	/* size of misc. strings */
#define LINSIZ		512	/* size of source line buffer */

#define MAXARGS		3	/* numeric command-line args */

#define WHITESPACE	" \t"	/* token delimiters in date file */
#define DIGITS    	"0123456789"

/* passes where flags may be recognized (cf. get_args(), pcallang.h) */
#define P_CMD0	(1 << 1)	/* parsed in command line pre-pass (-Z only) */
#define P_ENV	(1 << 2)	/* parsed from environment variable */
#define P_CMD1	(1 << 3)	/* parsed in first command-line pass */
#define P_OPT	(1 << 4)	/* parsed on "opt" lines in date file */
#define P_CMD2	(1 << 5)	/* parsed in second command-line pass */

/*
 * Defaults for calendar layout:
 */

#define DAYFONT		"Times-Bold"	/* default font names */
#define TITLEFONT	"Times-Bold"
#define NOTESFONT	"Helvetica-Narrow/6"
#define NOTESFONTSIZE	6

#define SHADING		"0.8/0.9"	/* default shading (dates/fill boxes) */

#define LFOOT 		""              /* default foot strings */
#define CFOOT 		""
#define RFOOT 		""

#define LANDSCAPE  	90		/* degrees to rotate */
#define PORTRAIT    	 0
#define ROTATE	   	LANDSCAPE	/* default */

/* scaling/translation factors based on 8.5 x 11 inch (612 x 792 point) page */

#define XSVAL_L		"1"		/* scale/translate values (landscape) */
#define YSVAL_L		"1"
#define XTVAL_L		"46"		/* (792 - 700) / 2 = 46 */
#define YTVAL_L		"-120"
#define XSVAL_P		"0.773"		/* scale/translate values (portrait) */
#define YSVAL_P		"0.773"		/* 612/792 */
#define XTVAL_P		"46"
#define YTVAL_P		"492"		/* 612 - 120 = 492 */

#if (ROTATE == LANDSCAPE)		/* select appropriate defaults */
#define XSVAL		XSVAL_L
#define YSVAL		YSVAL_L
#define XTVAL		XTVAL_L
#define YTVAL		YTVAL_L
#else
#define XSVAL		XSVAL_P
#define YSVAL		YSVAL_P
#define XTVAL		XTVAL_P
#define YTVAL		YTVAL_P
#endif

#define USA_DATES	0		/* date styles */
#define EUR_DATES	1
#define DATE_STYLE	USA_DATES	/* default */

#define FIRST_DAY	SUN		/* first day of logical week */

#define BLACK		0		/* colors for dates */
#define GRAY		1
#define DAY_COLOR	{ GRAY, BLACK, BLACK, BLACK, BLACK, BLACK, GRAY }

#define NO_DATEFILE 	0		/* date file (if any) to use */
#define USER_DATEFILE	1
#define SYS_DATEFILE	2

/* position of small calendars */
#define SC_NONE		0		/* suppress small calendars */
#define SC_LAST		1		/* use last two boxes */
#define SC_FIRST	2		/* use first two boxes */
#define SC_SPLIT	3		/* split between first and last */
#define SMALL_CAL_POS	SC_LAST		/* default */

/* box numbers for small calendars - must conform to order defined above */
#define PREV_CAL_BOX	\
	{ NULL_NOTE_BOX, LAST_NOTE_BOX - 1, FIRST_NOTE_BOX, FIRST_NOTE_BOX }
#define NEXT_CAL_BOX	\
	{ NULL_NOTE_BOX, LAST_NOTE_BOX, FIRST_NOTE_BOX + 1, LAST_NOTE_BOX }

/* values below must be in sync with cond[] and gray[] in writefil.c */
#define NO_MOONS	0		/* no moon icons */
#define ALL_MOONS	1		/* moon icon on every day */
#define SOME_MOONS	2		/* icons on full, half, new moons */
#define DRAW_MOONS	NO_MOONS	/* default */

#define NO_JULIANS	0		/* no Julian dates */
#define ALL_JULIANS	1		/* Julian date + days left */
#define SOME_JULIANS	2		/* just Julian dates */
#define JULIAN_DATES	NO_JULIANS	/* default: none */

#define	GRAY_NUMS	0		/* print "gray" dates in gray */
#define OUTLINE_NUMS	1		/* print as unfilled outlines */
#define FILLED_NUMS	2		/* print as filled outlines */
#define	NUM_STYLE	GRAY_NUMS	/* default */

/*
 * Global typedef declarations for data structure (cf. readfile.c)
 */

/*
 * The internal data structure consists of a series of linked lists as 
 * pictured below (for clarity, only one instance of each node is shown):
 *
 *
 * head
 *  |
 *  |        year_info
 *  |    -----------------           month_info
 *   -->| year            |       -----------------            day_info
 *      | month_info*[0]  |----->| holidays        |       ----------------
 *      |   ...           |      | day_info*[0]    |----->| is_holiday     |
 *      | month_info*[11] |--->  |   ...           |      | text_string*   |--->
 *      | next year_info* |--    | day_info*[30]   |--->  | next day_info* |-- 
 *       -----------------   |   | day_info*[NOTE] |--->   ----------------   |
 *                           |   |   ...           |                          |
 *                           |    -----------------                           |
 *             ...        <--                                     ...      <--
 *
 * 
 * Each year_info node consists of the year, 12 pointers (Jan .. Dec) to
 * month_info nodes, and a pointer to the next year_info node in the chain.
 * (The year_info chain is maintained in ascending order by year.)
 *
 * Each month_info node consists of a bitmap of the holidays for that month, 
 * a pointer to the day_info chain for all 31 possible days of the month, and
 * N additional pointers to the day_info chains for the "Notes" pseudo-days. 
 * (A day_info chain is a linked list of all the text entries for a given
 * day, in the order in which they were encountered in the date file.)
 *
 * Each day_info node consists of a flag indicating whether the associated
 * text string is holiday or non-holiday text (any given day may have both),
 * a pointer to the text string itself, and a pointer to the next day_info 
 * node in the chain.
 *
 */

typedef struct d_i {
	int is_holiday;
	char *text;
	struct d_i *next;
	} day_info;

typedef struct m_i {
	unsigned long holidays;
	day_info *day[LAST_NOTE_DAY];	/* including extra days for notes */
	} month_info;

typedef struct y_i {
	int year;
	month_info *month[12];
	struct y_i *next;
	} year_info;

/*
 * Global typedef declarations for keyword descriptors (cf. pcallang.h)
 */

typedef struct {
	char *name;
	int   code;
	} KWD;

typedef struct {
	char *name;
	int   code;
	int   value;
	} KWD_O;		/* keyword plus ordinal value */

typedef struct {
	char *name;
	int   code;
#ifdef PROTOS
	int (*pfcn)(char *);
#else
	int (*pfcn)();
#endif
	} KWD_F;		/* keyword plus dispatch function */


/*
 * Global typedef declaration for date structure (cf. parse_ord())
 */

typedef struct {
	int mm;
	int dd;
	int yy;
} DATE;

/* 
 * Global typedef declaration for flag usage struct (cf. pcallang.h, get_args())
 */

typedef struct {
	char flag;		/* name of flag */
	char has_arg;		/* TRUE if flag takes (optional) arg */
	char passes;		/* "or" of P_xxx for relevant passes */
	} FLAG_USAGE;

/*
 * Global typedef declaration for debugging info struct (cf. pcallang.h)
 */

typedef struct {
	char flag;		/* name of flag */
	int  value;		/* value corresponding to flag (1 << n) */
	} DEBUG_INFO;

/*
 * Global typedef declarations for usage message (cf. pcallang.h, usage())
 */

typedef struct {
	char flag;		/* name of flag */
	char *meta;		/* metaname for argument (if any) */
	char *text;		/* associated text */
	char *def;		/* default value (if any) */
	} FLAG_MSG;

typedef struct {
	char *desc;		/* description of param */
	char *text;		/* associated text */
	} PARAM_MSG;

typedef char *DATE_MSG;		/* date file syntax message */

