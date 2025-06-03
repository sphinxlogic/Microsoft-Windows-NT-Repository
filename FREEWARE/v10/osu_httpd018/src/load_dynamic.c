/*
 * Load routine from shareable image.
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
 *	fspec		Directory name, defaults to WWW_ROOT:[000000].
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
 */
#include "pthread_np.h"
#include <stdlib.h>
#include <stdio.h>
#include <descrip.h>
#include "access.h"
#include "ident_map.h"
#include "tutil.h"
#include "file_access.h"

int http_log_level, tlog_putlog();
map_callout http_usermap_failure;	/* forward reference */
static struct callback_vector *create_callback_vector();
/*****************************************************************************/
int http_load_dynamic_rule ( info_ptr blk,	/* rule definition block */
	char *name,			/* Routine name */
	char *image,			/* Shareable image */
	char *fspec )			/* Directory specification */
{
    int status, LIB$FIND_IMAGE_SYMBOL();
    struct dsc$descriptor name_dx, image_dx, fspec_dx;
    char image_default[256], init_name[40];
    /*
     * Build VMS descriptos for LIB$FIND_IMAGE_SYMBOL.
     */
    name_dx.dsc$b_dtype = DSC$K_DTYPE_T;		/* text data */
    name_dx.dsc$b_class = DSC$K_CLASS_S;		/* fixed (Static) */
    name_dx.dsc$w_length = tu_strlen ( name );
    name_dx.dsc$a_pointer = name;

    image_dx.dsc$b_dtype = DSC$K_DTYPE_T;		/* text data */
    image_dx.dsc$b_class = DSC$K_CLASS_S;		/* fixed (Static) */
    if ( image ) {
        image_dx.dsc$a_pointer = image;
    } else {
	image_dx.dsc$a_pointer = image_default;
	tu_strcpy ( image_default, "WWW_RULE_" );
	tu_strnzcpy ( &image_default[9], name, sizeof(image_default)-10 );
    }
    image_dx.dsc$w_length = tu_strlen ( image_dx.dsc$a_pointer );

    fspec_dx.dsc$b_dtype = DSC$K_DTYPE_T;		/* text data */
    fspec_dx.dsc$b_class = DSC$K_CLASS_S;		/* fixed (Static) */
    fspec_dx.dsc$a_pointer = fspec ? fspec : "WWW_ROOT:[000000]";
    fspec_dx.dsc$w_length = tu_strlen ( fspec_dx.dsc$a_pointer );

    status = LIB$FIND_IMAGE_SYMBOL 
	( &name_dx, &image_dx, &blk->usermap, &fspec_dx );

    if ( 1 == (status&1) ) {
	/*
	 * Look for init routine.
	 */
	int (*init_routine)();
	name_dx.dsc$a_pointer = init_name;
	tu_strcpy ( init_name, "INIT_" );
	tu_strnzcpy ( &init_name[5], name, sizeof(init_name)-6 );
	name_dx.dsc$w_length = tu_strlen ( init_name );
	status = LIB$FIND_IMAGE_SYMBOL
		( &name_dx, &image_dx, &init_routine, &fspec_dx );
	if ( 1 == (status&1) ) {
	    status = init_routine ( create_callback_vector(),
		sizeof(struct callback_vector) );
	}
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
