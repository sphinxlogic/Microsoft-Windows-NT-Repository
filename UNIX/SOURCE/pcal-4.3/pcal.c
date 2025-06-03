static char  VERSION_STRING[]	= "@(#)pcal v4.3 - generate Postscript calendars";
/*
 * pcal.c - generate PostScript file to print calendar for any month and year
 *
 * The original PostScript code to generate the calendars was written by
 * Patrick Wood (Copyright (c) 1987 by Patrick Wood of Pipeline Associates,
 * Inc.), and authorized for modification and redistribution.  The calendar
 * file inclusion code was originally written in "bs(1)" by Bill Vogel of
 * AT&T.  Patrick's original PostScript was modified and enhanced several
 * times by King Ables, Tom Tessin, and others whose names have regrettably 
 * been lost.  This C version was originally created by Ken Keirnan of Pacific
 * Bell; additional enhancements by Joseph P. Larson, Ed Hand, Andrew Rogers, 
 * Mark Kantrowitz, Joe Brownlee, Andy Fyfe, and Geoff Kuenning.  The moon
 * routines were originally written by Jef Poskanzer and Craig Leres, and
 * were incorporated into Pcal by Richard Dyson.
 *
 * Contents:
 *
 *		alt_fopen
 *		check_numargs
 *		color_msg
 *		gen_shading
 *		get_args
 *		get_flag
 *		init_misc
 *		main
 *		set_color
 *		set_debug_flag
 *		usage
 *
 * Revision history:
 *
 *	4.3	AWR	12/06/91	Attempted to simplify some of the
 *					mysteries surrounding command-line
 *					parsing; moved some processing from
 *					get_args() to new check_numargs()
 *
 *			12/05/91	Search for moon file in directory
 *					where Pcal lives (cf. moonphas.c)
 *
 *			12/03/91	Add -s flag to override default values
 *					for date/fill box shading
 *
 *			11/22/91	Use cvt_escape() (new; cf. pcalutil.c)
 *					to convert escape sequences in command
 *					line strings
 *
 *			11/18/91	Improve documentation; add init_misc()
 *					as catch-all for various initializations
 *
 *			10/25/91	Support moon phases as wildcards
 *
 *			10/17/91	Add -Z flag to generate debugging
 *					information; add a pre-pass through
 *					command line flags to detect -ZO
 *					prior to parsing PCAL_OPTS
 *
 *			10/15/91	Revise logic of date file search
 *
 *	4.2	AWR	10/08/91	Add -k and -K flags to control
 *					positioning of small calendars
 *
 *			10/03/91	Add "note{/<n>}" to select box for
 *					note text (as per Geoff Kuenning)
 *
 *					Add -S flag to suppress generation
 *					of the small calendars
 *
 *			10/02/91	Add -N flag to specify alternate
 *					heading for notes box
 *
 *					Allow user to specify alternate notes
 *					font size (-n <name>/<size>)
 *
 *			10/01/91	Add -u flag to generate version info
 *					and parameter usage message
 *
 *			09/30/91	Support "if" and "elif" in date file
 *
 *			09/19/91	Add -c flag to generate input file for
 *					Un*x "calendar" utility
 *
 *	4.11	AWR	08/20/91	Add support for "nearest" keyword
 *					(as per Andy Fyfe)
 *
 *					define "whole_year" when -w set
 *
 *		AWR	08/21/91	Support %u, %w, %D, %M format specs
 *					and optional number following %[+-]
 *					(cf. writefil.c)
 *
 *	4.1	AWR	08/16/91	Add -G flag to print "gray" dates as
 *					outlined, gray-filled characters
 *
 *					Fix potential bug in julday() (cf.
 *					moonphas.c)
 *
 *	4.02	AWR	07/02/91	Add -v flag to print version info only;
 *					call find_executable() to get true
 *					program pathname (cf. pcalutil.c);
 *					add format specifiers to text strings
 *					(cf. writefil.c)
 *
 *	4.01	AWR	03/19/91	Incorporate revised moonphas.c (q.v.)
 *
 *	4.0	AWR	02/24/91	Add alt_fopen() to search for file
 *					in alternate path; use to look for
 *					date file in same directory as
 *					Pcal executable (as per Floyd Miller)
 *
 *					Support negative ordinals (cf.
 *					readfile.c, pcalutil.c)
 *
 *					Support expressions in preprocessor
 *					"if{n}def" lines (cf. exprpars.c)
 *
 *					Support "even", "odd" ordinals (cf.
 *					readfile.c) and ordinals > 5th
 *
 *					Support -B (leave unused boxes blank)
 *					flag
 *
 *					Separated into moonphas.c, pcal.c,
 *					pcalutil.c, readfile.c, and writefil.c;
 *					added support for moon phase file
 *
 *					Support -w (whole year) flag; fix
 *					various bugs and nonportable constructs
 *
 *	3.0	AWR	12/10/90	Support concept of "weekday", "workday",
 *					and "holiday" (and converses)
 *
 *					Substantial revision of program logic:
 *					extracted pcaldefs.h and pcallang.h,
 *					moving all language dependencies (even
 *					flag names) to the latter.
 *
 *					add -I flag to reinitialize all
 *	 				flags to program defaults; -j and -J
 *					flags (print Julian dates); add -x,
 *				 	-y, -X, -Y flags (as per Ed Hand)
 *					for output scaling and translation
 *
 *					allow "wildcard" dates (e.g., "all
 *					Thursday{s} in Oct", "last Friday in
 *					all") and notes ("note all <text>);
 *					print full "help" message (including
 *					date file syntax)
 *
 *	2.6	AWR	10/15/90	parse floating dates (e.g. "first
 *					Monday in September") and relative
 *					floating dates (e.g., "Friday after
 *					fourth Thursday in November"); simplify
 *					logic of -F option; add -F to usage 
 *					message; replace COLOR_MSG() string
 *					with color_msg() routine; add -h
 *					(help message) and -A | -E (American |
 *					European date format) flags; renamed
 *					flag sets for clarity; more comments
 *
 *	2.5	JAB	10/04/90	added -F option
 *
 *	2.4	---	10/01/90	* no modifications *
 *
 *	2.3	JWZ/AWR	09/18/90	added moon routines
 *
 *	2.2	AWR	09/17/90	revise logic of parse(); new usage
 *					message
 *
 *		JAB/AWR	09/14/90	support "note" lines in date file
 *
 *	2.1	MK/AWR	08/27/90	support -L, -C, -R, -n options;
 *					print holiday text next to date
 *
 *		AWR	08/24/90	incorporate cpp-like functionality;
 *					add -D and -U options; save date file
 *					information in internal data structure;
 *					look for PCAL_OPTS and PCAL_DIR; look
 *					for ~/.calendar and ~/calendar
 *
 *	2.0	AWR	08/08/90	included revision history; replaced -r
 *					flag with -l and -p; replaced -s and -S
 *					flags with -b and -g; recognize flags
 *					set in date file; translate ( and ) in
 *					text to octal escape sequence; usage()
 *					message condensed to fit 24x80 screen
 *
 *	Parameters:
 *
 *		pcal [opts]		generate calendar for current month/year
 *					(current year if -w flag specified)
 *
 *		pcal [opts] yy		generate calendar for entire year yy
 *
 *		pcal [opts] mm yy	generate calendar for month mm
 *					(Jan = 1), year yy (19yy if yy < 100)
 *					(12 months starting with mm/yy if -w
 *					specified)
 *
 *		pcal [opts] mm yy n	as above, for n consecutive months (n
 *					rounded to next multiple of 12 if -w
 *					specified)
 *
 *	Output:
 *
 *		PostScript file to print calendars for all selected months.
 *
 *	Options:
 *
 *		-I		initialize all parameters to program defaults
 *
 *		-b <DAY>	print specified weekday in black
 *		-g <DAY>	print specified weekday in gray
 *				(default: print Saturdays and Sundays in gray)
 *
 *		-G		print "gray" dates as filled outlines
 *		-O		print "gray" dates as unfilled outlines
 *				(default: gray characters)
 *
 *		-s {<DATE>}{/<FILL>}
 *				specify alternate shading values for dates
 *				and fill boxes
 *				(default: dates = 0.8, fill boxes = 0.9)
 *
 *		-d <FONT>	specify alternate font for day names
 *				(default: Times-Bold)
 *
 *		-n <FONT>{/<SIZE>}
 *				specify alternate font/size for notes in boxes
 *				(default: Helvetica-Narrow/6)
 *
 *		-t <FONT>	specify alternate font for titles
 *				(default: Times-Bold)
 *
 *		-D <SYM>	define preprocessor symbol
 *		-U <SYM>	un-define preprocessor symbol
 *
 *		-e		generate empty calendar (ignore date file)
 *
 *		-f <FILE>	specify alternate date file (default:
 *				~/.calendar on Un*x, SYS$LOGIN:CALENDAR.DAT
 *				on VMS, s:calendar.dat on Amiga; if
 *				environment variable [logical name on VMS]
 *				PCAL_DIR exists, looks there instead; if
 *				not found in either place, looks in same
 *				directory as Pcal executable)
 *
 *		-o <FILE>	specify alternate output file (default:
 *				stdout on Un*x, CALENDAR.PS on VMS, 
 *				RAM:calendar.ps on Amiga)
 *
 *		-L <STRING>	specify left foot string   (default: "")
 *		-C <STRING>	specify center foot string (default: "")
 *		-R <STRING>	specify right foot string  (default: "")
 *
 *		-N <STRING>	specify notes box header (default: "Notes")
 *
 *		-l		generate landscape-mode calendars
 *		-p		generate portrait-mode calendars
 *				(default: landscape-mode)
 *
 *		-h		(command line only) write version information
 *				and full help message to stdout
 *		-u		(command line only) write version information
 *				and parameter usage message to stdout
 *		-v		(command line only) write version information
 *				alone to stdout
 *
 *		-m		draw a small moon icon on the days of the
 *				full, new, and half moons.
 *		-M		draw a small moon icon every day.  
 *				(default: no moons)
 *
 *		-F <DAY>	select alternate day to be displayed as the 
 *				first day of the week (default: Sunday)
 *
 *		-A		dates are in American format (e.g., 10/15/90,
 *				Oct 15) (default)
 *		-E		dates are in European format (e.g., 15.10.90,
 *				15 Oct)
 *
 *		-x <XSCALE>	These two options can be used to change
 *		-y <YSCALE>	the size of the calendar.
 *
 *		-X <XTRANS>	These two options can be used to relocate
 *		-Y <YTRANS>	the position of the calendar on the page.
 *
 *		-j		print Julian dates (day of year)
 *		-J		print Julian dates and days remaining
 *				(default: neither)
 *
 *		-w		print whole year (12 months) per page
 *
 *		-c		generate input for Un*x calendar(1) utility
 *
 *		-B		leave unused calendar boxes blank
 *
 *
 *	There are many ways to specify these options in addition to using the
 *	command line; this facilitates customization to the user's needs.
 *
 *	If the environment variable (global symbol on VMS) PCAL_OPTS is
 *	present, its value will be parsed as if it were a command line.
 *	Any options specified will override the program defaults.
 *
 *	All options but -[cefhuvDU] may be specified in the date file by
 *	including one or more lines of the form "opt <options>".  Any such
 *	options override any previous values set either as program defaults,
 *	via PCAL_OPTS, or in previous "opt" lines.
 *
 *	Options explicitly specified on the command line in turn override all
 *	of the above.
 *
 *	Any flag which normally takes an argument may also be specified without
 *	an argument; this resets the corresponding option to its default.  -D
 *	alone un-defines all symbols; -U alone has no effect.
 *
 *	Parameters and flags may be mixed on the command line.  In some cases
 *	(e.g., when a parameter follows a flag without its optional argument)
 *	this may lead to ambiguity; the dummy flag '-' (or '--') may be used
 *	to separate them, i.e. "pcal -t - 9 90".
 *
 *
 *	Date file syntax:
 *
 *	The following rules describe the syntax of date file entries:
 *
 *	  year <year>
 *
 *	  opt <options>
 *
 *	  note{/<number>} <month_spec> <text>
 *	  note{/<number>} <month> <text>
 *
 *	  if -A flag (American date formats) specified:
 *	    <month_name> <day>{*} {<text>}
 *	    <month><sep><day>{<sep><year>}{*} {<text>}
 *
 *	  if -E flag (European date formats) specified:
 *	    <day> <month_name>{*} {<text>}
 *	    <day><sep><month>{<sep><year>}{*} {<text>}
 *
 *	  <ordinal> <day_name> in <month_spec>{*} {<text>}
 *	  <day_name> <prep> <date_spec>
 *
 *	where
 *
 *	  {x}          means x is optional
 *
 *	  <date_spec> := any of the above date specs (not year, note, or opt)
 *	  <month_name> := first 3+ characters of name of month, or "all"
 *	  <month_spec> := <month_name>, or "year"
 *	  <day_name> := first 3+ characters of name of weekday, "day",
 *			"weekday", "workday", "holiday", "nonweekday",
 *			"nonworkday", "nonholiday", "new_moon",
 *			"first_quarter", "full_moon", or "last_quarter"
 *	  <ordinal> := ordinal number ("1st", "2nd", etc.), "first" .. "fifth",
 *			"last", "even", "odd", or "all"
 *	  <prep> := "before", "preceding", "after", "following", "nearest",
 *			"on_or_before", or "on_or_after"
 *	  <sep> := one or more non-numeric, non-space, non-'*' characters
 *	  <month>, <day>, <year> are the numeric forms
 *
 *	  <options> := any command-line option except -[cefhuvDU]
 *
 *	Comments start with '#' and run through end-of-line.
 *
 *	Holidays may be flagged by specifying '*' as the last character of
 *	the date field(s), e.g. "10/12* Columbus Day", "July 4* Independence
 *	Day", etc.  Any dates flagged as holidays will be printed in gray, and
 *	any associated text will appear adjacent to the date.
 *
 *	Note that the numeric date formats (mm/dd{/yy}, dd.mm{.yy}) support
 *	an optional year, which will become the subsequent default year.  The
 *	alphabetic date formats (month dd, dd month) do not support a year
 *	field; the "year yy" command is provided to reset the default year.
 *
 *	"Floating" days may be specified in the date file as "first Mon in 
 *	Sep", "last Mon in May", "4th Thu in Nov", etc.; any word may be
 *	used in place of "in".  "Relative floating" days (e.g. "Fri after 4th 
 *	Thu in Nov") are also accepted; they may span month/year bounds.
 *	Pcal also accepts date specs such as "all Friday{s} in October", "last
 *	Thursday in all", etc., and produces the expected results; "each" and
 *	"every" are accepted as synonyms for "all".  Negative ordinals are
 *	allowed; "-2nd" means "next to last".
 *
 *	The words "day", "weekday", "workday", and "holiday" may be used as
 *	wildcards: "day" matches any day, "weekday" matches any day normally
 *	printed in black, "workday" matches any day normally printed in black
 *	and not explicitly flagged as a holiday, and "holiday" matches any
 *	day explicitly flagged as a holiday.  "Nonweekday", "nonworkday",
 *	and "nonholiday" are also supported and have the obvious meanings.
 *	Moon phases may also appear as wildcards; "nm" is accepted as a
 *	synonym for "new_moon", "1q" and "fq" for "first_quarter", "fm" for
 *	"full_moon", and "3q", "lq", and "third_quarter" for "last_quarter". 
 *
 *	"Odd" and "even" do not refer to the actual date; instead, "odd"
 *	means "alternate, starting with the first"; "even" means "alternate,
 *	starting with the second".  Thus, "odd Fridays in March" refers to
 *	the first, third, and (if present) fifth Fridays in March - not to
 *	those Fridays falling on odd dates.
 *
 *	"All" refers to each individual month; "year" refers to the year
 *	as an entity.  Thus "odd Fridays in all" refers to the first/third/
 *	fifth Friday of each month, while "odd Fridays in year" refers to
 *	the first Friday of January and every other Friday thereafter.
 *
 *	Additional notes may be propagated to an empty calendar box by the
 *	inclusion of one or more lines of the form "note{/<number>} <month>
 *	<text>", where <month> may be numeric or alphabetic; "note{/<number>}
 *	all <text>" propagates <text> to each month in the current year.
 *	<number> is an optional positive or negative number specifying the
 *	empty box where the associated text is to be placed: if positive,
 *	Pcal counts forward from the first empty box; if negative, Pcal counts
 *	backward from the last empty box.  Thus, "note/1 ..." places the
 *	associated text in the first empty box, and "note/-3 ..." in the
 *	third-to-last; the default is -1 (last empty box).  (Note that if the
 *	-S option is used, it must be specified either on the command line or
 *	prior to any "note" lines in the date file.)
 *
 *	Pcal also allows format specifiers in the text (and foot strings -
 *	cf. the -L, -C, -R, and -N options); each will be replaced by its
 *	equivalent string as outlined in the table below.  (Most of these are
 *	derived from the strftime() function; %[louwMD0+-] are Pcal-specific.)
 *
 *		%a : abbreviated weekday
 *		%A : full weekday
 *		%b : abbreviated month name
 *		%B : full month name
 *		%d : day of month (1-31)
 *		%j : day of year (1-366)
 *		%l : days left in year (0-365)
 *		%m : month (1-12)
 *		%u : week number (1-54)
 *		%U : week number (0-53)
 *		%w : week number (1-54)
 *		%W : week number (0-53)
 *		%y : year w/o century (00-99)
 *		%Y : year w/century
 *		%% : '%' character
 *
 *		%o : print number as ordinal
 *		%0 : print number with leading zeroes
 *		%+ : use following month or year
 *		%- : use previous month or year
 *		%{+N}[DWMY] : adjust date by +N days/weeks/months/years
 *		%{-N}[DWMY] : adjust date by -N days/weeks/months/years
 *
 *	%u considers the week containing 1/1 as week 1 and the following
 *	logical Sunday (the first day of the week as printed; cf. the -F
 *	flag) as the start of week 2; %U considers the first logical Sunday
 *	as the first day of week 1.  %w and %W behave like %u and %U
 *	respectively, but use the first logical Monday instead.  (Note that
 *	%w has a different meaning to strftime().)
 *
 *	%o prints a number as an ordinal, with the appropriate suffix ("st",
 *	"nd", "rd", or "th" in English) appended; for example, "%od" prints
 *	the day of the month as "1st", "2nd", "3rd", etc.
 *
 *	Unlike strftime(), Pcal's default is to print numbers (except %y)
 *	without leading zeroes.  If leading zeroes are desired, the '0'
 *	prefix may be used; for example, "%0j" prints the day of year as
 *	001-365.
 *
 *	%+ and %- direct Pcal to substitute the following/previous month/year
 *	in the following [bBmyY] specifier; for example, "%+B" prints the
 *	name of the next month.
 *
 *	%{[+-]N}[DWMY] do not print anything, but instead adjust the
 *	working date by +-N days (D), weeks (W), months (M), or years (Y);
 *	subsequent format specifiers use the adjusted date instead of the
 *	current date.  For example, "%+1M %B %Y" adjusts the date forward by
 *	one month and then prints the resulting month and year ("January
 *	1991" in December, 1990); "%-2W %b %d" adjusts the date backward
 *	by two weeks and prints the resulting month and day ("Jul 26" on
 *	August 9).
 *
 *	Such date adjustments are normally cumulative; for example,
 *	"%+1Y%-1D" adjusts the date forward by one year and then backward
 *	by one day.  If %D or %M is specified alone (or if N is zero), Pcal
 *	restores the original date.  (Note that %M has a different meaning
 *	to strftime().)
 *
 *	The "Notes" box uses the first of the current month as the default
 *	date.  All foot strings use the first of the current month in single-
 *	month mode and the first of the starting month in whole-year mode.
 *
 *	Simple cpp-like functionality is provided.  The date file may include
 *	the following commands, which work like their cpp counterparts:
 *
 *		define <sym>
 *		undef <sym>
 *
 *		if{{n}def} <expr>
 *		   ...
 *		{ elif <expr>
 *		   ... }*
 *		{ else
 *		   ... }
 *		endif
 *
 *		include <file>
 *
 *	Note that these do not start with '#', which is reserved as a comment
 *	character.
 *
 *	<sym> is a symbol name consisting of a letter followed by zero or
 *	more letters, digits, or underscores ('_').  Symbol names are always
 *	treated in a case-insensitive manner.
 *
 *	<expr> is an expression consisting of symbol names joined by the logical
 *	operators (in order of precedence, high to low) '!' (unary negate), '&'
 *	(and), '^' (exclusive or), and '|' (inclusive or).  '&&' and '||' are
 *	accepted as synonyms for '&' and '|' respectively; the order of
 *	evaluation may be altered by the use of parentheses.  A symbol whose
 *	name is currently defined evaluates to TRUE; one whose name is not
 *	currently defined evaluates to FALSE.  Thus "ifdef A | B | C" is TRUE
 *	if any of the symbols A, B, and C is currently defined, and
 *	"ifdef A & B & C" is TRUE if all of them are.
 *
 *	"ifndef A | B | C" is equivalent to "ifdef !(A | B | C)" (or, using
 *	DeMorgan's Law, "ifdef !A & !B & !C") - in other words, TRUE if none of
 *	the symbols A, B, and C is currently defined.
 *
 *	"if" is accepted as a synonym for "ifdef".
 *
 *	"elif A" is TRUE if A is defined.  Multiple "elif" clauses may appear;
 *	at most one "if{{n}def}", "elif", or "else" clause in a given block
 *	will be processed.
 *
 *	"define" alone deletes all the current definitions; "if{def}" alone is
 *	always false; "ifndef" alone is always true.
 *
 *	The file name in the "include" directive may optionally be surrounded
 *	by "" or <>; in any case, path names are taken to be relative to
 *	the location of the file containing the "include" directive.  If the
 *	string "%y" appears in the file name, it is replaced by the last two
 *	digits of the current year.
 *
 *	The "-w" flag defines the symbol "whole_year", which may be tested
 *	in the same manner as any user-defined symbol.
 *
 *
 *	Moon file syntax:
 *
 *	The user may enter the dates and (optionally) times of quarter
 *	moons (from a reliable source such as an almanac or astronomical
 *	table) into a file called .moon%y (moon%y.dat on VMS), where %y is
 *	the last two digits of the year.  If such a file exists (in the
 *	same directory as the date file, or in the directory where Pcal
 *	itself lives), Pcal will interpolate the phase of the moon from the
 *	information in this file instead of using the default algorithm.
 *
 *	(Pcal originally used an extremely simplistic moon phase algorithm;
 *	the moon file was added to v4.0 to enable Pcal to interpolate the
 *	phase of the moon from the [presumably more accurate] information
 *	within.  More recently, the original moon phase algorithm was
 *	superseded by an astronomer-strength version, largely obviating
 *	the need for a moon file; however, it will continue to be
 *	supported for the foreseeable future.)
 *
 *	Entries in the moon file must conform to the following syntax:
 *
 *	  if -A flag (American date formats) specified:
 *	    <quarter> <month><sep><day> {<hour><sep><min>}
 *
 *	  if -E flag (European date formats) specified:
 *	    <quarter> <day><sep><month> {<hour><sep><min>}
 *
 *	where
 *
 *	  <quarter> := "nm", "fq" or "1q", "fm", "lq" or "3q" (new
 *			moon, first quarter, full moon, last quarter)
 *	  <hour>    := number 0-23 (24-hour clock)
 *	  <min>     := number 0-59
 *
 *	This file must contain entries for all quarter moons in the year,
 *	in chronological order; if any errors are encountered, Pcal will
 *	revert to using its default algorithm.
 *
 *	As in the date file, comments start with '#' and run through
 *	end-of-line.  
 */


