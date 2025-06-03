/*
 * Handle reading and parsing of rule file.
 *
 *    int http_parse_elements ( int limit, char *line, string *elements );
 *    int http_read_rules ( char *rule_file );
 *    int http_multihomed_hostname ( unsigned long address, access_info acc );
 *
 * Revised:  6-APR-1994			 Added redirect and exec support.
 * Revised: 26-MAY-1994			Added protect rule. and AddType syn.
 * Revised: 27-MAY-1994			Make rule names case insensitive
 * Revised: 2-JUN-1994			Log stats of rule file.
 * Revised: 7-JUN-1994			Added validity checks to read_rule_file
 * Revised: 27-JUN-1994
 * Revised:  2-AUG-1994			Support decnet_searchlist module.
 * Revised: 24-AUG-1994			Define taskname for presentation rules.
 * Revised:  4-NOV-1994			Initialize index file search list.
 * Revised:  5-NOV-1994			Support welcome rule.
 * Revised: 16-NOV-1994			Support DirAccess rule.
 * Revised:  6-JAN-1994			Support scriptname for search.
 * Revised: 12-JAN-1995			Added George Carrette's mods for
 *					hostname, localhost (gjc@village.com).
 * Revised: 25-JAN-1995			Fixed bug in tracelevel rule.
 * Revised: 24-FEB-1995			Support port configuration rule.
 * Revised: 26-FEB-1995			Support usermap directive.
 * Revised: 24-MAR-1995			Added mapimage rule.
 * Revised: 21-APR-1995			Added counter rule.
 * Revised: 11-MAY-1995			Hack for OSF version.
 * Revised: 15-MAY-1995			Conditionally compile counters.
 *					(skip if SCRIPTSERVER defined).
 * Revised:  3-JUN-1995			Add threadpool and service rules
 * Revised: 12-JUN-1995			Support continuation lines.
 * Revised:  15-JUL-1995		Add multihomed hacks.
 */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "access.h"
#include "ident_map.h"
#include "decnet_searchlist.h"
#include "tutil.h"
#ifndef SCRIPTSERVER
#include "tserver_tcp.h"
#include "counters.h"
#endif
/*
 * Work around problems with DECC and forbidden (non-ANSI) names.
 */
#ifdef __DECC
#ifndef fgetname
#define fgetname decc$fgetname
char *fgetname ( FILE *f, char *fname );
#endif
#else
#ifdef VMS
char *fgetname();
#else
#define fgetname(a,b) tu_strcpy(b,"rule_file")
#endif
#endif

#ifdef VMS
#define VMS_FOPEN_OPT , "mbc=64", "dna=.conf"
#else
#define VMS_FOPEN_OPT
#ifndef vaxc$errno
#define vaxc$errno 20
#endif
#endif

typedef struct { int l; char *s; } string;
char *http_search_script;		/* GLobal variable, search enable. */
char **http_index_filename;		/* Filename for index files */
int http_dir_access;
int http_ports[2];			/* TCP/IP port numbers */
char *http_dir_access_file;		/* Directory access check file */
char *http_authenticator_image;		/* Global variable, level 2 access */
char *http_default_host;		/* server name to advertise */
int http_multihomed;			/* true if multihomed */
int http_dns_enable;			/* flag to control host name lookups */
int http_log_level, tlog_putlog();	/* Global variable, logger detail level */

int http_define_suffix(), http_define_presentation(), http_define_ident();
int http_load_dynamic_service();

static int index_file_alloc=0, index_file_count;
static int multihome_count=0;
static struct host_def {
    struct host_def *next;
    char *name;			/* host name to use */
    union {
        unsigned long number;
	unsigned char octet[4];
    } a;
} *multihome_list, *multihome_last;
static char *default_index_file[] = { "index.html", "index.htmlx", (char *) 0 };
/***********************************************************************/
/*
 * Parse line into whitespace-delimited tokens, trimming leading and trailing
 * whitespace.  Each token parse is explicitly null-terminataed.  Function 
 * value returned is number of elements found.  May be 1 more than limit if 
 * last element does not end line.
 */
