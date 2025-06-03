/*
 * readfile.c - Pcal routines concerned with reading and parsing the datefile
 *
 * Contents:
 *
 *		cleanup
 *		clear_syms
 *		date_type
 *		do_define
 *		do_ifdef
 *		do_ifndef
 *		do_include
 *		do_undef
 *		enter_day_info
 *		find_sym
 *		find_year
 *		get_keywd
 *		get_month
 *		get_ordinal
 *		get_phase
 *		get_prep
 *		get_token
 *		get_weekday
 *		is_anyday
 *		is_fquarter
 *		is_fullmoon
 *		is_holiday
 *		is_lquarter
 *		is_newmoon
 *		is_weekday
 *		is_workday
 *		not_holiday
 *		not_weekday
 *		not_workday
 *		parse
 *		parse_date
 *		parse_ord
 *		parse_rel
 *		read_datefile
 *
 * Revision history:
 *
 *	4.3	AWR	10/25/91	Support moon phase wildcards and
 *					-Z flag (debug information)
 *
 *	4.2	AWR	10/03/91	Support "note/<n>" (user-selected
 *					notes box) as per Geoff Kuenning
 *
 *			09/30/91	Support "elif" in datefile
 *
 *	4.11	AWR	08/20/91	Support "nearest" keyword as per
 *					Andy Fyfe
 *
 *	4.0	AWR	02/19/91	Support negative ordinals
 *
 *		AWR	02/06/91	Support expressions in "if{n}def"
 *
 *		AWR	02/04/91	Support "even" and "odd" ordinals
 *					and ordinals > 5th; support "year"
 *
 *		AWR	01/15/91	Extracted from pcal.c
 *
 */

/*
 * Standard headers:
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

/*
 * Pcal-specific definitions:
 */

#include "pcaldefs.h"
#include "pcalglob.h"
#include "pcallang.h"

/*
 * Macros:
 */

/* status codes returned by parse(), enter_day_info() */
#define PARSE_OK	0	/* successful date parse */
#define PARSE_INVDATE	1	/* nonexistent date */
#define PARSE_INVLINE	2	/* syntax error */
#define PARSE_NOMATCH	3	/* no match for wildcard */

/* codes for states in read_datefile() */
#define PROCESSING	0	/* currently processing datefile lines */
#define AWAITING_TRUE	1	/* awaiting first TRUE branch in "if{n}def" */
#define SKIP_TO_ENDIF	2	/* finished processing first TRUE branch */

/* append date to list; terminate list */
#define ADD_DATE(_m, _d, _y)	if (1) { \
				if (DEBUG(DEBUG_DATES)) \
					FPR(stderr, "matched %d/%d/%d\n", \
						_m, _d, _y); \
				pdate->mm = _m, pdate->dd = _d, pdate++->yy = _y; \
				} else

#define TERM_DATES		pdate->mm = pdate->dd = pdate->yy = 0

/*
 * Globals:
 */

static DATE dates[MAX_DATES+1];		/* array of date structures */
static char *pp_sym[MAX_PP_SYMS];	/* preprocessor defined symbols */


/*
 * read_datefile - read and parse date file, handling preprocessor lines
 *
 * This is the main routine of this module.  It calls getline() to read each
 * non-null line (stripped of leading blanks and trailing comments), loadwords()
 * to "tokenize" it, and get_token() to classify it as a preprocessor directive
 * or "other".  A switch{} statement takes the appropriate action for each
 * token type; "other" lines are further classified by parse() (q.v.) which
 * calls parse_date() (q.v.) to parse date entries and enter them in the data
 * structure (as described in pcaldefs.h).
 *
 */
#ifdef PROTOS
void read_datefile(FILE *fp,
		   char *filename)
#else
void read_datefile(fp, filename)
	FILE *fp;		/* file pointer (assumed open) */
	char *filename;		/* file name (for error messages) */
