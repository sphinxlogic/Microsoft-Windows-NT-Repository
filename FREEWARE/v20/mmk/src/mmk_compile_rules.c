/*
**++
**  FACILITY:	MMK
**
**  ABSTRACT:	MMK rules compiler.
**
**  MODULE DESCRIPTION:
**
**  	This program parses the rules in an MMK rules file and converts them
**  into C structures to be compiled and linked into the MMK utility.
**
**  AUTHOR: 	    M. Madison
**  	    	    COPYRIGHT © 1993-1995  MADGOAT SOFTWARE.  ALL RIGHTS RESERVED.
**
**  CREATION DATE:  30-APR-1993
**
**  MODIFICATION HISTORY:
**
**  	30-APR-1993 V1.0    Madison 	Initial coding.
**  	17-OCT-1993 V1.1    Madison 	Update for MMK V2.2.
**  	14-JUL-1994 V1.2    Madison 	Update for MMK V3.2.
**  	22-AUG-1994 V1.2-1  Madison 	Update for MMK V3.2-2.
**  	10-JAN-1995 V1.2-2  Madison 	Add put_command (for MMK V3.3-1).
**  	21-JUN-1995 V1.3    Madison 	Changes for MMK V3.4.
**--
*/
#define MMKC_VERSION 	"V1.3"
#define MMKC_COPYRIGHT	"Copyright © 1993-1995  MadGoat Software.  All Rights Reserved."

#ifdef __DECC
#pragma module MMK_COMPILE_RULES MMKC_VERSION
#else
#ifndef __GNUC__
#module MMK_COMPILE_RULES MMKC_VERSION
#endif
#endif

    char $$$Copyright[] = MMKC_COPYRIGHT;
    char *Version = MMKC_VERSION, *Copyright = &$$$Copyright[0];

#include "mmk.h"
#pragma nostandard
#include "mmk_msg.h"
#pragma standard
#include "clidefs.h"

/*
** Forward declarations
*/
    unsigned int main(void);
    unsigned int put_output(struct dsc$descriptor *);
    unsigned int put_command(struct dsc$descriptor *);
    static unsigned int cli_get_value(char *, char *, int);
    static unsigned int cli_present(char *);

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
** External references
*/
extern int mmk_compile_rules_cld();
#define MMK_COMPILE_RULES_CLD ((unsigned int) mmk_compile_rules_cld)

    extern void Read_Description(char *, char *, int); /* module READDESC */
    extern void Generate_Structures(char *, char *); /* module GENSTRUC */

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
    char Output_File[256], tmp[256];
    $DESCRIPTOR(cmdname, "MMKC ");
    unsigned int status;
    int i;

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

/*
** Fetch and parse command string
*/
    INIT_DYNDESC(cmdstr);
    status = lib$get_foreign(&cmdstr);
    str$prefix(&cmdstr, &cmdname);
    status = cli$dcl_parse(&cmdstr, MMK_COMPILE_RULES_CLD,
    	    	lib$get_input, lib$get_input);
    if (!OK(status)) return (status | STS$M_INHIB_MSG);

/*
** Get the command parameters and qualifiers
*/
    Output_File[0] = '\0';
    if (cli_present("OUTPUT") == CLI$_PRESENT) {
    	cli_get_value("OUTPUT", Output_File, sizeof(Output_File));
    }

    status = cli_get_value("RULES_FILE", tmp, sizeof(tmp));
    if (!OK(status)) return status | STS$M_INHIB_MSG;

    Read_Description(tmp, "SYS$DISK:[].MMS", 1);
    if (OK(exit_status)) Generate_Structures(tmp, Output_File);

    return exit_status | STS$M_INHIB_MSG;

}

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
**  	put_output(struct dsc$descriptor *dsc)
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
unsigned int put_output(struct dsc$descriptor *dsc) {

    return lib$put_output(dsc);

} /* put_output */

/*
**++
**  ROUTINE:	put_output
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
unsigned int put_command(struct dsc$descriptor *dsc) {

    return lib$put_output(dsc);

} /* put_output */

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