/*
 * Standard headers:
 */

#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include <string.h>

/*
 * Pcal-specific definitions:
 */

#define MAIN_MODULE	1
#include "pcaldefs.h"
#include "pcalglob.h"
#include "pcallang.h"

/*
 * Globals:
 */

static int nargs = 0;				/* count of non-flag args  */
static int numargs[MAXARGS];			/* non-flag (numeric) args */
static int init_month, init_year, nmonths;	/* parsed values of above  */


/*
 * Main program - parse and validate command-line arguments, open files,
 * generate PostScript boilerplate and code to generate calendars.
 *
 * Program structure:
 *
 * For maximum user flexibility, Pcal gives the user several different ways
 * to set program flags and/or override earlier choices.  This necessitates
 * that main() call get_args() (directly or indirectly) several times:
 *
 *	a) to parse the command line, looking only for -Z flags (which turn on
 *	   debugging information) and numeric parameters
 *
 *	b) to parse environment variable (global symbol on VMS) PCAL_OPTS, if
 *	   defined
 *
 *	c) to parse the command line a second time, looking for options
 *	   related to finding/interpreting the date file: -[cefhuvDU]
 *
 * 	d) main() calls read_datefile() to read and parse the date file; it
 *	   in turn calls get_args() once per "opt" line in the date file
 *
 *	e) to parse the command line one final time, allowing the user to
 *	   override any flags other than those listed in c) above
 *
 * The rest of it is straightforward: main() attempts to open the output file
 * (if any), and, if successful, calls write_psfile() to generate the
 * PostScript output (or write_calfile() to generate the "calendar" input).
 * Some minor housekeeping and we're done.
 *
 */
