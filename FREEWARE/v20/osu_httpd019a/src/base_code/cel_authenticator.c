/*
 * Sample authenticator using extensions supplied by Charles Lane.
 *
 * The setup file simply defines a list of usernames/hosts and passwords.
 * If a password of '*' is specified, the corresponding username is checked
 * against the SYSUAF file.  If the username specification includes
 * a host address (e.g. SMITH@128.146.235.*), the password applies to only
 * the matching hosts.  A null username with a host address will accept
 * any connect from that host with no password check.
 *
 * Summary:
 *  <tag> tag-args...		   -> Special configuration info:
 *					<realm> string
 *  @host    *	   	           -> Host address must match
 *  username[@host] password       -> must match both
 *  *[@host]        password       -> any username, must match password
 *  username[@host] *              -> must match username, password from SYSUAF
 *  *[@host]        *              -> any username, password from SYSUAF
 *
 * If host restrictions would deny access regardless of username/password
 * given, the authentication fails with a 403 status so client doesn't waste
 * it's time prompting user for username/password.
 *
 * MACRO symbols for compile-time constants:
 *    DNS_ENABLE	Enable remote address translation via external
 *		  	lookup_host() function (authhost.c).
 *    UAF_HTBL_SIZE	Size of hash table for UAF cache must be power of 2.
 *			Default is 256.
 *    UAF_CACHE_LIMIT	Absolute limit of number of cache blocks allocated.
 *			Default = 10000
 *    UAF_CACHE_TIMEOUT	Timeout on uaf cache entries, specified as delta time.
 *			default = "0 01:00:00" (1 hour).
 *
 * Author:      David Jones
 * Date:        29-MAY-1994
 * Revised:      5-FEB-1995	Fixes for DECC.  Support host address.
 * Revised:	 8-FEB-1995	Add caching of UAI information.
 *				Also check flags for disuser's account.
 * Revised:	13-FEB-1995	Bugfix from CEL, make context var. static.
 * Revised:	23-FEB-1995	Add auth_callback prototype for notosimple.
 * Revised:	7-SEP-1995	Report SYSPRV status in startup message.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <descrip.h>
#include <ctype.h>
#include <uaidef.h>
#include <prvdef.h>
#include "authlib.h"
typedef struct { int l; char *s; } string;

#define MAXLINE 1024

static struct AUTHINFO {
   char user[MAXLINE];
   char pass[MAXLINE];
   char *host;			/* Host restrictions in username */
   struct AUTHINFO *next;
} *uhead, *utail, *ufree;
static struct {
    char setup_file[256];		/* Name of setup file */
    char realm[256];			/* realm (signature) for file */
} suf;

/*
 * Define structures for caching sysuaf data.
 */
#ifndef UAF_HTBL_SIZE
#define UAF_HTBL_SIZE 256
#endif
#ifndef UAF_CACHE_LIMIT
#define UAF_CACHE_LIMIT 10000
#endif
#ifndef UAF_CACHE_TIMEOUT
#define UAF_CACHE_TIMEOUT "0 01:00:00"
#endif
static struct UAFINFO {
    struct UAFINFO *next;
    long expiration[2];
    char username[40];
    char password[40];
} *uaf_hashtbl[UAF_HTBL_SIZE], *uaf_free;
static int cache_allocated;
static $DESCRIPTOR(cache_timeout_str, UAF_CACHE_TIMEOUT);
static long cache_timeout[2];
static long priv_mask[2];

/* prototypes for forward references */
int hash_realm(char *s);
int sys$setprv(), sys$getuai(), sys$hash_password();
int sys$bintim(), sys$gettim(), lib$subx();
int check_vms_password(char *username, char *password);
int http_parse_elements ( int limit, char *line, string *elem );
static int expand_uaf_free ( int alloc_size );
auth_callback notsosimple;		/* typedef auth_callback in authlib.h*/

/*
 * Main routine.
 */
