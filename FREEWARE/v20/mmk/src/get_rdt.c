/*
**++
**  FACILITY:	MMK
**
**  ABSTRACT:	Revision date/time routines
**
**  MODULE DESCRIPTION:
**
**  	This module contains routines that obtain the RDT for
**  MMS objects (mainly for library modules).
**
**  AUTHOR: 	    M. Madison
**  	    	    COPYRIGHT © 1992-1994, MADGOAT SOFTWARE.
**  	    	    ALL RIGHTS RESERVED.
**
**  CREATION DATE:  21-AUG-1992
**
**  MODIFICATION HISTORY:
**
**  	21-AUG-1992 V1.0    Madison 	Initial coding.
**  	27-AUG-1992 V1.1    Madison 	Comments, cleanup.
**  	12-SEP-1992 V1.1-1  Madison 	Cache RDT's.
**  	09-APR-1993 V1.1-2  Madison 	Comments.
**  	01-JUL-1994 V1.2    Madison 	Update for CMS.
**  	12-JUL-1994 V1.2-1  Madison 	Set RDT to zero on failure.
**  	17-JUL-1995 V1.2-2  Madison 	Set have_rdt flag if successful.
**--
*/
#ifdef __DECC
#pragma module GET_RDT "V1.1-2"
#else
#ifndef __GNUC__
#module GET_RDT "V1.1-2"
#endif
#endif
#include "mmk.h"
#include <rms.h>
#include "mhddef.h"
#include <lbrdef.h>

/*
** Forward declarations
*/
    unsigned int lbr_get_rdt(char *, char *, TIME *);
    void lbr_flush(void);

/*
** Context structure and header for the list that tracks them
*/

    struct LBR {
    	struct LBR *flink, *blink;
    	struct NAM nam;
    	unsigned int lbrctx;
    	char espec[256];
    	char rspec[256];
    };

    static struct QUE lbrque = {&lbrque, &lbrque};

/*
** External references
*/
    extern unsigned int file_find(), file_get_rdt();  /* module FILEIO */
    extern unsigned int cms_get_rdt(char *, char *, TIME *); /* module CMS_INTERFACE */

    extern unsigned int lbr$ini_control(), lbr$open(), lbr$lookup_key();
    extern unsigned int lbr$set_module(), lbr$close();

#ifdef __GNUC__
    extern int lbr$_hdrtrunc();
#define LBR$_HDRTRUNC ((unsigned int) lbr$_hdrtrunc)
#else
#pragma nostandard
    globalvalue unsigned int LBR$_HDRTRUNC;
#pragma standard
#endif


/*
**++
**  ROUTINE:	lbr_get_rdt
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Gets the RDT of a library module.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	lbr_get_rdt(char *libfile, char *module, TIME *rdt);
**
**  libfile:	ASCIZ_string, read only, by reference
**  module: 	ASCIZ_string, read only, by reference
**  rdt:    	date_time, quadword (signed), write only, by reference
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:	Any from the LBR$ routines.
**
**  SIDE EFFECTS:   	lbrque modified.
**
**--
*/
unsigned int lbr_get_rdt(char *lib, char *mod, TIME *rdt) {

    unsigned int lbrfunc=LBR$C_READ;
    char real_name[256], fid[28];
    struct dsc$descriptor libdsc, moddsc;
    struct LBR *lbr;
    unsigned int status, len;
    unsigned short rfa[3];
    struct mhddef mhd;

/*
** First look up the library file
*/
    status = file_find(lib, 0, real_name, fid);
    if (!OK(status)) return status;

/*
** Already open?
*/
    for (lbr = (struct LBR *)lbrque.head; lbr != (struct LBR *)&lbrque;
    	    	    lbr = lbr->flink) {
    	if (memcmp(fid, &lbr->nam.nam$t_dvi, 28) == 0) break;
    }

/*
** If not open yet, construct a context block and open it.
*/
    if (lbr == (struct LBR *) &lbrque) {
    	lbr = malloc(sizeof(struct LBR));
    	queue_insert(lbr, lbrque.tail);
    	lbr->lbrctx = 0;
    	lbr->nam = cc$rms_nam;
    	lbr->nam.nam$b_rss = sizeof(lbr->rspec)-1;
    	lbr->nam.nam$b_ess = sizeof(lbr->espec)-1;
    	lbr->nam.nam$l_esa = lbr->espec;
    	lbr->nam.nam$l_rsa = lbr->rspec;
    	status = lbr$ini_control(&lbr->lbrctx, &lbrfunc, 0, &lbr->nam);
    	if (!OK(status)) lib$signal(status);
    	INIT_SDESC(libdsc, strlen(real_name), real_name);
    	status = lbr$open(&lbr->lbrctx, &libdsc);
    	if (!OK(status)) return status;
    }

/*
** Look up the module in question...
*/
    INIT_SDESC(moddsc, strlen(mod), mod);
    status = lbr$lookup_key(&lbr->lbrctx, &moddsc, rfa);
    if (!OK(status)) return status;

/*
**  ... and get the RDT from the module header
*/
    INIT_SDESC(moddsc, sizeof(mhd), &mhd);
    status = lbr$set_module(&lbr->lbrctx, rfa, &moddsc, &len);
    if (!OK(status) && (status != LBR$_HDRTRUNC)) lib$signal(status);
    memcpy(rdt, &mhd.mhd$l_datim, 8);
    return SS$_NORMAL;

}

/*
**++
**  ROUTINE:	lbr_flush
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Closes any libraries we have left open.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	lbr_flush()
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:	Any from LBR$ routines.
**
**  SIDE EFFECTS:   	lbrque modified.
**
**--
*/
void lbr_flush() {

    struct LBR *lbr;

    while (queue_remove(lbrque.head, &lbr)) {
    	lbr$close(&lbr->lbrctx);
    	free(lbr);
    }
}

/*
**++
**  ROUTINE:	get_rdt
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Generic RDT fetch routine for MMS objects.  Fills in the
**  RDT field in the object structure.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	get_rdt(struct OBJECT *obj)
**
**  obj:    OBJECT structure, modify, by reference
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:	Any from file_get_rdt or lbr_get_rdt
**  	    	    	0: object is not "tangible" - no revision date
**
**  SIDE EFFECTS:   	None.
**
**--
*/
unsigned int get_rdt(struct OBJECT *obj) {

    unsigned int status;

    if (obj->have_rdt) return SS$_NORMAL;

    memset(&obj->rdt, 0, sizeof(obj->rdt));

    switch (obj->type) {
    	case MMK_K_OBJ_FILE:
    	case MMK_K_OBJ_LIB:
    	    status = file_get_rdt(obj->name, &obj->rdt);
    	    break;
    	case MMK_K_OBJ_LIBMOD:
       	    status = lbr_get_rdt(obj->libfile->name, obj->name, &obj->rdt);
    	    break;
    	case MMK_K_OBJ_CMSFILE:
    	    status = cms_get_rdt(obj->name, obj->cms_gen, &obj->rdt);
    	    break;
    	default:
    	    status = 0;
    	    break;
    }

    if (OK(status)) obj->have_rdt = 1;

    return status;
}
