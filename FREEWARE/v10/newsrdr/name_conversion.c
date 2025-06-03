/*
!++
!
! MODULE:   	    NAME_CONVERSION
!
! FACILITY: 	    NEWSRDR (sample source)
!
! ABSTRACT: 	    Example of site-installable nickname conversion.
!
! MODULE DESCRIPTION:
!
!   This module contains routines for use by NEWSRDR for translating
!   between actual VMS usernames and site-specific aliases.
!
!   This module contains a fairly primitive lookup table to implement
!   the translation.
!
!   To use this module: MODIFY IT AS NEEDED FOR YOUR SITE, then compile it
!   and link it with the commands:
!
!   On VAX systems:
!       $ cc name_conversion
!   	$ link/share/notrace name_conversion,sys$input:/opt
!   	    sys$share:vaxcrtl/share
!   	    universal=init,convert,full_convert,cleanup
!   	    <ctrl/Z>
!   On AXP systems:
!   	$ cc name_conversion
!   	$ link/share/notrace name_conversion,sys$input:/opt
!   	    symbol_vector=(init=proc,convert=proc,-
!   	    	full_convert=proc,cleanup=proc)
!   	    <ctrl/Z>
!
!   Then copy it to SYS$SHARE and make it available with the commands:
!
!   	$ copy name_conversion.exe sys$common:[syslib]/protection=w:re
!   	$ install create sys$share:name_conversion/share/open/header
!   	$ define/system/exec newsrdr_name_conversion sys$share:name_conversion
!
!   (You need a suitably privileged account to do this.)
!
! AUTHOR:   	    M. Madison
!   	    	    COPYRIGHT © 1993  MADGOAT SOFTWARE.
!
!   THIS SOFTWARE IS PROVIDE "AS IS".  NEITHER THE AUTHOR NOR TGV, INC.
!   MAKE ANY GUARANTEES REGARDING THE SUITABILITY, RELIABILITY, SECURITY,
!   USEFULNESS, OR PERFORMANCE OF THIS SOFTWARE.  >>USE AT YOUR OWN RISK.
!
! CREATION DATE:    03-DEC-1990
!
! MODIFICATION HISTORY:
!
!   03-DEC-1990	V1.0	Madison	    Initial coding.
!   11-MAR-1992	V1.1	Madison	    Update for MX V3.1.
!   15-MAY-1992	V1.2	Madison	    Correct "restat" typo.  Add full_convert.
!   05-OCT-1993	V1.2-1	Madison	    Add link info for OpenVMS AXP.
!--
*/

#include descrip
#include string
#include stdio
#include ssdef
#include str$routines
#include lib$routines

#define NICK_TO_ADDRESS	    1
#define USERNAME_TO_NICK    2

#define NAME_COUNT  	    2

static char *user [] = {"SMYTHE", "SYSTEM"};
static char *nick [] = {"J.Smythe", "System.Manager"};

#define FULL_COUNT  	    2

static char *full_user[] = {"MADISON", "SHANDY_P"};
static char *full_nick[] = {"madison@tgv.com",
    	    	    	 "Peter_Shandy@portulaca-purple-passion.balaclava.edu"};

struct context {
    struct dsc$descriptor localnode;
    };
    

/*
!++
!
! ROUTINE NAME:	    INIT
!
! FUNCTIONAL DESCRIPTION:
!
!   Allocates and initializes context block for subsequent name conversions.
!
! RETURNS:  	cond_value, longword (unsigned), write only, by value
!
! PROTOTYPE:
!
!   INIT  ctxptr
!
! ctxptr:   pointer, longword (unsigned), modify, by reference
!
! IMPLICIT INPUTS:  None.
!
! IMPLICIT OUTPUTS: None.
!
! COMPLETION CODES:
!
!   SS$_NORMAL:	    	normal successful completion.
!
! SIDE EFFECTS:
!
!   None.
!--
*/
unsigned int
init (struct context **ctx) {

    int ctxsize;
    $DESCRIPTOR(mx_node_name,"MX_NODE_NAME");

    ctxsize = sizeof(struct context);
    lib$get_vm (&ctxsize, ctx);
    (*ctx)->localnode.dsc$b_dtype = DSC$K_DTYPE_T;
    (*ctx)->localnode.dsc$b_class = DSC$K_CLASS_D;
    (*ctx)->localnode.dsc$w_length = 0;
    (*ctx)->localnode.dsc$a_pointer = NULL;
    lib$sys_trnlog (&mx_node_name, 0, &(*ctx)->localnode);
    return SS$_NORMAL;
}  /* init */

