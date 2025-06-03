/*
 * This module is an MST server script for the DECthread HTTP_SERVER.  It
 * provides support for extended directory browsing.  Options are read
 * from a separate configuration file at startup.
 *
 * Use the following rules in the configuration file to load this module and 
 * delcare it a converter script for file directories.
 *
 *    ThreadPool ds_pool stack=162000 q_flag=1 limit=5
 *    Service dirserv pool=ds_pool dynamic=(dirserv,http_dirserv_mst)\
 *	info=www_system:http_dirserv.conf
 *    presentation text/file-directory %dirserv:/dirserv
 *
 *
 *  Author:	David Jones
 *  Date:	5-SEP-1995
 */ 
#include "pthread_np.h"
#include <time.h>
#ifdef VMS
#include <stat.h>
#else
#include <sys/stat.h>
#endif
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "dirserv_options.h"	/* dir_opt typdef */
#define CONDITIONAL_YIELD if ( !http_reentrant_c_rtl ) pthread_yield();
#include <stdlib.h>
#include "tutil.h"
#include "file_access.h"
#include "mst_share.h"	/* mst support routines */

struct stream_ctx {
    void *ff;			/* Input file */
    int used, filled;
    char buffer[4096];
};
typedef struct stream_ctx *stream;
int htds_read_rules(), htds_get_rules();
/********************************************************************/
/* Init routine for use when this MST is a dynamically loaded shareable image.
 */
int dirserv_init ( mst_linkage vector, char *info, char *errmsg )
{
    int status;
    status = mstshr_init ( vector, info, errmsg );
    if ( (status&1) == 0 ) return status;
    tf_initialize("");
    /*
     * Read config file name, specified via info argument.
     */
    status = htds_read_rules ( info );
    sprintf ( errmsg, "Status of reading '%s' is %d", info, status );
    return status;
}

/**************************************************************************/

/*
 * Thread-based DECnet script server for use with HTTP server.  Provide
 * extended directory listing.  Note that this is a scriptserver program,
 * not a WWWEXEC-based script.
 *
 * The program reads a configuration file to determine how to present
 * the directory.  The file format is that same as for the HTTP server
 * but the rule set is different:
 *
 *     Include file-name		# Include specified file.
 *     Welcome file-name		# add directory index to search list.
 *     DirAccess [ON|OFF|SELECTIVE [control-file [OVERRIDE]]]
 *					# controls file listing.
 *     DirReadme [OFF|TOP|BOTTOM] [readme-file]
 *					# controls inclusion of README files.
 *     DirShowDate [ON|OFF]		# Include last modify date.
 *     DirShowSize [ON|OFF]		# Include file size.
 *     DirShowBytes [ON|OFF]		# Report size in bytes rather than Kb.
 *     DirShowHidden [ON|OFF]		# Supress listing ".ext" files.
 *
 * The name of the configuration file is supplied by the HTTP server 
 * configuration (see below).
 *
 * Command-line format:
 *
 *	$ dirserv err-file
 *
 * args:
 *    err-file	    Name of log file to create, connections and query strings
 *		    are written to the log file.
 *
 * Logical names:
 *    WWW_DIRSERV_OBJECT	If defined as an exec mode logical, indicates
 *				that server should become a persitent network
 *				object using the equivalence name as taskname.
 *				If not defined, program will translate sys$net
 *				and service a single request.
 *
 *    WWW_DIRSERV_ACCESS	List of node/username pairs that are allowed
 *				to access this object.  If missing, no access
 *				checks are performed.
 *
 * Environment variables:
 *
 * HTTP server configuration:
 *     To configure this program as the server's directory presentation
 *     script, add a presentation rule with the following form:
 *
 *		presentation text/file-directory node::"0=WWWDIR"config-file
 *
 *  Author:	David Jones
 *  Date:	 1-DEC-1994
 *  Revised:    16-DEC-1994		Support HEAD command (req. by spec).
 *  Revised:    17-DEC-1994		Added remote user check.
 *  Revised:	21-FEB-1995		Fix SYSNAM report and cleanly close
 *					connections.
 *  Revised:    16-MAR-1995		Fix bug introduced by 21-feb fix.
 *  Revised:	31-MAR-1995		Work around Netscape bug.
 */