#ifdef PROTOS
int main(int argc,
	 char **argv)
#else
int main(argc, argv)
	int argc;
	char **argv;
#endif
{
	FILE *dfp = NULL;		/* date file pointer */
	char *p, **ap, *pathlist[10];
	char tmp[STRSIZ];
	int n;

	init_misc();			/* handle initialization warts */

	/* extract root program name and program path - note that some
	 * systems supply the full pathname and others just the root
	 */

	strcpy(progname, **argv ? *argv : "pcal");

	if ((p = strrchr(progname, END_PATH)) != NULL)
		strcpy(progname, ++p);
	if ((p = strchr(progname, '.')) != NULL)
		*p = '\0';

	mk_path(progpath, find_executable(*argv));

	/* get version from VERSION_STRING (for use in PostScript comment) */
	strcpy(tmp, VERSION_STRING + 4);
	p = strchr(tmp, ' ') + 1;	/* skip program name */
	*strchr(p, ' ') = '\0';		/* terminate after version */
	strcpy(version, p);

	/*
	 * Get the arguments from a) the command line (pre-pass to pick up
	 * debug flags only), b) the environment variable PCAL_OPTS, c) the
	 * first command line pass, d) "opt" lines in the date file, and e)
	 * a final command line pass, in that order
	 */

	/* make an preliminary pass to look for the debug flags (to ensure
	 * ensure that -ZO will print any flags set in PCAL_OPTS); also
	 * get and validate the numeric command-line arguments
	 */
	if (!get_args(argv, P_CMD0, NULL, TRUE)) {
		usage(stderr, FALSE);		/* invalid flag or param */
		exit(EXIT_FAILURE);
	}

	/* parse environment variable PCAL_OPTS as a command line */

	if ((p = getenv(PCAL_OPTS)) != NULL) {
		strcpy(lbuf, "pcal ");		/* dummy program name */
		strcat(lbuf, p);
		(void) loadwords(words, lbuf);	/* split string into words */
		if (! get_args(words, P_ENV, PCAL_OPTS, FALSE)) {
			usage(stderr, FALSE);
			exit(EXIT_FAILURE);
		}
	}

	/* parse command-line arguments once to find name of date file, etc. */

	(void) get_args(argv, P_CMD1, NULL, FALSE);

	/* Attempt to open the date file as specified by the [-e | -f] flags */

	switch (datefile_type) {
	case NO_DATEFILE:
		dfp = NULL;
		break;

	case USER_DATEFILE:	
		/* Attempt to open user-specified calendar file: search
		 * first in the current directory, then in PCAL_DIR (if
		 * defined), and finally in the directory where the Pcal
		 * executable lives.  It is a fatal error if the
		 * user-specified date file cannot be found.
		 */
		n = 0;
		pathlist[n++] = "";
		if ((p = trnlog(PCAL_DIR)) != NULL)
			pathlist[n++] = p;
		pathlist[n++] = progpath;
		pathlist[n] = NULL;
		
		strcpy(tmp, datefile);	/* save original name for error msg */

		if ((dfp = alt_fopen(datefile, tmp, pathlist, "r")) == NULL) {
			FPR(stderr, E_FOPEN_ERR, progname, tmp);
			exit(EXIT_FAILURE);
		}
		break;

	case SYS_DATEFILE:
		/* Attempt to open system-specified calendar file: search
		 * first in PCAL_DIR, then in HOME_DIR (current directory
		 * if neither is defined) and finally in the directory where
		 * the Pcal executable lives.  It is not an error if the
		 * system-specified date file cannot be found; Pcal will
		 * simply generate an empty calendar.
		 */
		n = 0;
		if ((p = trnlog(PCAL_DIR)) != NULL)
			pathlist[n++] = p;
		if ((p = trnlog(HOME_DIR)) != NULL)
			pathlist[n++] = p;
		if (n == 0)
			pathlist[n++] = "";
		pathlist[n++] = progpath;
		pathlist[n] = NULL;
		
		dfp = alt_fopen(datefile, DATEFILE, pathlist, "r");

		/* if the date file has not been found and ALT_DATEFILE is
		 * defined, search same paths for ALT_DATEFILE before
		 * giving up
		 */
#ifdef ALT_DATEFILE
		if (!dfp)
			dfp = alt_fopen(datefile, ALT_DATEFILE, pathlist, "r");
#endif
		break;
	}

	/* read the date file (if any) and build internal data structure */

	if (dfp) {
		curr_year = init_year;
		read_datefile(dfp, datefile);
		fclose(dfp);
	} else
		datefile[0] = '\0';		/* for PostScript comment */

	/* reparse command line - flags there supersede those in date file */

	(void) get_args(argv, P_CMD2, NULL, FALSE);

	/* if in whole-year mode, round number of months up to full year and
	 * set default starting month to January of current year
	 */
	if (do_whole_year) {
		nmonths = ((nmonths + 11) / 12) * 12;
		if (nargs == 0)
			init_month = JAN;
	}

	/* done with the arguments and flags - try to open the output file */

	if (*outfile && freopen(outfile, "w", stdout) == (FILE *) NULL) {
		FPR(stderr, E_FOPEN_ERR, progname, outfile);
		exit(EXIT_FAILURE);
	}

	/* generate the "calendar" or PostScript code (cf. writefil.c) */

	if (calendar_out)
		write_calfile(init_month, init_year, nmonths);
	else
		write_psfile(init_month, init_year, nmonths);
	
	cleanup();		/* free allocated data */

	/* if output was written to a non-obvious location, tell user where */

#ifdef DEFAULT_OUTFILE
	FPR(stderr, I_OUT_NAME, progname, outfile);
#endif

	exit(EXIT_SUCCESS);
}


