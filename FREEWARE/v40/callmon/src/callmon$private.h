/*  CALLMON - A Call Monitor for OpenVMS Alpha
 *
 *  File:     CALLMON$PRIVATE.H
 *  Author:   Thierry Lelegard
 *  Version:  1.0
 *  Date:     24-JUL-1996
 *
 *  Abstract: Private Interfaces of CALLMON
 */


#ifndef CALLMON_PRIVATE_H_LOADED
#define CALLMON_PRIVATE_H_LOADED 1

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <ints.h>
#include <builtins.h>
#include <descrip.h>
#include <ssdef.h>
#include <stsdef.h>
#include <chfdef.h>
#include <secdef.h>
#include <psldef.h>
#include <vadef.h>
#include <jpidef.h>
#include <syidef.h>
#include <lnmdef.h>
#include <prtdef.h>
#include <libdef.h>
#include <pdscdef.h>
#include <shldef.h>
#include <imcbdef.h>
#include <eihddef.h>
#include <eisddef.h>
#include <eihsdef.h>
#include <eiafdef.h>
#include <eobjrecdef.h>
#include <egsdef.h>
#include <egstdef.h>
#include <egsydef.h>
#include <starlet.h>
#include <evx_opcodes.h>
#include <lib$routines.h>
#include <ots$routines.h>
#include <rmsdef.h>
#include <rms.h>
#include "callmon.h"


/*******************************************************************************
 *
 *  These macros compute the offset of a field inside a structure and
 *  the base of the structure from the address of a field.
 */

#ifdef offsetof  /* from stddef.h */
#define OFFSET(type,field) offsetof (type, field)
#else
#define OFFSET(type,field) ((size_t) (&((type*) NULL)->field))
#endif

#define BASE(type,field,field_addr) \
    ((type*) ((char*) (field_addr) - OFFSET (type, field)))

/*
 *  This macro computes the number of elements in an array.
 */

#define NUMELEM(array) ((int) (sizeof (array) / sizeof (array [0])))


/*******************************************************************************
 *
 *  OpenVMS miscellaneous definitions.
 */

typedef struct dsc$descriptor_s desc_t; /* String descriptor */
typedef struct pdscdef pdsc_t;          /* Procedure descriptor */
typedef struct _imcb imcb_t;            /* Image control block */
typedef struct _shl shl_t;              /* Shareable image list entry */
typedef struct eobjrecdef eobj_t;       /* Object file record */
typedef struct egstdef egst_t;          /* Global symbol table entry */
typedef struct egsdef egsd_t;           /* Global symbol definition */

#define FILE_SPEC_SIZE    256  /* Maximum size + 1 of a file specification */
#define FILE_NAME_SIZE     40  /* Maximum size + 1 of a file name-part */
#define DEVICE_NAME_SIZE   65  /* Maximum size + 1 of a device name */
#define NODE_NAME_SIZE    256  /* Maximum size + 1 of a node name */
#define LINK_SYMBOL_SIZE   32  /* Maximum size + 1 of a linker symbol */
#define USER_NAME_SIZE     13  /* Maximum size + 1 of a user name */
#define LOGNAME_SIZE       65  /* Maximum size + 1 of a logical name */
#define MAX_TRANSLATIONS  128  /* Maximum number of logical name translations */
#define DCL_SYMBOL_SIZE   256  /* Maximum size + 1 of a DCL symbol name */
#define PROCESS_NAME_SIZE  16  /* Maximum size + 1 of a process name */
#define DISK_BLOCK_SIZE   512  /* Size of a disk block */
#define PAGELET_SIZE      512  /* Size memory pagelet (aka VAX page) */
#define MIN_ALPHA_PAGE   8192  /* Minimum size of Alpha memory page */
#define MAX_ALPHA_PAGE  65536  /* Maximum size of Alpha memory page */
#define MAX_ARG           255  /* Maximum number of procedure arguments */

#pragma nostandard
#pragma member_alignment save
#pragma nomember_alignment

typedef struct {                /* I/O Status block */
    uint16 status;
    uint16 count;
    uint32 info;
} iosb_t;

typedef struct {                /* Memory address range */
    char *base;
    char *end;
} range_t;

typedef struct {                /* Standard item sor system services */
    uint16  length;
    uint16  code;
    void*   buffer;
    uint16* retlen;
} item_t;

typedef struct {                /* Node header for LIB$INSERT_TREE */
    void*  left_link;
    void*  right_link;
    uint16 reserved;
    uint16 pad;
} libtree_t;