int main ( int argc, char **argv )
{
    int status, i;
    long prev_privs[2];
    char log_message[120];

    /*
     * turn off any initial SYSPRV
     */

    priv_mask[0] = PRV$M_SYSPRV; priv_mask[1] = 0;
    (void) sys$setprv ( 0, priv_mask, 0, prev_privs );
    /*
     * Initialize data structures needed to perform authentication.
     * Using a linked list, null out the head and tail pointers.
     */

     uhead = utail = ufree = NULL;
     suf.setup_file[0] = '\0';
     cache_allocated = 0;
     for ( i = 0; i < UAF_HTBL_SIZE; i++ ) 
	uaf_hashtbl[i] = (struct UAFINFO *) 0;
     uaf_free = (struct UAFINFO *) 0;
     expand_uaf_free(40);
     status = sys$bintim ( &cache_timeout_str, cache_timeout );
     if ( 0 == (status&1) ) { fprintf(stderr,"Bad cache timeout specified\n");
	exit ( status );
     }
    /*
     * Process server requests.  The auth_server routine connects to the
     * the HTTP server that created us and effecitively makes the authenticate
     * function (notsosimple()) a remote procedure call for the server.
     * The third argument is text to be placed in the server's log file.
     */
    sprintf ( log_message,
	"CEL Authenticator, V 1.3a, initialized successfully, %s SYSPRV",
	(PRV$M_SYSPRV&prev_privs[0]) ? "has" : "does not have" );

    status = auth_server ( argc, argv, log_message, 1, notsosimple );
    /*
     * Getting to this point means an error occurred since auth_server
     * loops to continuously get authentication requests.  Most likely the
     * the HTTP server exitted so we should run down as well.  Perform any
     * necessary cleanup and exit.
     */
    exit ( status );
}
/************************************************************************/
/*
 * Read protection setup file and store in global variable.
 */
static int load_setup ( char *setup_file )
{
     int i, j, length, count;
     struct AUTHINFO *u;
     FILE *fp;
     char line[MAXLINE];
     string token[10];
     /*
      * Check if setup already loaded, otherwise try to open it.
      */
     if ( 0 == strcmp ( setup_file, suf.setup_file ) ) {
	return 1;
     }

     fp = fopen(setup_file,"r","shr=upd");
     if ( !fp ) return 0;
     /*
      * Free old UAF info blocks and init suf structure.
      */
     if ( utail != NULL ) {
	utail->next = ufree;
     }
     ufree = uhead;
     uhead = utail = NULL;
     suf.setup_file[0] = '\0';
     suf.realm[0] = '\0';

     while(fgets(line,MAXLINE,fp)) {
        for (j=0; (j<MAXLINE)&&line[j]; j++) {
            if ((line[j] == '#') || (line[j] == '\n')) {
                line[j] = '\0';
                break;
            }
        }
        length = strlen(line);
        if (length > 0) {
            count = http_parse_elements(9,line,token);
	    if ( count == 1 ) if ( token[0].s[0] == '@' ) count = 2;
            if ( count > 1 ) {
		/*
		 * Check for special control tags.
		 */
		if ( (token[0].s[0] == '<') && 
			(token[0].s[token[0].l-1] == '>') ) {
		    for ( j = 0; token[0].s[j]; j++ ) 
			token[0].s[j] = _toupper(token[0].s[j]);
		    if ( strcmp ( token[0].s, "<REALM>" ) == 0 ) {
			/* Concatenate rest of tokens as realm name */
			for ( j=0, i = 1; i < count; i++ ) {
			    if ( j ) suf.realm[j++] = ' ';
			    if ( (token[1].l + j) >= sizeof(suf.realm) ) break;
			    strcpy ( &suf.realm[j], token[i].s );
			    j = j + token[i].l;
			}
			suf.realm[j] = '\0';
		    }
		    continue;
		}
		/*
		 * Allocate block.
		 */
		if ( ufree == NULL ) {
                    u = (struct AUTHINFO *)malloc(sizeof(struct AUTHINFO));
		    if ( u == NULL ) {
			fclose ( fp );
			return 0;
		    };
 		} else {
		    u = ufree;			/* Get from lookaside list */
		    ufree = u->next;
  		}
		/*
		 * Copy tokens to authoinfo structure.
		 */
		u->host = NULL;
                for (i=0; i<token[0].l; i++) {
		    (u->user)[i] = _toupper((token[0].s)[i]);
		    if ( (u->user)[i] == '@' ) {
			/* Terminate username and make remainder host string */
		 	(u->user)[i] = '\0';
			u->host = &(u->user)[i+1];
		    }
  		}
                (u->user)[token[0].l] = '\0';
                for (i=0; i<token[1].l; i++) 
			(u->pass)[i] = _toupper((token[1].s)[i]);
                (u->pass)[token[1].l] = '\0';
		/*
		 * Add to linked list of authinfo structures.
		 */
                u->next = NULL;
		if ( utail == NULL ) uhead = u;
		else utail->next = u;
		utail = u;
            }
        }
     }
     fclose(fp);
     strcpy ( suf.setup_file, setup_file );
     return 1;
}
/************************************************************************/
/* Dummy routine for returning hostname for address.  To replace, compile
 * module DNS_ENABLE defined.
 */
