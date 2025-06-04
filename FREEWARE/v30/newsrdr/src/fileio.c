/*
**++
**  FACILITY:	NEWSRDR
**
**  ABSTRACT:	File I/O routines
**
**  MODULE DESCRIPTION:
**
**  	This module contains RMS interface routines used by NEWSRDR.
**
**  AUTHOR: 	    M. Madison
**  	    	    COPYRIGHT © 1992, MADGOAT SOFTWARE.  ALL RIGHTS RESERVED.
**
**  CREATION DATE:  08-SEP-1992
**
**  MODIFICATION HISTORY:
**
**  	08-SEP-1992 V1.0    Madison 	Initial coding.
**  	22-SEP-1992 V1.0-1  Madison 	Fix Copy_File buffer length problem.
**  	23-MAR-1993 V1.0-2  Madison 	Fix file_read buf len problem.
**--
*/
#define MODULE_FILEIO
#include "newsrdr.h"
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
    	char espec[256], rspec[256];
    };

/*
** Forward declarations
*/

    unsigned int file_create(char *, struct UNIT **, char *, char *);
    unsigned int file_open(char *, struct UNIT **, char *, char *, int *);
    unsigned int file_append(char *, struct UNIT **, char *, char *, int *);
    unsigned int file_find(char *, char *, char *, char *);
    unsigned int file_fidopen(char *, struct UNIT **);
    unsigned int file_exists(char *, char *);
    unsigned int file_close(struct UNIT *);
    unsigned int file_dclose(struct UNIT *);
    unsigned int file_read(struct UNIT *, char *, int, int *);
    unsigned int file_write(struct UNIT *, char *, int);
    unsigned int file_getpos(struct UNIT *, short[3]);
    unsigned int file_setpos(struct UNIT *, short[3]);
    unsigned int file_get_rdt(char *, TIME *);
    unsigned int file_delete(char *);


