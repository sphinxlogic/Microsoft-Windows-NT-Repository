/*
 * This program is a server script for the DECthread HTTP_SERVER.  It
 * provides support for keyword searches index files associated with html documents.
 *
 * This program assumes that it was invoked by the WWWEXEC decnet task and
 * the logical dnet_link is a process permanent file connected to the
 * the HTTP server.
 *
 * Usage:
 *	index_search method url protocol
 *
 * Command line arguments:
 *	method		Ignored, assumed to be "GET". (argv[1])
 *	url		Filepath portion of URL, after translation by rules 
 *			file.  The index file to search is derived from this 
 *			filename, the type field changed to .idx. (argv[2])
 *
 * Environment variables:
 *	none
 *
 *  Author:	David Jones
 *  Date:	29-APR-1994
 *  Revised:    16-MAR-1995	Update for newer selector file format.
 *  Revised:    20-MAR-1995	Update to work with both old/new formats.
 *  Revised:	18-JUL-1995	Fix for DECC 5.0 macros.
 */ 

/* Index_Search.c Revsion History:
 * ===============================

20-Aug-1994 (FM)  Stipped down to just what's needed for use as the DECthreads
		   httpd's Search script for foo.html files with iSINDEX tags.
		   The foo.idx and foo.sel index pairs should be in the same
		   directory as the foo.html file, should have the same root
		   name, and should have been built by Build_Index with the
		   /whole switch.  The mods eliminate a security problem in
		   the original Index_Search.c, for gerry-rigged URL's that
		   could invoke file fetches without any authorization check,
		   and eliminate the need for redirection in hit-file fetches.
		   -- Foteos Macrides

 */

/*=================
** Modification of:
**=========================================================================**/
/* WWWquery.c -- Apply a boolean query to (a) keyword file(s), and return a
**		  hit list, a requested section with HTML "packaging", or a
**		  "raw" HTML document from an indexed file set.
**		 The symbol WWW_SEND_RAW_FILE must be made non-NULL by the
**		  calling script for a requested file to be sent raw (i.e.,
**		  without additional headers and PRE tags). 
**		 The symbol WWW_OMIT_PRE_TAG can be made non-NULL by the
**		  calling script to have document sections returned without
**		  PRE tags bounding them, but with the other HTML "packaging"
**		  retained.
**
** Foteos Macrides -- Worcester Foundation for Experimental Biology
**=================
** Modification of:
**=========================================================================**/
/* query.c -- apply a boolean query to a keyword file */
/* Bruce Tanner -- Cerritos College */

/* Version history:

   0.0 1993/06/18 Start the program
   0.1 1993/07/03 Finish coding
   0.8 1993/07/06 Squashed most of the bugs
   1.0 1993/07/13 Released
   1.1 1993/07/29 Changed grammar to allow Query inside Factor
   1.2 1993/07/29 Invoke interactive mode if argc < 4
   1.3 1993/08/04 Change name to query, calling it search is confusing
   1.4 1993/08/06 Move wildcard processing from shell to inside program
   1.5 1993/11/17 Include punctuation in query string to match build_index
   1.6 1993/12/01 Handle multiple topic field sizes
   1.7 1994/03/01 Fix bad reference to freed pointer

*/

/* Usage: search idx-file out-file query host port directory */

/* Query:  expr {expr}              implicit 'and' between expressions */
/* Expr:   term {or term}                                              */
/* Term:   factor {and|not factor}                                     */
/* Factor: (query) | token                                             */
/* Token:  [field name] word                                           */
/* Word:   a-z{a-z}[*]                                                 */
/*=========================================================================**
**
** WWWquery.c Version History:

   0.0 1994/04/08 Made mods for use with CERN v2.16betavms httpd.
   		  Uses a query constructed from WWW_KEY fields by an htbin
		  script.  Returns an HTML menu of hits with "escaped" URL's
		  or the requested document/section. - FM
   1.0 1994/04/24 Seems to working OK, so posted the Initial Release. - FM
   1.1 1994/04/28 Added WWW_SEND_RAW_FILE and WWW_OMIT_PRE_TAG symbols, and
   		  fixed up partial URL handling. - FM
*/