#endif
{
	static int file_level = 0;
	int if_level = 0;
	int line = 0;

	int pptype, extra, ntokens, save_year, expr;
	int (*pfcn)();
	char *ptok;
	char **pword;
	char msg[STRSIZ], incpath[STRSIZ];

	/* stack for processing nested "if{n}defs" - required for "elif" */
	struct {
		int state;	/* PROCESSING, AWAITING_TRUE, SKIP_TO_ENDIF */
		int else_ok;	/* is "elif" or "else" legal at this point? */
	} if_state[MAX_IF_NESTING+1];

	if (fp == NULL)				/* whoops, no date file */
		return;

	/* note that there is no functional limit on file nesting; this is
	 * mostly to catch infinite loops (e.g., a includes b, b includes a)
	 */
	if (++file_level > MAX_FILE_NESTING) {
		ERR(E_FILE_NESTING);
		exit(EXIT_FAILURE);
	}

	save_year = curr_year;			/* save default year */

	if_state[0].state = PROCESSING;		/* set up initial state */
	if_state[0].else_ok = FALSE;

	/* read lines until EOF */

	while (getline(fp, lbuf, &line)) {

		if (DEBUG(DEBUG_PP)) {
			FPR(stderr, "%s (%d)", filename, line);
			if (if_state[if_level].state == PROCESSING)
				FPR(stderr, ": '%s'", lbuf);
			FPR(stderr, "\n");
		}

		ntokens = loadwords(words, lbuf); /* split line into tokens */
		pword = words;			/* point to the first */

		/* get token type and pointers to function and name */

		pptype = get_token(*pword++);
		pfcn = pp_info[pptype].pfcn;
		ptok = pp_info[pptype].name;

		switch (pptype) {

		case PP_DEFINE:
		case PP_UNDEF:
			if (if_state[if_level].state == PROCESSING)
				(void) (*pfcn)(*pword);
			extra = ntokens > 2;
			break;

		case PP_ELIF:
			if (!if_state[if_level].else_ok) {
				ERR(E_ELIF_ERR);
				break;
			}

			/* if a true expression has just been processed, disable
			 * processing and skip to endif; if no true expression
			 * has been found yet and the current expression is
			 * true, enable processing
			 */
			switch (if_state[if_level].state) {
			case PROCESSING:
				if_state[if_level].state = SKIP_TO_ENDIF;
				break;
			case AWAITING_TRUE:
				copy_text(lbuf, pword);	/* reconstruct string */
				if ((expr = (*pfcn)(lbuf)) == EXPR_ERR) {
					ERR(E_EXPR_SYNTAX);
					expr = FALSE;
				}
				if (expr)
					if_state[if_level].state = PROCESSING;
				break;
			}

			extra = FALSE;
			break;

		case PP_ELSE:
			if (!if_state[if_level].else_ok) {
				ERR(E_ELSE_ERR);
				break;
			}

			/* if a true condition has just been processed, disable
			 * processing and skip to endif; if no true condition
			 * has been found yet, enable processing
			 */
			switch (if_state[if_level].state) {
			case PROCESSING:
				if_state[if_level].state = SKIP_TO_ENDIF;
				break;
			case AWAITING_TRUE:
				if_state[if_level].state = PROCESSING;
				break;
			}

			/* subsequent "elif" or "else" forbidden */
			if_state[if_level].else_ok = FALSE;
			extra = ntokens > 1;
			break;

		case PP_ENDIF:
			if (if_level < 1) {
				ERR(E_END_ERR);
				break;
			}
			if_level--;
			extra = ntokens > 1;
			break;

		case PP_IFDEF:
		case PP_IFNDEF:
			/* "if{n}def"s nested too deeply? */
			if (++if_level > MAX_IF_NESTING) {
				ERR(E_IF_NESTING);
				exit(EXIT_FAILURE);
				break;
			}

			/* if processing enabled at outer level, evaluate
			 * expression and enable/disable processing for
			 * following clause; if not, skip to matching endif
			 */
			if (if_state[if_level-1].state == PROCESSING) {
				copy_text(lbuf, pword);	/* reconstruct string */
				if ((expr = (*pfcn)(lbuf)) == EXPR_ERR) {
					ERR(E_EXPR_SYNTAX);
					expr = FALSE;
				}
				if_state[if_level].state = expr ? PROCESSING :
								  AWAITING_TRUE;
			} else
				if_state[if_level].state = SKIP_TO_ENDIF;

			if_state[if_level].else_ok = TRUE;
			extra = FALSE;
			break;

		case PP_INCLUDE:
			if (if_state[if_level].state == PROCESSING)
				do_include(mk_path(incpath, filename), *pword);
			extra = ntokens > 2;
			break;

		case PP_OTHER:	/* none of the above - parse as date */
			if (if_state[if_level].state == PROCESSING) {
				switch (parse(words, filename)) {
				case PARSE_INVDATE:
					ERR(E_INV_DATE);
					break;

				case PARSE_INVLINE:
					ERR(E_INV_LINE);
					break;

				case PARSE_NOMATCH:
					ERR(E_NO_MATCH);
					break;
				}
			}
			extra = FALSE;
			break;

		} /* end switch */

		if (extra) {		/* extraneous data? */
			sprintf(msg, E_GARBAGE, ptok);
			ERR(msg);
		}

	} /* end while */

	if (if_level > 0)
		FPR(stderr, E_UNT_IFDEF, progname, filename);

	file_level--;
	curr_year = save_year;		/* restore default year */
}


/*
 * Routines to free allocated data (symbol table and data structure) 
 */


/*
 * clear_syms - clear and deallocate the symbol table
 */
#ifdef PROTOS
void clear_syms(void)
#else
void clear_syms()
#endif
{
	int i;

	for (i = 0; i < MAX_PP_SYMS; i++)
		if (pp_sym[i]) {
			free(pp_sym[i]);
			pp_sym[i] = NULL;
		}
}


/*
 * cleanup - free all allocated data
 */
#ifdef PROTOS
void cleanup(void)
#else
void cleanup()
#endif
{
	int i, j;
	year_info *py, *pny;
	month_info *pm;
	day_info *pd, *pnd;

	for (py = head; py; py = pny) {		/* main data structure */
		pny = py->next;
		for (i = 0; i < 12; i++) {
			if ((pm = py->month[i]) == NULL)
				continue;
			for (j = 0; j < LAST_NOTE_DAY; j++)
				for (pd = pm->day[j]; pd; pd = pnd) {
					pnd = pd->next;
					free(pd->text);
					free(pd);
				}
			free(pm);
		}
	free(py);
	}

	clear_syms();				/* symbol table */

}


/*
 * Preprocessor token and symbol table routines
 */