#define RESPONSE_LINE_LIMIT 100
typedef struct { int l; char *s; } string;
struct out_stream { void *ctx; int used; char buf[1024]; };
/*
 * Global (program-wide) variables.
 */
char **http_index_filename;		/* List of welcome files */

static int min_available;
static int send_module(), send_catalogue();
static char conf_file[256];
static int list_files ( struct out_stream *out, void *dirf, dir_opt *opt,
	char *dirbuf, int bufsize, char *full_name, char *tail );
/**************************************************************************/
/* Utility routine to buffer output, accumulating small text fragments
 * into larger chunks for efficient I/O
 */

static int put_text ( struct out_stream *stream, char *ctlstr, ... )
{
    int used, status, i, count, lo;
    char *buf, *text;
    va_list arg;
    used = stream->used;
    buf = &stream->buf[used];
    va_start ( arg, ctlstr );
    for (i = 0; ctlstr[i]; i++ ) {
	if ( (ctlstr[i] == '%') && 
	    ((ctlstr[i+1] == 's') || (ctlstr[i+1] == 't')) ) {
	    /* Replace %s in ctlstr with next argument */
	    i++;
	    if ( ctlstr[i] == 's' ) 
		for (text = va_arg(arg,char *); *text; used++) {
		    if ( used >= sizeof(stream->buf) ) {
	        	status = mst_write ( stream->ctx, stream->buf, used, &lo );
	        	if ( (status&1) == 0 ) return status;
	        	used = 0;
	        	buf = stream->buf;
		    }
	            *buf++ = *text++;
		}
	    else for (text = va_arg(arg,char *); *text; used++) {
		    if ( *text == '\n' ) {
		        if ( used >= sizeof(stream->buf) ) {
	        	    status = mst_write ( stream->ctx, 
					stream->buf, used, &lo );
	        	    if ( (status&1) == 0 ) return status;
	        	    used = 0;
	        	    buf = stream->buf;
		        }
			*buf++ = '\r';
			used++;
		    }
		    if ( used >= sizeof(stream->buf) ) {
	        	status = mst_write ( stream->ctx, stream->buf, used, &lo );
	        	if ( (status&1) == 0 ) return status;
	        	used = 0;
	        	buf = stream->buf;
		    }
	            *buf++ = *text++;
		}
	} else {
	    /* Add ctlstr character to output stream. */
	    if ( used >= sizeof(stream->buf) ) {
	        status = mst_write ( stream->ctx, stream->buf, used, &lo );
	        if ( (status&1) == 0 ) return status;
	        used = 0;
	        buf = stream->buf;
	    }
	    *buf++ = ctlstr[i]; used++;
	}
    }
    stream->used = used;
    return 1;
}
/*************************************************************************/
/* Main routine for handling htds server connection.  This function is called 
 * as the thread init routine for MST server threads.
 */
