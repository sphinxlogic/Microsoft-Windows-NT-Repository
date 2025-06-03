/*
 * Support routines for dynamically loaded (shareable image) MST routines:
 *
 *   int mstshr_init ( mst_linkage vector, char *info, char *errmst );
 *   int mstshr_cgi_symbols ( mst_link_t cnx, envbf *env )
 *   char *mstshr_getenv ( char *name, envbuf *env );
 *
 * Note that the MSTs triggered by exec rules are technically script-servers, 
 * not scripts.  The mstshr_cgi_symbols routine constructs the SCRIPT_NAME
 * and PATH_INFO variables as if a script name is part of the URL.
 *
 * Author: David Jones
 * Date:   10-JUN-1995
 * Revised:12-JUN-1995		rev 2 of mst_linkage struct.
 * Revised:14-JUN-1995		Use verified xlate for path.
 */
#include "pthread_np.h"
#include <stdlib.h>
#include <stdio.h>
#include "mst_share.h"
#include "tutil.h"

static char *basenv_name[] = { "PATH", "HOME", "TERM", "USER", "" };
static char *basenv_val[] =  { "",     "",     "",     "",     ""};

/****************************************************************************/
/* Initialize linkage structure for callbacks.
 */
int mstshr_init ( mst_linkage vector, char *info, char *errmsg )
{
    int i;
    /*
     * Check that versions match so be know linkage structure layout the same.
     */
    if ( vector->version != MST_CB_VERSION ) {
	tu_strcpy ( errmsg, 
	   "Callback structure version mismatch in shareable image" );
	return 0;
    }
    /*
     * load function pointers to allow us to get mst comm routines in 
     * main image.
     */
    mst_read_cb = vector->read;
    mst_write_cb = vector->write;
    mst_close_cb = vector->close;
    mst_exit_cb = vector->exit;
    mst_format_error_cb = vector->format_error;
    mst_connect_cb = vector->connect;
    mst_create_service_pool_cb = vector->create_service_pool;
    mst_register_service_cb = vector->register_service;

    http_log_level_p = vector->trace_level;
    tlog_putlog_cb = vector->putlog;
    http_reentrant_c_rtl_p = vector->reentrant_c_rtl;
    http_reentrant_vms_rtl_p = vector->reentrant_vms_rtl;
    /*
     * Setup static data for use by CGI support routines.
     */
    for ( i = 0; *basenv_name[i]; i++ ) {
	basenv_val[i] = getenv ( basenv_name[i] );
	if ( !basenv_val[i] ) basenv_val[i] = "<undefined>";
    }
    return 1;
}
/*****************************************************************************/
/* forward references */
static int add_var ( char *name, char *value, int vlen, 
	struct mstshr_envbuf *env );
static int merge_var ( char *name, char *value, int vlen, 
	struct mstshr_envbuf *env );
static void convert_headers ( mst_link_t link, struct mstshr_envbuf *env, 
	char *buffer, int *content_length );
/***************************************************************************/
/* Define fake environment array. in structure.  This routine reads the
 * scriptserver prologue send by script_execute.
 */