#include <ssdef.h>
#include <stdlib.h>
#include <stdio.h>
#define strerror strerror_1
#include <string.h>
#undef strerror
char strerror ( int errnum, ... );
#include <ctype.h>
#include <errno.h>
#include <rms.h>
#include <descrip.h>
#include <fscndef.h>
#include <unixlib.h>
#include "cgilib.h"

int sys$open(), sys$display(), SYS$CONNECT(), sys$close(), sys$find();
int sys$get(), LIB$STOP();
#ifdef __DECC
#ifndef shell$translate_vms		/* pre 5.0 on VAX */
char *decc$translate_vms();
int decc$to_vms();
#endif
#define SHELL_TRANSLATE_VMS decc$translate_vms
#define SHELL_TO_VMS(a,b,c) decc$to_vms(a,b,c,0)	/* set no-directory*/
#else
#define SHELL_TRANSLATE_VMS shell$translate_vms
#define SHELL_TO_VMS shell$to_vms
char *shell$translate_vms();
int shell$to_vms();
#endif

#define KEY_NAME 32    /* maximum size of key name */
#define MAX_QUERY 500  /* maximum size of query */
#define SPEC_SIZE 256  /* file specification size */
#define DEBUG 0        /* perform debugging printfs */
#define MOD_REALLOC 1  /* handle initial realloc() problem */
#define MEM_DEBUG 0    /* realloc() debugging printfs */

typedef struct {
    int index;        /* selector index */
    int file;         /* file index */
} Select;

typedef struct {
    int count;         /* number of members in result set */
    Select *select;    /* set of selectors */
} Result;

typedef struct {
    int krf;           /* key-of-reference for this token */
    char *str;         /* token string */
} Token;


void and(Result, Result, Result *);
void or(Result, Result, Result *);
void select_result(Result);
void *my_realloc(void *, int);
char * URLescape(char * str);
int strncasecomp(char *, char *, int);

static struct FAB idxfab, selfab;
static struct RAB idxrab, selrab;
static struct XABSUM xabsum;
static FILE *outfile;
static char *index_type = ".IDX", *selector_type = ".SEL";
static int index_offset, keys, max_key = 0, last_field = 0;
static char *index_field;
static struct XABKEY **keytab;
static char orig_qstr[MAX_QUERY];
static int selector_index = 0;  /* selector file index */
static char **selector_name;    /* array of selector file names */
static int *index_size;         /* array of selector file index field sizes */
static char Host[SPEC_SIZE];    /* server_name, and port if not 80 */

static char *hex = "0123456789ABCDEF";		/* For escaping URL's */
static unsigned char isAcceptable[96] =	/* For escaping URL's */
    /*   0 1 2 3 4 5 6 7 8 9 A B C D E F */
    {    0,0,0,0,0,0,0,0,0,0,7,6,0,7,7,4,	/* 2x   !"#$%&'()*+,-./	 */
         7,7,7,7,7,7,7,7,7,7,0,0,0,0,0,0,	/* 3x  0123456789:;<=>?	 */
	 7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,	/* 4x  @ABCDEFGHIJKLMNO  */
	 7,7,7,7,7,7,7,7,7,7,7,0,0,0,0,7,	/* 5X  PQRSTUVWXYZ[\]^_	 */
	 0,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,	/* 6x  `abcdefghijklmno	 */
	 7,7,7,7,7,7,7,7,7,7,7,0,0,0,0,0 };	/* 7X  pqrstuvwxyz{\}~	DEL */

