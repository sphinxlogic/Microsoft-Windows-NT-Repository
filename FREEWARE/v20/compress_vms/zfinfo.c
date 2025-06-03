/* Copyright(c) 1986 Association of Universities for Research in Astronomy Inc.
 */

#include <dcdef.h>
#include <descrip.h>
#include <dvidef.h>
#include <jpidef.h>
#include <ssdef.h>

#include "spp.h"
#include "rms.h"
#include "vms.h"
#include "finfo.h"

extern	char	*strchr();


/* ZFINFO -- Get information about a file or device.
 */
zfinfo (osfn, out_struct)
char	*osfn;
struct  _finfo  *out_struct;
{
	struct 	FAB     fab;
	struct 	NAM     nam;
	struct 	XABFHC  xabfhc;
	struct 	XABPRO  xabpro;
	struct 	XABDAT  xabdat;
	char	nam_esa[NAM$C_MAXRSS];
	int	i;

	/* Templates are not allowed.
	 */
	if (strchr (osfn, '*'))
	    return (XERR);

	/* Init FAB and NAM blocks for RMS parsing; every string should
	 * pass the parse okay.
	 */
	fab = cc$rms_fab;
	fab.fab$l_fna = osfn;
	fab.fab$b_fns = strlen (fab.fab$l_fna);
	fab.fab$l_nam = &nam;
	nam = cc$rms_nam;
	nam.nam$l_esa = nam_esa;
	nam.nam$b_ess = NAM$C_MAXRSS;

	if (sys$parse (&fab) != RMS$_NORMAL)
	    return (XERR);

	/* Do an RMS search on the parsed string.  This checks for the 
	 * existence of a file and will work in most cases.  If not, then
	 * we could have a device or a directory (IRAF drops the .DIR
	 * extension).  Use the original 'osfn' in this case, not the output 
	 * of _osfn() or sys$parse().  If the file name and type strings are
	 * zero length we were probably called with a directory name ending
	 * in ] (no file specified); do not call sys$search as this will
	 * succeed if there is a file ".;*" in the directory, causing zinfo
	 * to return not-a-directory when passed a directory name.  Note that
	 * VMS includes 1 for the . in the nam$b_type field.
	 */
	if ((nam.nam$b_name + nam.nam$b_type) <= 1 ||
	    sys$search (&fab) != RMS$_NORMAL) {
	    return (XERR);
	}

	if (nam.nam$b_type == 1)		    	/* null extension */
	    out_struct->fi_type = FI_REGULAR;
	else {
	    if 	    (!strncmp (nam.nam$l_type, ".EXE", nam.nam$b_type))
	    	out_struct->fi_type = FI_EXECUTABLE;
	    else if (!strncmp (nam.nam$l_type, ".DIR", nam.nam$b_type))
	    	out_struct->fi_type = FI_DIRECTORY;
	    else
	    	out_struct->fi_type = FI_REGULAR;
	}

	/* Set up RMS fields and get file info we need.
	 */
	fab.fab$b_fac = FAB$M_GET;
	fab.fab$l_fop |= FAB$M_NAM;
	fab.fab$l_xab = &xabfhc;
	xabfhc = cc$rms_xabfhc;
	xabpro = cc$rms_xabpro;
	xabdat = cc$rms_xabdat;
	xabfhc.xab$l_nxt = &xabpro;
	xabpro.xab$l_nxt = &xabdat;
	xabdat.xab$l_nxt = NULL;

	if (sys$open (&fab) != RMS$_NORMAL || sys$close (&fab) != RMS$_NORMAL)
	    return (XERR);

	/* Calculate file size (in bytes)
	 *   file_size = (block_length) * (last_block-1) + first_free_byte
	 */
	out_struct->fi_size = SZ_DISKBLOCK * (xabfhc.xab$l_ebk - 1) +
	    	    xabfhc.xab$w_ffb;

	/* Get access times, converting from VMS to IRAF time.
	out_struct->fi_ctime = _convtime (&xabdat.xab$q_cdt);
	out_struct->fi_atime = _convtime (&xabdat.xab$q_rdt);
	out_struct->fi_mtime = out_struct->fi_atime;
	 */

	/* Get permission bits
	 */
	i = ~xabpro.xab$w_pro >> 4;
	out_struct->fi_perm =  (i & 03);
	i >>= 2;
	out_struct->fi_perm |= (i & 014);
	i >>= 2;
	out_struct->fi_perm |= (i & 060);

	/* Get owner name.  For VMS, convert the UIC code to a string.
	 */
        _make_uic (xabpro.xab$l_uic, out_struct->fi_owner, SZ_OWNERSTR);
	return (XOK);
}


/* _MAKE_UIC -- Convert the UIC into the string name of the file owner.
 * If the UIC cannot be translated into the owner name, return the UIC
 * as a numeric string instead.
 */
_make_uic (uic, buf, maxch)
unsigned long  uic;
char  *buf;
int   maxch;
{
	if (_uic2uname (uic, buf, maxch) < 0) {
	    buf += _make_octal (buf, (short) (uic >> 16));	/* group */
	    *buf++ = ',';
	    buf += _make_octal (buf, (short) uic);		/* member */
	    *buf = EOS;
	}
}

/* _MAKE_OCTAL -- convert value to ASCII string in octal format
 * (Note: string is NOT null terminated)
 */
static  
_make_octal (buf, val)
char  *buf;
short  val;
{
	int  len, i=2;

	if (val < 010)   
	    i = 0;
	else if (val < 0100)  
	    i = 1;
	len = i + 1;

	for ( ; i >= 0; i--) {
	    buf[i] = (val & 07) + '0';
	    val >>= 3;
	}
	return (len);	    	/* return the length of the string made */
}

/* _UIC2UNAME -- Convert a UIC (user identifier) to the equivalent ASCII 
 * username.  Depends on the "rights database" being set up at the system 
 * level.
 */
static
_uic2uname (uic, uname, maxch)
unsigned long uic;
char	*uname;
int	maxch;
{
	DESCRIPTOR  u;
	short	namelen;
	int     stat;

	_strdesc_l (uname, &u, maxch);

	if (sys$idtoasc (uic, &namelen, &u, 0,0,0) == SS$_NORMAL) {
	    uname[namelen] = EOS;
	    uname;
	    return (0);
	} else
	    return (-1);
}


/* _STRDESC  -- Set up a string descriptor for a C string (required by many 
 * system services and run-time library routines).
 */
_strdesc (str, sptr)
char   *str;
DESCRIPTOR  *sptr;
{
	sptr->dsc$a_pointer = str;
	sptr->dsc$w_length  = strlen (str);
	sptr->dsc$b_dtype   = DSC$K_DTYPE_T;
	sptr->dsc$b_class   = DSC$K_CLASS_S;
}


/* _STRDESC_L  -- _STRDESC with a length parameter to initialize dsc$w_length
 * field.  Used when specifying result buffers for some system services.
 */
_strdesc_l (str, sptr, len)
char   *str;
DESCRIPTOR  *sptr;
int    len;
{
	sptr->dsc$a_pointer = str;
	sptr->dsc$w_length  = len;
	sptr->dsc$b_dtype   = DSC$K_DTYPE_T;
	sptr->dsc$b_class   = DSC$K_CLASS_S;
}