int http_parse_elements (
	int limit,	    /* Max number of element to delimit */
	char *line,	    /* input line to parse */
	string *elem )	    /* Output array. */
{
    int tcnt, in_token, length, i;
    char *ptr;
    /* Delimit up to three tokens */
    for ( in_token = tcnt = length = 0, ptr = line; *ptr; ptr++ ) {
	if ( in_token ) {
	    if ( isspace ( *ptr ) ) {
		/* End current token */
		*ptr = '\0';  /* terminate string */
		elem[tcnt++].l = length;
		in_token = 0;
	    }
	    else length++;
	} else {
	    if ( !isspace ( *ptr ) ) {
		/* start next token */
		if ( tcnt >= limit ) {
		    /* more tokens than expected */
		    tcnt++;
		    break;
		}
		elem[tcnt].s = ptr;
		in_token = 1;
		length = 1;
	    }
        }
    }
    /*
     * Make final adjust to element count and make remaining elment null.
     */
    if ( in_token ) { elem[tcnt++].l = length; }
    for ( i = tcnt; i < limit; i++ ) { elem[i].l = 0; elem[i].s = ptr; }
    return tcnt;
}
/***********************************************************************/
/* Read next line from nested stack of input file.  Trim line of comments
 * and final linefeed and concatentate continuation lines if
 * last character is a backslash (\).
 */
static char *read_nested 
	( char *line, int linesize, FILE **fp, int *sp, int *lines )
{
    char *result;
    int filled, trim;
    for ( *lines = filled = 0; filled < (linesize-1); ) {
	result = fgets ( &line[filled], linesize-filled, fp[*sp] );
	if ( !result ) {
	    /*
	     * End-of-file.
	     */
	    if ( filled > 0 ) break;		/* return what we got. */
	    if ( *sp == 0 ) return result;	/* final EOF */
	    /*
	     * Resume with previous file.
	     */
	    *lines = 0;
	    fclose ( fp[*sp] );
	    *sp = *sp - 1;
	} else {
	    *lines = *lines + 1;
	    /*
	     * Trim comments and check for continuation line.
	     */
	    for (trim=(-1); line[filled] && (line[filled]!='\n'); filled++) {
		if ( (line[filled] == '#') && (trim < 0) ) trim = filled;
	    }
	    if ( filled > 0 ) if ( line[filled-1] == '\\' )  {
		filled = ( trim < 0 ) ? (filled-1) : trim;
		for ( ; filled > 0; --filled ) 
			if ( !isspace(line[filled-1]) ) break;
		continue;
	    }
	    if ( trim >= 0 ) filled = trim;
	    break;
	}
    }
    line[filled] = '\0';
    return line;
}
/***********************************************************************/
/*  Append filename to list of index files (welcome pages) to search for
 *  when doing directory listings.
 */
static void add_index_file ( char *fname )
{
    /*  Allocate enough for all names plus a null */
    if ( index_file_count+2 >= index_file_alloc ) {
	index_file_alloc += 20;
	if ( index_file_alloc == 20 ) 		/* first call */
	    http_index_filename = (char **) malloc ( sizeof(char *)*index_file_alloc );
	else
	    http_index_filename = (char **) realloc ( http_index_filename, 
		sizeof(char *)*index_file_alloc );

	if ( !http_index_filename ) {
	   tlog_putlog ( 0, "error allocating index file list!/");
	   index_file_alloc = 0;
	   http_index_filename = default_index_file;
	   return;
	}
    }
    /*
     * Copy string and append to list.  Mark end of list with null.
     */
    http_index_filename[index_file_count] = malloc (tu_strlen(fname)+1);
    tu_strcpy ( http_index_filename[index_file_count], fname );
    index_file_count++;
    http_index_filename[index_file_count] = (char *) 0;
}
/***********************************************************************/
/*
 * Initialize rules and load rules database from file.
 */