int mstshr_cgi_symbols 
	( mst_link_t link, char *info, struct mstshr_envbuf *env )
{
    int start, length, i, j, status, content_length, xlate_len, ep_len;
    int length2;
    char *path_info, *arg_buf;
    char qbuf[4096], bin_path[256];
    /*
     * Initialize env structure to emtpy.
     */
    env->count = 0;
    env->ptr[0] = (char *) 0;
    env->used = 1;
    env->buf[0] = '\0';
    for ( i = 0; i < 4; i++ ) env->prolog[i] = env->buf;
    /*
     * Read prologue msgs: subfunc, method, protocol, xlated_ident
     */
    j = env->used;		/* portion of arg_buf filled */
    for ( arg_buf = env->buf, i = 0; i < 4; i++ ) {
	env->prolog[i] = &arg_buf[j];
	status = mst_read 
		( link, env->prolog[i], sizeof(env->buf)-1-j, &length );
	if ( status != MST_NORMAL ) break;
	arg_buf[j+length] = '\0';
	j = j + length + 1;
    }
    if ( i < 4 ) return 0;		/* protocol error */
    env->used = j;
    /*
     * Copy standard environment variables determined at program startup
     * and make variables out of 2 of the proloque items.
     */
    for ( i = 0; *basenv_name[i]; i++ ) {
	add_var(basenv_name[i], basenv_val[i], tu_strlen(basenv_val[i]), env);
    }
    add_var ( "GATEWAY_INTERFACE", "CGI/1.1", 7, env );
    add_var ( "REQUEST_METHOD", env->prolog[1], tu_strlen(env->prolog[1]), 
		env );
    add_var ( "SERVER_PROTOCOL", env->prolog[2], 
		tu_strlen(env->prolog[2]), env );
    /*
     * The ID2 tag returns information needed for several variables, delimited
     * by spaces.
     */
    status = mst_write ( link, "<DNETID2>", 9, &length );
    if ( (status&1) == 0 ) return status;
    status = mst_read ( link, qbuf, sizeof(qbuf)-1, &length );
    if ( (status&1) == 0 ) return status;
    else {
	/*
	 * Parse result into the separate environment variables.
	 */
	static char *id2_varname[] = { "SERVER_SOFTWARE", "SERVER_NAME",
	    "SERVER_PORT", "REMOTE_PORT", "!REMOTE_ADDR", "REMOTE_USER",
	    "REMOTE_HOST", "" };
	start = 0;
	for ( i = j = 0; i < length; i++ ) if ( qbuf[i] == ' ' ) {
	    char * varname, dot_fmt[16]; int addr;
	    varname = id2_varname[j++];
	    if ( *varname == '!' ) {	/* special formatting */
		varname = &varname[1];
		qbuf[i] = '\0';
		LOCK_C_RTL
		addr = atoi ( &qbuf[start] );
		sprintf ( dot_fmt, "%d.%d.%d.%d", (addr&255),
		    ((addr>>8)&255), ((addr>>16)&255), ((addr>>24)&255) );
		UNLOCK_C_RTL
		add_var ( varname, dot_fmt, tu_strlen(dot_fmt), env );

	    } else if ( *varname == '?' ) { /* conditional add */
		varname = &varname[1];
		if ( i > start )
	            add_var ( varname, &qbuf[start], i - start, env );
	    } else {
	        add_var ( varname, &qbuf[start], i - start, env );
	    }
	    if ( id2_varname[j][0] == '\0' ) break;
	    start = i + 1;
	}
	if ( i == length ) 
	    add_var ( id2_varname[j], &qbuf[start], i - start, env );
    }
    /*
     * Determine SCRIPT_PATH, SCRIPT_NAME, leave remainder (i.e. PATH_INFO)
     * in path_info variable.
     */
    tu_strnzcpy ( qbuf, info, sizeof(qbuf)-2 );
    ep_len = tu_strlen ( qbuf );
    add_var ( "EXEC_INFO",  qbuf, ep_len, env );
    status = mst_write ( link, "<DNETPATH>", 10, &length );
    if ( status == MST_NORMAL )  status = mst_read 
		( link, bin_path, sizeof(bin_path)-1, &length );
    if ( status != MST_NORMAL ) return status;

    bin_path[length] = '\0';		/* terminate string */
    add_var ( "SCRIPT_PATH", bin_path, length, env );

    path_info = env->prolog[3];			/* default PATH_INFO */
    if ( bin_path[0] == '/' ) {
        if ( 0 != tu_strncmp(path_info, bin_path, length) ) {
	   tlog_putlog(0, "Invalid URL (!AZ), must start with !AZ!/",
		env->prolog[3], bin_path);
	    return 0;
	}
	/*
	 * Determine length of script-name element and append to exec_path
	 */
	for ( i = 0; 
		path_info[i+length] && (path_info[i+length] != '/'); i++ ) {
	    if ( ep_len < sizeof(qbuf)-2 ) qbuf[ep_len++] =
		path_info[i+length];
	}
	qbuf[ep_len] = '\0';
	add_var ( "SCRIPT_NAME", path_info, length+i, env );
	path_info = &path_info[i+length];
        j = tu_strlen ( path_info );
    } else {
	/*
	 * Assume we are a presentation script, so entire string is path_info.
	 */
	add_var ( "SCRIPT_NAME", qbuf, ep_len, env );
	path_info = qbuf;
	status = mst_write ( link, "<DNETRQURL>", 11, &length2 );
	if ( (status&1) == 0 ) return status;
	status = mst_read ( link, qbuf, sizeof(qbuf)-1, &length2 );
	if ( (status&1) == 0 ) return status;
	else {
	    /* Trim off search arg */
	   for ( j = 0; j < length2; j++ ) if ( qbuf[j] == '?' ) break; 
	   qbuf[j] = '\0';
	}
    }
    /*
     * Set path_info and path_translated.
     */
    add_var ( "PATH_INFO", path_info, j, env );
    
    if ( (bin_path[0] == '/') ) {
        /*
         * Translate the path.
         */
        status = mst_write ( link, "<DNETXLATEV>", 12, &length2 );
        if ( (status&1) == 0 ) { 
	    tlog_putlog(0,"MST I/O failure in cgifork!/"); return status;}
        status = mst_write ( link, path_info, j, &length2 );
	status = mst_read ( link, qbuf, sizeof(qbuf)-1, &xlate_len);
        if ( (status&1) == 0 ) {
	    tlog_putlog(0,"MST I/O failure in cgifork!/"); return status;}
	if ( *path_info == '(' ) xlate_len = 0; /* illegal path */
        qbuf[xlate_len] = '\0';
        add_var ( "PATH_TRANSLATED", qbuf, xlate_len, env );
    } else {
	/*
	 * The file referenced was included in the prolog.
	 */
	add_var ( "PATH_TRANSLATED", env->prolog[3], 
		tu_strlen (env->prolog[3]), env );
    }
    /*
     * Fetch search argument.  Trim leading '?' in environment value.
     */
    status = mst_write ( link, "<DNETARG2>", 10, &length2 );
    status = mst_read ( link, qbuf, sizeof(qbuf)-1, &length2 );
    if ( (status&1) == 0 ) { 
	tlog_putlog(0,"MST I/O failure in cgifork!/"); return status;
    } else if ( length2 > 0 ) {
	qbuf[length2] = '\0';
	add_var ( "QUERY_STRING", &qbuf[1], length2-1, env );
    }
    /*
     * Convert request headers to environment variables, then terminate list.
     */
    convert_headers ( link, env, qbuf, &content_length );
    env->ptr[env->count] = (char *) 0;
    return 1;
}
/***************************************************************************/
/* Lookup symbol in envbuf, same semantics as getenv (returns NULL if name not
 * found).  Pointer returned is to part of env struct's buf array (i.e.
 * readonly).
 */
