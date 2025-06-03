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
    EXTERN struct SYMBOL    global_symbols;
    EXTERN struct SYMBOL    local_symbols;
    EXTERN struct SYMBOL    cmdline_symbols;
    EXTERN struct RULE      rules;
    EXTERN struct RULE	    *default_rule;
    EXTERN struct DEPEND    dependencies, dep_internal, dep_deferred;
    EXTERN struct SFX       suffixes;
    EXTERN struct CMD       do_first;
    EXTERN struct CMD       do_last;
    EXTERN int  	    verify, do_log, did_an_update, noaction;
    EXTERN int	    	    force, from_sources, ignore, use_cms;
    EXTERN int	    	    override_silent, override_ignore;
    EXTERN unsigned int     exit_status;
    EXTERN char	    	    cms$lib[256];
    EXTERN char	    	    cms_default_generation[256];
#pragma standard
