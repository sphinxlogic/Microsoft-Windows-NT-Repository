/*
**++
**  FACILITY:	MMK
**
**  ABSTRACT:	File I/O routines
**
**  MODULE DESCRIPTION:
**
**  	General file I/O routines used by MMK.
**
**  AUTHOR: 	    M. Madison
**  	    	    COPYRIGHT � 1992, 1993  MADGOAT SOFTWARE. 
**  	    	    ALL RIGHTS RESERVED.
**
**  CREATION DATE:  20-AUG-1992
**
**  MODIFICATION HISTORY:
**
**  	20-AUG-1992 V1.0    Madison 	Initial coding.
**  	29-SEP-1992 V1.1    Madison 	Add defspec to file_create.
**  	09-APR-1993 V1.1-1  Madison 	Comments.
**  	27-SEP-1993 V1.2    Madison 	Add file_create_share.
**  	27-SEP-1993 V1.2-1  Madison 	Fix up file_get_rdt to also do shared open.
**  	27-SEP-1993 V1.2-2  Madison 	Retract V1.2-1.
**  	02-DEC-1994 V1.3    Madison 	Add file_get_filespec.
**--
*/
#ifdef __DECC
#pragma module FILEIO "V1.3"
#else
#ifndef __GNUC__
#module FILEIO "V1.3"
#endif
#endif
#include "mmk.h"
#include <rms.h>
#ifdef __DECC
#include <starlet.h>
#endif

/*
** Context structure used by most of these routines
*/
    struct UNIT {
    	struct FAB fab;
    	struct RAB rab;
    	struct NAM nam;
    	struct XABFHC xabfhc;
    	char espec[255], rspec[255];
    };
/*
** Forward declarations
*/

    unsigned int file_create(char *, struct UNIT **, char *);
    unsigned int file_create_share(char *, struct UNIT **, char *);
    static unsigned int file_create_common(char *, struct UNIT **, char *, int);
    unsigned int file_open(char *, struct UNIT **, char *, char *, int *);
    unsigned int file_find(char *, char *, char *, unsigned char *);
    unsigned int file_fidopen(unsigned char *, struct UNIT **);
    unsigned int file_exists(char *, char *);
    unsigned int file_close(struct UNIT *);
    unsigned int file_dclose(struct UNIT *);
    unsigned int file_read(struct UNIT *, char *, int, int *);
    unsigned int file_write(struct UNIT *, char *, int);
    unsigned int file_getpos(struct UNIT *, unsigned short[3]);
    unsigned int file_setpos(struct UNIT *, unsigned short[3]);
    unsigned int file_get_rdt(char *, TIME *);
    unsigned int file_get_filespec(struct UNIT *, char *, int);


/*
**++
**  ROUTINE:	file_create
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Create a file.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	file_create (fspec, unit, [defspec])
**
**  fspec:  	ASCIZ_string, read only, by reference
**  unit:   	ctxptr, unsigned longword, write only, by reference
**  defspec:	ASCIZ_string, read only, by reference
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
unsigned int file_create(char *fspec, struct UNIT **upp, char *defspec) {

    return file_create_common(fspec, upp, defspec, 0);

} /* file_create */

/*
**++
**  ROUTINE:	file_create_share
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Create a file, opened for shared write.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	file_create_share (fspec, unit, [defspec])
**
**  fspec:  	ASCIZ_string, read only, by reference
**  unit:   	ctxptr, unsigned longword, write only, by reference
**  defspec:	ASCIZ_string, read only, by reference
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
unsigned int file_create_share(char *fspec, struct UNIT **upp, char *defspec) {

    return file_create_common(fspec, upp, defspec, 1);

} /* file_create_share */

