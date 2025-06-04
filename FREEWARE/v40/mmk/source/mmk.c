/*
**++
**  FACILITY:	MMK
**
**  ABSTRACT:	MadGoat Make Utility
**
**  MODULE DESCRIPTION:
**
**  	This is the main routine for MMK.
**
**  AUTHOR: 	    M. Madison
**  	    	    COPYRIGHT © 1992-1996,  MADGOAT SOFTWARE.  ALL RIGHTS RESERVED.
**
**  CREATION DATE:  20-AUG-1992
**
**  MODIFICATION HISTORY:
**
**  	20-AUG-1992 V1.0    Madison 	Initial coding.
**  	27-AUG-1992 V1.1    Madison 	Spruce up a bit.
**  	29-SEP-1992 V1.2    Madison 	Add /FORCE, /FROM, /IDENT, /OUTPUT.
**  	12-OCT-1992 V1.2-1  Madison 	Fix some parsing problems.
**  	23-DEC-1992 V1.2-2  Madison 	Change MMS$xxx file refs to MMK_xxx
**  	12-JAN-1993 V1.2-3  Madison 	Fix a couple of minor bugs.
**  	08-MAR-1993 V1.2-4  Madison 	Fix library module reference bugs.
**  	02-APR-1993 V1.3    Madison 	Add support for "-" on command lines.
**  	23-APR-1993 V1.3-1  Madison 	Fix suffix processing.
**  	29-APR-1993 V1.3-2  Madison 	Fix dependency rule parsing.
**  	30-APR-1993 V1.4    Madison 	Default rules are now compiled-in.
**  	04-JUN-1993 V1.5    Madison 	Add support for some more directives.
**  	07-JUN-1993 V1.6    Madison 	Add MMS$CHANGED_LIST.
**	21-AUG-1993 V1.7    Madison 	Main source fix, build-rule fix.
**  	28-SEP-1993 V2.0    Madison 	Fix up for general release.
**  	24-SEP-1993 V2.0-1  Madison 	Fixed /FROM_SOURCES problem.
**  	27-SEP-1993 V2.0-2  Madison 	Fixed a couple more MMS discrepancies.
**  	27-SEP-1993 V2.0-3  Madison 	Changed file_get_rdt to do shared open.
**  	27-SEP-1993 V2.0-4  Madison 	Retracted the -2 and -3 shared-open changes.
**  	17-OCT-1993 V2.1    Madison 	Allow substitution rules in var refs.
**  	17-OCT-1993 V2.2    Madison 	Delete intermediate libfiles.
**  	20-OCT-1993 V2.3    Madison 	Add CTRL/T support.
**  	22-OCT-1993 V2.3-1  Madison 	Fix lines w/just blanks, deletion of
**  	    	    	    	    	    intermediates for multiple libmods.
**  	28-OCT-1993 V2.3-2  Madison 	More fixes.
**  	22-NOV-1993 V2.3-3  Madison 	Fix parsing of target from command line.
**  	01-DEC-1993 V2.3-4  Madison 	Fix /IGNORE getting ignored for built-in rules.
**  	02-DEC-1993 V2.3-5  Madison 	Allow symbol refs inside symbol refs.
**  	09-DEC-1993 V2.3-6  Madison 	extract_name fix.
**  	12-DEC-1993 V2.4    Madison 	Add MMS, MMSQUAL macros, support for
**  	    	    	    	    	    multiple dependencies for targets.
**  	02-MAR-1994 V2.4-1  Madison 	Fix non-resolvable specials.
**  	03-MAR-1994 V2.4-2  Madison 	Fix description parsing symbol problem.
**  	04-APR-1994 V2.4-3  Madison 	Fixed a couple of symbol problems.
**  	04-APR-1994 V2.4-4  Madison 	Another symbol problem.  Argh!
**  	08-APR-1994 V2.4-5  Madison 	Fix space-sep lists on lhs of dep rules.
**  	11-APR-1994 V2.4-6  Madison 	Make default rules more MMS-compatible.
**  	14-APR-1994 V2.4-7  Madison 	More random fixes.
**  	06-MAY-1994 V2.4-8  Madison 	Output @-prefixed commands on /NOACT.
**  	14-JUN-1994 V2.5    Madison 	Support for multiple targets on command
**  	    	    	    	    	line, courtesy of Richard Levitte.
**  	29-JUN-1994 V2.6    Madison 	TEX, library module, and source list fixes.
**  	01-JUL-1994 V3.0    Madison 	Support for CMS.
**  	06-JUL-1994 V3.0-1  Madison 	A couple of bug fixes.
**  	12-JUL-1994 V3.1    Madison 	Add :: support, circular-dependency checks.
**  	14-JUL-1994 V3.2    Madison 	Add prefix support in inference rules.
**  	17-AUG-1994 V3.2-1  Madison 	Upcase sfx list; fix MMS definition.
**  	22-AUG-1994 V3.2-2  Madison 	Fix LIBMOD bypass in dependecny creation.
**  	18-OCT-1994 V3.2-3  Madison 	Fix PARSE_DESCRIP error-signal bug.
**  	02-DEC-1994 V3.3    Madison 	Add /GENERATION qualifier for specifying
**                                         default CMS generation on fetches.
**  	10-JAN-1995 V3.3-1  Madison 	Prefix commands with $ if /OUTPUT specified.
**      13-JAN-1995 V3.3-1  Madison 	Add /NOACTION to $(MMS) symbol when noaction.
**  	16-MAR-1995 V3.3-2  Madison 	Turn off verify of CMS libraries.
**  	30-APR-1995 V3.3-3  Madison 	Fix lib mod prefixing.
**  	07-JUN-1995 V3.3-4  Madison 	Use default CMS generation in CMS RDT compares.
**  	19-JUN-1995 V3.3-5  Madison 	Fix for Fill_In_Missing_Sources.
**  	21-JUN-1995 V3.4    Madison 	Support /MACRO=fspec, /SKIP_INTERMEDIATES,
**  	    	    	    	    	    /OVERRIDE, /CHECK_STATUS.
**  	27-JUN-1995 V3.4-1  Madison 	Fix CMS fetches of description files.
**  	12-JUL-1995 V3.4-2  Madison 	Clean up after RMS search lists.
**  	17-JUL-1995 V3.4-3  Madison 	Wasn't setting have_rdt flag when getting RDT.
**  	21-JUL-1995 V3.4-4  Madison 	Fix for /SKIP.
**  	03-OCT-1995 V3.4-5  Madison 	Handle parsing of target file specs
**  	    	    	    	    	with names longer than 32 characters.
**  	05-OCT-1995 V3.4-6  Madison 	Fixed error paths causing ACCVIOs in SP_MGR.
**  	09-OCT-1995 V3.4-7  Madison 	Another case of incorrect use of free().
**  	06-NOV-1995 V3.4-8  Madison 	Fix library problem, rule problem.
**  	21-FEB-1996 V3.4-9  Madison 	Fix in READDESC.
**  	29-MAY-1996 V3.4-10 Madison 	Special hack for macros with dots.
**--
*/
#define MMK_VERSION 	"V3.4-10"
#define MMK_COPYRIGHT	"Copyright © 1992-1996, MadGoat Software.  All Rights Reserved."