static Result result;
int net_unescape_string(char *s, int *l);
/*************************************************************************/
int main(int argc, char *argv[])
{
    int status, count, qstr_len, query_index(), net_link_close();
    int net_link_open(), net_link_query();
    char *cp, *port;
    /*
     * Validate arguments command line arguments.  The config file name
     * is appended to the script name that invokes this program.
     */
    if ( argc < 3 ) {
	fprintf( stderr, "Missing arguments, usage: prog method url\n");
	exit ( 1 );
    }
    status = cgi_init ( argc, argv );
    if ( (status&1) == 0 ) exit ( status );
    /*
    ** Get the host, and port if not 80.
    */
    strcpy(Host, cgi_info("SERVER_NAME"));
    for (cp=Host; *cp; cp++)
        *cp = _tolower(*cp);
    if (strcmp((port=cgi_info("SERVER_PORT")), "80")) {
        strcat(Host, ":");
	strcat(Host, port);
    }
    /*
     */
    cp = cgi_info ( "QUERY_STRING" );
    /* printf("Query string: %s\n", cp ? cp : "{null}" ); */
    if ( !cp ) cp = "";
    strncpy ( orig_qstr, cp, sizeof(orig_qstr) );
    orig_qstr[sizeof(orig_qstr)-1] = '\0';
    qstr_len = strlen ( orig_qstr );
    keytab = (struct XABKEY **) 0;

    /*
     * Restore '+' separators to spaces
     */
    for (cp = orig_qstr; *cp; cp++)
        if (*cp == '+') *cp = ' ';

    /*
     * Convert escaped characters, and make string lowercase.
     */
    net_unescape_string ( orig_qstr, &qstr_len );
    orig_qstr[qstr_len] = '\0';
    for (cp = orig_qstr; *cp; cp++) {
#ifdef CONVERT_PUNCTUATION
            if (ispunct(*cp) && (*cp != '(') && (*cp != ')') && (*cp != '*'))
                *cp = ' ';    { convert punct. except '(' ')' '*' to spaces }
#endif
        *cp = _tolower(*cp);  /* force words lowercase */
    }
    /*
     * Process input file(s), exapanding wildcards.
     */
    strcat(orig_qstr, " ");          /* query ends with a space */

    result.count = 0;           /* init result */
    result.select = NULL;
    count = SHELL_TO_VMS ( argv[2], query_index, 1 );
 /* printf("file found count: %d, result count: %d\n", count, result.count);*/
    select_result(result);
    net_link_close();
}
/****************************************************************************/
/* Action routine for SHELL_TO_VMS.
 */