/*
 * find_sym - look up symbol; return symbol table index if found, PP_SYM_UNDEF
 * if not found
 */
#ifdef PROTOS
int find_sym(char *sym)
#else
int find_sym(sym)
	char *sym;
#endif
{
	int i;

	if (!sym)
		return PP_SYM_UNDEF;

	for (i = 0; i < MAX_PP_SYMS; i++)
		if (pp_sym[i] && ci_strcmp(pp_sym[i], sym) == 0)
			return i;

	return PP_SYM_UNDEF;
}


/*
 * do_ifdef - return TRUE if 'expr' is true; FALSE if not; EXPR_ERR if invalid
 */
#ifdef PROTOS
int do_ifdef(char *expr)
#else
int do_ifdef(expr)
	char *expr;
#endif
{
	return parse_expr(expr);
}


/*
 * do_ifndef - return FALSE if 'expr' is true; TRUE if not; EXPR_ERR if invalid
 */
#ifdef PROTOS
int do_ifndef(char *expr)
#else
int do_ifndef(expr)
	char *expr;
#endif
{
	int val;

	return (val = parse_expr(expr)) == EXPR_ERR ? EXPR_ERR : ! val;
}


/*
 * do_define - enter 'sym' into symbol table; if 'sym' NULL, clear symbol table.
 * Always returns 0 (for compatibility with other dispatch functions).
 */
#ifdef PROTOS
int do_define(char *sym)
#else
int do_define(sym)
	char *sym;
#endif
{
	int i;

	if (! sym) {		/* null argument - clear all definitions */
		clear_syms();
		return 0;
	}

	if (do_ifdef(sym))	/* already defined? */
		return 0;

	for (i = 0; i < MAX_PP_SYMS; i++)	/* find room for it */
		if (! pp_sym[i]) {
			strcpy(pp_sym[i] = alloc(strlen(sym)+1), sym);
			return 0;
		}

	FPR(stderr, E_SYMFULL, progname, sym);
	return 0;
}


/*
 * do_undef - undefine 'sym' and free its space; no error if not defined.
 * Always return 0 (for compatibility with other dispatch fcns).
 */
#ifdef PROTOS
int do_undef(char *sym)
#else
int do_undef(sym)
	char *sym;
#endif
{
	int i;

	if (! sym) 
		return 0;

	if ((i = find_sym(sym)) != PP_SYM_UNDEF) {
		free(pp_sym[i]);
		pp_sym[i] = NULL;
	}

	return 0;
}


/*
 * do_include - include specified file (optionally in "" or <>); always
 * returns 0 (for compatibility with related functions returning int)
 */
#ifdef PROTOS
int do_include(char *path,
	       char *name)
#else
int do_include(path, name)
	char *path;		/* path to file */
	char *name;		/* file name */
#endif
{
	FILE *fp;
	char *p, incfile[STRSIZ], tmpnam[STRSIZ];

	if (! name)		/* whoops, no date file */
		return 0;

	/* copy name, stripping "" or <> */
	strcpy(tmpnam, name + (*name == '"' || *name == '<'));
	if ((p = P_LASTCHAR(tmpnam)) && *p == '"' || *p == '>')
		*p = '\0';

	/* special hack - replace %y with last two digits of curr_year */
	while ((p = strchr(tmpnam, '%')) != NULL && *(p+1) == 'y') {
		*p++ = (curr_year / 10) % 10 + '0';
		*p = curr_year % 10 + '0';
	}

	if ((fp = fopen(mk_filespec(incfile, path, tmpnam), "r")) == NULL) {
		FPR(stderr, E_FOPEN_ERR, progname, incfile);
		exit(EXIT_FAILURE);
	}

	read_datefile(fp, incfile);	/* recursive call */
	fclose(fp);

	return 0;
}



/*
 * Dispatch functions for wildcard matching
 */


/*
 * is_anyday - dummy function which always returns TRUE
 */
#ifdef PROTOS
int is_anyday(int mm,
	      int dd,
	      int yy)
#else
int is_anyday(mm, dd, yy)
	int mm;
	int dd;
	int yy;
#endif
{
	return TRUE;
}


/*
 * is_weekday - determine whether or not mm/dd/yy is a weekday (i.e., the
 * day of the week normally prints in black)
 */
#ifdef PROTOS
int is_weekday(int mm,
	       int dd,
	       int yy)
#else
int is_weekday(mm, dd, yy)
	int mm;
	int dd;
	int yy;
#endif
{
	return day_color[calc_weekday(mm, dd, yy)] == BLACK;
}


/*
 * is_workday - determine whether or not mm/dd/yy is a workday (i.e., the
 * day of the week normally prints in black and the date is not a holiday)
 */
#ifdef PROTOS
int is_workday(int mm,
	       int dd,
	       int yy)
#else
int is_workday(mm, dd, yy)
	int mm;
	int dd;
	int yy;
#endif
{
	return is_weekday(mm, dd, yy) && ! is_holiday(mm, dd, yy);
}


/*
 * is_holiday - determine whether or not mm/dd/yy is a holiday
 */
#ifdef PROTOS
int is_holiday(int mm,
	       int dd,
	       int yy)
#else
int is_holiday(mm, dd, yy)
	int mm;
	int dd;
	int yy;