#ifdef __DECC
#pragma module MMK MMK_VERSION
#else
#ifndef __GNUC__
#module MMK MMK_VERSION
#endif
#endif

#ifdef __DECC
#pragma extern_model save
#pragma extern_model common_block shr
#endif
    char $$$Copyright[] = MMK_COPYRIGHT;
#ifdef __DECC
#pragma extern_model restore
#endif

#include "mmk.h"
#pragma nostandard
#include "mmk_msg.h"
#pragma standard
#include "clidefs.h"
#ifdef __GNUC__
#include <vms/libclidef.h>
#else
#include <libclidef.h>
#endif
/*
** Forward declarations
*/
    unsigned int main(void);
    static void add_to_mmsqual(char *);
    unsigned int put_output(void *);
    unsigned int put_command(struct dsc$descriptor *);
    static unsigned int cli_get_value(char *, char *, int);
    static unsigned int cli_present(char *);
    static void Dump_It_All(void);
    static void dump_dependency(struct DEPEND *d);

/*
** Globals used throughout the program
*/

#pragma nostandard
    GLOBAL struct SYMTABLE  global_symbols;
    GLOBAL struct SYMTABLE  local_symbols;
    GLOBAL struct SYMTABLE  cmdline_symbols;
    GLOBAL struct SYMTABLE  builtin_symbols;
    GLOBAL struct RULE      rules;
    GLOBAL struct RULE	    *default_rule = 0;
    GLOBAL struct DEPEND    dependencies;
    GLOBAL struct DEPEND    dep_internal;
    GLOBAL struct DEPEND    dep_deferred;
    GLOBAL struct SFX       suffixes;
    GLOBAL struct CMD       do_first;
    GLOBAL struct CMD       do_last;
    GLOBAL int  	    verify, do_log, did_an_update, noaction, check_status;
    GLOBAL int	    	    from_sources, force, ignore, use_cms, skip_intermediates;
    GLOBAL int	    	    override_silent, override_ignore, symbol_override;
    GLOBAL unsigned int	    exit_status;
    GLOBAL char	    	    cms$lib[256];
    GLOBAL char	    	    cms_default_generation[256];
