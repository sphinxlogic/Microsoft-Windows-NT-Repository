/*
**++
**  FACILITY:	MMK
**
**  ABSTRACT:	Miscellaneous routines for MMK.
**
**  MODULE DESCRIPTION:
**
**  	Miscellaneous utility routines.
**
**  AUTHOR: 	    M. Madison
**  	    	    COPYRIGHT © 1992-1994, MADGOAT SOFTWARE.  ALL RIGHTS RESERVED.
**
**  CREATION DATE:  20-AUG-1992
**
**  MODIFICATION HISTORY:
**
**  	20-AUG-1992 V1.0    Madison 	Initial coding.
**  	01-SEP-1992 V1.1    Madison 	Comments.
**  	12-JAN-1993 V1.1-1  Madison 	Fix make_object_name.
**  	04-JUN-1993 V1.2    Madison 	Add default rule support.
**  	17-OCT-1993 V1.3    Madison 	Add strneql_case_blind.
**  	20-OCT-1993 V1.4    Madison 	Add ctrlt_ast routines.
**  	28-OCT-1993 V1.4-1  Madison 	Fix behavior of .SUFFIXES directive.
**  	09-DEC-1993 V1.4-2  Madison 	Fix extract_name to use $FILESCAN.
**  	04-APR-1994 V1.4-3  Madison 	Fix extract_name again!
**  	14-APR-1994 V1.4-4  Madison 	Fix extract_name again!
**  	11-JUL-1994 V1.4-5  Madison 	Have find_rule return 0 if none found.
**  	14-JUL-1994 V1.5    Madison 	New extract routines, scan_rule_list.
**  	11-AUG-1994 V1.5-1  Madison 	Convert suffixes to upper case.
**--
*/
#ifdef __DECC
#pragma module MISC "V1.5-1"
#else
#ifndef __GNUC__
#module MISC "V1.5-1"
#endif
#endif
#include "mmk.h"
#include "globals.h"
#ifdef __GNUC__
#include <vms/lnmdef.h>
#include <vms/iodef.h>
#include <vms/dvidef.h>
#include <vms/fscndef.h>
#else
#include <lnmdef.h>
#include <iodef.h>
#include <dvidef.h>
#include <fscndef.h>
#endif
#ifdef __DECC
#include <builtins.h>
#else
#pragma builtins
#endif

    static unsigned short sysinput_chan = 0;
    static unsigned int (*ctrlt_ast_rtn)(unsigned int) = 0;
    static unsigned int ctrlt_ast_arg = 0;

/*
** Forward declarations
*/
    void Build_Suffix_List(char *, int);
    char *find_char(char *, char *, char);
    void upcase(char *);
    int extract_name(char *, char *);
    int extract_prefix(char *, char *);
    int extract_filetype(char *, char *);
    int extract_filename(char *, char *);
    static int split_path(char *, char *, unsigned int);
    int prefix_match(char *, char *);
    struct SFX *find_suffix(char *);
    struct RULE *find_rule(char *, char *);
    struct RULE *find_rule_with_prefixes(struct OBJECT *, struct OBJECT *);
    struct RULE *scan_rule_list(struct RULE *, char *, int);
    int make_object_name(char *, struct OBJECT *);
    int logical_present(char *);
    int get_logical(char *, char *, int);
    int strneql_case_blind(char *, char *, int);
    void set_ctrlt_ast(unsigned int (*)(), unsigned int);
    void clear_ctrlt_ast(void);
    static unsigned int ctrlt_ast(void);
    unsigned int find_image_symbol(char *, char *, void *);
    static unsigned int x_find_image_symbol(struct dsc$descriptor *,
    	    	    	    struct dsc$descriptor *, void *);

/*
** External references
*/
    extern void mem_free_sfx(struct SFX *);
    extern struct SFX *mem_get_sfx(void);
    extern unsigned int cms_get_rdt(char *, char *, TIME *);
    extern unsigned int file_exists(char *, char *);

#ifdef __GNUC__

