/*
 * This module provides auxillary support functions for the DECNET_ACCESS
 * module, namely construction of task specifications from a default and
 * script directory prefix.
 *
 * The module is initialized by calling dnetx_initialize() with the name
 * of a DECnet task (e.g. WWWEXEC) or a logical that translates to
 * a task specification.  This provides the default task specification used
 * by dnetx_parse_task().
 *
 * The default task name logical may specify a search list of nodes in
 * one of 2 ways:
 *
 *	$ DEFINE taskname "node1::""0=taskname""",node2::,node3::,...
 *
 *	$ DEFINE taskname "node_list::""0=taskname"""
 *	$ DEFINE node_list node1::,node2::,node3::,...
 *
 * Additional targets defined by dnetx_define_task() must use the second
 * method (logical node name) to specify a search list.
 *
 * int dnetx_initialize ( default_taskname );
 * int dnetx_define_task ( char *logical_task );
 * int dnetx_parse_task 
 *	( char *logical_task, int *task_len, char ***tasklist, int *rr_point );
 *
 * Author:	David Jones
 * Date:	 6-AUG-1994
 */
#include "pthread_np.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <descrip.h>
#include <iodef.h>
#include <lnmdef.h>

#include "tutil.h"
#include "decnet_searchlist.h"	/* validate prototypes against actual */
#define DISABLE_ASTS pthread_mutex_lock(&ctx_list); /* disable AST delivery */
#define ENABLE_ASTS pthread_mutex_unlock(&ctx_list); /* enable AST delivier */
int SYS$DASSGN(), SYS$QIO(), tlog_putlog();
int http_log_level;				/* global variable */
/*
 * Global (module-wide) variables, Initialized by dnetx_initialize.
 */
static int dnet_defnode;			/* Length of node portion */

struct task_block {
    struct task_block *next;
    int rr_point;
    int log_task_len;			/* Length of log_task value */
    char log_task[256];			/* Original task name */
    char *asn_task[128];		/* List of logicals */
};
static struct task_block dnet_default_task;	/* First block in list */
static struct task_block *dnet_task_defs_end;	/* Last block in list */

static pthread_mutex_t dnet_task;		/* mutex for task list update*/
/****************************************************************************/
/* Private routine to expand log_task string in task block to list of 
 * assigned tasks.  If name_override is non-NULL, it will specify the
 * logical name to attempt to translate rather than the node name parsed
 * from tb->log_task.
 */
static int expand_logical_task ( char *name_override, struct task_block *tb )
{
    int length, flags, status, node_len, SYS$TRNLNM(), task_len, rr_point;
    int index, max_index, name_len, outndx;
    char *asn_spec, string[256];
    struct { short length, code; void *buf; void *retlen; } item[4];
    $DESCRIPTOR(table_name,"LNM$FILE_DEV");
    $DESCRIPTOR(log_name,"");
    /*
     * Default to asn_task list that consists solely of itself.
     */
    if ( http_log_level > 6 ) 
	tlog_putlog ( 7, "Expanding logical task: '!AZ'!/", tb->log_task );
    tb->asn_task[0] = tb->log_task;
    tb->asn_task[1] = (char *) 0;
   /*
     * Parse the first element (assumed to be node) out of the logical name
     */
    for ( node_len = name_len = 0; tb->log_task[node_len]; node_len++ ) {
	if ( tb->log_task[node_len] == ':' ) { 
	    if ( name_len == 0 ) name_len = node_len;
	    if ( tb->log_task[node_len+1] == ':' ) node_len++; 
            node_len++;
	    break; 
	}
	if ( tb->log_task[node_len] == '"' ) name_len = node_len;;
    }
    if ( node_len == 0 ) return 1;
    if ( name_override ) {
	/* Override name to use for translation */
        if ( http_log_level > 6 ) 
	    tlog_putlog ( 0, "Translation override: !AZ!/", name_override );
        log_name.dsc$w_length = tu_strlen ( name_override );
        log_name.dsc$a_pointer = name_override;
    } else {
        log_name.dsc$w_length = name_len;
        log_name.dsc$a_pointer = tb->log_task;
    }
    /*
     * Attempt to translate logical names.  Make item list for $TRNLNM.
     */
    flags = LNM$M_CASE_BLIND;
    length = 0;
    item[0].length = sizeof(int); item[0].code = LNM$_INDEX;
    item[0].buf = &index; item[0].retlen = (void *) 0;

    item[1].length = sizeof(string)-1; item[1].code = LNM$_STRING;
    item[1].buf = string; item[1].retlen = &length;

    item[2].length = sizeof(int); item[2].code = LNM$_MAX_INDEX;
    item[2].buf = (void *) &max_index; item[2].retlen = (void *) 0;

    item[3].length = item[3].code = 0;	/* terminate list */
    max_index = 0;
    /*
     * Call TRNLNM for every equivalence name, first call will get
     * number of translations (max_index).
     */
    for ( index = outndx = 0; index <= max_index; index++ ) {
        status = SYS$TRNLNM ( &flags, &table_name, &log_name, 0, &item );
	if ( (status&1) == 0 ) {
	    return 1;
	}
	if ( length > 0 ) {
	    /*
	     * Make asn_list entry by replacing node portion of log_name
	     * with the equivalence string.
	     */
	    if ( string[length-1] != '"' ) {
	        tu_strnzcpy 
		    ( &string[length], &tb->log_task[node_len], 255-length);
	        length = tu_strlen ( string );
	    }
	    tb->asn_task[outndx] = malloc ( length+1 );
	    tb->asn_task[outndx+1] = (char *) 0;
	    tu_strnzcpy ( tb->asn_task[outndx], string, length );
	    outndx++;
        }
	    
        item[2].length = item[2].code = 0;	/* new  list termination */
    }
    return 1;
}    
 /****************************************************************************/
