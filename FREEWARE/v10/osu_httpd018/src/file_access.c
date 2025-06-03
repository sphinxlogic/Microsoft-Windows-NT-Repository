/*
 * This module handles C file access in a thread-safe fashion.
 * The special open mode "d" is added for reading directories via
 * LIB$FIND_FILE.
 *
 * typedef void *fhandle;
 * int tf_initialize ( char *prefix )
 * fhandle tf_open ( char *ident, char *mode, errmsg[256] );
 * int tf_last_error ( fhandle f, int *last_errno, int *last_vms_cond, char *s );
 * int tf_close ( fhandle f );
 * int tf_read ( fhandle f, char *buffer, size_t length );
 * int tf_getline ( void *fptr, char *buffer, int bufsize, int max_lines )
 * 
 * Revised: 22-JUL-1994		Fixed locking bug in tf_rundown.
 * Revised:  5-AUG-1994		Added CONDITIONAL_YIELD macro (doesn't
 *				yield if RTL is reentrant).
 * Revised: 14-AUG-1994		Modified tf_header_info to include owner.
 * Revised:  7-DEC-1994		Added tf_format_time().
 * Revised: 17-DEC-1994		Added tf_decode_time().
 * Revised: 19-JAN-1995		Let NUL terminate lines in tf_getline()
 * Revised: 28-FEB-1995		Fixed bug in malloc call in tf_initialize().
 */
#include "pthread_np.h"
#include <stdio.h>
#include <stdlib.h>
#include <stat.h>
#include <unixlib.h>
#include <errno.h>
#define strerror strerror_1
#include <string.h>
#undef strerror
char *strerror ( int errnum, ... );	/* work around bugs in early AXP includes */
#include "file_access.h"	/* validate prototypes against actual */
#include "tutil.h"
#include <descrip.h>
#include <fscndef.h>
#include <rmsdef.h>
/*
 * Global(module-wide) variables
 */
struct file_context {
    struct file_context *flink, *blink;		/* Open file list. */
    FILE *fptr;					/* File pointer */
    int cerrno;					/* Status of last operation */
    int vmserrno;				/* VMS condition code if error*/
    int read_mode;				/* true for ru access */
    int write_mode;				/* true for wu access */
    int binary_mode;				/* true for b access */
    int lf_pending;				/* used for cr/lf expansion */
    int dir_mode;				/* Context for find-file */
    char fspec[256];				/* Filename for dir searches */
};
typedef struct file_context file_ctx_struct, *file_ctx;

int http_log_level, tlog_putlog(int level, char *ctlstr, ... );
int LIB$FIND_FILE(), LIB$FIND_FILE_END(), SYS$FILESCAN();
static file_ctx free_file_ctx;			/* cache of free contexts */
static pthread_mutex_t ctx_list;		/* Guards free list */
static pthread_key_t file_key;			/* Private data for client */
static char *global_prefix;
static $DESCRIPTOR(ff_defdir,"[000000]");
static int tf_read_dir ( int *ff_ctx,		/* Forward reference */
	char *fspec, char *buffer, int bufsize,	int *count );
#define CONDITIONAL_YIELD if ( !http_reentrant_c_rtl ) pthread_yield();
/************************************************************************/
/* Define context key rundown routine that closes dangling files.
 * Return value 0 for success.
 */
static void tf_rundown ( file_ctx ctx )
{
    file_ctx cur;
    if ( http_log_level > 1 ) tlog_putlog ( 0,
	"Fallback cleanup of file_access context block, adr=!UL!/", ctx );
    if ( !ctx ) return;			/* null context value */
    /*
     * Close open files.
     */
    for ( cur = ctx; cur; cur = cur->flink ) {
	LOCK_VMS_RTL
	if ( cur->fptr ) { fclose ( cur->fptr ); cur->fptr = (FILE *) NULL; }
	if ( cur->dir_mode && cur->lf_pending ) 
	    { LIB$FIND_FILE_END(&cur->lf_pending); cur->dir_mode = 0; }
	UNLOCK_VMS_RTL
    }
    /*
     * Place blocks onto free-list.
     */
    for ( cur=ctx; cur->flink; cur = cur->flink );
    pthread_mutex_lock ( &ctx_list );
    cur->flink = free_file_ctx;
    free_file_ctx = ctx;
    pthread_mutex_unlock ( &ctx_list );
}
/************************************************************************/
/*
 * Initialize module-wide variables.  Return value 0 if success, -1 if failure.
 */
