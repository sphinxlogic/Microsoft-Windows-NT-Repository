/*	trnlnm.c
 *	V1.0			19-Oct-1994	IfN/Mey
 *				03-Dec-1998	J. Malmberg
 *				    *		Adjust to compile with ANSI C
 *				12-Dec-1999	J. Malmberg
 *				    *		Need <string.h>.  DECC will not
 *						let unsigned and signed mix.
 *+
 * Logical name translation
 *-
 */

#include <stdio.h>
#include <errno.h>
#include <descrip.h>
#include <lnmdef.h>
#include <ssdef.h>
#include <ctype.h>
#include <string.h>

#include "[-]socketprv.h"

struct ITEM_LIST {
	unsigned short itm$w_length;
	unsigned short itm$w_itmcode;
	char * itm$a_bufaddr;
	unsigned short * itm$a_retlen;
};

 /* Prototype for sys$trnlnm() */
/*----------------------------*/
unsigned long sys$trnlnm
       (const unsigned long * attr,
	const struct dsc$descriptor_s * tabnam,
	const struct dsc$descriptor_s * lognam,
	const unsigned char * acmode,
	const struct ITEM_LIST * itmlst);
/*
 * sys$trnlnm() is declared in starlet.h with "unknown params" because it is
 * technically a varargs function, so there's no type-checking for it.
 */

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * int __trnlnm(char *table, char *name, char *mode, char *buff, int len)
 *
 * Description:
 *	Translate a logical name.  Terminate the output buffer with a null
 *	character.
 *
 * table - pointer to string that describes the logical name table to search
 * name  - pointer to character string of logical name to search for
 * mode  - mimimum access mode of logical name (kernel, exec, super, user)
 * buff  - pointer to buffer that is to receive the translation
 * len   - size of buff
 *
 * Returns:
 *	0 on success.  -1 on error.  vaxc$errno is set to status of
 * sys$trnlnm service call and errno is set to EVMSERR.
 */

int __trnlnm(
char	*table,	/* table name */
char	*name,	/* logical name */
char	*mode,	/* access mode */
char	*buff,	/* return buffer */
int	len)	/* size of return buffer */
{
unsigned long attr;
unsigned char *acmode;
struct	dsc$descriptor_s tabnam, lognam, eqvstr;
struct	ITEM_LIST itmlst[2];

/* definitions for different access modes
 * See STARLET.REQ $PSLDEF
 */
unsigned char	PSL$C_KERNEL = 0;
unsigned char	PSL$C_EXEC   = 1;
unsigned char	PSL$C_SUPER  = 2;
unsigned char	PSL$C_USER   = 3;

    tabnam.dsc$b_dtype	= DSC$K_DTYPE_T;
    tabnam.dsc$b_class	= DSC$K_CLASS_S;
    tabnam.dsc$a_pointer= table;
    tabnam.dsc$w_length	= strlen(table);

    lognam.dsc$b_dtype	= DSC$K_DTYPE_T;
    lognam.dsc$b_class	= DSC$K_CLASS_S;
    lognam.dsc$a_pointer= name;
    lognam.dsc$w_length	= strlen(name);

    eqvstr.dsc$b_dtype	= DSC$K_DTYPE_T;
    eqvstr.dsc$b_class	= DSC$K_CLASS_S;
    eqvstr.dsc$a_pointer= buff;
    eqvstr.dsc$w_length	= len;

    itmlst[0].itm$w_length = len;
    itmlst[0].itm$w_itmcode= LNM$_STRING;
    itmlst[0].itm$a_bufaddr= eqvstr.dsc$a_pointer;
    itmlst[0].itm$a_retlen = &eqvstr.dsc$w_length;

    itmlst[1].itm$w_length = 0;
    itmlst[1].itm$w_itmcode= 0;
    itmlst[1].itm$a_bufaddr= 0;
    itmlst[1].itm$a_retlen = 0;

    attr = LNM$M_CASE_BLIND;


    switch (toupper(mode[0])) {
	case 'K':
	    acmode = &PSL$C_KERNEL;
	    break;
	case 'E':
	    acmode = &PSL$C_EXEC;
	    break;
	case 'S':
	    acmode = &PSL$C_SUPER;
	    break;
	case 'U':
	    acmode = &PSL$C_USER;
	    break;
	default :
	    acmode = NULL;
    }

    vaxc$errno = sys$trnlnm( &attr, &tabnam, &lognam, acmode, itmlst );

    if(vaxc$errno == SS$_NORMAL) {
	buff[eqvstr.dsc$w_length] = 0;
	return(0);
    }
    else {
	errno = EVMSERR;
	return(-1);
    }
}