#ifndef DNS_ENABLE
void lookup_hostname ( unsigned char *address, char *remote_host )
{
    strcpy ( remote_host, "-1.-1.-1.-1" );
}
#endif
/************************************************************************/
/* Compare host string with IP address, return 1 if it matches. 
 */
static int check_host ( char *host, unsigned char *address )
{
    static char octet_str[256][4];
    static int octet_str_init = 0;

    if ( ( *host >= '0' ) && ( *host <= '9' ) ) {
	/*
	 * Numeric host address.  Compare octet by octet, host string can
	 * substitute * for an octet.
	 */
        char number[16], *d, *octet;
        int i, j, len;
	if ( !octet_str_init ) {
	    /* First time, init numeric strings */
	    for ( i = 0; i < 256; i++ ) sprintf ( octet_str[i], "%d", i );
	    octet_str_init = 1;
	}

        for ( d = host, i = 0; i < 4; i++ ) {
	    if ( (*d == '*') && ((d[1]=='.')||(d[1]=='\0')) ) {
		d += 2;
	    } else {
		octet = octet_str[address[i]];
		for ( len = 0; d[len] && (d[len] != '.'); len++ ) {
		    if ( d[len] != octet[len] ) break;		/* mismatch */
		}

		if ( octet[len] != '\0' ) break;
		else if ( i < 3 ) {
		    if ( d[len] != '.' ) break;		/* wrong delimiter */
		} else {
		    if ( d[len] != '\0' ) break;
		}
		d += len + 1;
	    }
	}
	if ( i == 4 ) return 1;	/* we have a match, 4 fields matched */

    } else {
	/*
	 * Compare domain name.
	 */
	char *d;
        int i, j, k, len, rhost_len;
	char remote_host[512];
        void lookup_hostname();

	lookup_hostname ( address, remote_host );
	rhost_len = strlen ( remote_host );
	remote_host[rhost_len++] = '.';

	/* Break hostname into separate labels */
	d = host;
	for (j=i=0; i <= rhost_len; i++ ) if (remote_host[i]=='.') {
	    /* 
	     * String goes from j to i, ending in period.  Determine
	     * length (k) of next label in pattern string (d)
	     */
	    for ( k=0; d[k] && (d[k]!='.'); k++ );
	    len = i - j;
	    if ( (k != 1) || (d[0] != '*') ) {

	        if ( k != len) break;	/* mismatch */
	        if ( strncmp(d,&remote_host[j],len) ) break;
	    }
	    d += k; if ( *d ) d++;
	    j = i+1;
	}
	/* We pass if If all labels matched and no more value */
	if ( (i > rhost_len) && !*d ) return 1;
    }
    return 0;
}
/************************************************************************/
/*
 * password checking routine.
 *  check against username/password pairs stored in linked list
 *  several cases:
 *
 *  @host    *	   	           -> Host address must match
 *  username[@host] password       -> must match both
 *  *[@host]        password       -> any username, must match password
 *  username[@host] *              -> must match username, password from SYSUAF
 *  *[@host]        *              -> any username, password from SYSUAF
 *
 *  Return values:
 *	1		Access granted.
 *	0		No match, username required.
 *	-1		No match, setup file specified no username checks
 *			for remote (or all) host so authorization will not help.
 */