/*
!++
!
! ROUTINE NAME:	    CONVERT
!
! FUNCTIONAL DESCRIPTION:
!
!   Converts username -> nickname or nickname -> RFC821-address.
!
!   NB: You MUST use STR$ routines to copy result to OUTSTR parameter
!       to ensure proper operation!!!!
!
!       You _may_ safely assume that INSTR is compatible with a DTYPE_T,
!   	CLASS_S (standard fixed-length) string descriptor.
!
! RETURNS:  	cond_value, longword (unsigned), write only, by value
!
! PROTOTYPE:
!
!   CONVERT  ctxptr, code, instr, outstr
!
! ctxptr:   pointer, longword (unsigned), modify, by reference
! code:	    longword_unsigned, longword (unsigned), read only, by reference
! instr:    char_string, character string, read only, by descriptor (fixed)
! outstr:   char_string, character string, write only, by descriptor
!
! IMPLICIT INPUTS:  None.
!
! IMPLICIT OUTPUTS: None.
!
! COMPLETION CODES:
!
!   SS$_NORMAL:	    	normal successful completion.
!
! SIDE EFFECTS:
!
!   None.
!--
*/
unsigned int
convert (struct context **ctx, int *code, struct dsc$descriptor *instr,
    	    struct dsc$descriptor *outstr) {

    struct dsc$descriptor tmp, tmp2;
    size_t count;
    int i, j, retstat;
    $DESCRIPTOR(lbrack, "<");
    $DESCRIPTOR(rbrack, ">");
    $DESCRIPTOR(atsign, "@");

    count = instr -> dsc$w_length;
    tmp.dsc$b_dtype = DSC$K_DTYPE_T;
    tmp.dsc$b_class = DSC$K_CLASS_D;
    tmp.dsc$w_length = 0;
    tmp.dsc$a_pointer = NULL;

    tmp2.dsc$b_dtype = DSC$K_DTYPE_T;
    tmp2.dsc$b_class = DSC$K_CLASS_S;

    switch (*code) {
/*
!++
!  Local alias -> address
!
!   This code should return a status of SS$_NORMAL if an alias is found,
!   0 otherwise.
!
!   If an alias is found, the resulting string MUST BE IN RFC821 format:
!
!   	    	    	    	<user@host>
!
!   >>>>>> EVEN IF THE ADDRESS IS FOR THE LOCAL HOST (so you have to
!   look up MX_NODE_NAME and tack it on after the translated name,
!   if you're just doing a local-host user directory).
!--
*/
    	case NICK_TO_ADDRESS:
    	    retstat = 0;
    	    str$copy_dx(&tmp, instr);
    	    for (i = 0; i < NAME_COUNT; i++) {
    	    	tmp2.dsc$w_length = strlen(nick[i]);
    	    	tmp2.dsc$a_pointer = nick[i];
    	    	if (str$case_blind_compare(instr, &tmp2) == 0) {
    	    	    j = strlen(user[i]);
    	    	    str$copy_r(&tmp, &j, user[i]);
    	    	    str$concat(outstr, &lbrack, &tmp, &atsign,
    	    	    	&(*ctx)->localnode, &rbrack);
    	    	    retstat = SS$_NORMAL;
    	    	    break;
    	    	}
    	    }
    	    break;

/*
!++
!   Username -> Alias
!
!   Return sucess status ONLY if you are actually converting the
!   username to an alias!  Otherwise, return a non-success status code.
!
!   For compatibility with the name_conversion interface prior to
!   MX V3.1, you should copy the input string to the output string
!   when you return a non-success status.
!
!--
*/
    	case USERNAME_TO_NICK:
    	    retstat = 0;
    	    str$copy_dx(outstr, instr);   /* for pre-V3.1 compatibility */
    	    for (i = 0; i < NAME_COUNT; i++) {
    	    	tmp2.dsc$w_length = strlen(user[i]);
    	    	tmp2.dsc$a_pointer = user[i];
    	    	if (str$case_blind_compare(instr, &tmp2) == 0) {
    	    	    j = strlen(nick[i]);
    	    	    str$copy_r(outstr, &j, nick[i]);
    	    	    retstat = SS$_NORMAL;
    	    	    break;
    	    	}
    	    }
    	    break;
    }

    return retstat;

}  /* convert */

