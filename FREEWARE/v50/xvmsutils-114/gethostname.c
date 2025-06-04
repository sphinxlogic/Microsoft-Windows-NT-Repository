/*
 *	gethostname(2) - VMS Version of the UNIX routine
 *
 *	This routine simply translates the system logical
 *	name SYS$NODE and returns the result. This does not
 *	exactly match UNIX behaviour. In particular, the
 *	double colon at the end of the node name may cause
 *	problems in ported code.
 */

#include <ssdef.h>
#include <lnmdef.h>
#include <descrip.h>
#include <errno.h>
#include <starlet.h>

#define	NULL	0L


int x11$gethostname(name, namelen)		/*--- Get node name ---*/
char name[];
int  namelen;
{
    /* Logical name to translate */
    static char logical[] = "SYS$NODE";
    $DESCRIPTOR(logical_dsc, logical);

    /* These descriptors is necessary to select the logical name table */
    static char sys_log_table[] = "LNM$SYSTEM_TABLE";
    $DESCRIPTOR(sys_log_table_dsc, sys_log_table);

    /* Item list as used by SYS$TRNLNM() */
    struct ITEM_LIST
    {
	unsigned short buflen;
	unsigned short code;
	int buffaddr;
	int retaddr;
    };

    struct ITEM_LIST log_list[2];

    unsigned short retlen;		/* Must be a WORD */

    /* 
	Buffer for the translation

	Max return length from SYS$TRNLNM is 255,
	but add a couple to be safe.
    */
    char string[257];

    int status, i;


    if (name == NULL)
    {
	errno = EFAULT;
	return (-1);
    }

    /* Setup the required item list for SYS$TRNLNM */

    log_list[0].buflen = 255;
    log_list[0].code   = LNM$_STRING;
    log_list[0].buffaddr = (int) &string;
    log_list[0].retaddr  = (int) &retlen;
    log_list[1].buflen = 0;
    log_list[1].code   = 0;

    /* Time to look for a logical name ... */

    status = sys$trnlnm(0, &sys_log_table_dsc,
		    &logical_dsc, 0, &log_list);

    if (status == SS$_NORMAL)
    {
	/* Got a translation */
	/* Copy the name back to the calling program */
	for (i=0; i<retlen && i<namelen; i++)
	{
	    name[i] = string[i];
	}
	if (i < namelen)
	    name[i] = 0;		/* Append a null */

	return (0);

    }
    else
    {
	errno = EFAULT;
	vaxc$errno = status;
	return (-1);
    }
}