/*
**++
**  ROUTINE:	queue_insert
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Inserts an entry into a queue.  The QUE structure isn't
**  required, as long as the first two longwords of the structure
**  being used is a queue header.
**
**  RETURNS:	void
**
**  PROTOTYPE:
**
**  	queue_insert(item, pred)
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
void queue_insert(void *item, void *pred) {

    ((struct QUE *) item)->head = pred;
    ((struct QUE *) item)->tail = ((struct QUE *) pred)->tail;
    (((struct QUE *) pred)->tail)->head = item;
    ((struct QUE *) pred)->tail = item;
}

/*
**++
**  ROUTINE:	queue_remove
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Removes an entry from a queue, if there is one.
**
**  RETURNS:	int
**
**  PROTOTYPE:
**
**  	queue_remove(void *entry, void **addr);
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	1: Remove successful.
**  	0: No entry to remove.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
int queue_remove(void *entry, void **addr) {

    if (((struct QUE *) entry)->head == entry) return 0;
    (((struct QUE *) entry)->tail)->head = ((struct QUE *) entry)->head;
    (((struct QUE *) entry)->head)->tail = ((struct QUE *) entry)->tail;
    *addr = entry;
    return 1;
}
#endif

/*
**++
**  ROUTINE:	Build_Suffix_List
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Builds the queue of suffixes from the right-hand side
**  of a .SUFFIXES directive.
**
**  RETURNS:	void
**
**  PROTOTYPE:
**
**  	Build_Suffix_List(char *line, int linelen)
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
void Build_Suffix_List(char *line, int linelen) {

    struct SFX *sfx;
    char *lp, *lpmax;
    int i;

    if (linelen == 0) {
    	while (queue_remove(suffixes.flink, &sfx)) mem_free_sfx(sfx);
    	return;
    }

    lp = line;
    lpmax = line+linelen;
    while (1) {
    	while (lp < lpmax && isspace(*lp)) lp++;
    	if (lp >= lpmax) break;
    	sfx = mem_get_sfx();
    	i = 0;
    	while (lp < lpmax && !isspace(*lp)) {
    	    if (i < MMK_S_SFX-1)
    	    	sfx->value[i++] = islower(*lp) ? toupper(*lp) : *lp;
    	    lp++;
    	}
    	sfx->value[i] = '\0';
    	queue_insert(sfx, suffixes.blink);
    }
}

/*
**++
**  ROUTINE:	find_char
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Locates a character in a string, given the start and end
**  address of the string.
**
**  RETURNS:	pointer to char
**
**  PROTOTYPE:
**
**  	find_char(char *base, char *end, char ch)
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	    0: not found
**  	non-0: pointer to the character
**
**  SIDE EFFECTS:   	None.
**
**--
*/
char *find_char(char *base, char *end, char ch) {

    register char *cp;

    for (cp = base; cp < end; cp++) {
    	if (*cp == ch) return cp;
    }

    return (char *) 0;
}

/*
**++
**  ROUTINE:	upcase
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Converts a string in-place to upper case.
**
**  RETURNS:	void
**
**  PROTOTYPE:
**
**  	upcase(char *str)
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
void upcase(char *str) {

    register char *cp;

    for (cp = str; *cp; cp++) {
    	*cp = islower(*cp) ? toupper(*cp) : *cp;
    }

    return;
}

/*
**++
**  ROUTINE:	extract_name
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Given a VMS file specification, returns everything up to,
**  but not including, the file type.
**
**  RETURNS:	int
**
**  PROTOTYPE:
**
**  	extract_name(char *dest, char *src)
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
int extract_name(char *dest, char *src) {

   return split_path(dest, src, FSCN$M_DEVICE|FSCN$M_ROOT|FSCN$M_DIRECTORY|FSCN$M_NAME);

} /* extract_name */

/*
**++
**  ROUTINE:	extract_prefix
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Given a VMS file specification, returns the device and directory.
**
**  RETURNS:	int
**
**  PROTOTYPE:
**
**  	extract_prefix(char *dest, char *src)
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
int extract_prefix(char *dest, char *src) {

    return split_path(dest, src, FSCN$M_DEVICE|FSCN$M_ROOT|FSCN$M_DIRECTORY);

} /* extract_prefix */

/*
**++
**  ROUTINE:	extract_filetype
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Given a VMS file specification, returns just the file type.
**
**  RETURNS:	int
**
**  PROTOTYPE:
**
**  	extract_filetype(char *dest, char *src)
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
int extract_filetype(char *dest, char *src) {

    return split_path(dest, src, FSCN$M_TYPE);

} /* extract_filetype */

/*
**++
**  ROUTINE:	extract_filename
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Given a VMS file specification, returns just the file name.
**
**  RETURNS:	int
**
**  PROTOTYPE:
**
**  	extract_name(char *dest, char *src)
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
int extract_filename(char *dest, char *src) {

    return split_path(dest, src, FSCN$M_NAME);

} /* extract_filename */

