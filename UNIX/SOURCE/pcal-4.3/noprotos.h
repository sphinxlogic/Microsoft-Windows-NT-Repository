/*
 * noprotos.h - K&R-style function declarations for Pcal sources
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
 *		AWR	02/19/91	adapted from protos.h (q.v.)
 *
 */


/*
 * Declarations for functions defined in exprpars.c:
 */
int parse_expr();


/*
 * Declarations for functions defined in moonphas.c:
 */
double	calc_phase();
double	find_phase();
char	*find_moonfile();
int	read_moonfile();


/*
 * Declarations for functions defined in pcal.c:
 */
FILE	*alt_fopen();
int	check_numargs();
char	*color_msg();
void	gen_shading();
int	get_args();
FLAG_USAGE *get_flag();
void	init_misc();
int	main();
void	set_color();
void	set_debug_flag();
void	usage();


/*
 * Declarations for functions defined in pcalutil.c:
 */
char	*alloc();
int	calc_day();
int	calc_weekday();
int	calc_year_day();
int	ci_strcmp();
int	ci_strncmp();
void	copy_text();
void	cvt_escape();
char	*find_executable();
int	getline();
int	is_valid();
int	loadwords();
char	*mk_filespec();
char	*mk_path();
void	normalize();
int	note_box();
int	note_day();
int	split_date();
char	*trnlog();


/*
 * Declarations for functions defined in readfile.c:
 */
void	cleanup();
void	clear_syms();
int	date_type();
int	do_define();
int	do_ifdef();
int	do_ifndef();
int	do_include();
int	do_undef();
int	enter_day_info();
int	find_sym();
year_info *find_year();
int	get_keywd();
int	get_month();
int	get_ordinal();
int	get_phase();
int	get_prep();
int	get_token();
int	get_weekday();
int	is_anyday();
int	is_firstq();
int	is_fullmoon();
int	is_holiday();
int	is_lastq();
int	is_newmoon();
int	is_weekday();
int	is_workday();
int	not_holiday();
int	not_weekday();
int	not_workday();
int	parse();
int	parse_date();
int	parse_ord();
int	parse_rel();
void	read_datefile();


/*
 * Declarations for functions defined in writefil.c:
 */
void	def_footstring();
char	*expand_fmt();
void	find_daytext();
void	find_holidays();
void	find_noteboxes();
void	print_dates();
void	print_julian_info();
void	print_month();
void	print_moon_info();
void	print_pstext();
void	print_text();
char	*print_word();
void	write_calfile();
void	write_psfile();


/*
 * Prototypes for miscellaneous library routines (if not already included
 * via <stdlib.h> - cf. pcaldefs.h)
 */
#ifndef STDLIB
extern int	atoi();
extern char	*calloc();
extern char	*getenv();
#endif
