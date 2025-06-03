/*
**++
**  FACILITY:	NEWSRDR
**
**  ABSTRACT:	Memory management routines
**
**  MODULE DESCRIPTION:
**
**  	These memory management routines are used to handle the
**  frequently used blocks of memory needed by NEWSRDR.  We create
**  VM zones specifically tailored to each type of block's needs.
**
**  AUTHOR: 	    M. Madison
**  	    	    COPYRIGHT © 1992, MADGOAT SOFTWARE  ALL RIGHTS RESERVED.
**
**  CREATION DATE:  05-SEP-1992
**
**  MODIFICATION HISTORY:
**
**  	05-SEP-1992 V1.0    Madison 	Initial coding.
**  	24-SEP-1993 V1.1    Madison 	Initialize queues in GRP structures.
**--
*/
#ifdef __GNUC__
#include <vms/libvmdef.h>
#else
#include <libvmdef.h>
#endif
#define MODULE_MEM
#include "newsrdr.h"

    struct HDR *mem_gethdr(int);
    void       mem_freehdr(struct HDR *);
    struct GRP *mem_getgrp(void);
    void       mem_freegrp(struct GRP *);
#ifdef notdef
    struct SEL *mem_getsel(void);
    void       mem_freesel(struct SEL *);
#endif /* notdef */
    struct RNG *mem_getrng(void);
    void       mem_freerng(struct RNG *);
    void       mem_initcache(int);
    void       *mem_getcache(void);
    void       mem_freecache(void *);
    void       mem_delcache(void);

    static unsigned int hdrzone=0, grpzone=0, rngzone=0, cachezn=0, selzone=0;
    static int cache_unit_size=0;


/*
**++
**  ROUTINE:	mem_gethdr
**
**  FUNCTIONAL DESCRIPTION:
**
**  	The header VM pool is set up as a FREQUENT-SIZES VM zone,
**  where the frequent sizes are 20, 40, 80, and 160 bytes (plus
**  the header overhead of 8 bytes).  When a request comes in for a size
**  that's not exactly one of those, the next larger size is allocated.
**  For headers longer than 160 bytes, none of the lookaside lists
**  are used.
**
**  RETURNS:	struct HDR *
**
**  PROTOTYPE:
**
**  	mem_gethdr(int size)
**
**  size: integer, read only, by value
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	All errors are signalled.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
struct HDR *mem_gethdr(int size) {

    int allosize;
    struct {
    	int size1;
    	int size2;
    	char rest[1];
    } *blk;
    unsigned int status;

    if (hdrzone == 0) {
    	unsigned int alg = LIB$K_VM_FREQ_SIZES;
    	unsigned int param = 4;
    	unsigned int flags = LIB$M_VM_GET_FILL0 | LIB$M_VM_EXTEND_AREA;
    	status = lib$create_vm_zone(&hdrzone, &alg, &param, &flags);
    	if (!OK(status)) lib$stop(status);
    }
    allosize = size + sizeof(struct HDR);
    if (allosize < 20) allosize = 20;
    else if (allosize <= 40) allosize = 40;
    else if (allosize <= 80) allosize = 80;
    else if (allosize <= 160) allosize = 160;

    allosize += sizeof(int) * 2;

    status = lib$get_vm(&allosize, &blk, &hdrzone);
    if (!OK(status)) lib$stop(status);
    blk->size1 = blk->size2 = allosize;

    return (struct HDR *) &(blk->rest);

} /* mem_gethdr */

/*
**++
**  ROUTINE:	mem_freehdr
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Frees a header.  Note that the actual size of the
**  allocated block is hidden "behind" the HDR structure we
**  passed back in mem_gethdr.
**
**  RETURNS:	void
**
**  PROTOTYPE:
**
**  	mem_freehdr(struct HDR *hdr)
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
void mem_freehdr(struct HDR *hdr) {

    struct BLK {
    	int size1;
    	int size2;
    	char rest[1];
    } *blk;
    int size;

    blk = (struct BLK *) (((char *) hdr) - sizeof(int)*2);
    if (blk->size1 != blk->size2) lib$stop(SS$_BADPARAM);
    size = blk->size1;
    lib$free_vm(&size, &blk, &hdrzone);

} /* mem_freehdr */

/*
**++
**  ROUTINE:	mem_getgrp
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Allocates a GRP structure from the GRPZONE.
**
**  RETURNS:	struct GRP *
**
**  PROTOTYPE:
**
**  	mem_getgrp()
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	All errors are signalled.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
struct GRP *mem_getgrp() {

    struct GRP *grp;
    unsigned int status;
    static int size=sizeof(struct GRP);

    if (grpzone == 0) {
    	unsigned int alg = LIB$K_VM_FIXED;
    	unsigned int flags = LIB$M_VM_GET_FILL0 | LIB$M_VM_EXTEND_AREA;
    	status = lib$create_vm_zone(&grpzone, &alg, &size, &flags);
    	if (!OK(status)) lib$stop(status);
    }

    status = lib$get_vm(&size, &grp, &grpzone);
    if (!OK(status)) lib$stop(status);

    grp->seenq.head = grp->seenq.tail = &grp->seenq;
    grp->killq.head = grp->killq.tail = &grp->killq;

    return grp;

} /* mem_getgrp */

/*
**++
**  ROUTINE:	mem_freegrp
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Frees a GRP block.
**
**  RETURNS:	void
**
**  PROTOTYPE:
**
**  	mem_freegrp(struct GRP *grp)
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
void mem_freegrp(struct GRP *grp) {

    static int size = sizeof(struct GRP);

    lib$free_vm(&size, &grp, &grpzone);

} /* mem_freegrp */