/*
**++
**  ROUTINE:	split_path
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Workhorse routine for the extract_xxx routines.  Uses the
**  $FILESCAN system service to parse a file specification, and
**  copies in just the requested parts.
**
**  RETURNS:	int
**
**  PROTOTYPE:
**
**  	split_path(char *dest, char *src, unsigned int flags)
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
static int split_path(char *dest, char *src, unsigned int flags) {

    static struct {
    	unsigned short len, code;
    	char *ptr;
    } itmlst[] = {0, FSCN$_DEVICE, 0,
                  0, FSCN$_ROOT, 0,
    	    	  0, FSCN$_DIRECTORY, 0,
    	    	  0, FSCN$_NAME, 0,
    	    	  0, FSCN$_TYPE, 0,
    	    	  0, 0, 0};
    static unsigned int part[] = {FSCN$M_DEVICE, FSCN$M_ROOT,
    	    	    	    	  FSCN$M_DIRECTORY, FSCN$M_NAME, FSCN$M_TYPE};
    unsigned int status;
    struct dsc$descriptor fdsc;
    char *cp;
    int i;

    INIT_SDESC(fdsc, strlen(src), src);
    status = sys$filescan(&fdsc, itmlst, 0);
    cp = dest;
    if (OK(status)) {
    	for (i = 0; i < sizeof(part)/sizeof(unsigned int); i++) {
    	    if ((flags & part[i]) && itmlst[i].len > 0) {
    	    	if (part[i] == FSCN$M_TYPE) {
    	    	    if (itmlst[i].ptr[itmlst[i].len] == '~') itmlst[i].len++;
    	    	}
    	    	memcpy(cp, itmlst[i].ptr, itmlst[i].len);
    	    	cp += itmlst[i].len;
    	    }
    	}
    }
    *cp = '\0';

    return (cp - dest);

} /* split_path */

/*
**++
**  ROUTINE:	prefix_match
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Given a "prefix" (i.e., a device+directory specification) and
**  a file specification, checks to see if the file spec has the given
**  prefix.
**
**  RETURNS:	int
**
**  PROTOTYPE:
**
**  	prefix_match(char *pfx, char *fspec)
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
int prefix_match(char *pfx, char *fspec) {

    char tmp[256];
    int len;

    len = split_path(tmp, fspec, FSCN$M_DEVICE|FSCN$M_ROOT|FSCN$M_DIRECTORY);
    if (len != strlen(pfx)) return 0;

    return strneql_case_blind(tmp, pfx, len);

} /* prefix_match */

/*
**++
**  ROUTINE:	find_suffix
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Locates a suffix in the suffix queue.
**
**  RETURNS:	pointer to a SFX structure
**
**  PROTOTYPE:
**
**  	find_suffix(char *str)
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	non-0:	address of entry in suffixes queue.
**  	    0:  not found
**
**  SIDE EFFECTS:   	None.
**
**--
*/
struct SFX *find_suffix(char *name) {

    struct SFX *sfx;

    for (sfx = suffixes.flink; sfx != &suffixes; sfx = sfx->flink) {
    	if (strcmp(name, sfx->value) == 0) return sfx;
    }

    return (struct SFX *) 0;
}

/*
**++
**  ROUTINE:	find_rule
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Given source and target suffix strings, locates the base default
**  build rule for those suffixes.
**
**  RETURNS:	pointer to struct RULE
**
**  PROTOTYPE:
**
**  	find_rule(char *target, char *source)
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	non-0:	pointer to rule entry
**  	    0:  not found
**
**  SIDE EFFECTS:   	None.
**
**--
*/
struct RULE *find_rule(char *trg, char *src) {

    struct RULE *r;

    for (r = rules.flink; r != &rules; r = r->flink) {
    	if (strcmp(trg, r->trg) == 0 && strcmp(src, r->src) == 0) return r;
    }

    return 0;

} /* find_rule */

/*
**++
**  ROUTINE:	find_rule_with_prefixes
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Given two objects, locates a build rule for those objects,
**  taking including scanning for prefixes.
**
**  RETURNS:	pointer to struct RULE
**
**  PROTOTYPE:
**
**  	find_rule_with_prefixes(struct OBJECT *target, struct OBJECT *source)
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	non-0:	pointer to rule entry
**  	    0:  not found
**
**  SIDE EFFECTS:   	None.
**
**--
*/
struct RULE *find_rule_with_prefixes(struct OBJECT *trg, struct OBJECT *src) {