/*
**++   
**  ROUTINE:	file_create_common
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Creates a new file, open for writing.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	file_create (fspec, unit, [defspec])
**
**  fspec:  	ASCIZ_string, read only, by reference
**  unit:   	ctxptr, unsigned longword, write only, by reference
**  defspec:	ASCIZ_string, read only, by reference
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:	Any RMS code.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
static unsigned int file_create_common(char *fspec, struct UNIT **upp, char *defspec, int share) {

    struct UNIT *u;
    unsigned int status;

    u = malloc(sizeof(struct UNIT));
    if (!u) return SS$_INSFMEM;
    u->fab = cc$rms_fab;
    u->rab = cc$rms_rab;
    u->nam = cc$rms_nam;
    u->fab.fab$l_fna = fspec;
    u->fab.fab$b_fns = strlen(fspec);
    if (defspec) {
    	u->fab.fab$l_dna = defspec;
    	u->fab.fab$b_dns = strlen(defspec);
    }
    u->fab.fab$l_fop = FAB$M_SQO | FAB$M_DFW;
    u->fab.fab$b_fac = FAB$M_PUT;
    u->fab.fab$b_rat = FAB$M_CR;
    u->fab.fab$b_rfm = FAB$C_VAR;
    if (share) u->fab.fab$b_shr = FAB$M_PUT | FAB$M_GET;
    u->fab.fab$w_mrs = 0;
    u->nam.nam$l_esa = u->espec;
    u->nam.nam$b_ess = sizeof(u->espec);
    u->nam.nam$l_rsa = u->rspec;
    u->nam.nam$b_rss = sizeof(u->rspec);
    u->fab.fab$l_nam = &(u->nam);
    u->rab.rab$l_fab = &(u->fab);
    u->rab.rab$l_rop |= RAB$M_WBH;
    status = sys$create(&(u->fab),0,0);
    if ($VMS_STATUS_SUCCESS(status)) {
    	status = sys$connect(&(u->rab),0,0);
    	if ($VMS_STATUS_SUCCESS(status)) {
    	    *upp = u;
    	    return status;
    	}
    	u->fab.fab$l_fop |= FAB$M_DLT;
    	sys$close(&(u->fab),0,0);
    }
    free(u);
    return status;

} /* file_create */

/*
**++
**  ROUTINE:	file_open
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Opens an existing file for reading.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	file_open(fspec, unit, [defspec], [resspec], [maxline])
**
**  fspec:  	ASCIZ_string, read only, by reference
**  unit:   	ctxptr, longword (unsigned), write only, by reference
**  defspec:	ASCIZ_string, read only, by reference
**  resspec:	ASCIZ_string (256 byte buffer), write only, by reference
**  maxline:	longword_signed, longword (signed), write only, by reference
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:	Any RMS code.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int file_open(char *fspec, struct UNIT **upp,
    	    	    	    	char *defspec, char *rspec, int *maxline) {

    struct UNIT *u;
    unsigned int status;

    u = malloc(sizeof(struct UNIT));
    if (!u) return SS$_INSFMEM;
    u->fab = cc$rms_fab;
    u->rab = cc$rms_rab;
    u->nam = cc$rms_nam;
    u->xabfhc = cc$rms_xabfhc;
    u->fab.fab$l_fna = fspec;
    u->fab.fab$b_fns = strlen(fspec);
    if (defspec) {
    	u->fab.fab$l_dna = defspec;
    	u->fab.fab$b_dns = strlen(defspec);
    }
    u->fab.fab$b_fac = FAB$M_GET;
    u->fab.fab$l_xab = (char *) &(u->xabfhc);
    u->nam.nam$l_esa = u->espec;
    u->nam.nam$b_ess = sizeof(u->espec);
    u->nam.nam$l_rsa = u->rspec;
    u->nam.nam$b_rss = sizeof(u->rspec);
    u->fab.fab$l_nam = &(u->nam);
    u->rab.rab$l_fab = &(u->fab);
    u->rab.rab$l_rop |= RAB$M_RAH;
    status = sys$open(&(u->fab),0,0);
    if ($VMS_STATUS_SUCCESS(status)) {
    	status = sys$connect(&(u->rab),0,0);
    	if ($VMS_STATUS_SUCCESS(status)) {
    	    if (maxline) *maxline = u->fab.fab$w_mrs == 0 ?
    	    	    (u->xabfhc.xab$w_lrl == 0 ? 32767 : u->xabfhc.xab$w_lrl) :
    	    	    u->fab.fab$w_mrs;
    	    if (rspec) {
    	    	memcpy(rspec,u->rspec,u->nam.nam$b_rsl);
    	    	*(rspec+u->nam.nam$b_rsl) = 0;
    	    }
    	    *upp = u;
    	    return status;
    	}
    	sys$close(&(u->fab),0,0);
    }
    free(u);
    return status;

} /* file_open */

