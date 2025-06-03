/*
 * Load routine from shareable image.
 *
 * int http_load_dynamic_service ( string token*, int tokcnt, char *errmsg );
 *
 * int http_load_dynamic_rule ( blk, name, image, fspec )
 *
 * Arguments:
 *     blk		Rule definition, load_dynamic fill in the name
 *			and usermap elements of this structure.
 *			If load fails, usermap is set to dummy routine
 *			that aways returns failure when called.
 *
 *	name		Symbolic name of usermap routine in image.
 *	image		Name of shareable image, it not specified, image
 *			of WWW_RULE_'name' used.
 *	fspec		Directory name, defaults to WWW_SYSTEM.
 *
 * The dynamic map rule is called with the following arguments:
 *	status = usermap ( blk, translation, maxlen, uic );
 *   Return value (symbolic value defined in ident_map.h):
 *	IDENT_MAP_LOOP		Continue with next rule, translation buffer
 *				may have been changed.
 *	IDENT_MAP_FALIED	Abort translation with failure status.
 *	IDENT_MAP_OK_xxx	Terminate translation and take action:
 *				    PASS, REDIRECT, EXEC, USERMAP
 *   Arguments:
 *	blk		Rule definition block.
 *	translation	Character array, maxlen long.  Working buffer for
 *			translating ident.  The usermap routine overwrites
 *			this buffer with the new result.
 *	maxlen		int, size of translation buffer.
 *	uic		long pointer.  output UIC.
 *
 *   Author:	David Jones, George Carrette
 *   Revised:	10-JUN-1995	Added load_dynamic_service routine
 *   Revised:	12-JUN-1995	Additional fields in callback vector.
 *   Revised:	23-JUN-1995	Don't upcase symbolic names in unix.
 */
#include "pthread_np.h"
#include <stdlib.h>
#include <stdio.h>
#ifdef VMS
#include <descrip.h>
int LIB$FIND_IMAGE_SYMBOL();
#else
#include <dlfcn.h>		/* Dynamic library interface */
struct dsc$descriptor { short l, ct; char *a };
#endif
#include "access.h"
#include "ident_map.h"
#include "tutil.h"
#include "file_access.h"
#include "message_service.h"

#define FSPEC_DEFAULT "WWW_SYSTEM:.EXE"
#define STACKSIZE_DEFAULT 60000

int http_log_level, tlog_putlog(int, char*, ...);
map_callout http_usermap_failure;	/* forward reference */
static struct callback_vector *create_callback_vector();
static struct dsc$descriptor *set_dx 
	( struct dsc$descriptor *dx, char *s, int l );