int http_read_rules ( char *rule_file )
{
    FILE *rf[20];
    string token[8];
    char line[512], tline[512], temp[300], fname[256];
    int length, count, colon, id_count, sfx_count, lnum[20], pass_count, sp;
    int lcnt;
    /*
     * Initialize global variables.
     */
    http_search_script = NULL;
    http_authenticator_image = NULL;
    http_dns_enable = 0;
    http_index_filename = default_index_file;
    http_dir_access = 0;		/* Unrestricted access */
    http_dir_access_file = ".www_browsable";
    /*
     * See if rule file specified.
     */
    if ( !(*rule_file) ) {
	tlog_putlog ( 0, "No rule file specified, abort.!/" );
	return 20;
    } else {
	if ( http_log_level > 1 ) tlog_putlog ( 0,
		"Loading configuration/rules file !AZ!/", rule_file );
	id_count = sfx_count = pass_count = 0;
	sp = lnum[0] = 0;
	rf[0] = fopen ( rule_file, "r" VMS_FOPEN_OPT );
	if ( !rf[0] ) {
	    tlog_putlog ( 0, "Error opening rule file '!AZ'!/", rule_file );
	    return  (vaxc$errno);
	} else while (read_nested ( line, sizeof(line)-1, rf, &sp, &lcnt )) {
	    /*
	     * Trim line of comments and line-feed and parse into tokens.
	     */
	    lnum[sp]+=lcnt;			/* track position in file */
	    for ( length = 0; line[length] /* && (line[length] != '#') &&
			(line[length] != '\n') */; 
			length++ ) tline[length] = line[length];
	    tline[length] = line[length] = '\0';
	    if ( length > 0 ) count = http_parse_elements ( 7, tline, token );
	    else count = 0;
	    /*
	     * Process record based upon first token.
	     */
	    if ( count > 0 ) {
		tu_strupcase ( token[0].s, token[0].s );
		if ( http_log_level > 14 ) tlog_putlog ( 15,
			"Processing rule '!AZ', tokens: !SL!/", token[0].s,
			count );
	    }
	    if ( count <= 0 ) {
		/* Ignore null line */

	    } else if ( (tu_strncmp ( token[0].s, "SUFFIX", 7 ) == 0) ||
			(tu_strncmp ( token[0].s, "ADDTYPE", 8 ) == 0) ) {
		/*
		 * Define attributes of suffix, syntax:
		 *    suffix <sfx> <representation> encoding [qual]
		 */
		if ( count < 3 ) {
		    tlog_putlog ( 0, "Insufficient arguments in suffix command: !AZ!/",
			line );
		}
		http_define_suffix ( token[1].s, token[2].s, token[3].s,
			token[4].s ); sfx_count++;

	    } else if ( tu_strncmp ( token[0].s, "MAP", 7 ) == 0 ) {
		http_define_ident ( token[1].s, 1, token[2].s ); id_count++;

	    } else if ( tu_strncmp ( token[0].s, "PASS", 7 ) == 0 ) {
		http_define_ident ( token[1].s, 2, token[2].s ); 
		id_count++; pass_count++;

	    } else if ( tu_strncmp ( token[0].s, "FAIL", 7 ) == 0 ) {
		http_define_ident ( token[1].s, 3, token[2].s ); id_count++;

	   } else if ( tu_strncmp ( token[0].s, "REDIRECT", 9 ) == 0 ) {
		http_define_ident ( token[1].s, 4, token[2].s ); id_count++;

	    } else if ( tu_strncmp ( token[0].s, "HTBIN", 7 ) == 0 ) {
		/* Make exec entry for compatibility. */
		http_define_ident ( "/htbin/*", 5, token[1].s ); 
		dnetx_define_task ( token[1].s );
		id_count++; pass_count++;

	    } else if ( tu_strncmp ( token[0].s, "EXEC", 7 ) == 0 ) {
		http_define_ident ( token[1].s, 5, token[2].s ); 
		if ( token[2].s[0] != '%' ) dnetx_define_task ( token[2].s );
		id_count++; pass_count++;

	    } else if ( tu_strncmp( token[0].s, "MAPIMAGE", 9) == 0 ) {
#ifndef SCRIPTSERVER
		int length;
		/*
		 * Save argument for fake 'exec' call and parse new line
		 * for creating dynamic service.
		 */
		tu_strcpy ( temp, "%mapimage:" );
		tu_strnzcpy ( &temp[10], token[1].s, sizeof(temp)-11 );
		tu_strcpy ( tline, "SERVICE mapimage builtin=mapimage" );
		count = http_parse_elements ( 7, tline, token );
		if ( 0 == http_load_dynamic_service ( token, count, temp ) ) {
		    tlog_putlog (0, 
			"!AZ, line !SL of rule file !AZ!/", temp,
			lnum[sp], sp ? fgetname(rf[sp],fname) : rule_file );
		}
		else {
		    /* Success */
		    http_define_ident ( "/$mapimage/*", 5, temp );
		}
#endif
	    } else if ( (tu_strncmp ( token[0].s, "DYNAMAP", 8 ) == 0 ) ||
			(tu_strncmp ( token[0].s, "USERMAP", 8 ) == 0 ) ) {
		/* Dynamically defined usermap */
#ifdef SCRIPTSERVER
		int length;
		temp[0] = '(';
		tu_strnzcpy ( &temp[1], token[3].s, 256 );
		length = tu_strlen ( temp );
		temp[length++] = ')';
		tu_strnzcpy ( &temp[length], token[2].s, 299-length );
		http_define_ident ( token[1].s, IDENT_MAP_RULE_USERMAP, temp ); 
		id_count++; pass_count++;
#endif
	    } else if ( tu_strncmp ( token[0].s, "USERDIR", 8) == 0 ) {
		/* Set template to match /~ */
		http_define_ident ( "/~*", 6, token[1].s );
		id_count++; pass_count++;

	    } else if ( tu_strncmp ( token[0].s, "PROTECT", 8) == 0 ) {
		/* level 2 protection check record.  Prepend '+' to setup */
		char l2_setup[512];
		l2_setup[0] = '+';
		tu_strnzcpy ( &l2_setup[1], token[2].s, 500 );
		http_define_ident ( token[1].s, 7, l2_setup ); id_count++;

	    } else if ( tu_strncmp ( token[0].s, "DEFPROT", 8) == 0 ) {
		/* protection check record */
		http_define_ident ( token[1].s, 8, token[2].s ); id_count++;

	    } else if ( tu_strncmp ( token[0].s, "HOSTPROT", 9) == 0 ) {
		/* protection check record */
		http_define_ident ( token[1].s, 7, token[2].s ); id_count++;

	    } else if ( tu_strncmp ( token[0].s, "SEARCH", 7 ) == 0 ) {
		http_search_script = malloc ( token[1].l+1 );
		tu_strnzcpy ( http_search_script, token[1].s, token[1].l );
		dnetx_define_task ( http_search_script );

	    } else if ( tu_strncmp ( token[0].s, "PRESENTATION", 12) == 0 ) {
		http_define_presentation ( token[1].s, token[2].s ); id_count++;
		dnetx_define_task ( token[2].s );

 	    } else if ( tu_strncmp ( token[0].s, "AUTHENTICATOR", 14 ) == 0 ){
		http_authenticator_image = malloc ( token[1].l+1 );
		tu_strnzcpy (http_authenticator_image, token[1].s, token[1].l);

	    } else if ( tu_strncmp ( token[0].s, "INCLUDE", 10 ) == 0 ) {
		if ( count < 2 ) {
		    tlog_putlog ( 0, "Missing filename in Include rule!/" );
		} else if ( sp > 18 ) {
		    tlog_putlog ( 0, "Nesting level too deep to include file!/");
		} else {
		    lnum[++sp] = 0;
		    if ( !(rf[sp] = fopen (token[1].s, "r" VMS_FOPEN_OPT)) ) {
			tlog_putlog ( 0, "Error openning include file '!AZ'!/",
				token[1].s );
			--sp;
		    }
		}	
	    } else if ( tu_strncmp ( token[0].s, "ACCESSLOG", 10 ) == 0 ) {
		int status, tlog_initlog();
		tu_strnzcpy ( temp, token[1].s, token[1].l );
		status = tlog_initlog ( -1, temp );
		if ( (status&1) == 0 ) tlog_putlog ( 0, 
			"Error opening access log file, '!AZ'!/", temp );
		else tlog_putlog ( 0, 
			"Opened access log file, '!AZ', level: !SL!/",
			temp, http_log_level );
	    } else if ( tu_strncmp ( token[0].s, "TRACELEVEL", 10 ) == 0 ) {
		int status, tlog_initlog();
		if ( count < 2 ) {
		    tlog_putlog ( 0, "Missing argument for TraceLevel rule!/" );
		    http_log_level = 1;
		} else {
		    tu_strnzcpy ( temp, token[1].s, token[1].l );
		    if ( (temp[0] < '0') || (temp[0] > '9') ) {
			char *val = getenv(temp);
			if ( !val ) tlog_putlog(0,
			    "Invalid variable name in TraceLevel rule: '!AZ'!/",
			    temp );
			tu_strcpy ( temp, val ? val : "1" );
		    }
		    http_log_level = atoi ( temp );
		}
		tu_strnzcpy ( temp, token[2].s, token[2].l );
		status = tlog_initlog ( http_log_level, temp );
		if ( (status&1) == 0 ) tlog_putlog ( 0, 
			"Error opening trace log file, '!AZ'!/", temp );
		else tlog_putlog ( 0, "Opened trace file, '!AZ', level: !SL!/",
			temp, http_log_level );
	    } else if ( tu_strncmp ( token[0].s, "DNSLOOKUP", 10 ) == 0 ) {
		tu_strupcase ( token[1].s, token[1].s );
		if ( tu_strncmp ( token[1].s, "OFF", 4 ) == 0 ) {
		    http_dns_enable = 0;
		} else if ( tu_strncmp ( token[1].s, "ON", 3 ) == 0 ) {
		    http_dns_enable = 1;
		} else {
		    tlog_putlog (0, 
			"Bad value for DNSLookup directive, line !SL of rule file !AZ!/",
			lnum[sp], sp ? fgetname(rf[sp],fname) : rule_file );
		}
	    } else if ( tu_strncmp ( token[0].s, "WELCOME", 8 ) == 0 ) {
		if ( (index_file_alloc == 0) || (token[1].l > 0) ) {
		    add_index_file ( token[1].s );
		    if ( token[1].l == 0 ) {
			free ( http_index_filename[0] );
			http_index_filename[0] = (char *) 0;
			tlog_putlog (1,"Disabled search for WELCOME files.!/" );
		    }
		} else {
		   tlog_putlog ( 0, 
		   "Missing filename on WELCOME directive, line !SL of rule file !AZ!/",
			lnum[sp], sp ? fgetname(rf[sp],fname) : rule_file );
		}
	    } else if ( tu_strncmp ( token[0].s, "DIRACCESS", 10 ) == 0 ) {
		tu_strupcase ( token[1].s, token[1].s );
		if ( tu_strncmp ( token[1].s, "OFF", 4 ) == 0 ) {
		    http_dir_access = 2;	/* fully restricted (none) */
		} else if ( tu_strncmp ( token[1].s, "ON", 3 ) == 0 ) {
		    http_dir_access = 0;
		} else if ( tu_strncmp ( token[1].s, "SELECTIVE", 10 ) == 0 ) {
		    http_dir_access = 1;
		    if ( token[2].l > 0 ) {
		 	/* Override filename that flags browsable directories */
			http_dir_access_file = malloc ( token[2].l+1 );
			tu_strnzcpy(http_dir_access_file,token[2].s,token[2].l);
		    }
		} else {
		    tlog_putlog (0, 
			"Bad value for DirAccess directive, line !SL of rule file !AZ!/",
			lnum[sp], sp ? fgetname(rf[sp],fname) : rule_file );
		}
	    } else if ( tu_strncmp ( token[0].s, "PORT", 5 ) == 0 ) {
		/* set port numbers */
		int i;
		if ( count < 2 ) {
		    tlog_putlog ( 0, "Missing argument for Port rule!/" );
		} else for ( i = 1; (i < count) && (i < 3); i++ ) {
		    tu_strnzcpy ( temp, token[1].s, token[1].l );
		    if ( (temp[0] < '0') || (temp[0] > '9') ) {
			char *val = getenv(temp);
			if ( !val ) tlog_putlog(0,
			    "Invalid variable name in Port rule: '!AZ'!/",
			    temp );
			tu_strcpy ( temp, val ? val : "1" );
		    }
		    http_ports[i-1] = atoi ( temp );
		}
	    } else if ( tu_strncmp ( token[0].s, "LOCALADDRESS", 13 ) == 0 ) {
#ifndef SCRIPTSERVER
		int status;
		if ( count > 1 ) {
		    /*
		     * Call TCP module to set listen address.
		     */
		    tu_strnzcpy ( temp, token[1].s, token[1].l );
		    status = ts_set_local_addr(temp);
		    if (status == 1 ) tlog_putlog ( 1, 
			"Set local ip address to, '!AZ'!/", temp );
		    else tlog_putlog ( 0, 
			"Error setting local ip address to, '!AZ'!/",
			temp);
		    /*
		     * If hostname present, go into multihomed mode
		     * and being block of address-specific rules.
		     */
		    if ( count > 2 ) {
			/*
			 * Add to list of host defs so we can get
			 * name back from address.
			 */
			struct host_def *new; unsigned long a[4];
			http_multihomed = 1;
			new = (struct host_def *) 
				malloc(sizeof(struct host_def));
			new->next = (struct host_def *) 0;
			new->name = malloc(token[2].l+1);
			tu_strcpy ( new->name, token[2].s );
			/*
			 * Decode 'dot' address.
			 */
			sscanf (temp, "%d.%d.%d.%d", &a[0],&a[1],&a[2],&a[3]);
			new->a.octet[0] = a[0]; new->a.octet[1] = a[1];
			new->a.octet[2] = a[2]; new->a.octet[3] = a[3];
			if ( multihome_count == 0 ) {
			    multihome_list = multihome_last = new;
			} else {
			    multihome_last->next = new;
			    multihome_last = new;
			}
			multihome_count++;
			http_define_ident ( "*", 9, new->name );
		    }
		} else if ( http_multihomed ) {
		    /* End localaddress block. */
		    http_define_ident ( "*", 9, "" );
		}
#endif
	    } else if ( tu_strncmp( token[0].s, "HOSTNAME", 9) == 0 ) {
	        http_default_host = malloc ( token[1].l + 1);
		tu_strnzcpy (http_default_host, token[1].s, token[1].l );
	    } else if ( tu_strncmp( token[0].s, "EVENTCOUNTER", 13) == 0 ) {
#ifndef SCRIPTSERVER
		tu_strupcase ( token[1].s, token[1].s );
		if ( tu_strncmp ( token[1].s, "CLIENTS", 10 ) == 0 ) {
		    http_enable_active_counters();
		    tlog_putlog (1,"Enabled counting of client threads.!/" );
		} else if ( tu_strncmp ( token[1].s, "HOSTCLASS", 10 ) == 0 ) {
		    if ( count >= 4 ) {
			http_define_host_counter ( token[2].s, token[3].s );
		    } else {
		        tlog_putlog ( 0, 
			    "Missing argument(s) for EventCounter rule!/" );
		    }
		} else {
		    tlog_putlog (0, 
			"Bad value for EventCounter directive, line !SL of rule file !AZ!/",
			lnum[sp], sp ? fgetname(rf[sp],fname) : rule_file );
		}
#endif
	    } else if ( tu_strncmp( token[0].s, "THREADPOOL", 13) == 0 ||
		tu_strncmp ( token[0].s, "SERVICE", 9 ) == 0 ) {
#ifndef SCRIPTSERVER
		if ( 0 == http_load_dynamic_service ( token, count, temp ) ) {
		    tlog_putlog (0, 
			"!AZ, line !SL of rule file !AZ!/", temp,
			lnum[sp], sp ? fgetname(rf[sp],fname) : rule_file );
		}
#endif
	    } else {
		tlog_putlog (0,
			"Unrecognized data, line !SL of rule file !AZ: !AZ!/",
			lnum[sp], sp ? fgetname(rf[sp],fname) : rule_file,
			line );
	    }
	}
	if ( http_log_level > 1 ) tlog_putlog ( 1,
		"Rule file has !SL suffix def!%S., !SL translation rule!%S!/",
		sfx_count, id_count );
	while ( sp >= 0 ) fclose ( rf[sp--] );
	if ( pass_count == 0 ) {
	    tlog_putlog ( 0, "Rule file contains no 'pass' actions, !AZ!/",
		"all requests would be _ruled_ out." );
	    return 2160;
	}
    }
    /*
     * Provide default rules if not found in rule file.
     */
    if ( sfx_count == 0 ) {
	tlog_putlog ( 0, "Adding default suffix rules!/" );
	http_define_suffix ( ".gif", "image/gif", "BINARY", "1.0" );
	http_define_suffix ( ".txt", "text/plain", "8BIT", "0.5" );
	http_define_suffix ( ".com", "text/plain", "8BIT", "0.5" );
	http_define_suffix ( ".html", "text/html", "8BIT", "1.0" );
    }
    return 1;
}
/*****************************************************************************/
/* Lookup IP address in multihomed list and set local_address element in
 * access info structure to point to corresponding host name.  If no match,
 * field is set to http_default_host.
 *
 * Return value:
 *	1		Success.
 *	0		Lookup error, acc loaded with default hostname.
 */
int http_multihomed_hostname ( unsigned long address, access_info acc )
{
    struct host_def *blk;
    if ( multihome_count > 0 ) {
	for ( blk = multihome_list; blk; blk = blk->next ) {
	    if ( blk->a.number == address ) {
		acc->local_address = blk->name;
		return 1;
	    }
	}
    }
    acc->local_address = http_default_host;	/* not found */
    return 0;
}
