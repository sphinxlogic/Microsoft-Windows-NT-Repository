/*
 * Return user authorization information about a user:
 *	uic			User uic
 *	Login dir.		User's login directory (device + dir),
 *				converted to unix filename format.
 *
 * Revised:	10-AUG-1994	Fixed bug in checking for disable account.
 * Revised:	14-FEB-1995	Check OS version and use context arg in GETUAI.
 */
#include <stdio.h>
#include <stdlib.h>
#include <descrip.h>
#include <uaidef.h>
#include <syidef.h>
#include <prvdef.h>
#include "pthread_np.h"
#include "tutil.h"

int http_log_level;		/* Logging control flag */
#define USERNAME_MAX 12

int http_user_information ( char *username, int user_len,
	int *uic,
	char *login_dir, int maxlen )
{
    int i, flags, devlen, dirlen, status, SYS$GETUAI(), SYS$GETSYIW(), length;
    int delta, privs[2], priority, scheduler, SYS$SETPRV(), tlog_putlog();
    long *ctxptr, iosb[2];
    pthread_t self;
    struct { short length, code; char *buffer; int *ret_len; } item[5];
    char device[40], defdir[64], userkey[USERNAME_MAX+1];
    $DESCRIPTOR (username_dx,"");    
    static int uai_version = 0;		/* 0- unknown, 1- old, 2 - new */
    static long context;
    /*
     * Determine if safe to use context argument.
     */
    if ( uai_version == 0 ) {
	pthread_lock_global_np();
	if ( 0 == uai_version ) {
	    uai_version = 1; context = -1;
	    item[0].code = SYI$_NODE_SWVERS;
	    item[0].length = 8;
	    item[0].buffer = device;
	    item[0].ret_len = &length;
	    item[1].code = item[1].length = 0;
	    device[0] = '\0';
	    status = SYS$GETSYIW ( 0, 0, 0, &item, iosb, 0, 0 );
	    if ( status&1 ) status = iosb[0];
	    if ( status&1 ) {
		device[length] = '\0';
		if ( tu_strncmp ( device, "V6.1", 8 ) >= 0 ) uai_version = 2;
	    }
	    tlog_putlog ( 10, "Using context arg in GETUAI: !SL!/", uai_version );
	}
	pthread_unlock_global_np();
    }
    ctxptr = (uai_version == 2) ? &context : (long *) 0;
    /*
     * Build upcased descriptor for username.
     */
    if ( user_len > USERNAME_MAX ) return 20;	/* invalid name */
    if ( user_len <= 0 ) return 20;			/* Null username */

    tu_strnzcpy ( userkey, username, user_len );
    tu_strupcase ( userkey, userkey );
    username_dx.dsc$w_length = user_len;
    username_dx.dsc$a_pointer = userkey;
    /*
     * Make item list for getuai.
     */
    item[0].code = UAI$_FLAGS; item[0].length = sizeof(flags);
    item[0].buffer = (char *) &flags; item[0].ret_len = (int *) 0;

    item[1].code = UAI$_DEFDEV; item[1].length = sizeof(device);
    item[1].buffer = device; item[1].ret_len = &devlen; 
    devlen = 0; device[0] = '\0';

    item[2].code = UAI$_DEFDIR; item[2].length = sizeof(defdir);
    item[2].buffer = defdir; item[2].ret_len = &dirlen; 
    dirlen = 0; defdir[0] = '\0';

    item[3].code = UAI$_UIC; item[3].length = sizeof(int);
    item[3].buffer = (char *) uic; item[3].ret_len = (int *) 0;

    item[4].code = 0; item[4].length = 0;	/* terminate list */
    /*
     * Fetch information abour user.  Block other activity and set privilege.
     */
    self = pthread_self();
    priority = pthread_getprio ( self );
    scheduler = pthread_getscheduler ( self );
    status = pthread_setscheduler ( self, SCHED_FIFO, PRI_FIFO_MAX );
    LOCK_VMS_RTL
    privs[1] = 0; privs[0] = PRV$M_SYSPRV;
    SYS$SETPRV ( 1, &privs, 0, 0 );
    status = SYS$GETUAI ( 0, ctxptr, &username_dx, item, 0, 0, 0 );
    SYS$SETPRV ( 0, &privs, 0, 0 );
    UNLOCK_VMS_RTL
    i = pthread_setscheduler ( self, scheduler, priority );
    /*
     * Check if logging enabled to this level and place info in log file.
     */
    if ( http_log_level > 10 ) {
	tlog_putlog ( 10,"userdir username: '!AS', getuai status: !SL (!SL, !SL)!/", 
		&username_dx, status, devlen, dirlen );
	tlog_putlog ( 10, "flags: !XL, device: '!AC', dir: '!AC'!/", flags,device, defdir );
    }
    /*
     * Return if error or account not enabled.
     */
    if ( (status&1) == 0 ) return status;		/* GETUAI error */
    if ( (flags&UAI$M_DISACNT) != 0 ) return 20;	/* Disusered account */
    /*
     * Fixup punctuation for conversion to unix filepath format.
     */
    devlen = device[0];		/* counted ascii */
    if ( device[devlen] == ':' ) devlen = devlen - 1;	/* trim colon */
    dirlen = defdir[0];
    if ( (defdir[dirlen] == ']') || (defdir[dirlen] == '>') ) {
	defdir[1] = '.';	/* Convert leading bracket */
	defdir[dirlen] = '.';	/* Convert trailing bracket */
    }
    for ( i = 1; i <= dirlen; i++ ) if ( defdir[i] == '.' ) defdir[i] = '/';
    /*
     * Load into users output.
     */
    if ( (devlen + dirlen + 2) > maxlen ) return 20;
    login_dir[0] = '/';
    tu_strncpy ( &login_dir[1], &device[1], devlen );
    tu_strnzcpy ( &login_dir[1+devlen], &defdir[1], dirlen );

    return 1;
}