    struct RULE *xr, *r;

    for (xr = rules.flink; xr != &rules; xr = xr->flink) {
    	for (r = xr; r != 0; r = r->next) {
    	    if (strcmp(trg->sfx, r->trg) == 0 && strcmp(src->sfx, r->src) == 0) {
    	    	if (prefix_match(r->trgpfx, trg->name) &&
    	    	    prefix_match(r->srcpfx, src->name)) return r;
    	    }
    	}
    	for (r = xr; r != 0; r = r->next) {
    	    if (strcmp(trg->sfx, r->trg) == 0 && strcmp(src->sfx, r->src) == 0) {
    	    	if (r->trgpfx[0] == '\0' && r->srcpfx[0] == '\0') return r;
    	    }
    	}
    }

    return 0;

} /* find_rule_with_prefixes */

/*
**++
**  ROUTINE:	scan_rule_list
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Given a base rule from a find_rule() call, scans
**  the list of rules (with prefixes, possibly) hanging off
**  the base, looking for a rule that might be used to
**  build a target.
**
**  RETURNS:	struct RULE *
**
**  PROTOTYPE:
**
**  	scan_rule_list(struct RULE *base, char *target_name, int generalize)
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	    0:	    	No rule found.
**  	    non-0:  	Matching rule found.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
struct RULE *scan_rule_list(struct RULE *base, char *target_name, int generalize) {

    TIME rdt ;
    struct RULE *r, *tmpr, *r_maybe[128], *tmpr_maybe[128];
    struct SFX *s;
    char trgnam[MMK_S_FILE], tmpsfx[MMK_S_SFX], fspec[MMK_S_FILE], *cp;
    unsigned int status;
    int pass, passmax, check_cms, trgnamlen, maybes, i;

    trgnamlen = extract_filename(trgnam, target_name);
    maybes = 0;

/*
**  When there's just one rule for this suffix pair, we want to avoid
**  checking it twice.  Hence this rather convoluted-looking logic,
**  which prevents us from doing a prefix-based scan plus a non-prefix-based
**  scan in the 1-rule case.
**
**  This is further complicated by our recursive inference check for files
**  residing in CMS.  When we call with generalize == 1, we want to check
**  _just_ the prefixed rules.  When we call with generalize == 2, we
**  want to check _just_ the non-prefixed rules.
**
**  Phew! Now I understand why NMAKE doesn't have generic rules that
**  work across directories.
*/
    if (base->next == 0) {
    	if (base->trgpfx[0] == '\0' && base->srcpfx[0] == '\0') {
    	    pass = 1;
    	    passmax = 2;
    	} else {
    	    pass = 0;
    	    passmax = 1;
    	}
    } else {
    	pass = generalize < 2 ? 0 : 1;
    	passmax = (generalize & 1) ? 1 : 2;
    }

/*
**  Now scan the base rule and any rules hanging off it.  We may do two
**  passes over the list -- one with prefixes being used, the other without.
*/
    for (r = base; pass < passmax; pass++, r = base) {
    	while (r) {
    	    /*
    	    **	The first pass is the prefix-based scan.
    	    */
    	    if (pass == 0) {
    	    	if (!prefix_match(r->trgpfx, target_name)) {
    	    	    r = r->next;
    	    	    continue;
    	    	}
    	    /*
    	    **	The second pass is the non-prefix-based scan.
    	    */
    	    } else {
    	    	if (r->trgpfxlen != 0 || r->srcpfxlen != 0) {
    	    	    r = r->next;
    	    	    continue;
    	    	}
    	    }
    	    check_cms = r->src[strlen(r->src)-1] == '~';
    	    memcpy(fspec, r->srcpfx, r->srcpfxlen);
    	    memcpy(fspec+r->srcpfxlen, trgnam, trgnamlen);
    	    strcpy(fspec+(r->srcpfxlen+trgnamlen), r->src);
    	    if (check_cms) status = cms_get_rdt(fspec, 0, &rdt);
    	    else status = file_exists(fspec, 0);
    	    if (OK(status)) break;
/*
**  OK, so the source file doesn't exist.  If we're using
**  CMS, let's see if we can infer the existence of the
**  source we want from its presence in the CMS library.
*/
    	    if (!check_cms && use_cms) {
    	    	strcpy(tmpsfx, r->src);
    	    	strcat(tmpsfx, "~");
    	    	s = find_suffix(tmpsfx);
    	    	if (s != 0) {
    	    	    tmpr = find_rule(r->src, s->value);
    	    	    if (tmpr != 0) {
    	    	    	if (scan_rule_list(tmpr, fspec, 1)) break;
/*
**  We schedule a later check on the generic rules if the rule we found
**  has both prefixed and generic ones.
*/
    	    	    	if (tmpr->next != 0) {
    	    	    	    r_maybe[maybes] = r;
    	    	    	    tmpr_maybe[maybes++] = tmpr;
    	    	    	}
    	    	    }
    	    	}
    	    }
    	    r = r->next;
    	}
    	if (r != 0) break;
    }

    if ((pass >= passmax || r == 0) && maybes > 0) {
    	for (i = 0; i < maybes; i++) {
    	    r = r_maybe[i];
    	    tmpr = tmpr_maybe[i];
    	    memcpy(fspec, r->srcpfx, r->srcpfxlen);
    	    memcpy(fspec+r->srcpfxlen, trgnam, trgnamlen);
    	    strcpy(fspec+(r->srcpfxlen+trgnamlen), r->src);
    	    if (scan_rule_list(tmpr, fspec, 2) != 0) break;
    	}
    	if (i >= maybes) r = 0;
    }

    return (pass < passmax) ? r : 0;

} /* scan_rule_list */