char *mstshr_getenv ( char *name, struct mstshr_envbuf *env )
{
    int i, j;
    char *var;

    for ( i = 0; i < env->count; i++ ) {
	/*
	 * We match name when first non-equal character is '=' sign.
	 * We assume string pointed to by env->ptr contains an '='.
	 */
	var = env->ptr[i];
	for ( j = 0; name[j]; j++ ) if ( name[j] != var[j] ) break;
	if ( (var[j] == '=') && (name[j] == '\0') ) return &var[j+1];
    }
   return (char *) 0;
}
/********************************************************************/
/*  Build array of character strings for execel call.
 */
static int add_var ( char *name, char *value, int vlen, 
	struct mstshr_envbuf *env )
{
    int i, nlen;

    if ( env->count >= 99 ) return -1;
    nlen = tu_strlen(name);
    if ( (nlen+vlen+2+env->used) > sizeof(env->buf) ) return -1;

    env->ptr[env->count++] = &env->buf[env->used];
    tu_strcpy ( &env->buf[env->used], name ); env->used += nlen;
    env->buf[env->used++] = '=';
    tu_strnzcpy ( &env->buf[env->used], value, vlen ); env->used += vlen;
    env->buf[env->used++] = '\0';
    return 1;
}
/********************************************************************/
static int merge_var ( char *name, char *value, int vlen, 
	struct mstshr_envbuf *env )
{
    int i, j, nlen;
    /*
     * Scan for matching name.
     */
    if ( vlen+2+env->used > sizeof(env->buf) ) return -1;	/* won't fit */
    nlen = tu_strlen(name);
    for ( i = env->count-1; i >= 0; --i ) {
	if ( tu_strncmp ( name, env->ptr[i], nlen ) == 0 ) {
	    if ( env->ptr[i][nlen] = '=' ) break;
        }
    }
    if ( i < 0 ) {
	/* Not found, append new name to end. */
	return add_var ( name, value, vlen, env );
    } else if ( i == env->count-1 ) {
	/*
	 * tack on to last variable.
	 */
	env->buf[env->used-1] = ',';
	env->buf[env->used++] = ' ';
	tu_strnzcpy ( &env->buf[env->used], value, vlen );
	env->used += (vlen+1);
	return 1;
    } else {
	/*
	 * Insert new data into list.
	 */
	char *base, *high;
	base = env->ptr[i+1] - 1;
	high = &env->buf[env->used];
	while ( high > base ) { high[vlen+2] = *high; --high; }
	for ( j = i+1; j < env->count; j++ ) env->ptr[j] += (vlen+2);
	*base++ = ',';
	*base++ = ' ';
	tu_strnzcpy ( base, value, vlen );
	env->used += (vlen+2);
	return 1;
    }
}
/********************************************************************/
/* Scan the request headers and convert into variables on the
 * environment list of for "HTTP_label".
 */