int tf_initialize ( char *prefix )
{
    int status;
    void tf_rundown();
    /*
     * Make copy of prefix string.
     */
    if ( prefix ) {
	global_prefix = malloc ( strlen(prefix)+1 );
	strcpy ( global_prefix, prefix );
    } else global_prefix = "";
    /*
     * Make context key for running down open files.
     */
    free_file_ctx = (file_ctx) NULL;
    status = pthread_mutex_init ( &ctx_list, pthread_mutexattr_default );
    status = pthread_keycreate ( &file_key, tf_rundown );
    if ( status == -1 ) perror ( "Error creating file access context key" );
    return status;
}
/***********************************************************************/
/*
 * Open file.  If open failure, error code is formatted.  errmsg may be null.
 * Return value is opaque pointer if success, NULL if error.
 */
void *tf_open ( char *ident, char *mode, char errmsg[256] )
{
    file_ctx cur, new;
    char *mptr;
    /*
     * Allocate context block.
     */
    if ( pthread_getspecific ( file_key, (void *) &cur ) < 0 ) 
		return (void *) NULL;
    pthread_mutex_lock ( &ctx_list );
    new = free_file_ctx;
    if ( new ) free_file_ctx = new->flink;
    pthread_mutex_unlock ( &ctx_list );
    if ( !new ) {
	/*
	 * Free list is empty, allocate a new block.
	 */
	LOCK_C_RTL
	new = (file_ctx) malloc ( sizeof(file_ctx_struct) );
	UNLOCK_C_RTL
	if ( !new ) return (void *) NULL;
    }
    /*
     * insert into per-thread file list;
     */
    new->blink = (file_ctx) NULL;
    new->flink = cur;
    if ( cur ) cur->blink = new;
    pthread_setspecific ( file_key, new );
    /*
     * Zero fields in block, scan mode argument and set flags.
     */
    new->read_mode = new->write_mode = new->binary_mode = new->dir_mode = 0;
    new->lf_pending = 0;
    for ( mptr = mode; *mptr; mptr++ ) switch ( *mptr ) {
	case 'd':
	    new->dir_mode = 1;
	    break;
	case 'r':
	    new->read_mode = 1;
	    break;
	case 'w':
	    new->write_mode = 1;
	    break;
	case 'u':
	    new->read_mode = new->write_mode = 1;
	    break;
	case 'b':
	    new->binary_mode = 1;
	default:
	    break;
    }
    /*
     * Attempt to open file.
     */
    if ( !new->dir_mode ) {
	/*
	 * RTL is not thread-safe, only have 1 file operation active at a time.
	 */
	CONDITIONAL_YIELD
        LOCK_C_RTL
        new->fptr = fopen ( ident, mode, "mbc=64", "dna=[000000]" );
        new->cerrno = errno;
        new->vmserrno = vaxc$errno;
        UNLOCK_C_RTL

        if ( !new->fptr ) {
	    /*
	     * Open failure, format error message and cleanup.  Call close to
	     * deallocate block.
	     */
	    if ( errmsg ) {
	        char *errstr; /* , *strerror(); */
	        LOCK_C_RTL
	        errstr = strerror ( new->cerrno, new->vmserrno );
	        tu_strnzcpy ( errmsg, errstr, 255 );
	        UNLOCK_C_RTL
	    }
	    tf_close ( new );
	    new = (file_ctx) NULL;
        } else if ( errmsg ) errmsg[0] = '\0';  /* success */

    } else {
	/*
	 * List directory.
	 */
	int i, j, scnt;		/* src, dst index and slash count */
	new->cerrno = 0;
	new->vmserrno = 1;
	new->fptr = (FILE *) NULL;
	/*
	 * Convert unix syntax for VMS spec for find_file.
	 */
	for ( scnt = j = i = 0; j < sizeof(new->fspec)-14; i++ ) {
	    if ( ident[i] == '\0' ) break;
	    if ( ident[i] == '/' ) {
		if ( scnt == 0 ) {
		} else if ( scnt == 1 ) {
		    /* Second slash is replaced by :[ */
		    new->fspec[j++] = ':';
		    new->fspec[j++] = '[';
		    if ( ident[i+1] == '\0' ) { --j;
			/*  No directory, assume [000000] */
			/* tu_strcpy ( &new->fspec[j], "000000]" );
			j += 7; */
		    }

		} else if ( ident[i+1] ) {
		    new->fspec[j++] = '.';
		} else {
		    new->fspec[j++] = ']';
		}
		scnt++;
	    } else new->fspec[j++] = ident[i];
	}
	tu_strcpy ( &new->fspec[j], "*.*;" );
    }


    return (void *) new;
}
/***************************************************************************/
/* Close file opened by tf_open.  Return status is 0 for success, EOF for
 * failure.
 */