#ifdef lkp$q_entry
#undef lkp$q_entry
#endif
#ifdef lkp$q_proc_value
#undef lkp$q_proc_value
#endif

typedef struct {                /* Linkage pair (better than pdscdef.h) */
    int64 lkp$q_entry;          /* Code entry point */
    int64 lkp$q_proc_value;     /* Address of procedure descriptor */
} lkp_t;

typedef struct {                /* Relocation record ($RELDEF in LIB.R64) */
    uint32 rel$l_count;         /* Size of bitmap (in bits) */
    int32  rel$l_addr;          /* Relative base address of code section */
    char   rel$t_bitmap [];     /* Variable size */
} rel_t;

typedef struct {                /* Fixup list element ($FXEDEF in LIB.R64) */
    uint32 fxe$l_offset;        /* Offset of reference */
    union {
        int32 fxe$l_usv;        /* Universal symbol value */
        int32 fxe$l_psb;        /* Procedure signature block */
    } fxe$l_usv_overlay;
} fxe_t;

#define fxe$l_usv fxe$l_usv_overlay.fxe$l_usv
#define fxe$l_psb fxe$l_usv_overlay.fxe$l_psb

typedef struct {                /* Fixup record ($FIXDEF in LIB.R64) */
    uint32 fix$l_count;         /* Number of fixups */
    uint32 fix$l_shlx;          /* Shareable image list index */
    union {
        fxe_t fix$t_fixlst [];  /* Fixup list */
        int32 fix$t_offlst [];  /* Image offset list */
    } fix$t_lst_overlay;
} fix_t;

#define fix$t_fixlst fix$t_lst_overlay.fix$t_fixlst
#define fix$t_offlst fix$t_lst_overlay.fix$t_offlst

#pragma member_alignment restore
#pragma standard


/*******************************************************************************
 *
 *  These inlined routines initialize string descriptors.
 */

#pragma inline (set_desc, set_str_desc)

static desc_t* set_desc (desc_t* desc, void* string, size_t size)
{
    desc->dsc$a_pointer = string;
    desc->dsc$w_length  = size;
    desc->dsc$b_class   = DSC$K_CLASS_S;
    desc->dsc$b_dtype   = DSC$K_DTYPE_T;
    return desc;
}

static desc_t* set_str_desc (desc_t* desc, char* string)
{
    desc->dsc$a_pointer = string;
    desc->dsc$w_length  = strlen (string);
    desc->dsc$b_class   = DSC$K_CLASS_S;
    desc->dsc$b_dtype   = DSC$K_DTYPE_T;
    return desc;
}

#define set_array_desc(desc,array) \
    set_desc ((desc), (array), sizeof (array) - 1)


/*******************************************************************************
 *
 *  These inlined routines initialize items for system services.
 */

#pragma inline (set_item, set_str_item)

static void set_item (
    item_t* item,
    uint16  code,
    void*   buffer,
    uint16  buffer_size,
    uint16* ret_length)
{
    item->code   = code;
    item->buffer = buffer;
    item->length = buffer_size;
    item->retlen = ret_length;
}

static void set_str_item (item_t* item, uint16 code, char* string)
{
    item->code   = code;
    item->buffer = string;
    item->length = strlen (string);
    item->retlen = NULL;
}

#define set_data_item(item,code,data) \
    set_item (item, code, &(data), sizeof (data), NULL)

#define set_final_item(item) \
    set_item (item, 0, NULL, 0, NULL)


/*******************************************************************************
 *
 *  Layout of an Alpha instruction.
 *  In the Alpha architecture, instructions have a fixed size (32 bits).
 */

#define INST_RA_MASK     0x03E00000
#define INST_RB_MASK     0x001F0000
#define INST_RC_MASK     0x0000001F

#define INST_LIT_MASK    0x001FE000
#define INST_LITFLG_MASK 0x00001000

