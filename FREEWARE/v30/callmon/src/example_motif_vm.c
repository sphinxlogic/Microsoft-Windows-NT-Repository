/*  CALLMON Examples
 *
 *  File:     EXAMPLE_MOTIF_VM.C
 *  Author:   Thierry Lelegard
 *  Version:  1.0
 *  Date:     24-JUL-1996
 *
 *  Abstract: This module uses CALLMON to trace all routines allocating
 *            virtual memory. Can be used with EXAMPLE_MOTIF.C.
 *
 *            Use option "-nocallmon" in command line to run the program
 *            without the interception of memory allocation routines.
 *
 *            Use option "-list" to display a list of intercepted routines.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stsdef.h>
#include <starlet.h>
#include "callmon.h"

static int active_phase = 0;


/*******************************************************************************
 *
 *  General pre- and post-processing routine for CALLMON.
 *  We simply display a message. The boolean "active_phase" is used
 *  to avoid tracing the usage of memory during initialization.
 */

static void interceptor (
    callmon$arguments_t* arguments,
    uint32               caller_invo_handle,
    char*                routine_name,
    uint64               (*intercepted_routine)(),
    uint64               (*jacket_routine)())
{
    if (active_phase) {
        if (arguments->post_processing)
            printf ("<== %s\n", routine_name);
        else
            printf ("==> %s (%d arg%s)\n", routine_name,
                arguments->arg_count, arguments->arg_count > 1 ? "s" : "");
    }
}


/*******************************************************************************
 *
 *  Initialization routine called by the application.
 */

void init_call_monitor (int argc, char** argv)
{
    uint32 status;
    char** name;
    int arg;
    int show_list = 0;

    /* List of routines involved in virtual memory allocation */

    static char* routine_list [] = {
        "DECC$CALLOC",
        "DECC$CALLOC_OPT",
        "DECC$CFREE",
        "DECC$CFREE_OPT",
        "DECC$FREE",
        "DECC$FREE_OPT",
        "DECC$MALLOC",
        "DECC$MALLOC_OPT",
        "DECC$REALLOC",
        "DECC$VAXC$CALLOC_OPT",
        "DECC$VAXC$CFREE_OPT",
        "DECC$VAXC$FREE_OPT",
        "DECC$VAXC$MALLOC_OPT",
        "DECC$VAXC$REALLOC_OPT",
        "DECC$_CALLOC64",
        "DECC$_MALLOC64",
        "DECC$_REALLOC64",
        "LIB$$GET_VM_POINTERS",
        "LIB$$GET_VM_POINTERS_64",
        "LIB$CREATE_USER_VM_ZONE",
        "LIB$CREATE_USER_VM_ZONE_64",
        "LIB$CREATE_VM_ZONE",
        "LIB$CREATE_VM_ZONE_64",
        "LIB$DELETE_VM_ZONE",
        "LIB$DELETE_VM_ZONE_64",
        "LIB$FIND_VM_ZONE",
        "LIB$FIND_VM_ZONE_64",
        "LIB$FREE_VM",
        "LIB$FREE_VM_64",
        "LIB$FREE_VM_PAGE",
        "LIB$FREE_VM_PAGE_64",
        "LIB$GET_VM",
        "LIB$GET_VM_64",
        "LIB$GET_VM_PAGE",
        "LIB$GET_VM_PAGE_64",
        "LIB$RESET_VM_ZONE",
        "LIB$RESET_VM_ZONE_64",
        "LIB$SHOW_VM",
        "LIB$SHOW_VM_64",
        "LIB$SHOW_VM_ZONE",
        "LIB$SHOW_VM_ZONE_64",
        "LIB$STAT_VM",
        "LIB$STAT_VM_64",
        "LIB$VERIFY_VM_ZONE",
        "LIB$VERIFY_VM_ZONE_64",
        "LIB$VM_CALLOC",
        "LIB$VM_CALLOC_64",
        "LIB$VM_FREE",
        "LIB$VM_FREE_64",
        "LIB$VM_MALLOC",
        "LIB$VM_MALLOC_64",
        "LIB$VM_REALLOC",
        "LIB$VM_REALLOC_64",
        "XMEMORY_CALLOC",
        "XMEMORY_FREE",
        "XMEMORY_MALLOC",
        "XMEMORY_REALLOC",
        "XTCALLOC",
        "XTFREE",
        "XTMALLOC",
        "XTREALLOC",
        "_XTHEAPALLOC",
        "_XTHEAPFREE",
        "_XTHEAPINIT",
        "_XMHEAPALLOC",
        "_XMHEAPCREATE",
        "_XMHEAPFREE",
        NULL};

    /* If "-nocallmon" is specified, keep silent */

    for (arg = 1; arg < argc; arg++) {
        if (strcmp (argv [arg], "-nocallmon") == 0)
            return;
        else if (strcmp (argv [arg], "-list") == 0)
            show_list = 1;
    }

    /* Intercept all memory allocation routines */

    active_phase = 0;

    for (name = routine_list; *name != NULL; name++) {

        printf ("Intercepting %s\n", *name);

        status = callmon$intercept (*name, interceptor, interceptor);

        if (!$VMS_STATUS_SUCCESS (status)) {
            uint32 msgvec [2];
            msgvec [0] = 1;
            msgvec [1] = status;
            sys$putmsg (msgvec, 0, 0, 0);
        }
    }

    /* Display a list of intercepted routines */

    if (show_list) {
        printf ("\n*** Intercepted routines:\n");
        callmon$dump_routine_tree (CALLMON$C_INTERCEPTED_ONLY);
        printf ("\n*** Non-interceptable routines:\n");
        callmon$dump_routine_tree (CALLMON$C_UNINTERCEPTABLE_ONLY);
        printf ("\n*** Non-relocatable routines:\n");
        callmon$dump_routine_tree (CALLMON$C_UNRELOCATABLE_ONLY);
        printf ("\n");
    }

    active_phase = 1;
}
