/*  Copyright (c) 1995 by Digital Equipment Corporation. All rights reserved.
 *
 *  File:   SHOW-CLUSTER.C
 *  Author: Thierry Lelegard
 *  Date:   13-OCT-1995
 *
 *  Abstract
 *  --------
 *
 *  This program is a CGI (Common Gateway Interface) script which must
 *  be activated in the context of a Web Server running on an OpenVMS
 *  system.
 *
 *  This script generates a MIME header and an HTML document representing
 *  the current state of the VMScluster. See the help() routine for details.
 *
 *  Compilation Notes
 *  -----------------
 *
 *  This source file uses the following VMS modules: UCBDEF, IHDDEF, IHIDEF,
 *  EIHDDEF and EIHIDEF. These modules are part of the LIB facility.
 *
 *  On Alpha systems, compile the module using the LIB module library:
 *
 *      $ CC SHOW-CLUSTER + SYS$LIBRARY:SYS$LIB_C/LIBRARY
 *
 *  On VAX systems, these modules do not exist in a C version. They
 *  were extracted from the library SYS$LIB_C.TLB of an OpenVMS Alpha
 *  system, then modified to get rid of Alpha-specific types like
 *  64-bits integer types and finally inlined in this source file.
 *
 *  Portability Notes
 *  -----------------
 *
 *  This program can run only on OpenVMS systems, either VAX or Alpha.
 *  The DEC C compiler is required (VAX C is not appropriate). The way
 *  this program gets its CGI keys depends on the communication method.
 *  This depends on the Web server. This version of the source file uses
 *  Purveyor's conventions. For other servers, the routine get_cgi() may
 *  need to be modified.
 *
 *  Execution Notes
 *  ---------------
 *
 *  To execute properly, this program requires the WORLD privilege.
 *  Either set this privilege to the account which run the Web Server
 *  or install the image with the WORLD privilege.
 *
 *  Example of second solution:
 *
 *      $ LINK/NOTRACEBACK SHOW-CLUSTER
 *      $ INSTALL ADD dev:[dir]SHOW-CLUSTER.EXE /PRIVILEGE=WORLD
 *
 *  Modification History
 *  --------------------
 *
 *  13-OCT-1995: Version 2.0. Original C version, rewritten from DCL script.
 *
 *  20-DEC-1995: Version 2.1. Added control for "software valid" bit on
 *               system disks. This prevent disks in mount-verify-timeout
 *               state from being considered as accessible.
 *
 *  23-FEB-1996: Version 2.2: Do not exit with error status if a node is
 *               unreachable. This is usually due to the lack of CLUSTER_SERVER
 *               process on the target node. In that case, the requested
 *               information is displayed and a note explains that the
 *               corresponding node is unreachable. Previously, nothing
 *               was displayed, except "%SYSTEM-F-UNREACHABLE, remote node is
 *               not currently reachable".
 */


/*
 *  Configuration.
 */

#define PRODUCT_NAME    "Show-Cluster CGI Script"
#define PRODUCT_VERSION "V2.2"
#define AUTHOR_MAIL     "lelegard@swthom.enet.dec.com"
#define NEW_PICTURE     "/images/new.gif"

#define MAX_WIDTH      70

#define NODE_WIDTH      6
#define ADDRESS_WIDTH   9
#define MODEL_WIDTH    25
#define VERSION_WIDTH   7

#define DISK_WIDTH     14
#define ARCH_WIDTH      5
#define LABEL_WIDTH    12
#define LIST_WIDTH     (MAX_WIDTH - DISK_WIDTH - ARCH_WIDTH - LABEL_WIDTH -\
                       VERSION_WIDTH - 8)

#define PRODVER_WIDTH  16


/*
 *  Standard and VMS-specific header files.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <descrip.h>
#include <ssdef.h>
#include <stsdef.h>
#include <libdef.h>
#include <dvidef.h>
#include <jpidef.h>
#include <syidef.h>
#include <prvdef.h>
#include <pscandef.h>
#include <alphadef.h>
#include <rms.h>
#include <starlet.h>


/*
 *  C language types and macros
 */

typedef unsigned char  ubyte_t;
typedef unsigned short uword_t;
typedef unsigned long  ulong_t;
typedef unsigned char  boolean_t;

#define array_size(array) (sizeof(array)/sizeof(*(array)))
#define offset(pointer,field) (((char*)(&((pointer)->field)))-(char*)(pointer))


/*
 *  OpenVMS status codes
 */

typedef unsigned long status_t;

#define success(code)     ((code) & STS$M_SUCCESS)
#define failure(code)     (~(code) & STS$M_SUCCESS)
#define severity(code)    ((code) & STS$M_SEVERITY)
#define unprintable(code) ((code) | STS$M_INHIB_MSG)
#define printable(code)   ((code) & ~STS$M_INHIB_MSG)


/*
 *  Miscellaneous OpenVMS size constants
 */

#define FILE_SPEC_SIZE    256  /* Maximum size + 1 of a file specification */
#define DEVICE_NAME_SIZE   65  /* Maximum size + 1 of a device name */
#define VOLUME_NAME_SIZE   13  /* Maximum size + 1 of a volume name (label) */
#define NODE_NAME_SIZE    256  /* Maximum size + 1 of a node name */
#define SCSNODE_NAME_SIZE  16  /* Maximum size + 1 of an SCS node name */
#define USER_NAME_SIZE     13  /* Maximum size + 1 of a user name */
#define HW_NAME_SIZE       32  /* Maximum size + 1 of a hardware name */
#define LOGNAME_SIZE       65  /* Maximum size + 1 of a logical name */
#define MAX_TRANSLATIONS  128  /* Maximum number of logical name translations */
#define DCL_SYMBOL_SIZE   256  /* Maximum size + 1 of a DCL symbol name */
#define PROCESS_NAME_SIZE  16  /* Maximum size + 1 of a process name */
#define DISK_BLOCK_SIZE   512  /* Size of a disk block */
#define MAX_CLUSTER_NDOE   96  /* Maximum number of nodes per VMScluster */


/*
 *  OpenVMS I/O types
 */

typedef unsigned short channel_t;

typedef struct {  /* I/O Status Block */
    uword_t status;
    uword_t count;
    ulong_t info;
} iosb_t;

#ifdef __ALPHA
#include <ucbdef.h>
#else
#define UCB$M_VALID 0x00000800
#endif


/*==============================================================================
 *
 *  These macros initialize string descriptors
 */

typedef struct dsc$descriptor_s desc_t;

#define set_desc(desc,string,length) (\
    (desc).dsc$a_pointer = (char*)(string),\
    (desc).dsc$w_length  = (length),\
    (desc).dsc$b_class   = DSC$K_CLASS_S,\
    (desc).dsc$b_dtype   = DSC$K_DTYPE_T)

#define set_array_desc(desc,array) (\
    (desc).dsc$a_pointer = (char*)(array),\
    (desc).dsc$w_length  = sizeof (array) - 1,\
    (desc).dsc$b_class   = DSC$K_CLASS_S,\
    (desc).dsc$b_dtype   = DSC$K_DTYPE_T)

#define set_str_desc(desc,string) (\
    (desc).dsc$w_length = strlen ((desc).dsc$a_pointer = (char*)(string)),\
    (desc).dsc$b_class  = DSC$K_CLASS_S,\
    (desc).dsc$b_dtype  = DSC$K_DTYPE_T)


/*==============================================================================
 *
 *  These macros initialize item lists.
 *  Warning: non-reentrant macros, must not be used in a multi-threaded program.
 */

typedef struct {
    uword_t  length;
    uword_t  code;
    void*    buffer;
    uword_t* retlen;
} item_t;

static item_t* _tmp_itm;

