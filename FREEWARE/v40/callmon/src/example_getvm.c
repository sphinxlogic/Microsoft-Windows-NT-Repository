/*  CALLMON Examples
 *
 *  File:     EXAMPLE_GETVM.C
 *  Author:   Thierry Lelegard
 *  Version:  1.0
 *  Date:     24-JUL-1996
 *
 *  Abstract: Simple interception example program.
 *            Intercept LIB$GET_VM and LIB$FREE_VM and trace the calls.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <builtins.h>
#include <psldef.h>
#include <ssdef.h>
#include <stsdef.h>
#include <starlet.h>
#include <lib$routines.h>
#include "callmon.h"


/*
 *  General pre- and post-processing routine for CALLMON.
 */

static void interceptor (
    callmon$arguments_t* arguments,
    uint32               caller_invo_handle,
    char*                routine_name,
    uint64               (*intercepted_routine)(),
    uint64               (*jacket_routine)())
{
    int arg;
    uint64* ref;

    printf ("%s calling %s at %08X, jacket at %08X\n",
        arguments->post_processing ? "AFTER" : "BEFORE",
        routine_name,
        intercepted_routine,
        jacket_routine);

    /* Print the argument list, try to derefence pointers */

    printf ("    arg count = %d\n", arguments->arg_count);
    for (arg = 0; arg < arguments->arg_count; arg++) {
        printf ("    arg%-2d = %016LX", arg + 1, arguments->arg_list [arg]);
        ref = (uint64*) arguments->arg_list [arg];
        if (__PAL_PROBER (ref, sizeof (uint64), PSL$C_USER))
            printf (" -> %016LX", *ref);
        printf ("\n");
    }

    /* In the post-processing phase, display the results */

    if (arguments->post_processing) {
        printf ("    Result R0 = %016LX\n", arguments->result_r0);
        printf ("    Result R1 = %016LX\n", arguments->result_r1);
        printf ("    Result F0 = %016LX\n", arguments->result_f0);
        printf ("    Result F1 = %016LX\n", arguments->result_f1);
    }
}


/*
 *  Test program entry point.
 */

main ()
{
    uint32 status;
    uint32 size;
    char* ptr;

    printf ("Before intercepting LIB$GET_VM = %08X, LIB$FREE_VM = %08X\n",
        lib$get_vm, lib$free_vm);

    status = callmon$intercept ("LIB$GET_VM", interceptor, interceptor);

    if (!$VMS_STATUS_SUCCESS (status))
        sys$exit (status);

    status = callmon$intercept ("LIB$FREE_VM", interceptor, interceptor);

    if (!$VMS_STATUS_SUCCESS (status))
        sys$exit (status);

    printf ("After intercepting LIB$GET_VM = %08X, LIB$FREE_VM = %08X\n",
        lib$get_vm, lib$free_vm);

    printf ("\n");
    callmon$dump_routine_tree (CALLMON$C_INTERCEPTED_ONLY);
    printf ("\n");

    size = 100;
    printf ("Calling LIB$GET_VM (100), &size = %08X, &ptr = %08X\n",
        &size, &ptr);
    status = lib$get_vm (&size, &ptr);
    printf ("Result of LIB$GET_VM (100) = %08X, status = %08X\n", ptr, status);

    status = lib$free_vm (&size, &ptr);
    printf ("Result of LIB$FREE_VM (100) = %08X, status = %08X\n", ptr, status);
}
