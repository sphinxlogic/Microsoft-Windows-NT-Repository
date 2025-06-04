/*  CALLMON - A Call Monitor for OpenVMS Alpha
 *
 *  File:     CALLMON$IMAGE.C
 *  Author:   Thierry Lelegard
 *  Version:  1.0
 *  Date:     24-JUL-1996
 *
 *  Abstract: This module contains the routines which build the tree
 *            of images.
 */


#include "callmon$private.h"


/*******************************************************************************
 *
 *  This routine is a "comparison routine" for LIB$*_TREE on the tree
 *  of image descriptors.
 */

static int image_tree_compare (
    char*    image_logname,
    image_t* comparison_node,
    void*    user_data)
{
    return strcmp (image_logname, comparison_node->logname);
}


/*******************************************************************************
 *
 *  This routine returns the description of an image.
 *  Return NULL if the requested image is not found.
 */

image_t* callmon$$get_image (char* logname)
{
    uint32 status;
    libtree_t* node;

    status = lib$lookup_tree (&callmon$$own.image_tree,
        logname, image_tree_compare, &node);

    if ($VMS_STATUS_SUCCESS (status))
        return BASE (image_t, tree_node, node);
    else if (status == LIB$_KEYNOTFOU)
        return NULL;
    else {
        callmon$$putmsg (CALLMON$_FNDIMAGE, 0, status);
        return NULL;
    }
}


/*******************************************************************************
 *
 *  This routine decodes the Global Symbol Table of an image and insert
 *  all its routines inside the tree of routines. Exit application in case
 *  of error.
 */

static void store_routines (image_t* image)
{
    char* record;
    uint16 rec_size;
    int index;
    eobj_t* eobj;
    egsd_t* egs;
    egst_t* subrec;
    egst_t* endrec;

    const uint16 flags = EGSY$M_DEF | EGSY$M_UNI | EGSY$M_REL | EGSY$M_NORM;

    /* If the GST is not available, nothing to do */

    if (image->gst == NULL || image->eihs == NULL)
        return;

    /* Loop on all records in the Global Symbol Table of the image */

    record = image->gst;

    for (index = 0; index < image->eihs->eihs$l_gstsize; index++) {

        /* First two bytes are the record length, then starts the record */

        rec_size = *(uint16*)record;
        record += 2;

        /* Only look at record typed "Global Symbol Definition" (GSD) */

        eobj = (eobj_t*) record;
        if (eobj->eobj$w_rectyp == EOBJ$C_EGSD) {

            /* Point to first GSD subrecord. */
            /* The actual length of the record is in the record header. */

            egs = (egsd_t*) record;
            subrec = (egst_t*) &egs->egsd$w_gsdtyp;
            endrec = (egst_t*) (record + eobj->eobj$w_size);

            /* Loop on sub-records until the whole record is processed */

            while (subrec < endrec) {

                /* We are only interested in universal symbol definition */
                /* of routines (not data or psect). */

                if (subrec->egst$w_gsdtyp == EGSD$C_SYMG &&
                    (subrec->egst$w_flags & flags) == flags)
                    callmon$$load_routine (image, subrec);

                /* Point to next subrecord. */
                /* EGSD length are rounded up to quadword boundary */

                if (subrec->egst$w_size % 8 == 0)
                    subrec = (egst_t*) ((char*)subrec +
                        subrec->egst$w_size);
                else
                    subrec = (egst_t*) ((char*)subrec +
                         subrec->egst$w_size + 8 -
                         (subrec->egst$w_size % 8));
            }
        }

        /* Next GST record is at a word boundary */

        record += rec_size + rec_size % 2;
    }
}


/*******************************************************************************
 *
 *  This routine allocates the image_t structure of an image, given its IMCB,
 *  and inserts it in the image tree.
 *  Return NULL in case of error. All errors are displayed.
 */