/*
**++
**  ROUTINE:	file_find
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Locates a file, gets its FID.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	file_find(fspec, [defspec], [resspec], unsigned char *fid)
**
**  fspec:  	ASCIZ_string, read only, by reference
**  defspec:	ASCIZ_string, read only, by reference
**  resspec:	ASCIZ_string [256 byte buffer], write only, by reference
**  fid:    	28-byte buffer, write only, by reference
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:	Any from RMS.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int file_find(char *fspec, char *defspec, char *xrspec,
    	    	    	    	    	    	    	unsigned char *fid) {

    struct FAB fab;
    struct NAM nam;
    char espec[256], rspec[256];
    unsigned int status;

    fab = cc$rms_fab;
    nam = cc$rms_nam;
    fab.fab$l_fna = fspec;
    fab.fab$b_fns = strlen(fspec);
    if (defspec) {
    	fab.fab$l_dna = defspec;
    	fab.fab$b_dns = strlen(defspec);
    }
    fab.fab$b_fac = FAB$M_GET;
    nam.nam$l_esa = espec;
    nam.nam$b_ess = sizeof(espec)-1;
    nam.nam$l_rsa = rspec;
    nam.nam$b_rss = sizeof(rspec)-1;
    fab.fab$l_nam = &nam;
    status = sys$parse(&fab, 0, 0);
    if ($VMS_STATUS_SUCCESS(status)) {
    	status = sys$search(&fab, 0, 0);
    	if ($VMS_STATUS_SUCCESS(status)) {
    	    if (xrspec) {
    	    	rspec[nam.nam$b_rsl] = 0;
    	    	strcpy(xrspec, rspec);
    	    }
    	    memcpy(fid, &nam.nam$t_dvi, 28);
    	}
    }
    return status;

} /* file_find */

/*
**++
**  ROUTINE:	file_fidopen
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Opens an existing file by fileid for reading.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	file_fidopen (fileid, unit)
**
**  fileid: 	28-byte buffer, read only, by reference
**  unit:   	ctxptr, longword (unsigned), write only, by reference
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:	Any from RMS.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int file_fidopen(unsigned char *fileid, struct UNIT **upp) {

    struct UNIT *u;
    unsigned int status;

    u = malloc(sizeof(struct UNIT));
    if (!u) return SS$_INSFMEM;
    u->fab = cc$rms_fab;
    u->rab = cc$rms_rab;
    u->nam = cc$rms_nam;
    u->xabfhc = cc$rms_xabfhc;
    u->fab.fab$b_fac = FAB$M_GET;
    u->fab.fab$l_xab = (char *) &(u->xabfhc);
    memcpy(&(u->nam.nam$t_dvi), fileid, 28);
    u->fab.fab$l_fop |= FAB$M_NAM;
    u->fab.fab$l_nam = &(u->nam);
    u->rab.rab$l_fab = &(u->fab);
    u->rab.rab$l_rop |= RAB$M_RAH;
    status = sys$open(&(u->fab),0,0);
    if ($VMS_STATUS_SUCCESS(status)) {
    	status = sys$connect(&(u->rab),0,0);
    	if ($VMS_STATUS_SUCCESS(status)) {
    	    u->rab.rab$w_usz = u->fab.fab$w_mrs == 0 ?
    	    	    (u->xabfhc.xab$w_lrl == 0 ? 32767 : u->xabfhc.xab$w_lrl) :
    	    	    u->fab.fab$w_mrs;
    	    u->rab.rab$l_ubf = malloc(u->rab.rab$w_usz);
    	    if (u->rab.rab$l_ubf) {
    	    	*upp = u;
    	    	return status;
    	    } else status = SS$_INSFMEM;
    	    sys$disconnect(&(u->rab),0,0);
    	}
    	sys$close(&(u->fab),0,0);
    }
    free(u);
    return status;

} /* file_fidopen */