/* Initialize decnet module, assume we are called from the initial thread
 * before any other threads are started.
 */
int dnetx_initialize ( char *default_taskname )
{
    int status, i, LIB$GET_EF(), pthread_keycreate(), SYS$TRNLNM();
    int flags, length, max_index;
    char *tmp;
    struct { short length, code; void *buf; void *retlen; } item[4];
    $DESCRIPTOR(table_name,"LNM$FILE_DEV");
    $DESCRIPTOR(log_name,"");
    /*
     * Initialize decthreads objects.
     */
    status = pthread_mutex_init ( &dnet_task, pthread_mutexattr_default );
    /*
     * Attempt to translate default_taskname (i.e. WWWEXEC) as a logical name.
     */
    flags = LNM$M_CASE_BLIND;
    max_index = length = 0;
    item[0].length = sizeof(dnet_default_task.log_task)-1; 
    item[0].code = LNM$_STRING;
    item[0].buf = dnet_default_task.log_task; item[0].retlen = &length;

    item[1].length = sizeof(int); item[1].code = LNM$_MAX_INDEX;
    item[1].buf = (void *) &max_index; item[1].retlen = (void *) 0;
    item[2].length = item[2].code = 0;	/* terminate list */

    log_name.dsc$w_length = tu_strlen ( default_taskname );
    log_name.dsc$a_pointer = default_taskname;

    status = SYS$TRNLNM ( &flags, &table_name, &log_name, 0, &item );
    if ( (status&1) == 1 ) {
	/*
	 * Leave translation as default string in block, terminate it.
	 */
	dnet_default_task.log_task[length] = '\0';
    } else {
	/*
	 * No translation, construct one of form: 0::"0='default_taskname'".
	 */
	tu_strncpy ( dnet_default_task.log_task, "0::\"0=", 6 );
	tu_strnzcpy ( &dnet_default_task.log_task[6], default_taskname,
		sizeof(dnet_default_task.log_task)-8 );
	tu_strcpy ( &dnet_default_task.log_task[tu_strlen(
		dnet_default_task.log_task)], "\"" );
    }
    dnet_default_task.log_task_len = tu_strlen (dnet_default_task.log_task);
    dnet_default_task.next = (struct task_block *) 0;
    dnet_default_task.rr_point = 0;
    /*
     * Expand either our translation or the default_taskname if it
     * has multiple value.
     */
    if ( max_index > 0 )
        status = expand_logical_task ( default_taskname, &dnet_default_task );
    else
	status = expand_logical_task ( (char *) 0, &dnet_default_task );
    /*
     * Determine length of node portion of default taskname including colons.
     */
    for ( dnet_defnode = i = 0; dnet_default_task.log_task[i]; i++ ) {
	if ( (dnet_default_task.log_task[i] == ':') && 
		(dnet_default_task.log_task[i+1] == ':') ) {
	    dnet_defnode = i+2;
	    break;
	}
    }
    /*
     * initialize task definition list to end with default task block;
     */
    dnet_task_defs_end = &dnet_default_task;
    return status;
}
/****************************************************************************/
/* Initialize DECnet task search list block and add to list.  We assume this
 * routine is called from the initial thread before any other threads
 * are created.
 *
 * Return value:	0	Malloc error on task block.
 *			1	Normal.
 */
