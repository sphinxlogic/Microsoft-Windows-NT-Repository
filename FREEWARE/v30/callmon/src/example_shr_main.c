/*  CALLMON Examples
 *
 *  File:     EXAMPLE_SHR_MAIN.C
 *  Author:   Thierry Lelegard
 *  Version:  1.0
 *  Date:     24-JUL-1996
 *
 *  Abstract: Calls the shareable image EXAMPLE_SHR.EXE.
 */


#include <stdio.h>
#include <stdlib.h>
#include <ssdef.h>
#include <stsdef.h>
#include <pdscdef.h>
#include <libicb.h>
#include <starlet.h>
#include <lib$routines.h>
#include "callmon.h"

extern int shr_proc ();
void end_of_main (void);

#define entry_of(routine) (((struct pdscdef*)(routine))->pdsc$l_entry)


/*
 *  Pre- and post-processing routine for CALLMON.
 *  Simply trace arguments.
 */

static void trace (
    callmon$arguments_t* arguments,
    uint32               caller_invo_handle,
    char*                routine_name,
    uint64               (*intercepted_routine)(),
    uint64               (*jacket_routine)())
{
    int arg;
    struct invo_context_blk invo;

    printf ("%s calling %s at %08X, jacket at %08X\n",
        arguments->post_processing ? "AFTER" : "BEFORE",
        routine_name,
        intercepted_routine,
        jacket_routine);

    /* In preprocessing phase, display caller information */

    if (!arguments->post_processing) {

        /* Display argument list */

        for (arg = 0; arg < arguments->arg_count; arg++) {
            printf ("    arg%-2d = %016LX (%e)\n", arg + 1,
                arguments->arg_list [arg], arguments->arg_list [arg]);
        }

        /* Display caller's PC */

        if (lib$get_invo_context (caller_invo_handle, &invo))
            printf ("Caller PC = %08X\n", invo.libicb$q_program_counter [0]);
        else
            printf ("Invalid caller handle: %08X\n", caller_invo_handle);
    }

    /* In post-processing phase, display the results */

    else {
        printf ("    Result R0 = %016LX\n", arguments->result_r0);
        printf ("    Result R1 = %016LX\n", arguments->result_r1);
        printf ("    Result F0 = %016LX\n", arguments->result_f0);
        printf ("    Result F1 = %016LX\n", arguments->result_f1);
    }
}


/*
 *  Pre--processing routine for CALLMON.
 *  Modify some parameters in the argument list.
 */

static void modify_arglist (
    callmon$arguments_t* arguments,
    uint32               caller_invo_handle,
    char*                routine_name,
    uint64               (*intercepted_routine)(),
    uint64               (*jacket_routine)())
{
    printf ("Before calling %s, setting a1 to 75 and a9 to 96\n", routine_name);

    arguments->arg_list [0] = 75;
    arguments->arg_list [8] = 96;
}


/*
 *  Pre--processing routine for CALLMON.
 *  Skip actual procedure call.
 */

static void skip_call (
    callmon$arguments_t* arguments,
    uint32               caller_invo_handle,
    char*                routine_name,
    uint64               (*intercepted_routine)(),
    uint64               (*jacket_routine)())
{
    arguments->call_it = 0;
    arguments->result_r0 = 1234;

    printf ("Skip call to %s, setting result to %d (0x%08X)\n",
        routine_name, arguments->result_r0, arguments->result_r0);
}


/*
 *  Test program entry point.
 */

main ()
{
    uint32 status;

    /* First test: simple trace */

    status = callmon$intercept ("SHR_PROC", trace, trace);
    if (!$VMS_STATUS_SUCCESS (status))
        sys$exit (status);

    printf ("First test: simple trace\n");
    printf ("Caller code range: %08X-%08X\n",
        entry_of (main), entry_of (end_of_main));
    shr_proc (1, 2.0, 3, 4.0, 5, 6.0, 7, 8.0, 9, 10.0);    

    /* Second test: modify argument list */

    status = callmon$intercept ("SHR_PROC", modify_arglist, trace);
    if (!$VMS_STATUS_SUCCESS (status))
        sys$exit (status);

    printf ("Second test: modify some parameters\n");
    shr_proc (1, 2.0, 3, 4.0, 5, 6.0, 7, 8.0, 9, 10.0);    

    /* Third test: skip routine call */

    status = callmon$intercept ("SHR_PROC", skip_call, trace);
    if (!$VMS_STATUS_SUCCESS (status))
        sys$exit (status);

    printf ("Third test: skip actual call\n");
    shr_proc (1, 2.0, 3, 4.0, 5, 6.0, 7, 8.0, 9, 10.0);    
}

void end_of_main (void) {}