#define set_item(item_ptr,func_code,buf_addr,buf_size,ret_length) (\
    _tmp_itm         = (item_ptr),\
    _tmp_itm->code   = (func_code),\
    _tmp_itm->buffer = (buf_addr),\
    _tmp_itm->length = (buf_size),\
    _tmp_itm->retlen = (ret_length))

#define set_var_item(item_ptr,func_code,variable) (\
    _tmp_itm         = (item_ptr),\
    _tmp_itm->code   = (func_code),\
    _tmp_itm->buffer = &(variable),\
    _tmp_itm->length = sizeof (variable),\
    _tmp_itm->retlen = 0)

#define set_array_item(item_ptr,func_code,array,ret_length) (\
    _tmp_itm         = (item_ptr),\
    _tmp_itm->code   = (func_code),\
    _tmp_itm->buffer = (array),\
    _tmp_itm->length = sizeof (array),\
    _tmp_itm->retlen = (ret_length))

#define set_str_array_item(item_ptr,func_code,array,ret_length) (\
    _tmp_itm         = (item_ptr),\
    _tmp_itm->code   = (func_code),\
    _tmp_itm->buffer = (array),\
    _tmp_itm->length = sizeof (array) - 1,\
    _tmp_itm->retlen = (ret_length))

#define set_string_item(item_ptr,func_code,string) (\
    _tmp_itm         = (item_ptr),\
    _tmp_itm->code   = (func_code),\
    _tmp_itm->length = strlen (_tmp_itm->buffer = (string)),\
    _tmp_itm->retlen = 0)

#define set_final_item(item_ptr) (\
    _tmp_itm         = (item_ptr),\
    _tmp_itm->code   = 0,\
    _tmp_itm->buffer = NULL,\
    _tmp_itm->length = 0,\
    _tmp_itm->retlen = NULL)


/*==============================================================================
 *
 *  Structure of an image header. Not defined on VAX systems.
 */

#ifdef __ALPHA

#include <ihddef.h>
#include <ihidef.h>
#include <eihddef.h>
#include <eihidef.h>

#else /* VAX */

#pragma member_alignment save
#pragma nomember_alignment

typedef struct {
    unsigned long low;
    unsigned long high;
} uquad_t;

/* VAX Image Header Record Definition */

#define IHD$K_MAJORID 12848             /* Major id value                   */
#define IHD$K_MINORID 13616             /* Minor id value                   */

typedef struct _ihd {
    uword_t ihd$w_size;
    uword_t ihd$w_activoff;
    uword_t ihd$w_symdbgoff;
    uword_t ihd$w_imgidoff;
    uword_t ihd$w_patchoff;
    uword_t ihd$w_version_array_off;
    uword_t ihd$w_majorid;
    uword_t ihd$w_minorid;
    ubyte_t ihd$b_hdrblkcnt;
    ubyte_t ihd$b_imgtype;
    uword_t ihddef$$_fill_2;
    uquad_t ihd$q_privreqs;
    uword_t ihd$w_iochancnt;
    uword_t ihd$w_imgiocnt;
    ulong_t ihd$l_lnkflags;
    ulong_t ihd$l_ident;
    ulong_t ihd$l_sysver;
    void *ihd$l_iafva;
    char ihd$t_skip [462];
    uword_t ihd$w_alias;
} IHD;
 
/* Alpha Image Header Record Definition */

#define EIHD$K_MAJORID 3
#define EIHD$K_MINORID 0

typedef struct _eihd {
    ulong_t eihd$l_majorid;
    ulong_t eihd$l_minorid;
    ulong_t eihd$l_size;
    ulong_t eihd$l_isdoff;
    ulong_t eihd$l_activoff;
    ulong_t eihd$l_symdbgoff;
    ulong_t eihd$l_imgidoff;
    ulong_t eihd$l_patchoff;
    union  {
        void *eihd$l_iafva;
        uquad_t eihd$q_iafva;
    } eihd$r_iafva;
    union  {
        void *eihd$l_symvva;
        uquad_t eihd$q_symvva;
    } eihd$r_symvva;
    ulong_t eihd$l_version_array_off;
    ulong_t eihd$l_imgtype;
    ulong_t eihd$l_subtype;
    ulong_t eihd$l_imgiocnt;
    ulong_t eihd$l_iochancnt;
    uquad_t eihd$q_privreqs;
    ulong_t eihd$l_hdrblkcnt;
    ulong_t eihd$l_lnkflags;
    ulong_t eihd$l_ident;
    ulong_t eihd$l_sysver;
    struct  {
        ubyte_t eihd$b_matchctl;
        ubyte_t eihd$b_fill_1;
        ubyte_t eihd$b_fill_2;
        ubyte_t eihd$b_fill_3;
        ulong_t eihd$l_symvect_size;
        ulong_t eihd$l_virt_mem_block_size;
    } eihd$r_control;
    char eihd$t_skip [406];
    uword_t eihd$w_alias;
} EIHD;
 
/* VAX Image Header Identification Section */

typedef struct _ihi {
    char ihi$t_imgnam [40];
    char ihi$t_imgid [16];
    uquad_t ihi$q_linktime;
    char ihi$t_linkid [16];
} IHI;
 
/* Alpha Image Header Identification Section */

#define EIHI$K_MAJORID 1
#define EIHI$K_MINORID 2

typedef struct _eihi {
    ulong_t eihi$l_majorid;
    ulong_t eihi$l_minorid;
    uquad_t eihi$q_linktime;
    char eihi$t_imgnam [40];
    char eihi$t_imgid [16];
    char eihi$t_linkid [16];
    char eihi$t_imgbid [16];
} EIHI;
 
#pragma member_alignment restore

#endif /* Alpha vs VAX */


/*==============================================================================
 *
 *  Application-specific data structures.
 */

/*
 *  Description of a node in the cluster.
 */

typedef struct node_s node_t;

struct node_s {
    ulong_t   dnet_area;
    ulong_t   dnet_number;
    char      node_name [SCSNODE_NAME_SIZE];
    boolean_t is_alpha;
    char      model [HW_NAME_SIZE];
    char      version_name [5];
    ulong_t   version;         /* Encoded in ascending order */
    node_t*   next;            /* Next node in alphabetical order */
    node_t*   next_same_disk;  /* Next node with same system disk */
    char      system_disk [DEVICE_NAME_SIZE];  /* May be empty */
};

#define MAX_VERSION 10000000


/*
 *  Description of a system disk in the cluster.
 */

typedef struct disk_s disk_t;

struct disk_s {
    char      name [DEVICE_NAME_SIZE];
    char      label [VOLUME_NAME_SIZE];
    boolean_t is_alpha;
    boolean_t is_mounted;
    ulong_t   version;         /* Encoded in ascending order */
    node_t*   node_list;       /* Sorted list of nodes booting here */
    disk_t*   next;            /* Next disk in descending version */
    char*     product_version; /* One software product version */
};


/*
 *  Description of a complete VMScluster.
 */

typedef struct cluster_s {
    int     node_count;          /* Number of nodes in the cluster */
    int     vax_count;           /* Number of VAX systems */
    int     alpha_count;         /* Number of Alpha systems */
    node_t* first_node;          /* First node */
    int     disk_count;          /* Number of system disks */
    int     mounted_disk_count;  /* Number of mounted system disks */
    int     unknown_disk_count;  /* Number of nodes with unknown system disk */
    disk_t* first_disk;          /* First system disk */
    int     version_count;       /* Number of VMS versions */
    node_t  node_list [MAX_CLUSTER_NDOE];  /* Unsorted buffer */
    disk_t  disk_list [MAX_CLUSTER_NDOE];  /* Unsorted buffer */
} cluster_t;


/*
 *  Description of a software product.
 */

typedef struct product_s product_t;
typedef struct image_s image_t;

struct image_s {
    char*    file_spec;
    image_t* next;  /* Next image for same product */
};