#pragma standard

/*
** Local statics
*/

    static unsigned int out_unit = 0;
    static char mmsqual[4096];

/*
** External references
*/
extern int mmk_cld();
#define MMK_CLD ((unsigned int) mmk_cld)

    extern void Read_Description(char *, char *, int); /* module READDESC */
    extern void Build_Target(char *);                  /* module BUILD_TARGET */
    extern void Define_Symbol(SYMTYPE, char *, char *, int); /* module SYMBOLS */
    extern int logical_present(char *);                /* module MISC */
    extern void close_subprocess(void);                /* module BUILD_TARGET */
    extern unsigned int file_create(char *, unsigned int *, char *);
    	    	    	    	    	    	       /* module FILEIO */
    extern unsigned int file_write(unsigned int, char *, int);
    	    	    	    	    	    	       /* module FILEIO */
    extern unsigned int file_open(char *, unsigned int *, char *, char *, int *);
    extern unsigned int file_read(unsigned int, char *, int, int *);
    extern unsigned int file_close(unsigned int);
    extern void Map_Default_Rules(void);    	       /* module DEFAULT_RULES */
    extern int get_logical(char *, char *, int);       /* module MISC */
    extern int make_object_name(char *, struct OBJECT *); /* module MISC */

/*
**++
**  ROUTINE:	main
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Main program.  Fetches the command from the command line,
**  uses CLI$ routines to parse it, then starts the build process.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	main
**
**  IMPLICIT INPUTS:	See global definitions at module head.
**
**  IMPLICIT OUTPUTS:	See global definitions at module head.
**
**  COMPLETION CODES:
**
**  	SS$_NORMAL, MMK__ALLOK : normal successful completion.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int main() {

    DESCRIP cmdstr;
    char target[256], descripfile[256], tmp[256];
    $DESCRIPTOR(cmdname, "MMK ");
    unsigned int status;
    int did1macro, doing_file, i;

/*
** Initialize the globals
*/
    for (i = 0; i < MMK_K_SYMTABLE_SIZE; i++) {
    	INIT_QUEUE(global_symbols.symlist[i]);
    	INIT_QUEUE(local_symbols.symlist[i]);
    	INIT_QUEUE(cmdline_symbols.symlist[i]);
    	INIT_QUEUE(builtin_symbols.symlist[i]);
    }
    INIT_QUEUE(rules);
    INIT_QUEUE(dependencies);
    INIT_QUEUE(dep_internal);
    INIT_QUEUE(dep_deferred);
    INIT_QUEUE(suffixes);
    INIT_QUEUE(do_first);
    INIT_QUEUE(do_last);
    exit_status = SS$_NORMAL;
    ignore = override_silent = override_ignore = symbol_override = 0;
    skip_intermediates = 0;
    get_logical("CMS$LIB", cms$lib, sizeof(cms$lib));

/*
** Fetch and parse command string
*/
    INIT_DYNDESC(cmdstr);
    status = lib$get_foreign(&cmdstr);
    str$prefix(&cmdstr, &cmdname);
    status = cli$dcl_parse(&cmdstr, MMK_CLD, lib$get_input, lib$get_input);
    if (!OK(status)) return (status | STS$M_INHIB_MSG);

/*
** Check for /IDENT
*/
    if (cli_present("IDENTIFICATION") == CLI$_PRESENT) {
    	lib$signal(MMK__IDENT, 1, MMK_VERSION, MMK__COPYRIGHT, 1, MMK_COPYRIGHT);
    	return SS$_NORMAL;
    }