static int check_password ( char *username, char *password, 
	unsigned char *remote_address )
{
    struct AUTHINFO *u;
    int user_check;

    user_check = -1;
    for ( u = uhead; u != NULL; u = u->next ) {
	if ( u->host ) {
	    /* Check host, go to next autinfo if check fails. */
	    if ( !check_host ( u->host, remote_address ) ) continue;

	    /* Host passed, Accept uncondionally if username null. */
	    if ( u->user[0] == '\0' ) return 1;
	}
	user_check = 0;
	/* fprintf(stderr,"checking user '%s'@'%s', password '%s'\n", u->user,
	    u->host ? u->host : "*", u->pass ); */
        if (strcmp(u->user,"*") == 0) {
            if (strcmp(u->pass,"*") == 0) {
                if (check_vms_password(username, password)) return 1;
            } else {
                if (strcmp(u->pass,password) == 0) return 1;
            }
        } else if (strcmp(u->user,username) == 0) {
            if (strcmp(u->pass,"*") == 0) {
                if (check_vms_password(username,password)) return 1;
            } else {
                if (strcmp(u->pass,password) == 0) return 1;
            }
        }
    }
    return user_check;
}


/****************************************************************************/
/* Do garbage collection (move expired cached entries to free list), if
 * free list after GC is empty, allocate more cache blocks for it.
 */
static int expand_uaf_free ( int alloc_size ) {
    int i, j, expired;
    long delta[2], cur_time[2];
    struct UAFINFO *cb, *prev_cb;
    /*
     * Check each hash table chain for expired entries.
     */
    sys$gettim ( cur_time );
    for ( i = 0; i < UAF_HTBL_SIZE; i++ ) if ( uaf_hashtbl[i] ) {
	prev_cb = (struct UAFINFO *) 0;
	for ( cb = uaf_hashtbl[i]; cb; ) {
	    /* 
	     * See if cur_time is larger (after) cache block's expiration.
	     */
	    lib$subx ( cb->expiration, cur_time, delta );
	    expired = (delta[1] < 0);
	    if ( expired ) {
		/*
		 * Remove block from hash chain and add to free list.
		 */
		/* fprintf(stderr, "Removing expired entry for cache, %s\n", cb->username); */
		for ( j = 0; cb->password[j]; j++ ) cb->password[j] = '\0';
		if ( prev_cb ) prev_cb->next = cb->next;
		else uaf_hashtbl[i] = cb->next;
		cb->next = uaf_free;
		uaf_free = cb;

		cb = (prev_cb) ? prev_cb->next : uaf_hashtbl[i];
	    } else {
		prev_cb = cb;
	        cb = cb->next;
	    }
	}
    }
    /*
     * Expand free list by alloc_size blocks if still emtpy and alloc_size > 0.
     * Limit total number of cache blocks allocated to UAF_CACHE_LIMIT.
     */
    if ( uaf_free ) return 1;
    if ( alloc_size > (UAF_CACHE_LIMIT - cache_allocated) )
	alloc_size = UAF_CACHE_LIMIT - cache_allocated;
    if ( alloc_size <= 0 ) return 0;
    /*
     * Allocate all in 1 chunk and divide up into linked list.
     */
    cb = uaf_free;
    uaf_free = malloc ( sizeof(struct UAFINFO) * alloc_size );
    if ( !uaf_free ) return 0;
    cache_allocated += alloc_size;
    for ( i = 0; i < 39; i++ ) 	uaf_free[i].next = &uaf_free[i+1];
    uaf_free[i].next = cb;
    return 1;
}

