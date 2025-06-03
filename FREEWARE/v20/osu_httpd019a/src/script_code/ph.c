/*
 * This program provides form-based access to a PH/QI server.  The
 * hostname of the ph server is specified as the path info.
 *
 * If no form data is available, we retrieve the field descriptions for
 * the target ph database and generate an HTML form that allows the user
 * to make a query.  If you plan to use this form often, you should save
 * it a file and link to it rather than regenerate it every time (also
 * allows you to customize the form).
 *
 * If form data is present in the request, we parse it and construct a ph
 * query command based on the input supplied, which is sent to the ph server.
 * The result of the query is formatted as HTML and returned to the client.
 *
 * The DECthreads HTTP server's cgilib/scriptlib routines are used to
 * interact with the server script environment.
 * Socket routines are used to make the communicate with the ph server.
 *
 * Author:	David Jones.
 * Date:	18-MAR-1995
 * Revised:	22-MAR-1995	Redo of generate_form(), now driven by
 *				template in character string array.
 * Revised:	31-MAR-1995	Miscellaneous
 * Revised:	 4-APR-1995	Fix infinte loop in $(NAMEFLD) search.
 * Revised:	 4-MAY-1995	Add Bruce Tanner's hack for url field
 *				attribute.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unixio.h>
#include <string.h>
#include <types.h>
#include <errno.h>
#include <ctype.h>
#include <socket.h>
#include <in.h>
#include <netdb.h>
#define QI_PORT 105

#include "cgilib.h"
#include "scriptlib.h"
void error_abort();
static int open_remote ( char *, int ), load_fields (int, char *);
static int build_query ( int, char *, int, char *, int, char *, int );
static char *next_line ( int, int *length );

struct field_info {
    struct field_info *next;
    char *name, *attributes, *description;
    int id;
    int lookup, indexed;
    int max_size;
    int defreturn, explicit_return, reserved;
    int public, url;
    char name_buf[16];		/* Avoid extra malloc if we can */
    char desc_buf[32];
    char attr_buf[80];		/* save on malloc if possible */
};

static struct field_info *fields;
static char *extra_info;
struct formgen_context {
    char *host, *seek;
    struct field_info *fld;
    int port, cur_line, cur_off, rep_line, rep_off;
    int public_override;
    int field_select;
};
static int interpret_key ( char *key, char *buffer, int *length,
	struct formgen_context *ctx );
/******************************************************************************/
/* Main program, command line arguments:
 *    argv[1]	Method in HTTP request.
 *    argv[2]	Ident portion of URL requested.
 *    argv[3]	HTTP protocol version.
 */