int query_index ( char * fname )
{
    int status, ind, qstr_len, size, RunQuery, context, query();
    char input_spec[SPEC_SIZE], idx_spec[SPEC_SIZE];
    static char  file_name[SPEC_SIZE];
    char *cp, qstr[MAX_QUERY];
    Result file_result;
    Token token;
    $DESCRIPTOR(input_dsc, "");
    $DESCRIPTOR(idx_dsc, "");

    strcpy(input_spec, fname); /* set up descriptor to wildcard input spec */
    input_dsc.dsc$w_length = (short) strlen(input_spec);
    input_dsc.dsc$a_pointer = input_spec;

    strcpy(idx_spec, fname); 	/* copy node, dev, dir, name */
    idx_dsc.dsc$w_length = (short) strlen(idx_spec);
    idx_dsc.dsc$a_pointer = idx_spec;
    strcpy ( file_name, fname );


        /* save the file names for when we need to get the selectors */
        selector_name = (char **) my_realloc((char **) selector_name,
                                     (++selector_index + 1) * sizeof(char *));
        selector_name[selector_index] = (char *) calloc(strlen(file_name) + 1,
                                                        sizeof(char));
        strcpy(selector_name[selector_index], file_name);
        
        /* initialize index fab and rab */
        idxfab = cc$rms_fab;
        idxrab = cc$rms_rab;
        xabsum = cc$rms_xabsum;
        
        idxfab.fab$l_fna = index_type;
        idxfab.fab$b_fns = strlen(index_type);
        idxfab.fab$l_dna = file_name;
        idxfab.fab$b_dns = strlen(file_name);
        idxfab.fab$b_shr = FAB$M_SHRGET;
        idxfab.fab$l_xab = (char *) &xabsum;
        
        idxrab.rab$l_fab = (struct FAB *) &idxfab;
        idxrab.rab$b_rac = RAB$C_KEY;
        
        /* open index file */
        if (((status = sys$open(&idxfab)) & 1) != SS$_NORMAL) {
	    fprintf(stderr,"Error opening index file %s: %s\n", file_name,
		strerror(EVMSERR,status) );
	    return 1;
	}
        
        if (idxfab.fab$b_org != FAB$C_IDX) {
            printf("Idx file must be indexed\n");
            exit(1);
        }
        
        keys = xabsum.xab$b_nok;
        
        keytab = (struct XABKEY **) my_realloc((struct XABKEY **) keytab,
                                              keys * sizeof(struct XABKEY **));
        
        /* build an array of XABKEY */
        for (ind = 0; ind < keys; ind++) {
            keytab[ind] = (struct XABKEY *)
                          my_realloc((struct XABKEY *) keytab[ind], 
                                     sizeof(struct XABKEY));
            *keytab[ind] = cc$rms_xabkey;
            keytab[ind]->xab$l_knm = (char *) my_realloc((char *)
                                                        keytab[ind]->xab$l_knm,
                                                        KEY_NAME + 1);
            strncpy((char *) keytab[ind]->xab$l_knm, "",KEY_NAME + 1);
            if (ind > 0) keytab[ind - 1]->xab$l_nxt = (char *) keytab[ind];
        }
        idxfab.fab$l_xab = (char *) keytab[0];
        
        /* fill in the key XABs */
        if (((status = sys$display(&idxfab)) & 1) != SS$_NORMAL)
            LIB$STOP(status);
        
        index_size = (int *) my_realloc((int *) index_size,
                                        (selector_index + 1) * sizeof(int));

        if (keys == 1) {
            index_size[selector_index] = 
	    			idxfab.fab$w_mrs - keytab[0]->xab$b_siz0;
            index_offset = keytab[0]->xab$w_pos0 + keytab[0]->xab$b_siz0 -
                           index_size[selector_index];
            index_field = (char *) my_realloc((char *) index_field,
                              (index_size[selector_index] + 1) * sizeof(char));
            strncpy(index_field, "", index_size[selector_index] + 1);
        }
        else   /* multi-key records don't have counts */
            index_size[selector_index] = 0;

        for (ind = 0; ind < keys; ind++) {
             max_key = max_key < keytab[ind]->xab$b_siz0 ?
                       keytab[ind]->xab$b_siz0 : max_key;
        }
        
        /* make the index file record */
        idxrab.rab$w_usz = idxfab.fab$w_mrs;
        idxrab.rab$l_ubf = (char *) my_realloc((char *) idxrab.rab$l_ubf,
                                    (idxrab.rab$w_usz + 1) * sizeof(char));
        strncpy(idxrab.rab$l_ubf, "", idxrab.rab$w_usz + 1);
        
        /* connect record streams */
        if (((status = SYS$CONNECT(&idxrab)) & 1) != SS$_NORMAL)
            LIB$STOP(status);
        
        strcpy(qstr, orig_qstr);           /* query and friends mangle qstr */
        query(qstr, &file_result, &token); /* evaluate query on this file   */
        or(result, file_result, &result);  /* accumulate results	    */
        
        status = sys$close(&idxfab);
    
}
void open_selector(char *file_name)
{
    int status;

    /* initialize selector fab and rab */
    selfab = cc$rms_fab;
    selrab = cc$rms_rab;

    selfab.fab$l_dna = file_name;
    selfab.fab$b_dns = strlen(file_name);
    selfab.fab$l_fna = selector_type;
    selfab.fab$b_fns = strlen(selector_type);
    selfab.fab$b_shr = FAB$M_SHRGET;

    selrab.rab$l_fab = (struct FAB *) &selfab;
    selrab.rab$b_rac = RAB$C_KEY;

    /* open selector file */
    if (((status = sys$open(&selfab)) & 1) != SS$_NORMAL)
        LIB$STOP(status);

    if (selfab.fab$b_org != FAB$C_IDX) {
        printf("Selector file must be indexed\n");
        exit(1);
    }

    /* make the selector file record */
    selrab.rab$w_usz = selfab.fab$w_mrs;
    selrab.rab$l_ubf = (char *) my_realloc((char *) selrab.rab$l_ubf,
                                        (selrab.rab$w_usz + 1) * sizeof(char));
    strncpy(selrab.rab$l_ubf, "", selrab.rab$w_usz + 1);

    if (((status = SYS$CONNECT(&selrab)) & 1) != SS$_NORMAL)
        LIB$STOP(status);
}


/* return the key of reference associated with the field name */ 
int is_field(char *str, int *krf)
{
    int ind;

    for (ind = 0; ind < keys; ind++)
        if (strcmp(str, keytab[ind]->xab$l_knm) == 0) {
            *krf = last_field = ind;  /* field name matched */
            return (1);
        }
    *krf = last_field;   /* not a field name, carry forward last field */
    return 0;
}