/*
 * init_misc - various initializations not easily handled in .h files
 */
#ifdef PROTOS
void init_misc(void)
#else
void init_misc()
#endif
{
	INIT_COLORS;		/* copy default_color to day_color */
	strcpy(notes_hdr, default_notes_hdr);	/* initialize notes_hdr */
}


/*
 * gen_shading - update the date and fill box shading values in "currstr"
 * as specified in "newstr"; one or both new values may be omitted
 */
#ifdef PROTOS
void gen_shading(char *currstr,
		 char *newstr)
#else
void gen_shading(currstr, newstr)
	char *currstr;
	char *newstr;
#endif
{
	char tmp1[STRSIZ], tmp2[STRSIZ];
	char *dval, *fval, *p;

	/* copy current date/fill string and split into its components */
	strcpy(tmp1, currstr);
	dval = tmp1;
	*(p = strchr(tmp1, '/')) = '\0';
	fval = p + 1;

	/* new value is of form <date> - use new date, current fill */
	if ((p = strchr(newstr, '/')) == NULL) {
		dval = newstr;

	/* new value is of form /<fill> - use current date, new fill */
	} else if (p == newstr) {
		if (p[1])
			fval = p + 1;

	/* new value is of form <date>/<fill> - use both new values */
	} else {
		strcpy(tmp2, newstr);
		dval = tmp2;
		*(p = strchr(tmp2, '/')) = '\0';
		if (p[1])	
			fval = p + 1;
	}

	/* concatenate date/fill values back into original string */
	strcpy(currstr, dval);
	strcat(currstr, "/");
	strcat(currstr, fval);
}