struct product_s {
    char*      identifier;
    char*      name;
    int        image_count;
    image_t*   first_image;
    product_t* next;  /* Next product in config file order */
    char*      version;
};


/*
 *  Description of the database of software products.
 */

typedef struct {
    int        product_count;
    int        name_width;    /* Width of longest product name */
    product_t* first_product;
} prodbase_t;


/*
 *  Description of commands.
 */

#define OPT_LISTNODES   "listnodes"
#define OPT_LISTDISKS   "listdisks"
#define OPT_HELP        "help"
#define OPT_GETSOURCE   "getsource"
#define OPT_GETPRODFILE "getprodfile"
#define OPT_SCANDISK    "scandisk"
#define OPT_SCANPRODUCT "scanproduct"
#define OPT_SELECTPROD  "selectprod"
#define OPT_FULL        "full"


/*
 *  Description of the invocation context of the CGI scripts.
 */

typedef struct {
    char*  script_name;    /* Name of invoked CGI script */
    char*  image_name;     /* Name of current image */
    int    key_count;      /* Number of parameters (CGI keys) */
    char** key;            /* Parameters (CGI keys) */
} cgi_t;


/*==============================================================================
 *
 *  This procedure sends an HTML header.
 */

static void start_html (const cgi_t* cgi, const char* format, ...)
{
    va_list ap;

    printf ("Content-Type: text/html\n\n<html><head>\n<title>" PRODUCT_NAME " "
        PRODUCT_VERSION "</title>\n</head>\n<body>\n<h1>");

    va_start (ap, format);
    vprintf (format, ap);
    va_end (ap);

    printf ("</h1>\n");

    if (cgi != NULL)
        printf ("<h4>[Options: "
            "<a href=\"%s?" OPT_LISTNODES "\">Nodes</a> | "
            "<a href=\"%s?" OPT_LISTDISKS "\">Disks</a> | "
            "<a href=\"%s?" OPT_SELECTPROD "\">Products</a> | "
            "<a href=\"%s?" OPT_HELP "\">Help</a>]</h4>\n",
            cgi->script_name, cgi->script_name,
            cgi->script_name, cgi->script_name);

    printf ("<hr>\n");
}


/*==============================================================================
 *
 *  Build an HTML document from a VMS status code and exit the application.
 */

static void error_exit (const cgi_t* cgi, status_t status)
{
    status_t msgvec [2];

    msgvec [0] = 1;
    msgvec [1] = status;

    start_html (cgi, "Error");
    printf ("Error while calling a system routine.<pre>\n");
    fflush (stdout);
    sys$putmsg (msgvec, 0, 0, 0);
    printf ("</pre></body></html>\n");

    exit (EXIT_SUCCESS);
}


/*==============================================================================
 *
 *  Build an HTML document from a printf-like syntax and exit the application.
 */

static void message_exit (const cgi_t* cgi, const char* format, ...)
{
    va_list ap;

    start_html (cgi, "Error");
    va_start (ap, format);
    vprintf (format, ap);
    va_end (ap);
    printf ("</body></html>\n");

    exit (EXIT_SUCCESS);
}


/*==============================================================================
 *
 *  Memory allocation. Exit application in case of failure.
 */

static void* memalloc (size_t size)
{
    void* result = malloc (size);

    if (result == NULL)
        error_exit (NULL, LIB$_INSVIRMEM);
    else
        return result;
}

#define new(type) ((type*)memalloc (sizeof (type)))

static char* new_string (const char* str)
{
    return strcpy (memalloc (strlen (str) + 1), str);
}


/*==============================================================================
 *
 *  This procedure fetches the invocation context of the current CGI script.
 *  This sequence depends on the type of Web server. Here, the conventions
 *  are for Purveyor.
 */

static cgi_t* get_cgi (int argc, char** argv)
{
    cgi_t* cgi = new (cgi_t);
    char   key_name [80];
    char** key;
    char*  p;
    int    n;

    /* Get the image name */

    cgi->image_name = argv [0];

    /* Get the local part of the script name. May be used in a
     * self-relative URL. */

    if ((cgi->script_name = getenv ("WWW_SCRIPT_NAME")) == NULL)
        cgi->script_name = "";
    else {
        for (p = cgi->script_name; *p != '\0'; p++)
            if (*p == '/')
                cgi->script_name = p + 1;
    }

    /* Get the number of parameters (CGI keys) */

    cgi->key_count = (p = getenv ("WWW_KEY_COUNT")) != NULL ? atoi (p) : 0;

    /* Get the parameters (CGI keys). Remove surrounding quotes */

    if (cgi->key_count > 0) {
        cgi->key = memalloc (cgi->key_count * sizeof (char*));

        for (n = 1, key = cgi->key; n <= cgi->key_count; n++, key++) {
            sprintf (key_name, "WWW_KEY_%d", n);
            if ((*key = getenv (key_name)) == NULL)
                *key = "";
            else if (*key [0] == '"') {
                char* last = *key + strlen (*key) - 1;
                (*key)++;
                if (*last == '"')
                    *last = '\0';
            }
        }
    }

    return cgi;
}


/*==============================================================================
 *
 *  This routine updates a string length and trim trailing blanks.
 */

static void trim_string (char* s, int length)
{
    while (length > 0 && isspace (s [length - 1]))
        length--;
    s [length] = '\0';
}


/*==============================================================================
 *
 *  This function parses a VMS file specification. Any of the input file
 *  specifications may be specified as NULL.
 *  Return a malloc'ated string. In case of error, return NULL and set errno.
 */

static char* parse_file (char* spec, char* defaulted, char* related)
{
    status_t status;
    char full_spec [FILE_SPEC_SIZE];
    struct FAB fab = cc$rms_fab;
    struct NAM nam = cc$rms_nam;
    struct NAM relnam = cc$rms_nam;
    char* double_bracket;

    fab.fab$l_nam = &nam;
    nam.nam$l_esa = full_spec;
    nam.nam$b_ess = sizeof (full_spec) - 1;
    nam.nam$b_nop |= NAM$M_SYNCHK;

    if (spec != NULL) {
        fab.fab$l_fna = spec;
        fab.fab$b_fns = strlen (spec);
    }
    if (defaulted != NULL) {
        fab.fab$l_dna = defaulted;
        fab.fab$b_dns = strlen (defaulted);
    }
    if (related != NULL) {
        nam.nam$l_rlf = &relnam;
        relnam.nam$l_rsa = related;
        relnam.nam$b_rsl = strlen (related);
    }

    if (failure (status = sys$parse (&fab))) {
        errno = EVMSERR;
        vaxc$errno = status;
        return NULL;
    }
    else {
        full_spec [nam.nam$b_esl] = '\0';
        if ((double_bracket = strstr (full_spec, "][")) != NULL) {
            do {
                double_bracket [0] = double_bracket [2];
            } while (double_bracket++ [2] != '\0');
        }
        return new_string (full_spec);
    }
}


/*==============================================================================
 *
 *  This function fetches the identification string of an image file.
 *  This image may be shareable or executable, Alpha or VAX.
 *  Return a malloc'ated string. In case of error, return NULL.
 */