/* return the next token or field name (key of reference) and token */
void get_token(char *qstr, Token *token)
{
    char *str = NULL, *cp;
    int krf = -1;

    while (*qstr && (*qstr <= ' '))  /* remove leading spaces and junk */
        strcpy(qstr, qstr + 1);

    if ((*qstr == '(') || (*qstr == ')')) {
        str = (char *) calloc(2, sizeof(char));
        strncpy(str, qstr, 1);
        strcpy(qstr, qstr + 1);
    }
    else {
        if (strchr(qstr, ' ') && strchr(qstr, ')'))
            cp = strchr(qstr, ' ') < strchr(qstr, ')') ?
                 strchr(qstr, ' ') : strchr(qstr, ')');
        else if (strchr(qstr, ')') == NULL)
            cp = strchr(qstr, ' ');
        else if (strchr(qstr, ' ') == NULL)
            cp = strchr(qstr, ')');
        else
            cp = NULL;
        if (cp) {
            str = (char *) calloc(max_key + 1, sizeof(char));
            strncpy(str, qstr, cp - qstr);
            if (*cp == ')')
                strcpy(qstr, cp);
            else
                strcpy(qstr, cp + 1);
            if (is_field(str, &krf)) {
                free(str);
                if (strchr(qstr, ' ') && strchr(qstr, ')'))
                    cp = strchr(qstr, ' ') < strchr(qstr, ')') ?
                         strchr(qstr, ' ') : strchr(qstr, ')');
                else if (strchr(qstr, ')') == NULL)
                    cp = strchr(qstr, ' ');
                else if (strchr(qstr, ' ') == NULL)
                    cp = strchr(qstr, ')');
                else
                    cp = NULL;
                str = (char *) calloc(keytab[krf]->xab$b_siz0 + 1,
				      sizeof(char));
                strncpy(str, qstr, cp - qstr);
                if (*cp == ')')
                    strcpy(qstr, cp);
                else
                    strcpy(qstr, cp + 1);
            }
        }
        else
            str = (char *) calloc(1, sizeof(char));
    }
    token->str = str;
    token->krf = krf;
}


/*
   Lance was right, realloc sometimes blows when initially allocating memory
   MOD_REALLOC indicates whether to use malloc() on initial allocation
 */
void *my_realloc(void *mem, int size)
{
    void *mem_ptr;

    if ((mem == (void *) 0) && (MOD_REALLOC)) {
	char *blk;
	blk = (char *) malloc(size);
	if ( blk ) memset ( blk, 0, size );	/* zero allocated memory */
        return (void *) blk;
    } else {
        mem_ptr = (void *) realloc(mem, size);
        return (mem_ptr);
    }
}