int tf_close ( void *fptr )
{
    file_ctx ptr, ctx, cur;
    int status;

    ptr = (file_ctx) fptr;		/* cast user's argument */
    /*
     * Find ptr block in threads context list.
     */
    pthread_getspecific ( file_key, (void *) &ctx );
    for ( cur=ctx; cur && (cur != ptr); cur = cur->flink );
    if ( !cur ) return -1;	/* not found */
    /*
     * Close file or rundown filescan.
     */
    CONDITIONAL_YIELD
    LOCK_VMS_RTL
    status =  ( cur->fptr ) ? fclose ( cur->fptr ) : 0;
    cur->fptr = (FILE *) NULL;
    if ( cur->dir_mode && cur->lf_pending ) 
	    { LIB$FIND_FILE_END(&cur->lf_pending); cur->dir_mode = 0; }
    UNLOCK_VMS_RTL
    /*
     * remove block from list.
     */
    if ( cur == ctx ) {
	/*
	 * List head is updated.
	 */
	pthread_setspecific ( file_key, cur->flink );
    }
    if ( cur->flink != (file_ctx) NULL ) cur->flink->blink = cur->blink;
    if ( cur->blink != (file_ctx) NULL ) cur->blink->flink = cur->flink;
    /*
     * Place context block on free list.
     */
    pthread_mutex_lock ( &ctx_list );
    cur->flink = free_file_ctx;
    free_file_ctx = cur;
    pthread_mutex_unlock ( &ctx_list );

    return status;
}
/***************************************************************************/
/*  Read from file into caller's buffer, returning number of characters read
 * or -1 if error.
 */
int tf_read ( void *fptr, char *buffer, int bufsize )
{
    int status;
    file_ctx ctx;

    ctx = (file_ctx) fptr;
    if ( !ctx->fptr ) {
	/*
	 * File is either no opened or openned in 'dir' mode.
	 */
	if ( ctx->dir_mode ) {
	    /* Read directory entries into buffer */
	    int count;
	    ctx->vmserrno = tf_read_dir 
		( &ctx->lf_pending, ctx->fspec, buffer, bufsize, &count );
	    if ( (ctx->vmserrno&1) == 0 ) {
		ctx->cerrno = EVMSERR;
		return -1;
	    } else {
		ctx->cerrno = 0;
	        return count;
	    }
	}
	return -1;
    }
    /*
     * Synchronize with global lock to do stdio call.  If I/O generates
     * error, save error codes in thread-private data structure before
     * releasing lock.
     */
    CONDITIONAL_YIELD
    LOCK_C_RTL
    status = fread ( buffer, sizeof(char), bufsize, ctx->fptr );
    if ( status < 0 ) { ctx->cerrno = errno; ctx->vmserrno = vaxc$errno; }
    else { ctx->cerrno = 0; ctx->vmserrno = 1; }
    UNLOCK_C_RTL
    return status;

}
/***************************************************************************/
/*  Read from file into caller's buffer, returning a CRLF terminated lines
 *  for the read records.  Return value is number of characters moved into
 *  buffer (0 on EOF, -1 on error).  To read a single line, specify 1 as
 * the max_lines parameter.
 */
