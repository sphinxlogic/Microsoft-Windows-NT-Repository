#ifndef MMK_H_LOADED
#define MMK_H_LOADED
/*
** MMK.H
**
**  Main #include file for Matt's Make utilty.
**
**  COPYRIGHT © 1992-1994, MADGOAT SOFTWARE.  ALL RIGHTS RESERVED.
**
**  MODIFICATION HISTORY:
**
**  27-Aug-1992	    Madison 	Initial commenting.
**  02-Apr-1993	    Madison 	Add new flag to CMD structure.
**  22-Oct-1993	    Madison 	Better support for deleting intermediates.
**  01-JUL-1994	    Madison 	Add CMS support.
**  14-JUL-1994	    Madison 	Add prefixed inference rule support.
*/

/*
** Generic stuff first
*/
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*
** Some private type names that I use sometimes
*/
#ifdef __GNUC__
#include <vms/descrip.h>
#include <vms/stsdef.h>
#include <vms/ssdef.h>
#define EXTERN	    extern
#define EXTERNAL    extern
#define GLOBAL	    /* */
    extern char *strchr(), *strrchr();
    extern void *memcpy(), *memmove();
    extern int memcmp();
#else
#include <descrip.h>
#include <stsdef.h>
#include <ssdef.h>
#include <lib$routines.h>
#include <str$routines.h>
#include <starlet.h>
#ifdef __DECC
#include <builtins.h>
#else
#pragma builtins
#endif
#define EXTERN	    globalref
#define EXTERNAL    globalref
#define GLOBAL	    globaldef
#endif

typedef unsigned long	LONG;
typedef unsigned short	WORD;
typedef	unsigned char	BYTE;
typedef void 	    	*POINTER;
typedef struct { LONG long1, long2; } TIME;
typedef struct dsc$descriptor DESCRIP;
typedef struct { WORD bufsiz, itmcod; POINTER bufadr, retlen; } ITMLST;

/*
** Handy macros
*/
#define OK(s) $VMS_STATUS_SUCCESS(s)
#define INIT_DYNDSCPTR(str) {str->dsc$w_length = 0; str->dsc$a_pointer = (void *) 0;\
    	    str->dsc$b_class = DSC$K_CLASS_D; str->dsc$b_dtype = DSC$K_DTYPE_T;}
#define INIT_DYNDESC(str) {str.dsc$w_length = 0; str.dsc$a_pointer = (void *) 0;\
    	    str.dsc$b_class = DSC$K_CLASS_D; str.dsc$b_dtype = DSC$K_DTYPE_T;}
#define INIT_SDESC(str,len,ptr) {str.dsc$w_length=(len);str.dsc$a_pointer=(void *)(ptr);\
    	    str.dsc$b_class=DSC$K_CLASS_S; str.dsc$b_dtype=DSC$K_DTYPE_T;}
#define ITMLST_INIT(itm,c,s,a,r) {itm.bufsiz=(s); itm.itmcod=(c);\
    	    itm.bufadr=(POINTER)(a); itm.retlen=(POINTER)(r);}

#ifdef __ALPHA
#define queue_insert(item,pred) __PAL_INSQUEL((void *)(pred),(void *)(item))
#define queue_remove(entry,addr) (((struct QUE *)(entry))->head == \
   (struct QUE *) (entry) ? 0:(__PAL_REMQUEL((void *)(entry),(void *)(addr)),1))
#else
#ifndef __GNUC__
#define queue_insert(item,pred) _INSQUE(item,pred)
#define queue_remove(entry,addr) (((struct QUE *)entry)->head == entry ? 0 :\
    	    	    	    (_REMQUE(entry,addr),1))
#endif
#endif

/*
** Structure definitions.  The following size definitions are just
** for the sizes of names for things, not for the structures themselves.
*/

#define MMK_S_SFX   	41  	/* 39 + leading dot + trailing null */
#define MMK_S_SYMBOL	33  	/* 32 is LIB$TPARSE limit, + trailing null */
#define MMK_S_MODULE	33  	/* 32 is librarian limit, + trailing null */
#define MMK_S_FILE  	256 	/* 255 is RMS limit, + trailing null */

/*
** Generic absolute queue header
*/

    struct QUE {
    	void *head, *tail;
    };

/*
** Symbol definition.  We don't get a whole lot of symbols defined,
** so we just store them in a regular linked list.  The value string
** is dynamically allocated.
*/

    struct SYMBOL {
    	struct SYMBOL *flink, *blink;
    	char name[MMK_S_SYMBOL];
    	char *value;
    };

/*
** Suffix defintion.  Linked list of suffixes is derived from .SUFFIX
** directive.
*/

    struct SFX {
    	struct SFX *flink, *blink;
    	char value[MMK_S_SFX];
    };

/*
** Command line definition.  The command string is dynamically allocated.
*/

#define CMD_M_NOECHO	1
#define CMD_M_IGNERR	2

    struct CMD {
    	struct CMD *flink, *blink;
    	unsigned int flags;
    	char *cmd;
    };

/*
** Default build rule definition.  Lookup is based on source suffix and
** target suffix.  Provides the commands used to build the target from
** the source, in the absence of any commands provided by a dependency
** rule.
*/

    struct RULE {
    	struct RULE *flink, *blink;
	struct RULE *next;
    	int srcpfxlen, trgpfxlen;
    	char src[MMK_S_SFX];
    	char trg[MMK_S_SFX];
	char srcpfx[MMK_S_FILE];
	char trgpfx[MMK_S_FILE];
    	struct CMD cmdque;
    };

/*
** Object definition.  An object can be a file, a module from a library,
** a library (which is just a file, really), or just a generic target
** name.  One data structure handles all types.
*/

#define MMK_K_OBJ_FILE	    1
#define MMK_K_OBJ_LIBMOD    2
#define MMK_K_OBJ_LIB	    3
#define MMK_K_OBJ_GENERIC   4
#define MMK_K_OBJ_CMSFILE   5

    struct OBJECT {
    	struct OBJECT *flink, *blink;
    	unsigned int reserved;  /* add nothing above this point! */
    	int type;
    	int build_in_progress;
    	TIME rdt;
    	struct QUE libmodque;	  /* Back pointer(s) to library module(s) */
    	int have_rdt;
    	int libfile_built;  	  /* OK to delete a library module's intermediate */
    	struct OBJECT *libfile;   /* for LIBMOD type, pointer to LIB object */
    	struct OBJECT *libmodobj; /* ref to LIBMOD object for use during build */
    	struct OBJECT *fileobj;   /* for LIBMOD type, pointer to file object */
    	char sfx[MMK_S_SFX];      /* not used for LIBMOD type */
    	char name[MMK_S_FILE];
    	char cms_gen[MMK_S_SYMBOL]; /* used only for CMSFILE type */
    };

/*
** Object reference definition.  Used in source lists in dependency rules
** to reference an actual object.
*/

    struct OBJREF {
    	struct OBJREF *flink, *blink;
    	struct OBJECT *obj;
    };

/*
** Dependency rule definition.  A single object can depend on several sources.
** When dependencies with multiple targets are given in a description file,
** each target gets its own dependency rule, and the common sources are
** duplicated into each rule.  Command lists are shared in the multiple-target
** case, though.
*/

    struct DEPEND {
    	struct DEPEND *flink, *blink;
    	struct DEPEND *dc_flink;
    	struct DEPEND *deplist_flink;
    	struct OBJECT *target;
    	struct OBJREF sources;
    	struct CMD    *cmdqptr;
    	int double_colon;
    };

#endif /* MMK_H_LOADED */