/*
 * set_color - set one or all weekdays to print in black or gray
 */
#ifdef PROTOS
void set_color(char *day,
	       int col)
#else
void set_color(day, col)
	char *day;		/* weekday name (or "all") */
	int  col;		/* select black or gray */
#endif
{
	int i;

	if (ci_strncmp(day, ALL, strlen(ALL)) == 0)	/* set all days */
		for (i = 0; i < 7; i++)
			day_color[i] = col;
	else						/* set single day */
		if ((i = get_weekday(day, FALSE)) != NOT_WEEKDAY)
			day_color[i] = col;
}


/*
 * get_flag() - look up flag in flag_tbl; return pointer to its entry
 * (NULL if not found)
 */
#ifdef PROTOS
FLAG_USAGE *get_flag(char flag)
#else
FLAG_USAGE *get_flag(flag)
	char flag;
#endif
{
	FLAG_USAGE *pflag;

	for (pflag = flag_tbl; pflag->flag; pflag++)
		if (flag == pflag->flag)
			return pflag;

	return flag ? NULL : pflag;		/* '\0' is a valid flag */
}


/*
 * set_debug_flag - look up "flag" in debug flag table and set selected
 * bits in debug flag word (clear word if "flag" is NULL)
 */
#ifdef PROTOS
void set_debug_flag(char *flag)
#else
void set_debug_flag(flag)
	char *flag;