int main ( int argc, char **argv )
{
    int status, i, length, port, qi, content_length, clen;
    char *query, *form, *path_info, *cp, *content_length_str;
    char host[256], message[256], command[4096];
    /*
     * Setup CGI emulation and extract hostname/port from path info.
     */
    status = cgi_init ( argc, argv );
    if ( (status&1) == 0 ) exit ( status );
    path_info = cgi_info ( "PATH_INFO" );
    extra_info = "";
    if ( !path_info ) error_abort ( "500 CGI bug", 
	"Missing PATH_INFO (ph server host)" );

    port = QI_PORT;
    cp = (*path_info == '/') ? path_info+1 : path_info;
    for ( i = 0; i < sizeof(host)-1; i++ ) {
	host[i] = *cp++;
	if ( !host[i] ) break;		/* end of string */
	else if ( host[i] == '/' ) {
	    /*
	     * Interpret data following host as port number
	     */
	    extra_info = malloc ( strlen(&host[i+1]) + 1 );
	    strcpy ( extra_info, &host[i+1] );
	    break;
	}
    }
    if ( i == 0 ) {
	error_abort ( "500 missing host", 
		"Ph server host name must be supplied as path info." );
	strcpy ( host, cgi_info("SERVER_NAME") );
    }
    else host[i] = '\0';
    /*
     * Connect to PH server and get its fields.
     */
    qi = open_remote ( host, port );
    if ( qi < 0 ) return 0;
    status = load_fields ( qi, message );
    if ( (status&1) == 0 ) error_abort ( "500 server error", message );
    /*
     * Determine if we have post data or GET query data
     */
    content_length_str = cgi_info ( "CONTENT_LENGTH" );
    content_length = atoi ( content_length_str );
    query = (char *) 0;
    if ( content_length <= 0 ) {
	/* See if query string present */
	query = cgi_info ( "QUERY_STRING" );
	if ( query ) {
	    if ( !*query ) query = (char *) 0;
	}
    }
    if ( content_length <= 0 && !query ) {
	/*
	 * No input data generate form only and return.
         */
	int generate_form();
        cgi_printf ( "content-type: text/html\n\n<HTML>\n" );
	return generate_form ( 0, qi, host, port );
    }
    /*
     * Read form data or synthesize form data from query string
     * (allows testing via fetch_http.exe).
     */
    if ( query ) {
	/*
	 * Users doing GET, synthesize form data.
	 */
	form = malloc ( strlen(query) + 30 );
	if ( !form ) error_abort ( "500 malloc error", "Error reading form" );
	sprintf(form, "%s", query );
	content_length = strlen ( form );
	/* printf("form string: %s\n", form ); */
    } else {
	/*
	 * Decode form data to make query.
	 */
	form = malloc ( content_length+1 );
	if ( !form ) error_abort ( "500 malloc error", "Error reading form" );
	for ( i = 0; i < content_length; i += length ) {
	    length = cgi_read ( &form[i], content_length - i );
	    if ( length < 0 ) break;
	}
    }
    /*
     * Convert form to PH command and send.
     */
    cgi_printf ( "content-type: text/html\n\n<HTML><HEAD>\n" );
    cgi_printf ( "<TITLE>ph results</TITLE>\n</HEAD>\n<BODY>\n");
    clen = build_query ( qi, form, content_length, command, sizeof(command),
	host, port );
    if ( clen > 0 ) {
	command[clen++] = '\n';
	length = write ( qi, command, clen );
	if ( length == clen ) {
	    void display_result();
	    display_result ( qi, command );
	} else {
	    cgi_printf ( "Error, write to ph server failed\n");
	}
        cgi_printf ( "</BODY>\n</HTML>\n" );
    }
    return 1;
}
/*****************************************************************************/
static int parse_field_info ( struct field_info *blk, char *msg )
{
    int i;
    char *info, *max_spec;
    /*
     * initialize all fields to default values.  
     */
    blk->indexed = blk->lookup = blk->reserved = blk->max_size = 0;
    blk->defreturn = blk->explicit_return = blk->public = blk->url = 0;
    /*
     * Search for keywords in info string and set values.  Attributes
     * are converted to all lower-case for comparison.
     */
    info = blk->attributes;
    for ( i = 0; info[i]; i++ ) info[i] = _tolower(info[i]);
    if ( strstr ( info, "indexed " ) ) blk->indexed = 1;
    if ( strstr ( info, "default " ) ) blk->defreturn = 1;
    if ( strstr ( info, "public " ) ) blk->public = 1;
    if ( strstr ( info, "lookup " ) ) blk->lookup = 1;
    if ( strstr ( info, "url " ) ) blk->url = 1;
    max_spec = strstr ( info, "max " );
    if ( max_spec ) {
	sscanf ( &max_spec[4], "%d", &blk->max_size );
    } else blk->max_size = 32;
    return 1;
}
/*****************************************************************************/
/* Query PH server for its field descriptions and build global list.
 */