/*
**++
**  ROUTINE:	file_create
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Creates a new file, open for writing.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	file_create (fspec, unit)
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
unsigned int file_create(char *fspec, struct UNIT **upp, char *defspec,
    	    	    	    	    	char *rspec) {

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
    u->fab.fab$w_mrs = 0;
    u->nam.nam$l_esa = u->espec;
    u->nam.nam$b_ess = sizeof(u->espec)-1;
    u->nam.nam$l_rsa = u->rspec;
    u->nam.nam$b_rss = sizeof(u->rspec)-1;
    u->fab.fab$l_nam = &(u->nam);
    u->rab.rab$l_fab = &(u->fab);
    u->rab.rab$l_rop |= RAB$M_WBH;
    status = sys$create(&(u->fab),0,0);
    if ($VMS_STATUS_SUCCESS(status)) {
    	u->rspec[u->nam.nam$b_rsl] = '\0';
    	status = sys$connect(&(u->rab),0,0);
    	if ($VMS_STATUS_SUCCESS(status)) {
    	    *upp = u;
    	    if (rspec) strcpy(rspec, u->rspec);
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
**  	file_open(fspec, unit, [defspec], [resspec])
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
    u->nam.nam$b_ess = sizeof(u->espec)-1;
    u->nam.nam$l_rsa = u->rspec;
    u->nam.nam$b_rss = sizeof(u->rspec)-1;
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
**  ROUTINE:	file_append
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Opens an existing file for reading.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	file_append(fspec, unit, [defspec], [resspec])
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
unsigned int file_append(char *fspec, struct UNIT **upp,
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
    u->fab.fab$b_fac = FAB$M_PUT;
    u->fab.fab$l_xab = (char *) &(u->xabfhc);
    u->nam.nam$l_esa = u->espec;
    u->nam.nam$b_ess = sizeof(u->espec)-1;
    u->nam.nam$l_rsa = u->rspec;
    u->nam.nam$b_rss = sizeof(u->rspec)-1;
    u->fab.fab$l_nam = &(u->nam);
    u->rab.rab$l_fab = &(u->fab);
    u->rab.rab$l_rop |= RAB$M_RAH | RAB$M_EOF;
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

} /* file_append */

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
**  	file_open(fspec, unit, [defspec], [resspec])
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
unsigned int file_find(char *fspec, char *defspec, char *xrspec, char *fid) {

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
**  	file_fidopen (fileid, unit, actual_filename, actual_filename_size)
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
unsigned int file_fidopen(char *fileid, struct UNIT **upp) {

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
    	    *upp = u;
    	    return status;
    	};
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
unsigned int file_exists(char *fspec, char *defspec) {

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
    nam.nam$l_esa = espec;
    nam.nam$b_ess = sizeof(espec)-1;
    nam.nam$l_rsa = rspec;
    nam.nam$b_rss = sizeof(rspec)-1;
    fab.fab$l_nam = &nam;
    status = sys$parse(&fab,0,0);
    if ($VMS_STATUS_SUCCESS(status)) {
    	if ((nam.nam$l_fnb) & NAM$M_WILDCARD) return RMS$_FNM;
    	status = sys$search(&fab,0,0);
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
unsigned int file_close(struct UNIT *u) {

    unsigned int status;

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
unsigned int file_dclose(struct UNIT *u) {

    unsigned int status;

    if (!(u->fab.fab$b_fac & FAB$M_PUT)) return SS$_BADPARAM;

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
unsigned int file_read(struct UNIT *u, char *buf, int buf_size, int *retlen) {

    unsigned int status;

/*
**  N.B.: buf_size is assumed to be the size of the read buffer minus 1
**  	  extra byte for the null terminator.
*/

    u->rab.rab$l_ubf = buf;
    u->rab.rab$w_usz = buf_size;
    status = sys$get(&(u->rab),0,0);
    u->rab.rab$b_rac = RAB$C_SEQ;
    if ($VMS_STATUS_SUCCESS(status)) {
    	*retlen = u->rab.rab$w_rsz;
    	*(buf+(*retlen)) = 0;
    }
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
unsigned int file_getpos(struct UNIT *u, short rfa[3]) {

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
unsigned int file_setpos(struct UNIT *u, short rfa[3]) {

    unsigned int status;

    if (rfa == (short *) 0) return sys$rewind(&(u->rab),0,0);
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
**  ROUTINE:	Copy_File
**
**  FUNCTIONAL DESCRIPTION:
**
**  	tbs
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
unsigned int Copy_File(char *infile, char *outfile, char *rspec, int append) {

    struct UNIT *inu, *outu;
    unsigned int status;
    int inmax, len;
    char *buf;

    status = file_open(infile, &inu, 0, 0, &inmax);
    if (!OK(status)) return status;
    if (append) {
    	status = file_append(outfile, &outu, 0, rspec, 0);
    } else {
    	status = file_create(outfile, &outu, 0, rspec);
    }
    if (!OK(status)) {
    	file_close(inu);
    	return status;
    }
    buf = malloc(inmax+1);
    if (!buf) {
    	if (append) {
    	    file_close(outu);
    	} else {
    	    file_dclose(outu);
    	}
    	file_close(inu);
    	return SS$_INSFMEM;
    }
    while (OK((status = file_read(inu, buf, inmax, &len)))) {
    	status = file_write(outu, buf, len);
    	if (!OK(status)) break;
    }
    free(buf);
    if (status != RMS$_EOF) {
    	if (append) {
    	    file_close(outu);
    	} else {
    	    file_dclose(outu);
    	}
    	file_close(inu);
    	return status;
    }
    file_close(inu);
    file_close(outu);
    return SS$_NORMAL;

} /* Copy_File */

/*
**++
**  ROUTINE:	file_delete
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Deletes a file.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	file_open(fspec, unit, [defspec], [resspec])
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
unsigned int file_delete(char *fspec) {

    struct FAB fab;
    struct NAM nam;
    char espec[255], rspec[255];
    unsigned int status;

    fab = cc$rms_fab;
    nam = cc$rms_nam;
    fab.fab$l_fna = fspec;
    fab.fab$b_fns = strlen(fspec);
    fab.fab$b_fac = FAB$M_GET;
    fab.fab$l_nam = &nam;
    nam.nam$l_esa = espec;
    nam.nam$b_ess = sizeof(espec)-1;
    nam.nam$l_rsa = rspec;
    nam.nam$b_rss = sizeof(rspec)-1;
    status = sys$parse(&fab, 0, 0);
    if ($VMS_STATUS_SUCCESS(status)) {
    	status = sys$search(&fab, 0, 0);
    	if ($VMS_STATUS_SUCCESS(status)) {
    	    fab.fab$l_fop |= FAB$M_NAM;
    	    status = sys$erase(&fab, 0, 0);
    	}
    }
    return status;

} /* file_delete */