int dirserv ( mst_link_t ctx, 	  	/* MST connection context */
	char *service, 			/* Service name */
	char *info, 			/* optional argument with exec rule */
	int ndx,			/* Thread index */
	int available )			/* # of contexts left on  free list */
{
    int length, status, i, tf_len, j, lo;
    int header_only;
    string prologue[4];
    string request[128];
    char *full_name, *tail, prolog_buf[1104];

    char errmsg[256], dir_file[256], dirbuf[4096];
    char taskname[20], cre_date[32], remote_node[256], remote_user[64];
    char *bp, *opcode, *method, *url, *protocol, *module, log_prefix[40];
    struct out_stream outbound, *out;
    void *dirf;
    dir_opt opt;
    /*
     * Make prefix string for log entries so we can follow interleaved
     * entries in log file.
     */
    tu_strcpy ( log_prefix, "Service" );
    tu_strnzcpy ( &log_prefix[7], service, sizeof(log_prefix)-18 );
    i = tu_strlen ( service ) + 7; log_prefix[i++] = '/';
    tu_strint ( ndx, &log_prefix[i] );
    if ( http_log_level > 0 ) tlog_putlog ( 1, 
	"!AZ connected, info: '!AZ', pool remaining: !SL!/", log_prefix,
	info, available );
    /*                   0      1        2        3
     * Read prologue (module, method, protocol, ident) sent by HTTP server.
     */
    for ( i = 0, bp = prolog_buf; i < 4; i++ ) {
	status = mst_read ( ctx, bp, 255, &length );
	if ( (status&1) == 1 ) {
	    prologue[i].l = length;
	    prologue[i].s = bp;
	    bp[length++] = '\0';	/* safety first, terminate string */
	    bp = &bp[length];
	} else {
	    tlog_putlog ( 0, "!AZ, Error reading prologue: !SL!/", log_prefix,
			status );
	    return status;
	}
    }
    htds_get_rules ( (void *) 0, &opt );	/* Load global values */
    /*
     * Fetch original URL so we can put it in page title.
     */
    mst_write ( ctx, "<DNETRQURL>", 11, &lo );
    status = mst_read ( ctx, dirbuf, sizeof(dirbuf)-1, &length );
    if ( (status&1) == 0 ) return status;
    dirbuf[length] = '\0';
    /*
     * Make pointers to portion of buffer with translated path and ending
     * location so we can easily append.
     */
    full_name = prologue[3].s;
    tail = &full_name[prologue[3].l];
    /*
     * Initialize structure for buffering output fragments into larger
     * messages (much more efficient) and put connection in CGI mode.
     */
    outbound.ctx = ctx;
    outbound.used = 0;
    out = &outbound;
    mst_write ( ctx, "<DNETCGI>", 9, &lo );	/* separate message */
    /*
     * Check method, must be GET or HEAD.  Set header_only flag if HEAD.
     */
    header_only = 0;
    if ( tu_strncmp ( prologue[1].s, "GET", 4 ) != 0 ) {
	if ( tu_strncmp ( prologue[1].s, "HEAD", 5 ) != 0 ) {
	    put_text ( out, "Status: 404 unsupported method\r\n%s",
	       "Content-type: text/plain\r\n\r\nError, unsupported method\r\n");
	    if ( outbound.used > 0 ) mst_write ( ctx, out->buf, out->used, &lo );
	    mst_write ( ctx, "</DNETCGI>", 10, &lo );
	    status = mst_close ( ctx );
	    return 0;
	}
	header_only = 1;
    }
    /*
     * Search list of welcome files and redirect.
     */
    for ( i = 0; http_index_filename[i][0]; i++ ) {
	void *wf;
	tu_strcpy ( tail, http_index_filename[i] );
	if ( http_log_level > 3 ) 
		tlog_putlog(4,"Searching for '!AZ'!/", full_name );
	wf = tf_open ( full_name, "r", errmsg );
	if ( wf ) {
	    /* Found file, redirect using original path client requested */
	    tf_close ( wf );
	    put_text ( out, "Location: %s%s\r\n\r\n", dirbuf, tail );
	    if ( outbound.used > 0 ) mst_write ( ctx, out->buf, out->used, &lo );
	    mst_write ( ctx, "</DNETCGI>", 10, &lo );
	    status = mst_close ( ctx );
	    return 1;
	}
    }
    /*
     * No welcome file, check restrictions on browsing:
     *    0 - no restrictions.
     *   -1 - Never browse.
     *    1 - Allow browse if control-file present (selective)
     *    2 - Allow browse if control-file present and read control file
     *        as additional config file rules (selective override).
     */
    if ( opt.access ) {
	void *cf;
	/* Look for control file. (selective) add*/
	tu_strnzcpy ( tail, opt.control_file, 80 );
	
	cf = tf_open ( full_name, "r", errmsg );
	if ( !cf ) opt.access = -2;
	else {
	    if ( opt.access > 1 ) htds_get_rules ( cf, &opt );
	    else tf_close ( cf );
	}

	if ( opt.access < 0 ) {
	    /*
	     * Browsing not allowed on this directory.
	     */
	    put_text ( out, "status: 403 Not browsable\r\n" );
	    put_text ( out, "content-type: text/plain\r\n\r\n" );
	    put_text (out, opt.access == -2 ?
		 "Directory not browsable (no %s file present).\r\n" :
		 "Directory browse disabled.\r\n", tail );
	    if ( out->used > 0 )
		status = mst_write ( ctx, out->buf, out->used, &lo );
	    mst_write ( ctx, "</DNETCGI>", 10, &lo );
	    status = mst_close ( ctx );
	    return 1;
	}
    }
    /*
     * Open ident passed to us in prologue in special 'd' mode that
     * performs directory scan.  (reads return multiple filenames
     * separated by nulls)
     */
    *tail = '\0';
    dirf = tf_open ( full_name, "d", errmsg );
    if ( !dirf ) return 0;
    put_text ( out, 
	"status: 200 Directory listing follows\r\ncontent-type: text/html\r\n\r\n" );
    if ( out->used > 0 ) {
	status = mst_write ( ctx, out->buf, out->used, &lo );
	outbound.used = 0;
    }
    if ( header_only ) {
	/* don't return any data */
	mst_write ( ctx, "</DNETCGI>", 10, &lo );
	tf_close ( dirf );
	status = mst_close ( ctx );
	if ( (status&1) == 0 )
        	tlog_putlog ( 0, "!AZ, status of close !SL!/", log_prefix, status );
	return status;
    }
    /*
     * Generate header section of HTML output.
     */
    put_text ( out, "<HTML><HEAD><TITLE>Directory %s</TITLE></HEAD>\r\n",
		dirbuf);
    put_text ( out, "<BODY><DL>" );
    /*
     * Body of document is list.  Check for readme with TOP option.
     */
    if ( opt.readme == 1 ) {
	void *rf;
	tu_strcpy ( tail, opt.readme_file );
	rf = tf_open ( full_name, "r", errmsg );
	if ( rf ) {
	    put_text ( out, "<DT>Description</DT>\r\n<DD><PRE>" );
	    while ( 0 < (length=tf_read(rf,dirbuf,sizeof(dirbuf)-1) ) ) {
		dirbuf[length] = '\0';
		status = put_text ( out, "%t", dirbuf );
		if ( (status&1) == 0 ) return status;
	    }
	    tf_close ( rf );
	    put_text ( out, "</PRE></DD>\r\n" );
	}
    }
    /*
     * Read directory and convert to HTML.
     */
    list_files ( out, dirf, &opt, dirbuf, sizeof(dirbuf), full_name, tail );
    put_text(out, "</DL><HR></BODY></HTML>\r\n");
    /*
     * Body of document is list.  Check for readme with BOTTOM option.
     */
    if ( opt.readme == 2 ) {
	void *rf;
	tu_strcpy ( tail, opt.readme_file );
	rf = tf_open ( full_name, "r", errmsg );
	if ( rf ) {
	    put_text ( out, "<DT>Description</DT>\r\n<DD><PRE>" );
	    while ( 0 < (length=tf_read(rf,dirbuf,sizeof(dirbuf)-1) ) ) {
		dirbuf[length] = '\0';
		status = put_text ( out, "%t", dirbuf );
		if ( (status&1) == 0 ) return status;
	    }
	    tf_close ( rf );
	    put_text ( out, "</PRE></DD>\r\n" );
	}
    }
    /*
     * Flush remaining buffer and mark end of output.
     */
    if ( outbound.used > 0 ) mst_write ( ctx, out->buf, out->used, &lo );
    mst_write ( ctx, "</DNETCGI>", 10, &lo );
    tf_close ( dirf );
    status = mst_close ( ctx );
    if ( (status&1) == 0 )
        tlog_putlog ( 0, "!AZ, status of close !SL!/", log_prefix, status );
    return status;
}
/*************************************************************************/
static int list_files ( struct out_stream *out, void *dirf, dir_opt *opt,
	char dirbuf[4096], int bufsize, char *full_name, char *tail ) 
{
    int need_stat, i, j, k, length;
#ifdef VMS
    stat_t statblk;
#else
    struct stat statblk;
#endif
    /*
     * Make summary flag to indicate whether a stat() call is needed,
     * only incur the disk I/O if needed.
     */
    need_stat = opt->showsize || opt->showdate || opt->showowner || 
		opt->showgroup || opt->showprot;
    /*
     * We assume we are inside DL list, start new topic (files)
     */
    put_text ( out, "<DT>Files</DT>\r\n<DD><PRE>");
    /*
     * Scan directory.
     */
    while ( 0 < (length = tf_read(dirf, dirbuf, bufsize)) ) {
	/*
	 * tf_read returns multiple names in each buffer, process each.
	 */
	for ( i = j = 0; i < length; i++ ) if ( dirbuf[i] == '\0' ) {
	    void *fp;
	    /*
	     * dirbuf[j] is start of filename, skip immediately if
	     * hidden.
	     */
	    if ( (dirbuf[j] == '.') && !opt->showhidden ) {
		j = i + 1;
		continue;
	    }
	    /*
	     * Construct complete filename
	     */
	    tu_strcpy ( tail, &dirbuf[j] );
	    if ( ((i-j) > 4) && (tu_strncmp(&tail[i-j-4],".DIR", 5) == 0) ) {
		/*
		 *  We are a directory, convert '.DIR' to '/' and make
		 * hypertext refernece.
		 */
		tail[i-j-4] = '\0';
	        put_text(out, "<A HREF=\"%s/\">%s/</A>", tail, tail );
	    } else {
	        /*
	         * Make Anchor with relative hypertext reference.
	         */
		int st_sts, size;
		unsigned int uic, cdate, mdate;

	        put_text(out, "<A HREF=\"%s\">%s</A>", tail, tail );
		/*
		 * Only get stat info if we have to.
		 */
		if ( need_stat ) {
		    /*
		     * Extract header information from file.
		     */
		    CONDITIONAL_YIELD
		    LOCK_C_RTL
		    st_sts = stat ( full_name, &statblk );
		    UNLOCK_C_RTL

		    if ( st_sts == 0 ) {
			uic = statblk.st_uid;
		        size = statblk.st_size;
		        cdate = statblk.st_ctime;
		        mdate = statblk.st_mtime;
		    } else uic = size = cdate = mdate = 0;
		} else st_sts = 0;
		/*
		 * If status OK, add optional data if any.
		 */
	        if ( st_sts == 0 ) {
	            char number[20], scratch[32];
		    int k;
		    /*
		     * Format size, either as full byte or kilobytes.
		     */
		    if ( opt->showsize ) {
			if ( opt->showbytes ) {
			    put_text ( out, ", %s bytes",
				    tu_strint(size,number) );
			} else {
			    put_text ( out, ", %s Kb", 
				    tu_strint((size+999)/1000,number) );
			}
		    }
		    /*
		     * Fetch time, ctime() is non-reentrant, so get lock.
		     */
		    if ( opt->showdate ) {
			pthread_lock_global_np();
			tu_strncpy ( scratch, ctime ( (time_t *) &mdate ), 31 );
			pthread_unlock_global_np();
			/* Trim trailing '\n' added by ctime() */
			for (k = 0; scratch[k]; k++) if (scratch[k] == '\n') {
			    scratch[k] = '\0';
			    break;
			}
			put_text ( out, ", %s", scratch );
		    }
		}
	    }
 	    put_text(out,"\r\n");
	    j = i+1;		/* Advance to next name in buffer */
	}
    }
    /* Close up HTML list structures */
    put_text(out, "</PRE></DD>\r\n" );
    return 1;
}