image_t* callmon$$load_image (imcb_t* imcb)
{
    uint32 status;
    int32 header_size;
    image_t* image;
    range_t inadr;
    range_t retadr;
    libtree_t* inserted;
    struct FAB fab;
    struct NAM nam;
    struct RAB rab;
    struct XABFHC xabfhc;
    char first_block [DISK_BLOCK_SIZE];

    /* Allocate the structure */
    /* Initial content of image_t is all zeroes */

    image = callmon$$alloc (sizeof (image_t));
    memset (image, 0, sizeof (image_t));
    image->imcb = imcb;

    /* If the image is the main executable image, get its name from $GETJPI */

    if (imcb->imcb$b_act_code == IMCB$K_MAIN_PROGRAM) {

        item_t itmlst [5];
        item_t* itm;
        uint16 length;
        iosb_t iosb;
        char resultant [FILE_SPEC_SIZE];

        /* Get the file specification */

        itm = itmlst;
        set_item (itm++, JPI$_IMAGNAME, image->file_spec,
            sizeof (image->file_spec) - 1, &length);
        set_final_item (itm++);

        status = sys$getjpiw (0, 0, 0, itmlst, &iosb, 0, 0);

        if ($VMS_STATUS_SUCCESS (status))
            status = iosb.status;
        if (!$VMS_STATUS_SUCCESS (status)) {
            callmon$$putmsg (CALLMON$_GETJPI, 0, status);
            callmon$$free (image, sizeof (image_t));
            return NULL;
        }

        image->file_spec [length] = '\0';

        /* Get the name part of the main image */

        fab = cc$rms_fab;
        fab.fab$l_fna = image->file_spec;
        fab.fab$b_fns = length;
        fab.fab$l_nam = &nam;

        nam = cc$rms_nam;
        nam.nam$l_esa = resultant;
        nam.nam$b_ess = sizeof (resultant) - 1;
        nam.nam$b_nop |= NAM$M_SYNCHK;

        status = sys$parse (&fab);

        if (!$VMS_STATUS_SUCCESS (status)) {
            callmon$$putmsg (CALLMON$_RMSPARSE, 1, image->file_spec, status);
            callmon$$free (image, sizeof (image_t));
            return NULL;
        }

        if (nam.nam$b_name >= sizeof (image->logname))
            nam.nam$b_name = sizeof (image->logname) - 1;

        memcpy (image->logname, nam.nam$l_name, nam.nam$b_name);
        image->logname [nam.nam$b_name] = '\0';

    }

    /* For shareable images, get the "logname" field from the IMCB */

    else {

        if (imcb->imcb$t_log_image_name [0] >= sizeof (image->logname)) {
            callmon$$putmsg (CALLMON$_IMGNAMTOOLNG, 2,
                imcb->imcb$t_log_image_name, sizeof (image->logname));
            callmon$$free (image, sizeof (image_t));
            return NULL;
        }

        memcpy (image->logname, imcb->imcb$t_log_image_name + 1,
            imcb->imcb$t_log_image_name [0]);
        image->logname [imcb->imcb$t_log_image_name [0]] = '\0';
    }

    if (callmon$$own.trace_flags & TRACE_IMAGE)
        callmon$$putmsg (CALLMON$_LOADIMG, 6, image->logname,
            imcb->imcb$l_base_address,
            imcb->imcb$l_starting_address,
            imcb->imcb$l_end_address,
            imcb->imcb$ps_symbol_vector_address,
            imcb->imcb$l_symbol_vector_size);

    /* Check if the image is installed in system space with the /resident or
     * /share=address_data option. Also check if the image is protected
     * (ie installed with inner mode memory page protection. */

    image->resident = imcb->imcb$v_discontiguous ||
        imcb->imcb$v_data_resident || imcb->imcb$v_share_link;

    image->protected = imcb->imcb$v_protected;

    if (image->resident && image->protected)
        callmon$$putmsg (CALLMON$_RESIDENT, 1, image->logname,
            CALLMON$_PROTECTD, 1, image->logname);
    else if (image->resident)
        callmon$$putmsg (CALLMON$_RESIDENT, 1, image->logname);
    else if (image->protected)
        callmon$$putmsg (CALLMON$_PROTECTD, 1, image->logname);

    /* The replacement of BSR instructions can occur only if the image
     * is not in system space and not in a protected section. */

    image->replace_bsr = !image->resident && !image->protected;

    /* Open the image file with block I/O access */

    fab = cc$rms_fab;
    nam = cc$rms_nam;
    rab = cc$rms_rab;
    xabfhc = cc$rms_xabfhc;

    fab.fab$b_fac = FAB$M_BIO | FAB$M_GET;
    fab.fab$l_xab = (char*) &xabfhc;
    rab.rab$l_fab = &fab;

    if (imcb->imcb$b_act_code == IMCB$K_MAIN_PROGRAM) {

        /* The specification of the main image has already been fetched */

        fab.fab$l_fna = image->file_spec;
        fab.fab$b_fns = strlen (fab.fab$l_fna);

        status = sys$open (&fab);
    }

    else {

        /* Shareable images may be found in various directories. */

        char** dir;
        static char* shrdirs [] = {
            "SYS$SHARE:.EXE",
            "SYS$MESSAGE:.EXE",
            "SYS$LOADABLE_IMAGES:.EXE",
            NULL};

        /* Also use the $OPEN to get the complete file specification of the
         * image (already done for main image, not for shareable images). */

        fab.fab$l_fna = imcb->imcb$t_log_image_name + 1;
        fab.fab$b_fns = imcb->imcb$t_log_image_name [0];
        fab.fab$l_nam = &nam;
        nam.nam$l_rsa = image->file_spec;
        nam.nam$b_rss = sizeof (image->file_spec) - 1;

        for (dir = shrdirs; *dir != NULL; dir++) {

            fab.fab$l_dna = *dir;
            fab.fab$b_dns = strlen (fab.fab$l_dna);

            status = sys$open (&fab);

            if ($VMS_STATUS_SUCCESS (status))
                image->file_spec [nam.nam$b_rsl] = '\0';

            if (status != RMS$_FNF)
                break;
        }
    }

    if (!$VMS_STATUS_SUCCESS (status)) {
        callmon$$putmsg (CALLMON$_OPEN, 1, image->logname,
            status, fab.fab$l_stv, CALLMON$_TONOTMON, 0);
        callmon$$free (image, sizeof (image_t));
        return NULL;
    }

    status = sys$connect (&rab);

    if (!$VMS_STATUS_SUCCESS (status)) {
        callmon$$putmsg (CALLMON$_OPEN, 1, image->file_spec,
            status, rab.rab$l_stv);
        sys$close (&fab);
        callmon$$free (image, sizeof (image_t));
        return NULL;
    }

    /* Save file size in image_t */

    image->file_blocks = xabfhc.xab$w_ffb > 0 ?
        xabfhc.xab$l_ebk : xabfhc.xab$l_ebk - 1;

    /* Read the first block of the file. The actual size of the header
     * will be found here. */

    rab.rab$l_bkt = 1; /* first VBN */
    rab.rab$l_ubf = first_block;
    rab.rab$w_usz = sizeof (first_block);

    status = sys$read (&rab);

    if (!$VMS_STATUS_SUCCESS (status)) {
        callmon$$putmsg (CALLMON$_READHEAD, 1, image->file_spec,
            status, rab.rab$l_stv);
        sys$close (&fab);
        callmon$$free (image, sizeof (image_t));
        return NULL;
    }

    /* Compute the header size (in blocks) */

    header_size = (((EIHD*)first_block)->eihd$l_size + DISK_BLOCK_SIZE - 1) /
        DISK_BLOCK_SIZE;

    /* Allocate the appropriate number of pagelets */

    status = lib$get_vm_page (&header_size, &image->eihd);

    if (!$VMS_STATUS_SUCCESS (status)) {
        callmon$$putmsg (CALLMON$_GETVM, 1, header_size * DISK_BLOCK_SIZE,
            status);
        sys$close (&fab);
        callmon$$free (image, sizeof (image_t));
        return NULL;
    }

    /* Read the complete image header */

    rab.rab$l_bkt = 1; /* first VBN */
    rab.rab$l_ubf = (char*) image->eihd;
    rab.rab$w_usz = header_size * DISK_BLOCK_SIZE;

    status = sys$read (&rab);

    if (!$VMS_STATUS_SUCCESS (status)) {
        callmon$$putmsg (CALLMON$_READHEAD, 1, image->file_spec,
            status, rab.rab$l_stv);
        sys$close (&fab);
        callmon$$free (image, sizeof (image_t));
        return NULL;
    }

    sys$close (&fab);

    /* Find the symbol table descriptor in the image header */

    if (image->eihd->eihd$l_symdbgoff != 0)
        image->eihs = (EIHS*) ((char*) image->eihd +
            image->eihd->eihd$l_symdbgoff);

    /* Find the table of image section descriptors in the image header.
     * The EISD table is the last part of the image header. The end of
     * the table is at the end of the image header. */

    if (image->eihd->eihd$l_isdoff != 0)
        image->eisd = (EISD*) ((char*) image->eihd +
            image->eihd->eihd$l_isdoff);

    /* Find the address of the fixup section. For shareable images,
     * the IAF VA is relative to the image base address. */

    if (imcb->imcb$b_act_code == IMCB$K_MAIN_PROGRAM)
        image->eiaf = image->eihd->eihd$l_iafva;

    else if (imcb->imcb$l_base_address != NULL &&
        image->eihd->eihd$l_iafva != NULL)
        image->eiaf = (EIAF*) ((char*) imcb->imcb$l_base_address +
            (int32) image->eihd->eihd$l_iafva);

    /* Find a channel on the image. If the image activator has left
     * an open channel in the IMCB, we use this one. Otherwise, we
     * open a new one. */

    if (imcb->imcb$w_chan != 0)
        image->channel = imcb->imcb$w_chan;

    else {
        fab = cc$rms_fab;
        fab.fab$b_fac = FAB$M_GET;
        fab.fab$l_fop = FAB$M_UFO;
        fab.fab$l_fna = image->file_spec;
        fab.fab$b_fns = strlen (fab.fab$l_fna);

        status = sys$open (&fab);

        if (!$VMS_STATUS_SUCCESS (status)) {
            callmon$$putmsg (CALLMON$_OPEN, 1, image->file_spec,
                status, fab.fab$l_stv);
            callmon$$free (image, sizeof (image_t));
            return NULL;
       }

       image->channel = fab.fab$l_stv;
    }

    /* Map the Global Symbol Table (GST) of the image */

    if (image->eihs != NULL && image->eihs->eihs$l_gstvbn != 0) {

        inadr.base = inadr.end = NULL;

        status = sys$crmpsc (&inadr, &retadr, 0, SEC$M_EXPREG, 0, 0, 0,
            image->channel,
            image->file_blocks - image->eihs->eihs$l_gstvbn + 1,
            image->eihs->eihs$l_gstvbn, 0, 0);

        if ($VMS_STATUS_SUCCESS (status))
            image->gst = retadr.base;

        else {
            image->gst = NULL;
            callmon$$putmsg (CALLMON$_MAPGST, 1, image->file_spec,
                status, CALLMON$_TONOTMON, 0);
        }

    }

    /* Insert all routines declared in the GST of the shareable image
     * inside the tree of routines. */

    store_routines (image);

    /* Insert the image in the tree of images */

    status = lib$insert_tree (&callmon$$own.image_tree, image->logname, &0,
        image_tree_compare, callmon$$tree_allocate_from_user_data,
        &inserted, &image->tree_node);

    if (!$VMS_STATUS_SUCCESS (status)) {
        callmon$$putmsg (CALLMON$_INSIMGTREE, 1, image->logname, status);
        return NULL;
    }

    if (callmon$$own.trace_flags & TRACE_IMAGE)
        callmon$$putmsg (CALLMON$_LDIMGOK, 2, image->logname,
            image->routine_count);

    return image;
}