#endif
{
	year_info *py;
	month_info *pm;

	pm = (py = find_year(yy, FALSE)) ? py->month[mm-1] : NULL;
	return pm ? (pm->holidays & (1L << (dd-1))) != 0 : FALSE;
}


/*
 * not_XXXXX - converses of is_XXXXX above
 */
#ifdef PROTOS
int not_weekday(int mm,
		int dd,
		int yy)
#else
int not_weekday(mm, dd, yy)
	int mm;
	int dd;
	int yy;
#endif
{
	return !is_weekday(mm, dd, yy);
}


#ifdef PROTOS
int not_workday(int mm,
		int dd,
		int yy)
#else
int not_workday(mm, dd, yy)
	int mm;
	int dd;
	int yy;
#endif
{
	return !is_workday(mm, dd, yy);
}


#ifdef PROTOS
int not_holiday(int mm,
		int dd,
		int yy)
#else
int not_holiday(mm, dd, yy)
	int mm;
	int dd;
	int yy;
#endif
{
	return !is_holiday(mm, dd, yy);
}


/*
 * is_newmoon - determine whether or not mm/dd/yy is the date of a new moon
 */
#ifdef PROTOS
int is_newmoon(int mm,
	       int dd,
	       int yy)
#else
int is_newmoon(mm, dd, yy)
	int mm;
	int dd;
	int yy;
#endif
{
	int quarter;

	(void) find_phase(mm, dd, yy, &quarter);
	return quarter == MOON_NM;
}


/*
 * is_firstq - determine whether or not mm/dd/yy is the date of a first quarter
 */
#ifdef PROTOS
int is_firstq(int mm,
	      int dd,
	      int yy)
#else
int is_firstq(mm, dd, yy)
	int mm;
	int dd;
	int yy;
#endif
{
	int quarter;

	(void) find_phase(mm, dd, yy, &quarter);
	return quarter == MOON_1Q;
}


/*
 * is_fullmoon - determine whether or not mm/dd/yy is the date of a full moon
 */
#ifdef PROTOS
int is_fullmoon(int mm,
	        int dd,
	        int yy)
#else
int is_fullmoon(mm, dd, yy)
	int mm;
	int dd;
	int yy;
#endif
{
	int quarter;

	(void) find_phase(mm, dd, yy, &quarter);
	return quarter == MOON_FM;
}


/*
 * is_lastq - determine whether or not mm/dd/yy is the date of a last quarter
 */
#ifdef PROTOS
int is_lastq(int mm,
	     int dd,
	     int yy)
#else
int is_lastq(mm, dd, yy)
	int mm;
	int dd;
	int yy;
#endif
{
	int quarter;

	(void) find_phase(mm, dd, yy, &quarter);
	return quarter == MOON_3Q;
}



/*
 * Keyword classification routines
 */

/*
 * get_month - convert alpha (or, optionally, numeric) string to month; return 
 * 1..12 if valid, NOT_MONTH if not, ALL_MONTHS if "all", ENTIRE_YEAR if "year"
 */
#ifdef PROTOS
int get_month(char *cp,
	      int numeric_ok,
	      int year_ok)
#else
int get_month(cp, numeric_ok, year_ok)
	char *cp;		/* string to convert */
	int numeric_ok;		/* accept numeric string ? */
	int year_ok;		/* accept "year"? */
#endif
{
	int mm;

	if (! cp)
		return NOT_MONTH;

	if (get_keywd(cp) == DT_ALL)
		return ALL_MONTHS;

	if (year_ok && get_keywd(cp) == DT_YEAR)
		return ENTIRE_YEAR;

	if (numeric_ok && isdigit(*cp))
		mm = atoi(cp);
	else
		for (mm = JAN;
		     mm <= DEC && ci_strncmp(cp, months[mm-1], MIN_MONTH_LEN);
		     mm++)
			;

	return mm >= JAN && mm <= DEC ? mm : NOT_MONTH;
}


/*
 * get_weekday - look up string in weekday list; return 0-6 if valid,
 * NOT_WEEKDAY if not.  If wild_ok flag is set, accept "day", "weekday",
 * "workday", "holiday", or moon phase and return appropriate value.
 */
#ifdef PROTOS
int get_weekday(char *cp,
		int wild_ok)
#else
int get_weekday(cp, wild_ok)
	char *cp;
	int wild_ok;
#endif
{
	int w;

	if (!cp)
		return NOT_WEEKDAY;

	if (wild_ok) {		/* try wildcards first */
		for (w = WILD_FIRST_WKD; w <= WILD_LAST_WKD; w++)
			if (ci_strncmp(cp, days[w], strlen(days[w])) == 0)
				return w;
		if ((w = get_phase(cp)) != MOON_OTHER)
			return w + WILD_FIRST_MOON;
	}

	for (w = SUN; w <= SAT; w++)
		if (ci_strncmp(cp, days[w], MIN_DAY_LEN) == 0)
			return w;

	return NOT_WEEKDAY;
}


/*
 * get_keywd - look up string in misc. keyword list; return keyword code
 * if valid, DT_OTHER if not
 */
#ifdef PROTOS
int get_keywd(char *cp)
#else
int get_keywd(cp)
	char *cp;
#endif
{
	KWD *k;

	if (!cp)
		return DT_OTHER;

	for (k = keywds;
	     k->name && ci_strncmp(cp, k->name, strlen(k->name));
	     k++)
		;

	return k->code;
}