/*
**++
**  ROUTINE:	make_object_name
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Given an object structure, formats the name in a string.
**
**  RETURNS:	void
**
**  PROTOTYPE:
**
**  	make_object_name(char *name, struct OBJECT *object)
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
int make_object_name(char *name, struct OBJECT *obj) {

    char *cp;
    int len;

    if (obj->type == MMK_K_OBJ_LIBMOD) {
    	cp =  name;
    	len = strlen(obj->libfile->name);
    	memcpy(cp, obj->libfile->name, len);
    	cp += len;
    	*cp++ = '(';
    	len = strlen(obj->name);
    	memcpy(cp, obj->name, len);
    	cp+= len;
    	if (obj->fileobj) {
    	    *cp++ = '=';
    	    len = strlen(obj->fileobj->name);
    	    memcpy(cp, obj->fileobj->name, len);
    	    cp += len;
    	}
    	*cp++ = ')';
    	*cp = '\0';
    	len = cp - name;
    } else {
    	len = strlen(obj->name);
    	memcpy(name, obj->name, len);
    	name[len] = '\0';
    }

    return len;

} /* make_object_name */

/*
**++
**  ROUTINE:	logical_present
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Checks to see if a logical name exists in one of the
**  LNM$FILE_DEV tables.
**
**  RETURNS:	int
**
**  PROTOTYPE:
**
**  	logical_present(char *lognam)
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	1: logical present
**  	0: logical not present
**
**  SIDE EFFECTS:   	None.
**
**--
*/
int logical_present(char *lognam) {

    struct dsc$descriptor namdsc;
    $DESCRIPTOR(tabdsc, "LNM$FILE_DEV");
    unsigned int attr = LNM$M_CASE_BLIND;

    INIT_SDESC(namdsc, strlen(lognam), lognam);
    return OK(sys$trnlnm(&attr, &tabdsc, &namdsc, 0, 0));

}

/*
**++
**  ROUTINE:	get_logical
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Checks to see if a logical name exists in one of the
**  LNM$FILE_DEV tables.
**
**  RETURNS:	int
**
**  PROTOTYPE:
**
**  	get_logical(char *lognam, char *buf, int bufsize)
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	1: logical present
**  	0: logical not present
**
**  SIDE EFFECTS:   	None.
**
**--
*/
int get_logical(char *lognam, char *buf, int bufsize) {

    struct dsc$descriptor namdsc;
    $DESCRIPTOR(tabdsc, "LNM$FILE_DEV");
    unsigned int attr = LNM$M_CASE_BLIND;
    ITMLST lnmlst[2];
    unsigned short retlen;
    unsigned int status;

    INIT_SDESC(namdsc, strlen(lognam), lognam);
    ITMLST_INIT(lnmlst[0], LNM$_STRING, bufsize-1, buf, &retlen);
    ITMLST_INIT(lnmlst[1], 0, 0, 0, 0);
    status = sys$trnlnm(&attr, &tabdsc, &namdsc, 0, lnmlst);
    if (OK(status)) buf[retlen] = '\0';
    return OK(status);

} /* get_logical */