int check_vms_password(char *username, char *password)
{
    int iss, hndx;
    unsigned long hash1[2], hash0[2];
    long *context_ptr, cur_time[2], delta[2];
    static long context = -1;
    $DESCRIPTOR(d_user,"");
    $DESCRIPTOR(d_pass,"");
    unsigned char algorithm;
    unsigned short salt;
    unsigned long flags, len0, len1, len2;

    struct item_list_3 {
        unsigned short il3$w_buflen;
        unsigned short il3$w_code;
        void *         il3$a_buffer;
        unsigned long *il3$a_retlen;
    };

    static struct item_list_3 uai_items[]  = {
        {  8, UAI$_PWD, NULL, NULL},    /* &hash0[0], &len0}, */
        {  2, UAI$_SALT, NULL, NULL},   /* &salt,     &len1}, */
        {  1, UAI$_ENCRYPT, NULL, NULL},/* &algorithm,&len2}, */
	{  4, UAI$_FLAGS, NULL, NULL},
        {  0, 0, NULL, NULL}
    };
    struct UAFINFO *cb;
    /*
     * Check cache for matching entry.
     */
    if ( username[0] == '\0' ) return 0;		/* null username */
    hndx = ((UAF_HTBL_SIZE-1)&hash_realm(username));
    /* fprintf(stderr,"search cache, hash ndx = %d (%d)\n", hndx, uaf_hashtbl[hndx]);*/
    sys$gettim ( cur_time );
    for ( cb = uaf_hashtbl[hndx]; cb; cb = cb->next ) {
	if ( 0 == strcmp ( username, cb->username ) ) {
	    /*
	     * Cache block fount for this username stale and re-validate if so. 
	     */
	    lib$subx ( cb->expiration, cur_time, delta );
	    if ( delta[1] < 0 ) break;
	    /* 
	     * Test password against cached copy
	     */
	    if ( 0 != strcmp ( password, cb->password ) ) break;
	    /* Accept */
	    return 1;
	}
    }
    /*
     * Build arguments for $getuai.
     */
    uai_items[0].il3$a_buffer = &hash0[0];
    uai_items[0].il3$a_retlen = &len0;
    uai_items[1].il3$a_buffer = &salt;
    uai_items[1].il3$a_retlen = &len1;
    uai_items[2].il3$a_buffer = &algorithm;
    uai_items[2].il3$a_retlen = &len2;
    uai_items[3].il3$a_buffer = &flags;

    d_user.dsc$a_pointer = username;
    d_user.dsc$w_length  = strlen(username);
    d_pass.dsc$a_pointer = password;
    d_pass.dsc$w_length  = strlen(password);
    /*
     * Enable SYSPRV temporarily, get UAF items and disable again.
     */
    iss = sys$setprv ( 1, priv_mask, 0, 0 );
    if (!(iss&1)) return 0;

    context_ptr = (long *) 0;
#ifdef __VMS_VERSION
    if ( strcmp ( __VMS_VERSION, "V6.1" ) > 0 ) context_ptr = &context;
#endif
    iss = sys$getuai(0,context_ptr,&d_user,&uai_items,0,0,0);

    (void) sys$setprv ( 0, priv_mask, 0, 0 );
    /*
     * Hash the password for comparison with the UAF entry.
     */
    if ((iss&1)) {
	iss = sys$hash_password(&d_pass,algorithm,salt,&d_user,&hash1[0]);
    }
    if (!(iss&1)) return 0;
    /*
     * At this point, we have hashed the user's password and have
     * his SYSUAF entry.  Compare them.
     */
    if ( ((flags&UAI$M_DISACNT) == 0 ) && 
		(hash0[0] == hash1[0]) && (hash0[1]==hash1[1]) ) {
	/* 
	 * Password validated, save it in cache block 
	 */
	if ( !cb ) {
	    if ( !uaf_free ) if ( !expand_uaf_free(100) ) return 1;
	    cb = uaf_free;
	    uaf_free = cb->next;
	    cb->next = uaf_hashtbl[hndx];
	    uaf_hashtbl[hndx] = cb;
	}
	/*
	 * Load/reload cache block with latest expriation and password.
	 */
	lib$subx ( cur_time, cache_timeout, cb->expiration );
	strncpy ( cb->username, username, 39 );
	strncpy ( cb->password, password, 39 );		/* pick up latest */
	cb->username[39] = '\0';
	cb->password[39] = '\0';
	return 1;
    } else if ( cb ) {
	/*
	 * Flush expired entries to get rid of the stale 1.
	 */
	lib$subx ( cb->expiration, cur_time, delta );
	if ( delta[1] < 0 ) expand_uaf_free ( 0 );
	return 0;
    } else return 0;
}

/*************************************************************************/
/* Authenticator callback routine for auth_server().  This routine is
 * called to process an authentication request sent by the server.
 *
 * This routine determines whether access to the requested object (ident)
 * with the specified method is to be granted, validating the authorization
 * information against the protections defined by the setup file.
 *
 * return values:
 *      0       Access to the object is denied.
 *      1       Access to the object is permitted.
 *
 * When access is denied, this routine sets the response argument to point
 * to a statically allocated string containing the HTTP error response headers
 * to send to the client, sans the HTTP version (i.e. "403 Access denied\r\n").
 * The newlines are included.  If a 401 status is returned, the response header
 * includes the WWW-authenticate headers for the authorization schemes that
 * this routine understands.
 */
