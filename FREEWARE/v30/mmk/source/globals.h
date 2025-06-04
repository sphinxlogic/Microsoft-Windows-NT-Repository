/*
** GLOBALS.H
**
**  External reference definitions for MMS globals.  These are defined
**  as globals in MMK.C.
**
**  COPYRIGHT © 1992-1994, MADGOAT SOFTWARE.  ALL RIGHTS RESERVED.
*/
#include "mmk.h"
#pragma nostandard
    EXTERN struct SYMTABLE  global_symbols;
    EXTERN struct SYMTABLE  local_symbols;
    EXTERN struct SYMTABLE  cmdline_symbols;
    EXTERN struct SYMTABLE  builtin_symbols;
    EXTERN struct RULE      rules;
    EXTERN struct RULE	    *default_rule;
    EXTERN struct DEPEND    dependencies, dep_internal, dep_deferred;
    EXTERN struct SFX       suffixes;
    EXTERN struct CMD       do_first;
    EXTERN struct CMD       do_last;
    EXTERN int  	    verify, do_log, did_an_update, noaction, check_status;
    EXTERN int	    	    force, from_sources, ignore, use_cms, skip_intermediates;
    EXTERN int	    	    override_silent, override_ignore, symbol_override;
    EXTERN unsigned int     exit_status;
    EXTERN char	    	    cms$lib[256];
    EXTERN char	    	    cms_default_generation[256];
#pragma standard