/*****************************************************************************/
int http_load_dynamic_rule ( info_ptr blk,	/* rule definition block */
	char *name,			/* Routine name */
	char *image,			/* Shareable image */
	char *fspec )			/* Directory specification */
{
    int status;
    struct dsc$descriptor name_dx, image_dx, fspec_dx;
    char image_default[256], init_name[40];
    void *dlib;				/* dynamic library handle */
    /*
     * Build VMS descriptors for LIB$FIND_IMAGE_SYMBOL.
     */
    set_dx ( &name_dx, name, -1 );
    if ( image ) {
	set_dx ( &image_dx, image, -1 );
    } else {
	tu_strcpy ( image_default, "WWW_RULE_" );
	tu_strnzcpy ( &image_default[9], name, sizeof(image_default)-10 );
	set_dx ( &image_dx, image_default, -1 );
    }
    set_dx ( &fspec_dx, fspec ? fspec : FSPEC_DEFAULT, -1 );

#ifdef VMS
    status = LIB$FIND_IMAGE_SYMBOL 
	( &image_dx, &name_dx, &blk->usermap, &fspec_dx );

    if ( 1 == (status&1) ) {
#else
    status = 1;
    dlib = dlopen ( image_dx.a, RTLD_NOW );
    if ( !dlib ) {
	return 0;
    }
    blk->usermap = dlsym ( dlib, name_dx.a );
    if ( !blk->usermap ) {
	return 0;
    } else {
#endif
	/*
	 * Look for init routine.
	 */
	int (*init_routine)();

	tu_strcpy ( init_name, "INIT_" );
	tu_strnzcpy ( &init_name[5], name, sizeof(init_name)-6 );
	set_dx ( &name_dx, init_name, -1 );
#ifdef VMS
	status = LIB$FIND_IMAGE_SYMBOL
		( &image_dx, &name_dx, &init_routine, &fspec_dx );
	if ( 1 == (status&1) ) {
	    status = init_routine ( create_callback_vector(),
		sizeof(struct callback_vector) );
	}
#else
	init_routine = dlsym ( dlib, name_dx.a );
	if ( !init_routine ) {
	    status = 0;
	}
#endif
    }

    if ( 1 == (status&1) ) {
	/* Re-write name field */
	int size;
	char *dispname;
	size = tu_strlen ( name ) + 4;
	if ( size < 10 ) size = 10;
	dispname = (char *) malloc ( size );  if ( !dispname ) return status;

	dispname[0] = '[';
	tu_strcpy ( &dispname[1], name );
	size = tu_strlen(dispname);
	dispname[size++] = ']';
	dispname[size++] = ':';
	while ( size < 9 ) dispname[size++] = ' ';
	dispname[size] = '\0';
	blk->name = dispname;
    } else {
	blk->usermap = http_usermap_failure;
    }
    return status;
}
/*****************************************************************************/
/* Fallback routine for usermap, always fail. */
int http_usermap_failure ( info_ptr blk, char * trans,
	int maxlen, access_info acc, char **cp_buf, int cpbuflen )
{
    return 0;
}
/***************************************************************************/
/*  Allocate and initialize callback vector.
 */
struct callback_vector *create_callback_vector()
{
    struct callback_vector *new;

    new = (struct callback_vector *) malloc ( sizeof(struct callback_vector) );
    new->maj_version = DYN_VERSION_MAJOR;
    new->min_version = DYN_VERSION_MINOR;
    new->trace_level = &http_log_level;
    new->reentrant_c_rtl = &http_reentrant_c_rtl;
    new->reentrant_vms_rtl = &http_reentrant_vms_rtl;
    new->putlog = tlog_putlog;
    new->tf_open = tf_open;
    new->tf_close = tf_close;
    new->tf_read = tf_read;
    new->tf_last_error = tf_last_error;
    new->tf_getline = tf_getline;
    new->tf_header_info = tf_header_info;
    return new;
}
/*****************************************************************************/
/* Dynamically load server thread routines and bind to service pool.
 * return values:
 *    1		Success.
 *    0		Error, detail in errmsg array.
 *
 * We assume this routine is called only by a single thread.  Token argument
 * is array of words parsed from rule file line:
 *
 *	ThreadPool poolname [attribute=val [...]]
 *			(attributes: stack, q_flag, limit)
 *
 *	Service servicename [attribute=val [...]]
 *			(attributes: pool, dynamic, builtin, info, init)
 *			(dynamic=(routine,image,file)).
 */
typedef struct { int l; char *s; } string;
struct pooldefinition { struct pooldefinition *next;
	char *name; 
	pthread_attr_t attr; 
	int id, limit, q_flag; };
typedef struct pooldefinition pooldef;

static pooldef *pool_list;

static int default_pool;		/* pool-id to use by default */

static pooldef *lookup_pool ( char *name, int create_if )
{
    int length;
    pooldef *blk;
    /*
     * Scan list and return block with matching name.
     */
    length = tu_strlen ( name ) + 1;
    for ( blk = pool_list; blk; blk = blk->next ) {
	if ( tu_strncmp ( name, blk->name, length ) == 0 ) return blk;
    }
    /*
     * Block not found, see if we should create it.
     */
    if ( create_if ) {
	blk = malloc ( sizeof ( pooldef ) );
	if ( !blk ) return blk;
	blk->name = malloc ( length );
	if ( !blk->name ) return (pooldef *) 0;
	tu_strcpy ( blk->name, name );

	pthread_attr_create ( &blk->attr );
	pthread_attr_setinheritsched ( &blk->attr, PTHREAD_DEFAULT_SCHED );
	pthread_attr_setstacksize ( &blk->attr, STACKSIZE_DEFAULT );
	blk->q_flag = 1;
	blk->id = -1;		/* flag as undefined */
	blk->limit = 3;
	blk->next = pool_list;
	pool_list = blk;
    }
    return blk;
}

static mst_linkage create_mst_linkage()
{
    mst_linkage new;
    new = (mst_linkage) malloc ( sizeof(struct mst_callback_vector) );
    new->version = MST_CB_VERSION;
    new->trace_level = &http_log_level;
    new->reentrant_c_rtl = &http_reentrant_c_rtl;
    new->reentrant_vms_rtl = &http_reentrant_vms_rtl;
    new->putlog = tlog_putlog;

    new->read = mst_read;
    new->write = mst_write;
    new->close = mst_close;
    new->exit = mst_exit;
    new->format_error = mst_format_error;
    new->connect = mst_connect;
    new->create_service_pool = mst_create_service_pool;
    new->register_service = mst_register_service;
    return new;
}

static struct dsc$descriptor *set_dx 
	( struct dsc$descriptor *dx, char *s, int l )
{
#ifdef VMS
    dx->dsc$b_dtype = DSC$K_DTYPE_T;			/* text data */
    dx->dsc$b_class = DSC$K_CLASS_S;			/* fixed (statics) */
    dx->dsc$a_pointer = s;				/* data */
    if ( l >= 0 ) dx->dsc$w_length = l;
    else {
	/* Take length from data, upper case if l != -1 */
	dx->dsc$w_length = tu_strlen ( s );
	if ( l < -1 ) tu_strupcase ( s, s );
    }
#else
    dx->ct = 0; dx->a = s;
    if ( l >= 0 ) dx->l = l;
   else {
	dx->l = tu_strlen ( s );
	/* if ( l < -1 ) tu_strupcase ( s, s ); */
   }
#endif
    return dx;
}

/****************************************************************************/
int http_load_dynamic_service ( string *token, int tokcnt, char errmsg[256] )
{
    int status, i, pool_id;
    static int initialized = 0;
    pooldef *blk;
    mst_start_routine_t *start_routine;
    int (*init_routine)(); /* (mst_linkage, char *, char *); */
    struct dsc$descriptor name_dx, image_dx, fspec_dx;
    char image_default[256], keyword[40], init_name[40], sname[40];
    void *dlib;
    /*
     * Initialize static pointers on first call.
     */
    if ( !initialized ) {
	pool_list = (pooldef *) 0;
	default_pool = -1;
	initialized = 1;
    }
    /*
     * See which rule we are handling.
     */
    if ( *token[1].s == '\0' ) {
	 tu_strcpy ( errmsg, "Syntax error in pool/service definition" );
	return 0;
    } else if ( tu_strncmp ( token[0].s, "THREADPOOL", 11 ) == 0 ) {
	/*
	 * Threadpool rule, lookup/allocate pool name in pool_list.
	 */
	blk = lookup_pool ( token[1].s, 1 );
	if ( !blk ) {		/* allocation error */
	    tu_strcpy ( errmsg, "Allocation failure on pool struct" );
	    return 0;
	} else if ( blk->id > -1 ) {
	    /*
	     * Existing pool definition found.
	     */
	    tu_strcpy ( errmsg, "Multiple pool definition" );
	    return 0;
	} else {
	    /*
	     * new pool created, parse set of tokens which are of form
	     * 'keyword=value'.
	     */
	    errmsg[0] = '\0';
	    for ( i = 2; i < tokcnt; i++ ) {
		tu_strnzcpy ( keyword, token[i].s, sizeof(keyword)-1 );
		tu_strupcase ( keyword, keyword );
		if ( tu_strncmp ( keyword, "STACK=", 6 ) == 0 ) {
		    int stacksize;
		    stacksize = atoi ( &token[i].s[6] );
		    if ( stacksize < 4000 ) stacksize = 4000;
		    pthread_attr_setstacksize ( &blk->attr, stacksize );

		} else if ( tu_strncmp ( keyword, "LIMIT=", 6 ) == 0 ) {
		    blk->limit = atoi ( &token[i].s[6] );
		} else if ( tu_strncmp ( keyword, "Q_FLAG=", 7 ) == 0 ) {
		    blk->q_flag = atoi ( &token[i].s[7] );
		} else {
		    sprintf (errmsg, "Unknown pool attribute (%s)", keyword);
		    return 0;
		}
	    }
	    /*
	     * Create a pool with the requested attributes.
	     */
	    status = mst_create_service_pool ( blk->attr,
		blk->limit, blk->q_flag, &blk->id );
	    if ( (status&1) == 0 ) tu_strcpy ( errmsg, 
		"Error creating service pool" );
	    return (status&1);
	}
    } else if ( tu_strncmp ( token[0].s, "SERVICE", 8 ) == 0 ) {
	char *image, *start, *dir, *info, *init;
	/*
	 * Service rule, parse tokens of form 'keyword=value'
	 */
	pool_id = default_pool;		/* default pool ID */
	start = token[1].s;
	image = dir = init = (char *) 0;
	info = "Dynamic service";
	for ( i = 2; i < tokcnt; i++ ) {
	    /*
	     * Compare token with possible keywords.
	     */
	    tu_strnzcpy ( keyword, token[i].s, sizeof(keyword)-1 );
	    tu_strupcase ( keyword, keyword );
	    if ( tu_strncmp ( keyword, "POOL=", 5 ) == 0 ) {
		/*
		 * Determine pool number.
		 */
		blk = lookup_pool ( &token[i].s[5], 0 );
		if ( !blk ) {
		    tu_strcpy ( errmsg, "Unknown pool name" );
		    return 0;
		}
		pool_id = blk->id;
	    } else if ( 0 == tu_strncmp ( keyword, "DYNAMIC=", 8 ) ) {
		/*
		 * Token value is parenthsized list: (routine,image[,dir])
		 */
		start = &token[i].s[8];
		if ( *start != '(' ) {
		    tu_strcpy ( errmsg, "Missing '(' on DYANMIC value" );
		    return 0;
		}
	    } else if ( 0 == tu_strncmp ( keyword, "BUILTIN=", 8 ) ) {
		/*
		 * Service uses pre-compiled MST.
		 */
		start = &token[i].s[8];
		if ( *start == '(' ) start++;
	    } else if ( tu_strncmp ( keyword, "INFO=", 5 ) == 0 ) {
		info = &token[i].s[5];
	    } else if ( tu_strncmp ( keyword, "INIT=", 5 ) == 0 ) {
		init = &token[i].s[5];
	    } else {
		sprintf (errmsg, "Unknown service attribute (%s)", keyword);
		return 0;
	    }
	}
        tlog_putlog(11,"service args: start='!AZ', init='!AZ'!/",
	     start, init ? init : "-" );
	/*
	 * Allocate pool if unknown, default pool will have 3 threads and
	 * stack of STACKSIZE_DEFAULT (60000).
	 */
	if ( pool_id < 0 ) {
	    pthread_attr_t attr;

	    pthread_attr_create ( &attr );
	    pthread_attr_setinheritsched ( &attr, PTHREAD_DEFAULT_SCHED );
	    pthread_attr_setstacksize ( &attr, STACKSIZE_DEFAULT );
	    status = mst_create_service_pool ( attr, 3, 1, &pool_id );
	    if ( (status&1) == 0 ) {
		tu_strcpy ( errmsg, "error creating default service pool" );
		return 0;
	    }
	    tlog_putlog(1,"Using default service pool for !AZ!/", start );
	    default_pool = pool_id;
	}
	/*
	 * Translate start routine name into routine address.
	 */
	if ( *start != '(' ) {
	    /*
	     * Lookup builtin name
	     */
	    if ( tu_strncmp ( start, "mapimage", 9 ) == 0 ) {
		extern mst_start_routine_t mapimage_mst;
		start_routine = mapimage_mst;
	    } else if ( tu_strncmp ( start, "cgifork", 8 ) == 0 ) {
		extern mst_start_routine_t cgifork_mst;
		int cgifork_configure();
		status = cgifork_configure();
		start_routine = cgifork_mst;
	    } else {
	        tu_strcpy ( errmsg, "builtin name unknown" );
	        return 0;
	    }
	} else {
	    /*
	     * Dynamically loaded MST, parse into routine, imge, dir
	     */
	    start++;
	    for ( i = 0; start[i] != ')'; i++ ) {
		if ( start[i] == ',' ) {
		    start[i] = '\0';
		    if ( !image  ) image = &start[i+1];
		    else if ( !dir ) dir = &start[i+1];
		    else {
			tlog_putlog(0,"Extra arg in DYNAMIC value ignored!/");
			break;
		    }
		} else if ( !start[i] ) {
		    tu_strcpy ( errmsg, "Missing ')' on DYNAMIC value" );
		    return 0;
		}
	    }
	    start[i] = '\0';
	    /*
	     * Make descriptors from the results.
	     */
	    set_dx ( &name_dx, start, -2 );	/* upcase the string */
            if ( image ) {
	        set_dx ( &image_dx, image, -1 );
            } else {
	        tu_strnzcpy ( image_default, start, sizeof(image_default)-5 );
	        tu_strcpy ( &image_default[tu_strlen(image_default)], "_mst" );
	        set_dx ( &image_dx, image_default, -2 );
	    }

	    set_dx ( &fspec_dx, dir ? dir : FSPEC_DEFAULT, -1 );
	    /*
	     * Attempt to load start routine.
	     */
#ifdef VMS
	    status = LIB$FIND_IMAGE_SYMBOL 
	        ( &image_dx, &name_dx, &start_routine, &fspec_dx );
	    if ( (status&1) == 0 ) {
	        sprintf ( errmsg, "Image load error: %d", status );
	        return 0;
	    }
#else
	    dlib = dlopen ( image_dx.a, RTLD_NOW );
	    if ( dlib ) {
		start_routine = dlsym ( dlib, name_dx.a );
		if ( !start_routine ) {
		sprintf ( errmsg, "symbol lookup error: %s", dlerror() );
		return 0;
		}
	    } else {
		sprintf ( errmsg, "Image load error: %s", dlerror() );
		return 0;
	    }
#endif
	    /*
	     * See if init routine present and call it.
	     */
            if ( init ) {
	        set_dx ( &name_dx, init, -1 );
            } else {
	        tu_strnzcpy ( init_name, start, sizeof(init_name)-6 );
	        tu_strcpy ( &init_name[tu_strlen(init_name)], "_init" );
	        set_dx ( &name_dx, init_name, -2 );
	    }
#ifdef VMS
	    status = LIB$FIND_IMAGE_SYMBOL 
	        ( &image_dx, &name_dx, &init_routine, &fspec_dx );
	    if ( (status&1) == 0 ) {
	        sprintf ( errmsg, "Image load error on init: %d", status );
	        return 0;
	    } else {
	        status = init_routine( create_mst_linkage(), info, errmsg );
	        if ( (status&1) == 0 ) return 0;
	    }
#else
	    init_routine = dlsym ( dlib, name_dx.a );
	    if ( !init_routine ) {
	        sprintf (errmsg, "Symbol lookup error on init: %s", dlerror());
	        return 0;
	    } else {
	        status = init_routine( create_mst_linkage(), info, errmsg );
	        if ( (status&1) == 0 ) return 0;
	    }
#endif
	}
	/*
	 * Register the new service, prefixing name with '%'.
	 */
	tu_strnzcpy ( &sname[1], token[1].s, sizeof(sname)-2 );
	sname[0] = '%';
	status = mst_register_service (sname, start_routine, info, pool_id);
	if ( (status&1) == 0 ) {
	    tlog_putlog ( 0, "Error registering service '!AZ'!/", token[1].s );
	    mst_format_error ( status, errmsg, 255 );
	} else {
	    if ( http_log_level > 0 ) tlog_putlog ( 1,
		"Loaded service !AZ, using pool !SL!/", token[1].s, pool_id );
	}
	return (status&1);

    } else {
	/*
	 * We shouldn't have been called with this token[0].
	 */
	tu_strcpy ( errmsg, "Internal error in config file processing" );
	return 0;
    }
    return 1;
}