int load_fields ( int qi, char *message )
{
    int i, length, code, prev_code, alen;
    char *bufp, *status_code, *index, *name;
    struct field_info *last, *new;
    /*
     * Ask server for field list.
     */
    length = write ( qi, "fields\n", 7 );
    if ( length != 7 ) {
	strcpy ( message, "Write error to ph server\n");
	return 0;
    }
    last = fields = (struct field_info *) 0;
    /*
     * Read results and make form fields.
     */
    prev_code = -2555;
    for ( bufp = "-"; *bufp == '-'; ) {
	bufp = next_line ( qi, &length );
	if ( length < 0 ) {
	    strcpy ( message, "ERROR: contact with ph server lost");
	    return 0;
	}
	/*
         * Check status, ignore any non-success.
         */
	status_code = (*bufp == '-') ? &bufp[1] : bufp;
	if ( *status_code != '2' ) continue;
	/*
	 * Parse fields withing returned line into status, ndx, name, data.
	 */
	index = name = (char *) 0;
	for ( i = 0; i < length; i++ ) if ( bufp[i] == ':' ) {
	    bufp[i] = '\0';
	    if ( !index ) {
		index = &bufp[i+1];
		code = atoi ( index );
	    } else if ( !name ) {
		name = &bufp[i+1];
	    } else { i++; break; }
        }
	/*
	 * Add data to field structure.
	 */
	if ( name ) {
	    if ( code == prev_code ) {
		/*
		 * Remaining data is description, save in curren info block.
		 */
		alen = strlen(&bufp[i]) + 1;
		if ( alen > sizeof(last->desc_buf) ) {
		    if ( !(new->description = malloc ( alen )) ) {
			strcpy(message,"malloc error"); return 0;
		    }
		}
		strcpy ( last->description, &bufp[i] );
	    } else {
		/*
		 * Initialize new block, append to end of list to preserv order
		 */
		new = malloc ( sizeof(struct field_info) );
		if ( !new ) { strcpy(message,"malloc error"); return 0; }
		if ( last ) last->next = new; else fields = new;
		last = new;

		new->next = (struct field_info *) 0;
		new->name = new->name_buf;
		alen = strlen(name) + 1;
		if ( alen > sizeof(new->name_buf) ) {
		    if ( !(new->name = malloc ( alen )) ) {
			strcpy(message,"malloc error"); return 0;
		    }
		}
		strcpy ( new->name, name );

		new->attributes = new->attr_buf;
		alen = strlen(&bufp[i]) + 2;
		if ( alen > sizeof(new->attr_buf) ) {
		    if ( !(new->attributes = malloc ( alen )) ) {
			strcpy(message,"malloc error"); return 0;
		    }
		}
		strcpy ( new->attributes, &bufp[i] );
		strcpy ( &new->attributes[alen-2], " " );
		new->description = new->desc_buf; 
		new->desc_buf[0] = '\0';
		new->id = atoi ( index );
		/*
		 * Scan for keywords.
		 */
		parse_field_info ( new, message );
	    }
	    prev_code = code;
	} else break;
    }
    return 1;
}
/*****************************************************************************/
/* Query PH server and generate HTML form for client may use to query the
 * server for information.  The form fields are given names of the form q_nn 
 * for query fields and r=nn for return fields where nn is the index number 
 * for the field.
 */