/* create a set of selectors that are associated with the token */
void find(Token token, Result *rx)
{
    int status, ind, value;

    rx->count = 0;        /* assume no match */
    rx->select = NULL;

    idxrab.rab$b_rac = RAB$C_KEY;
    idxrab.rab$b_krf = token.krf;
    idxrab.rab$l_kbf = token.str;
    idxrab.rab$l_rop = 0;            /* set up exact match */
    idxrab.rab$b_ksz = keys == 1 ? keytab[token.krf]->xab$b_siz0 -
                                   index_size[selector_index] :
                                   keytab[token.krf]->xab$b_siz0;
    if (token.str[strlen(token.str) - 1] == '*') {
        idxrab.rab$b_ksz = strlen(token.str) - 1;
        idxrab.rab$l_rop = RAB$M_KGE;  /* set up approximate generic match */
    }
    /* key can't be shorter than field size */
    while (strlen(token.str) < idxrab.rab$b_ksz)
        strcat(token.str, " ");

    /* find the start record */
    if (((status = sys$find(&idxrab)) & 1) != SS$_NORMAL)
        return;  /* no match */

    idxrab.rab$b_rac = RAB$C_SEQ;
    while (((status = sys$get(&idxrab)) & 1) == SS$_NORMAL) {
        if (strncmp((char *)
		    (idxrab.rab$l_ubf + keytab[token.krf]->xab$w_pos0),
                    token.str, idxrab.rab$b_ksz) != 0)
            break;  /* no match */

        if (keys == 1) {
            strncpy(index_field,
                    (char *) (idxrab.rab$l_ubf + index_offset),
                    index_size[selector_index]);
            value = atoi(index_field);
        }
        /* else handle multi-key rfa here */

        for (ind = 0; ind < rx->count; ind++)
            if ((rx->select[ind].index == value) &&
                (rx->select[ind].file == selector_index))
	    /* if the value already is there */
                break;       /* don't add it */
        /* unfortunately, you can't put a 'continue' in the previous line */
        if ((ind < rx->count) && (rx->select[ind].index == value) &&
            (rx->select[ind].file == selector_index))
            continue;

        rx->select = (Select *) my_realloc((Select *) rx->select,
                                           (rx->count + 1) * sizeof(Select));

        /* keep the values in ascending order */
        for (ind = rx->count; ind >= 0; ind--)
            if ((ind == 0) ||
                (rx->select[ind - 1].file < selector_index) ||
                ((rx->select[ind - 1].file == selector_index) &&
                 (rx->select[ind - 1].index < value))) {
                rx->select[ind].file = selector_index;
                rx->select[ind].index = value;
                break;
            }
            else
                rx->select[ind] = rx->select[ind - 1];

        rx->count++;
    }
}


/* selector booleans */
int select_lt(Select s1, Select s2)
{
    return ((s1.file < s2.file) ||
            ((s1.file == s2.file) && (s1.index < s2.index)));
}


int select_eq(Select s1, Select s2)
{
    return ((s1.file == s2.file) && (s1.index == s2.index));
}


/* perform set intersection */
void and(Result r1, Result r2, Result *r3)
{
    int ind1 = 0, ind2 = 0;
    Result rx;
                  
    rx.count = 0;
    rx.select = NULL;

    for (;;) {
        if ((ind1 == r1.count) || (ind2 == r2.count))
            break;
        else if (select_lt(r1.select[ind1], r2.select[ind2])) ind1++;
        else if (select_lt(r2.select[ind2], r1.select[ind1])) ind2++;
        else if (select_eq(r1.select[ind1], r2.select[ind2])) {
            rx.select = (Select *) my_realloc((Select *) rx.select,
                                              (rx.count + 1) * sizeof(Select));
            rx.select[rx.count].file = r1.select[ind1].file;
            rx.select[rx.count].index = r1.select[ind1].index;
            rx.count++;
            ind1++; ind2++;
        }
    }
    if (r3->select) free(r3->select);
    *r3 = rx;

}


/* perform set inclusion */
void or(Result r1, Result r2, Result *r3)
{
    int ind1 = 0, ind2 = 0;
    Result rx;
                  
    rx.count = 0;
    rx.select = NULL;

    for (;;) {
        if ((ind1 == r1.count) && (ind2 == r2.count))
            break;
        else if ((ind2 == r2.count) ||
                 ((ind1 < r1.count) &&
                  select_lt(r1.select[ind1], r2.select[ind2]))) {
            rx.select = (Select *) my_realloc((Select *) rx.select,
                                              (rx.count + 1) * sizeof(Select));
            rx.select[rx.count].file = r1.select[ind1].file;
            rx.select[rx.count].index = r1.select[ind1].index;
            rx.count++;
            ind1++;
        }
        else if ((ind1 == r1.count) ||
                 ((ind2 < r2.count) &&
                  select_lt(r2.select[ind2], r1.select[ind1]))) {
            rx.select = (Select *) my_realloc((Select *) rx.select,
                                              (rx.count + 1) * sizeof(Select));
            rx.select[rx.count].file = r2.select[ind2].file;
            rx.select[rx.count].index = r2.select[ind2].index;
            rx.count++;
            ind2++;
        }
        else if (select_eq(r1.select[ind1], r2.select[ind2])) {
            rx.select = (Select *) my_realloc((Select *) rx.select,
                                              (rx.count + 1) * sizeof(Select));
            rx.select[rx.count].file = r1.select[ind1].file;
            rx.select[rx.count].index = r1.select[ind1].index;
            rx.count++;
            ind1++; ind2++;
        }
    }

    if (r3->select) free(r3->select);
    *r3 = rx;

}


