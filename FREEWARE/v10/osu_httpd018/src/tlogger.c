/* This module provides log output for the http server.
 *
 * The output may be routed to any of three files:
 *     Error log
 *     Access log
 *     Trace log
 *
 * This module provides output to an application log file.  It formats
 * data via SYS$FAO to facilitate putting timestamps in the control strings.
 *
 *  int tlog_init ( char *log_file_name );
 *  int tlog_initlog ( int type, char * fname );	! type 1 
 *  int tlog_putlog ( int level, char *ctlstr, ... );
 *  int tlog_flush;
 *  int tlog_reopen ( level );
 *  int tlog_rundown();
 *
 * Author:	David Jones
 * Revised:	27-AUG-1994		Added tlog_reopen().
 * Revised:      3-NOV-1994		Added tlog_rundown().
 * Revised:	3-MAR-1995		Use pthreads timing for flushing, add
 *					structure for daily rollover of files.
 */
#include <stdio.h>
#include <stdarg.h>
#ifdef VMS
#include <descrip.h>
#include <ssdef.h>
#define VMS_ARG(a) ,a
#else
#define SYS$FAOL SYS_FAOL
struct dsc$descriptor {
    short int dsc$w_length;
    unsigned char dsc$b_dtype, dsc$b_class;
    void *dsc$a_pointer;
};
#define DSC$K_DTYPE_T 0
#define DSC$K_CLASS_S 0
#define VMS_ARG(a)
#endif
#include <string.h>
#include "pthread_np.h"
#include <errno.h>
#include <time.h>

static FILE *log_file;			/* error messages (type 0) */
static FILE *acc_file;			/* access log (type 1) */
static FILE *trc_file;			/* trace file */
static int log_valid = 0;
static int acc_valid = 0;
static int trc_valid = 0;
static int trace_level = 0;
static struct dsc$descriptor control;	/* control string */
static struct dsc$descriptor output;	/* control string */
static char log_file_spec[256];
static char acc_file_spec[256];
static char trc_file_spec[256];
static pthread_mutex_t log_write;
static pthread_cond_t log_modified;
static int flush_pending;		/* -1-starting, 0-idle, 1-pending */
int http_log_level;
#ifdef __DECC
/* Work around problems with fsync() not being ANSI standard */
#ifndef fsync
#define fsync decc$fsync
int decc$fsync(int fd);
#endif
#else
int fsync();
#endif
#define FLUSH_INTERVAL 60
/****************************************************************************/
/* Define start routine for thread that flushes log files to disk peiodically
 * as well as takes action every midnight.
 */