#endif
{
	char f;
	DEBUG_INFO *pd;

	if (!flag) {			/* clear all if NULL */
		debug_flags = 0;
		return;
	}

	/* loop through all characters in "flag", setting corresponding
	 * bits in debug_flags
	 */
	while (f = *flag++)
		for (pd = debug_info; pd->flag; pd++)
			if (pd->flag == f)
				debug_flags |= pd->value;

}

/*
 * get_args - walk the argument list, parsing all arguments but processing only
 * those specified (in flag_tbl[]) to be processed this pass; return TRUE if
 * OK, FALSE if invalid flag found.
 */
#ifdef PROTOS
int get_args(char **argv,
	     int curr_pass,
	     char *where,
	     int get_numargs)
#else
int get_args(argv, curr_pass, where, get_numargs)
	char **argv;		/* argument list */
	int  curr_pass;		/* current pass (P_xxx) */
	char *where;		/* for error messages */
	int  get_numargs;	/* parse and save numeric arguments? */
#endif
{
	char *parg, *opt, *p, *pass, tmpbuf[STRSIZ];
	FLAG_USAGE *pflag, *pf;
	int i, flag, sv_debug;
	int flags_ok = TRUE;	/* return value */

/*
 * If argument follows flag (immediately or as next parameter), return
 * pointer to it (and bump argv if necessary); else return NULL
 */
#define GETARG() (*(*argv + 2) ? *argv + 2 : \
		  (*(argv+1) && **(argv+1) != '-' ? *++argv : NULL))

	/* set up pass name to print in debug messages */

	pass = where ? where :
	       curr_pass == P_CMD0 ? "CMD0" :
	       curr_pass == P_ENV  ? "ENV"  :
	       curr_pass == P_CMD1 ? "CMD1" :
	       curr_pass == P_CMD2 ? "CMD2" :
			    	     "OPT"  ;

	/* Walk argument list, ignoring first element (program name) */

 	while (opt = *++argv) {

		/* Assume that any non-flag argument is a numeric argument */
		if (*opt != '-') {
		    	if (get_numargs && nargs < MAXARGS) {
				if (! IS_NUMERIC(opt))
					goto bad_par;
				numargs[nargs++] = atoi(opt);
				if (DEBUG(DEBUG_OPTS))
					FPR(stderr, "%s: %s\n", pass, opt);
			}
			continue;
		}

		/* Check that flag is a) legal, and b) to be processed this pass */

		if (! (pflag = get_flag(flag = opt[1])) )
			goto bad_par;

		/* get optional argument even if flag not processed this pass */

		parg = pflag->has_arg ? GETARG() : NULL;

		if (! (pflag->passes & curr_pass)) {	/* skip flag this pass? */
			if (curr_pass == P_OPT)
				FPR(stderr, E_FLAG_IGNORED, progname, flag,
					DATE_FILE, where);
			continue;
		}

		/* echo pass name and flag if debugging on */
		if (DEBUG(DEBUG_OPTS))
			FPR(stderr, "%s: -%c%s%s\n", pass, flag,
				parg ? " " : "", parg ? parg : "");

		/* convert escape sequences in command-line parameters */
		if (parg && (curr_pass == P_CMD1 || curr_pass == P_CMD2)) {
			cvt_escape(tmpbuf, parg);
			parg = tmpbuf;
		}

		switch (flag) {

		case F_INITIALIZE:	/* reset all flags to defaults */

			/* set up a command line to reset all of the
			 * flags; call get_args() recursively to parse it
			 * (note that some of the flags must be reset
			 * explicitly, as no command-line flags exist to
			 * reset them)
			 */

			/* reset flags described above */
			julian_dates  = JULIAN_DATES;
			draw_moons    = DRAW_MOONS;
			do_whole_year = FALSE;
			blank_boxes   = FALSE;
			num_style     = NUM_STYLE;
			calendar_out  = FALSE;
			small_cal_pos = SMALL_CAL_POS;

			/* select program default for landscape/portrait
			 * mode (must be done first because -[xXyY] depend
			 * on it) and US/European date styles
			 */
			sprintf(lbuf, "pcal -%c -%c",
#if (ROTATE == LANDSCAPE)
				F_LANDSCAPE,
#else
				F_PORTRAIT,
#endif
#if (DATE_STYLE == USA_DATES)
				F_USA_DATES);
#else
				F_EUR_DATES);
#endif
			p = lbuf + strlen(lbuf);

			/* all other flags take arguments and are reset
			 * by specifying the flag without an argument
			 */
			for (pf = flag_tbl; pf->flag; pf++)
				if ((pf->passes & curr_pass) && pf->has_arg) {
					sprintf(p, " -%c", pf->flag);
					p += strlen(p);
				}

			/* split new command line into words; parse it */
			(void) loadwords(words, lbuf);
			(void) get_args(words, curr_pass, NULL, FALSE);

			break;

		case F_BLACK_DAY:	/* print day in black or gray */
		case F_GRAY_DAY:
			if (parg)
				set_color(parg, flag == F_BLACK_DAY ?
						  BLACK : GRAY);
			else
				INIT_COLORS;	/* reset to defaults */
			break;

 		case F_DAY_FONT:	/* specify alternate day font */
			strcpy(dayfont, parg ? parg : DAYFONT);
 			break;

		case F_NOTES_FONT:	/* specify alternate notes font */
			strcpy(notesfont, parg ? parg : NOTESFONT);
			break;

 		case F_TITLE_FONT:	/* specify alternate title font */
			strcpy(titlefont, parg ? parg : TITLEFONT);
 			break;

		case F_EMPTY_CAL:	/* generate empty calendar */
			datefile_type = NO_DATEFILE;
			strcpy(datefile, "");
			break;

		case F_DATE_FILE:	/* specify alternate date file */
			datefile_type = parg ? USER_DATEFILE : SYS_DATEFILE;
			strcpy(datefile, parg ? parg : "");
			break;

		case F_OUT_FILE:	/* specify alternate output file */
			strcpy(outfile, parg ? parg : OUTFILE);
			break;

		case F_LANDSCAPE:	/* generate landscape calendar */
 			rotate = LANDSCAPE;
			strcpy(xsval, XSVAL_L);
			strcpy(ysval, YSVAL_L);
			strcpy(xtval, XTVAL_L);
			strcpy(ytval, YTVAL_L);
 			break;
 
		case F_PORTRAIT:	/* generate portrait calendar */
 			rotate = PORTRAIT;
			strcpy(xsval, XSVAL_P);
			strcpy(ysval, YSVAL_P);
			strcpy(xtval, XTVAL_P);
			strcpy(ytval, YTVAL_P);
 			break;

		case F_HELP:		/* request version info and/or help */
		case F_USAGE:
		case F_VERSION:
			FPR(stdout, "%s\n", VERSION_STRING + 4);
			if (flag != F_VERSION)
				usage(stdout, flag == F_HELP);
			exit(EXIT_SUCCESS);
			break;

		case F_MOON_4:		/* draw four moons */
		case F_MOON_ALL:	/* draw a moon for each day */
			draw_moons = flag == F_MOON_ALL ? ALL_MOONS : SOME_MOONS;
			break;

		case F_DEFINE:		/* define preprocessor symbol */
			(void) do_define(parg);
			break;

		case F_UNDEF:		/* undef preprocessor symbol */
			(void) do_undef(parg);
			break;

		case F_L_FOOT:		/* specify alternate left foot */
			strcpy(lfoot, parg ? parg : LFOOT);
			break;

		case F_C_FOOT:		/* specify alternate center foot */
			strcpy(cfoot, parg ? parg : CFOOT);
			break;

 		case F_R_FOOT:		/* specify alternate right foot */
			strcpy(rfoot, parg ? parg : RFOOT);
			break;

 		case F_NOTES_HDR:	/* specify alternate notes header */
			strcpy(notes_hdr, parg ? parg : default_notes_hdr);
			break;

		case F_FIRST_DAY:	/* select starting day of week */
			if (parg) {
				if ((i = get_weekday(parg, FALSE)) != NOT_WEEKDAY)
					first_day_of_week = i;
			}
			else
				first_day_of_week = FIRST_DAY;
			break;

		case F_USA_DATES:	/* select American date style */
		case F_EUR_DATES:	/* select European date style */
			date_style = flag == F_USA_DATES ? USA_DATES : EUR_DATES;
			break;

		case F_X_TRANS:		/* set x-axis translation factor */
			strcpy(xtval, parg ? parg :
				(rotate == LANDSCAPE ? XTVAL_L : XTVAL_P));
			break;

		case F_Y_TRANS:		/* set y-axis translation factor */
			strcpy(ytval, parg ? parg :
				(rotate == LANDSCAPE ? YTVAL_L : YTVAL_P));
			break;

		case F_X_SCALE:		/* set x-axis scaling factor */
			strcpy(xsval, parg ? parg :
				(rotate == LANDSCAPE ? XSVAL_L : XSVAL_P));
			break;

		case F_Y_SCALE:		/* set y-axis scaling factor */
			strcpy(ysval, parg ? parg :
				(rotate == LANDSCAPE ? YSVAL_L : YSVAL_P));
			break;

		case F_JULIAN:
		case F_JULIAN_ALL:
			julian_dates = flag == F_JULIAN_ALL ? ALL_JULIANS :
							      SOME_JULIANS;
			break;

		case F_WHOLE_YEAR:
			do_whole_year = TRUE;
			(void) do_define(DEF_WHOLE_YEAR);
			break;

		case F_CALENDAR:
			calendar_out = TRUE;
			break;

		case F_BLANK_BOXES:
			blank_boxes = TRUE;
			break;

		case F_SC_NONE:
			small_cal_pos = SC_NONE;
			break;

		case F_SC_FIRST:
		case F_SC_SPLIT:
			small_cal_pos = flag == F_SC_FIRST ? SC_FIRST :
							     SC_SPLIT;
			break;

		case F_OUTLINE:
		case F_OUTLINE_GRAY:
			num_style = flag == F_OUTLINE ? OUTLINE_NUMS :
							FILLED_NUMS;
			break;

		case F_SHADING:		/* set date/fill shading levels */
			if (parg)
				gen_shading(shading, parg);
			else
				strcpy(shading, SHADING);
			break;

		case F_DEBUG:
			sv_debug = DEBUG(DEBUG_OPTS);
			set_debug_flag(parg);

			/* print -ZO flag if first time set */
			if (!sv_debug && DEBUG(DEBUG_OPTS))
				FPR(stderr, "%s: -%c%s\n", pass, flag,
					parg ? parg : "");
			break;

		case '-' :		/* accept - and -- as dummy flags */
		case '\0':
			break;

		default:		/* missing case label if reached!!! */

bad_par:				/* unrecognized parameter */

			FPR(stderr, E_ILL_OPT, progname, opt);
			if (where)
				FPR(stderr, E_ILL_OPT2,
					curr_pass == P_ENV ? ENV_VAR :
					curr_pass == P_OPT ? DATE_FILE : "",
					where);
			FPR(stderr, "\n");
			flags_ok = FALSE;
			break;
		}
        }

	/* if we read the numeric arguments, validate and interpret them */
	if (get_numargs)
		flags_ok &= check_numargs();

	return flags_ok;
}