/*
 * get_ordinal - look up string in ordinal list; return ordinal code (and
 * fill in ordinal value) if valid, return ORD_OTHER if not
 */
#ifdef PROTOS
int get_ordinal(char *cp,
		int *pval)
#else
int get_ordinal(cp, pval)
	char *cp;
	int *pval;
#endif
{
	KWD_O *o;
	int val;
	char **psuf;

	if (!cp)
		return ORD_OTHER;

	if (isdigit(*cp) || *cp == '-') {		/* numeric? */
		if ((val = atoi(cp)) == 0)
			return ORD_OTHER;

		if (*cp == '-')				/* skip over number */
			cp++;
		cp += strspn(cp, DIGITS);

		for (psuf = ord_suffix; *psuf; psuf++)	/* find suffix */
			if (ci_strcmp(cp, *psuf) == 0) {
				*pval = val;
				return val < 0 ? ORD_NEGNUM : ORD_POSNUM;
			}

		return ORD_OTHER;
	}

	/* look for word in ordinals list */

	for (o = ordinals; o->name && ci_strncmp(cp, o->name, MIN_ORD_LEN); o++)
		;

	*pval = o->value;
	return o->code;
}


/*
 * get_phase - convert moon phase string to appropriate value
 */
#ifdef PROTOS
int get_phase(char *cp)
#else
int get_phase(cp)
	char *cp;
#endif
{
	KWD *p;

	if (!cp)
		return MOON_OTHER;

	for (p = phases; p->name && ci_strcmp(cp, p->name); p++)
		;

	return p->code;
}


/*
 * get_prep - look up string in preposition list; return preposition code if 
 * valid, PR_OTHER if not
 */
#ifdef PROTOS
int get_prep(char *cp)
#else
int get_prep(cp)
	char *cp;
#endif
{
	KWD *p;

	if (!cp)
		return PR_OTHER;

	for (p = preps; p->name && ci_strncmp(cp, p->name, MIN_PREP_LEN); p++)
		;

	return p->code;
}


/*
 * get_token - look up 'token' in list of preprocessor tokens; return its
 * index if found, PP_OTHER if not
 */
#ifdef PROTOS
int get_token(char *token)
#else
int get_token(token)
	char *token;
#endif
{
	KWD_F *p;

	for (p = pp_info;
             p->name && ci_strncmp(token, p->name, MIN_PPTOK_LEN);
	     p++)
		;

	return p->code;
}


/*
 * date_type - examine token and return date type code; if DT_MONTH, DT_ORDINAL,
 * or DT_WEEKDAY, fill in appropriate code (and value if DT_ORDINAL)
 */
#ifdef PROTOS
int date_type(char *cp,
	      int *pn,
	      int *pv)
#else
int date_type(cp, pn, pv)
	char *cp;	/* pointer to start of token  */
	int *pn;	/* token type code (returned) */
	int *pv;	/* ordinal value (returned)   */
#endif
{
	int n, v;

	/* look for weekdays first, to catch wildcards "1q", "3q", etc. */
	if ((n = get_weekday(cp, TRUE)) != NOT_WEEKDAY)	/* weekday name? */
		return (*pn = n, DT_WEEKDAY);

	if ((n = get_ordinal(cp, &v)) != ORD_OTHER)	/* ordinal? */
		return (*pn = n, *pv = v, DT_ORDINAL);

	if (isdigit(*cp))				/* other digit? */
		return IS_NUMERIC(cp) ? DT_EURDATE : DT_DATE;

	/* "all" can be either a keyword or a month wildcard - look for
	   the former usage first */

	if ((n = get_keywd(cp)) != DT_OTHER)
		return n;

	if ((n = get_month(cp, FALSE, FALSE)) != NOT_MONTH)  /* month name? */
		return (*pn = n, DT_MONTH);

	return DT_OTHER;		/* unrecognized keyword - give up */

}



/*
 * Routines for entering data in the data structure (described in pcaldefs.h)
 */


/*
 * find_year - find record in year list; optionally create if not present 
 */
#ifdef PROTOS
year_info *find_year(int year,
		     int insert)
#else
year_info *find_year(year, insert)	/* find record in year list */
	int year;
	int insert;			/* insert if missing */
#endif
{
	year_info *pyear, *plast, *p;

	for (plast = NULL, pyear = head;		/* search linked list */
	     pyear && pyear->year < year;
	     plast = pyear, pyear = pyear->next)
		;

	if (pyear && pyear->year == year)		/* found - return it */
		return pyear;

	if (insert) {		/* not found - insert it if requested */
		p = (year_info *) alloc((int) sizeof(year_info));	/* create new record */
		p->year = year;

		p->next = pyear;				/* link it in */
		return *(plast ? &plast->next : &head) = p;
	}
	else
		return NULL;
}


/*
 * enter_day_info - enter text for specified day; avoid entering duplicates.
 * Returns PARSE_INVDATE if date invalid, PARSE_OK if OK; if symbol FEB_29_OK
 * is non-zero (cf. pcaldefs.h), will silently ignore Feb 29 of common year.
 */
#ifdef PROTOS
int enter_day_info(int m,
		   int d,
		   int y,
		   int text_type,
		   char **pword)
