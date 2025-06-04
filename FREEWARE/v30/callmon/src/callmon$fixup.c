/*  CALLMON - A Call Monitor for OpenVMS Alpha
 *
 *  File:     CALLMON$FIXUP.C
 *  Author:   Thierry Lelegard
 *  Version:  1.0
 *  Date:     24-JUL-1996
 *
 *  Abstract: This module processes the fixup inside an image.
 */


#include "callmon$private.h"


/*******************************************************************************
 *
 *  This routine modifies the fixups for one routine inside another
 *  image. Inside an image, the fixups are references to another
 *  shareable image. We explore each fixup table (they were generated
 *  by the linker). When we find a fixup which was adjusted by the image
 *  activator to point to the old routine, we modify it to point to the
 *  jacket routine.
 *
 *  There are 4 types of fixups:
 *    - quadword address fixups
 *    - longword address fixups
 *    - code address fixups
 *    - linkage pair fixups
 *
 *  Actually, 2 additional types of fixups exist but we ignore them:
 *    - Page protection fixup
 *    - Fixups for linkage pairs with procedure signature blocks
 *      (used in translated images)
 */

void callmon$$process_fixups (image_t* image, routine_t* routine)
{
    int64 old_entry, old_pdsc, new_entry, new_pdsc;
    pdsc_t* old_pdsc_p;
    shl_t* shl;
    int shlind;
    int hook_count = 0;

    /* We cannot modify resident and protected images */

    if (image->resident || image->protected)
        return;

    /* If there is no fixup in this image, nothing to do */

    if (image->eiaf == NULL)
        return;

    /* Compute the address of the old and new routine */

    old_pdsc_p = (pdsc_t*) routine->lkp.lkp$q_proc_value;
    old_pdsc   = routine->lkp.lkp$q_proc_value;
    old_entry  = routine->lkp.lkp$q_entry;
    new_pdsc   = (int64) routine->jacket;
    new_entry  = *(int64*) routine->jacket->pdsc$q_entry;

    /* Check if the current image has references into the target image.
     * We walk through the shareable image list (SHL) of the current image
     * until we find the target image. First, locate the SHL. */

    if (image->eiaf->eiaf$l_shrimgcnt <= 0 || image->eiaf->eiaf$l_shlstoff == 0)
        return;

    shl = (shl_t*) ((char*) image->eiaf + image->eiaf->eiaf$l_shlstoff);

    /* Then look for an entry for the target image */

    for (shlind = 0; shlind < image->eiaf->eiaf$l_shrimgcnt; shlind++)
        if (shl[shlind].shl$l_icb == routine->image->imcb)
            break;

    if (shlind == image->eiaf->eiaf$l_shrimgcnt)
        return;

    if (callmon$$own.trace_flags & TRACE_FIXUP)
        callmon$$putmsg (CALLMON$_DOFIXUP, 6, routine->name, image->logname,
            old_pdsc, old_entry, new_pdsc, new_entry);

    /* Modify all quadword address fixups in the current image */

    if (image->eiaf->eiaf$l_qdotadroff != 0) {

        fix_t* fix;
        fxe_t* fxe;
        int64* addr;

        /* Locate the table of quadword address fixups in current image */

        fix = (fix_t*) ((char*) image->eiaf + image->eiaf->eiaf$l_qdotadroff);

        /* Loop on all records of this table */

        for (; fix->fix$l_count != 0;
             fix = (fix_t*) (fix->fix$t_fixlst + fix->fix$l_count)) {

            /* If this fixup record does not point to the intercepted
             * image, we are not interested. */

            if (fix->fix$l_shlx != shlind)
                continue;

            /* Loop on all fixup elements for this fixup record */

            for (fxe = fix->fix$t_fixlst;
                 fxe < fix->fix$t_fixlst + fix->fix$l_count;
                 fxe++) {

                /* Compute the location inside current image where the
                 * fixup has been made. */

                addr = (int64*) ((char*) image->imcb->imcb$l_base_address +
                    fxe->fxe$l_offset);

                if (*addr == old_pdsc) {

                    /* The fixup points to the procedure descriptor of the
                     * intercepted routine. Modify to point to new routine. */

                    if (callmon$$own.trace_flags & TRACE_FIXUP)
                        callmon$$putmsg (CALLMON$_QPFIX, 1, addr);

                    if (callmon$$writeable (addr, sizeof (int64))) {
                        *addr = new_pdsc;
                        hook_count++;
                    }
                }

                else if (*addr == old_entry) {

                    /* The fixup points to the procedure entry code of the
                     * intercepted routine. Modify to point to new routine. */

                    if (callmon$$own.trace_flags & TRACE_FIXUP)
                        callmon$$putmsg (CALLMON$_QEFIX, 1, addr);

                    if (callmon$$writeable (addr, sizeof (int64))) {
                        *addr = new_entry;
                        hook_count++;
                    }
                }
            }
        }
    }

    /* Modify all longword address fixups in current image */

    if (image ->eiaf->eiaf$l_ldotadroff != 0) {

        fix_t* fix;
        fxe_t* fxe;
        int32* addr;

        /* Locate the table of longword address fixups in current image */

        fix = (fix_t*) ((char*) image->eiaf + image->eiaf->eiaf$l_ldotadroff);

        /* Loop on all records of this table */

        for (; fix->fix$l_count != 0;
             fix = (fix_t*) (fix->fix$t_fixlst + fix->fix$l_count)) {

            /* If this fixup record does not point to the intercepted
             * image, we are not interested. */

            if (fix->fix$l_shlx != shlind)
                continue;

            /* Loop on all fixup elements for this fixup record */

            for (fxe = fix->fix$t_fixlst;
                 fxe < fix->fix$t_fixlst + fix->fix$l_count;
                 fxe++) {

                /* Compute the location inside current image where the
                 * fixup has been made. */

                addr = (int32*) ((char*) image->imcb->imcb$l_base_address +
                    fxe->fxe$l_offset);

                if (*addr == old_pdsc) {

                    /* The fixup points to the procedure descriptor of the
                     * intercepted routine. Modify to point to new routine. */

                    if (callmon$$own.trace_flags & TRACE_FIXUP)
                        callmon$$putmsg (CALLMON$_LPFIX, 1, addr);

                    if (callmon$$writeable (addr, sizeof (int32))) {
                        *addr = new_pdsc;
                        hook_count++;
                    }
                }

                else if (*addr == old_entry) {

                    /* The fixup points to the procedure entry code of the
                     * intercepted routine. Modify to point to new routine. */

                    if (callmon$$own.trace_flags & TRACE_FIXUP)
                        callmon$$putmsg (CALLMON$_LEFIX, 1, addr);

                    if (callmon$$writeable (addr, sizeof (int32))) {
                        *addr = new_entry;
                        hook_count++;
                    }
                }
            }
        }
    }

    /* Modify all code address fixups */

    if (image->eiaf->eiaf$l_codeadroff != 0) {

        fix_t* fix;
        int32* offset;
        int64* addr;

        /* Locate the table of code address fixups in current image */

        fix = (fix_t*) ((char*) image->eiaf + image->eiaf->eiaf$l_codeadroff);

        /* Loop on all records of this table */

        for (; fix->fix$l_count != 0;
             fix = (fix_t*) (fix->fix$t_offlst + fix->fix$l_count)) {

            /* If this fixup record does not point to the intercepted
             * *image, we are not interested. */

            if (fix->fix$l_shlx != shlind)
                continue;

            /* Loop on all fixup elements for this fixup record */

            for (offset = fix->fix$t_offlst;
                 offset < fix->fix$t_offlst + fix->fix$l_count;
                 offset++) {

                /* Compute the location inside current image where the
                 * fixup has been made. */

                addr = (int64*) ((char*) image->imcb->imcb$l_base_address +
                    *offset);

                if (*addr == old_pdsc) {

                    /* The fixup points to the procedure descriptor of the
                     * intercepted routine. Modify to point to new routine. */

                    if (callmon$$own.trace_flags & TRACE_FIXUP)
                        callmon$$putmsg (CALLMON$_CPFIX, 1, addr);

                    if (callmon$$writeable (addr, sizeof (int64))) {
                        *addr = new_pdsc;
                        hook_count++;
                    }
                }

                else if (*addr == old_entry) {

                    /* The fixup points to the procedure entry code of the
                     * intercepted routine. Modify to point to new routine. */

                    if (callmon$$own.trace_flags & TRACE_FIXUP)
                        callmon$$putmsg (CALLMON$_CEFIX, 1, addr);

                    if (callmon$$writeable (addr, sizeof (int64))) {
                        *addr = new_entry;
                        hook_count++;
                    }
                }
            }
        }
    }

    /* Modify all linkage pair fixups */

    if (image->eiaf->eiaf$l_lpfixoff != 0) {

        fix_t* fix;
        int32* offset;
        lkp_t* addr;

        /* Locate the table of linkapge pair fixups in current image */

        fix = (fix_t*) ((char*) image->eiaf + image->eiaf->eiaf$l_lpfixoff);

        /* Loop on all records of this table */

        for (; fix->fix$l_count != 0;
             fix = (fix_t*) (fix->fix$t_offlst + fix->fix$l_count)) {

            /* If this fixup record does not point to the intercepted
             * image, we are not interested. */

            if (fix->fix$l_shlx != shlind)
                continue;

            /* Loop on all fixup elements for this fixup record */

            for (offset = fix->fix$t_offlst;
                 offset < fix->fix$t_offlst + fix->fix$l_count;
                 offset++) {

                /* Compute the location inside current image where the
                 * fixup has been made. */

                addr = (lkp_t*) ((char*) image->imcb->imcb$l_base_address +
                    *offset);

                if (addr->lkp$q_proc_value == old_pdsc) {

                    /* The fixup has been resolved to point to the intercepted
                     *  routine. Modify to point to new routine. */

                    if (callmon$$own.trace_flags & TRACE_FIXUP)
                        callmon$$putmsg (CALLMON$_LKFIX, 1, addr);

                    if (callmon$$writeable (addr, sizeof (lkp_t))) {
                        addr->lkp$q_proc_value = new_pdsc;
                        addr->lkp$q_entry = new_entry;
                        hook_count += 2;
                    }
                }
            }
        }
    }

    if ((callmon$$own.trace_flags & TRACE_REFERENCES) && hook_count != 0)
        callmon$$putmsg (CALLMON$_FIXUPCNT, 3, hook_count, routine->name,
            image->logname);
}