/*
** Get the command parameters and qualifiers
*/
    descripfile[0] = target[0] = mmsqual[0] = '\0';
    (void) cli_get_value("DESCRIPTION", descripfile, sizeof(descripfile));
    if (descripfile[0]) {
    	add_to_mmsqual("/DESCRIPTION=");
    	add_to_mmsqual(descripfile);
    }
    status = cli_present("VERIFY");
    verify = (status != CLI$_NEGATED);
    if (!verify) add_to_mmsqual("/NOVERIFY");
    override_silent = (status != CLI$_ABSENT);
    if (override_silent && verify) add_to_mmsqual("/VERIFY");
    do_log = (cli_present("LOG") == CLI$_PRESENT);
    if (do_log) add_to_mmsqual("/LOG");
    symbol_override = (cli_present("OVERRIDE") == CLI$_PRESENT);
    if (symbol_override) add_to_mmsqual("/OVERRIDE");
    skip_intermediates = (cli_present("SKIP_INTERMEDIATES") == CLI$_PRESENT);
    if (skip_intermediates) add_to_mmsqual("/SKIP_INTERMEDIATES");
    noaction = cli_present("ACTION") == CLI$_NEGATED;
    if (noaction) add_to_mmsqual("/NOACTION");
    force = cli_present("FORCE") == CLI$_PRESENT;
    if (force) add_to_mmsqual("/FORCE");
    from_sources = cli_present("FROM_SOURCES") == CLI$_PRESENT;
    if (from_sources) {
    	add_to_mmsqual("/FROM_SOURCES");
    	skip_intermediates = 0;
    }
    check_status = (cli_present("CHECK_STATUS") == CLI$_PRESENT);
    if (check_status) {
    	add_to_mmsqual("/CHECK_STATUS");
    	noaction = 1;
    }

    use_cms = cli_present("CMS") == CLI$_PRESENT;
    if (use_cms) add_to_mmsqual("/CMS");
    if (use_cms && cms$lib[0] == '\0') use_cms = 0;
    if (use_cms) {
    	if (!OK(cli_get_value("GENERATION", cms_default_generation, 
    	    	    	    sizeof(cms_default_generation)))) {
    	    strcpy(cms_default_generation, "1+");
    	}
    	status = cli_present("GENERATION");
    	if (status == CLI$_PRESENT) {
    	    add_to_mmsqual("/GENERATION=");
    	    add_to_mmsqual(cms_default_generation);
    	}
    }
    status = cli_present("IGNORE");
    if (status == CLI$_PRESENT) {
    	override_ignore = 1;
    	if (OK(cli_get_value("IGNORE", tmp, sizeof(tmp)))) {
    	    ignore = *tmp == 'F' ? 3 : (*tmp == 'E' ? 2 : 1);
    	    add_to_mmsqual("/IGNORE=");
    	    add_to_mmsqual(tmp);
    	} else {
    	    ignore = 3;
    	    add_to_mmsqual("/IGNORE");
    	}
    } else if (status == CLI$_NEGATED) {
    	override_ignore = 1;
    	add_to_mmsqual("/NOIGNORE");
    }
    if (cli_present("OUTPUT") == CLI$_PRESENT) {
    	cli_get_value("OUTPUT", tmp, sizeof(tmp));
    	status = file_create(tmp, &out_unit, ".LOG");
    	if (!OK(status)) {
    	    lib$signal(MMK__OPENOUT, 1, tmp, status);
    	    return (MMK__OPENOUT|STS$M_INHIB_MSG);
    	}
    	add_to_mmsqual("/OUTPUT=SYS$OUTPUT:");
    }

/*
** Command-line definition of symbols
*/
    if (cli_present("MACRO") == CLI$_PRESENT) add_to_mmsqual("/MACRO=(");
    did1macro = 0;
    doing_file = 0;
    while (1) {
    	char *ep, *vp, *sp;
    	char macro_file[256];
    	unsigned int munit;
    	int slew, len;

    	if (doing_file) {
    	    status = file_read(munit, tmp, sizeof(tmp), &len);
    	    if (!OK(status)) {
    	    	file_close(munit);
    	    	doing_file = 0;
    	    	continue;
    	    }
    	    slew += 1;
    	    tmp[len] = '\0';
    	} else {
    	    status = cli_get_value("MACRO", tmp, sizeof(tmp));
    	    if (!OK(status)) break;
    	}

/*
**  Trim leading blanks from macro name.  If it's a null line, just
**  ignore it.
*/
    	for (sp = tmp; isspace(*sp); sp++);
    	if (*sp == '\0') continue;

/*
**  Locate the "=".  If there is none present and we're doing command-line
**  macros, check to see if this is a file specification.
*/
    	ep = strchr(sp,'=');
    	if (!doing_file && ep == 0) {
    	    status = file_open(sp, &munit, "SYS$DISK:[].MMS", macro_file, 0);
    	    if (OK(status)) {
    	    	doing_file = 1;
    	    	if (did1macro) add_to_mmsqual(",");
    	    	add_to_mmsqual(sp);
    	    	did1macro = 1;
    	    	slew = 0;
    	    	continue;
    	    }
/*
**  For compatibility with previous versions of MMK, we allow ":" in place
**  of "=" (but only in command-line macros).
*/
    	    ep = strchr(sp, ':');
    	}

    	if (ep != 0) {
    	    vp = ep + 1;
    	    while (isspace(*vp)) vp++;
    	    while (isspace(*(ep-1))) ep--;
    	    *ep = 0;
    	    Define_Symbol(MMK_K_SYM_CMDLINE, sp, vp, strlen(vp));
    	    if (!doing_file) {
    	    	if (did1macro) add_to_mmsqual(",");
    	    	add_to_mmsqual(tmp);
    	    	add_to_mmsqual("=\"");
    	    	add_to_mmsqual(vp);
    	    	add_to_mmsqual("\"");
    	    	did1macro = 1;
    	    }
    	} else if (!doing_file) {
/*
**  For compatibility with previous versions, we allow MACRO to be equivalent
**  to MACRO=1 (but only for command-line macros).
*/
    	    Define_Symbol(MMK_K_SYM_CMDLINE, tmp, "1", 1);
    	    if (did1macro) add_to_mmsqual(",");
    	    add_to_mmsqual(tmp);
    	} else {
    	    lib$signal(MMK__MACFILSYNTAX, 2, slew, macro_file);
    	}

    }

    if (cli_present("MACRO") == CLI$_PRESENT) add_to_mmsqual(")");
    did_an_update = 0;

