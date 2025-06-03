/*
 * pcalglob.h - definitions and declarations of globals used throughout Pcal
 *
 * Revision history:
 *
 *	4.3	AWR	12/05/91	Add global "progpath" (used when
 *					searching for moon file)
 *
 *			12/03/91	Added support for -s flag
 *
 *	4.2	AWR	10/08/91	Added support for -[kK] flags
 *
 *			10/02/91	Added support for -c, -N, -S flags
 *
 *	4.1	AWR	07/16/91	Added support for -G flag
 *
 *	4.0	AWR	01/28/91	Added support for -b and -w flags
 *
 *			01/15/91	Extracted from pcal.c
 *
 */

#ifdef PROTOS
#include "protos.h"
#else
#include "noprotos.h"
#endif

/*
 * Definitions and/or declarations of globals
 */

#ifdef MAIN_MODULE

/*
 * Misc. globals
 */

year_info *head = NULL;		/* head of internal data structure */
int curr_year;			/* current default year for date file entries */
char *words[MAXWORD];		/* maximum number of words per date file line */
char lbuf[LINSIZ];		/* date file source line buffer */
char progname[STRSIZ];		/* program name (for error messages) */
char progpath[STRSIZ];		/* directory where executable lives */
char version[20];		/* program version (for info messages) */

/* lengths and offsets of months in common year */
char month_len[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
short month_off[12] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

/* dispatch table for wildcard matching routines */
#ifdef PROTOS
int (*pdatefcn[])(int, int, int) = {
#else
int (*pdatefcn[])() = {
#endif
	is_anyday, is_weekday, is_workday, is_holiday, not_weekday,
	not_workday, not_holiday, is_newmoon, is_firstq, is_fullmoon,
	is_lastq };

/*
 * Default values for command-line options:
 */

char default_color[] = DAY_COLOR; 	/* -b, -g */
char day_color[7];

int datefile_type = SYS_DATEFILE;	/* -e, -f */
char datefile[STRSIZ] = "";

int rotate = ROTATE;			/* -l, -p */

int draw_moons = DRAW_MOONS;		/* -m, -M */

char dayfont[STRSIZ] = DAYFONT;		/* -d, -t, -n */
char titlefont[STRSIZ] = TITLEFONT;
char notesfont[STRSIZ] = NOTESFONT;

char shading[STRSIZ] = SHADING;		/* -s */

char lfoot[STRSIZ] = LFOOT;             /* -L, -C, -R */
char cfoot[STRSIZ] = CFOOT;
char rfoot[STRSIZ] = RFOOT;

char notes_hdr[STRSIZ];			/* -N (initialized in main()) */

int first_day_of_week = FIRST_DAY;	/* -F */

int date_style = DATE_STYLE;		/* -A, -E */

char outfile[STRSIZ] = OUTFILE;		/* -o */

char xsval[12] = XSVAL;			/* -x, -y, -X, -Y */
char ysval[12] = YSVAL;
char xtval[12] = XTVAL;
char ytval[12] = YTVAL;

int julian_dates = JULIAN_DATES;	/* -j */

int do_whole_year = FALSE;		/* -w */

int calendar_out = FALSE;		/* -c */

int blank_boxes = FALSE;		/* -B */

int small_cal_pos = SMALL_CAL_POS;	/* -k, -K, -S */
int prev_cal_box[4] = PREV_CAL_BOX;
int next_cal_box[4] = NEXT_CAL_BOX;

int num_style = NUM_STYLE;		/* -G, -O */

int debug_flags = 0;			/* -Z */

#else

/*
 * Misc. globals
 */

extern year_info *head;		/* head of internal data structure */
extern int curr_year;		/* current default year for date file entries */
extern char *words[];		/* maximum number of words per date file line */
extern char lbuf[];		/* date file source line buffer */
extern char progname[];		/* program name (for error messages) */
extern char progpath[];		/* directory where executable lives */
extern char version[];		/* program version (for info messages) */

/* lengths and offsets of months in common year */
extern char month_len[];
extern short month_off[];

/* dispatch functions and table for wildcard processing */
extern int (*pdatefcn[])(); 

/*
 * Default values for command-line options:
 */

extern char default_color[];	 	/* -b, -g */
extern char day_color[];

extern int datefile_type;		/* -e, -f */
extern char datefile[];

extern int rotate;			/* -l, -p */

extern int draw_moons;			/* -m, -M */

extern char dayfont[];			/* -d, -t, -n */
extern char titlefont[];
extern char notesfont[];

extern char shading[];			/* -s */

extern char lfoot[];			/* -L, -C, -R */
extern char cfoot[];
extern char rfoot[];

extern char notes_hdr[];		/* -N */

extern int first_day_of_week;		/* -F */

extern int date_style;			/* -A, -E */

extern char outfile[];			/* -o */

extern char xsval[];			/* -x, -y, -X, -Y */
extern char ysval[];
extern char xtval[];
extern char ytval[];

extern int julian_dates;		/* -j */

extern int do_whole_year;		/* -w */

extern int calendar_out;		/* -c */

extern int blank_boxes;			/* -B */

extern int small_cal_pos;		/* -k, -K, -S */
extern int prev_cal_box[];
extern int next_cal_box[];

extern int num_style;			/* -G, -O */

extern int debug_flags;			/* -Z */

#endif