typedef struct {
    union {
        uint32 instr;                    /* Entire instruction */
        struct {                         /* PALcode format */
            unsigned int number : 26;    /* PAL function */
            unsigned int opcode :  6;    /* Opcode */
        } pal;
        struct {                         /* Branch format */
            signed int   disp   : 21;    /* Displacement */
            unsigned int ra     :  5;    /* Register Ra */
            unsigned int opcode :  6;    /* Opcode */
        } branch;
        struct {                         /* Memory format */
            signed int   disp   : 16;    /* Displacement */
            unsigned int rb     :  5;    /* Register Rb */
            unsigned int ra     :  5;    /* Register Ra */
            unsigned int opcode :  6;    /* Opcode */
        } mem;
        struct {                         /* Memory format with unsigned disp */
            unsigned int func   : 16;    /* Function code */
            unsigned int rb     :  5;    /* Register Rb */
            unsigned int ra     :  5;    /* Register Ra */
            unsigned int opcode :  6;    /* Opcode */
        } umem;
        struct {                         /* Memory format for jump */
            unsigned int predic : 14;    /* Predicted target */
            unsigned int hint   :  2;    /* JSR/JMP/RET/COROUTINE */
            unsigned int rb     :  5;    /* Register Rb */
            unsigned int ra     :  5;    /* Register Ra */
            unsigned int opcode :  6;    /* Opcode */
        } jump;
        struct {                         /* Operate format with register */
            unsigned int rc     :  5;    /* Register Rc */
            unsigned int func   :  7;    /* Instruction class */
            unsigned int litflg :  1;    /* Zero (register) */
            unsigned int sbz    :  3;    /* Should be zero */
            unsigned int rb     :  5;    /* Register Rb */
            unsigned int ra     :  5;    /* Register Ra */
            unsigned int opcode :  6;    /* Opcode */
        } op;
        struct {                         /* Operate format with literal */
            unsigned int rc     :  5;    /* Register Rc */
            unsigned int func   :  7;    /* Instruction class */
            unsigned int litflg :  1;    /* One (literal) */
            unsigned int lit    :  8;    /* Literal */
            unsigned int ra     :  5;    /* Register Ra */
            unsigned int opcode :  6;    /* Opcode */
        } oplit;
        struct {                         /* Floating-point format */
            unsigned int fc     :  5;    /* Register Fc */
            unsigned int func   :  4;    /* Instruction class */
            unsigned int src    :  2;    /* Source data type */
            unsigned int rnd    :  2;    /* Rounding mode */
            unsigned int trp    :  3;    /* Trapping mode */
            unsigned int fb     :  5;    /* Register Fb */
            unsigned int fa     :  5;    /* Register Fa */
            unsigned int opcode :  6;    /* Opcode */
        } fp;
        struct {                         /* Floating-point Aux. format */
            unsigned int fc     :  5;    /* Register Fc */
            unsigned int func   : 11;    /* Instruction class */
            unsigned int fb     :  5;    /* Register Fb */
            unsigned int fa     :  5;    /* Register Fa */
            unsigned int opcode :  6;    /* Opcode */
        } fpaux;
    } u;                                 /* Union name */
} inst_t;


/*******************************************************************************
 *
 *  The image activator maintains a list of the currently activated
 *  images (called the "done" list). This is a list of Image Control
 *  Blocks (IMCB) starting at IAC$GL_IMAGE_LIST. Note that referencing
 *  this cell requires to link /SYSEXE.
 */

#pragma extern_model save
#pragma extern_model strict_refdef
extern imcb_t iac$gl_image_list; /* List head only, not full IMCB */
#pragma extern_model restore


/*******************************************************************************
 *
 *  CALLMON internal data structure:
 *
 *  There is one image_t structure per main or shareable image in the
 *  current process.
 */

typedef struct {
    libtree_t tree_node;     /* Private to lib$insert_tree */
    imcb_t*   imcb;          /* Image control block (never NULL) */
    EIHD*     eihd;          /* Image header (or NULL) */
    EIHS*     eihs;          /* Image symbol table section (or NULL) */
    EIAF*     eiaf;          /* Image activator fixup section (or NULL) */
    EISD*     eisd;          /* First image section descriptor */
    uint16    channel;       /* Channel to the image (or zero) */
    void*     gst;           /* Global symbol table (or NULL) */
    int       file_blocks;   /* File size in blocks (or zero) */
    int       resident;      /* Boolean: image is resident */
    int       protected;     /* Boolean: image is protected */
    int       routine_count; /* Number of routines in this image */
    int       replace_bsr;   /* Boolean: the BSR instr. must be replaced */
    char      logname [FILE_NAME_SIZE];     /* Image logical name */
    char      file_spec [FILE_SPEC_SIZE];   /* Full file specification */
} image_t;