/*
**++
**  ROUTINE:	file_exists
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Opens an existing file for reading.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	file_exists(fspec, [defspec])
**
**  fspec:  	ASCIZ_string, read only, by reference
**  defspec:	ASCIZ_string, read only, by reference
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:	Any from RMS.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int file_exists(char *fspec, char *defspec) {

    struct UNIT unit, *u;
    unsigned int status;

    u = &unit;
    u->fab = cc$rms_fab;
    u->nam = cc$rms_nam;
    u->xabfhc = cc$rms_xabfhc;
    u->fab.fab$l_fna = fspec;
    u->fab.fab$b_fns = strlen(fspec);
    if (defspec) {
    	u->fab.fab$l_dna = defspec;
    	u->fab.fab$b_dns = strlen(defspec);
    }
    u->fab.fab$b_fac = FAB$M_GET;
    u->fab.fab$l_xab = (char *) &(u->xabfhc);
    u->nam.nam$l_esa = u->espec;
    u->nam.nam$b_ess = sizeof(u->espec);
    u->nam.nam$l_rsa = u->rspec;
    u->nam.nam$b_rss = sizeof(u->rspec);
    u->fab.fab$l_nam = &(u->nam);
    status = sys$parse(&(u->fab),0,0);
    if ($VMS_STATUS_SUCCESS(status)) {
    	if ((u->nam.nam$l_fnb) & NAM$M_WILDCARD) return RMS$_FNM;
    	status = sys$search(&(u->fab),0,0);
    }
    return status;

} /* file_exists */

/*
**++
**  ROUTINE:	file_close
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Closes a file opened by file_open or file_create.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	file_close(unit)
**
**  unit:   ctxptr, longword (unsigned), modify, by reference
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:	Any from RMS.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int file_close(struct UNIT *u) {

    unsigned int status;

    if (u->rab.rab$w_usz > 0) free(u->rab.rab$l_ubf);
    sys$disconnect(&(u->rab),0,0);
    status = sys$close(&(u->fab),0,0);
    free(u);
    return status;

}

/*
**++
**  ROUTINE:	file_dclose
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Closes & deletes a file currently open for writing.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	file_dclose(unit)
**
**  unit:   ctxptr, longword (unsigned), read only, by value
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:	Any from RMS.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int file_dclose(struct UNIT *u) {

    unsigned int status;

    if (!(u->fab.fab$b_fac & FAB$M_PUT)) return SS$_BADPARAM;

    if (u->rab.rab$w_usz > 0) free(u->rab.rab$l_ubf);
    sys$disconnect(&(u->rab),0,0);
    u->fab.fab$l_fop |= FAB$M_DLT;
    status = sys$close(&(u->fab),0,0);
    free(u);
    return status;

}

/*
**++
**  ROUTINE:	file_read
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Reads a record from a file.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	file_read(unit, buf, bufsize, retlen)
**
**  unit:   	ctxptr, longword (unsigned), read only, by value
**  buf:    	ASCIZ_string, longword (unsigned), write only, by reference
**  bufsize:	longword_signed, longword (signed), read only, by value
**  retlen: 	longword_signed, longword (signed), write only, by reference
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:	Any from RMS.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int file_read(struct UNIT *u, char *buf, int buf_size, int *retlen) {

    unsigned int status;

    u->rab.rab$l_ubf = buf;
    u->rab.rab$w_usz = buf_size-1;
    status = sys$get(&(u->rab),0,0);
    u->rab.rab$b_rac = RAB$C_SEQ;
    *retlen = u->rab.rab$w_rsz;
    if ($VMS_STATUS_SUCCESS(status)) *(buf+(*retlen)) = 0;
    return status;
}

/*
**++
**  ROUTINE:	file_write
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Writes a record to a file.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	file_write(unit, buf, bufsize)
**
**  unit:   	ctxptr, read only, by value
**  buf:    	user_buffer, read only, by reference
**  bufsize:	longword_signed, longword (signed), read only, by value
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:	Any from RMS.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int file_write(struct UNIT *u, char *buf, int buf_size) {

    u->rab.rab$l_rbf = buf;
    u->rab.rab$w_rsz = buf_size;
    return sys$put(&(u->rab),0,0);

}

/*
**++
**  ROUTINE:	file_getpos
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Returns the RFA of the next available record in the file.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	file_getpos(unit, rfa)
**
**  unit:   	ctxptr, read only, by value
**  rfa:    	3-word buffer, write only, by reference
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:	Any from RMS.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int file_getpos(struct UNIT *u, unsigned short rfa[3]) {

    unsigned int status;

    status = sys$find(&(u->rab),0,0);
    if ($VMS_STATUS_SUCCESS(status))
    	memcpy(rfa, &(u->rab.rab$w_rfa), 6);
    u->rab.rab$b_rac = RAB$C_RFA;
    return status;

} /* file_getpos */

