/*
** DTquery.c  -- Modification for the DECthreads server of my WWWquery.c
**		  for the CERN server:
**		  (NOTE: If you want to indicate file types via icons, for
**		   the DECthreads server place the CERN foo.mbx files in
**		   a directory for which /icons/* is mapped, with a pass
**		   rule. - FM)
**
** WWWquery.c -- Apply a boolean query to (a) keyword file(s), and return a
**		  hit list, a requested section with HTML "packaging", or a
**		  "raw" HTML, text or binary file from an indexed file set.
**		 The symbol WWW_SHOW_FILETYPE can be made non-NULL by the
**		  calling script to have icons (or ALT text) indicating the
**		  file types to be inserted into the hit lists returned from
**		  searches.
**		 The symbol WWW_SEND_RAW_FILE can be made non-NULL by the
**		  calling script for requested files to be sent raw (i.e.,
**		  without additional headers and PRE tags).  The hit lists
**		  for such files have direct URL's.  If a ?TEXT=R... URL is
**		  used to fetch them from gerry-rigged URL's, it will be
**		  redirected via "Location:", so that the server will check
**		  access authorization via it's native procedures.  Otherwise,
**		  the path to the document must begin with "www_root" or
**		  "gopher_root" (case-insensitive) or the client will be
**		  sent a 403 error message.
**		 The symbol WWW_OMIT_PRE_TAG can be made non-NULL by the
**		  calling script to have document sections returned without
**		  PRE tags bounding them, but with the other HTML "packaging"
**		  retained.
**		 The symbol WWW_MAXIMUM_HITS can be made non-NULL by the
**		  calling script to set a maximum number of hits to be
**		  returned to the client.
**		 The symbol WWW_SHOW_SIZE can be made non-NULL by the calling
**		  script to have the size of each file or database section
**		  indicated in the hit lists.
**		 The symbol WWW_SHOW_DATE can be made non-NULL by the calling
**		  script to have the dates of files (or date of the database)
**		  indicated in the hit lists (DD-MMM-YY).  If WWW_SHOW_TIME
**		  also is made non-NULL, for the current year the hour and
**		  minute will be indicated instead of the year (DD-MMM HH:MM).
**		  If WWW_USE_MDATE is made non-NULL, the date of the last
**		  modification (e.g., from an APPEND) will be used.  The
**		  default is the file creation date (i.e., of the highest
**		  version).
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
   1.7a 1994/06/29 Added some include files and related stuff for DECC. - FM
   1.8 1994/11/04 Handle host/port included in selector file
   1.9 1994/12/31 Trim off host/port from file_name in select_result(),
   		   and report any failure to access file_name. - FM

*/

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
   1.2 1994/04/29 Added code for invoking the native CERN httpd and VMS
   		  gopherd security measures / authorization checks for raw
		  file requests. - FM
		  Added requirement that the path to sectional documents begin
		  with the string "www_root" or "gopher_root" for sections
		  from them to be sent (versus a 403 error message).  This
		  meets all the security measures for the VMS gopherd.  For
		  sectional databases in the httpd data tree(s), you can
		  implement additional authorization checks only for access to
		  the htbin script.  The only subsequent security check is the
		  requirement that the database reside in a secure path, based
		  on it beginning with one of the above two strings.
		  For raw file requests, if a lead "www_root" string is in a
		  device field, a lead "000000." is inserted into the
		  directory field (if it was not already present), and all
		  paths passed back to the httpd for checking are first
		  converted to a www symbolic hierarchy - FM
   1.3 1994/04/30 Allow both "TEXT= R" and "TEXT=R" as flag for a file or
   		  database section fetch. - FM
		  Fix up misuse of argv[1], and return error messages if
		  maximum argument lengths are exceeded. - FM
   1.4 1994/05/06 Various fixes to path and filename handling. - FM
   1.4a 1994/06/29 Added some include files and related stuff for DECC. - FM
       1994/08/22 Modified to return direct URL's if WWW_SEND_RAW_FILE
       		  was defined, avoiding the need for redirection via
		  Location: on requests for the file. - FM
       1994/10/03 Added WWW_MAXIMUM_HITS symbol for setting the maximum
       		  number of hits to return to the client, if made non-NULL
		  in the calling script. - FM
       1994/10/13 Added code for handling both text and binary files in
       		  searches, based on Bruce Tanner's modification of the
		  indexer (build_index.c) for indexing of binary files. - FM
       		  Added WWW_SHOW_FILETYPE symbol for inserting icons (or ALT
       		  text) into hit lists to indicate the file types.  Use this
		  if your indexes include binary file types. - FM
       1994/10/14 Fixed select_result() to handle name and path fields of
       		   of any size in the .SEL files. - FM
       1994/10/16 Added code for WWW_SHOW_SIZE, WWW_SHOW_DATE, WWW_SHOW_TIME,
       		   WWW_USE_MDATE symbols - FM.
       1994/10/17 Efficiency tweeks for latest build_index.c. - FM
   1.9 1994/12/31 Added code for handling host/path in selectors. - FM