#else
int enter_day_info(m, d, y, text_type, pword)	/* fill in information for given day */
	int m, d, y;
	int text_type;
	char **pword;
#endif
{
	static year_info *pyear;
	static int prev_year = 0;
	month_info *pmonth;
	day_info *pday, *plast;
	int is_holiday = text_type == HOLIDAY_TEXT;
	char text[LINSIZ];

	if (! is_valid(m, d >= FIRST_NOTE_DAY && text_type == NOTE_TEXT ? 1 : d, y))
		return (m == FEB && d == 29 && FEB_29_OK) ? PARSE_OK : PARSE_INVDATE;

	if (y != prev_year)		/* avoid unnecessary year lookup */
		pyear = find_year(y, 1);

	--m, --d;			/* adjust for use as subscripts */

	if ((pmonth = pyear->month[m]) == NULL)	/* find/create month record */
		pyear->month[m] = pmonth = (month_info *) alloc((int) sizeof(month_info));

	if (is_holiday)
		pmonth->holidays |= (1L << d);

	/* insert text for day at end of list (preserving the order of entry
	 * for multiple lines on same day); eliminate those differing only
	 * in spacing and capitalization from existing entries
         */

	copy_text(text, pword);	/* consolidate text from lbuf into text */

	if (DEBUG(DEBUG_DATES)) {
		char *p;
		FPR(stderr, "%02d/%02d/%d%c '", m+1, d+1, y,
			is_holiday ? '*' : ' ');
		for (p = text; *p; p++)
			FPR(stderr, isprint(*p) ? "%c" : "\\%03o", *p & 0377);
		FPR(stderr, "'\n");
	}

	if (*text) {
		for (plast = NULL, pday = pmonth->day[d];
		     pday;
		     plast = pday, pday = pday->next)
			if (ci_strcmp(pday->text, text) == 0) {
				pday->is_holiday |= is_holiday;
				return PARSE_OK;
			}

		/* unique - add to end of list */

		pday = (day_info *) alloc(sizeof(day_info));
		pday->is_holiday = is_holiday;
		strcpy(pday->text = (char *) alloc(strlen(text)+1), text);
		pday->next = NULL;
		*(plast ? &plast->next : &pmonth->day[d]) = pday;
	}

	return PARSE_OK;
}



/*
 * Date parsing routines:
 */


/*
 * parse_ord - parse an ordinal date spec (e.g. "first Monday in September",
 * "every Sunday in October", "last workday in all"); return PARSE_OK if line
 * syntax valid, PARSE_INVLINE if not.  Write all matching dates (if any) to
 * global array dates[]; terminate date list with null entry.
 */
#ifdef PROTOS
int parse_ord(int ord,
	      int val,
	      char **pword)
#else
int parse_ord(ord, val, pword)
	int ord;		/* valid ordinal code - from get_ordinal() */
	int val;		/* ordinal value - also from get_ordinal() */
	char **pword;		/* pointer to word after ordinal */
#endif
{
	int wkd, mon, mm, dd, len, (*pfcn)(), doit;
	int val_first, val_last, val_incr, mon_first, mon_last;
	DATE *pdate, date;

	if ((wkd = get_weekday(*pword, TRUE)) == NOT_WEEKDAY ||	/* weekday */
	    *++pword == NULL ||					/* any word */
	    (mon = get_month(*++pword, FALSE, TRUE)) == NOT_MONTH) /* month */
		return PARSE_INVLINE;

	/* set up loop boundaries for month loop */
	mon_first = mon == ALL_MONTHS || mon == ENTIRE_YEAR ? JAN : mon;
	mon_last  = mon == ALL_MONTHS || mon == ENTIRE_YEAR ? DEC : mon;

	pdate = dates;			/* start of date array */

	/* special case of "all|odd|even <wildcard> in <month>|all|year" */

	if ((ord == ORD_ALL || ord == ORD_EVEN || ord == ORD_ODD) &&
	    IS_WILD(wkd)) {
		pfcn = pdatefcn[wkd - WILD_FIRST];
		doit = ord != ORD_EVEN;
		for (mm = mon_first; mm <= mon_last; mm++) {
			len = LENGTH_OF(mm, curr_year);
			if (mon != ENTIRE_YEAR)
				doit = ord != ORD_EVEN;
			for (dd = 1; dd <= len; dd++)
				if ((*pfcn)(mm, dd, curr_year)) {
					if (doit)
						ADD_DATE(mm, dd, curr_year);
					if (ord != ORD_ALL)
						doit = ! doit;
				}
		}
	}

	/* special case of "odd|even <weekday> in year" */

	else if ((ord == ORD_EVEN || ord == ORD_ODD) && mon == ENTIRE_YEAR) {
		date.mm = JAN;			/* starting date */
		date.dd = calc_day(ord == ORD_EVEN ? 2 : 1, wkd, JAN);
		date.yy = curr_year;
		do {				/* alternates throughout year */
			ADD_DATE(date.mm, date.dd, date.yy);
			date.dd += 14;
			normalize(&date);
		} while (date.yy == curr_year);
	}

	/* special case of "<ordinal>|last <weekday>|<wildcard> in year" */

	else if ((ord == ORD_NEGNUM || ord == ORD_POSNUM) &&
	         mon == ENTIRE_YEAR) {
		if (calc_year_day(val, wkd, &date))
			ADD_DATE(date.mm, date.dd, date.yy);
	}

	/* all other combinations of ordinal and day */

	else {
		/* set up loop boundaries for "wildcard" ordinals */

		val_first = ord == ORD_ALL || ord == ORD_ODD ? 1 :
			    ord == ORD_EVEN ? 2 : val;
		val_last  = ord == ORD_ALL || ord == ORD_ODD ? 5 :
			    ord == ORD_EVEN ? 4 : val;
		val_incr  = ord == ORD_ODD || ord == ORD_EVEN ? 2 : 1;

		for (mm = mon_first; mm <= mon_last; mm++)
			for (val = val_first; val <= val_last; val += val_incr)
				if ((dd = calc_day(val, wkd, mm)) != 0)
					ADD_DATE(mm, dd, curr_year);
	}

	TERM_DATES;		/* terminate array with null entry */
	return PARSE_OK;
}


