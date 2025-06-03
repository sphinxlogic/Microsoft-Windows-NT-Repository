/*
!++
!
! MODULE:   	    ADDRESS_CONVERSION
!
! FACILITY: 	    NEWSRDR (sample source)
!
! ABSTRACT: 	    Example of site-installable address conversion.
!
! MODULE DESCRIPTION:
!
!   This module contains routines for use by NEWSRDR for converting
!   RFC822-type addresses into a format acceptable to the local
!   network mail handler.
!
!   This module simply takes the address passed in, encloses it in
!   quotation marks, and tacks SMTP% on the front.  A routine
!   called "reverse_domain" is also included, which can be used
!   by sites whose mail handlers use backwards ordering of domain
!   parts.
!
!   To use this module: MODIFY IT AS NEEDED FOR YOUR SITE, then compile it
!   and link it with the commands:
!
!       $ cc address_conversion
!   	$ link/share/notrace address_conversion,sys$input:/opt
!   	    sys$share:vaxcrtl/share
!   	    universal=init,convert,cleanup
!   	    <ctrl/Z>
!
!   Then copy it to SYS$SHARE and make it available with the commands:
!
!   	$ copy address_conversion.exe sys$common:[syslib]/protection=w:re
!   	$ install create sys$share:address_conversion/share/open/header
!   	$ define/system/exec newsrdr_address_conversion sys$share:address_conversion
!
!   (You need a suitably privileged account to do this.)
!
! AUTHOR:   	    M. Madison
!   	    	    COPYRIGHT © 1992, MADGOAT SOFTWARE.
!
!   THIS SOFTWARE IS PROVIDED "AS IS".  NEITHER THE AUTHOR NOR RENSSELAER
!   MAKE ANY GUARANTEES REGARDING THE SUITABILITY, RELIABILITY, SECURITY,
!   USEFULNESS, OR PERFORMANCE OF THIS SOFTWARE.  >>USE AT YOUR OWN RISK.
!
! CREATION DATE:    12-FEB-1991
!
! MODIFICATION HISTORY:
!
!   12-FEB-1991	V1.0	Madison	    Initial coding.
!--
*/

#include descrip
#include string
#include stdio
#include ssdef
#include str$routines
#include lib$routines

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
!   Allocates and initializes context block for subsequent address
!   conversions.
!
!   Note that we don't actually use a context block here, so this
!   just returns normal status.
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
init(struct context **ctx) {

    return SS$_NORMAL;
}  /* init */

/*
!++
!
! ROUTINE NAME:	    CONVERT
!
! FUNCTIONAL DESCRIPTION:
!
!   Converts RFC822 address into something VMS Mail (and the "foreign
!   protocol" handler for network mail) will understand.
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
!   CONVERT  ctxptr, instr, outstr
!
! ctxptr:   pointer, longword (unsigned), modify, by reference
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
convert(struct context **ctx, struct dsc$descriptor *instr,
    	    struct dsc$descriptor *outstr) {

/*
!++
!  Use the following with reverse_domain
!
!  void reverse_domain(struct dsc$descriptor *, struct dsc$descriptor *);
!  struct dsc$descriptor tmp;
!  int status;
!--
*/

    $DESCRIPTOR(prefix, "SMTP%\"");  /* alter as needed for your site */
    $DESCRIPTOR(suffix, "\"");

/*
!++
!   The following is what NEWSRDR normally does with the NEWSRDR_MAIL_PROTOCOL
!   logical name, and is provided here only for illustration.
!--
--*/
    return str$concat(outstr, &prefix, instr, &suffix);

/*
!++
!   Use the following when using reverse_domain:
!
!   tmp.dsc$b_dtype = DSC$K_DTYPE_T;
!   tmp.dsc$b_class = DSC$K_CLASS_D;
!   tmp.dsc$w_length = 0;
!   tmp.dsc$a_pointer = NULL;
!   reverse_domain(instr, &tmp);
!   status = str$concat(outstr, &prefix, &tmp, &suffix);
!   str$free1_dx(&tmp);
!   return status;
!
!--
*/

}  /* convert */

/*
!++
!
! ROUTINE NAME:	    CLEANUP
!
! FUNCTIONAL DESCRIPTION:
!
!   Deallocates context block allocated by init routine.
!
!   Note that we don't actually use a context block, so this just returns
!   normal status.
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
cleanup(struct context **ctx) {

    return SS$_NORMAL;
}  /* cleanup */

/*
!++
!
! ROUTINE NAME:	    REVERSE_DOMAIN
!
! FUNCTIONAL DESCRIPTION:
!
!   Reverses the parts of a domain name.
!
!   Examples:  user@host.domain.edu -> user@edu.domain.host
!              user@host.domain.ac.uk -> user@uk.ac.domain.host
!
! RETURNS:  	cond_value, longword (unsigned), write only, by value
!
! PROTOTYPE:
!
!   CLEANUP  in, out
!
! in:	char_string, character string, read only, by descriptor
! out:	char_string, character string, write only, by descriptor
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
void
reverse_domain(struct dsc$descriptor *in, struct dsc$descriptor *out) {

    char *cp, *placeholder;
    struct dsc$descriptor tmp, tmp2;
    $DESCRIPTOR(dot, ".");

    tmp.dsc$b_dtype = DSC$K_DTYPE_T;
    tmp.dsc$b_class = DSC$K_CLASS_D;
    tmp.dsc$w_length = 0;
    tmp.dsc$a_pointer = NULL;

    tmp2.dsc$b_dtype = DSC$K_DTYPE_T;
    tmp2.dsc$b_class = DSC$K_CLASS_S;

    placeholder = in->dsc$a_pointer + in->dsc$w_length;
    for (cp = placeholder-1; cp > in->dsc$a_pointer; cp--) {
    	switch (*cp) {

    	    case '.' :
    	    	tmp2.dsc$w_length = placeholder-cp-1;
    	    	if (tmp2.dsc$w_length > 0) {
    	    	    tmp2.dsc$a_pointer = cp+1;
    	    	    str$append(&tmp, &tmp2);
    	    	    str$append(&tmp, &dot);
    	    	    }
    	    	placeholder = cp;
    	    	break;

    	    case '@' :
    	    	tmp2.dsc$w_length = placeholder-cp-1;
    	    	if (tmp2.dsc$w_length > 0) {
    	    	    tmp2.dsc$a_pointer = cp+1;
    	    	    str$append(&tmp, &tmp2);
    	    	    }
    	    	tmp2.dsc$w_length = cp-(in->dsc$a_pointer)+1;
    	    	tmp2.dsc$a_pointer = in->dsc$a_pointer;
    	    	str$concat(out, &tmp2, &tmp);
    	    	str$free1_dx(&tmp);
    	    	return;
    	    }
    	}

    /* ?? this shouldn't happen ?? */

    str$free1_dx(&tmp);
    str$copy_dx(out,in);  

}  /* reverse_domain */
