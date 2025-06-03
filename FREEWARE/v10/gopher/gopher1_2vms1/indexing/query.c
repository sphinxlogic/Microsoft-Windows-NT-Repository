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

*/

/* Usage: search idx-file out-file query host port directory */

/* Query:  expr {expr}              implicit 'and' between expressions */
/* Expr:   term {or term}                                              */
/* Term:   factor {and|not factor}                                     */
/* Factor: (query) | token                                             */
/* Token:  [field name] word                                           */
/* Word:   a-z{a-z}[*]                                                 */


#include <ssdef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <rms.h>
#include <descrip.h>
#include <fscndef.h>
#include <unixlib.h>
#include <lib$routines.h>
#include <starlet.h>

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


int query(char *, Result *, Token *);
void emit(Select, char*, char*);
void and(Result, Result, Result *);
void or(Result, Result, Result *);
void select_result(Result);
void *my_realloc(void *, int);

struct FAB idxfab, selfab;
struct RAB idxrab, selrab;
struct XABSUM xabsum;
FILE *outfile;
char *index_type = ".IDX", *selector_type = ".SEL";
int index_offset, keys, max_key = 0, last_field = 0;
char *index_field;
struct XABKEY **keytab;
int selector_index = 0;  /* selector file index */
char **selector_name;    /* array of selector file names */
int *index_size;         /* array of selector file index field sizes */