/*=========================================================================**
**
** DTquery.c Revision History:

17-Aug-1994 (FM)  Initial version.
   		  Convert '+' separators in the query to spaces, before the
		   net_unescape_string() call.
20-Aug-1994 (FM)  Use actual URL's instead of Ranges in hit lists if
		   WWW_SEND_RAW_FILE is TRUE.
03-Oct-1994 (FM)  Limit hit lists if WWW_MAXIMUM_HITS is set to a number.

13-Oct-1994 (FM)  Mods for handling binary files and option to show icons,
		   (as in 13-Oct-1994 WWWquery.c mods).
14-Oct-1994 (FM)  Fixed select_result() to handle name and path fields of
       		   of any size in the .SEL files.
16-Oct-1994 (FM)  Added code for WWW_SHOW_SIZE, WWW_SHOW_DATE, WWW_SHOW_TIME,
       		   WWW_USE_MDATE symbols.
17-Oct-1994 (FM)  Efficiency tweeks for latest build_index.c. - FM
31-Dec-1995 (FM)  Added code for handling host/path in selectors. - FM

*/

#include <ssdef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <rms.h>
#include <descrip.h>
#include <fscndef.h>
#include <unixlib.h>
#include <lib$routines.h>
#include <starlet.h>
#include <stat.h>
#include <time.h>
#include "cgilib.h"
#include "scriptlib.h"

#define KEY_NAME 32    /* maximum size of key name */
#define MAX_QUERY 500  /* maximum size of query */
#define SPEC_SIZE 256  /* file specification size */
#define MOD_REALLOC 1  /* handle initial realloc() problem */

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


int query(char *, Result *, Token *);
void and(Result, Result, Result *);
void or(Result, Result, Result *);
void select_result(Result);
void *my_realloc(void *, int);
void display_result(char *);
char *URLescape(char *);
char *HTVMS_wwwName(char *);
char *FileType(char);
void too_bad(char *);
int strcasecomp(char *, char *);
int strncasecomp(char *, char *, int);

static struct FAB idxfab, selfab;
static struct RAB idxrab, selrab;
static struct XABSUM xabsum;
static char *index_type = ".IDX", *selector_type = ".SEL";
static int index_offset, keys, max_key = 0, last_field = 0;
static char *index_field;
static struct XABKEY **keytab;
static char orig_qstr[MAX_QUERY];
static char orig_idx[SPEC_SIZE];/* buffer for index filespec from argv[1]   */
static int selector_index = 0;  /* selector file index			    */
static char **selector_name;    /* array of selector file names		    */
static int *index_size;         /* array of selector file index field sizes */
static char Host[SPEC_SIZE];    /* server_name, and port if not 80	    */
static char *MISSING_ARGUMENTS = "Script invoked with missing arguments.";
static char *QUERY_TOO_BIG     = "Query exceeds maximum length.";
static char *SPEC_TOO_BIG      = "Path exceeds maximum length.";
static char *IDX_NOT_INDEXED   = "Idx file must be indexed.";
static char *SEL_NOT_INDEXED   = "Selector file must be indexed.";