/*
 * check_numargs - validate and interpret numeric command-line parameters;
 * return TRUE if all OK, print error message and return FALSE if not
 */
#ifdef PROTOS
int check_numargs(void)
#else
int check_numargs()
#endif
{
	/* Validate non-flag (numeric) parameters */

	struct tm *p_tm;	/* for getting current month/year */
	long tmp;
	int params_ok = TRUE;	/* return value */

	switch (nargs) {
	case 0:		/* no arguments - print current month and/or year */
		time(&tmp);
		p_tm = localtime(&tmp);
		init_month = p_tm->tm_mon + 1;
		init_year = p_tm->tm_year;
		nmonths = 1;
		break;			
	case 1:		/* one argument - print entire year */
		init_month = JAN;
		init_year = numargs[0];
		nmonths = 12;
		break;
	default:	/* two or three arguments - print one or more months */
		init_month = numargs[0];
		init_year = numargs[1];
		nmonths = nargs > 2 ? numargs[2] : 1;
		break;
	}

	if (nmonths < 1)		/* ensure at least one month */
		nmonths = 1;

	/* check range of month and year */

	if (init_month < JAN || init_month > DEC) {
		FPR(stderr, E_ILL_MONTH, progname, init_month, JAN, DEC);
		params_ok = FALSE;
	}
	
	if (init_year > 0 && init_year < 100)	/* treat nn as 19nn */
		init_year += CENTURY;
	
	if (init_year < MIN_YR || init_year > MAX_YR) {
		FPR(stderr, E_ILL_YEAR, progname, init_year, MIN_YR, MAX_YR);
		params_ok = FALSE;
	}

	return params_ok;
}



