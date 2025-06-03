/*
 * protos.h - ANSI-style function prototypes for Pcal sources
 *
 * Revision history:
 *
 *	4.3	AWR	12/06/91	added more routines
 *
 *	4.2	AWR	10/03/91	added various routines newly
 *					incorporated into writefil.c
 *					and pcalutil.c
 *
 *	4.02	AWR	06/07/91	added find_executable()
 *
 *	4.0	AWR	03/01/91	use <stdlib.h> where possible
 *
 *		AWR	02/19/91	adapted from Bill Hogsett's source
 *
 */


/*
 * Prototypes for functions defined in exprpars.c:
 */
int parse_expr(char *pbuf);


/*
 * Prototypes for functions defined in moonphas.c:
 */
double	calc_phase(int month, int day, int year);
double	find_phase(int month, int day, int year, int *pquarter);
char	*find_moonfile(int year);
int	read_moonfile(int year);


/*
 * Prototypes for functions defined in pcal.c:
 */
FILE	*alt_fopen(char *fullpath, char *name, char *pathlist[], char *access);
int	check_numargs(void);
char	*color_msg(void);
void	gen_shading(char *currstr, char *newstr);
int	get_args(char **argv, int curr_pass, char *where, int get_numargs);
FLAG_USAGE *get_flag(char flag);
void	init_misc(void);
int	main(int argc, char **argv);
void	set_color(char *day, int col);
void	set_debug_flag(char *flag);
void	usage(FILE *fp, int fullmsg);


/*
 * Prototypes for functions defined in pcalutil.c:
 */
char	*alloc(int size);
int	calc_day(int ord, int wkd, int mm);
int	calc_weekday(int mm, int dd, int yy);
int	calc_year_day(int ord, int wkd, DATE *pdate);
int	ci_strcmp(register char *s1, register char *s2);
int	ci_strncmp(register char *s1, register char *s2, int n);
void	copy_text(char *pbuf, char **ptext);
void	cvt_escape(char *obuf, char *ibuf);
char	*find_executable(char *prog);
int	getline(FILE *fp, char *buf, int *pline);
int	is_valid(register int m, register int d, register int y);
int	loadwords(char **words, char *buf);
char	*mk_filespec(char *filespec, char *path, char *name);
char	*mk_path(char *path, char *filespec);
void	normalize(DATE *pd);
int	note_box(int mm, int dd, int yy);
int	note_day(int mm, int n, int yy);
int	split_date(char *pstr, int *pn1, int *pn2, int *pn3);
char	*trnlog(char *logname);


/*
 * Prototypes for functions defined in readfile.c:
 */
void	cleanup(void);
void	clear_syms(void);
int	date_type(char *cp, int *pn, int *pv);
int	do_define(char *sym);
int	do_ifdef(char *expr);
int	do_ifndef(char *expr);
int	do_include(char *path, char *name);
int	do_undef(char *sym);
int	enter_day_info(int m, int d, int y, int text_type, char **pword);
int	find_sym(char *sym);
year_info *find_year(int year, int insert);
int	get_keywd(char *cp);
int	get_month(char *cp, int numeric_ok, int year_ok);
int	get_ordinal(char *cp, int *pval);
int	get_phase(char *cp);
int	get_prep(char *cp);
int	get_token(char *token);
int	get_weekday(char *cp, int wild_ok);
int	is_anyday(int mm, int dd, int yy);
int	is_firstq(int mm, int dd, int yy);
int	is_fullmoon(int mm, int dd, int yy);
int	is_holiday(int mm, int dd, int yy);
int	is_lastq(int mm, int dd, int yy);
int	is_newmoon(int mm, int dd, int yy);
int	is_weekday(int mm, int dd, int yy);
int	is_workday(int mm, int dd, int yy);
int	not_holiday(int mm, int dd, int yy);
int	not_weekday(int mm, int dd, int yy);
int	not_workday(int mm, int dd, int yy);
int	parse(char **pword, char *filename);
int	parse_date(char **pword, int *ptype, char ***pptext);
int	parse_ord(int ord, int val, char **pword);
int	parse_rel(int wkd, char **pword, int *ptype, char ***pptext);
void	read_datefile(FILE *fp, char *filename);


/*
 * Prototypes for functions defined in writefil.c:
 */
void	def_footstring(char *p, char *str);
char	*expand_fmt(char *buf, char *p);
void	find_daytext(int month, int year, int is_holiday);
void	find_holidays(int month, int year);
void	find_noteboxes(int month, int year);
void	print_dates(int month, int year);
void	print_julian_info(int month, int year);
void	print_month(int month, int year);
void	print_moon_info(int month, int year);
void	print_pstext(char *p);
void	print_text(char *p);
char	*print_word(char *p);
void	write_calfile(int month, int year, int nmonths);
void	write_psfile(int month, int year, int nmonths);


/*
 * Prototypes for miscellaneous library routines (if not already included
 * via <stdlib.h> - cf. pcaldefs.h):
 */
#ifndef STDLIB
extern int	atoi(char *);
extern char	*calloc(unsigned int, unsigned int);
extern char	*getenv(char *);
#endif