int tf_getline ( void *fptr, char *buffer, int bufsize, int max_lines )
{
    int status, line_begin, used;
    file_ctx ctx;
    char *line;

    ctx = (file_ctx) fptr;
    if ( !ctx->fptr ) return -1;
    if ( ctx->binary_mode ) {
	ctx->cerrno = EINVAL;
	ctx->vmserrno = 20;
	return -1;
    }
    /*
     * Tack on linefeed if it didn't fit in last buffer.
     */
    used = 0;
    if ( ctx->lf_pending ) {
	buffer[used++] = '\n';
	ctx->lf_pending = 0;
    }
    /*
     * Synchronize with global lock to to stdio call.  If I/O generates
     * error, save error codes in thread-private data structure before
     * releasing lock.
     */
    CONDITIONAL_YIELD
    LOCK_C_RTL
    line_begin = used;
    for ( ; (max_lines > 0) && (used<bufsize-1); --max_lines ) {
        line = fgets ( &buffer[used], bufsize-used, ctx->fptr );
        if ( ! line ) { 
	    ctx->cerrno = errno; ctx->vmserrno = vaxc$errno; 
	    /*
	     * Last line may have ended in zero, not \n.  Get correct EOL.
	     */
	    while ( line_begin < used ) {
		if ( !buffer[line_begin] ) used = line_begin;
		line_begin++;
	    }
	    break;
	} else { 
	    /*
	     * Find line feed and insert carriage return.
	     */
	    char *p;
	    ctx->cerrno = 0; ctx->vmserrno = 1;
	    line_begin = used;			/* Last line loaded */
	    for ( ; used < bufsize; used++ ) if ( buffer[used] == '\n' ) {
		/*
		 * We found line-feed, convert it to carriage return and
		 * append line-feed if room or flag it pending for next getline.
		 */
		buffer[used++] = '\r';
		if ( used < bufsize ) buffer[used++] = '\n';
		else ctx->lf_pending = 1;
		used++;		/* psuedo null */
		break;
	     }
	     --used;		/* trim trailing null */
	}
    }
    UNLOCK_C_RTL
    return used;

}
/***************************************************************************/
/* Retrieve error codes saved by tf_routine operation.  Return value is
 * last error number.
 */
int tf_last_error ( void *fptr, int *last_errno, int *last_vms_cond )
{
    file_ctx ctx;

    ctx = (file_ctx) fptr;
    if ( ctx->fptr ) {
        *last_errno = ctx->cerrno;
        *last_vms_cond = ctx->vmserrno;
    } else {
	/*  No open file for this context block */
	*last_errno = EINVAL;
	*last_vms_cond = 20;
    }
    return *last_errno;
}
/***************************************************************************/
/* Read directory contents into buffer.  Return filenames, separating each
 * by a linefeed.  Return value is VMS status code (odd for success).
 */
