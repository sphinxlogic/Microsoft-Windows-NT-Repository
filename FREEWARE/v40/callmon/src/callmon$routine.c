/*  CALLMON - A Call Monitor for OpenVMS Alpha
 *
 *  File:     CALLMON$ROUTINE.C
 *  Author:   Thierry Lelegard
 *  Version:  1.0
 *  Date:     24-JUL-1996
 *
 *  Abstract: This module contains the routines which build the trees
 *            of routines.
 */


#include "callmon$private.h"


/*******************************************************************************
 *
 *  This routine is a "comparison routine" for LIB$*_TREE on the tree
 *  of routine descriptors by name.
 */

static int routine_compare_by_name (
    char*      routine_name,
    libtree_t* comparison_node,
    void*      user_data)
{
    routine_t* routine = BASE (routine_t, by_name, comparison_node);
    return strcmp (routine_name, routine->name);
}


/*******************************************************************************
 *
 *  This routine is a "comparison routine" for LIB$*_TREE on the tree
 *  of routine descriptors by value.
 */

static int routine_compare_by_value (
    int64*     routine_value,
    libtree_t* comparison_node,
    void*      user_data)
{
    routine_t* routine = BASE (routine_t, by_value, comparison_node);

    if (*routine_value == routine->lkp.lkp$q_proc_value)
        return 0;
    else if (*routine_value < routine->lkp.lkp$q_proc_value)
        return -1;
    else
        return 1;
}


/*******************************************************************************
 *
 *  This routine is a "comparison routine" for LIB$*_TREE on the tree
 *  of routine descriptors by entry.
 */

static int routine_compare_by_entry (
    int64*     routine_entry,
    libtree_t* comparison_node,
    void*      user_data)
{
    routine_t* routine = BASE (routine_t, by_entry, comparison_node);

    if (*routine_entry == routine->lkp.lkp$q_entry)
        return 0;
    else if (*routine_entry < routine->lkp.lkp$q_entry)
        return -1;
    else
        return 1;
}


/*******************************************************************************
 *
 *  These routines return a routine description.
 *  Return NULL if the requested routine is not found.
 */

routine_t* callmon$$get_routine_by_name (char* name)
{
    uint32 status;
    libtree_t* node;

    status = lib$lookup_tree (&callmon$$own.routine_tree_by_name,
        name, routine_compare_by_name, &node);

    if ($VMS_STATUS_SUCCESS (status))
        return BASE (routine_t, by_name, node);
    else if (status == LIB$_KEYNOTFOU)
        return NULL;
    else {
        callmon$$putmsg (CALLMON$_FNDNAME, 0, status);
        return NULL;
    }
}


routine_t* callmon$$get_routine_by_value (int64 value)
{
    uint32 status;
    libtree_t* node;

    status = lib$lookup_tree (&callmon$$own.routine_tree_by_value,
        &value, routine_compare_by_value, &node);

    if ($VMS_STATUS_SUCCESS (status))
        return BASE (routine_t, by_value, node);
    else if (status == LIB$_KEYNOTFOU)
        return NULL;
    else {
        callmon$$putmsg (CALLMON$_FNDVALUE, 0, status);
        return NULL;
    }
}


routine_t* callmon$$get_routine_by_entry (int64 entry)
{
    uint32 status;
    libtree_t* node;

    status = lib$lookup_tree (&callmon$$own.routine_tree_by_entry,
        &entry, routine_compare_by_entry, &node);

    if ($VMS_STATUS_SUCCESS (status))
        return BASE (routine_t, by_entry, node);
    else if (status == LIB$_KEYNOTFOU)
        return NULL;
    else {
        callmon$$putmsg (CALLMON$_FNDENTRY, 0, status);
        return NULL;
    }
}


/*******************************************************************************
 *
 *  These routines mark a routine as uninterceptable or unrelocatable.
 *  Accept a NULL parameter (does nothing).
 */

void callmon$$set_uninterceptable (routine_t* routine)
{
    if (routine != NULL) {
        for (routine = routine->master_by_entry;
             routine != NULL;
             routine = routine->next_by_entry) {

            routine->uninterceptable = 1;
        }
    }
}


void callmon$$set_unrelocatable (routine_t* routine)
{
    if (routine != NULL) {
        for (routine = routine->master_by_entry;
             routine != NULL;
             routine = routine->next_by_entry) {

            routine->unrelocatable = 1;
        }
    }
}


/*******************************************************************************
 *
 *  This routine insert a routine inside the tree of routines.
 *  The routine is described by its entry in the Global Symbol Table.
 *  Return NULL in case of error. All errors are displayed.
 */