int generate_form ( int full_flag, int qi, char * host, int port )
{
    int i, j, length, code, out, prev_code;
    char *bufp, *key, *line, *status_code, *index, *name;
    char buffer[2048];
    struct formgen_context ctx;
    static char *template[] = {
   "<HEAD>\n<TITLE>Ph server on $(HOST)</TITLE>\n</HEAD>\n<BODY>",
   "<H2>Ph query, form for $(HOST)</H2>\n",
   "To search the database for a name, fill in one or more of the fields in",
   "the form below in activate the 'make query' button,  At least one of",
   "the entered fields must be flagged as indexed.",
   "<HR><FORM method=\"POST\" action=\"$(SCRIPT_PATH)\">",
   "<input type=\"submit\" value=\" make query \"> ",
   "<input type=\"reset\" value=\" clear fields \">",
   "<P><DL>",
   "  <DT>Search parameters (* indicates indexed field):</DT>",
   "  <DD>", "$(NAMEFLD)    <DL>\n      <DT>$(FDESC)$(FNDX)</DT>",
   "     <DD>Last: <input name=\"q_$(FID)\" type=\"text\" size=30$(FSIZE2)>",
   "     First: <input name=\"q_$(FID)\" type=\"text\" size=20$(FSIZE2)></DD>",
   "$(QFIELDS)      <DT>$(FDESC)$(FNDX)</DT>",
   "      <DD><input name=\"q_$(FID)\" type=\"text\" $(FSIZE)></DD>\n$(NEXTFLD)",
   "    </DL>",
   "  </DD></DL><P>",
   "<DL><DT>Output format:</DT>",
   "    <DD>Returned data option: <select name=\"return\"><option selected>default",
   "     <option>all<option>selected<option>full query form</select><BR>",
   "$(RFIELDS)      <input type=\"checkbox\" name=\"r_$(FID)\"$(FDEF)>$(FDESC)<BR>",
   "$(NEXTFLD)    </DD>",
   "  </DL>",
   "</DL></FORM><HR>",
   "</BODY></HTML>",
   (char *) 0
     };

    out = 0;
    ctx.host = host;
    ctx.seek = (char *) 0;
    ctx.port = port;
    ctx.fld = (struct field_info *) 0;
    ctx.public_override = full_flag;
    /*
     * Parse the strings in the template array to produce HTML document
     * to send to client.  First line is skipped for 'full' lists.
     */
    out = 0; buffer[out] = '\0';
    for ( i = full_flag ? 1 : 0; template[i]; i++ ) {
	/*
	 * Search the current string for substitution, flagged by $(
	 */
	for ( line=template[i], j = 0; line[j]; j++ ) {
	    if ( (line[j] == '$') && (line[j+1] == '(') ) {
		/*
		 * Command detected, flush output buffer and find closing ')'
		 * that delimits the command.
		 */
		buffer[out] = '\0'; 
		if ( out > 0 ) cgi_printf("%s", buffer); 
	        out = 0;
		for ( key = &line[j]; line[j+1] && (line[j] != ')'); j++ );
		/*
		 * Save context, interpet command and restore updated context.
		 */
		ctx.cur_line = i; ctx.cur_off = j;
		interpret_key ( key, buffer, &length, &ctx );
		i = ctx.cur_line; j = ctx.cur_off;
		line = template[i];
		out = length;

		if ( ctx.seek ) {
		    /*
		     * command wants us to skip (forward) to indicated token.
		     * Start at current position.
		     */
		    int slen = strlen ( ctx.seek );
		    for ( ; template[i]; i++ ) {
			for ( line = template[i]; line[j]; j++ ) {
			    if ( line[j] == '$' ) 
				if ( 0 == strncmp(ctx.seek, &line[j], slen) ) {
				    if ( j == 0 ) j = strlen(template[--i])-1;
				    else --j;
				    line = template[i];
				    ctx.seek = (char *) 0; break;
			        }
			}
			if ( !ctx.seek ) break;
			j = 0;
		    }
		    if ( ctx.seek ) cgi_printf("Seek fail on %s\n", ctx.seek );
		}
	    } else {
		/*
		 * Non-command text, add to output buffer.
		 */
		buffer[out++] = line[j];
		if ( out > (sizeof(buffer)-3) ) {
		    buffer[out] = '\0';
		    cgi_printf ( "%s", buffer );
		    out = 0;
		}
	    }
	}
	buffer[out++] = '\n';
	buffer[out] = '\0';
    }
    if ( out > 0 ) cgi_printf ( "%s", buffer );

    return 1;
}
/*****************************************************************************/
static int interpret_key ( char *key, char *buffer, int *length,
	struct formgen_context *ctx )
{
    struct field_info *fld;