/* perform set exclusion */
void not(Result r1, Result r2, Result *r3)
{
    int ind1 = 0, ind2 = 0;
    Result rx;
                  
    rx.count = 0;
    rx.select = NULL;

    for (;;) {
        if (ind1 == r1.count)
            break;
        else if ((ind2 == r2.count) ||
                 select_lt(r1.select[ind1], r2.select[ind2])) {
            rx.select = (Select *) my_realloc((Select *) rx.select,
                                              (rx.count + 1) * sizeof(Select));
            rx.select[rx.count].file = r1.select[ind1].file;
            rx.select[rx.count].index = r1.select[ind1].index;
            rx.count++;
            ind1++;
        }
        else if (select_lt(r2.select[ind2], r1.select[ind1])) ind2++;
        else if (select_eq(r1.select[ind1], r2.select[ind2])) {
            ind1++; ind2++;
        }
    }
    if (r3->select) free(r3->select);
    *r3 = rx;

}


/* parse factor */
int factor(char *qstr, Result *result, Token *op)
{
    Token token;
    int query();

    get_token(qstr, &token);
    if (strcmp(token.str, "(") == 0) {
        if (!query(qstr, result, op)) {
            return (0);
        }
        if (strcmp(op->str, ")") != 0) {
            return (0);
        }
    }
    else
        find(token, result);

    free(token.str);
    get_token(qstr, op);
    return (1);
}


/* parse term */
int term(char *qstr, Result *result, Token *op)
{
    Result temp;
    Token token;

    if (factor(qstr, result, op)) {
        while ((strcmp(op->str, "and") == 0) ||
	       (strcmp(op->str, "not") == 0)) {
            if (!factor(qstr, &temp, &token))
                return (0);
            if (strcmp(op->str, "and") == 0)
                and(*result, temp, result);
            else if (strcmp(op->str, "not") == 0)
                not(*result, temp, result);
            free(op->str);
            *op = token;
            free(temp.select);
        }
        return (1);
    }
    return (0);
}


/* parse expression */
int expr(char *qstr, Result *result, Token *op)
{
    Result temp;
    Token token;

    if (term(qstr, result, op)) {
        while (strcmp(op->str, "or") == 0) {
            if (!term(qstr, &temp, &token))
                return (0);
            or(*result, temp, result);
            free(op->str);
            *op = token;
            free(temp.select);
        }
        return (1);
    }
    return (0);
}

/* parse query */
int query(char *qstr, Result *result, Token *op)
{
    Result result2;
    char temp_qstr[MAX_QUERY];
    int status;

    status = expr(qstr, result, op);     /* evaluate the expr                */
    while ((strlen(op->str) > 0) &&      /* non-boolean at end of expression */
           (strcmp(op->str, ")") != 0)) {/* and it isn't a ")"               */
        strcpy(temp_qstr, op->str);       /* put token          */
        strcat(temp_qstr, " ");           /* back on the        */
        strcat(temp_qstr, qstr);          /* front of the query */
        strcpy(qstr, temp_qstr);
        free(op->str);
        status = expr(qstr, &result2, op);
        and(*result, result2, result);    /* 'and' expressions together */
        free(result2.select);
    }
    free(op->str);

    return (status);
}


/* read selector record */
void find_selector(Select sel)
{
    int status;
    static int current_file = 0;

    sprintf(index_field, "%0*d", index_size[sel.file], sel.index);

    if (sel.file != current_file) {
        current_file = sel.file;
        status = sys$close(&selfab);
        open_selector(selector_name[current_file]);
    }
    selrab.rab$b_rac = RAB$C_KEY;
    selrab.rab$b_krf = 0;
    selrab.rab$l_kbf = index_field;
    selrab.rab$l_rop = 0;            /* set up exact match */
    selrab.rab$b_ksz = index_size[sel.file];

    /* find the selector record */
    if (((status = sys$get(&selrab)) & 1) != SS$_NORMAL)
        LIB$STOP(status);

    *(char *)(selrab.rab$l_ubf+selrab.rab$w_rsz) = '\0'; /* terminate string */
}