static int tlog_new_day();
static int tlog_flusher ( char * fname )
{
    int status, new_day;
    struct timespec delta, abstime, midnight;
    /*
     * Any time this routine executes, we will have the log_write mutex.
     * (mutex is relesed while waiting). Make initial call to new_day
     * routine to get absolute time of first midnight.
     */
    pthread_mutex_lock ( &log_write );
    new_day = 0;
    tlog_new_day ( new_day, &midnight );
    /*
     * Main loop.
     */
    for ( ; ; ) {
        /*
         *  Wait for someone (anyone, even spurious) to signal us.
         */
	flush_pending = 0;
	status = pthread_cond_timedwait (&log_modified, &log_write, &midnight);
	/*
	 * Compute 1 minute from now and do timed wait.  Set flush_pending
	 * to 1 to inhibit any more signals from other threads.
	 * We ignore any other signals that show up.
	 */
	flush_pending = 1;
        delta.tv_sec = FLUSH_INTERVAL;
        delta.tv_nsec = 0;
	if ( 0 == pthread_get_expiration_np ( &delta, &abstime ) ) {
	    /* Don't wait past midnight */
	    if ( abstime.tv_sec >= midnight.tv_sec ) {
		abstime = midnight;
		new_day = 1;
	    }
	    while ( 0 == pthread_cond_timedwait ( &log_modified,
		&log_write, &abstime ) );
	}
	/*
	 * We get to this point either because it is midnight (new day) or
	 * the 1 minute flush interval after a write has expired.
	 */
	if ( !new_day ) {
	    /*
	     * Wait for tlog_flush() to release us, then flush.  By waiting 
	     * for the next call to tlog_flush the disk writes are deferred 
	     * until the server is at a quieter point.
	     */
	    for ( flush_pending = 2; flush_pending == 2; ) {
		status = pthread_cond_timedwait 
			( &log_modified, &log_write, &midnight );
		if ( status == -1 ) {
		    /* We turned into a pumpkin */
		    new_day = 1;
		    break;
		}
	    }
            if ( log_valid ) {
	        LOCK_C_RTL
	        fflush ( log_file ); status = fsync ( fileno(log_file) );
	        UNLOCK_C_RTL
	    }
            if ( acc_valid ) {
	        LOCK_C_RTL
	        fflush ( acc_file ); status = fsync ( fileno(acc_file) );
	        UNLOCK_C_RTL
	    }
            if ( trc_valid ) {
	        LOCK_C_RTL
	        fflush ( trc_file ); status = fsync ( fileno(trc_file) );
	        UNLOCK_C_RTL
	    }
	}
	if ( new_day ) {
	    /*
	     * Mark flush state as 'new day' and call routine to do
	     * start-of-day process.  This routine must update midnight.
	     */
	    flush_pending = 3;
	    tlog_new_day ( new_day, &midnight );
	    new_day = 0;
	}
    }
    return 0;		/* can't really get here */
}
/*****************************************************************************/
static int tlog_new_day ( int new_day, struct timespec *midnight )
{
    time_t bintim;
    struct tm now;
    struct timespec delta;
    /*
     * If we want to roll over log files, do it here while we still
     * have log_write mutex.
     */
    pthread_mutex_unlock ( &log_write );
    /*
     * Reset for next midnight.
     */
    pthread_lock_global_np();		/* localtime() not reentrant */
    time ( &bintim );
    now = *(localtime(&bintim));
    pthread_unlock_global_np();
    delta.tv_sec = 86400-(((now.tm_hour*60)+now.tm_min)*60+now.tm_sec);
    delta.tv_nsec = 0;
    pthread_get_expiration_np ( &delta, midnight );
    pthread_mutex_lock ( &log_write );
    return 0;
}
/*****************************************************************************/
int tlog_init ( char *log_file_name )
{
    int status, pthread_create();
    pthread_t flusher;
    if ( log_valid ) return 3;
    log_file = fopen ( log_file_name, "w"
	VMS_ARG("dna=.log") VMS_ARG("alq=100") VMS_ARG("deq=600")
	VMS_ARG("rfm=var") VMS_ARG("mrs=300") VMS_ARG("ctx=rec") 
	VMS_ARG("shr=upd") );
    strcpy ( log_file_spec, log_file_name );
    if ( log_file ) log_valid = 1;
    if ( !log_valid ) perror ( "Can't open log file" );
    /*
     * initialize static fields in descriptors.
     */
    control.dsc$b_dtype = DSC$K_DTYPE_T;
    control.dsc$b_class = DSC$K_CLASS_S;
    output.dsc$b_dtype = DSC$K_DTYPE_T;
    output.dsc$b_class = DSC$K_CLASS_S;
    /*
     * Initialize mutexes and conditions.
     */
    pthread_mutex_init ( &log_write, pthread_mutexattr_default );
    pthread_cond_init ( &log_modified, pthread_condattr_default );
    /*
     * Start thread whose job in life is to flush the log files to
     * disk periodically.
     */
    flush_pending = 0;
    status = pthread_create ( &flusher, pthread_attr_default,
	tlog_flusher, (pthread_addr_t) log_file_name );
    if ( status ) perror ( "Can't start log flusher thread" );

    return log_valid;
}
/***********************************************************************/
/* Initialize secondary log files (access log and trace log).
 * If trace log file name is empty, send trace into to error log.
 */

int tlog_initlog ( int level,		/* which file: -1 access, 1 trace */
	char *fname )
{
   if ( level < 0 ) {		/* access log */
       if ( acc_valid ) return 3;
        strcpy ( acc_file_spec, fname );
        acc_file = fopen ( fname, "a"
	    VMS_ARG("dna=.log") VMS_ARG("alq=100") VMS_ARG("deq=600")
	    VMS_ARG("mrs=900") VMS_ARG("shr=upd") );
	if ( acc_file ) acc_valid = 1;
	return acc_valid;

    } else {
	trace_level = level;
        if ( trc_valid ) return 3;		/* Already selected */
	if ( *fname == '\0' ) fname = log_file_spec;
        strcpy ( trc_file_spec, fname );
	if ( strcmp ( fname, log_file_spec ) == 0 ) {
	    /*
	     * Trace log and error log are same, make log file invalid.
	     */
	    trc_file = log_file;
	    log_valid = 0;
	    trc_valid = 1;
	    return trc_valid;
	}
        trc_file = fopen ( fname, "w"
	    VMS_ARG("dna=.log") VMS_ARG("alq=100") VMS_ARG("deq=600")
	    VMS_ARG("rfm=var") VMS_ARG("mrs=1200") VMS_ARG("ctx=rec") 
	    VMS_ARG("shr=upd") );
	return trc_valid;
    }
 }

/*
 * Format log output and send to appropriate log file based upon log
 * level argument and trace level set with tlog_initlog;
 *     0	Error, write to trace file and error log.
 *    <0	Normal access, write to access log.
 *    >0	Write to trace file if trace_level greater or equal to level.
 */