/*
!++
!
! ROUTINE NAME:	    FULL_CONVERT
!
! FUNCTIONAL DESCRIPTION:
!
!   Converts username -> alias address (full address substitution)
!
!   Unlike the CONVERT routine, FULL_CONVERT converts a username
!   to a complete RFC822-type address.  You must be running MX V3.1C
!   or later to use this feature.
!
!   NB: You MUST use STR$ routines to copy result to OUTSTR parameter
!       to ensure proper operation!!!!
!
!       You _may_ safely assume that INSTR is compatible with a DTYPE_T,
!   	CLASS_S (standard fixed-length) string descriptor.
!
! RETURNS:  	cond_value, longword (unsigned), write only, by value
!
! PROTOTYPE:
!
!   FULL_CONVERT  ctxptr, code, instr, outstr
!
! ctxptr:   pointer, longword (unsigned), modify, by reference
! code:	    longword_unsigned, longword (unsigned), read only, by reference
! instr:    char_string, character string, read only, by descriptor (fixed)
! outstr:   char_string, character string, write only, by descriptor
!
! IMPLICIT INPUTS:  None.
!
! IMPLICIT OUTPUTS: None.
!
! COMPLETION CODES:
!
!   SS$_NORMAL:	    	normal successful completion.
!
! SIDE EFFECTS:
!
!   None.
!--
*/
unsigned int
full_convert (struct context **ctx, int *code, struct dsc$descriptor *instr,
    	    struct dsc$descriptor *outstr) {

    struct dsc$descriptor tmp, tmp2;
    size_t count;
    int i, j, retstat;
    $DESCRIPTOR(lbrack, "<");
    $DESCRIPTOR(rbrack, ">");
    $DESCRIPTOR(atsign, "@");

    count = instr -> dsc$w_length;
    tmp.dsc$b_dtype = DSC$K_DTYPE_T;
    tmp.dsc$b_class = DSC$K_CLASS_D;
    tmp.dsc$w_length = 0;
    tmp.dsc$a_pointer = NULL;

    tmp2.dsc$b_dtype = DSC$K_DTYPE_T;
    tmp2.dsc$b_class = DSC$K_CLASS_S;

    if (*code != USERNAME_TO_NICK) return 0;

/*
!++
!   Username -> alias (full address conversion)
!
!   Return sucess status ONLY if you are actually converting the
!   username to an alias!  Otherwise, return a non-success status code.
!
!--
*/
    retstat = 0;
    for (i = 0; i < FULL_COUNT; i++) {
    	tmp2.dsc$w_length = strlen(full_user[i]);
    	tmp2.dsc$a_pointer = full_user[i];
    	if (str$case_blind_compare(instr, &tmp2) == 0) {
    	    j = strlen(full_nick[i]);
    	    str$copy_r(outstr, &j, full_nick[i]);
    	    retstat = SS$_NORMAL;
    	    break;
    	    }
    	}

    return retstat;

}  /* full_convert */

/*
!++
!
! ROUTINE NAME:	    CLEANUP
!
! FUNCTIONAL DESCRIPTION:
!
!   Deallocates context block allocated by init routine.
!
! RETURNS:  	cond_value, longword (unsigned), write only, by value
!
! PROTOTYPE:
!
!   CLEANUP  ctxptr
!
! ctxptr:   pointer, longword (unsigned), modify, by reference
!
! IMPLICIT INPUTS:  None.
!
! IMPLICIT OUTPUTS: None.
!
! COMPLETION CODES:
!
!   SS$_NORMAL:	    	normal successful completion.
!
! SIDE EFFECTS:
!
!   None.
!--
*/
unsigned int
cleanup (struct context **ctx) {

    int ctxsize;

    str$free1_dx(&(*ctx)->localnode);
    ctxsize = sizeof(struct context);
    lib$free_vm (&ctxsize, ctx);
    *ctx = NULL;
    return SS$_NORMAL;
}  /* cleanup */