int dnetx_define_task ( char *logical_task )
{
    int length, status, node_len, task_len, rr_point;
    char *asn_spec, **task_list, string[256];
    struct task_block *tb;
    /*
     * See if task already defined, use side effect to determine
     * length of node/object portion.
     */
    if ( dnetx_parse_task ( logical_task, &task_len, &task_list, &rr_point) ) {
	return 3;	/* already defined */
    }
    /*
     * Allocate task block block and add to list.
     */
    tb = malloc ( sizeof(struct task_block) );
    if ( !tb ) return 0;
    dnet_task_defs_end->next = tb;
    dnet_task_defs_end = tb;
    /*
     * Initialize all of task block except for asn_list and set up default
     * asn_list to be the logical_task string itself.
     */
    tb->next = (struct task_block *) 0;
    tb->rr_point = 0;
    tu_strnzcpy ( tb->log_task, logical_task, task_len );
    if ( (task_len == 0) || (logical_task[task_len-1] == ':') ) {
        tu_strnzcpy ( &tb->log_task[task_len],
		&dnet_default_task.log_task[dnet_defnode],
		sizeof(tb->log_task)-task_len-1 );
    }
    tb->log_task_len = tu_strlen ( tb->log_task );
    /*
     * Translate logical task to list of assigned tasks.
     */
    status = expand_logical_task ( (char *) 0, tb );
    return status;
}
/****************************************************************************/
/*
 * Parse of script directory specifition for node and object info
 * construct list task specifcations for use with dnet_connect.
 * of the parsed string is returned in.
 */
int dnetx_parse_task ( char *logical_task,	/* Script-dir string */
	int *prefix_len,			/* chars used from script-dir*/
	char ***task_list,			/* List of expanded task specs*/
	int *rr_point )				/* Preferred starting point */
{
    int i, node, status, length;
    char *bindir, *task_spec, ovr_spec[256];
    struct task_block *tb;
    /*
     * Construct initial DECnet task specification, using www_exec_task
     * as the default and overriding with script_dir prefix.
     */
    bindir = logical_task;
    *prefix_len = 0;
    length = 0;
    task_spec = dnet_default_task.log_task;

    for ( i = 0; bindir[i] && (i < 128); i++ ) {
	if ( (bindir[i] == ':') && (bindir[i+1] == ':') ) {
	    /*
	     * Script dir has node, see if object ("obj=") specified as well 
	     */
	    i = i + 2;			/* length of node + "::" */
	    task_spec = ovr_spec;
	    if ( bindir[i] == '"' ) {
		/* bindir has both, override. */
		for ( i = i + 1; bindir[i] && (i < 127); i++ )
		    if ( bindir[i] == '"' ) { i++; break; }
		tu_strnzcpy ( task_spec, bindir, i );
	    } else {
		/*
		 * Only override node portion of dnet_default_task, get
		 * object portion from default task string.
		 */
		tu_strncpy ( ovr_spec, bindir, i );
		tu_strnzcpy ( &task_spec[i], 
			&dnet_default_task.log_task[dnet_defnode],
				255 - i );
	    }
	    /*
	     * Return prefix length to caller and determine length of
	     * full task with defaults applied.
	     */
	    *prefix_len = i;
	    length = tu_strlen ( task_spec );
	    break;
	}
    }
    if ( http_log_level > 6 ) tlog_putlog ( 7, 
	"task specification after default: '!AZ' prefix_l=!SL, bindir: '!AZ'!/", 
		task_spec, *prefix_len, bindir);
    /*
     * Now we have task_spec, search list of defined tasks for matching
     * specification and return it's expanded list to caller.
     */
    for ( tb = &dnet_default_task; tb; tb = tb->next ) {
	if ( http_log_level > 7 ) tlog_putlog (8,
		"Testing against '!AF'!/", tb->log_task_len, tb->log_task );

	if ( (task_spec == dnet_default_task.log_task) ||
		((length == tb->log_task_len) &&
		(0 == tu_strncmp ( tb->log_task, task_spec, length ))) ) {
	    /*
	     * We found block, update Round Robin point and return.
	     * Only dick with rr_point if more than one in list.
	     */
	    if ( tb->asn_task[1] ) {
    		status = pthread_mutex_lock ( &dnet_task );
	        *rr_point = tb->rr_point + 1;
	        if ( !tb->asn_task[*rr_point] ) *rr_point = 0;
	        tb->rr_point = *rr_point;
		status = pthread_mutex_unlock ( &dnet_task );
	    } else {
		*rr_point = 0;
	    }
	    *task_list = tb->asn_task;
	    return 1;
	}
    }
    /*
     * Return error value.
     */
    return 0;
}