static char *hex = "0123456789ABCDEF";	/* For escaping URL's */
static unsigned char isAcceptable[96] =	/* For escaping URL's */
    /*   0 1 2 3 4 5 6 7 8 9 A B C D E F */
    {    0,0,0,0,0,0,0,0,0,0,7,6,0,7,7,4,	/* 2x   !"#$%&'()*+,-./	 */
         7,7,7,7,7,7,7,7,7,7,0,0,0,0,0,0,	/* 3x  0123456789:;<=>?	 */
	 7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,	/* 4x  @ABCDEFGHIJKLMNO  */
	 7,7,7,7,7,7,7,7,7,7,7,0,0,0,0,7,	/* 5X  PQRSTUVWXYZ[\]^_	 */
	 0,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,	/* 6x  `abcdefghijklmno	 */
	 7,7,7,7,7,7,7,7,7,7,7,0,0,0,0,0 };	/* 7X  pqrstuvwxyz{\}~	DEL */

int main(int argc, char *argv[])
{
    int status, qstr_len, ind, size, context = 0;
    static char input_spec[SPEC_SIZE], idx_spec[SPEC_SIZE];
    static char  file_name[SPEC_SIZE];
    char *cp, qstr[MAX_QUERY], *port;
    Result file_result, result;
    Token token;
    struct fscndef scan_list[] = {{(short) 0, (short) FSCN$_NODE,   (long) 0},
                                  {(short) 0, (short) FSCN$_DEVICE, (long) 0},
                                  {(short) 0, (short) FSCN$_DIRECTORY,
				  				    (long) 0},
                                  {(short) 0, (short) FSCN$_NAME,   (long) 0},
                                  {(short) 0, (short) FSCN$_VERSION,(long) 0},
                                  {(short) 0, (short) 0,            (long) 0}};

    $DESCRIPTOR(input_dsc, input_spec);
    $DESCRIPTOR(idx_dsc, idx_spec);
    $DESCRIPTOR(file_name_dsc, file_name);

    /*
    ** Validate command line arguments.
    */
    if (argc < 5) {
    /* Usage: prog method url protocol index_spec */
	too_bad(MISSING_ARGUMENTS);
	exit(1);
    }
    status = cgi_init(argc, argv);
    if ( (status&1) == 0 )
        exit(status);

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
    ** Get the index specification.
    */
    if (strlen(argv[4]) >= SPEC_SIZE) {
        too_bad(SPEC_TOO_BIG);
	net_link_close();
	exit(1);
    }
    strcpy(orig_idx, argv[4]);

    /*
    ** Get the query-string.
    */
    if ( !(cp=cgi_info("QUERY_STRING")) ) {
	too_bad(MISSING_ARGUMENTS);
	exit(1);
    }
    if (strlen(cp) >= MAX_QUERY) {
        too_bad(QUERY_TOO_BIG);
	net_link_close();
	exit(1);
    }
    strncpy(orig_qstr, cp, sizeof(orig_qstr));
    orig_qstr[sizeof(orig_qstr)-1] = '\0';
    qstr_len = strlen(orig_qstr);
    keytab = (struct XABKEY **) 0;

    /*
    ** If it's a search rather than a fetch,
    ** restore the '+' separators to spaces.
    */
    if (strncasecomp(orig_qstr, "TEXT= R", 7) &&
        strncasecomp(orig_qstr, "TEXT=R",  6)) {
        for (cp = orig_qstr; *cp; cp++)
            if (*cp == '+') *cp = ' ';
    }

    /*
    ** Convert escaped characters.
    */
    net_unescape_string(orig_qstr, &qstr_len);
    orig_qstr[qstr_len] = '\0';

    /*
    ** If it's a search rather than a fetch,
    ** force the query lowercase.
    */
    if (strncasecomp(orig_qstr, "TEXT= R", 7) &&
        strncasecomp(orig_qstr, "TEXT=R",  6)) {
        for (cp = orig_qstr; *cp; cp++)
            *cp = _tolower(*cp);
    }
    else {
        display_result(orig_idx);
	net_link_close();
	exit(1);
    }

    strcat(orig_qstr, " ");          /* query ends with a space */

    result.count = 0;           /* init result */
    result.select = NULL;

    strcpy(input_spec, orig_idx); /* set up descriptor to wildcard input spec */
    input_dsc.dsc$w_length = (short) strlen(input_spec);

    if (((status = sys$filescan(&input_dsc, scan_list, 0)) & 1) != SS$_NORMAL)
        lib$stop(status);

    cp = NULL;
    size = 0;
    for (ind = 0; ind < 4; ind++) {
        if (cp == NULL)
            cp = (char *) scan_list[ind].fscn$l_addr;
        size += scan_list[ind].fscn$w_length;
    }

    strncpy(idx_spec, cp, size);       /* copy node, dev, dir, name */
    idx_spec[size] = '\0';
    strcat(idx_spec, index_type);      /* add .idx */
    idx_dsc.dsc$w_length = (short) strlen(idx_spec);

    while (((status = lib$find_file(&idx_dsc, &file_name_dsc, &context,
    				    0, 0, 0, 0))
           & 1) == SS$_NORMAL) {  /* while lib$find_file finds file names */

        cp = strchr(file_name, ' ');
        if (cp) *cp = '\0';            /* chop off trailing spaces */
        
        /* save the file names for when we need to get the selectors */
        selector_name = (char **) my_realloc((char **) selector_name,
                                     (++selector_index + 1) * sizeof(char *));
        selector_name[selector_index] = (char *) calloc(strlen(file_name) + 1,
                                                        sizeof(char));
        cp = strrchr(file_name, '.');
        if (cp) *cp = '\0';           /* once again throw out file type */

        strcpy(selector_name[selector_index], file_name);
        
        /* initialize index fab and rab */
        idxfab = cc$rms_fab;
        idxrab = cc$rms_rab;
        xabsum = cc$rms_xabsum;
        
        idxfab.fab$l_fna = file_name;
        idxfab.fab$b_fns = strlen(file_name);
        idxfab.fab$l_dna = index_type;
        idxfab.fab$b_dns = strlen(index_type);
        idxfab.fab$b_shr = FAB$M_SHRGET;
        idxfab.fab$l_xab = (char *) &xabsum;
        
        idxrab.rab$l_fab = (struct FAB *) &idxfab;
        idxrab.rab$b_rac = RAB$C_KEY;
        
        /* open index file */
        if (((status = sys$open(&idxfab)) & 1) != SS$_NORMAL)
            lib$stop(status);
        
        if (idxfab.fab$b_org != FAB$C_IDX) {
            too_bad(IDX_NOT_INDEXED);
	    net_link_close();
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
            lib$stop(status);
        
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
        if (((status = sys$connect(&idxrab)) & 1) != SS$_NORMAL)
            lib$stop(status);
        
        strcpy(qstr, orig_qstr);           /* query and friends mangle qstr */
        query(qstr, &file_result, &token); /* evaluate query on this file   */
        or(result, file_result, &result);  /* accumulate results	    */
        
        status = sys$close(&idxfab);
    }  /* while finding input files */
    
    select_result(result);
    status = sys$close(&selfab);
    net_link_close();
    exit(1);
}


void open_selector(char *file_name)
{
    int status;

    /* initialize selector fab and rab */
    selfab = cc$rms_fab;
    selrab = cc$rms_rab;

    selfab.fab$l_fna = file_name;
    selfab.fab$b_fns = strlen(file_name);
    selfab.fab$l_dna = selector_type;
    selfab.fab$b_dns = strlen(selector_type);
    selfab.fab$b_shr = FAB$M_SHRGET;

    selrab.rab$l_fab = (struct FAB *) &selfab;
    selrab.rab$b_rac = RAB$C_KEY;

    /* open selector file */
    if (((status = sys$open(&selfab)) & 1) != SS$_NORMAL)
        lib$stop(status);

    if (selfab.fab$b_org != FAB$C_IDX) {
        too_bad(SEL_NOT_INDEXED);
	net_link_close();
        exit(1);
    }

    /* make the selector file record */
    selrab.rab$w_usz = selfab.fab$w_mrs;
    selrab.rab$l_ubf = (char *) my_realloc((char *) selrab.rab$l_ubf,
                                        (selrab.rab$w_usz + 1) * sizeof(char));
    strncpy(selrab.rab$l_ubf, "", selrab.rab$w_usz + 1);

    if (((status = sys$connect(&selrab)) & 1) != SS$_NORMAL)
        lib$stop(status);
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

    if ((mem == (void *) 0) && (MOD_REALLOC))
        return ((void *) malloc(size));
    else {
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
        lib$stop(status);

    *(char *)(selrab.rab$l_ubf+selrab.rab$w_rsz) = '\0'; /* terminate string */
}


void select_result(Result result)
{
    int ind, start, end, bytes, status;
    struct stat buf;
    char	*ThisYear;
    time_t	elapsed;
    struct tm	*local;
    char	cdate[26];
#define	MMM	cdate+4
#define DD	cdate+8
#define HHMM	cdate+11
#define	YY	cdate+22
    unsigned short send_raw = FALSE;
    unsigned short show_date = FALSE, show_time = FALSE, use_mdate = FALSE;
    unsigned short show_size = FALSE;
    char *cp, *ptr1, *ptr2, *name, *path;
    char file_name[SPEC_SIZE], Path[SPEC_SIZE], size[128], DateAndOrSize[256];
    char gtype;
    char host[SPEC_SIZE];
    FILE *fp;

    /* Check whether we're searching whole file sets. */
    if (getenv("WWW_SEND_RAW_FILE") != NULL)
        send_raw = TRUE;

    /* Check whether we've set a maximum number of hits. */
    if ((cp=getenv("WWW_MAXIMUM_HITS")) != NULL) {
        int MaxHits = atoi(cp);
	if (result.count > MaxHits)
	    result.count = MaxHits;
    }

    /* Check whether we're showing the size. */
    if ((cp=getenv("WWW_SHOW_SIZE")) != NULL)
        show_size = TRUE;

    /* Check whether we're showing the date. */
    if ((cp=getenv("WWW_SHOW_DATE")) != NULL) {
        show_date = TRUE;

        /* Do we prefer the modification date? */
        if ((cp=getenv("WWW_USE_MDATE")) != NULL)
            use_mdate = TRUE;

        /* Should we show the time in place of the current year? */
        if ((cp=getenv("WWW_SHOW_TIME")) != NULL)
            show_time = TRUE;
    }

    /* Set up the HTML rendition. */
    cgi_printf("Content-Type: text/html\n\n");
    cgi_printf("<HTML>\n<HEAD>\n");
    cgi_printf("<TITLE>VMS Indexed Database Search</TITLE>\n");
    cgi_printf("</HEAD>\n<BODY>\n");
    cgi_printf("<H1>%s</H1>\n", orig_qstr);
    cgi_printf("There are %d items found:\n<p>\n", result.count);
    cgi_printf("<OL>\n");

    /* List all the selectors. */
    for (ind = 0; ind < result.count; ind++) {
        find_selector(result.select[ind]);
        ptr1 = (char *) (selrab.rab$l_ubf +
			 index_size[result.select[ind].file]);
	gtype = *(ptr1++);
        ptr2 = strchr(ptr1, '\t');
        *ptr2 = '\0';
	name = (char *)ptr1;
	ptr1 = ptr2+1;
	bytes = 0;
	DateAndOrSize[0] = '\0';

	/* Set up the host field for the URL. */
	if ((cp=strchr(ptr1+1, '\t')) != NULL) {
	    /* Use the host and port, if not 80, from the selector. */
	    *cp = '\0';
	    strcpy(host, cp+1);
	    if ((cp=strchr(host, '\t')) != NULL) {
	        *cp = '\0';
		if (strcmp(cp+1, "80")) {
		    strcat(host, ":");
		    strcat(host, cp+1);
		}
	    }
	} else {
	    /* Use the default Host string */
	    strcpy (host, Host);
	}

        /* Get the path field for the URL. */
	if (*ptr1 != 'R') {
	    /* No range is indicated.  Use a direct URL. */
	    strcpy(file_name, ptr1+1);
	    path = HTVMS_wwwName(file_name);

	} else {
	    /* Get file_name and number of bytes from the Range selector. */
	    sscanf(ptr1+1, "%d-%d-%s", &start, &end, file_name);
	    bytes = (end - start) + 1;

	    if (send_raw == TRUE) {
	        /* Ignore the Range and use a direct URL. */
	        path = HTVMS_wwwName(file_name);

	    } else {
	        /* We're including a Range and VMS pathspecs so hex escape. */
	        path = URLescape(ptr1);
	    }
	}

	/* Path looks nicer lowercase */
	strcpy(Path, (cp=cgi_info("SCRIPT_NAME")) ? cp : "");
	for (cp=Path; *cp; cp++)
	    *cp = _tolower(*cp);

	/* Check whether to load the DateAndOrSize string. */
	if (show_date || show_size) {
	    /* Check whether we need to call stat(). */
	    if (show_date || *ptr1 != 'R')
	        status = stat(file_name, &buf);

	    /* Check whether we need to load the size string. */
	    if (show_size) {
	        if (!bytes)
	            bytes = status ? status : (int)buf.st_size;
		sprintf(size, "%d%s",
	    		(bytes >= 1000) ? (bytes+1023)/1024 : bytes,
	    		(bytes >= 1000) ? "KB" : " Bytes");
	    }

	    /* Check whether we need to load the date fields. */
	    if (show_date) {
	        if (use_mdate)
		    /* Use the last modification date. */
		    strcpy(cdate,ctime((unsigned long *) &buf.st_mtime));
		else
		    /* Default to the creation date. */
		    strcpy(cdate,ctime((unsigned long *) &buf.st_ctime));

		/* Terminate each date field. */
		cdate[3] = cdate[7] = cdate[10] = cdate[16] = cdate[24]= '\0';
		if (cdate[8]==' ')
		    cdate[8] = '0';

		/* Check whether we need to know ThisYear. */
		if (show_time) {
		    elapsed = time(&elapsed);
		    local = localtime(&elapsed);
		    ThisYear = asctime(local) + 22;
		}
	    }
            /* Load the DateAndOrSize string. */
	    if (show_date) {
	        if (show_time && show_size) {
	            if (strncmp(YY,ThisYear,2)==0)
	                sprintf(DateAndOrSize, " [%s-%s %s, %s]",
						DD, MMM, HHMM, size);
		    else
	                sprintf(DateAndOrSize, " [%s-%s-%s, %s]",
						DD, MMM, YY, size);

		} else if (show_time) {
	            if (strncmp(YY,ThisYear,2)==0)
	                sprintf(DateAndOrSize, " [%s-%s %s]",
						DD, MMM, HHMM);
		    else
	                sprintf(DateAndOrSize, " [%s-%s-%s]",
						DD, MMM, YY);

	        } else if (show_size) {
	            sprintf(DateAndOrSize, " [%s-%s-%s, %s]",
					    DD, MMM, YY, size);

	        } else {
	            sprintf(DateAndOrSize, " [%s-%s-%s]", DD, MMM, YY);
		}
	    } else if (show_size) {
	        sprintf(DateAndOrSize, " [%s]", size);
	    }
	}

        /* Output the hit. */
	if (*ptr1 != 'R' || send_raw == TRUE) {
	    /* Direct URL. */
	    cgi_printf ("<LI>%s<A HREF=\"%s%s%s\"\n>%s</A>%s\n",
	    	    FileType(gtype),
	    	    strlen(host) ? "http://" : "",
		    strlen(host) ? host : "",
		    path, name,
		    DateAndOrSize);
	} else {
	    /* Hex escaped URL with range. */
            cgi_printf ("<LI>%s<A HREF=\"%s%s%s?TEXT=%s\"\n>%s</A>%s\n",
	    	    FileType(gtype),
	    	    strlen(host) ? "http://" : "",
		    strlen(host) ? host : "",
		    Path ? Path : "/htbin/querydt",
		    path, name,
		    DateAndOrSize);
	    free(path);
	}
    }

    /* Complete the HTML rendition. */
    cgi_printf("</OL>\n</BODY><iSINDEX></HTML>\n");
    return;
}

void display_result(char *file_name)
{
    int start, end, i;
    unsigned short omit_pre = FALSE;
    char *cp, inputline[SPEC_SIZE];
    FILE *fp;

    /* Get the range and VMS filename. */
    if (orig_qstr[5] == ' ')
        i = 7;
    else
        i = 6; 
    sscanf(orig_qstr+i, "%d-%d-%s", &start, &end, file_name);

#ifdef NOTDEFINED
    /* Force any paths whose device begins with WWW_Root to begin at root. */
    if (!strncasecomp(file_name, "WWW_Root", 8) &&
        (cp=strstr(file_name, ":[")) != NULL) {
        cp += 2;
        if (*cp && strncmp(cp, "000000", 6)) {
	    strncpy(inputline, file_name, cp-file_name);
	    inputline[cp-file_name] = '\0';
	    strcat(inputline, "000000.");
	    strcat(inputline, cp);
	    strcpy(file_name, inputline);
	}
    }
#endif /* NOTDEFINED */

    /* Use Location: for raw files to implement httpd authorization checks. */
    /* 		(still need this for old or gerry-rigged URL's)		    */
    if (getenv("WWW_SEND_RAW_FILE") != NULL) {
        cgi_printf("Location: %s%s%s\n\n",
			  strlen(Host) ? "http://" : "",
			  strlen(Host) ? Host : "",
			  HTVMS_wwwName(file_name));
	return;
    }

    /* Set up the HTML rendition for document sections. */
    cgi_printf("Content-Type: text/html\n\n");
    cgi_printf("<HTML>\n<HEAD>\n");
    cgi_printf("<TITLE>VMS Indexed Database Search</TITLE>\n");
    cgi_printf("</HEAD>\n<BODY>\n");

    /* Require that the document be in a WWW_Root or Gopher_Root path. */
    if (strncasecomp(file_name, "WWW_Root", 8) &&
        strncasecomp(file_name, "Gopher_Root", 11)) {
	cgi_printf("<H1>ERROR 403</H1>\nForbidden -- by rule");
        cgi_printf("\n</BODY></HTML>\n");
	return;
    }

    /* Check whether we should omit PRE formatting. */
    if (getenv("WWW_OMIT_PRE_TAG") != NULL) {
	omit_pre = TRUE;
    }
    else
        cgi_printf("<PRE>\n");

    /* Get and send the section with appropriate further packaging. */ 
    fp = fopen(file_name, "r", "shr=get", "mbc=32");
    fseek(fp, start, SEEK_SET);
    if(!omit_pre)
        cgi_printf("This is from the document %s\n\n", file_name);
    else
	cgi_printf("<PRE>\nThis is from the document %s\n</PRE>\n", file_name);
    while (fgets(inputline, sizeof(inputline), fp) != NULL) {
        cgi_printf("%s", inputline);
	if (ftell(fp) >= end)
            break;
    }
    fclose(fp);
    if (omit_pre)
        cgi_printf("\n</BODY></HTML>\n");
    else
        cgi_printf("\n</PRE>\n</BODY></HTML>\n");
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
    return(result);
}

char *HTVMS_wwwName(char *vmsname)
{
static char wwwname[256];
char *src, *dst;
int dir;
   dst = wwwname;
   src = vmsname;
   dir = 0;
   if (strchr(src,':')) *(dst++) = '/';
   for ( ; *src != '\0' ; src++)
   {
      switch(*src)
      {
         case ':':  *(dst++) = '/'; break;
         case '-': if (dir)
	 	   {
	 	      if ((*(src-1)=='[' || *(src-1)=='.' || *(src-1)=='-') && 
		          (*(src+1)=='.' || *(src+1)=='-'))
		      {
		          *(dst++) = '/';
                          *(dst++) = '.'; 
                          *(dst++) = '.';
		      }
		      else
		          *(dst++) = '-';
		   }
		   else
		   {
		      if (*(src-1) == ']') *(dst++) = '/';
		      *(dst++) = '-';
		   }
                   break;
         case '.': if (dir)
                   {
                      if (*(src-1) != '[') *(dst++) = '/';
                   }
                   else
		   {
		      if (*(src-1) == ']') *(dst++) = '/';
                      *(dst++) = '.';
		   }
                   break;
         case '[': dir = 1; break;
         case ']': dir = 0; break;
         default:  if (*(src-1) == ']') *(dst++) = '/';
                   *(dst++) = *src; 
                   break;
      }
   }
   *(dst++) = '\0';
   return(wwwname);
}

char *FileType(char gtype)
{
static char filetype[256];

    if (getenv("WWW_SHOW_FILETYPE") == NULL) {
        filetype[0] = '\0';
	return(filetype);
    }

    switch(gtype)
    {
        case '0':
	case 'R':
	case 'h':
	case 'M':
	    sprintf (filetype, "<IMG SRC=\"%s%s%s\" ALT=\"[TXT]\"\n> ",
	    	    strlen(Host) ? "http://" : "",
		    strlen(Host) ? Host : "",
		    "/icons/text.xbm");
	    break;

	case '1':
	    sprintf (filetype, "<IMG SRC=\"%s%s%s\" ALT=\"[DIR]\"\n> ",
	    	    strlen(Host) ? "http://" : "",
		    strlen(Host) ? Host : "",
		    "/icons/directory.xbm");
	    break;

	case '4':
	    sprintf (filetype, "<IMG SRC=\"%s%s%s\" ALT=\"[HEX]\"\n> ",
	    	    strlen(Host) ? "http://" : "",
		    strlen(Host) ? Host : "",
		    "/icons/binhex.xbm");
	    break;

	case '5':
	case '9':
	    sprintf (filetype, "<IMG SRC=\"%s%s%s\" ALT=\"[BIN]\"\n> ",
	    	    strlen(Host) ? "http://" : "",
		    strlen(Host) ? Host : "",
		    "/icons/binary.xbm");
	    break;

	case '6':
	    sprintf (filetype, "<IMG SRC=\"%s%s%s\" ALT=\"[UUE]\"\n> ",
	    	    strlen(Host) ? "http://" : "",
		    strlen(Host) ? Host : "",
		    "/icons/uu.xbm");
	    break;

	case '7':
	    sprintf (filetype, "<IMG SRC=\"%s%s%s\" ALT=\"[IDX]\"\n> ",
	    	    strlen(Host) ? "http://" : "",
		    strlen(Host) ? Host : "",
		    "/icons/index.xbm");
	    break;

	case 'g':
	case 'I':
	    sprintf (filetype, "<IMG SRC=\"%s%s%s\" ALT=\"[IMG]\"\n> ",
	    	    strlen(Host) ? "http://" : "",
		    strlen(Host) ? Host : "",
		    "/icons/image.xbm");
	    break;

	case 's':
	    sprintf (filetype, "<IMG SRC=\"%s%s%s\" ALT=\"[SND]\"\n> ",
	    	    strlen(Host) ? "http://" : "",
		    strlen(Host) ? Host : "",
		    "/icons/sound.xbm");
	    break;

	case ';':
	    sprintf (filetype, "<IMG SRC=\"%s%s%s\" ALT=\"[MOV]\"\n> ",
	    	    strlen(Host) ? "http://" : "",
		    strlen(Host) ? Host : "",
		    "/icons/movie.xbm");
	    break;

	default:
	    sprintf (filetype, "<IMG SRC=\"%s%s%s\" ALT=\"[UNK]\"\n> ",
	    	    strlen(Host) ? "http://" : "",
		    strlen(Host) ? Host : "",
		    "/icons/unknown.xbm");
	    break;
    }

    return(filetype);
}

void too_bad(char *reason)
{
    /* set up the HTML rendition */
    cgi_printf("Content-Type: text/html\n\n");
    cgi_printf("<HTML>\n<HEAD>\n");
    cgi_printf("<TITLE>VMS Indexed Database Search</TITLE>\n");
    cgi_printf("</HEAD>\n<BODY>\n");

    /* display the error message */
    cgi_printf("<H1>ERROR 403</H1>\n%s", reason);
    cgi_printf("\n</BODY></HTML>\n");

    return;
}

int strcasecomp(char *a, char *b)
{
	char *p = a;
	char *q = b;
	for(p=a, q=b; *p && *q; p++, q++) {
	    int diff = tolower(*p) - tolower(*q);
	    if (diff) return diff;
	}
	if (*p) return 1;	/* p was longer than q */
	if (*q) return -1;	/* p was shorter than q */
	return 0;		/* Exact match */
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
