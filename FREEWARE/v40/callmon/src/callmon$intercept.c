/*  CALLMON - A Call Monitor for OpenVMS Alpha
 *
 *  File:     CALLMON$INTERCEPT.C
 *  Author:   Thierry Lelegard
 *  Version:  1.0
 *  Date:     24-JUL-1996
 *
 *  Abstract: This module contains the routines which intercept routines
 *            in shareable images.
 */


#include "callmon$private.h"


/*******************************************************************************
 *
 *  The following structure is a context block for LIB$TRAVERSE_TREE
 *  when all routines of a image are being intercepted.
 */

typedef struct {
    image_t* image;        /* Shareable image to intercept */
    pdsc_t*  pre_routine;  /* Pre-routine descriptor */
    pdsc_t*  post_routine; /* Post-routine descriptor */
    int      override;     /* Boolean: override already intercepted routines */
    int      use_desc;     /* Boolean: use string descriptor */
} intercept_ctx_t;


/*******************************************************************************
 *
 *  Jacket routines.
 *
 *  When a routine is intercepted, it is replaced by a jacket routine
 *  which receives its arguments. Module CALLMON$JACKET.M64 contains
 *  one jacket routine. Each time a new jacket routine is needed,
 *  we clone the linkage section of this routine, thus creating a
 *  new instance of the jacket routine. The cloned linkage section
 *  is filled with the routine-specific data.
 */

/*
 *  The following symbols are defined in file CALLMON$JACKET.M64.
 *  They define the various points inside the template linkage section.
 *  The char type is dummy and used to be able to compute offsets.
 */

#pragma extern_model save
#pragma extern_model strict_refdef

extern char callmon$$jacket;           /* Start of template linkage section */
extern char callmon$$clone_routine;    /* PDSC of original routine */
extern char callmon$$clone_pre;        /* PDSC of pre-processing routine */
extern char callmon$$clone_post;       /* PDSC of post-processing routine */
extern char callmon$$clone_name;       /* Name of original routine */
extern char callmon$$jacket_link_end;  /* End of template linkage section */

#pragma extern_model restore

/*
 *  When a jacket routine is cloned, we allocate a new linkage section.
 *  The original content of the template linkage section must be first
 *  copied into the cloned linkage section.
 */

#define CLONE_LINKAGE_SECTION_SIZE \
    ((char*) &callmon$$jacket_link_end - (char*) &callmon$$jacket)

/*
 *  Then, the routine-specific data are modified inside the clone
 *  linkage section. This includes the addresses of the original
 *  routine, the pre- and post-processing routines and the routine
 *  name.
 */