static int tf_read_dir ( int *ff_ctx,		/* Find_file context */
	char *fspec,
	char *buffer,
	int bufsize,
	int *count )				/* Chars returned, 0 on EOF */
{
    int status, length, used;
    char *p, result[256];
    struct { short length, code; char *component; } item[3];
    struct dsc$descriptor fname, ff_result;
    if ( *fspec ) {
	/*
	 * Retrieve next set of files in directory.
	 */
	fname.dsc$b_dtype = DSC$K_DTYPE_T;
	fname.dsc$b_class = DSC$K_CLASS_S;
	fname.dsc$w_length = tu_strlen ( fspec );
	fname.dsc$a_pointer = fspec;

	item[0].code = FSCN$_NAME;
	item[1].code = FSCN$_TYPE;
	item[2].code = item[2].length = 0;	/* terminate list */
	/*
	 * Assume find_file is not thread-reentrant.  Read as many names
	 * as will fit in user's buffer, separating each with an ASCII NUL.
	 */
	CONDITIONAL_YIELD
	LOCK_VMS_RTL
	ff_result.dsc$b_dtype = DSC$K_DTYPE_T;
	ff_result.dsc$b_class = DSC$K_CLASS_S;
	ff_result.dsc$w_length = 256;
	ff_result.dsc$a_pointer = result;	/* per-thread copy */
	status = 1;
	for ( used = 0; (used + 80) <= bufsize; buffer[used++] = '\0' ) {
	    /*
	     * Get next file and parse out name+type.
	     */
	    status = LIB$FIND_FILE ( &fname, &ff_result, ff_ctx, &ff_defdir );
	    if ( (status &1) == 1 ) 
	        status = SYS$FILESCAN ( &ff_result, &item, 0 );
	    /*
	     * Check for no more files.  Convert NoMoreFiles to success.
	     */
	    if ( (status&1) == 0 ) {
	        *fspec = '\0';			/* mark EOF */
		if ( status == RMS$_NMF ) status = 1;
		break;
	    };
	    /*
	     * Extract filename, truncating to fit in buffer.
	     */
	    length = item[0].length + item[1].length;
	    tu_strncpy ( &buffer[used], item[0].component, length );
	    used += length;
	}
	UNLOCK_VMS_RTL
        *count = used;
	return status;

    } else {
	/*
	 * A null fspec indicates all data returned, report EOF.
	 */
	*count = 0;
	return 1;
    }
}
/***************************************************************************/
/* Perform fstat on indicate file
 */
int tf_header_info ( void *fptr, int *size, unsigned *uic,
	unsigned int *cdate, unsigned int *mdate )
{
    int status;
    file_ctx ctx;
    stat_t statblk;

    ctx = (file_ctx) fptr;
    if ( !ctx->fptr ) {
	/*
	 * File is either no opened or openned in 'dir' mode.
	 */
	if ( ctx->dir_mode ) {
	    /* Read directory entries into buffer */
	    int count;
	    return -1;
	}
	return -1;
    }
    /*
     * Synchronize with global lock to do stdio call.  If I/O generates
     * error, save error codes in thread-private data structure before
     * releasing lock.
     */
    CONDITIONAL_YIELD
    LOCK_C_RTL
    status = fstat ( fileno (ctx->fptr), &statblk );
    if ( status < 0 ) { ctx->cerrno = errno; ctx->vmserrno = vaxc$errno; }
    else { ctx->cerrno = 0; ctx->vmserrno = 1; }
    UNLOCK_C_RTL

    if ( status >= 0 ) {
	/*
	 * Stat succeeded, return data.
	 */
	*uic = statblk.st_uid;
	*size = statblk.st_size;
	*cdate = statblk.st_ctime;
	*mdate = statblk.st_mtime;
    } else {
	*size = *cdate = *mdate = 0;
    }
    return status;

}
/****************************************************************************/
/* Convert unix binary time to RFC 1123 time format:
 *  	Wdy, DD Mon YY HH:MM:SS TIMEZONE
 *
 * If DATE_FORMAT_850 defined, use older format.
 *  	Weekday, DD-Mon-YY HH:MM:SS TIMEZONE
 *
 */
extern int lib$emul(), lib$ediv(), lib$addx(), lib$subx(), sys$bintim();
static unsigned long base_time[2], zero_time[2];
static pthread_once_t base_time_setup = pthread_once_init;