    if ( fld = ctx->fld ) {
	/*
	 * Most substitutions only recongined inside of loops.
	 */
	int error = 0;
	if ( 0 == strncmp (key, "$(FID)", 6) ) {
	    sprintf ( buffer, "%d", fld->id ); 
        } else if ( (0 == strncmp (key, "$(FDESC)", 8)) ) {
	    sprintf ( buffer, "%s%s%s", fld->description,
		ctx->public_override ? " " : "",
		ctx->public_override ? fld->attributes : "" ); 
        } else if ( (0 == strncmp (key, "$(FDEF)", 7)) ) {
            strcpy ( buffer, fld->defreturn ? " checked" : "" );
        } else if ( (0 == strncmp (key, "$(FNDX)", 7)) ) {
	    strcpy ( buffer, fld->indexed ? "*" : "" );
        } else if ( (0 == strncmp (key, "$(FSIZE)", 8)) ) {
	    sprintf ( buffer, " size=%d maxlength=%d",
		fld->max_size > 60 ? 60 : fld->max_size, fld->max_size);
        } else if ( (0 == strncmp (key, "$(FSIZE2)", 9)) ) {
	    sprintf ( buffer, " maxlength=%d", fld->max_size );
        } else {
	    error = 1;
	}
	if ( !error ) { *length = strlen(buffer); return 1; }
    }
    /*
     */
    buffer[0] = '\0';
    if ( 0 == strncmp (key, "$(NEXTFLD)", 10 ) ) {
	if ( !ctx->fld ) fld = fields; else fld = ctx->fld->next;
	switch ( ctx->field_select ) {
	  case 0:
	    /* 'Query' fields, public and lookup attributes */
	    for ( ; fld; fld = fld->next )
		 if (fld->public && (fld->lookup==1)) break;
	    break;
	  case 1:
	    /* 'Query' fields, accept lookup attribute */
	    for ( ; fld; fld = fld->next ) if ( fld->lookup == 1 ) break;
	    break;
	  case 2:
	    /* 'Return' fields, public only */
	    for ( ; fld; fld = fld->next ) if ( fld->public ) break;
	    break;
	  case 3:
	    /* all fields */
	    break;
	}
	if ( fld ) { 
	    ctx->cur_line = ctx->rep_line;
	    ctx->cur_off = ctx->rep_off;
	}
	ctx->fld = fld;

    } else if ( (0 == strncmp (key, "$(QFIELDS)", 10)) ||
		(0 == strncmp (key, "$(RFIELDS)", 10)) ) {
	/*
	 * Begin interation sequence.
	 */
	ctx->rep_line = ctx->cur_line;
	ctx->rep_off = ctx->cur_off;
	ctx->fld = (struct field_info *) 0;
	ctx->seek = "$(NEXTFLD)";
	ctx->field_select = (key[2] == 'Q') ? 0 : 2;
	if ( ctx->public_override ) ctx->field_select++;

    } else if ( 0 == strncmp (key, "$(NAMEFLD)", 10) ) {
	/*
	 * Special, locate name field.  Flag lookup so QFIELDS will skip it.
	 */
	for ( fld = fields; fld; fld = fld->next )
	    if ( strcmp (fld->name, "name" ) == 0 ) { 
		if ( fld->lookup ) fld->lookup = 2; 
		break;
	    }
	ctx->fld = fld;
    } else if ( 0 == strncmp (key, "$(HOST)", 7) ) {
	strcpy ( buffer, ctx->host ); 
    } else if ( 0 == strncmp (key, "$(SCRIPT_PATH)", 14) ) {
	sprintf ( buffer, "%s%s/%d", cgi_info("SCRIPT_NAME"), 
		cgi_info("PATH_INFO"), ctx->port );
    } else {
	/*
	 * No match, dump key to buffer so client sees it for debugging.
	 */
	int out = 0;
	while ( *key && (*key != ')') ) {
	    buffer[out++] = (*key++);
	    if ( out > sizeof(buffer)-2 ) { buffer[out] = '\0';
		cgi_printf("%s", buffer); out= 0;
	    }
	}
	buffer[out++] = ')'; buffer[out] = '\0';
    }
    *length = strlen ( buffer );
    return 1;
}
/*****************************************************************************/
void error_abort ( char *status_line, char *message )
{
    cgi_printf ( "content-type: text/plain\nstatus: %s\n\n%s\n",
	status_line, message );
    exit(1);
}
/*****************************************************************************/
/* Read line of data and return pointer to it.
 */
static char linebuf[8192];
static int linestate, linefilled, lineused;
static char *next_line ( int socket, int *length )
{
    char cur;
    int i, start;
    /*
     * Scan buffer for next line beginning where we left off.
     */
#ifdef DEBUG
printf("next_line state: %d, used: %d, filled %d\n", linestate,
lineused, linefilled );
#endif
    for ( start = lineused; linestate >= 0; ) {
        /*
         * search for end-of-line, trimming CR if it precedes the LF.
         */
        for ( i = start; i < linefilled; i++ ) {
	    cur = linebuf[i];
	    if ( cur == '\n' ) {
		/* Found EOL */
	        *length = i - start - linestate;
	        lineused = i + 1;
	        linebuf[i-linestate] = '\0';
	        linestate = 0;
	        return &linebuf[start];
	    } else if ( cur == '\r' ) {
	        linestate = 1;
	    } else if ( linestate ) linestate = 0;
        }
        /*
         * Getting here means no EOL found.  Scoot unused portion to beginning of
         * buffer.
         */
        if ( start > 0 ) {
	    /* No more room in  buffer */
	    for ( lineused = 0; start < linefilled; ) 
		linebuf[lineused++] = linebuf[start++];
	    start = 0;
	}
	/*
	 * Read more data into buffer
	 */
        i = read ( socket, &linebuf[lineused], sizeof(linebuf)-lineused-1 );
#ifdef DEBUG
printf("read %d into buffer at position %d\n", i, lineused );
#endif
        if ( i <= 0 ) {
	    *length = lineused;
	    linestate = 0;
	    return linebuf;
	}
	linefilled = lineused + i;
    }
    /*
     * Getting here means connection broken.
     */
    *length = -1;
    linebuf[0] = '\0';
    return linebuf;
}
/*****************************************************************************/
/* Create socket and connect to remote host.
 */