/*
 * parse_rel - parse a relative date spec (e.g. "Friday after fourth Thursday
 * in November", "Saturday after first Friday in all"; return PARSE_OK if
 * line syntax valid, PARSE_INVLINE if not.  Transform all dates that match
 * the base date to the appropriate day, month, and year.
 *
 * This calls parse_date() recursively in order to handle cases such as
 * "Friday after Tuesday before last day in all".
 */
#ifdef PROTOS
int parse_rel(int wkd,
	      char **pword,
	      int *ptype,
	      char ***pptext)
#else
int parse_rel(wkd, pword, ptype, pptext)
	int wkd;		/* valid weekday code - from get_weekday() */
	char **pword;		/* pointer to word after weekday */
	int *ptype;		/* return text type (holiday/non-holiday) */
	char ***pptext;		/* return pointer to first word of text */
#endif
{
	int prep, rtn, base_wkd, incr, (*pfcn)();
	DATE *pd;

	/* we have the weekday - now look for the preposition */
	if ((prep = get_prep(*pword++)) == PR_OTHER)
		return PARSE_INVLINE;

	/* get the base date */
	if ((rtn = parse_date(pword, ptype, pptext)) != PARSE_OK)
		return rtn;

	/* transform date array in place - note that the relative date may
	   not be in the same month or even year */

	if (IS_WILD(wkd)) {		/* wildcard for weekday name? */
		pfcn = pdatefcn[wkd - WILD_FIRST];
		incr = prep == PR_BEFORE || prep == PR_ON_BEFORE ? -1 : 1;

		for (pd = dates; pd->mm; pd++) {
			/* search for nearest matching date */

			if (prep == PR_BEFORE || prep == PR_AFTER) {
				pd->dd += incr;
				normalize(pd);
			}
			/* If NEAREST_INCR (cf. pcaldefs.h) is 1, Pcal will
			 * disambiguate "nearest" in favor of the later date;
			 * if -1, in favor of the earlier.  "incr" will take
			 * the values 1, -2, 3, -4, ... or -1, 2, -3, 4 ...
			 * respectively.
			 */
			if (prep == PR_NEAREST)
				incr = NEAREST_INCR;
			while (!(*pfcn)(pd->mm, pd->dd, pd->yy)) {
				pd->dd += incr;
				normalize(pd);
				if (prep == PR_NEAREST)
					incr -= (incr > 0) ? (2 * incr + 1) :
							     (2 * incr - 1);
			}
		}

	} else  {			/* explicit weekday name */
		for (pd = dates; pd->mm; pd++) {
			/* calculate nearest matching date */

			base_wkd = calc_weekday(pd->mm, pd->dd, pd->yy);

			if (prep == PR_BEFORE ||
			    (prep == PR_ON_BEFORE && wkd != base_wkd))
				pd->dd -= 7 - (wkd - base_wkd + 7) % 7;

			if (prep == PR_AFTER ||
			    (prep == PR_ON_AFTER && wkd != base_wkd))
				pd->dd += (wkd - base_wkd + 6) % 7 + 1;

			normalize(pd);	/* adjust for month/year crossing */
		}
	}

	return PARSE_OK;
}


/*
 * parse_date - parse a date specification in any of its myriad forms; upon
 * return, array dates[] will contain a list of all the dates that matched,
 * terminated by a null entry.  Also fill in the date type (holiday/non-
 * holiday) code and the pointer to the first word of text.
 */
#ifdef PROTOS
int parse_date(char **pword,
	       int *ptype,
	       char ***pptext)
#else
int parse_date(pword, ptype, pptext)
	char **pword;		/* first word to parse */
	int *ptype;		/* return date type (holiday/non-holiday) */
	char ***pptext;		/* return pointer to first word of text */