static void init_base_time() {
    int status, tdiff, conv, offset;
    unsigned long diff_time[2];
    $DESCRIPTOR ( unix_zero, "1-JAN-1970 00:00:00.0" );
    char *tzone;
    /*
     * Convert unix base time to VMS format.
     */
    status = sys$bintim ( &unix_zero, &zero_time );
    /*
     * Get offset from GMT. and adjust base time.
     */
    pthread_lock_global_np();
    tzone = getenv ( "SYS$TIMEZONE_DIFFERENTIAL" );
    if ( tzone ) {
	tdiff = atoi ( tzone );
	conv = -10000000;
	offset = 0;
	lib$emul ( &tdiff, &conv, &offset, diff_time );
	lib$addx ( diff_time, zero_time, base_time );
    } else {
	base_time[0] = zero_time[0];
	base_time[1] = zero_time[1];
    }
    pthread_unlock_global_np();
}
char *tf_format_time ( unsigned long bintim, char *buffer )
{
#ifdef DATE_FORMAT_850
    static char *weekday[] = {"Thursday", "Friday", "Saturday", "Sunday", 
		"Monday", "Tuesday", "Wednesday"};
#define MONTH_DELIMITER '-'
#else
    static char *weekday[] = {"Thu", "Fri", "Sat", "Sun", "Mon", "Tue", "Wed"};
#define MONTH_DELIMITER ' '
#endif
    static char *month[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
		"Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
    unsigned long offset, conv, quad_time[2], final_time[2];
    int status, i, wdy, mon, sys$numtim();
    unsigned short num_time[7];
    /*
     * Initialize constants used in conversion.
     */
    pthread_once ( &base_time_setup, init_base_time );
    /*
     * Convert Unix time (seconds since 1970) to VMS binary time, base
     * time is adjust to result time in UTC (GMT) timezone.
     */
    offset = 0;
    conv = 10000000;		/* 100 nanosecond ticks to seconds */
    lib$emul ( &bintim, &conv, &offset, quad_time );
    lib$addx ( quad_time, base_time, final_time );
    status = sys$numtim ( num_time, final_time );
    /*
     * Get day of week.
     */
    wdy = bintim / 86400;	/* day-number since 1970 */
    wdy = wdy % 7;		/* Day of week */
    for ( i = 0; weekday[wdy][i]; i++ ) buffer[i] = weekday[wdy][i];
    buffer[i++] = ','; buffer[i++] = ' ';
    /*
     * Add date;
     */
    buffer[i] = (char) (48+(num_time[2]/10)); /* Day of month */
    i++;					/* always include leading 0 */
    buffer[i++] = (char) (48+(num_time[2]%10));
    buffer[i++] = MONTH_DELIMITER;

    mon = num_time[1] - 1;		/* Month of year */
    buffer[i++] = month[mon][0]; buffer[i++] = month[mon][1];
    buffer[i++] = month[mon][2]; buffer[i++] = MONTH_DELIMITER;

#ifndef DATE_FORMAT_850
    buffer[i++] = (char) (48+(num_time[0]/1000));  /* high order digit */
    num_time[0] = num_time[0]%1000;		   /* Get rid of millenia */
    buffer[i++] = (char) (48+(num_time[0]/100));   /* century digit */
#endif
    num_time[0] = num_time[0]%100;		/* Get rid of centuries */
    buffer[i++] = (char) (48+(num_time[0]/10));   /* Show last 2 digits of year*/
    buffer[i++] = (char) (48+(num_time[0]%10));
    buffer[i++] = ' ';
    /*
     * add time.
     */
    buffer[i++] = (char) (48+(num_time[3]/10));	/* hours */
    buffer[i++] = (char) (48+(num_time[3]%10));
    buffer[i++] = ':';

    buffer[i++] = (char) (48+(num_time[4]/10)); /* minutes */
    buffer[i++] = (char) (48+(num_time[4]%10));
    buffer[i++] = ':';

    buffer[i++] = (char) (48+(num_time[5]/10)); /* seconds */
    buffer[i++] = (char) (48+(num_time[5]%10));
    buffer[i++] = ' '; 
    /*
     * add timezone.
     */
    buffer[i++] = 'G'; buffer[i++] = 'M'; buffer[i++] = 'T';
    buffer[i++] = '\0';

    return buffer;
}
/****************************************************************************/
/* Convert text time into binary time.  Input format may be 1 of following:
 *    sun,06nov199408:49:37GMT		; RFC 822 (1123)
 *    sunday,6-nov-9408:49:37GMT	; RFC 850 (1036)
 *    sunnov608:49:371994		; asctime() function.
 *
 * Return value is zero on format error.
 */
unsigned long tf_decode_time ( char *string )
{
    int i, comma, hyphen1, hyphen2, colon1, colon2, status;
    struct { long l; char *s; } dx;
    unsigned long vms_time[2], offset[2], seconds, remainder, conv;
    char asctim[32];
    /*
     * Scan string for landmarks.
     */
    comma = hyphen1 = hyphen2 = colon1 = colon2 = -1;
    for ( i = 0; string[i]; i++ ) {
	char c;
	c = string[i];
	if ( c == ',' ) comma = i;
	else if ( c == '-' ) {
	    if ( hyphen1 < 0 ) hyphen1 = i; else hyphen2 = i;
	} else if ( c == ':' ) {
	    if ( colon1 < 0 ) colon1 = i; else colon2 = i;
	}
    }
    /*
     * Do some sanity checks on landmarks.
     */
#ifdef DEBUG
    tlog_putlog(1,"h1: !SL, h2: !SL, c1: !SL, c2: !SL, comma: !SL!/", hyphen1,
    	hyphen2, colon1, colon2, comma );
#endif
    if ( (hyphen1 > hyphen2) || (colon1 > colon2) || (hyphen2 > colon1) ||
	(comma > colon1) || (colon1+3 != colon2) || (colon2+5 >= i) ) return 0;
    /*
     * Rearrange components into VMS absolute time string:
     *      "DD-MMM-YYYY HH:MM:SS"
     *       012345678901234567890
     */
    tu_strcpy ( asctim, "01-JAN-1970 00:00:00.00" );
    asctim[19] = string[colon2+2];
    asctim[18] = string[colon2+1];
    asctim[16] = string[colon2-1];
    asctim[15] = string[colon2-2];
    asctim[13] = string[colon1-1];
    asctim[12] = string[colon1-2];		/* Finished HH::MM::SS */

    if ( comma >= 0 ) {
	asctim[10] = string[colon1-3];
	asctim[9] = string[colon1-4];
	if ( hyphen2 != colon1-5 ) {
	    /*
	     * No hyphens in time string, synthesize them.
	     */
	    asctim[8] = string[colon1-5];
	    asctim[7] = string[colon1-6];	/* 4 digit year */
	    hyphen2 = colon1 - 6;
	    hyphen1 = colon1 - 9;
	}
	asctim[5] = string[hyphen2-1];
	asctim[4] = string[hyphen2-2];
	asctim[3] = string[hyphen2-3];
	asctim[1] = string[hyphen1-1];
	asctim[0] = string[hyphen1-2];

    } else {	/* ctime() */
	asctim[1] = string[colon1-3];
	if ( (string[colon1-4] >= '0') && (string[colon1-4] <= '9') ) {
	    if ( colon1 < 7 ) return 0;
	    asctim[0] = string[colon1-4];
	    asctim[3] = string[colon1-7];
	    asctim[4] = string[colon1-6];
	    asctim[5] = string[colon1-5];
	} else {
	    if ( colon1 < 6 ) return 0;
	    asctim[3] = string[colon1-6];
	    asctim[4] = string[colon1-5];
	    asctim[5] = string[colon1-4];
	}
	asctim[10] = string[colon2+6];
	asctim[9] = string[colon2+5];
	asctim[8] = string[colon2+4];
	asctim[7] = string[colon2+3];
    }
    /*
     * Convert string to binary time.  Month must be in upper case.
     */
    dx.l = 23;
    dx.s = asctim;
    tu_strupcase ( asctim, asctim );
    status = sys$bintim ( &dx, vms_time );
    if ( (status&1) == 0 ) return 0;		/* Invalid time */
    /*
     * Convert to delta seconds since unix base time.
     */
    pthread_once ( &base_time_setup, init_base_time );
    lib$subx ( vms_time, base_time, offset );
    conv = 10000000;		/* 100 nanosecond ticks to seconds */
    lib$ediv ( &conv, &offset, &seconds, &remainder );
    return seconds;
}
