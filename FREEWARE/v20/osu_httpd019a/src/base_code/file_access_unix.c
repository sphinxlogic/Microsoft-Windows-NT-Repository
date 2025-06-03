/*
 * This module handles C file access in a thread-safe fashion.
 * The special open mode "d" is added for reading directories via
 * readdir().
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
 * Revised: 11-MAY-1995		Fix bug on OSF version of tf_format_time.
 * Revised: 29-MAY-1995		Support "rd" mode in tf_open.
 * Revised: 30-MAY-1995		Support time formatting/decoding.
 */
#include "pthread_np.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include "file_access.h"	/* validate prototypes against actual */
#include "tutil.h"
#define vaxc$errno errno
#define EVMSERR EPIPE
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
    DIR *dptr;					/* Directory pointer */
    int size;
    uid_t uid;
    time_t ctime, mtime;
    char fspec[256];				/* Filename for dir searches */
};
typedef struct file_context file_ctx_struct, *file_ctx;

#ifdef DYNAMIC_MST
#include "mst_share.h"
#else
int http_log_level, tlog_putlog(int level, char *ctlstr, ... );
#endif
#define LIB$FIND_FILE_END(a) 1
#define LIB$FIND_FILE(a,b,c,d) 20
#define SYS$FILESCAN(a,c,b) 1
static file_ctx free_file_ctx;			/* cache of free contexts */
static pthread_mutex_t ctx_list;		/* Guards free list */
static pthread_key_t file_key;			/* Private data for client */
static char *global_prefix;
static int tf_read_dir ( 			/* forward refernce */
	DIR **dptr,				/* opendir pointer */
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
	if ( cur->fptr ) { fclose ( cur->fptr ); cur->fptr = (FILE *) NULL; }
	if ( cur->dir_mode && cur->dptr ) {
	    closedir ( cur->dptr );
	    cur->dir_mode = 0; }
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
	int size;
	size = strlen(prefix)+1;
	global_prefix = malloc ( size );
	if ( global_prefix ) strcpy ( global_prefix, prefix );
	else { global_prefix = ""; 
		printf("Error mallocing prefix, size: %d\n", size ); }
    } else global_prefix = "";
    /*
     * Make context key for running down open files.
     */
    free_file_ctx = (file_ctx) NULL;
    status = pthread_mutex_init ( &ctx_list, pthread_mutexattr_default );
    status = pthread_keycreate ( &file_key, 
		(pthread_destructor_t) tf_rundown );
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
    int status, flags; struct stat statblk;
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
	if ( !new ) {
	    tu_strcpy ( errmsg, "Malloc failed for new context block" );
	    return (void *) NULL;
	}
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
    new->dptr = (DIR *) 0;
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
    if ( !new->dir_mode || new->read_mode ) {
	/*
	 * RTL is not thread-safe, only have 1 file operation active at a time.
	 */
	CONDITIONAL_YIELD
        LOCK_C_RTL
        new->fptr = fopen ( ident, new->dir_mode ? "r" : mode );
        new->cerrno = errno;
        new->vmserrno = vaxc$errno;
	if ( new->fptr ) {

	    status = fstat ( fileno(new->fptr), &statblk );
	    if ( status == 0 ) {
		/* Copy stats, don't permit directories to be open */
		new->size = statblk.st_size;
		new->uid = statblk.st_uid;
		new->ctime = statblk.st_ctime;
		new->mtime = statblk.st_mtime;
		if ( S_ISDIR(statblk.st_mode) ) status = 1;
	    }
	    if ( status ) {
		new->cerrno = EISDIR;
		fclose ( new->fptr );
		new->fptr = (FILE *) 0;
	    }
	}
        UNLOCK_C_RTL
	if ( !new->fptr && new->dir_mode && (new->cerrno == EISDIR) ) {
	    /*
	     * Open failure due to named file being directory, fixup.
	     */
	    new->cerrno = 0;
	    new->vmserrno = 1;
	    new->fptr = (FILE *) NULL;
	    tu_strnzcpy ( new->fspec, ident, sizeof(new->fspec)-2 );
	    tu_strcpy ( &new->fspec[tu_strlen(new->fspec)], "/" );
	    if ( errmsg ) errmsg[0] = '\0';

        } else if ( !new->fptr ) {
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
	 * copy directory name to fspec buffer.
	 */
	tu_strnzcpy ( new->fspec, ident, sizeof(new->fspec)-1 );
	i = tu_strlen ( new->fspec );
	if ( i > 0 ) new->fspec[i-1] = '\0';
	status = stat ( ident, &statblk );
	if ( status == 0 ) {
	    if ( !S_ISDIR(statblk.st_mode) ) status = 1;
	}
	if ( status != 0 ) {
	    tf_close ( new );
	    new = (file_ctx) NULL;
	}
	else if ( i > 0 ) new->fspec[i-1] = '/';
    }


    return (void *) new;
}
/***************************************************************************/
/* Return boolean value indicating whether fptr is openned in 'd' mode.
 */