/*
** Rules processing is as follows:
**
**  1.  The default rules are mapped in, having been compiled
**  	by the description file compiler.
**
**  2.	Unless /NOLOCAL_RULES is specified, the rules file
**  	located through the logical name MMK_LOCAL_RULES is
**  	processed.  Use this logical to point to files containing
**  	system-wide build rules.
**
**  3.	If the /RULES=(file[,...]) qualifier is present, the
**  	specified rules files are read in.  If it is absent,
**  	a personal rules file is located by the logical name
**  	MMK_PERSONAL_RULES.  If /NORULES is specified, no
**  	per-build or personal rules files are read in.
*/

    status = cli_present("RULES_FILE");
    if (status != CLI$_NEGATED) {
    	Map_Default_Rules();
    	if (!OK(exit_status)) return exit_status | STS$M_INHIB_MSG;
    }

    if (cli_present("LOCAL_RULES") != CLI$_NEGATED) {
    	if (logical_present("MMK_LOCAL_RULES")) {
    	    Read_Description("MMK_LOCAL_RULES", "SYS$DISK:[].MMS", 1);
    	}
    } else add_to_mmsqual("/NOLOCAL_RULES");

    if (status == CLI$_PRESENT || status == CLI$_DEFAULTED) {
    	did1macro = 0;
    	if (status == CLI$_PRESENT) add_to_mmsqual("/RULES_FILE=(");
    	while (OK(cli_get_value("RULES_FILE", tmp, sizeof(tmp)))) {
    	    Read_Description(tmp, "SYS$DISK:[].MMS", 1);
    	    if (status == CLI$_PRESENT) {
    	    	if (did1macro) add_to_mmsqual(",");
    	    	add_to_mmsqual(tmp);
    	    	did1macro = 1;
    	    }
    	}
    	if (status == CLI$_PRESENT) add_to_mmsqual(")");
    } else if (status != CLI$_NEGATED) {
    	if (logical_present("MMK_PERSONAL_RULES")) {
    	    Read_Description("MMK_PERSONAL_RULES","SYS$DISK:[].MMS", 1);
    	}
    } else add_to_mmsqual("/NORULES_FILE");

/*
**  We've parsed all the qualifiers, so create the MMSQUALIFIERS macro
**  (and MMS macro, too).
*/
    Define_Symbol(MMK_K_SYM_BUILTIN, "MMSQUALIFIERS", mmsqual, strlen(mmsqual));

/*
**  Now read in and parse the description file.
*/
    Read_Description(descripfile, "SYS$DISK:[]DESCRIP.MMS", 0);

/*
**  Dump our brains if we're told to
*/
    if (cli_present("DUMP") == CLI$_PRESENT) Dump_It_All();

/*
**  Now define the MMS symbol (left undefined while we were parsing so
**  we can identify $(MMS) references in action lines and execute them
**  even if /NOACTION is specified.
*/
    Define_Symbol(MMK_K_SYM_DESCRIP,
    	    	   "MMS", (noaction ? "MMK/NOACTION" : "MMK"),
                          (noaction ? 12             : 3));

/*
** Now that all the dependencies are defined, we can do the build.
*/
    status = cli_present("TARGET");
    if (status == CLI$_PRESENT) cli_get_value("TARGET", target, sizeof(target));
    else target[0] = '\0';

    do {
    	did_an_update = 0;
    	Build_Target(target);

    	if ($VMS_STATUS_SEVERITY(exit_status) == STS$K_SEVERE) break;

/*
** If everything is up-to-date, say so.
*/
    	if (check_status) {
    	    static $DESCRIPTOR(mms$status, "MMS$STATUS");
    	    static $DESCRIPTOR(one, "1");
    	    static $DESCRIPTOR(zero, "0");
    	    static unsigned int gsym = LIB$K_CLI_GLOBAL_SYM;
    	    lib$set_symbol(&mms$status, did_an_update ? &zero : &one, &gsym);
    	    if (did_an_update) {
    	    	lib$signal(MMK__NEEDSUPD, 1, target);
    	    } else {
    	    	lib$signal(MMK__TRGCURRENT, 1, target);
    	    }
    	} else {
    	    if (!did_an_update) {
    	    	lib$signal(MMK__NOUPDATE, 1, target);
    	    	if (exit_status == SS$_NORMAL) exit_status = MMK__NOUPDATE;
    	    }
    	}

    } while (OK(cli_get_value("TARGET", target, sizeof(target))));