static char* image_ident (const char* file)
{
    status_t status;
    struct FAB fab = cc$rms_fab;
    struct RAB rab = cc$rms_rab;
    char data [2 * DISK_BLOCK_SIZE];
    char* ident;
    EIHD* eihd;
    EIHI* eihi;
    IHD* ihd;
    IHI* ihi;

    if (file == NULL)
        return NULL;

    /* Read the beginning of the image file. The size of the data buffer */
    /* is assumed to be large enough to contain the image header. */

    fab.fab$l_fna = (char*) file;
    fab.fab$b_fns = strlen (file);
    fab.fab$b_fac = FAB$M_BIO | FAB$M_GET;

    rab.rab$l_fab = &fab;
    rab.rab$l_bkt = 0;
    rab.rab$l_ubf = data;
    rab.rab$w_usz = sizeof (data);

    if (failure (sys$open (&fab)))
        return NULL;

    if (success (status = sys$connect (&rab)))
        status = sys$read (&rab);

    sys$close (&fab);

    if (failure (status) || rab.rab$w_rsz != sizeof (data))
        return NULL;

    /* Now analyze the header. Compute the pointers if Alpha or VAX header */

    eihd = (EIHD*) data;
    eihi = (EIHI*) (data + eihd->eihd$l_imgidoff);
    ihd = (IHD*) data;
    ihi = (IHI*) (data + ihd->ihd$w_imgidoff);

    /* Is it an Alpha image? */

    if (eihd->eihd$l_majorid == EIHD$K_MAJORID &&
        eihd->eihd$l_minorid == EIHD$K_MINORID &&
        (char*)eihi > data &&
        (char*)eihi + sizeof (EIHI) <= data + sizeof (data) &&
        eihi->eihi$t_imgid [0] >= 0 &&
        eihi->eihi$t_imgid [0] < sizeof (eihi->eihi$t_imgid)) {

        /* Note that the following test should be made but is not always
         * true: eihi->eihi$l_minorid == EIHI$K_MINORID */

        /* The image identification is a counted ascii string */

        ident = eihi->eihi$t_imgid + 1;
        ident [eihi->eihi$t_imgid [0]] = '\0';
    }

    /* If not an Alpha image, could it be a VAX image */

    else if (ihd->ihd$w_majorid == IHD$K_MAJORID &&
        ihd->ihd$w_minorid == IHD$K_MINORID &&
        (char*)ihi > data &&
        (char*)ihi + sizeof (IHI) <= data + sizeof (data) &&
        ihi->ihi$t_imgid [0] >= 0 &&
        ihi->ihi$t_imgid [0] < sizeof (ihi->ihi$t_imgid)) {

        /* The image identification is a counted ascii string */

        ident = ihi->ihi$t_imgid + 1;
        ident [ihi->ihi$t_imgid [0]] = '\0';
    }

    /* Else, this is not an image file */

    else
        return NULL;

    /* Return a trimmed identification string */

    while (isspace (*ident))
        ident++;

    return new_string (ident);
}


/*==============================================================================
 *
 *  This routine finds all nodes in the cluster and get the required
 *  information on them. Return the number of nodes.
 */

static int get_node_list (
    const cgi_t* cgi,
    node_t*      node_list,
    int          node_list_size)
{
    status_t status;
    iosb_t   iosb;
    long     system_id = -1;
    ulong_t  scan_ctx = 0;
    node_t*  node;
    int      node_count;
    item_t   item_list [40];
    item_t*  item;
    uword_t  hardware;
    uword_t  name_len;
    uword_t  model_len;
    uword_t  version_len;
    char     image_name [FILE_SPEC_SIZE];
    char     vletter;
    int      major, minor, revision;

    /* Get the list of nodes in the cluster. */

    for (node = node_list; node < node_list + node_list_size; node++) {

        node->next = node->next_same_disk = NULL;

        /* Get all information about a node (except name of system disk) */

        item = item_list;
        set_var_item (item++, SYI$_NODE_AREA, node->dnet_area);
        set_var_item (item++, SYI$_NODE_NUMBER, node->dnet_number);
        set_var_item (item++, SYI$_HW_MODEL, hardware);
        set_str_array_item (item++, SYI$_NODENAME, node->node_name, &name_len);
        set_str_array_item (item++, SYI$_HW_NAME, node->model, &model_len);
        set_str_array_item (item++, SYI$_NODE_SWVERS, node->version_name,
            &version_len);
        set_final_item (item++);

        status = sys$getsyiw (0, &system_id, 0, item_list, &iosb, 0, 0);

        if (success (status))
            status = iosb.status;
        if (status == SS$_NOMORENODE)
            break;
        if (failure (status))
            error_exit (cgi, status);

        trim_string (node->node_name, name_len);
        trim_string (node->model, model_len);
        trim_string (node->version_name, version_len);
        node->is_alpha = hardware >= ALPHA$K_ID_BASE;

        /* The expected version format is Xn.n-n. Omitted fields are zero */

        major = minor = revision = 0;
        sscanf (node->version_name, "%c%d.%d-%d", &vletter,
            &major, &minor, &revision);
        node->version = node->is_alpha +
            10 * (revision + 100 * (minor + 100 * major));
    }

    node_count = node - node_list;

    /* Get the name of the system disk for all nodes. This name is not
     * available by $GETSYI. To do this, we select a process that meets
     * the following criteris: run on each node, can be found easily,
     * executes an image from the system disk. One candidate is the
     * process CLUSTER_SERVER. */

    for (node = node_list; node < node_list + node_count; node++) {

        uword_t image_len;
        char* start;
        char* end;

        /* Establish scan context to match a process named CLUSTER_SERVER
         * on the specified node. */

        item = item_list;
        set_string_item (item++, PSCAN$_NODENAME, node->node_name);
        set_string_item (item++, PSCAN$_PRCNAM, "CLUSTER_SERVER");
        set_final_item (item++);

        status = sys$process_scan (&scan_ctx, item_list);

        if (failure (status))
            error_exit (cgi, status);

        /* Get the image name of the CLUSTER_SERVER process */

        item = item_list;
        set_str_array_item (item++, JPI$_IMAGNAME, image_name, &image_len);
        set_final_item (item++);

        status = sys$getjpiw (0, &scan_ctx, 0, item_list, &iosb, 0, 0);

        if (success (status))
            status = iosb.status;

        /* The status may be erroneous if the node is currently booting
         * and has not yet created the CLUSTER_SERVER process or if the
         * node had performed a minimal startup. */

        if (failure (status))
            node->system_disk [0] = '\0';

        else {
            trim_string (image_name, image_len);

            /* The image name is on the system disk -> get device name.
             * Do not try to $GETDVI on it (may be not known locally). */

            for (start = image_name; *start == '_' || isspace (*start);start++);
            for (end = start; *end != '\0' && *end != ':'; end++);
            if (*end == ':')
                end [1] = '\0';
            strcpy (node->system_disk, start);
        }
    }

    return node_count;
}


/*==============================================================================
 *
 *  This routine finds all system disks in the cluster.
 *  Return the number of disks.
 */

static int get_disk_list (disk_t* disk_list, int disk_list_size, node_t* node)
{
    int      disk_count = 0;
    status_t status;
    iosb_t   iosb;
    disk_t*  disk;
    node_t** link;
    item_t   item_list [40];
    item_t*  item;
    char     name_buffer [DEVICE_NAME_SIZE];
    uword_t  name_len;
    char*    name;
    char     label [VOLUME_NAME_SIZE];
    uword_t  label_len;
    ulong_t  is_mounted;
    ulong_t  unit_status;
    desc_t   name_desc;

    for ( ; node != NULL; node = node->next) {

        /* If the system disk is unknown, skip this node */

        if (node->system_disk [0] == '\0')
            continue;

        /* Try to get the characteristics of the disk. This may fail
         * since the device may not be served. */

        set_str_desc (name_desc, node->system_disk);

        item = item_list;
        set_var_item (item++, DVI$_MNT, is_mounted);
        set_var_item (item++, DVI$_STS, unit_status);
        set_str_array_item (item++, DVI$_FULLDEVNAM, name_buffer, &name_len);
        set_str_array_item (item++, DVI$_VOLNAM, label, &label_len);
        set_final_item (item++);

        status = sys$getdviw (0, 0, &name_desc, item_list, &iosb, 0, 0, 0);

        if (success (status))
            status = iosb.status;

        if (failure (status)) {
            is_mounted = 0;
            name = node->system_disk;
            label [0] = '\0';
        }
        else {
            trim_string (name_buffer, name_len);
            trim_string (label, label_len);
            for (name = name_buffer; *name == '_'; name++);
        }

        /* Is the disk already known? Search the list */

        for (disk = disk_list; disk < disk_list + disk_count &&
            strcmp (disk->name, name) != 0; disk++);

        /* If found in the list, add the node in the node-list for this
         * disk. Add at end of list to preserve node order */

        if (disk < disk_list + disk_count) {
            for (link = &disk->node_list;
                 *link != NULL;
                 link = &(*link)->next_same_disk);
            *link = node;
        }

        /* If not found, add a disk entry */

        else if (disk_count < disk_list_size) {
            disk_count++;
            disk->is_alpha = node->is_alpha;
            disk->version = node->version;
            disk->node_list = node;
            disk->is_mounted = is_mounted && (unit_status & UCB$M_VALID);
            disk->product_version = NULL;
            strcpy (disk->name, name);
            strcpy (disk->label, label);
        }
    }

    return disk_count;
}