int notsosimple (
        int local_port,                 /* Local port # of TCP/IP connection */
        int remote_port,                /* Remote TCP/IP port of client */
        unsigned char remote_address[4],/* Remote binary IP address of client*/
        char *method,                   /* Requested method (GET, PUT, etc); */
        char *setup_file,               /* Protection setup file name */
        char *ident,                    /* Ident portion of translated URL */
        char *authorization,            /* Auth. headers included in request */
        char **response,                /* HTTP error response headers */
        char **log )                    /* Optional log message */
{
    int status, allowed, auth_basic_authorization();
    char username[40], password[40];
    /*
     * Note that response and log buffers must be statically allocated.
     * Fail and succeed are pointers to string constants (implicitly static),
     * if you make them char arrays be to all make them static storage class.
     */
    static char log_message[200];
    static char fail[200];
    char *succeed = "200 access to protected file granted\r\n";
    char *setup_failure = "500 failed to open protection db file\r\n";
    char *host_failure = "403 Forbidden, authorization will not help\r\n";
    /*
     * At this point, a real authenticator would read the protection setup
     * file to locate the databases (password file, group file) that define
     * the protections for the object.  If this operation failed, we would
     * return failure (0) with a "500" error status line.
     */
     if ( !load_setup ( setup_file ) ) {
        *response = setup_failure;
        sprintf(log_message,"unable to open setup file %s",setup_file);
        *log = log_message;
        return 0;
     }

    /*
     * Set default response to the "unauthorized" error response headers string
     */
    *response = fail;
    *log = log_message;
    /*
     * See if request has a basic scheme authorization line and decode it
     * using utility routines from authlib.c.  The last argument in the
     * auth_basic_authorization() call forces the result to upper case.
     */
    if ( auth_basic_authorization ( authorization, username, sizeof(username),
                password, sizeof(password), 1 ) ) {
        /*
         * Validate password using simple rule.  A real authenticator would
         * validate it against the password database.
         */
        allowed = check_password ( username, password, remote_address );
        if ( allowed == 1 ) *response = succeed;
        /*
         * Generate a message for the server's log file.  A real authenticator
         * wouldn't place the password in the log file.
	 * The server checks if the log line begins with "[username]" and
	 * makes 'username' the local user in the access.log file.
         */
        sprintf(log_message,"[%s]Authentication for user='%s' pass='%s' %s.",
                allowed ? username : "", username, "[CENSORED]", allowed ?
                "succeeded" : "failed" );
    } else {
	/*
	 * Validate password using null username.
	 */
	allowed = check_password ( "", "", remote_address );
        sprintf(log_message,"Default access authentication %s", 
		(allowed==1) ? "succeeded" :"failed" );
    }
    if ( allowed == -1 ) {
	/* Host checks failed, send 403 status */
	*response = host_failure;
	allowed = 0;
    } else if ( *response == fail ) {
	/* Only do work of generating header line if needed */
	if ( suf.realm[0] == '\0' ) 
		sprintf ( suf.realm, "CEL%08X",	hash_realm(suf.setup_file) );
       sprintf(fail,
        "401 Unauthorized\r\nWWW-Authenticate: Basic realm=\"%s\"\r\n",
         suf.realm );
    }
    return allowed;
}

/* following swiped from rule_file.c ... we don't want the rest of the
 * baggage though.
 */

/*
 * Parse line into whitespace-delimited tokens, trimming leading and trailing
 * whitespace.  Each token parse is explicitly null-terminataed.  Function
 * value returned is number of elements found.  May be 1 more than limit if
 * last element does not end line.
 */
int http_parse_elements (
        int limit,          /* Max number of element to delimit */
        char *line,         /* input line to parse */
        string *elem )      /* Output array. */
{
    int tcnt, in_token, length, i;
    char *ptr;
    /* Delimit up to limit tokens */
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


/*
    just a simple minded hash of ascii -> 32 bit integer
    so that we can (hopefully) tell one setup file from another
    and assign them unique realms without giving away too much
    information about where the setup files actually live.
*/

int hash_realm(char *s)
{
    char c;
    int i = 0;

    while (c = *s++) {
        i = ((i>>24)&0x3F) ^ c ^ (i<<6);
    }
    return i;
}