static void convert_headers ( mst_link_t link, struct mstshr_envbuf *env, 
	char *buffer, int *content_length )
{
    char label[256];
    int status, length, colon, label_len;

    *content_length = 0;
    status = mst_write ( link, "<DNETHDR>", 9, &length );
    if ( (status&1) == 0 ) return;
    do {
	status = mst_read ( link, buffer, 4096, &length );
	if ( (status&1) == 0 ) return;
	/* Parse out header label */
	for (colon = 0; colon < length; colon++ ) if ( buffer[colon] == ':') {
	    label_len = colon + 5;
	    if ( label_len >= sizeof(label) ) label_len = sizeof(label) - 1;
	    tu_strcpy ( label, "HTTP_" );
	    tu_strnzcpy ( &label[5], buffer, label_len - 5 );
	    tu_strupcase ( label, label );
	    /* Trim leading whitespace */
	    for ( colon++; buffer[colon] == ' ' || buffer[colon] == '\t';
		colon++ );

	    /*
	     * Check for special labels we are interested in.
	     */
	    if ( tu_strncmp ( label, "HTTP_CONTENT_LENGTH", 20 ) == 0 ) {
		tu_strcpy ( label, "CONTENT_LENGTH" );
		LOCK_C_RTL
		*content_length = atoi(&buffer[colon]);
		UNLOCK_C_RTL
	    } else if ( tu_strncmp(label,"HTTP_CONTENT_TYPE", 20) == 0 ) {
		tu_strcpy ( label, "CONTENT_TYPE" );
	    }
	    merge_var ( label, &buffer[colon], length-colon, env );
	    break;
	} else if ( buffer[colon] == '-' ) buffer[colon] = '_';
    } while ( length > 0 ); 	/* continue until nul line read */
}