/*==============================================================================
 *
 *  Sort a list of nodes by alphabetical order (update the 'next' field).
 *  Return the address of the first node in the list.
 */

static node_t* sort_node_list (node_t* unsorted, int count)
{
    node_t* first = NULL;
    node_t** node;

    while (count-- > 0) {

        for (node = &first;
             *node != NULL &&
             strcmp (unsorted->node_name, (*node)->node_name) > 0;
             node = &(*node)->next);

        unsorted->next = *node;
        *node = unsorted++;
    }

    return first;
}


/*==============================================================================
 *
 *  Sort a list of system disks by decreasing version number (update the
 *  'next' field). Return the address of the first disk in the list.
 */

static disk_t* sort_disk_list (disk_t* unsorted, int count)
{
    disk_t* first = NULL;
    disk_t** disk;

    while (count-- > 0) {

        for (disk = &first;
             *disk != NULL && unsorted->version <= (*disk)->version;
             disk = &(*disk)->next);

        unsorted->next = *disk;
        *disk = unsorted++;
    }

    return first;
}


/*==============================================================================
 *
 *  This procedure fetches the configuration of the cluster.
 */

static cluster_t* get_cluster_config (const cgi_t* cgi)
{
    cluster_t*   cluster = new (cluster_t);
    node_t*      node;
    disk_t*      disk;
    ulong_t      previous_version;
    status_t     status;
    union prvdef privileges;

    /* Get the WOLD privilege. This is required for the following.
     * Turn non-fatal status NOTALLPRIV into fatal. */

    memset (&privileges, 0, sizeof (privileges));
    privileges.prv$v_world = 1;

    status = sys$setprv (1, &privileges, 0, 0);

    if (status == SS$_NOTALLPRIV)
        status = SS$_NOPRIV;
    if (failure (status))
        error_exit (cgi, status);

    /* Get the list of nodes and system disks in the cluster */

    cluster->node_count = get_node_list (cgi, cluster->node_list,
        array_size (cluster->node_list));

    cluster->first_node = sort_node_list (cluster->node_list,
        cluster->node_count);

    /* Count the number of VAX and Alpha */

    cluster->alpha_count = cluster->vax_count = cluster->unknown_disk_count = 0;

    for (node = cluster->first_node; node != NULL; node = node->next) {
        if (node->is_alpha)
            cluster->alpha_count++;
        else
            cluster->vax_count++;
        if (node->system_disk [0] == '\0')
            cluster->unknown_disk_count++;
    }

    /* Get the list of system disks in the cluster */

    cluster->disk_count = get_disk_list (cluster->disk_list,
        array_size (cluster->disk_list), cluster->first_node);

    cluster->first_disk = sort_disk_list (cluster->disk_list,
        cluster->disk_count);

    /* Count the number of versions of VMS and the number of mounted disks */

    previous_version = MAX_VERSION;
    cluster->version_count = 0;
    cluster->mounted_disk_count = 0;

    for (disk = cluster->first_disk; disk != NULL; disk = disk->next) {
        if (disk->is_mounted)
            cluster->mounted_disk_count++;
        if (disk->version != previous_version) {
            cluster->version_count++;
            previous_version = disk->version;
        }
    }

    return cluster;
}


/*==============================================================================
 *
 *  The procedure get_product fetches one product description.
 *  It returns NULL on end-of-file or syntax-error.
 */

/* Get the next non-blank character on the current line */

static int getnbc (FILE* fp)
{
    int c;

    while ((c = getc (fp)) != EOF && c != '\n' && isspace (c));
    if (c == '!')
        while ((c = getc (fp)) != EOF && c != '\n');
    return c;
}


/* Return a quoted string */

static char* get_quoted (FILE* fp)
{
    char  buffer [FILE_SPEC_SIZE];
    char* end = buffer + sizeof (buffer) - 1;
    char* p = buffer;
    int   c;

    if ((c = getnbc (fp)) != '"')
        return NULL;
    while ((c = getc (fp)) != '"') {
        if (c == '\n' || c == EOF)
            return NULL;
        if (p < end)
            *p++ = c;
    }
    *p = '\0';

    return new_string (buffer);
}


/* Return a string without blanks */

static char* get_ident (FILE* fp)
{
    char  buffer [FILE_SPEC_SIZE];
    char* end = buffer + sizeof (buffer) - 1;
    char* p = buffer;
    int   c;

    for (c = getnbc (fp); c != EOF && !isspace (c); c = getc (fp)) {
        if (p < end)
            *p++ = c;
    }
    *p = '\0';
    ungetc (c, fp);

    return p == buffer ? NULL : new_string (buffer);
}


/* Return a product description */

static product_t* get_product (FILE* fp)
{
    product_t* product = new (product_t);
    image_t**  next_image;
    char*      image_name;
    int        c;

    /* Skip empty lines */

    while ((c = getnbc (fp)) == '\n');
    ungetc (c, fp);

    /* We are a the beginning of a non-empty line or at end-of-file */

    if ((product->identifier = get_ident (fp)) == NULL ||
        (product->name = get_quoted (fp)) == NULL)
        return NULL;

    product->next = NULL;
    product->version = NULL;
    product->image_count = 0;
    next_image = &product->first_image;
    while ((image_name = get_ident (fp)) != NULL) {
        *next_image = new (image_t);
        (*next_image)->file_spec = image_name;
        next_image = &(*next_image)->next;
        product->image_count++;
    }

    *next_image = NULL;
    return product;
}


/*==============================================================================
 *
 *  This procedure fetches the product description file.
 */

static prodbase_t* get_prodbase (const cgi_t* cgi)
{
    FILE* fp;
    prodbase_t* prodbase;
    product_t** next;
    char* file_spec = parse_file (".PRODUCTS;", cgi->image_name, NULL);

    /* Allocate and initialize the database */

    prodbase = new (prodbase_t);
    prodbase->product_count = 0;
    prodbase->name_width = 0;
    prodbase->first_product = NULL;

    /* Read the description file */

    if (file_spec == NULL || (fp = fopen (file_spec, "r")) == NULL)
        return prodbase;

    for (next = &prodbase->first_product;
         (*next = get_product (fp)) != NULL;
         next = &(*next)->next) {

        int length = strlen ((*next)->name);
        if (length > prodbase->name_width)
            prodbase->name_width = length;
        prodbase->product_count++;
    }

    fclose (fp);

    return prodbase;
}


/*==============================================================================
 *
 *  This procedure displays 'count' times the string 'pattern',
 *  then the string 'trailer' only once.
 */

static void repeat (int count, const char* pattern, const char* trailer)
{
    while (count-- > 0)
        printf ("%s", pattern);
    printf ("%s", trailer);
}