/*
** If we did have to execute some commands, there will be a subprocess
** hanging around that we should kill.
*/
    close_subprocess();

    return exit_status | STS$M_INHIB_MSG;

} /* main */

/*
**++
**  ROUTINE:	add_to_mmsqual
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Adds a string to the mmsqual variable.
**
**  RETURNS:	void
**
**  PROTOTYPE:
**
**  	add_to_mmsqual(char *)
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:	None.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
static void add_to_mmsqual(char *str) {

    static char *qp  = mmsqual;
    static int  qlen = 0;
    int i;

    i = strlen(str);

    if (i == 0) return;
    if (qlen + i > sizeof(mmsqual)-1) return;

    if (i == 1) {
    	*qp++ = *str;
    	*qp = '\0';
    } else {
    	strcpy(qp, str);
    	qp += i;
    }
    qlen += i;

} /* add_to_mmsqual */

/*
**++
**  ROUTINE:	cli_get_value
**
**  FUNCTIONAL DESCRIPTION:
**
**  	C Interface to CLI$GET_VALUE.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	cli_get_value (char *argname, DESCRIP *arg)
**
** argname: ASCIZ_string, read only, by reference
** arg:	    char_string, write only, by descriptor (dynamic)
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**
**  	All those from CLI$PRESENT and CLI$GET_VALUE.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
static unsigned int cli_get_value(char *argname, char *arg, int argsize) {
    DESCRIP argnamd, argd;
    unsigned short arglen;
    int status;

    INIT_SDESC(argnamd, strlen(argname), argname);
    INIT_SDESC(argd, argsize-1, arg);
    status = cli$present(&argnamd);
    if ($VMS_STATUS_SUCCESS(status)) {
    	status = cli$get_value(&argnamd, &argd, &arglen);
    	if (OK(status)) *(arg+arglen) = '\0';
    }
    return status;
}

/*
**++
**  ROUTINE:	put_output
**
**  FUNCTIONAL DESCRIPTION:
**
**  	LIB$PUT_OUTPUT replacement that directs things either
**  to SYS$OUTPUT or to the file specified in /OUTPUT.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	put_output(void *dsc)
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int put_output(void *xdsc) {

    struct dsc$descriptor *dsc;

    dsc = xdsc;
    return (out_unit ? file_write(out_unit, dsc->dsc$a_pointer, dsc->dsc$w_length)
    	    	     : lib$put_output(dsc));
} /* put_output */

/*
**++
**  ROUTINE:	put_command
**
**  FUNCTIONAL DESCRIPTION:
**
**  	LIB$PUT_OUTPUT replacement that directs things either
**  to SYS$OUTPUT or to the file specified in /OUTPUT (for commands).
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	put_command(struct dsc$descriptor *dsc)
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int put_command(struct dsc$descriptor *xdsc) {

    static $DESCRIPTOR(ctrstr, "$ !AS");
    struct dsc$descriptor dsc;
    unsigned int status;
    int i;

    if (out_unit == 0) return lib$put_output(xdsc);

    for (i = 0; i < xdsc->dsc$w_length; i++) {
    	if (isspace(xdsc->dsc$a_pointer[i])) continue;
    	if (xdsc->dsc$a_pointer[i] == '$') {
    	    return file_write(out_unit, xdsc->dsc$a_pointer, xdsc->dsc$w_length);
    	} else break;
    }
    INIT_DYNDESC(dsc);
    status = lib$sys_fao(&ctrstr, 0, &dsc, xdsc);
    if (OK(status)) status = file_write(out_unit, dsc.dsc$a_pointer, dsc.dsc$w_length);
    str$free1_dx(&dsc);
    return status;

} /* put_command */

