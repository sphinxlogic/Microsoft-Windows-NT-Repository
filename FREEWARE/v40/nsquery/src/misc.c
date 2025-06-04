/*
**++
**  FACILITY:	NSQUERY
**
**  ABSTRACT:	Miscellaneous routines
**
**  MODULE DESCRIPTION:
**
**  	Various utility routines needed by various other NSQUERY
**  modules.
**
**  AUTHOR: 	    M. Madison
**  	    	    COPYRIGHT © 1993, MADGOAT SOFTWARE. ALL RIGHTS RESERVED.
**
**  CREATION DATE:  05-SEP-1992
**
**  MODIFICATION HISTORY:
**
**  	05-SEP-1992 V1.0    Madison 	Initial coding.
**  	28-SEP-1992 V1.1    Madison 	Add address_to_string
**--
*/
#include "nsquery.h"
#ifdef __GNUC__
#include <vms/jpidef.h>
#include <vms/lnmdef.h>
#include <vms/psldef.h>
#else
#include <jpidef.h>
#include <lnmdef.h>
#include <psldef.h>
#include <smg$routines.h>
#endif
#include <varargs.h>

/*
**  Forward declarations
*/
    void upcase(char *);
    unsigned int cli_get_value(char *, char *, int);
    unsigned int cli_present(char *);
    unsigned int get_logical(char *, char *, int);
    struct dsc$descriptor *address_to_string(unsigned int);
    void Print();

/*
**++
**  ROUTINE:	upcase
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Converts a string to upper case.
**
**  RETURNS:	void
**
**  PROTOTYPE:
**
**  	upcase(char *str)
**
**  str:    character string, modify, by reference (ASCIZ)
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
void upcase(char *s) {

    register char *cp;

    for (cp = s; *cp; cp++) if (islower(*cp)) *cp = toupper(*cp);

} /* upcase */

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
unsigned int cli_get_value(char *argname, char *arg, int argsize) {

    struct dsc$descriptor argnamd, argd;
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
unsigned int cli_present(char *argname) {

    struct dsc$descriptor argnamd;

    INIT_SDESC(argnamd, strlen(argname), argname);
    return cli$present(&argnamd);
}

/*
**++
**  ROUTINE:	get_logical
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Translates a logical name using a standard LNM$FILE_DEV search.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	get_logical(char *lnm, char *str)
**
**  lnm:    logical_name, read only, by reference (ASCIZ string)
**  str:    character string, write only, by reference (ASCIZ string)
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	SS$_NORMAL: 	Normal successful completion.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int get_logical(char *lnm, char *str, int str_size) {

    ITMLST lnmlst[2];
    struct dsc$descriptor lnmdsc;
    short slen;
    unsigned int status;
    static unsigned int attr=LNM$M_CASE_BLIND;
    static $DESCRIPTOR(tabnam, "LNM$FILE_DEV");

    ITMLST_INIT(lnmlst[0], LNM$_STRING, str_size-1, str, &slen);
    ITMLST_INIT(lnmlst[1], 0, 0, 0, 0);
    INIT_SDESC(lnmdsc, strlen(lnm), lnm);
    status = sys$trnlnm(&attr, &tabnam, &lnmdsc, 0, lnmlst);
    if (OK(status)) *(str+slen) = '\0';
    return status;

} /* get_logical */

/*
**++
**  ROUTINE:	address_to_string
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Takes an IP address and formats it into an ASCIZ string.
**
**  RETURNS:	char_string, write only, by descriptor
**
**  PROTOTYPE:
**
**  	address_to_string(unsigned int addr)
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
struct dsc$descriptor *address_to_string(unsigned int addr) {

    static char buf[32];
    static $DESCRIPTOR(ctrstr,"!UB.!UB.!UB.!UB");
    static struct dsc$descriptor bufdsc = {0, DSC$K_DTYPE_T, DSC$K_CLASS_S, buf};
    short buflen;

    buflen = 0;
    bufdsc.dsc$w_length = sizeof(buf);
    sys$fao(&ctrstr, &buflen, &bufdsc, addr&0xff, (addr>>8)&0xff,
    	    	(addr>>16)&0xff, (addr>>24)&0xff);
    bufdsc.dsc$w_length = buflen;

    return &bufdsc;

} /* address_to_string */

/*
**++
**  ROUTINE:	Print
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Print routine with FAO formatting.
**
**  RETURNS:	void
**
**  PROTOTYPE:
**
**  	Print(char *string,...)
**
**  string: ASCIZ_string, read only, by reference
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
void Print(va_alist)
va_dcl
{
    int argcnt;
    va_list ap;
    char *ctrstr;
    struct dsc$descriptor dsc, bdsc;
    char buf[512];
    unsigned short buflen;
    unsigned int arglst[32], status;
    int i;

    va_start(ap);
    va_count(argcnt);
    ctrstr = va_arg(ap, char *);
    INIT_SDESC(dsc, strlen(ctrstr), ctrstr);
    if (argcnt == 1) {
    	lib$put_output(&dsc);
    } else {
    	INIT_SDESC(bdsc, sizeof(buf), buf);
    	if (argcnt > 32) argcnt = 32;
    	for (i = 0; i < argcnt-1; i++) arglst[i] = va_arg(ap, unsigned int);
    	status = sys$faol(&dsc, &buflen, &bdsc, arglst);
    	if (OK(status)) {
    	    bdsc.dsc$w_length = buflen;
    	    lib$put_output(&bdsc);
    	}
    }

} /* Print */