/*******************************************************************************
 *
 *  CALLMON internal data structure:
 *
 *  Each routine is described by a routine_t structure. These structures
 *  are maintained in several balanced trees. There is one tree for names,
 *  one tree for procedure values and one tree for entry code addresses.
 *  Each routine_t belongs to all trees, using a different libtree_t field.
 *
 *  Duplicate entries: If two routines have the same name (from two
 *  different images), both have a routine_t structure but only one
 *  routine_t is inserted in the tree. This routine_t is the "master
 *  by name". Then, a list of duplicate entries starts at "next by name".
 *  Each duplicate entry points to the "master by name".
 *
 *  The same system applies for the duplication of procedure values and
 *  entries (if one procedure has two names).
 *
 *  A routine is intercepted when its "jacket" field is non-null.
 *  If several routine names share the same value, they point to the
 *  same jacket.
 */

typedef struct routine_s routine_t;

struct routine_s {
    libtree_t  by_name;         /* Private to lib$insert_tree */
    libtree_t  by_value;        /* Private to lib$insert_tree */
    libtree_t  by_entry;        /* Private to lib$insert_tree */
    image_t*   image;           /* Shareable image of the routine */
    lkp_t      lkp;             /* Linkage pair from the symbol vector */
    int        uninterceptable; /* Don't intercept this routine */
    int        unrelocatable;   /* Don't hook relocation (fixup only) */
    pdsc_t*    jacket;          /* Jacket routine */
    routine_t* master_by_name;  /* Head of list with same name */
    routine_t* next_by_name;    /* Next in list with same name */
    routine_t* master_by_value; /* Head of list with same value */
    routine_t* next_by_value;   /* Next in list with same value */
    routine_t* master_by_entry; /* Head of list with same entry code */
    routine_t* next_by_entry;   /* Next in list with same entry code */
    desc_t     name_d;          /* String descriptor for field "name" */
    char       name [LINK_SYMBOL_SIZE]; /* Routine name */
};


/*******************************************************************************
 *
 *  This structure defines the association between a name and a value.
 */

typedef struct {
    int32 value;
    char* name;
} name_value_t;


/*******************************************************************************
 *
 *  During interception of routines, any kind of threading must be
 *  disabled. This structure is used to save the previous state.
 */

typedef struct {
    uint32 ast_state;   /* SS$_WASCLR or SS$_WASSET */
} thread_state_t;


/*******************************************************************************
 *
 *  CALLMON own storage.
 *
 *  The CALLMON$OWN area is shared by all modules of CALLMON.
 *
 *  There is no thread-synchronization when accessing this area.
 *  This means that CALLMON's API is not thread-reentrant. However,
 *  the jacket routine is thread reentrant. In other words, if an
 *  intercepted routine is reentrant, it is still reentrant after
 *  interception (in the user-supplied pre-/post-processing routines
 *  are also reentrant).
 */

typedef struct {

    /* Trace flags are used for troubleshooting purpose. The trace_flags
     * field is a OR of TRACE_* constants as defined below. This field
     * is written during initialization of CALLMON and read-only later. */

    uint32 trace_flags;

    /* Each time a BSR R26 is found in a code section, an CALLMON attempts
     * to replace it with a JSR R26. If the BSR is not replaced, the target
     * routine cannot be intercepted inside its image. In other words,
     * intra-image calls are not intercepted (although inter-image calls are).
     * It is possible to moderate the way BSR are replaced. */

    int bsr_replacement;

    #define BSR_OFF    0  /* No BSR replacement allowed */
    #define BSR_IF_R27 1  /* Allow replacement only if a LDx R27 is present */
    #define BSR_ALL    2  /* Allow replacement in all cases */

    /* When a BSR is transformed into a JSR, we must find a NOP instruction
     * (that is to say some unused placeholder where to store the appropriate
     * LDQ R26 instruction). The field bsr_depth gives the maximum number of
     * instructions between the NOP and the BSR. */

    int bsr_depth;

    /* The page size field contains the physical page size of the system.
     * The initial value of this field is zero and is initialized the
     * first time it is used, provided that it is accessed using the
     * macro get_page_size (). */

    uint32 page_size;

    #define get_page_size() \
        (callmon$$own.page_size != 0 ? callmon$$own.page_size : \
        (callmon$$own.page_size = callmon$$page_size ()))

    /* Tree of image_t structures. Each image in the process has one
     * structure in the tree. The tree is indexed using the image
     * logical name. The field image_tree is the head of the tree. */

    void* image_tree;

    /* Tree of routine_t structures. Each routine in the process has one
     * structure in the tree. The tree is indexed using three criteria:
     * symbol name, procedure value (pdsc_t*) and entry code address.
     * The following three fields are the heads of the tree according
     * to each criteria. */

    void* routine_tree_by_name;
    void* routine_tree_by_value;
    void* routine_tree_by_entry;

    /* Some low-level RTL routines cannot be intercepted for some reason.
     * The following array is constant and contains a list of routine
     * which should not be intercepted. */

    char** uninterceptable_routines;

    /* The following array is initially empty. It contains a user-defined
     * list of routines which should not be intercepted. This list is
     * filled during CALLMON initialization. */

    char** uninterceptable_routines_sup;

    /* Some routine cannot be relocated. This means that CALLMON must not
     * modify the relocations which point to it (but the modification of
     * fixup is still allowed). Any routine which is the target of an
     * unreplaced BSR is unrelocatable. Moreover, some routine are
     * unrelocatable by definition. They are listed in the following table. */

    char** unrelocatable_routines;

    /* The following array is initially empty. It contains a user-defined
     * list of routines which should not be relocated. This list is
     * filled during CALLMON initialization. */

    char** unrelocatable_routines_sup;

} callmon$$own_t;


