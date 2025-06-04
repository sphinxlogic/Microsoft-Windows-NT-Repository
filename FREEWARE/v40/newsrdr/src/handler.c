/*
**++
**  FACILITY:	NEWSRDR
**
**  ABSTRACT:	Condition handler for NEWSRDR.
**
**  MODULE DESCRIPTION:
**
**  	tbs
**
**  AUTHOR: 	    M. Madison
**  	    	    COPYRIGHT © 1993, MADGOAT SOFTWARE.  ALL RIGHTS RESERVED.
**
**  CREATION DATE:  26-SEP-1993
**
**  MODIFICATION HISTORY:
**
**  	26-SEP-1993 V1.0    Madison 	Initial coding.
**--
*/
#define MODULE_HANDLER
#include "newsrdr.h"

    EXTERN int pager_set_up;

#ifdef __GNUC__
#define CLI$_FACILITY ((unsigned int) cli$_facility)
    extern unsigned int cli$_facility();
#else
#pragma nostandard
    globalvalue unsigned int CLI$_FACILITY;
#pragma standard
#endif

/*
**++
**  ROUTINE:	cli_error_filter
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Condition handler for NEWSRDR.  Filters any signals coming
**  from CLI$ routines; uses our own output routines for all other
**  signals (if our output routines have been set up).
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	cli_error_filter(signal_array, mechanism_array)   <condition handler>
**
**  IMPLICIT INPUTS:	pager_set_up
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:	SS$_NORMAL, SS$_RESIGNAL.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int cli_error_filter(unsigned int sig[], unsigned int mech[]) {

    unsigned int return_status = SS$_NORMAL;
    unsigned int lclsig[32];
    int i;

    if (sig[1] == SS$_UNWIND) return SS$_NORMAL;
    if ($VMS_STATUS_FAC_NO(sig[1]) == CLI$_FACILITY) return SS$_NORMAL;
    if ($VMS_STATUS_SEVERITY(sig[1]) == STS$K_SEVERE) {
    	if (pager_set_up) Finish_Paged_Output(1);
    	return_status = SS$_RESIGNAL;
    } else if (!pager_set_up) {
    	return_status = SS$_RESIGNAL;
    } else {
    	lclsig[0] = sig[0] - 2;
    	if (lclsig[0] > 31) lclsig[0] = 31;
    	for (i = 1; i <= lclsig[0]; i++) lclsig[i] = sig[i];
    	sys$putmsg(lclsig, put_errmsg, 0, 0);
    }

    return return_status;
} /* cli_error_filter */