#endif
{
	int mm, dd, yy;
	int token, n, v, ord, val, wkd, rtn;
	DATE *pdate;
	char *cp;

	pdate = dates;

	switch (token = date_type(*pword, &n, &v)) {

	case DT_MONTH:		/* <month> dd */
		if (date_style != USA_DATES)
			return PARSE_INVLINE;

		if ((cp = *++pword) == NULL)
			return PARSE_INVLINE;

		ADD_DATE(n, atoi(cp), curr_year);
		TERM_DATES;

		break;

	case DT_DATE:		/* mm/dd{/yy} | dd/mm{/yy} */
		n = split_date(*pword,
			       date_style == USA_DATES ? &mm : &dd,
			       date_style == USA_DATES ? &dd : &mm,
			       &yy);

		if (n > 2) {			/* year present? */
			if (yy < 100)
				yy += CENTURY;
			curr_year = yy;		/* reset current year */
		}

		ADD_DATE(mm, dd, curr_year);
		TERM_DATES;

		break;

	case DT_EURDATE:	/* dd [ <month> | "all" ] */
		if (date_style != EUR_DATES)
			return PARSE_INVLINE;

		dd = atoi(*pword);

		if (get_keywd(*++pword) == DT_ALL) {
			for (mm = JAN; mm <= DEC; mm++)		/* wildcard */
				ADD_DATE(mm, dd, curr_year);
		}
		else {						/* one month */
			if ((mm = get_month(*pword, FALSE, FALSE)) == NOT_MONTH)
				return PARSE_INVLINE;

			ADD_DATE(mm, dd, curr_year);
		}

		TERM_DATES;
		break;

	case DT_ALL:		/* "all" <weekday> "in" [ <month> | "all" ] */
				/* or "all" <day>" */

		if ((cp = *(pword+1)) && (*(cp += strspn(cp, DIGITS)) == '\0' ||
		    *cp == '*')) {
			dd = atoi(*++pword);		/* "all" <day> */
			for (mm = JAN; mm <= DEC; mm++)
				ADD_DATE(mm, dd, curr_year);
			TERM_DATES;
			break;		/* leave switch */
		}

		n = ORD_ALL;	/* "all" <weekday> ... */
		v = 0;
	     /* fall through */

	case DT_ORDINAL:	/* <ordinal> <weekday> in [ <month> | "all" ] */
		ord = n;
		val = v;
		if ((rtn = parse_ord(ord, val, pword + 1)) != PARSE_OK)
			return rtn;

		pword += 3;		/* last word of date */
		break;

	case DT_WEEKDAY:	/* <weekday> <prep> <date> */
		wkd = n;
		/* parse_rel() calls parse_date() recursively */
		return parse_rel(wkd, ++pword, ptype, pptext);
		break;

	default:
		return PARSE_INVLINE;
		break;
	}

	/* at this point, pword points to the last component of the date;
	 * fill in type code and pointer to following word (start of text)
	 */
	*ptype = LASTCHAR(*pword) == '*' ? HOLIDAY_TEXT : DAY_TEXT;
	*pptext = ++pword;

	return PARSE_OK;
}


/*
 * parse - parse non-preprocessor lines in date file
 *
 * This routine parses "year", "opt", "note", and date entries in the date
 * file.  It calls parse_date() to parse date entries (and enter the date(s)
 * matched in global array "dates"), and then calls enter_day_info() to
 * enter each date found (and its associated text) in the date file.
 *
 * N.B.: "inc" and other cpp-like lines are handled in read_datefile().
 *
 */
#ifdef PROTOS
int parse(char **pword,
	  char *filename)
#else
int parse(pword, filename)
	char **pword;		/* pointer to first word to parse */
	char *filename;		/* name of file (for error messages) */
#endif
{
	register char *cp;
	char **ptext;
	int mm, yy;
	int text_type, n, v, rtn, match, valid;
	int token;
	DATE *pd;

	/*
	 * Get first field and call date_type() to decode it
         */
	cp = *pword;

	switch (token = date_type(cp, &n, &v)) {

	case DT_YEAR:
		if ((cp = *++pword) != NULL && (yy = atoi(cp)) > 0) {
			if (yy < 100)
				yy += CENTURY;
			curr_year = yy;
			return PARSE_OK;
		}
		return PARSE_INVLINE;	/* year missing or non-numeric */
		break;

	case DT_OPT:
	 	if (!get_args(pword, P_OPT, filename, FALSE)) {
			usage(stderr, FALSE);
			exit(EXIT_FAILURE);
		}
		return PARSE_OK;
		break;

	case DT_NOTE:
		/* look for optional "/<n>" following keyword */
		n = (cp = strrchr(cp, '/')) ? atoi(++cp) : 0;

		if ((mm = get_month(*++pword, TRUE, TRUE)) == NOT_MONTH)
			return PARSE_INVLINE;

		if (mm == ALL_MONTHS || mm == ENTIRE_YEAR) {   /* "note all"? */
			valid = FALSE;	/* is at least one note box valid? */
			for (mm = JAN; mm <= DEC; mm++)
				valid |= enter_day_info(mm,
						note_day(mm, n, curr_year),
						curr_year, NOTE_TEXT,
						pword+1) == PARSE_OK;
			return valid ? PARSE_OK : PARSE_NOMATCH;
		}

		return enter_day_info(mm, note_day(mm, n, curr_year),
				curr_year, NOTE_TEXT, pword+1);
		break;

	case DT_OTHER:		/* unrecognized token */
		return PARSE_INVLINE;
		break;

	/* assume anything else is a date */

	default:
		if ((rtn = parse_date(pword, &text_type, &ptext)) == PARSE_OK) {
			match = FALSE;	/* is at least one date valid? */
			for (pd = dates; pd->mm; pd++)
				match |= enter_day_info(pd->mm, pd->dd, pd->yy,
					       text_type, ptext) == PARSE_OK;
			rtn = match ? PARSE_OK : PARSE_NOMATCH;
		}
		return rtn;
		break;

	}
}
