/*
**++
**  FACILITY:	MMK
**
**  ABSTRACT:	CMS access routines.
**
**  MODULE DESCRIPTION:
**
**  	This module contains routines for interfacing with DEC/CMS.
**
**  AUTHOR: 	    M. Madison
**  	    	    COPYRIGHT � 1994, MADGOAT SOFTWARE.
**  	    	    ALL RIGHTS RESERVED.
**
**  CREATION DATE:  01-JUL-1994
**
**  MODIFICATION HISTORY:
**
**  	01-JUL-1994 V1.0    Madison 	Initial coding.
**--
*/
#ifdef __DECC
#pragma module CMS_INTERFACE "V1.0"
#else
#ifndef __GNUC__
#module CMS_INTERFACE "V1.0"
#endif
#endif
#include "mmk.h"
#include "globals.h"
#include "cmsdef.h"
#include <fscndef.h>

/*
** Forward declarations
*/
    unsigned int cms_init(void);
    unsigned int cms_get_rdt(char *, char *, TIME *);
    static unsigned int cms_show_gen_callback(int *,
    	struct CMS_LDB *, void *, struct dsc$descriptor **,
    	struct dsc$descriptor **, struct dsc$descriptor **,
    	TIME *, TIME *, TIME *, struct dsc$descriptor **, 
    	struct dsc$descriptor **, int *, int *, int *, int *, int *, int *);
    unsigned int cms_fetch_file(char *);
    unsigned int cms_parse_name(char *, char *, int, int *,
    	    char *, int, int *, int);
    static unsigned int message_routine(unsigned int [], unsigned int [],
    	    	    	    	struct CMS_LDB *);

/*
**  OWN storage
*/
    static int cms_initialized = 0;
    static unsigned int cms$_normal, cms$_exclude, cms$_noref;
    static unsigned int (*set_library)(struct CMS_LDB *,
    	    struct dsc$descriptor *, unsigned int (*)()) = 0;
    static unsigned int (*show_generation)(struct CMS_LDB *, unsigned int (*)(),
    	void *, struct dsc$descriptor *, struct dsc$descriptor *,
    	struct dsc$descriptor *, int *, int *, int *, unsigned int (*)()) = 0;
    static unsigned int (*fetch)(struct CMS_LDB *, struct dsc$descriptor *,
    	struct dsc$descriptor *, struct dsc$descriptor *, struct dsc$descriptor *,
    	int *, int *, int *, int *, struct dsc$descriptor *,
    	unsigned int (*)(), int *, struct dsc$descriptor *,
    	struct dsc$descriptor *, int *);
    static char current_library[256];
    static unsigned short current_liblen = 0;
    static struct CMS_LDB cmsctx;

/*
**  External references
*/
    extern unsigned int find_image_symbol(char *, char *, void *); /* module MISC */
    extern int strneql_case_blind(char *, char *, int);	    	   /* module MISC */

/*
**++
**  ROUTINE:	cms_init
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
unsigned int cms_init(void) {

    unsigned int status;

    if (cms_initialized) return SS$_NORMAL;

    status = find_image_symbol("CMSSHR", "CMS$_NORMAL", &cms$_normal);
    if (!OK(status)) return status;
    status = find_image_symbol("CMSSHR", "CMS$_EXCLUDE", &cms$_exclude);
    if (!OK(status)) return status;
    status = find_image_symbol("CMSSHR", "CMS$_NOREF", &cms$_noref);
    if (!OK(status)) return status;
    status = find_image_symbol("CMSSHR", "CMS$SET_LIBRARY", &set_library);
    if (!OK(status)) return status;
    status = find_image_symbol("CMSSHR", "CMS$SHOW_GENERATION", &show_generation);
    if (!OK(status)) return status;
    status = find_image_symbol("CMSSHR", "CMS$FETCH", &fetch);
    if (!OK(status)) return status;
    current_library[0] = '\0';
    memset(&cmsctx, 0, sizeof(cmsctx));

    cms_initialized = 1;

    return SS$_NORMAL;

} /* cms_init */

/*
**++
**  ROUTINE:	cms_get_rdt
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
unsigned int cms_get_rdt(char *fspec, char *generation, TIME *rdt) {

    char libname[256];
    char element[256];
    unsigned int status;
    int llen, elen;
    struct dsc$descriptor dsc, gdsc;

    status = cms_parse_name(fspec, libname, sizeof(libname), &llen,
    	    	    	    element, sizeof(element), &elen, 1);
    if (!OK(status)) return status;

    if (!cms_initialized) {
    	status = cms_init();
    	if (!OK(status)) return status;
    }

    if (current_liblen != llen ||
    	    !strneql_case_blind(libname, current_library, llen)) {
    	INIT_SDESC(dsc, llen, libname);
    	if (llen == 0) return cms$_noref;
    	status = (*set_library)(&cmsctx, &dsc, message_routine);
    	if (!OK(status)) {
    	    if (current_liblen > 0) {
    	    	INIT_SDESC(dsc, current_liblen, current_library);
    	    	(*set_library)(&cmsctx, &dsc, message_routine);
    	    }
    	    return status;
    	}
    	current_liblen = llen;
    	memcpy(current_library, libname, llen);
    	current_library[llen] = '\0';
    }
    if (current_liblen == 0) return cms$_noref;
    INIT_SDESC(dsc, elen, element);
    if (generation != 0 && *generation != '\0') {
    	INIT_SDESC(gdsc, strlen(generation), generation);
    }
    status = (*show_generation)(&cmsctx, cms_show_gen_callback,
    	    rdt, &dsc, (generation != 0 && *generation != '\0') ? &gdsc : 0,
    	    0, 0, 0, 0, message_routine);
    return status;

} /* cms_get_rdt */