/*******************************************************************************
 *
 *  Definition and initial value of CALLMON$$OWN.
 *
 *  If symbol DEFINE_CALLMON_OWN_STORAGE is not defined, external reference
 *  are generated. If it is defined, the actual definition is included.
 */

#pragma extern_model save
#pragma extern_model strict_refdef
#ifndef DEFINE_CALLMON_OWN_STORAGE
extern callmon$$own_t callmon$$own;
#else

static char* uninterceptable_routines [] = {
    "LIB$GET_CURR_INVO_CONTEXT", /* called in callmon$$jacket */
    "LIB$GET_PREV_INVO_CONTEXT", /* called in callmon$$jacket */
    "LIB$GET_INVO_HANDLE",       /* called in callmon$$jacket */
    "OTS$DIV_UI",                /* called by relocation hook sequence */
    "DECC$STRCMP",               /* pragma linkage (notneeded (ai,lp)) */
    "DECC$STRCPY",               /* pragma linkage (notneeded (ai,lp)) */
    "DECC$STRLEN",               /* pragma linkage (notneeded (ai,lp)) */
    "DECC$STRNLEN",              /* pragma linkage (notneeded (ai,lp)) */
    NULL};

static char* unrelocatable_routines [] = {
    "DECC$STRCMP",   /* pragma linkage (notneeded (ai,lp)) */
    "DECC$STRCPY",   /* pragma linkage (notneeded (ai,lp)) */
    "DECC$STRLEN",   /* pragma linkage (notneeded (ai,lp)) */
    "DECC$STRNLEN",  /* pragma linkage (notneeded (ai,lp)) */
    NULL};

callmon$$own_t callmon$$own = {
    /* trace_flags                  */  0,
    /* bsr_replacement              */  BSR_IF_R27,
    /* bsr_depth                    */  1000,  /* almost unbounded */
    /* page_size                    */  0,
    /* image_tree                   */  NULL,
    /* routine_tree_by_name         */  NULL,
    /* routine_tree_by_value        */  NULL,
    /* routine_tree_by_entry        */  NULL,
    /* uninterceptable_routines     */  uninterceptable_routines,
    /* uninterceptable_routines_sup */  NULL,
    /* unrelocatable_routines       */  unrelocatable_routines,
    /* unrelocatable_routines_sup   */  NULL,
};

#endif
#pragma extern_model restore


/*******************************************************************************
 *
 *  CALLMON trace flags
 */

#define TRACE_API           0x00000001  /* Calls to CallMon API */
#define TRACE_IMAGE         0x00000002  /* Loaded images */
#define TRACE_ROUTINE       0x00000004  /* Loaded routines */
#define TRACE_MEMORY        0x00000008  /* Modification of page protection */
#define TRACE_UNRELOCATABLE 0x00000010  /* All unrelocatable routines */
#define TRACE_UNRELOCATED   0x00000020  /* Unrelocated routines */
#define TRACE_DUPLICATE     0x00000040  /* Duplicate routine name or value */
#define TRACE_REFERENCES    0x00000080  /* Fixup and relocation summary */
#define TRACE_RELOCATION    0x00000100  /* All relocations */
#define TRACE_FIXUP         0x00000200  /* All fixups */
#define TRACE_INSTRUCTIONS  0x00000400  /* Instruction processing */
#define TRACE_CONFIGURATION 0x00000800  /* Configuration options */