/*
**++
**  ROUTINE:	file_setpos
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Moves the file to the specified RFA.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	file_setpos(unit, rfa)
**
**  unit:   	ctxptr, read only, by value
**  rfa:    	3-word buffer, read only, by reference
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:	Any from RMS.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int file_setpos(struct UNIT *u, unsigned short rfa[3]) {

    unsigned int status;

    if (rfa == (unsigned short *) 0) return sys$rewind(&(u->rab),0,0);
    if (rfa[0] == 0 && rfa[1] == 0 && rfa[2] == 0)
    	return sys$rewind(&(u->rab),0,0);

    u->rab.rab$b_rac = RAB$C_RFA;
    memcpy(&(u->rab.rab$w_rfa), rfa, 6);
    status = sys$find(&(u->rab),0,0);
    return status;

} /* file_setpos */

/*
**++
**  ROUTINE:	file_get_rdt
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Fetches the RDT for a file.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	file_get_rdt(fspec, rdt)
**
**  fspec:  	ASCIZ_string, read only, by reference
**  rdt:    	date_time, quadword (signed), write only, by reference
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:	Any from RMS.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int file_get_rdt(char *fspec, TIME *rdt) {

    struct FAB fab;
    struct XABRDT xabrdt;
    unsigned int status;

    fab = cc$rms_fab;
    xabrdt = cc$rms_xabrdt;
    fab.fab$l_fna = fspec;
    fab.fab$b_fns = strlen(fspec);
    fab.fab$b_fac = FAB$M_GET;
    fab.fab$l_xab = (char *) &xabrdt;
    status = sys$open(&fab,0,0);
    if ($VMS_STATUS_SUCCESS(status)) {
    	memcpy(rdt, &xabrdt.xab$q_rdt, 8);
    	sys$close(&fab,0,0);
    }
    return status;

} /* file_get_rdt */

/*
**++
**  ROUTINE:	file_get_filespec
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Returns the file specification for a unit.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	tbs
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
unsigned int file_get_filespec (struct UNIT *unit, char *fspec, int fspec_size) {

    if (fspec_size > 255)  {
    	strcpy(fspec, unit->rspec);
    } else {
    	int i;
    	i = strlen(unit->rspec);
    	if (i > fspec_size-1) i = fspec_size-1;
    	memcpy(fspec, unit->rspec, i);
    	fspec[i] = '\0';
    }

    return SS$_NORMAL;

} /* file_get_filespec */