/*
**++
**  ROUTINE:	strneql_case_blind
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Counted case-blind string equality function.
**
**  RETURNS:	boolean
**
**  PROTOTYPE:
**
**  	strneql_case_blind(char *str1, char *str2, int len)
**
**  str1: character string, read only, by reference (ASCIZ)
**  str2: character string, read only, by reference (ASCIZ)
**  len:  integer, read only, by value
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	    0:	strings are not equal up to the specified length (case-blind)
**  	non-0:	strings are equal up to the specified length (case-blind)
**
**  SIDE EFFECTS:   	None.
**
**--
*/
int strneql_case_blind(char *s1, char *s2, int len) {

    register unsigned char c1, c2;

    while (len > 0) {
        c1 = _toupper(*s1);
        c2 = _toupper(*s2);
        if (c1 != c2) return 0;
        if (c1 == '\0') return 1;
        s1++; s2++;
    	len--;
    }

    return 1;
}

/*
**++
**  ROUTINE:	set_ctrlt_ast
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Sets up an out-of-band AST for ctrl/T.
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
void set_ctrlt_ast(unsigned int (*routine)(), unsigned int arg) {

    static $DESCRIPTOR(sysinput, "SYS$INPUT:");
    static unsigned int dvi_trm = DVI$_TRM;
    static unsigned int trm_mask[2] = {0, 1<<('T'-'@')};
    unsigned int status, is_term;

    status = sys$assign(&sysinput, &sysinput_chan, 0, 0);
    if (!OK(status)) {
    	sysinput_chan = 0;
    	return;
    }
    status = lib$getdvi(&dvi_trm, &sysinput_chan, 0, &is_term);
    if (!OK(status)) {
    	sys$dassgn(sysinput_chan);
    	sysinput_chan = 0;
    	return;
    }
    if (is_term) {
    	ctrlt_ast_rtn = routine;
    	ctrlt_ast_arg = arg;
    	sys$qiow(0, sysinput_chan, IO$_SETMODE|IO$M_OUTBAND,
    	    0, 0, 0, ctrlt_ast, trm_mask, 0, 0, 0, 0);
    } else {
    	sys$dassgn(sysinput_chan);
    	sysinput_chan = 0;
    	ctrlt_ast_rtn = 0;
    	ctrlt_ast_arg = 0;
    }

    return;

} /* set_ctrlt_ast */

/*
**++
**  ROUTINE:	clear_ctrlt_ast
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Clears the out-of-band AST for ctrl/T.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	clear_ctrlt_ast()
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
void clear_ctrlt_ast(void) {

    static unsigned int trm_mask[2] = {0, 1<<('T'-'@')};

    if (sysinput_chan == 0) return;
    sys$qiow(0, sysinput_chan, IO$_SETMODE|IO$M_OUTBAND,
    	    0, 0, 0, 0, trm_mask, 0, 0, 0, 0);
    sys$dassgn(sysinput_chan);
    sysinput_chan = 0;
    ctrlt_ast_rtn = 0;
    ctrlt_ast_arg = 0;

    return;

} /* clear_ctrlt_ast */

/*
**++
**  ROUTINE:	ctrlt_ast
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Ctrl/T AST routine.
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
static unsigned int ctrlt_ast(void) {

    return ctrlt_ast_rtn ? (*ctrlt_ast_rtn)(ctrlt_ast_arg)
    	    	    	 : SS$_NORMAL;

} /* ctrlt_ast */

/*
**++
**  ROUTINE:	find_image_symbol
**
**  FUNCTIONAL DESCRIPTION:
**
**  	C interface to LIB$FIND_IMAGE_SYMBOL.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	find_image_symbol(char *image, char *symbol, void *symval)
**
**  image:  file_spec, read only, by reference (ASCIZ string)
**  symbol: character string, read only, by reference (ASCIZ)
**  symval: unspecified, write only, by reference
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
unsigned int find_image_symbol(char *image, char *symbol, void *symval) {

    struct dsc$descriptor idsc, sdsc;

    lib$establish(lib$sig_to_ret);
    INIT_SDESC(idsc, strlen(image), image);
    INIT_SDESC(sdsc, strlen(symbol), symbol);
    return x_find_image_symbol(&idsc, &sdsc, symval);

} /* find_image_symbol */

static unsigned int x_find_image_symbol(struct dsc$descriptor *imgnam,
    	    	    	struct dsc$descriptor *symnam, void *symval) {

    return lib$find_image_symbol(imgnam, symnam, symval);

}