/*==============================================================================
 *
 *  This procedure sends a plain text file to the Web client.
 */

static void send_text_file (
    const cgi_t* cgi,
    const char*  file,
    const char*  title)
{
    FILE* fp;
    int c;

    if (file == NULL)
        message_exit (cgi, "No file to send\n");

    if ((fp = fopen (file, "r")) == NULL)
        message_exit (cgi, "The %s is not present on this system.\n"
            "You may ask the WebMaster to install it in the following\n"
            "location:<pre>%s</pre>You may also\n<a href=\"mailto:"
            AUTHOR_MAIL "\">mail to the author</a> to receive a copy.\n",
            title, file);

    printf ("Content-Type: text/plain\n\n");
    while ((c = getc (fp)) != EOF)
        putchar (c);
    fclose (fp);
}


/*==============================================================================
 *
 *  These procedures send specific text files to the Web client.
 */

static void send_source (const cgi_t* cgi)
{
    send_text_file (cgi, parse_file (".C;", cgi->image_name, NULL),
        "C source file");
}

static void send_products_file (const cgi_t* cgi)
{
    send_text_file (cgi, parse_file (".PRODUCTS;", cgi->image_name, NULL),
        "products description file");
}


/*==============================================================================
 *
 *  This procedure displays a summary of the cluster.
 */

static void cluster_summary (cluster_t* cluster)
{
    ulong_t previous_version;
    disk_t* disk;
    char*   sep;

    printf ("This cluster contains:\n"
        "<ul>\n"
        "<li>%d nodes (%d Alpha, %d VAX)\n"
        "<li>%d system disks\n"
        "<li>%d versions of OpenVMS\n"
        "</ul>\n",
        cluster->node_count, cluster->alpha_count, cluster->vax_count,
        cluster->disk_count, cluster->version_count);

    printf ("The operation system versions are:<ul><li>OpenVMS Alpha ");
    previous_version = MAX_VERSION;
    sep = "";

    for (disk = cluster->first_disk; disk != NULL; disk = disk->next) {
        if (disk->is_alpha && disk->version != previous_version) {
            printf ("%s%s", sep, disk->node_list->version_name);
            previous_version = disk->version;
            sep = ", ";
        }
    }

    printf ("\n<li>OpenVMS VAX ");
    previous_version = MAX_VERSION;
    sep = "";

    for (disk = cluster->first_disk; disk != NULL; disk = disk->next) {
        if (!disk->is_alpha && disk->version != previous_version) {
            printf ("%s%s", sep, disk->node_list->version_name);
            previous_version = disk->version;
            sep = ", ";
        }
    }

    printf ("</ul>\n");
}


/*==============================================================================
 *
 *  This procedure the list of nodes with unknown system disk.
 */

static void display_unknown_disks (const cluster_t* cluster)
{
    node_t* node;
    int count;

    /* If all disks are known, do not modify display */

    if (cluster->unknown_disk_count == 0)
        return;

    printf ("<p><b>WARNING:</b> The system disk%s of node%s ",
        cluster->unknown_disk_count > 1 ? "s" : "",
        cluster->unknown_disk_count > 1 ? "s" : "");

    count = 0;

    for (node = cluster->first_node; node != NULL; node = node->next)
        if (node->system_disk [0] == '\0')
            printf ("%s%s", count++ > 0 ? ", " : "", node->node_name);

    printf ("\n%s not identified because the node%s not yet reachable.\n",
        cluster->unknown_disk_count > 1 ? "are" : "is",
        cluster->unknown_disk_count > 1 ? "s are" : " is");
}


/*==============================================================================
 *
 *  This procedure displays the list of nodes in the cluster.
 */

static void list_nodes (const cgi_t* cgi)
{
    cluster_t* cluster = get_cluster_config (cgi);
    node_t* node;
    char address [40];

    start_html (cgi, "VMScluster Configuration:<br>List of Nodes");
    cluster_summary (cluster);

    printf ("<hr><h2>List of Nodes</h2><pre>\n%-*s  %-*s  %-*s  %s\n",
        NODE_WIDTH, "Node", ADDRESS_WIDTH, "Address",
        MODEL_WIDTH, "Model", "Version");

    repeat (NODE_WIDTH, "-", "  ");
    repeat (ADDRESS_WIDTH, "-", "  ");
    repeat (MODEL_WIDTH, "-", "  ");
    repeat (VERSION_WIDTH, "-", "\n");

    for (node = cluster->first_node; node != NULL; node = node->next) {
        sprintf (address, "(%d.%d)", node->dnet_area, node->dnet_number);
        printf ("%-*s  %-*s  %-*s  %s",
            NODE_WIDTH, node->node_name, ADDRESS_WIDTH, address,
            MODEL_WIDTH, node->model, node->version_name);

        /* Non-standard versions are flagged with a "NEW" logo */

        if (node->version_name [0] != 'V')
            printf (" <img src=\"" NEW_PICTURE "\" align=bottom "
                "alt=\"[NEW]\">\n");
        else
            printf ("\n");
    }

    printf ("</pre>\n");
    display_unknown_disks (cluster);
    printf ("</body></html>\n");
}


/*==============================================================================
 *
 *  This procedure displays the list of system disks in the cluster.
 */

static void list_disks (const cgi_t* cgi)
{
    cluster_t* cluster = get_cluster_config (cgi);
    ulong_t previous_version;
    disk_t* disk;
    node_t* node;
    char* sep;
    int column;
    int max_column;

    start_html (cgi, "VMScluster Configuration:<br>List of System Disks");
    cluster_summary (cluster);

    printf ("<hr><h2>List of System Disks</h2>\n"
        "Click on the name of a system disk to scan the installed software\n"
        "products. Disks which are not mounted or not accessible on the host\n"
        "running the Web server cannot be selected.\n"
        "<pre>\n%-*s  %-*s  %-*s  %-*s  %s\n",
        DISK_WIDTH, "Disk", ARCH_WIDTH, "Arch.",
        VERSION_WIDTH, "Version", LABEL_WIDTH, "Label", "Nodes");

    repeat (DISK_WIDTH, "-", "  ");
    repeat (ARCH_WIDTH, "-", "  ");
    repeat (VERSION_WIDTH, "-", "  ");
    repeat (LABEL_WIDTH, "-", "  ");
    repeat (LIST_WIDTH, "-", "\n");

    for (disk = cluster->first_disk; disk != NULL; disk = disk->next) {

        column = DISK_WIDTH + ARCH_WIDTH + VERSION_WIDTH + LABEL_WIDTH + 8;
        max_column = column + LIST_WIDTH;

        if (!disk->is_mounted)
            printf ("%-*s", DISK_WIDTH, disk->name);
        else {
            char* colon = strchr (disk->name, ':');
            int dlength = strlen (disk->name);
            int clength = colon != NULL ? colon - disk->name : dlength;
            printf ("<a href=\"%s?" OPT_SCANDISK "+%.*s\">%s</a>%*s",
                cgi->script_name, clength, disk->name,
                disk->name, DISK_WIDTH - dlength, "");
        }

        printf ("  %-*s  %-*s  %-*s  ",
            ARCH_WIDTH, disk->is_alpha ? "Alpha" : "VAX",
            VERSION_WIDTH, disk->node_list->version_name,
            LABEL_WIDTH, disk->label);

        sep = "";
        for (node = disk->node_list; node != NULL; node = node->next_same_disk){
            column += strlen (sep) + strlen (node->node_name);
            if (column > max_column) {
                column = DISK_WIDTH + ARCH_WIDTH + VERSION_WIDTH +
                    LABEL_WIDTH + 8;
                printf (",\n%*s", column, "");
                sep = "";
            }
            printf ("%s%s", sep, node->node_name);
            sep = ", ";
        }
        printf ("\n");
    }

    printf ("</pre>\n");
    display_unknown_disks (cluster);
    printf ("</body></html>\n");
}


