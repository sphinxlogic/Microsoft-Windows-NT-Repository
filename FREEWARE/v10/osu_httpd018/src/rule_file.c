/*
 * Handle reading and parsing of rule file.
 *
 *    int http_parse_elements ( int limit, char *line, string *elements );
 *    int http_read_rules ( char *rule_file );
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
 */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "access.h"
#include "ident_map.h"
#include "decnet_searchlist.h"
#include "tutil.h"
#include "tserver_tcp.h"
/*
 * Work around problems with DECC and forbidden (non-ANSI) names.
 */
#ifdef __DECC
#ifndef fgetname
#define fgetname decc$fgetname
char *fgetname ( FILE *f, char *fname );
#endif
#else
char *fgetname();
#endif
#ifdef VMS
#define VMS_FOPEN_OPT , "mbc=64", "dna=.conf"
#else
#define VMS_FOPEN_OPT
#endif

typedef struct { int l; char *s; } string;
char *http_search_script;		/* GLobal variable, search enable. */
char **http_index_filename;		/* Filename for index files */
int http_dir_access;
int http_ports[2];			/* TCP/IP port numbers */
char *http_dir_access_file;		/* Directory access check file */
char *http_authenticator_image;		/* Global variable, level 2 access */
char *http_default_host;		/* server name to advertise */
int http_dns_enable;			/* flag to control host name lookups */
int http_log_level, tlog_putlog();	/* Global variable, logger detail level */

int http_define_suffix(), http_define_presentation(), http_define_ident();

static int index_file_alloc=0, index_file_count;
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
/* Read next line from nested stack of input file.
 */
static char *read_nested ( char *line, int linesize, FILE **fp, int *sp )
{
    char *result;
    for ( ; ; ) {
	result = fgets ( line, linesize, fp[*sp] );
	if ( result || (*sp == 0) ) return result;
	fclose ( fp[*sp] );
	*sp = *sp - 1;
    }
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
    char line[512], tline[512], temp[300];
    int length, count, colon, id_count, sfx_count, lnum[20], pass_count, sp;
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
	} else while ( read_nested ( line, sizeof(line)-1, rf, &sp ) ) {
	    /*
	     * Trim line of comments and line-feed and parse into tokens.
	     */
	    lnum[sp]++;			/* track position in file */
	    for ( length = 0; (line[length] != '#') && line[length] &&
			(line[length] != '\n'); 
			length++ ) tline[length] = line[length];
	    tline[length] = line[length] = '\0';
	    if ( length > 0 ) count = http_parse_elements ( 7, tline, token );
	    else count = 0;
	    /*
	     * Process record based upon first token.
	     */
	    if ( count > 0 ) {
		tu_strupcase ( token[0].s, token[0].s );
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
		dnetx_define_task ( token[2].s );
		id_count++; pass_count++;

	    } else if ( (tu_strncmp ( token[0].s, "DYNAMAP", 8 ) == 0 ) ||
			(tu_strncmp ( token[0].s, "USERMAP", 8 ) == 0 ) ) {
		/* Dynamically defined usermap */
		int length;
		temp[0] = '(';
		tu_strnzcpy ( &temp[1], token[3].s, 256 );
		length = tu_strlen ( temp );
		temp[length++] = ')';
		tu_strnzcpy ( &temp[length], token[2].s, 299-length );
		http_define_ident ( token[1].s, IDENT_MAP_RULE_USERMAP, temp ); 
		id_count++; pass_count++;
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
		    char fname[256];
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
		    char fname[256];
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
		    char fname[256];
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
		int status;
		tu_strnzcpy ( temp, token[1].s, token[1].l );
		status = ts_set_local_addr(temp);
		if (status == 1 ) tlog_putlog ( 1, 
			"Set local ip address to, '!AZ'!/", temp );
		else tlog_putlog ( 0, 
			"Error setting local ip address to, '!AZ'!/",
			temp);
	    } else if ( tu_strncmp( token[0].s, "HOSTNAME", 9) == 0 ) {
	        http_default_host = malloc ( token[1].l + 1);
		tu_strnzcpy (http_default_host, token[1].s, token[1].l );
	    } else {
		char fname[256];
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
