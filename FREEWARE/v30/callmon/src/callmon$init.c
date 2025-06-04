/*  CALLMON - A Call Monitor for OpenVMS Alpha
 *
 *  File:     CALLMON$INIT.C
 *  Author:   Thierry Lelegard
 *  Version:  1.0
 *  Date:     24-JUL-1996
 *
 *  Abstract: This module contains CALLMON initialization routine.
 */

#define DEFINE_CALLMON_OWN_STORAGE 1 /* allocate callmon$$own */
#include "callmon$private.h"


/*
 *  This redefinition of CALLMON$_FACILITY forces a reference to
 *  the message object file. This is necessary to have it included
 *  in the application image file.
 */

#ifdef CALLMON$_FACILITY
#undef CALLMON$_FACILITY
#endif

#pragma extern_model save
#pragma extern_model globalvalue
extern unsigned int CALLMON$_FACILITY;
static unsigned int unused_facility = CALLMON$_FACILITY;
#pragma extern_model restore


/*******************************************************************************
 *
 *  The following declarations include CALLMON initialization routine
 *  in psect LIB$INITIALIZE. Note that DEC C V5.3 or higher is required
 *  to include psect attributes in pragma extern_model.
 *
 *  These declarations are not compiled by default. If the macro 
 *  CALLMON_AUTO_INITIALIZE is not defined (default) CALLMON is
 *  initialized during the first call to one of its public routines.
 */

#ifdef CALLMON_AUTO_INITIALIZE

void LIB$INITIALIZE ();
static void (*dummy)() = LIB$INITIALIZE;  /* force a reference */

#pragma extern_model save
#pragma extern_model strict_refdef "LIB$INITIALIZE" long, gbl, noshr, nowrt
uint32 (*const callmon$$initialize_vector) (void) = callmon$initialize;
#pragma extern_model restore

#endif


/*******************************************************************************
 *
 *  This routine initializes the trace flags according to logical name.
 *
 *  The logical name CALLMON$TRCE should be defined as a search list of
 *  keywords. The list of keywords is defined in the trace_flags_list array.
 *
 *  Examples:
 *
 *      $ DEFINE CALLMON$TRACE RELOCATION, FIXUP
 *      $ DEFINE CALLMON$TRACE ALL
 */

static void init_trace_flags (void)
{
    uint32 status;
    int32 flag;
    int count;
    int index;
    char value [256];

    if ((count = callmon$$translation_count ("CALLMON$TRACE", NULL)) <= 0)
        return;

    for (index = 0; index < count; index++) {

        status = callmon$$translate ("CALLMON$TRACE", value, sizeof (value),
            NULL, index);

        if (!$VMS_STATUS_SUCCESS (status))
            continue;

        flag = callmon$$name_to_value (callmon$$trace_flags_names, value, 0);

        if (flag == 0)
            callmon$$putmsg (CALLMON$_IVTRFLG, 1, value);
        else
            callmon$$own.trace_flags |= flag;
    }

    if (callmon$$own.trace_flags != 0)
        callmon$$putmsg (CALLMON$_TRFLG, 1, callmon$$own.trace_flags);
}


/*******************************************************************************
 *
 *  This routine initializes the logical name CALLMON$TRACE_DIRECTORY.
 *  If it is not defined, define it to SYS$DISK:[] in used mode.
 */

static void init_trace_directory (void)
{
    uint32 status;
    item_t* itm;
    item_t itmlst [5];
    char value [FILE_SPEC_SIZE];

    static $DESCRIPTOR (name_d, "CALLMON$TRACE_DIRECTORY");
    static $DESCRIPTOR (table_d, "LNM$PROCESS_TABLE");

    /* If the logical name exists, return */

    status = callmon$$translate (name_d.dsc$a_pointer, value, sizeof (value),
        NULL, 0);

    if ($VMS_STATUS_SUCCESS (status))
        return;

    /* Otherwise, define to default value */

    itm = itmlst;
    set_str_item (itm++, LNM$_STRING, "SYS$DISK:[]");
    set_final_item (itm++);

    status = sys$crelnm (0, &table_d, &name_d, 0, itmlst);

    if (!$VMS_STATUS_SUCCESS (status))
        callmon$$putmsg (CALLMON$_DEFINE, 1, name_d.dsc$a_pointer, status);
}


/*******************************************************************************
 *
 *  This routine determines the BSR replacement according to logical name
 *  CALLMON$BSR_REPLACEMENT. See file CALLMON$BRANCH,C for the usage of
 *  the BSR replacement.
 */