/*
 * color_msg - return character string explaining default day colors
 */
#ifdef PROTOS
char *color_msg(void)
#else
char *color_msg()
#endif
{
	int i, ngray = 0, others;
	static char msg[80];

	for (i = SUN; i <= SAT; i++)	/* count gray weekdays */
		if (default_color[i] == GRAY)
			ngray++;

	if (ngray == 0 || ngray == 7) {		/* all same color? */
		sprintf(msg, COLOR_MSG_1, ngray ? W_GRAY : W_BLACK);
		return msg;
	}

	others = ngray <= 3 ? BLACK : GRAY;	/* no - get predominant color */
	msg[0] = '\0';
	for (i = SUN; i <= SAT; i++)
		if (default_color[i] != others) {
			strncat(msg, days[i], MIN_DAY_LEN);
			strcat(msg, "/");
		}
	LASTCHAR(msg) = ' ';

	sprintf(msg + strlen(msg), COLOR_MSG_2,
		others == BLACK ? W_GRAY : W_BLACK,
                others == BLACK ? W_BLACK : W_GRAY);
	return msg;
}


/*
 * usage - print message explaining correct usage of the command-line
 * arguments and flags.  If "fullmsg" is true, print associated text
 */
#ifdef PROTOS
void usage(FILE *fp,
	   int fullmsg)
#else
void usage(fp, fullmsg)
	FILE *fp;	/* destination of usage message */
	int fullmsg;	/* print complete message? */
#endif
{
	FLAG_MSG *pflag;
	PARAM_MSG *ppar;
	DATE_MSG *pdate;
	char buf[30], *p, flag, *meta;
	int nchars, first, i, indent, n;

	sprintf(buf, "%s: %s", W_USAGE, progname);
	nchars = indent = strlen(buf);
	first = TRUE;
	meta = p = NULL;
	FPR(fp, "\n%s", buf);

	/* loop to print command-line syntax message (by group of flags) */

	for (pflag = flag_msg; (flag = pflag->flag) != '\0'; pflag++) {
		if (flag == '\n') {		/* end of group? */
			if (p)
				*p = '\0';
			if (meta) {		/* append metavariable name */
				strcat(buf, " ");
				strcat(buf, meta);
			}
			strcat(buf, "]");
			n = strlen(buf);
			if (nchars + n > SCREENWIDTH) {	/* does it fit on line? */
				FPR(fp, "\n");		/* no - start new one */
				for (nchars = 0; nchars < indent; nchars++)
					FPR(fp, " ");
			}
			FPR(fp, "%s", buf);
			nchars += n;
			first = TRUE;
		}
		else if (flag != ' ') {		/* accumulate flags for group */
			if (first) {
				sprintf(buf, " [");
				p = buf + strlen(buf);
			}
			else
				*p++ = '|';
			*p++ = '-';
			*p++ = flag;
			meta = pflag->meta;	/* save metavariable name */
			first = FALSE;
		}
	}

	/* loop to print selected numeric parameter descriptions */

	for (i = 0; i < PARAM_MSGS; i++) {
		sprintf(buf, " [%s]%s", param_msg[i].desc,
			i < PARAM_MSGS - 1 ? " |" : "");
		n = strlen(buf);
		if (nchars + n > SCREENWIDTH) {	/* does it fit on line? */
			FPR(fp, "\n");		/* no - start new one */
			for (nchars = 0; nchars < indent; nchars++)
				FPR(fp, " ");
		}
		FPR(fp, "%s", buf);
		nchars += n;
	}

	FPR(fp, "\n\n");
	if (! fullmsg) {
		FPR(fp, USAGE_MSG, progname, F_HELP);
		return;
	}
	
	/* loop to print the full flag descriptions */

	for (pflag = flag_msg; (flag = pflag->flag) != '\0'; pflag++) {
		if (flag == '\n') {	/* newline?  print and quit */
			FPR(fp, "\n");
			continue;
		}
		p = buf;		/* copy flag and metavariable to buffer */
		if (flag != ' ')
			*p++ = '-';
	/* special hack for VMS - surround upper-case flags in quotes */
#ifdef VMS
		if (isupper(flag)) {
			*p++ = '"';
			*p++ = flag;
			*p++ = '"';
		}
		else
			*p++ = flag;
#else
		*p++ = flag;
#endif
		*p = '\0';
		if (pflag->meta)
			sprintf(p, " %s", pflag->meta);
		FPR(fp, "\t%-16.16s", buf);
		if (pflag->text)
			FPR(fp, "%s", pflag->text);

		/* print default value if specified */
		if (pflag->def)
			FPR(fp, " (%s: %s)", W_DEFAULT, pflag->def[0] ? pflag->def : "\"\"" );
		FPR(fp, "\n");

		/* special case - print color messages after F_GRAY_DAY */
		if (flag == F_GRAY_DAY)
			FPR(fp, "\t\t\t  (%s: %s)\n", W_DEFAULT, color_msg());

	}
	
	/* now print the information about the numeric parameters */

	for (ppar = param_msg; ppar->desc; ppar++)
		FPR(fp, "\t%-16.16s%s\n", ppar->desc, ppar->text);
	
	/* print the date file syntax message */

	FPR(fp, "\n");
	for (pdate = date_msg; *pdate; pdate++)
		FPR(fp, "\t%s\n", *pdate);

}


/*
 * alt_fopen - attempt to open a file in one of several paths in a
 * NULL-terminated path list.  If successful, return (opened) file pointer
 * and fill in full path name; if not, return NULL
 */
#ifdef PROTOS
FILE *alt_fopen(char *fullpath,
		char *name,
		char *pathlist[],
		char *access) 
#else
FILE *alt_fopen(fullpath, name, pathlist, access)
	char *fullpath;		/* full path name (output) */
	char *name;		/* base name (or full path spec) */
	char *pathlist[];	/* NULL-terminated path list */
	char *access;		/* permission requested */
#endif
{
	char **path;
	FILE *fp;

	if (DEBUG(DEBUG_PATHS)) {
		FPR(stderr, "Searching for %s in the following paths:\n",
			name);
		for (path = pathlist; *path; path++)
			FPR(stderr, "  %s\n", **path ? *path : ".");
	}

	for (path = pathlist; *path; path++) {
		mk_filespec(fullpath, *path, name);
		if ((fp = fopen(fullpath, access)) != NULL) {
			if (DEBUG(DEBUG_PATHS))
				FPR(stderr, "found %s\n", fullpath);
			return fp;
		}
	}

	fullpath[0] = '\0';		/* file not found */
	return NULL;
}