/*
**++
**  ROUTINE:	cli_present
**
**  FUNCTIONAL DESCRIPTION:
**                               
**  	C Interface to CLI$PRESENT.
**
**  RETURNS:	cond_value, intword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	cli_present (char *argname)
**
** argname: ASCIZ_string, read only, by reference
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**
**  	All those from CLI$PRESENT.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
static unsigned int cli_present(char *argname) {
    DESCRIP argnamd;

    INIT_SDESC(argnamd, strlen(argname), argname);
    return cli$present(&argnamd);
}


/*
**++
**  ROUTINE:	Dump_It_All
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Prints out the contents of the suffix, global symbol, rule,
**  and dependency lists.
**
**  	Mainly for use in debugging MMK, or in debugging description files.
**
**  RETURNS:	void
**
**  PROTOTYPE:
**
**  	Dump_It_All
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**
**
**  SIDE EFFECTS:   	None.
**
**--
*/
static void Dump_It_All(void) {

    struct SYMBOL *sym;
    struct RULE *xr, *r;
    struct CMD *c;
    struct OBJREF *o;
    struct DEPEND *d;
    struct SFX *sfx;
    struct dsc$descriptor dsc, sdsc;
    char nam[MMK_S_FILE];
    int len, i, j;

    static struct SYMTABLE *symtable[] = {
    	&builtin_symbols, &global_symbols, &cmdline_symbols
    };
    static char *symtable_name[] = {
    	"MMK built-in macro definitions",
        "Rules-file and description-file macro definitions",
    	"Command-line macro definitions"
    };

    static $DESCRIPTOR(slhdr, ".SUFFIXES :");
    static $DESCRIPTOR(ctrcmd, "    !AD!AD !AZ");
    static $DESCRIPTOR(first, ".FIRST :");
    static $DESCRIPTOR(last, ".LAST :");
    static $DESCRIPTOR(dflt, ".DEFAULT :");
    static $DESCRIPTOR(internal, "! Internally-generated dependencies:");
    static $DESCRIPTOR(blank, "");
    static $DESCRIPTOR(comma, ", ");
    static $DESCRIPTOR(comhyp, ",-  ");
    static $DESCRIPTOR(leader, "        ");
    static $DESCRIPTOR(end_tag, "! --- end of description ---");
    static $DESCRIPTOR(symhdr, "!! !AZ:");
    static $DESCRIPTOR(symctr, "  !AZ = !AZ");

    INIT_DYNDESC(dsc);
    for (i = 0; i < sizeof(symtable)/sizeof(symtable[0]); i++) {
    	lib$sys_fao(&symhdr, 0, &dsc, symtable_name[i]);
    	put_output(&dsc);
    	put_output(&blank);
    	for (j = 0; j < MMK_K_SYMTABLE_SIZE; j++) {
    	    for (sym = symtable[i]->symlist[j].head;
    	    	    sym != (struct SYMBOL *) &symtable[i]->symlist[j];
    	    	    sym = sym->flink) {
    	    	lib$sys_fao(&symctr, 0, &dsc, sym->name, sym->value);
    	    	put_output(&dsc);
    	    }
    	}
    	put_output(&blank);
    }

    str$copy_dx(&dsc, &slhdr);
    for (sfx = suffixes.flink; sfx != &suffixes; sfx = sfx->flink) {
    	static $DESCRIPTOR(spc, " ");
    	static $DESCRIPTOR(spcs, "         ");
    	static $DESCRIPTOR(hyp, " -");
    	str$append(&dsc, &spc);	
    	INIT_SDESC(sdsc, strlen(sfx->value), sfx->value);
    	str$append(&dsc, &sdsc);
    	if (dsc.dsc$w_length > 70 && sfx->flink != &suffixes) {
    	    str$append(&dsc, &hyp);
    	    put_output(&dsc);
    	    str$copy_dx(&dsc, &spcs);
    	}
    }
    put_output(&dsc);

    if (rules.flink != &rules) {
    	put_output(&blank);
    	for (xr = rules.flink; xr != &rules; xr = xr->flink) {
    	    static $DESCRIPTOR(ctrpfx, "{!AZ}!AZ{!AZ}!AZ : ");
    	    static $DESCRIPTOR(ctr, "!AZ!AZ : ");
    	    for (r = xr; r != 0; r = r->next) {
    	    	if (r->trgpfx[0] || r->srcpfx[0]) {
    	    	    lib$sys_fao(&ctrpfx, 0, &dsc, r->srcpfx, r->src,
    	    	    	    r->trgpfx, r->trg);
    	    	} else {
    	    	    lib$sys_fao(&ctr, 0, &dsc, r->src, r->trg);
    	    	}
    	    	put_output(&dsc);
    	    	for (c = r->cmdque.flink; c != &r->cmdque; c = c->flink) {
    	    	    lib$sys_fao(&ctrcmd, 0, &dsc, 
    	    	    	(c->flags & CMD_M_IGNERR ? 1 : 0), "-", 
    	    	    	(c->flags & CMD_M_NOECHO ? 1 : 0), "@", c->cmd);
    	    	    put_output(&dsc);
    	    	}
    	    }
    	}
    }

    if (default_rule != 0) {
    	put_output(&blank);
    	put_output(&dflt);
    	for (c = default_rule->cmdque.flink; c != &default_rule->cmdque;
    	    	    	    	    	    	    	    c = c->flink) {
    	    lib$sys_fao(&ctrcmd, 0, &dsc, 
    	    	    (c->flags & CMD_M_IGNERR ? 1 : 0), "-", 
    	    	    (c->flags & CMD_M_NOECHO ? 1 : 0), "@", c->cmd);
    	    put_output(&dsc);
    	}
    }

    if (do_first.flink != &do_first) {
    	put_output(&blank);
    	put_output(&first);
    	for (c = do_first.flink; c != &do_first; c = c->flink) {
    	    lib$sys_fao(&ctrcmd, 0, &dsc, 
    	    	    (c->flags & CMD_M_IGNERR ? 1 : 0), "-", 
    	    	    (c->flags & CMD_M_NOECHO ? 1 : 0), "@", c->cmd);
    	    put_output(&dsc);
    	}
    }

    if (do_last.flink != &do_last) {
    	put_output(&blank);
    	put_output(&last);
    	for (c = do_last.flink; c != &do_last; c = c->flink) {
    	    lib$sys_fao(&ctrcmd, 0, &dsc, 
    	    	    (c->flags & CMD_M_IGNERR ? 1 : 0), "-", 
    	    	    (c->flags & CMD_M_NOECHO ? 1 : 0), "@", c->cmd);
    	    put_output(&dsc);
    	}
    }

    if (dependencies.flink != &dependencies) {
    	put_output(&blank);
    	for (d = dependencies.flink; d != &dependencies; d = d->flink) {
    	    struct DEPEND *d1;
    	    dump_dependency(d);
    	    for (d1 = d->dc_flink; d1 != 0; d1 = d1->dc_flink) {
    	    	dump_dependency(d1);
    	    }
    	}
    }

    if (dep_internal.flink != &dep_internal) {
    	put_output(&blank);
    	put_output(&internal);

    	for (d = dep_internal.flink; d != &dep_internal; d = d->flink) dump_dependency(d);
    }

    str$free1_dx(&dsc);
    put_output(&blank);
    put_output(&end_tag);
    put_output(&blank);

} /* Dump_It_All */