static int open_remote ( char *remhost, int port )
{
    struct sockaddr local, remote;
    struct hostent *hostinfo;
    int i, j, rem_port, sd, status;
    char *alt_port;
    /* */
    linestate = -1;
    linefilled = 0;
    lineused = 0;
    rem_port = port;
printf("remote host: %s, port: %d\n", remhost, port );
    alt_port = strchr ( remhost, ':' );
    if ( alt_port ) {
	*alt_port++ = '\0';
	rem_port = atoi ( alt_port );
    }
    hostinfo = gethostbyname ( remhost );
    if ( !hostinfo ) {
	error_abort ( "503 Connect error", "Could not find host address" );
	return -1;
    }
    /*
     * Attempt connect to remote host.
     */
    sd = socket ( AF_INET, SOCK_STREAM, 0 );
    if ( sd < 0 ) {
	    fprintf(stderr,"Error creating socket: %s\n", strerror(errno) );
	return sd;
    }
    local.sa_family = AF_INET;
    for ( j = 0; j < sizeof(local.sa_data); j++ ) local.sa_data[j] = '\0';
    local.sa_data[0] = local.sa_data[1] = 0;
    status = bind ( sd, &local, sizeof ( local ) );
    if ( status < 0 ) {
	error_abort ( "503 Connect error", "Could not bind socket" );
	return -1;
    }

    remote.sa_family = hostinfo->h_addrtype;
    for ( j = 0; j < hostinfo->h_length; j++ ) {
	    remote.sa_data[j+2] = hostinfo->h_addr_list[0][j];
    }
    remote.sa_data[0] = rem_port >> 8;
    remote.sa_data[1] = (rem_port&255);
    status = connect ( sd, &remote, sizeof(remote) );
    if ( status != 0 ) {
	error_abort ( "503 Connect error", "Could not connect to host" );
	return -1;
    }
    linestate = 0;		/* no CR pending */
    linefilled = 0;
    lineused = 0;
    return sd;
}
/*************************************************************************/
/* Interpret contents for form data string and construct ph query command.
 * the fields array and return_type variable are also set.
 */
int build_query ( int qi, char *fdata, int content_length, char *command,
	int command_max, char *host, int port )
{
    int length, i, j, start, finish, flen, ndx, clen;
    int return_type, has_indexed;
    struct field_info *fld;

    return_type = 0;
    has_indexed = 0;
    length = content_length;
    if ( fdata[length-1] != '&' ) fdata[length++] = '&';  /* force termination*/
    start = finish = clen = 0;
    for ( i = 0; i < length; i++ ) if ( !fdata[i] || (fdata[i] == '&') ) {
	/*
	 * Value parsed.  Unescape characters and look for first '='
	 * to delimit field name from value.
	 */
	flen = i - start;
	finish = start + flen;
	for ( j = start; j < finish; j++ )
	    if ( fdata[j] == '+' ) fdata[j] = ' ';
	net_unescape_string ( &fdata[start], &flen );
	finish = start + flen;
	fdata[finish] = '\0';
#ifdef DEBUG
cgi_printf("field: %s<BR>\n", &fdata[start] );
#endif
	for ( j = start; j < finish; j++ ) if ( fdata[j] == '=' ) {
	    /*
	     * fdata[start..j-1] is field name, [j+1..finish-1] is value.
	     */
	    if ( (fdata[start+1] == '_') && ((fdata[start] == 'r') || 
		(fdata[start] == 'q')) ) {
		/*
		 * decode fields number and lookup field info.
		 */
		sscanf ( &fdata[start+2], "%d=", &ndx );
		for (fld = fields; fld; fld = fld->next) if ( ndx==fld->id ) {
		    if ( (j+1) >= finish ) break;	/* ignore nulls */
		    if ( fdata[start] == 'q' ) {
			/*
			 * Append field to query line.
			 */
			if ( fld->lookup ) {
			    if ( fld->indexed ) has_indexed = 1;
			    if ( clen == 0 ) {
				    strcpy ( command, "query " ); clen = 6;
			    } else command[clen++] = ' ';

			    sprintf(&command[clen], "%s=\"%s\"",
				    fld->name, &fdata[j+1] );
			    clen += strlen ( &command[clen] );
			} else {
			    cgi_printf("Warning: non-lookup field ignored<BR>\n");
			}
		    } else if ( fdata[start]== 'r' ) {
			fld->explicit_return = 1;
		    
		    }
		    break;
		}
	    } else if ( strncmp(&fdata[start],"return=",7)==0 ) {
		if ( strcmp(&fdata[start+7],"all") == 0 ) return_type = 1;
		else if ( strcmp(&fdata[start+7],"selected") == 0 )
		   return_type = 2;
		else if ( strncmp(&fdata[start+7],"full",4) == 0 ) {
		    /* Client asking for full form. */
		    generate_form ( 1, qi, host, port );
		    return 0;
		}
	    }

	}
	start = i + 1;
    }
    if ( (clen == 0) || !has_indexed ) {
	cgi_printf ( "Error, at least one indexed field value must be specified<BR>\n");
	return 0;
    }
    /*
     * Append return fields.
     */
    if ( return_type == 1 ) strcpy ( &command[clen], " return all" );
    else if ( return_type == 2 ) {
	strcpy ( &command[clen]," return" ); clen += 7;
	for ( fld = fields; fld; fld = fld->next ) if ( fld->explicit_return ) {
	    command[clen++] = ' ';
	    strcpy ( &command[clen], fld->name );
	    clen += strlen ( &command[clen] );
	}
    }
    cgi_printf("ph command: '%s'<BR>\n", command, clen );
    return clen;
}
/****************************************************************************/
/* Read ph server response to query and format in html.
 */