#define CLONE_LINKAGE_ADDR(routine,symbol) \
    ((int64*) ((char*) ((routine)->jacket) + \
    ((char*) &(callmon$$clone_##symbol) - (char*) &callmon$$jacket)))

#define CLONE_ROUTINE(routine) (*CLONE_LINKAGE_ADDR (routine, routine))
#define CLONE_PRE(routine)     (*CLONE_LINKAGE_ADDR (routine, pre))
#define CLONE_POST(routine)    (*CLONE_LINKAGE_ADDR (routine, post))
#define CLONE_NAME(routine)    (*CLONE_LINKAGE_ADDR (routine, name))


/*******************************************************************************
 *
 *  This routine modifies one image. It hooks one jacket routine.
 *  It is an action routine for LIB$TRAVERSE_TREE on the tree of images.
 *  The user_data is the routine_t structure.
 */

static uint32 hook_image (image_t* image, routine_t* routine)
{
    if (image != routine->image)
        callmon$$process_fixups (image, routine);
    else {
        callmon$$remove_bsr (image);
        callmon$$process_relocations (routine);
    }

    return SS$_NORMAL;
}


/*******************************************************************************
 *
 *  This routine intercepts one specific routine.
 */

static uint32 intercept_routine (
    routine_t* routine,
    pdsc_t*    pre_routine,
    pdsc_t*    post_routine,
    int        use_desc)
{
    uint32 status;
    void** res;
    routine_t* dup;

    /* If the routine is not interceptable, give up */

    if (routine->uninterceptable)
        return CALLMON$_RESROUT;

    /* If the routine has already a jacket routine, simply modify pre/post */

    if (routine->jacket != NULL) {
        CLONE_PRE (routine) = (int64) pre_routine;
        CLONE_POST (routine) = (int64) post_routine;
        CLONE_NAME (routine) = use_desc ?
            (int64) &routine->name_d : (int64) routine->name;
        return SS$_NORMAL;
    }

    /* Allocate one jacket routine. Note that the PDSC must be aligned on a
     * quadword boundary. Here, we rely on the fact that LIBVM's default
     * zone alignment attribute is also quadword. */

    routine->jacket = callmon$$alloc (CLONE_LINKAGE_SECTION_SIZE);

    /* The initial content of the clone linkage section is a copy
     * of the template linkage section. */

    memcpy (routine->jacket, &callmon$$jacket, CLONE_LINKAGE_SECTION_SIZE);

    /* Now fill the new clone linkage section with specific data */

    CLONE_ROUTINE (routine) = routine->lkp.lkp$q_proc_value;
    CLONE_PRE (routine) = (int64) pre_routine;
    CLONE_POST (routine) = (int64) post_routine;
    CLONE_NAME (routine) = use_desc ?
        (int64) &routine->name_d : (int64) routine->name;

    /* All routine_t for this procedure must point to the same jacket */

    for (dup = routine->master_by_value; dup; dup = dup->next_by_value)
        dup->jacket = routine->jacket;

    /* In all images, modify all references to the routine */

    return lib$traverse_tree (&callmon$$own.image_tree, hook_image, routine);
}


/*******************************************************************************
 *
 *  This public routine intercepts one specific routine.
 */

uint32 callmon$intercept (
    char*               routine_name,
    callmon$intercept_t pre_routine,
    callmon$intercept_t post_routine)
{
    uint32 status;
    uint32 final_status;
    routine_t* routine;
    thread_state_t thread_state;

    /* Initialize CALLMON if not yet done */

    callmon$initialize ();

    if (callmon$$own.trace_flags & TRACE_API)
        callmon$$putmsg (CALLMON$_INTERCEPT, 3, 
            routine_name, pre_routine, post_routine);

    callmon$$disable_threading (&thread_state);

    /* Find the description of the routine */

    if ((routine = callmon$$get_routine_by_name (routine_name)) == NULL)
        final_status = CALLMON$_RNF;

    else {

        /* Hook all routines with that name */

        final_status = SS$_NORMAL;

        for (routine = routine->master_by_name;
             routine != NULL;
             routine = routine->next_by_name) {

            status = intercept_routine (routine, (pdsc_t*) pre_routine,
                (pdsc_t*) post_routine, 0);

            if (!$VMS_STATUS_SUCCESS (status))
                final_status = status;
        }
    }

    callmon$$restore_threading (&thread_state);

    return final_status;
}


/*******************************************************************************
 *
 *  This public routine intercepts one specific routine.
 *  Use string descriptors (for non-C application).
 */

uint32 callmon$intercept_dsc (
    struct dsc$descriptor_s* routine_name,
    callmon$intercept_t      pre_routine,
    callmon$intercept_t      post_routine)
{
    uint32 status;
    uint32 final_status;
    char* routine_name_z;
    routine_t* routine;
    thread_state_t thread_state;

    /* Allocate a nul-terminated string on stack */

    routine_name_z = __ALLOCA (routine_name->dsc$w_length + 1);
    memcpy (routine_name_z, routine_name->dsc$a_pointer,
        routine_name->dsc$w_length);
    routine_name_z [routine_name->dsc$w_length] = '\0';

    /* Initialize CALLMON if not yet done */

    callmon$initialize ();

    if (callmon$$own.trace_flags & TRACE_API)
        callmon$$putmsg (CALLMON$_INTERCEPT, 3, 
            routine_name_z, pre_routine, post_routine);

    callmon$$disable_threading (&thread_state);

    /* Find the description of the routine */

    if ((routine = callmon$$get_routine_by_name (routine_name_z)) == NULL)
        final_status = CALLMON$_RNF;

    else {

        /* Hook all routines with that name */

        final_status = SS$_NORMAL;

        for (routine = routine->master_by_name;
             routine != NULL;
             routine = routine->next_by_name) {

            status = intercept_routine (routine, (pdsc_t*) pre_routine,
                (pdsc_t*) post_routine, 1);

            if (!$VMS_STATUS_SUCCESS (status))
                final_status = status;
        }
    }

    callmon$$restore_threading (&thread_state);

    return final_status;
}


/*******************************************************************************
 *
 *  Intercept one image.
 *  Action routine for LIB$TRAVERSE_TREE on the tree of routines by value.
 */

static uint32 intercept_all (libtree_t* node, intercept_ctx_t* ctx)
{
    routine_t* routine = BASE (routine_t, by_value, node);
    uint32 status;

    /* If no routine at this address belong to this image, don't interested */

    for (routine = routine->master_by_value;
         routine != NULL && routine->image != ctx->image;
         routine = routine->next_by_value);

    if (routine == NULL)
        return SS$_NORMAL;

    /* If routine already intercepted, override only on demand */

    if (!ctx->override && routine->jacket != NULL &&
        (CLONE_PRE (routine) != 0 || CLONE_POST (routine) != 0))
        return SS$_NORMAL;

    /* Hook the routine. Note that we must skip reserved routines
     * without reporting an error. */

    status = intercept_routine (routine, ctx->pre_routine, ctx->post_routine,
        ctx->use_desc);

    return status == CALLMON$_RESROUT ? SS$_NORMAL : status;
}


/*******************************************************************************
 *
 *  This public routine intercepts all routines in an image.
 */

uint32 callmon$intercept_all (
    char*               image_name,
    callmon$intercept_t pre_routine,
    callmon$intercept_t post_routine,
    callmon$boolean_t   override)
{
    uint32 status;
    intercept_ctx_t ctx;
    thread_state_t thread_state;

    /* Initialize CALLMON if not yet done */

    callmon$initialize ();

    if (callmon$$own.trace_flags & TRACE_API)
        callmon$$putmsg (CALLMON$_INTCPTALL, 4, 
            image_name, pre_routine, post_routine,
            override ? "TRUE" : "FALSE");

    callmon$$disable_threading (&thread_state);

    /* Find the description of the image */

    if ((ctx.image = callmon$$get_image (image_name)) == NULL)
        status = CALLMON$_INF;

    else {

        /* Intercept all routines in this image */

        ctx.pre_routine = (pdsc_t*) pre_routine;
        ctx.post_routine = (pdsc_t*) post_routine;
        ctx.override = override;
        ctx.use_desc = 0;

        status = lib$traverse_tree (&callmon$$own.routine_tree_by_value,
            intercept_all, &ctx);
    }

    callmon$$restore_threading (&thread_state);

    return status;
}


/*******************************************************************************
 *
 *  This public routine intercepts all routines in an image.
 *  Use string descriptors (for non-C application).
 */

uint32 callmon$intercept_all_dsc (
    struct dsc$descriptor_s* image_name,
    callmon$intercept_t      pre_routine,
    callmon$intercept_t      post_routine,
    callmon$boolean_t        override)
{
    uint32 status;
    intercept_ctx_t ctx;
    char* image_name_z;
    thread_state_t thread_state;

    /* Allocate a nul-terminated string on stack */

    image_name_z = __ALLOCA (image_name->dsc$w_length + 1);
    memcpy (image_name_z, image_name->dsc$a_pointer, image_name->dsc$w_length);
    image_name_z [image_name->dsc$w_length] = '\0';

    /* Initialize CALLMON if not yet done */

    callmon$initialize ();

    if (callmon$$own.trace_flags & TRACE_API)
        callmon$$putmsg (CALLMON$_INTCPTALL, 4, 
            image_name_z, pre_routine, post_routine,
            override ? "TRUE" : "FALSE");

    callmon$$disable_threading (&thread_state);

    /* Find the description of the image */

    if ((ctx.image = callmon$$get_image (image_name_z)) == NULL)
        status = CALLMON$_INF;

    else {

        /* Intercept all routines in this image */

        ctx.pre_routine = (pdsc_t*) pre_routine;
        ctx.post_routine = (pdsc_t*) post_routine;
        ctx.override = override;
        ctx.use_desc = 1;

        status = lib$traverse_tree (&callmon$$own.routine_tree_by_value,
            intercept_all, &ctx);
    }

    callmon$$restore_threading (&thread_state);

    return status;
}