/*
**++
**  ROUTINE:	mem_getsel
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Allocates a SEL structure from the SELZONE.
**
**  RETURNS:	struct SEL *
**
**  PROTOTYPE:
**
**  	mem_getsel()
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	All errors are signalled.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
#ifdef notdef
struct SEL *mem_getsel() {

    struct SEL *sel;
    unsigned int status;
    static int size=sizeof(struct SEL);

    if (selzone == 0) {
    	unsigned int alg = LIB$K_VM_FIXED;
    	unsigned int flags = LIB$M_VM_GET_FILL0 | LIB$M_VM_EXTEND_AREA;
    	status = lib$create_vm_zone(&selzone, &alg, &size, &flags);
    	if (!OK(status)) lib$stop(status);
    }

    status = lib$get_vm(&size, &sel, &selzone);
    if (!OK(status)) lib$stop(status);
    sel->rngque.head = sel->rngque.tail = &sel->rngque;
    sel->subjque.head = sel->subjque.tail = &sel->subjque;
    sel->kwdque.head = sel->kwdque.tail = &sel->kwdque;
    sel->fromque.head = sel->fromque.tail = &sel->fromque;
    sel->xsubjque.head = sel->xsubjque.tail = &sel->xsubjque;
    sel->xfromque.head = sel->xfromque.tail = &sel->xfromque;
    return sel;

} /* mem_getsel */

/*
**++
**  ROUTINE:	mem_freesel
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Frees a SEL block.
**
**  RETURNS:	void
**
**  PROTOTYPE:
**
**  	mem_freesel(struct SEL *sel)
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
void mem_freesel(struct SEL *sel) {

    static int size = sizeof(struct SEL);
    struct RNG *rng;
    struct HDR *hdr;

    while (queue_remove(sel->rngque.head, &rng)) mem_freerng(rng);
    while (queue_remove(sel->subjque.head &hdr)) mem_freehdr(hdr);
    while (queue_remove(sel->kwdque.head &hdr)) mem_freehdr(hdr);
    while (queue_remove(sel->fromque.head &hdr)) mem_freehdr(hdr);
    while (queue_remove(sel->xsubjque.head &hdr)) mem_freehdr(hdr);
    while (queue_remove(sel->xfromque.head &hdr)) mem_freehdr(hdr);
    lib$free_vm(&size, &sel, &selzone);

} /* mem_freesel */
#endif /* notdef */

/*
**++
**  ROUTINE:	mem_getrng
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Allocates a RNG block from the RNGZONE.
**
**  RETURNS:	struct RNG *
**
**  PROTOTYPE:
**
**  	mem_getrng()
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	All errors are signalled.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
struct RNG *mem_getrng() {

    struct RNG *rng;
    unsigned int status;
    static int size=sizeof(struct RNG);

    if (rngzone == 0) {
    	unsigned int alg = LIB$K_VM_FIXED;
    	unsigned int flags = LIB$M_VM_GET_FILL0 | LIB$M_VM_EXTEND_AREA;
    	status = lib$create_vm_zone(&rngzone, &alg, &size, &flags);
    	if (!OK(status)) lib$stop(status);
    }

    status = lib$get_vm(&size, &rng, &rngzone);
    if (!OK(status)) lib$stop(status);
    return rng;

} /* mem_getrng */

/*
**++
**  ROUTINE:	mem_freerng
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Frees a RNG structure.
**
**  RETURNS:	void
**
**  PROTOTYPE:
**
**  	mem_freerng(struct RNG *rng)
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
void mem_freerng(struct RNG *rng) {

    static int size = sizeof(struct RNG);

    lib$free_vm(&size, &rng, &rngzone);

} /* mem_freerng */

/*
**++
**  ROUTINE:	mem_initcache
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Initializes a cache of fixed-size blocks.  Used by the
**  ARTICLE module for its private CHDR structures.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	mem_initcache(int size)
**
**  size:   integer, read only, by value
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	All errors are signalled.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
void mem_initcache(int size) {

    unsigned int status;

    if (cachezn == 0) {
    	unsigned int alg = LIB$K_VM_FIXED;
    	unsigned int flags = LIB$M_VM_GET_FILL0 | LIB$M_VM_EXTEND_AREA;
    	status = lib$create_vm_zone(&cachezn, &alg, &size, &flags);
    	if (!OK(status)) lib$stop(status);
    }

    cache_unit_size = size;

    return;

} /* mem_initcache */

/*
**++
**  ROUTINE:	mem_getcache
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Allocates a block from the cache created by mem_initcache.
**
**  RETURNS:	void *
**
**  PROTOTYPE:
**
**  	mem_getcache()
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:
**  	All errors are signalled.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
void *mem_getcache() {

    void *cache;
    unsigned int status;

    if (cachezn == 0) lib$stop(SS$_BADPARAM);
    status = lib$get_vm(&cache_unit_size, &cache, &cachezn);
    if (!OK(status)) lib$stop(status);
    return cache;

} /* mem_getcache */

/*
**++
**  ROUTINE:	mem_freecache
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Frees a block allocated by mem_getcache.
**
**  RETURNS:	void
**
**  PROTOTYPE:
**
**  	mem_freecache(void *cache_block)
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
void mem_freecache(void *cache) {

    lib$free_vm(&cache_unit_size, &cache, &cachezn);

} /* mem_freecache */

/*
**++
**  ROUTINE:	mem_delcache
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Deletes the VM zone created by mem_initcache, in one fell swoop.
**
**  RETURNS:	void
**
**  PROTOTYPE:
**
**  	mem_delcache()
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
void mem_delcache() {

    if (cachezn) {
    	lib$delete_vm_zone(&cachezn);
    	cachezn = cache_unit_size = 0;
    }

} /* mem_delcache */