void display_result ( int qi )
{
    int length, i, j, prev_ndx, ndx;
    char *bufp, *rcode, *ndx_str, *fname, *fvalue, buffer[1024];
    struct field_info *fld;
    /*
     * Read lines until non-negative status.
     */
    prev_ndx = -100;
    for ( bufp = "-"; (*bufp == '-') || (*bufp == '1'); ) {
	/*
	 * Get next status line.
	 */
	bufp = next_line ( qi, &length );
	if ( length < 0 ) {
	    cgi_printf ( "ERROR: contact with ph server lost" );
	    break;
	}
	/*
	 * Parse line.
	 */
	rcode = (*bufp == '-') ? &bufp[1] : bufp;
	ndx_str = fname = (char *) 0;
	for ( i = 0; i < length; i++ ) if ( bufp[i] == ':' ) {
	    bufp[i] = '\0';
	    if ( !ndx_str ) { fname = ndx_str = &bufp[i+1]; }
	    else { fname = &bufp[i+1]; break; }
	}
	if ( ndx_str ) {
	    ndx = atoi ( ndx_str );
	    if ( prev_ndx != ndx ) {
		if ( prev_ndx != -100 ) cgi_printf ( "</DL></DD></DL>\n" );
		if ( ndx == 0 ) {
		     cgi_printf ( 
			"<HR><DL><DT>Information/status</DT><DD><DL>\n" );
		    
		} else {
		     cgi_printf ( "<HR><DL><DT>Entry %d:</DT><DD><DL>\n", ndx );
		}
		prev_ndx = ndx ;
	    }
	} else { cgi_printf ( "%s<BR>\n", rcode ); continue; }

	if ( (*rcode >= '2') && (*rcode <= '5') && (fname != ndx_str) ) {
	    while ( *fname == ' ' ) fname++;	/* trim leading spaces */
	    for ( fvalue = fname; *fvalue; fvalue++ ) if (*fvalue == ':') {
		*fvalue++ = '\0'; break;
	    } else if ( *fvalue == ' ' ) *fvalue = '\0'; /* trim trailing */

	    if ( *fname ) {
		for ( fld = fields; fld; fld = fld->next ) {
		    if ( 0 == strcmp ( fld->name, fname ) ) {
			if ( fld->description ) fname = fld->description;
			break;
		    }
		}
		if ( fld->url ) {
		    int ns;
		    for ( ns = 0; isspace(fvalue[ns]); ns++ );
	            cgi_printf ( "<DT><A HREF=\"%s\">%s</A></DT><BR>\n", 
			&fvalue[ns], fname );
		}
		else
	            cgi_printf ( "<DT>%s</DT><DD>%s<BR>\n", fname, fvalue );
	    } else
		cgi_printf ( "%s<BR>\n", fvalue );
	} else cgi_printf("%s<BR>\n", fname ? fname : rcode );
    }
    if ( prev_ndx != -100 ) cgi_printf ( "</DL></DD></DL>\n" );
}