#pragma extern_model save
#pragma extern_model strict_refdef
#ifdef DEFINE_CALLMON_OWN_STORAGE
name_value_t callmon$$trace_flags_names [] = {
    {TRACE_API,           "API"},
    {TRACE_IMAGE,         "LOAD_IMAGE"},
    {TRACE_ROUTINE,       "LOAD_ROUTINE"},
    {TRACE_MEMORY,        "MEMORY"},
    {TRACE_UNRELOCATABLE, "UNRELOCATABLE"},
    {TRACE_UNRELOCATED,   "UNRELOCATED"},
    {TRACE_DUPLICATE,     "DUPLICATE"},
    {TRACE_REFERENCES,    "REFERENCES"},
    {TRACE_RELOCATION,    "RELOCATION"},
    {TRACE_FIXUP,         "FIXUP"},
    {TRACE_INSTRUCTIONS,  "INSTRUCTIONS"},
    {TRACE_CONFIGURATION, "CONFIGURATION"},
    {0xFFFFFFFF,          "ALL"},
    {0,                   NULL}
};
#else
extern name_value_t callmon$$trace_flags_names [];
#endif
#pragma extern_model restore


/*******************************************************************************
 *
 *  All CALLMON public routines which use nul-terminated strings in their
 *  parameter profile have alternate routines which use class S string
 *  descriptors. These routines are for use by non-C languages.
 */

typedef void (*callmon$intercept_dsc_t) (
    callmon$arguments_t*     arguments,
    uint32                   caller_invo_handle,
    struct dsc$descriptor_s* routine_name,
    uint64                   (*intercepted_routine)(),
    uint64                   (*jacket_routine)());

typedef void (*callmon$disassemble_output_dsc_t) (
    void*                    pc,
    struct dsc$descriptor_s* opcode,
    struct dsc$descriptor_s* operands,
    void*                    user_data);

uint32 callmon$intercept_dsc (
    struct dsc$descriptor_s* routine_name,
    callmon$intercept_t      pre_routine,
    callmon$intercept_t      post_routine);

uint32 callmon$intercept_all_dsc (
    struct dsc$descriptor_s* image_name,
    callmon$intercept_t      pre_routine,
    callmon$intercept_t      post_routine,
    callmon$boolean_t        override);

void callmon$disassemble_dsc (
    void*                            address,
    size_t                           size_in_bytes,
    void*                            displayed_pc,
    callmon$disassemble_output_dsc_t output_routine,
    void*                            user_data);


/*******************************************************************************
 *
 *  These macros supplement routines callmon$$read/write_register_mask
 *  They check if a specified register is used by an instruction.
 */

#define callmon$$is_read_register(pc,reg) \
    ((callmon$$read_register_mask (pc) & (1 << (reg))) != 0)

#define callmon$$is_write_register(pc,reg) \
    ((callmon$$write_register_mask (pc) & (1 << (reg))) != 0)


/*******************************************************************************
 *
 *  Declarations of CALLMON private routines.
 */

void*      callmon$$alloc (size_t);
int        callmon$$branch_instruction (inst_t*);
inst_t*    callmon$$branch_target (inst_t*);
uint32     callmon$$disable_threading (thread_state_t*);
void       callmon$$disassemble (inst_t*, size_t, void*,
               callmon$disassemble_output_t,
               callmon$disassemble_output_dsc_t, void*);
uint32     callmon$$free (void*, size_t);
image_t*   callmon$$get_image (char*);
routine_t* callmon$$get_routine_by_entry (int64);
routine_t* callmon$$get_routine_by_name (char*);
routine_t* callmon$$get_routine_by_value (int64);
image_t*   callmon$$load_image (imcb_t*);
routine_t* callmon$$load_routine (image_t*, egst_t*);
int32      callmon$$name_to_value (name_value_t*, char*, int32);
uint32     callmon$$page_size (void);
void       callmon$$process_fixups (image_t*, routine_t*);
void       callmon$$process_relocations (routine_t*);
void       callmon$$putmsg (uint32, ...);
uint32     callmon$$read_register_mask (inst_t*);
void       callmon$$remove_bsr (image_t*);
uint32     callmon$$restore_threading (thread_state_t*);
char**     callmon$$search_list (char*, char*);
void       callmon$$set_uninterceptable (routine_t*);
void       callmon$$set_unrelocatable (routine_t*);
uint32     callmon$$translate (char*, char*, size_t, char*, int);
int        callmon$$translation_count (char*, char*);
uint32     callmon$$tree_allocate_from_user_data (void*, void**, void*);
int        callmon$$writeable (void*, size_t);
uint32     callmon$$write_register_mask (inst_t*);


#endif /* CALLMON_PRIVATE_H_LOADED */