/*==============================================================================
 *
 *  This procedure displays a list of known products and give the
 *  opportunity to scan the cluster for a product.
 */

static void select_product (const cgi_t* cgi)
{
    prodbase_t* prodbase = get_prodbase (cgi);
    product_t* product;
    image_t* image;

    start_html (cgi, "Software Products");

    printf ("This utility can detect the following software products.\n"
        "Click on a product name to search all installed versions\n"
        "of this product in the cluster. See the <a href=\"%s?" OPT_HELP
        "\">online help</a> to add the recognition of new products.\n",
        cgi->script_name);

    if (cgi->key_count >= 2 && strcmp (cgi->key [1], OPT_FULL) == 0) {

        /* Detailed format */

        printf ("<ul>\n");

        for (product = prodbase->first_product;
             product != NULL;
             product = product->next) {

            printf ("<li><b><a href=\"%s?" OPT_SCANPRODUCT
                "+%s\">%s</a></b> (image%s: ",
                cgi->script_name, product->identifier, product->name,
                product->image_count > 1 ? "s" : "");

            for (image = product->first_image;
                 image != NULL;
                 image = image->next)
                printf ("%s%s", image->file_spec,
                    image->next == NULL ? ")\n" : ", ");
        }

        printf ("</ul>\nAlso available in <a href=\"%s?" OPT_SELECTPROD
            "\">brief</a> format.\n", cgi->script_name);
    }
    else {

        /* Brief format */

        int num_columns;
        int num_lines;
        product_t** row;
        int line;
        int col;

        /* Compute the number of columns */

        num_columns = (MAX_WIDTH + 2) / (prodbase->name_width + 2);
        if (num_columns == 0)
            num_columns = 1;

        /* Compute the number of lines per column */

        num_lines = (prodbase->product_count + num_columns - 1) / num_columns;

        /* Compute the start products of the first line */

        row = memalloc (num_columns * sizeof (product_t*));
        product = prodbase->first_product;

        for (col = 0; col < num_columns; col++) {
            if ((row[col] = product) != NULL) {
                for (line = 0; line < num_lines && product != NULL; line++)
                    product = product->next;
            }
        }

        /* Display the list of products */

        printf ("<pre>\n");

        for (line = 0; line < num_lines; line++) {
            for (col = 0; col < num_columns && row[col] != NULL; col++) {
                printf ("<a href=\"%s?" OPT_SCANPRODUCT "+%s\">%s</a>",
                    cgi->script_name, row[col]->identifier, row[col]->name);

                if (col == num_columns - 1 || row[col+1] == NULL)
                    printf ("\n");
                else
                    printf ("%*s", prodbase->name_width + 2 -
                        strlen (row[col]->name), "");

                row[col] = row[col]->next;
            }
        }

        printf ("</pre>\nAlso available in <a href=\"%s?" OPT_SELECTPROD "+"
            OPT_FULL "\">detailed</a> format.\n", cgi->script_name);

        free (row);
    }

    printf ("</body></html>\n");
}


/*==============================================================================
 *
 *  This procedure scans all system disks in the cluster, looking for
 *  one software product.
 */

static void scan_product (const cgi_t* cgi)
{
    const char* product_ident = cgi->key [1];
    prodbase_t* prodbase = get_prodbase (cgi);
    cluster_t*  cluster = get_cluster_config (cgi);
    product_t*  product;
    image_t*    img;
    disk_t*     disk;
    char*       file;
    int         count;
    int         width;

    /* Look for description of the product */

    for (product = prodbase->first_product;
         product != NULL && strcmp (product->identifier, product_ident) != 0;
         product = product->next);

    /* Give up if no description found */

    if (product == NULL)
        message_exit (cgi,
            "No known product has an identifier named <tt>%s</tt>.\n",
            product_ident);

    /* Look for the product on all mounted system disks */

    count = 0;
    for (disk = cluster->first_disk; disk != NULL; disk = disk->next) {
        if (!disk->is_mounted)
            disk->product_version = NULL;
        else {
            for (img = product->first_image; img != NULL; img = img->next) {
                file = parse_file (disk->name, img->file_spec,
                    "[VMS$COMMON.SYSEXE].EXE");
                if ((disk->product_version = image_ident (file)) != NULL) {
                    count++;
                    break;
                }
            }
        }
    }

    /* Display the list of disks having this product installed */

    start_html (cgi, "%s in the Cluster", product->name);

    if (count == 0) {
        printf ("%s was not found on any system disk in the cluster.\n<p>",
            product->name);
    }
    else {
        if ((width = strlen (product->name)) < PRODVER_WIDTH)
            width = PRODVER_WIDTH;

        printf ("%s was found on %d system disk%s:\n<pre>\n"
            "%-*s  %-*s  %-*s  %s\n",
            product->name, count, count > 1 ? "s" : "",
            DISK_WIDTH, "Disk", ARCH_WIDTH, "Arch.",
            VERSION_WIDTH, "OpenVMS", product->name);

        repeat (DISK_WIDTH, "-", "  ");
        repeat (ARCH_WIDTH, "-", "  ");
        repeat (VERSION_WIDTH, "-", "  ");
        repeat (width, "-", "\n");

        for (disk = cluster->first_disk; disk != NULL; disk = disk->next) {
            if (disk->product_version != NULL) {
                printf ("%-*s  %-*s  %-*s  %s\n",
                    DISK_WIDTH, disk->name,
                    ARCH_WIDTH, disk->is_alpha ? "Alpha" : "VAX",
                    VERSION_WIDTH, disk->node_list->version_name,
                    disk->product_version);
            }
        }
        printf ("</pre>\n");

        if (count > 0 && count < cluster->mounted_disk_count) {

            int not_count = cluster->mounted_disk_count - count;
            printf ("<p>%s was NOT found on %d system disk%s:\n<pre>\n"
                "%-*s  %-*s  OpenVMS\n",
                product->name, not_count, not_count > 1 ? "s" : "",
                DISK_WIDTH, "Disk", ARCH_WIDTH, "Arch.");

            repeat (DISK_WIDTH, "-", "  ");
            repeat (ARCH_WIDTH, "-", "  ");
            repeat (VERSION_WIDTH, "-", "\n");

            for (disk = cluster->first_disk; disk != NULL; disk = disk->next) {
                if (disk->is_mounted && disk->product_version == NULL) {
                    printf ("%-*s  %-*s  %s\n",
                        DISK_WIDTH, disk->name,
                        ARCH_WIDTH, disk->is_alpha ? "Alpha" : "VAX",
                        disk->node_list->version_name);
                }
            }
            printf ("</pre>\n");
        }

        if (cluster->mounted_disk_count < cluster->disk_count) {

            int not_count = cluster->disk_count - cluster->mounted_disk_count;
            printf ("<p>%d system disk%s not mounted or not accessible "
                "and %s not scanned:\n"
                "<pre>\n%-*s  %-*s  OpenVMS\n",
                not_count, not_count > 1 ? "s are" : " is",
                not_count > 1 ? "were" : "was",
                DISK_WIDTH, "Disk", ARCH_WIDTH, "Arch.");

            repeat (DISK_WIDTH, "-", "  ");
            repeat (ARCH_WIDTH, "-", "  ");
            repeat (VERSION_WIDTH, "-", "\n");

            for (disk = cluster->first_disk; disk != NULL; disk = disk->next) {
                if (!disk->is_mounted) {
                    printf ("%-*s  %-*s  %s\n",
                        DISK_WIDTH, disk->name,
                        ARCH_WIDTH, disk->is_alpha ? "Alpha" : "VAX",
                        disk->node_list->version_name);
                }
            }
            printf ("</pre>\n");
        }
    }

    display_unknown_disks (cluster);
    printf ("</body></html>\n");
}