/*
**++
**  ROUTINE:	dump_dependency
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Prints out a dependency rule.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	tbs
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**
**
**  SIDE EFFECTS:   	None.
**
**--
*/
static void dump_dependency (struct DEPEND *d) {

    struct dsc$descriptor dsc, sdsc;
    char nam[MMK_S_FILE];
    struct OBJREF *o;
    struct CMD *c;
    int did1, len;

    static $DESCRIPTOR(ctrcmd, "    !AD!AD !AZ");
    static $DESCRIPTOR(sep, " : ");
    static $DESCRIPTOR(dsep, " :: ");
    static $DESCRIPTOR(blank, "");
    static $DESCRIPTOR(comma, ", ");
    static $DESCRIPTOR(comhyp, ",-  ");
    static $DESCRIPTOR(leader, "        ");

    INIT_DYNDESC(dsc);
    len = make_object_name(nam, d->target);
    INIT_SDESC(sdsc, len, nam);
    str$copy_dx(&dsc, &sdsc);
    str$append(&dsc, d->double_colon ? &dsep : &sep);

    did1 = 0;
    for (o = d->sources.flink; o != &d->sources; o = o->flink) {
    	if (did1 == 1) str$append(&dsc, &comma);
    	else if (did1 == 2) str$copy_dx(&dsc, &leader);
    	len = make_object_name(nam, o->obj);
    	INIT_SDESC(sdsc, len, nam);
    	str$append(&dsc, &sdsc);
    	if (dsc.dsc$w_length > 70 && o->flink != &d->sources) {
    	    str$append(&dsc, &comhyp);
    	    put_output(&dsc);
    	    did1 = 2;
    	} else did1 = 1;
    }
    put_output(&dsc);

    if (d->cmdqptr != 0) {
    	for (c = d->cmdqptr->flink; c != d->cmdqptr; c = c->flink) {
    	    lib$sys_fao(&ctrcmd, 0, &dsc, 
    	    	    	(c->flags & CMD_M_IGNERR ? 1 : 0), "-", 
    	    	    	(c->flags & CMD_M_NOECHO ? 1 : 0), "@", c->cmd);
    	    	    put_output(&dsc);
    	}
    }

} /* dump_dependency */