int tf_isdir ( void *fptr )
{
    int status;
    file_ctx ctx;
    struct stat statblk;
    ctx = (file_ctx) fptr;
    if ( !ctx ) return 0;		/* invalid ctx */
    if ( ctx->fptr ) return 0;
    return ctx->dir_mode;
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
    LOCK_C_RTL
    status =  ( cur->fptr ) ? fclose ( cur->fptr ) : 0;
    cur->fptr = (FILE *) NULL;
    if ( cur->dir_mode && cur->dptr ) {
	closedir ( cur->dptr );
        cur->dir_mode = 0; }
    UNLOCK_C_RTL
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
		( &ctx->dptr, ctx->fspec, buffer, bufsize, &count );
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
 * by a null.  Return value is VMS status code (odd for success).
 */
static int tf_read_dir ( 
	DIR **dptr,				/* opendir pointer */
	char *fspec,
	char *buffer,
	int bufsize,
	int *count )				/* Chars returned, 0 on EOF */
{
    int status, length, used;
    struct dirent result;
    if  ( *fspec ) {
	/*
	 * if first call, init dptr.
	 */
	if ( !*dptr ) {
	    *dptr = opendir ( fspec );
	    if ( !*dptr ) {
		*fspec = '\0';		/* no more */
		*count = 0;
		return 36;
	    }
	}
	/*
	 * Retrieve next entry and copy to buffer.
         */
	for ( status = 1; status; ) {
	    status = readdir_r ( *dptr, &result );
	    if ( status == 0 ) {
		tu_strnzcpy ( buffer, result.d_name,
		    (bufsize >= result.d_namlen) ? bufsize-1 : result.d_namlen);
		*count = 1;
	    } else {
		*fspec = '\0';
		*count = 0;
		status = 0;
	    }
	}
	return 1;

    } else {
	/*
	 * A null fspec indicates all data returned, report EOF.
	 */
	*count = 0;
	return 1;
    }
}
/***************************************************************************/
/* Return file stats saved on file open.
 */
int tf_header_info ( void *fptr, int *size, unsigned *uic,
	unsigned int *cdate, unsigned int *mdate )
{
    int status;
    file_ctx ctx;
    ctx = (file_ctx) fptr;
    *size = ctx->size;
    *uic = (unsigned) ctx->uid;
    *cdate = (unsigned int) ctx->ctime;
    *mdate = ctx->mtime;
    return 0;
}
/****************************************************************************/
/* Convert unix binary time to RFC 1123 time format:
 *  	Wdy, DD Mon YYYY HH:MM:SS TIMEZONE
 *
 * If DATE_FORMAT_850 defined, use older format.
 *  	Weekday, DD-Mon-YY HH:MM:SS TIMEZONE
 *
 */
#ifdef DATE_FORMAT_850
#define DATE_FMT "%A, %d-%b-%y %T GMT"
#else
#define DATE_FMT "%a, %d %b %Y %T GMT" 
#endif
char *tf_format_time ( unsigned long bintim, char *buffer ) {
    char *timstr; int i, status;
    struct tm utc;
    /*
     * Convert to time string and copy to caller's buffer, don't include '\n'
     */
    status = gmtime_r ( (time_t *) &bintim, &utc );
    if ( status == 0 ) {
        i = strftime ( buffer, 40, DATE_FMT, &utc );
	if ( i > 0 ) buffer[i] = '\0';
    } else {
	tu_strcpy ( buffer, "???" );
    }
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
    unsigned long offset[2], seconds, conv;
    char asctim[32], *ptime;
    struct tm utc;
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
    asctim[20] = '\0';
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
     * strptime is unforgiving about capitization of the month, so
     * make sure only first character is upper case.
     */
    tu_strlowcase ( &asctim[3], &asctim[3] );
    asctim[3] = _toupper ( asctim[3] );
#ifdef DEBUG
    tlog_putlog(0,"asctim for decode: !AZ!/", asctim );
#endif
    /*
     * Decode normalized time and convert to bintim.
     */
    ptime = strptime ( asctim, "%d-%b-%Y %H:%M:%S", &utc );
#ifdef DEBUG
    tlog_putlog(0,"utc: yr=!SL, mon=!SL, dy=!SL, hr=!SL min=!SL s=!SL (!SL)!/",
	utc.tm_year, utc.tm_mon, utc.tm_mday, utc.tm_hour, utc.tm_min,
	utc.tm_sec, ptime );
#endif
    utc.tm_isdst = 0;
    if ( ptime ) seconds = (unsigned long) mktime ( &utc );
    else seconds = 0;
#ifdef DEBUG
    tlog_putlog(0,"Seconds: !SL!/", seconds );
#endif
    return seconds;
}