int main(int argc, char *argv[])
{
    int status, ind, size, RunQuery, context = 0;
    static char input_spec[SPEC_SIZE], idx_spec[SPEC_SIZE], file_name[SPEC_SIZE];
    char *cp, orig_qstr[MAX_QUERY], qstr[MAX_QUERY];
    Result file_result, result;
    Token token;
    struct fscndef scan_list[] = {{(short) 0, (short) FSCN$_NODE, (long) 0},
                                  {(short) 0, (short) FSCN$_DEVICE, (long) 0},
                                  {(short) 0, (short) FSCN$_DIRECTORY, (long) 0},
                                  {(short) 0, (short) FSCN$_NAME, (long) 0},
                                  {(short) 0, (short) FSCN$_VERSION, (long) 0},
                                  {(short) 0, (short) 0, (long) 0}};

    $DESCRIPTOR(input_dsc, input_spec);
    $DESCRIPTOR(idx_dsc, idx_spec);
    $DESCRIPTOR(file_name_dsc, file_name);

    RunQuery = (argc < 4);  /* verb & 2 args means interactive query */

    /* assume that the wrapper command file handles validation */
    if (!RunQuery && (argc != 6)) {
        printf("Usage: query idx-file out-file query-string host port\n");
        exit(1);
    }

    if (RunQuery && argc < 2) {
        argv[1] = (char *) malloc(80);
        printf("Index file to search: ");
        fgets(argv[1], 80, stdin);
        argv[1][strlen(argv[1]) - 1] = '\0';  /* remove \n */
    }

    if (RunQuery && argc < 3) {
        printf("Enter query: ");
        fgets(orig_qstr, MAX_QUERY, stdin);
        orig_qstr[strlen(orig_qstr) - 1] = '\0';  /* remove \n */
    }
    else if (argc == 3)
        strcpy(orig_qstr, argv[2]);
    else
        strcpy(orig_qstr, argv[3]);  /* make our copy of the query */

    for (cp = orig_qstr; *cp; cp++) {
/*
   Now that the indexer punctuation set is programmable,
   we can't afford to omit punctuation any more

        if (ispunct(*cp) && (*cp != '(') && (*cp != ')') && (*cp != '*'))
            *cp = ' ';    { convert punct. except '(' ')' '*' to spaces }
*/
        *cp = _tolower(*cp);  /* force word lowercase */
    }
    strcat(orig_qstr, " ");          /* query ends with a space */

    result.count = 0;           /* init result */
    result.select = NULL;

    strcpy(input_spec, argv[1]);  /* set up descriptor to wildcard input spec */
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

    while (((status = lib$find_file(&idx_dsc, &file_name_dsc, &context, 0, 0, 0, 0))
           & 1) == SS$_NORMAL) {       /* while lib$find_file finds file names */

        if (DEBUG)
            printf("Find_file returned %s\n", file_name);

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
        if (DEBUG)
            printf("Saving selector index %d = %s\n", selector_index,
                   selector_name[selector_index]);
        
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
            printf("Idx file must be indexed\n");
            exit(1);
        }
        
        keys = xabsum.xab$b_nok;
        if (DEBUG)
            printf("Number of keys = %d\n", keys);
        
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
            strncpy((char *) keytab[ind]->xab$l_knm, "", KEY_NAME + 1);
            if (ind > 0) keytab[ind - 1]->xab$l_nxt = (char *) keytab[ind];
        }
        idxfab.fab$l_xab = (char *) keytab[0];
        
        /* fill in the key XABs */
        if (((status = sys$display(&idxfab)) & 1) != SS$_NORMAL)
            lib$stop(status);
        
        index_size = (int *) my_realloc((int *) index_size,
                                        (selector_index + 1) * sizeof(int));

        if (keys == 1) {
            index_size[selector_index] = idxfab.fab$w_mrs - keytab[0]->xab$b_siz0;
            index_offset = keytab[0]->xab$w_pos0 + keytab[0]->xab$b_siz0 -
                           index_size[selector_index];
            index_field = (char *) my_realloc((char *) index_field,
                                   (index_size[selector_index] + 1) * sizeof(char));
            strncpy(index_field, "", index_size[selector_index] + 1);
        }
        else   /* multi-key records don't have counts */
            index_size[selector_index] = 0;

        if (DEBUG)
            printf("Selector %d index size = %d\n", selector_index,
                   index_size[selector_index]);

        for (ind = 0; ind < keys; ind++) {
            if (DEBUG)
                printf("key %d: size = %d name = %s\n",
                       ind, 
                       keys == 1 ? keytab[ind]->xab$b_siz0 - 
                                   index_size[selector_index] :
                                   keytab[ind]->xab$b_siz0,
                                   keytab[ind]->xab$l_knm);
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
        
        strcpy(qstr, orig_qstr);            /* query and friends mangle qstr */
        query(qstr, &file_result, &token);  /* evaluate the query on this file */
        or(result, file_result, &result);   /* accumulate results */
        
        status = sys$close(&idxfab);
    }  /* while finding input files */
    
    if (RunQuery)
        select_result(result);
    else {
        /* open the output file and write the resulting selector set */
        outfile = fopen(argv[2], "a");
        if (outfile == NULL) {
            perror("Output file could not be opened");
            exit(1);
        }
        for (ind = 0; ind < result.count; ind++)
            emit(result.select[ind], argv[4], argv[5]);
        fclose(outfile);
    }
    status = sys$close(&selfab);
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
        printf("Selector file must be indexed\n");
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

    if (DEBUG)
        printf("Token: '%s'  ", qstr);
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
                str = (char *) calloc(keytab[krf]->xab$b_siz0 + 1, sizeof(char));
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
    if (DEBUG)
        printf("Returns: '%s' Op: '%s' %d\n", qstr, str, krf);
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
        if (MEM_DEBUG)
            printf("Called realloc(%X, %d)\n", mem, size);
        mem_ptr = (void *) realloc(mem, size);
        if (MEM_DEBUG)
            printf("Realloc returned %X\n", mem_ptr);
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
        if (strncmp((char *) (idxrab.rab$l_ubf + keytab[token.krf]->xab$w_pos0),
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
                (rx->select[ind].file == selector_index))  /* if the value already there */
                break;                        /* don't add it */
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
    if (DEBUG) {
        printf("Find: %s -> ", token.str);
        for (ind = 0; ind < rx->count; ind++)
            printf("%d-%d ", rx->select[ind].file, rx->select[ind].index);
        printf("\n");
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

    if (DEBUG) {
        int ind;
        printf("R1: ");
        for (ind = 0; ind < r1.count; ind++)
            printf("%d-%d ", r1.select[ind].file, r1.select[ind].index);
        printf("\n");
        printf("R2: ");
        for (ind = 0; ind < r2.count; ind++)
            printf("%d-%d ", r2.select[ind].file, r2.select[ind].index);
        printf("\n");
    }

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

    if (DEBUG) {
        int ind;
        printf("AND: ");
        for (ind = 0; ind < rx.count; ind++)
            printf("%d-%d ", rx.select[ind].file, rx.select[ind].index);
        printf("\n");
     }
}


/* perform set inclusion */
void or(Result r1, Result r2, Result *r3)
{
    int ind1 = 0, ind2 = 0;
    Result rx;
                  
    rx.count = 0;
    rx.select = NULL;

    if (DEBUG) {
        int ind;
        printf("R1: ");
        for (ind = 0; ind < r1.count; ind++)
            printf("%d-%d ", r1.select[ind].file, r1.select[ind].index);
        printf("\n");
        printf("R2: ");
        for (ind = 0; ind < r2.count; ind++)
            printf("%d-%d ", r2.select[ind].file, r2.select[ind].index);
        printf("\n");
    }

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

    if (DEBUG) {
        int ind;
        printf("OR: ");
        for (ind = 0; ind < rx.count; ind++)
            printf("%d-%d ", rx.select[ind].file, rx.select[ind].index);
        printf("\n");
     }
}


/* perform set exclusion */
void not(Result r1, Result r2, Result *r3)
{
    int ind1 = 0, ind2 = 0;
    Result rx;
                  
    rx.count = 0;
    rx.select = NULL;

    if (DEBUG) {
        int ind;
        printf("R1: ");
        for (ind = 0; ind < r1.count; ind++)
            printf("%d-%d ", r1.select[ind].file, r1.select[ind].index);
        printf("\n");
        printf("R2: ");
        for (ind = 0; ind < r2.count; ind++)
            printf("%d-%d ", r2.select[ind].file, r2.select[ind].index);
        printf("\n");
    }

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

    if (DEBUG) {
        int ind;
        printf("NOT: ");
        for (ind = 0; ind < rx.count; ind++)
            printf("%d-%d ", rx.select[ind].file, rx.select[ind].index);
        printf("\n");
     }
}


/* parse factor */
int factor(char *qstr, Result *result, Token *op)
{
    Token token;

    if (DEBUG)
        printf("Factor\n");

    get_token(qstr, &token);
    if (strcmp(token.str, "(") == 0) {
        if (!query(qstr, result, op)) {
            if (DEBUG)
                printf("Query failed; exit Factor with failure\n");
            return (0);
        }
        if (strcmp(op->str, ")") != 0) {
            if (DEBUG)
                printf("Query success, no rparen; exit Factor with failure\n");
            return (0);
        }
    }
    else
        find(token, result);

    free(token.str);
    get_token(qstr, op);
    if (DEBUG)
        printf("Exit Factor\n");
    return (1);
}


/* parse term */
int term(char *qstr, Result *result, Token *op)
{
    Result temp;
    Token token;

    if (DEBUG)
        printf("Term\n");

    if (factor(qstr, result, op)) {
        while ((strcmp(op->str, "and") == 0) || (strcmp(op->str, "not") == 0)) {
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
        if (DEBUG)
            printf("Exit Term\n");
        return (1);
    }
    if (DEBUG)
        printf("Exit Term with failure\n");
    return (0);
}


/* parse expression */
int expr(char *qstr, Result *result, Token *op)
{
    Result temp;
    Token token;

    if (DEBUG)
        printf("Expr\n");

    if (term(qstr, result, op)) {
        while (strcmp(op->str, "or") == 0) {
            if (!term(qstr, &temp, &token))
                return (0);
            or(*result, temp, result);
            free(op->str);
            *op = token;
            free(temp.select);
        }
        if (DEBUG)
            printf("Exit Expr\n");
        return (1);
    }
    if (DEBUG)
        printf("Exit Expr with failure\n");
    return (0);
}

/* parse query */
int query(char *qstr, Result *result, Token *op)
{
    Result result2;
    char temp_qstr[MAX_QUERY];
    int status;

    if (DEBUG)
        printf("Query\n");

    status = expr(qstr, result, op);      /* evaluate the expr */
    while ((strlen(op->str) > 0) &&       /* non-boolean at end of expression */
           (strcmp(op->str, ")") != 0)) { /* and it isn't a ")" */
        if (DEBUG)
            printf("Found \"%s\" at end of expression; 'and' assumed\n", op->str);
        strcpy(temp_qstr, op->str);       /* put token */
        strcat(temp_qstr, " ");           /* back on the */
        strcat(temp_qstr, qstr);          /* front of the query */
        strcpy(qstr, temp_qstr);
        free(op->str);
        status = expr(qstr, &result2, op);
        and(*result, result2, result);    /* 'and' expressions together */
        free(result2.select);
    }
    free(op->str);
    if (DEBUG)
        printf("Exit Query\n");

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

    * (char *) (selrab.rab$l_ubf + selrab.rab$w_rsz) = '\0'; /* terminate string */
}


/* lookup selector with key 'index'; print selector, host, port */
void emit(Select sel, char *host, char *port)
{
    if (DEBUG)
        printf("Selector %d\n", sel.index);

    find_selector(sel);
    /*
       this assumes that the first 'index_size' characters of the selector 
       record is the selector index field and the rest is the selector itself
    */
    fprintf(outfile, "%s\t%s\t%s\n", (char *) (selrab.rab$l_ubf +
            index_size[sel.file]), host, port);
}


void select_result(Result result)
{
    int ind, start, end;
    char *ptr1, *ptr2, file_name[SPEC_SIZE];
    char gtype, inputline[SPEC_SIZE], answer[20];
    FILE *fp = NULL;

    printf("There are %d topics found:\n", result.count);

    /* list all the selectors */
    for (ind = 0; ind < result.count; ind++) {
        find_selector(result.select[ind]);
        ptr1 = (char *) (selrab.rab$l_ubf +
			 index_size[result.select[ind].file] + 1);
        ptr2 = strchr(ptr1, '\t');
        *ptr2 = '\0';
        printf ("%d.  %s\n", ind + 1, ptr1);
    }

    /* display the topics selected */
    for (;;) {
        do {
            ind = 0;  /* if non-numeric, ind will stay 0 */
            printf("\nSelect topic to view [0 to quit]: ");
            fgets(answer, 10, stdin);
            sscanf(answer, "%d", &ind);
        } while ((ind < 0) || (ind > result.count));
        if (ind == 0)
            return;

        find_selector(result.select[ind - 1]);
        ptr1 = (char *) (selrab.rab$l_ubf +
			 index_size[result.select[ind-1].file] + 1);
        ptr2 = strchr(ptr1, '\t');
	if ((gtype = *(ptr2+1)) == 'R') {
	    /* Section from a text database */
            sscanf(ptr2 + 2, "%d-%d-%s", &start, &end, file_name);
            fp = fopen(file_name, "r", "shr=get", "mbc=32");
            fseek(fp, start, SEEK_SET);
            printf("\n\nThis is from the document %s\n\n", file_name);
            while (fgets(inputline, sizeof(inputline), fp) != NULL) {
                printf("%s", inputline);
	        if (ftell(fp) >= end)
                    break;
	    }
	} else if (gtype == '0') {
	    /* Whole text file */
	    strcpy(file_name, ptr2+2);
	    fp = fopen(file_name, "r", "shr=get", "mbc=32");
            while (fgets(inputline, sizeof(inputline), fp) != NULL)
                printf("%s", inputline);
	} else
	    /* Binary or other, non-plain text */
	    printf("\n\nThis item is not plain text.\n");
        if (fp)
	    fclose(fp);
    }
}