/*==============================================================================
 *
 *  This procedure scans the software products of a system disk.
 */

static void scan_disk (const cgi_t* cgi)
{
    const char* disk_name = cgi->key [1];
    prodbase_t* prodbase = get_prodbase (cgi);
    cluster_t*  cluster = get_cluster_config (cgi);
    product_t*  prod;
    image_t*    img;
    disk_t*     disk;
    node_t*     node;
    char*       file;
    char*       sep;
    int         count;

    /* The input device name may contain no colon */

    if (strchr (disk_name, ':') == NULL) {
        char* full_name = memalloc (strlen (disk_name) + 2);
        strcpy (full_name, disk_name);
        strcat (full_name, ":");
        disk_name = full_name;
    }

    /* Look for a description of the disk */

    for (disk = cluster->first_disk;
         strcmp (disk_name, disk->name) != 0;
         disk = disk->next);

    /* Give up if no description found or can't scan disk */

    if (disk == NULL)
        message_exit (cgi, "No system disk is named %s.\n", disk_name);

    if (!disk->is_mounted)
        message_exit (cgi, "Disk %s is not mounted or not accessible "
            "on the Web Server host.\n", disk_name);

    /* Find all installed products on the disk */

    count = 0;
    for (prod = prodbase->first_product; prod != NULL; prod = prod->next) {
        for (img = prod->first_image; img != NULL; img = img->next) {
            file = parse_file (disk->name, img->file_spec,
                "[VMS$COMMON.SYSEXE].EXE");
            if ((prod->version = image_ident (file)) != NULL) {
                count++;
                break;
            }
        }
    }

    /* Display a usage summary of the disk */

    start_html (cgi, "Software Products on<br>System Disk %s", disk_name);

    printf ("<br>Device: %s\n<br>Label: %s\n<br>Architecture: %s\n"
        "<br>OpenVMS: %s\n<br>Nodes:",
        disk->name, disk->label, disk->is_alpha ? "Alpha" : "VAX",
        disk->node_list->version_name, disk_name);

    sep = " ";
    for (node = disk->node_list; node != NULL; node = node->next_same_disk){
        printf ("%s%s", sep, node->node_name);
        sep = ", ";
    }
    printf ("\n");

    /* Print the list of installed products */

    if (count == 0)
        printf ("<p>No software product was found.<p>\n");
    else {
        printf ("<p>The following software product%s installed:\n<pre>\n"
            "%-*s  Version\n",
            count > 1 ? "s are" : " is",
            prodbase->name_width, "Product");
        repeat (prodbase->name_width, "-", "  ");
        repeat (PRODVER_WIDTH, "-", "\n");
        for (prod = prodbase->first_product; prod != NULL; prod = prod->next) {
            if (prod->version != NULL)
                printf ("%-*s  %s\n", prodbase->name_width, prod->name,
                    prod->version);
        }
        printf ("</pre>\n");
    }

    /* Print the list of uninstalled products */

    if (count == prodbase->product_count)
        printf ("All known software products are installed.\n");
    else {
        int not_count = prodbase->product_count - count;
        printf ("The following software product%s not installed:",
            not_count > 1 ? "s are" : " is", disk->name);
        sep = "\n";
        for (prod = prodbase->first_product; prod != NULL; prod = prod->next) {
            if (prod->version == NULL) {
                printf ("%s%s", sep, prod->name);
                sep = ",\n";
            }
        }
        printf ("\n");
    }

    printf ("</body></html>\n");
}


/*==============================================================================
 *
 *  This procedure sends a help document.
 */

static void help (const cgi_t* cgi)
{
    start_html (cgi, PRODUCT_NAME " " PRODUCT_VERSION "<br>Online Help");

    printf ("This utility displays the configuration of a VMScluster through\n"
        "the World-Wide Web (WWW). The displayed information is generated\n"
        "using a CGI (Common Gateway Interface) script running\n"
        "on the Web server host.\n"

        "<p><h2>Functions</h2>\n"

        "This utility can:<ul>\n"
        "<li>List the nodes of the cluster with their name, DECnet address, "
        "hardware model and OpenVMS version.\n"
        "<li>List all system disks in the cluster with their OpenVMS version "
        "and the list of nodes booting on them.\n"
        "<li>List all software products which are installed on a selected "
        "system disk with their version.\n"
        "<li>List all versions of a selected software product with the list "
        "of system disks where they are installed.\n"
        "</ul>\n"
        "Use the <b>Options</b> bar on top of the page to select a function.\n"

        "<p><h2>Products Configuration File</h2>\n"

        "The <i>Products Configuration File</i> describes a set of known\n"
        "software products. This is a text file.\n"
        "Each product is described by one line of text.\n"
        "This line has three fields. Fields are separated by spaces\n"
        "or tabulations.\n"

        "<ul><li>First field: Identifier. This is a unique alpha-numerical "
        "name, internally used for CGI callback.\n"
        "<li>Second field: Product name, between double quotes. This is the\n"
        "name which will be displayed.\n"
        "<li>Third field: List of image files. The default specification is\n"
        "<tt>[vms$common.sysexe].exe</tt> on the system disk. Separate\n"
        "multiple specifications with spaces.</ul>\n"

        "<p>Comments may be used anywhere in the file. A comment extends\n"
        "from an exclamation mark <tt>!</tt> to the end of the line.\n"
        "Empty lines are ignored.\n"

        "<p>The <i>Products Configuration File</i> must be placed in the\n"
        "same directory as the CGI script.\n"
        "The configuration file must have the same name as the script's\n"
        "executable image, with a <i>.PRODUCTS</i> extension.\n"

        "<p><h2>Installing the " PRODUCT_NAME " on your Web Server</h2>\n"

        "This script is written in C. There is only one source file.\n"
        "<a href=\"%s?" OPT_GETSOURCE "\">Click here</a> to get a copy.\n"

        "<p>Simply compile and link this file using the DEC C compiler.\n"
        "Then place the executable file in a script directory of your Web\n"
        "Server. To allow further distribution of the source file, also\n"
        "place it in the same directory.\n"

        "<p>The last thing you need is the Products Configuration File.\n"
        "<a href=\"%s?" OPT_GETPRODFILE "\">Click here</a> to get a copy of\n"
        "the file used by this Web server. Also place it in the same\n"
        "directory\n"

        "<p>The script directory should consequently contain the following "
        "three files:<ul>\n"

        "<li><tt>show-cluster.exe</tt>\n"
        "<li><tt>show-cluster.products</tt>\n"
        "<li><tt>show-cluster.c</tt> (optional)</ul>\n"

        "<p>In case of problem, feel free to <a href=\"mailto:" AUTHOR_MAIL
        "\">mail to the author</a>.\n"

        "</body></html>\n",

        cgi->script_name, cgi->script_name);
}


/*==============================================================================
 *
 *  Program entry point.
 */

int main (int argc, char** argv)
{
    typedef struct {
        const char* option_name;
        int required_arguments;
        void (*action) (const cgi_t*);
    } option_t;

    static const option_t options [] = {
        {OPT_LISTNODES,   1, list_nodes},  /* first is default */
        {OPT_LISTDISKS,   1, list_disks},
        {OPT_HELP,        1, help},
        {OPT_GETSOURCE,   1, send_source},
        {OPT_GETPRODFILE, 1, send_products_file},
        {OPT_SCANDISK,    2, scan_disk},
        {OPT_SCANPRODUCT, 2, scan_product},
        {OPT_SELECTPROD,  1, select_product},
    };

    cgi_t* cgi = get_cgi (argc, argv);
    const option_t *opt = options + array_size (options);

    while (--opt > options && (cgi->key_count < opt->required_arguments ||
        strcmp (cgi->key [0], opt->option_name) != 0));

    opt->action (cgi);
}