void select_result(Result result)
{
    int ind, start, end, fname_len;
    char *ptr1, *ptr2, *ptr3, *ptr4, file_name[SPEC_SIZE], selector[SPEC_SIZE];
    char inputline[SPEC_SIZE];
    FILE *fp;

    /* set up the HTML rendition */
    cgi_printf("Content-Type: text/html\n\n");
    cgi_printf("<HTML>\n<HEAD>\n");
    cgi_printf("<TITLE>VMS Indexed Database Search</TITLE>\n");
    cgi_printf("</HEAD>\n<BODY>\n");
    cgi_printf("<H1>%s</H1>\n", orig_qstr);
    cgi_printf("There are %d topics found:\n<p>\n", result.count);
    cgi_printf("<OL>\n");

    /* list all the selectors  selector formats in selrab.raab$l_ubf:
     *		<kkkk>0(title)<TAB>0(filename)[<TAB>host<TAB>port]]
     *		<kkkk>0(title)<TAB>R(nnn-mmm-filename)[<TAB>host<TAB>port]]
     */
    for (ind = 0; ind < result.count; ind++) {
        find_selector(result.select[ind]);
        ptr1 = (char *) (selrab.rab$l_ubf +
			 index_size[result.select[ind].file] + 1);
        ptr2 = strchr(ptr1, '\t');
        if ( ptr2 ) *ptr2++ = '\0'; else ptr2 = "";	/* filename */
        ptr3 = (*ptr2) ? strchr(ptr2, '\t') : (char *) 0;
        if ( ptr3 ) *ptr3++ = '\0'; else ptr3 = "";	/* host */
        ptr4 = (*ptr3) ? strchr(ptr3, '\t') : (char *) 0;
        if ( ptr4 ) {
	    *ptr4++ = '\0'; 				/* port */
	    if ( strcmp(ptr4,"80") ) ptr4 = "";		/* is default */
	} else ptr4 = "";	/* port */

        /* final adjust on filename */
	if ( *ptr2 == 'R' ) { 
		/* Move ptr2 to skip range numbers (Rnnn-mmm-filename). */
	    char *tmp;
	    tmp = strchr ( ptr2, '-' );
	    if ( tmp ) tmp = strchr(tmp+1,'-' );
	    if ( tmp ) ptr2 = tmp + 1;
	    else {
		   /* format error in selector!! */
	    }
	}
	else if ( *ptr2 ) ptr2++;	/* skip first char (0) */

        cgi_printf ( "<LI><A HREF=\"http://%s%s%s%s\"\n>%s</A>\n", 
	    *ptr3 ? ptr3 : Host, *ptr4 ? ":" : "", *ptr4 ? ptr4 : "",
		SHELL_TRANSLATE_VMS(ptr2), ptr1 );
    }

    /* complete the HTML rendition */
    cgi_printf("</OL>\n</BODY><iSINDEX></HTML>\n");
    return;

}

char * URLescape(char * str)
{
#define ACCEPTABLE(a)	( a>=32 && a<128 && ((isAcceptable[a-32]) & 1))
    char *p;
    char *q;
    char *result;
    int unacceptable = 0;
    for(p=str; *p; p++)
        if (!ACCEPTABLE((unsigned char)*p))
		unacceptable++;
    result = (char *) malloc(p-str + unacceptable+ unacceptable + 1);
    for(q=result, p=str; *p; p++) {
    	unsigned char a = *p;
	if (!ACCEPTABLE(a)) {
	    *q++ = '%';	/* Means hex commming */
	    *q++ = hex[a >> 4];
	    *q++ = hex[a & 15];
	}
	else *q++ = *p;
    }
    *q++ = '\0';			/* Terminate */
    if (strlen(result) > SPEC_SIZE)
        result[SPEC_SIZE] = '\0';
    return(result);
}

int strncasecomp(char *a, char *b, int n)
{
	char *p = a;
	char *q = b;
	
	for(p=a, q=b;; p++, q++) {
	    int diff;
	    if (p == a+n) return 0;	/*   Match up to n characters */
	    if (!(*p && *q)) return *p - *q;
	    diff = tolower(*p) - tolower(*q);
	    if (diff) return diff;
	}
}