static void init_bsr_replacement (void)
{
    static name_value_t name_list [] = {
        {BSR_OFF,    "OFF"},
        {BSR_IF_R27, "IF_R27"},
        {BSR_ALL,    "ALL"},
        {0,          NULL}
    };

    uint32 status;
    int32 value;
    char str [80];

    status = callmon$$translate ("CALLMON$BSR_REPLACEMENT", str, sizeof (str),
        NULL, 0);

    if (!$VMS_STATUS_SUCCESS (status))
        return;

    value = callmon$$name_to_value (name_list, str, -1);

    if (value < 0)
        callmon$$putmsg (CALLMON$_IVBSRREP, 1, str);
    else {
        callmon$$own.bsr_replacement = value;
        if (callmon$$own.trace_flags & TRACE_CONFIGURATION)
            callmon$$putmsg (CALLMON$_BSRREP, 2, str,
                callmon$$own.bsr_replacement);
    }
}


/*******************************************************************************
 *
 *  This routine initializes the BSR depth according to logical name
 *  CALLMON$BSR_DEPTH. See file CALLMON$BRANCH,C for the usage of the
 *  BSR depth.
 */

static void init_bsr_depth (void)
{
    uint32 status;
    int val;
    char str [80];

    status = callmon$$translate ("CALLMON$BSR_DEPTH", str, sizeof (str),
        NULL, 0);

    if ($VMS_STATUS_SUCCESS (status)) {
        if ((val = atoi (str)) <= 0)
            callmon$$putmsg (CALLMON$_INVBSRDEP, 1, str);
        else {
            callmon$$own.bsr_depth = val;
            if (callmon$$own.trace_flags & TRACE_CONFIGURATION)
                callmon$$putmsg (CALLMON$_BSRDEPTH, 1, callmon$$own.bsr_depth);
        }
    }
}


/*******************************************************************************
 *
 *  CALLMON initialization routine.
 *
 *  Called during the LIB$INITIALIZE process.
 */

uint32 callmon$initialize (void)
{
    imcb_t* imcb;
    int image_count;
    int routine_count;
    image_t* image;
    char** name;
    int not_monitored;
    thread_state_t thread_state;

    /* Execute only once */

    static int done = 0;

    if (done)
        return SS$_NORMAL;

    done = 1;

    callmon$$disable_threading (&thread_state);

    /* Get initial state */

    init_trace_flags ();

    if (callmon$$own.trace_flags)
        callmon$$putmsg (CALLMON$_INIT, 0);

    init_trace_directory ();
    init_bsr_replacement ();
    init_bsr_depth ();

    callmon$$own.uninterceptable_routines_sup = 
        callmon$$search_list ("CALLMON$NO_INTERCEPTION", NULL);

    callmon$$own.unrelocatable_routines_sup =
        callmon$$search_list ("CALLMON$NO_RELOCATION", NULL);

    /* Load all image descriptors */

    image_count = 0;
    routine_count = 0;
    not_monitored = 0;

    for (imcb = iac$gl_image_list.imcb$l_blink;
         imcb != &iac$gl_image_list;
         imcb = imcb->imcb$l_blink) {

        if ((image = callmon$$load_image (imcb)) != NULL) {
            image_count++;
            routine_count += image->routine_count;
            if (image->resident && image->protected)
                not_monitored++;
        }
    }

    if (not_monitored > 0)
        callmon$$putmsg (CALLMON$_FROMNOTMON, 0);

    /* Record uninterceptable routines */

    for (name = callmon$$own.uninterceptable_routines; *name != NULL; name++)
        callmon$$set_uninterceptable (callmon$$get_routine_by_name (*name));

    if (callmon$$own.uninterceptable_routines_sup != NULL) {

        for (name = callmon$$own.uninterceptable_routines_sup;
             *name != NULL; name++)
            callmon$$set_uninterceptable (callmon$$get_routine_by_name (*name));

        if (callmon$$own.trace_flags & TRACE_CONFIGURATION)
            for (name = callmon$$own.uninterceptable_routines_sup;
                 *name != NULL; name++)
                callmon$$putmsg (CALLMON$_UDUNINT, 1, *name);
    }

    /* Record unrelocatable routines */

    for (name = callmon$$own.unrelocatable_routines; *name != NULL; name++)
        callmon$$set_unrelocatable (callmon$$get_routine_by_name (*name));

    if (callmon$$own.unrelocatable_routines_sup != NULL) {

        for (name = callmon$$own.unrelocatable_routines_sup;
             *name != NULL; name++)
            callmon$$set_unrelocatable (callmon$$get_routine_by_name (*name));

        if (callmon$$own.trace_flags & TRACE_CONFIGURATION)
            for (name = callmon$$own.unrelocatable_routines_sup;
                 *name != NULL; name++)
                callmon$$putmsg (CALLMON$_UDUNREL, 1, *name);
    }

    /* End of initialization */

    if (callmon$$own.trace_flags)
        callmon$$putmsg (CALLMON$_INOK, 2, image_count, routine_count);

    callmon$$restore_threading (&thread_state);

    return SS$_NORMAL;
}