/*
**++
**  ROUTINE:	cms_show_gen_callback
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
static unsigned int cms_show_gen_callback(int *new_elementp,
    	struct CMS_LDB *cmsctxp, void *rdtp, struct dsc$descriptor **elementp,
    	struct dsc$descriptor **genp, struct dsc$descriptor **usernamep,
    	TIME *trans_timep, TIME *create_timep, TIME *revision_timep,
    	struct dsc$descriptor **remarkp, struct dsc$descriptor **classp,
    	int *formatp, int *attrp, int *revnop, int *reservp, int *recsizp,
    	int *review_statp) {

    if (*new_elementp == 1) *((TIME *) rdtp) = *revision_timep;

    return cms$_normal;

} /* cms_show_gen_callback */

/*
**++
**  ROUTINE:	cms_fetch_file
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
unsigned int cms_fetch_file(char *fspec) {

    char libname[256];
    unsigned int status;
    struct dsc$descriptor dsc;
    int i;

    if (!cms_initialized) {
    	status = cms_init();
    	if (!OK(status)) return status;
    }

    i = strlen(cms$lib);
    if (current_liblen != i ||
    	    !strneql_case_blind(cms$lib, current_library, current_liblen)) {
    	INIT_SDESC(dsc, i, cms$lib);
    	if (i == 0) return cms$_noref;
    	status = (*set_library)(&cmsctx, &dsc, message_routine);
    	if (!OK(status)) {
    	    if (current_liblen > 0) {
    	    	INIT_SDESC(dsc, current_liblen, current_library);
    	    	(*set_library)(&cmsctx, &dsc, message_routine);
    	    }
    	    return status;
    	}
    	current_liblen = i;
    	memcpy(current_library, cms$lib, i);
    	current_library[i] = '\0';
    }
    if (current_liblen == 0) return cms$_noref;
    INIT_SDESC(dsc, strlen(fspec), fspec);
    status = (*fetch)(&cmsctx, &dsc, 0, 0, 0, 0, 0, 0, 0, 0,
    	    	    	    message_routine, 0, 0, 0, 0);
    return status;

} /* cms_fetch_file */

/*
**++
**  ROUTINE:	cms_parse_name
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Parses a CMS-type file specification into a library name
**  plus an element name.
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
unsigned int cms_parse_name(char *fspec, char *libname,
    	    int libname_size, int *libname_len, char *element,
    	    int element_size, int *element_len, int fill_in_lib) {

    struct item_list_2 {
    	unsigned short len;
    	unsigned short code;
    	char *pointer;
    } fscnlst[6];
    char *cp;
    struct dsc$descriptor dsc;
    int i, j, len;
    unsigned int status, flags;

    memset(fscnlst, 0, sizeof(fscnlst));
    fscnlst[0].code = FSCN$_DEVICE;
    fscnlst[1].code = FSCN$_ROOT;
    fscnlst[2].code = FSCN$_DIRECTORY;
    fscnlst[3].code = FSCN$_NAME;
    fscnlst[4].code = FSCN$_TYPE;

    INIT_SDESC(dsc, strlen(fspec), fspec);
    if (fspec[dsc.dsc$w_length-1] == '~') dsc.dsc$w_length -= 1;

    status = sys$filescan(&dsc, fscnlst, &flags, 0, 0);
    if (!OK(status)) return status;
    if (flags & (FSCN$M_NODE | FSCN$M_VERSION)) return SS$_BADPARAM;
    len = fscnlst[0].len + fscnlst[1].len + fscnlst[2].len;
    if (len > libname_size-1) len = libname_size-1;
    if (len > 0) {
    	if (libname_len) *libname_len = len;
    	cp = libname;
    	for (i = 0; len > 0 && i < 3; i++) {
    	    j = (len < fscnlst[i].len) ? len : fscnlst[i].len;
    	    memcpy(cp, fscnlst[i].pointer, j);
    	    len -= j;
    	    cp += j;
    	}
    	*cp = '\0';
    } else if (fill_in_lib) {
    	len = strlen(cms$lib);
    	if (len > libname_size-1) len = libname_size-1;
    	memcpy(libname, cms$lib, len);
    	libname[len] = '\0';
    	if (libname_len) *libname_len = len;
    } else {
    	if (libname_size > 0) libname[0] = '\0';
    	if (libname_len) *libname_len = 0;
    }
    len = fscnlst[3].len + fscnlst[4].len;
    if (len > element_size-1) len = element_size-1;
    if (element_len) *element_len = len;
    cp = element;
    for (i = 3; len > 0 && i < 5; i++) {
    	j = (len < fscnlst[i].len) ? len : fscnlst[i].len;
    	memcpy(cp, fscnlst[i].pointer, j);
    	len -= j;
    	cp += j;
    }
    *cp = '\0';

    return SS$_NORMAL;

} /* cms_parse_name */

/*
**++
**  ROUTINE:	message_routine
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
static unsigned int message_routine(unsigned int sig[],
    	unsigned int mech[], struct CMS_LDB *libctx) {

    return SS$_NORMAL;

} /* message_routine */
