/*  CALLMON - A Call Monitor for OpenVMS Alpha
 *
 *  File:     CALLMON$DUMP.C
 *  Author:   Thierry Lelegard
 *  Version:  1.0
 *  Date:     24-JUL-1996
 *
 *  Abstract: This module contains the routines which dumps the list
 *            of images and routines.
 */


#include "callmon$private.h"


/*******************************************************************************
 *
 *  Dump a node of the tree of images. Action routine for LIB$TRAVERSE_TREE.
 */

static uint32 dump_image (image_t* image, void* user_data)
{
    printf ("%s at %08X, symbol vector at %08X\n", image->logname,
        image->imcb->imcb$l_base_address,
        image->imcb->imcb$ps_symbol_vector_address);

    printf ("    %s (%d blocks)\n", image->file_spec, image->file_blocks);

    return SS$_NORMAL;
}


/*******************************************************************************
 *
 *  Dump the tree of images.
 */

uint32 callmon$dump_image_tree (void)
{
    uint32 status;
    thread_state_t thread_state;

    callmon$initialize ();
    callmon$$disable_threading (&thread_state);

    status = lib$traverse_tree (&callmon$$own.image_tree, dump_image, NULL);

    callmon$$restore_threading (&thread_state);

    return status;
}


/*******************************************************************************
 *
 *  Dump a node of the tree of routines. Action routine for LIB$TRAVERSE_TREE.
 */

static uint32 dump_routine (libtree_t* node, callmon$dump_routine_t rclass)
{
    int32 entry;
    routine_t* dup;
    routine_t* routine;

    /* Loop on all routines with that name */

    for (routine = BASE (routine_t, by_name, node);
         routine != NULL;
         routine = routine->next_by_name) {

        /* If the routine has several names, only display if first name */

        if (routine != routine->master_by_value)
            continue;

        /* Display the routine only if requested */

        if (rclass == CALLMON$C_INTERCEPTED_ONLY && routine->jacket == NULL)
            continue;

        if (rclass == CALLMON$C_UNRELOCATABLE_ONLY && !routine->unrelocatable)
            continue;

        if (rclass == CALLMON$C_UNINTERCEPTABLE_ONLY &&
            !routine->uninterceptable)
            continue;

        /* Display the name and value of the symbol */

        printf ("%-*s %08X %08X", LINK_SYMBOL_SIZE - 1, routine->name,
            routine->lkp.lkp$q_proc_value, routine->lkp.lkp$q_entry);

        /* If the routine has been revectored, display new value */

        if (routine->jacket != NULL)
            printf (" %08X %08X", routine->jacket,
                routine->jacket->pdsc$l_entry);

        printf ("\n");

        /* Sanity check for the linkage pair */

        entry = ((pdsc_t*)routine->lkp.lkp$q_proc_value)->pdsc$l_entry;

        if (routine->lkp.lkp$q_entry != entry)
            printf ("    [Entry inconsistency: %08X in LKP, %08X in PDSC]\n",
                routine->lkp.lkp$q_entry, entry);

        /* List all duplicate names for this routine */

        for (dup = routine->master_by_value; dup; dup = dup->next_by_value) {
            if (dup != routine) {

                printf ("= %s\n", dup->name);

                /* Sanity checks */

                if (routine->lkp.lkp$q_proc_value != dup->lkp.lkp$q_proc_value)
                    printf ("    [Inconsistency: alias PDSC = %08X]\n",
                        dup->lkp.lkp$q_proc_value);

                if (routine->lkp.lkp$q_entry != dup->lkp.lkp$q_entry)
                    printf ("    [Inconsistency: alias entry = %08X]\n",
                        dup->lkp.lkp$q_entry);

                if (routine->jacket != dup->jacket)
                    printf ("    [Inconsistency: alias jacket is different]\n");
            }
        }
    }

    return SS$_NORMAL;
}


/*******************************************************************************
 *
 *  Dump the tree of routines.
 */

uint32 callmon$dump_routine_tree (callmon$dump_routine_t rclass)
{
    int n;
    uint32 status;
    thread_state_t thread_state;

    callmon$initialize ();
    callmon$$disable_threading (&thread_state);

    printf ("%-*s                   Jacket   Jacket\n",
        LINK_SYMBOL_SIZE - 1, "");
    printf ("%-*s Value    Entry    Value    Entry\n",
        LINK_SYMBOL_SIZE - 1, "Symbol");
    for (n = 1; n++ < LINK_SYMBOL_SIZE; printf ("-"));
    printf (" -------- -------- -------- --------\n");

    status = lib$traverse_tree (&callmon$$own.routine_tree_by_name,
        dump_routine, rclass);

    callmon$$restore_threading (&thread_state);

    return status;
}