routine_t* callmon$$load_routine (image_t* image, egst_t* egst)
{
    uint32 status;
    routine_t* routine;
    libtree_t* inserted;

    if (callmon$$own.trace_flags & TRACE_ROUTINE)
        callmon$$putmsg (CALLMON$_LOADRTN, 2,
            egst->egst$b_namlng, egst->egst$t_name);

    /* Allocate the routine_t node */

    routine = callmon$$alloc (sizeof (routine_t));

    /* Initialize record */

    memset (routine, 0, sizeof (*routine));
    routine->image = image;

    /* Store routine name in the record */

    if (egst->egst$b_namlng >= sizeof (routine->name)) {
        callmon$$putmsg (CALLMON$_RTNNAMTOOLNG, 3, egst->egst$b_namlng,
            egst->egst$t_name, sizeof (routine->name));
        callmon$$free (routine, sizeof (routine_t));
        return NULL;
    }

    memcpy (routine->name, egst->egst$t_name, egst->egst$b_namlng);
    routine->name [egst->egst$b_namlng] = '\0';
    set_str_desc (&routine->name_d, routine->name);

    /* Get the content of the linkage pair inside the symbol vector. */

    routine->lkp = *(lkp_t*)((char*)image->imcb->imcb$ps_symbol_vector_address +
        egst->egst$l_value);

    /* If the routine is in system space or in protected code section,
     * we will not be able to modify it. */

    routine->unrelocatable = image->resident || image->protected;

    /* Insert the routine in the tree of routines by name */

    status = lib$insert_tree (&callmon$$own.routine_tree_by_name,
        routine->name, &0,
        routine_compare_by_name,
        callmon$$tree_allocate_from_user_data,
        &inserted, &routine->by_name);

    if (!$VMS_STATUS_SUCCESS (status)) {
        callmon$$putmsg (CALLMON$_INSRTNTREE, 1, routine->name, status);
        callmon$$free (routine, sizeof (routine_t));
        return NULL;
    }

    /* Handle duplicate name (same routine name in two different images) */

    if (status != LIB$_KEYALRINS)
        routine->master_by_name = routine;

    else {
        routine->master_by_name = BASE (routine_t, by_name, inserted);
        routine->next_by_name = routine->master_by_name->next_by_name;
        routine->master_by_name->next_by_name = routine;

        if (callmon$$own.trace_flags & TRACE_DUPLICATE)
            callmon$$putmsg (CALLMON$_DUPNAM, 3,
                routine->name, routine->image->logname,
                routine->master_by_name->image->logname);
    }

    /* Insert the routine in the tree of routines by value */

    status = lib$insert_tree (&callmon$$own.routine_tree_by_value,
        &routine->lkp.lkp$q_proc_value, &0,
        routine_compare_by_value,
        callmon$$tree_allocate_from_user_data,
        &inserted, &routine->by_value);

    if (!$VMS_STATUS_SUCCESS (status)) {
        callmon$$putmsg (CALLMON$_INSRTNTREE, 1, routine->name, status);
        callmon$$free (routine, sizeof (routine_t));
        return NULL;
    }

    /* Handle duplicate value (two names for same procedure) */

    if (status != LIB$_KEYALRINS)
        routine->master_by_value = routine;

    else {
        routine->master_by_value = BASE (routine_t, by_value, inserted);
        routine->next_by_value = routine->master_by_value->next_by_value;
        routine->master_by_value->next_by_value = routine;
        routine->jacket = routine->master_by_value->jacket;

        if (callmon$$own.trace_flags & TRACE_DUPLICATE)
            callmon$$putmsg (CALLMON$_DUPVAL, 2, routine->name,
                routine->master_by_value->name);
    }

    /* Insert the routine in the tree of routines by entry */

    status = lib$insert_tree (&callmon$$own.routine_tree_by_entry,
        &routine->lkp.lkp$q_entry, &0,
        routine_compare_by_entry,
        callmon$$tree_allocate_from_user_data,
        &inserted, &routine->by_entry);

    if (!$VMS_STATUS_SUCCESS (status)) {
        callmon$$putmsg (CALLMON$_INSRTNTREE, 1, routine->name, status);
        callmon$$free (routine, sizeof (routine_t));
        return NULL;
    }

    /* Handle duplicate value (two names for same procedure) */

    if (status != LIB$_KEYALRINS)
        routine->master_by_entry = routine;

    else {
        routine->master_by_entry = BASE (routine_t, by_entry, inserted);
        routine->next_by_entry = routine->master_by_entry->next_by_entry;
        routine->master_by_entry->next_by_entry = routine;
        routine->jacket = routine->master_by_entry->jacket;

        if (callmon$$own.trace_flags & TRACE_DUPLICATE)
            callmon$$putmsg (CALLMON$_DUPVAL, 2, routine->name,
                routine->master_by_entry->name);
    }

    /* One more routine for this image */

    image->routine_count++;

    return routine;
}