int tlog_putlog ( int level, char *ctlstr, ... )
{
    va_list param_list;
#ifdef __ALPHA
    unsigned int __VA_COUNT_BUILTIN(void), arg_count;
    int i, fao_param[32];
#endif
    int status, SYS$FAOL(), length, clen, valid;
    char outbuf[1000];
    FILE *fp;
    va_start(param_list,ctlstr);
    /*
     * See if log file available and synchronize access to static areas.
     */
    if ( !log_valid && !acc_valid && !trc_valid ) return 0;
    if ( (http_log_level == 997) && (level > 1) ) {
	int ts_tcp_stack_used();
	tlog_putlog ( 1, "Stack usage: !SL bytes!/", ts_tcp_stack_used() );
    }
    pthread_mutex_lock ( &log_write );
    for ( clen = 0; ctlstr[clen]; clen++ );
    control.dsc$w_length = clen;
    control.dsc$a_pointer = ctlstr;
    output.dsc$w_length = sizeof(outbuf);
    output.dsc$a_pointer = outbuf;

    length = 0;
    LOCK_C_RTL
#ifdef __ALPHA
    arg_count = __VA_COUNT_BUILTIN() - 1;
    for ( i = 0; i < arg_count; i++ ) fao_param[i] = va_arg(param_list,int);
    LOCK_VMS_RTL
    status = SYS$FAOL ( &control, &length, &output, fao_param );
#else
    LOCK_VMS_RTL
    status = SYS$FAOL ( &control, &length, &output, 
#ifdef VMS
	param_list );
#else
	param_list.a0 + param_list.offset );	/* OSF/1 */
#endif
#endif
    UNLOCK_VMS_RTL
    if ( (status&1) == 1 ) {
	if ( level < 0 ) {
	    status = acc_valid ? fwrite(outbuf, length, 1, acc_file) : 1;
	    if ( status != 1 ) {
	        char errtxt[64];
	        sprintf(errtxt, "access log write error (length=%d)", length );
	        perror ( errtxt );
	    }
	} else {
	    status = log_valid && ( 0 == level ) ? 
			fwrite(outbuf, length, 1, log_file) : 1;
	    if ( status != 1 ) {
	        char errtxt[64];
	        sprintf(errtxt, "log file write error (length=%d)", length );
	        perror ( errtxt );
	    }
	    status = trc_valid && ( trace_level >= level ) ?
			 fwrite(outbuf, length, 1, trc_file) : 1;
	    if ( status != 1 ) {
	        char errtxt[64];
	        sprintf(errtxt, "trace file write error (length=%d)", length );
	        perror ( errtxt );
	    }
	}
    } else {
	fprintf(log_file,"FAO error: %d '%s'\n", status, ctlstr );
    }
    UNLOCK_C_RTL
    /*
     * Restart flush timer if idle.
     */
    if ( !flush_pending ) pthread_cond_signal ( &log_modified );
    pthread_mutex_unlock ( &log_write );
    return status;
}
/*
 * Flush pending data to log file periodically.
 */

int tlog_flush()
{
    if ( log_valid  || acc_valid || trc_valid ) {
	pthread_mutex_lock ( &log_write );
	if ( flush_pending == 2 ) {
	    flush_pending = 1;
	    pthread_cond_signal ( &log_modified );
	}
	pthread_mutex_unlock ( &log_write );
    }
    return 0;
}
/*****************************************************************************/
/* Make new version of open log file. */
tlog_reopen ( int level ) {
{
   if ( level < 0 ) {		/* access log */
        /*
	 * Make new version of access log.
	 */
	pthread_mutex_lock ( &log_write );
       if ( acc_valid ) {
	    LOCK_C_RTL
	    fclose ( acc_file );
            acc_file = fopen ( acc_file_spec, "w"
	        VMS_ARG("dna=.log") VMS_ARG("alq=100") VMS_ARG("deq=600")
	        VMS_ARG("mrs=900") VMS_ARG("shr=upd") );
	    UNLOCK_C_RTL
	    if ( acc_file ) acc_valid = 1;
	}
	pthread_mutex_unlock ( &log_write );
	return acc_valid;

    } else {
	/*
	 * Make version version of trace log and set logger level.
	 */
	pthread_mutex_lock ( &log_write );
	if ( trc_valid ) {
	    trace_level = level;
	    LOCK_C_RTL
	    fclose ( trc_file );
            trc_file = fopen ( trc_file_spec, "w"
	        VMS_ARG("dna=.log") VMS_ARG("alq=100") VMS_ARG("deq=600")
	        VMS_ARG("rfm=var") VMS_ARG("mrs=1200") VMS_ARG("ctx=rec")
	        VMS_ARG("shr=upd") );
	    UNLOCK_C_RTL
	}
	pthread_mutex_unlock ( &log_write );
	return trc_valid;
    }
 }
}
/*************************************************************************/
/* Close open files. to cleanly flush. 
*/
int tlog_rundown()
{
    int status;
    pthread_mutex_lock ( &log_write );
    if ( log_valid ) {
	log_valid = 0;
	LOCK_C_RTL
	fclose ( log_file );
	UNLOCK_C_RTL
    }
    if ( acc_valid ) {
	acc_valid = 0;
	LOCK_C_RTL
	fclose ( acc_file );
	UNLOCK_C_RTL
    }
    if ( trc_valid ) {
	trc_valid = 0;
	LOCK_C_RTL
	fclose ( trc_file );
	UNLOCK_C_RTL
    }

    pthread_mutex_unlock ( &log_write );

    return 1;
}
