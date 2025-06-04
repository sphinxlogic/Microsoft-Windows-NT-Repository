/* qi_query - module for query command */
/* Bruce Tanner - Cerritos College */


/* 1993/09/14 - JLW@psulias.psu.edu fixed misextracted ATTR byte in display_id()
                                    that caused sequence numbers to spill over
                                    into ATTR                 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <ssdef.h>
#include <descrip.h>
#include <psldef.h>
#include <starlet.h>
#include <str$routines.h>
#include <lib$routines.h>
#define define_field_attributes
#include "qi.h"


typedef struct rstruct {
    int element;
    int field;
    unsigned long int id;
} Result;

extern void respond(context *, int, char *, ...);
extern void swrite(context *, char *, ...);
extern char *new_string(context *, char *);
extern Arg *make_arg(context *, char *, int, char *, int, int);
extern void free_args(Arg *);
extern void qilog(context *, int, char *, ...);
extern void *my_realloc(context *, void *, int);
extern void *check(context *, void *);
extern char *lc(context *, char *);
extern void chop(char *);
extern int is_local(context *);
extern Arg *parse_cmd(char *, context *);
extern char *soundex(char *, char *, int);
extern void crypt_start(char *);
extern char *decrypt(char *, char *);
extern void init_dsc(struct dsc$descriptor_s *, char *, int);
extern int strcasecmp(char *, char *);
extern int strncasecmp(char *, char *, int);

int get_rec(struct RAB *);


/* get field given id */
char *get_field(context *ctx, int id, char *field, int seq, int unlock)
{
    int status;

    sprintf(ctx->dat_key, "%0*d%s%0*d", ctx->id_size, id, field, SEQ_SIZE, seq);
    ctx->datrab.rab$b_rac = RAB$C_KEY;
    ctx->datrab.rab$b_krf = PRIMARY_KEY;
    ctx->datrab.rab$l_rop = 0;          /* exact match */
    ctx->datrab.rab$b_ksz = strlen(ctx->dat_key);
    if (DEBUG)
        swrite(ctx, "get_field lookup %s", ctx->dat_key);

    status = get_rec(&ctx->datrab);
    if (unlock)
        sys$release(&ctx->datrab);
    if ((status & 1) == 0) {
        if (DEBUG)
            swrite(ctx, "get_field doesn't find field %s, status %d (%X)",
                   field, status, status);
        return NULL;
    }
    ctx->dat_record[ctx->datrab.rab$w_rsz] = '\0';  /* terminate string */   
    if (strncmp(ctx->dat_key, ctx->dat_record, strlen(ctx->dat_key))) {
        if (DEBUG)
            swrite(ctx, "get field finds wrong record %s status %X",
                   ctx->dat_record, status);
        return NULL;
    }
    if (DEBUG)
        if (ctx->fields[atoi(field)].attrib & ATTR_ENCRYPTED)
            swrite(ctx, "get_field finds encrypted field %s", field);
        else
            swrite(ctx, "get_field finds field %s value %s status %X",
                   field, ctx->dat_record, status);
    return (ctx->dat_record + DAT_OH_SIZE);
}


/* get field given another field/value pair */
char *get_value(context *ctx, char *key, char *in_field, char *out_field, int seq)
{
    char *cp;
    int status, id;

    ctx->idxrab.rab$b_rac = RAB$C_KEY;
    ctx->idxrab.rab$l_rop = 0;            /* exact match */
    ctx->idxrab.rab$b_ksz = ctx->keyword_size + ctx->field_size;
    chop(key);          /* strip trailing spaces */
    sprintf(ctx->idx_key, "%-*s%s", ctx->keyword_size, cp=lc(ctx, key), in_field);
    free(cp);

    if (DEBUG)
        swrite(ctx, "get_value lookup %s", ctx->idx_key);
    status = get_rec(&ctx->idxrab);
    sys$release(&ctx->idxrab);
    if ((status & 1) == 0) {
        if (DEBUG)
            swrite(ctx, "get_value doesn't find key %s status = %d", ctx->idx_key, status);
        return NULL;
    }
    if (DEBUG)
        swrite(ctx, "get_value finds record %s", ctx->idx_record);
    id = atoi(ctx->idx_record + ctx->keyword_size + ctx->field_size);
    if (strcmp(out_field, ID_FIELD) == 0)
        return (ctx->idx_record + ctx->keyword_size + ctx->field_size);
    else
        return get_field(ctx, id, out_field, seq, True);
}


/* find index that exactly matches query */
int find_exact(context *ctx, char *query, int field)
{
    char *cp;
    int status;

    ctx->idxrab.rab$b_rac = RAB$C_KEY;
    ctx->idxrab.rab$l_rop = 0;            /* set up exact match */
    ctx->idxrab.rab$b_ksz = ctx->keyword_size + ctx->field_size;
    chop(query);                /* strip trailing spaces */
    if (field == atoi(SOUNDEX_FIELD))         /* soundex field means convert */
        soundex(query, query, SOUNDEX_SIZE);  /* and store soundex in query */

    sprintf(ctx->idx_key, "%-*s%0*d", ctx->keyword_size, query, ctx->field_size, field);

    status = get_rec(&ctx->idxrab);
    sys$release(&ctx->idxrab);
    if (DEBUG)
        swrite(ctx, "exact match returns status %d", status);
    return (status & 1);
}


/* find index that starts with the query string */
int find_approx(context *ctx, char *query, int field)
{
    int status;
    char *cp;

    ctx->idxrab.rab$b_rac = RAB$C_KEY;
    ctx->idxrab.rab$l_rop = RAB$M_KGE;     /* set up approximate generic match */
    ctx->idxrab.rab$b_ksz = strlen(query); /* actual key size */
    sprintf(ctx->idx_key, "%-*s", ctx->keyword_size, query);

    if (strlen(query) == 0)   /* special case of query '*' */
        ctx->idxrab.rab$b_ksz = 1; /* idx_key is spaces, make legal key size */

    status = get_rec(&ctx->idxrab);
    sys$release(&ctx->idxrab);
    if ((status & 1) == 0) {
        if (DEBUG)
            swrite(ctx,"approx match returns status %d", status);
        return (status & 1);
    }
    /* this should always find something, is it the right record? */
    status = ((strlen(query) == 0) ||
              (strncmp(ctx->idx_record, query, strlen(query)) == 0));
    if (DEBUG)
        swrite(ctx, "approx match finds %s and returns %d",
                ctx->idx_record, status);
    return status;
}
            

/* find soundex match */
int find_soundex(context *ctx, char *query, int *field)
{
    int status;

    /* ensure only name field is searched for soundex */
    if (*field != atoi(NAME_FIELD)) {
        if (DEBUG)
            swrite(ctx, "soundex match rejects field %d", *field);
        return 0;
    }

    *field = atoi(SOUNDEX_FIELD);
    ctx->idxrab.rab$b_rac = RAB$C_KEY;
    ctx->idxrab.rab$l_rop = 0;            /* set up exact match */
    ctx->idxrab.rab$b_ksz = ctx->keyword_size + ctx->field_size;
    sprintf(ctx->idx_key, "%-*s%s", ctx->keyword_size,
            soundex(query, query, SOUNDEX_SIZE),
            SOUNDEX_FIELD);

    status = get_rec(&ctx->idxrab);
    sys$release(&ctx->idxrab);
    if (DEBUG)
        swrite(ctx, "soundex match returns status %d", status);
    return (status & 1);
}


/* build result[] from matches to query */
int find(context *ctx, Arg *arg, Result **result, int size)
{
    int status, field;
    char *cp, query[DATA_SIZE + 1], wild_query[DATA_SIZE + 1];
    char found_keyword[DATA_SIZE + 1], found_id[20], found_field[20];
    Result *rptr, *pptr;
    struct dsc$descriptor_s wild_dsc, found_dsc;
 
    /* find() only works on indexed fields */
    if ((ctx->fields[arg->field].attrib & ATTR_INDEXED) == 0)
        return size;

    /* find() only works when there's something to find */
    if ((arg->type & TYPE_VALUE) == 0)
        return size;

    if (DEBUG)
        swrite(ctx, "Find %s in field %d", arg->value, arg->field);

    init_dsc(&wild_dsc, wild_query, sizeof(wild_query)-1);
    init_dsc(&found_dsc, found_keyword, sizeof(found_keyword)-1);
    strcpy(query, cp=lc(ctx, arg->value));
    free(cp);
    wild_query[0] = '\0';
    field = arg->field;

    for (cp = query; *cp; cp++)              /* convert all '?' to '%' */
        if (*cp == '?')                      /* STR$WILDCARD uses '%' */
            *cp = '%';
    if ((cp = strchr(query, '*')) || (cp = strchr(query, '%'))) {  /* wildcard? */
        strcpy(wild_query, query);           /* make a copy with the wildcard */
        *cp = '\0';                          /* truncate at the wildcard */
        if (!find_approx(ctx, query, field)) /* try to find the first part */
            return size;                     /* no match */
    }
    else
        if (!find_exact(ctx, query, field))     /* no wildcard, find the item */
        /* no exact match, try other matches, depending on APPROX and SOUNDEX */
            if ((APPROX && !find_approx(ctx, query, field)) &
                (SOUNDEX && !find_soundex(ctx, query, &field)))
                return size;             /* no match */

 
    ctx->idxrab.rab$b_rac = RAB$C_SEQ;
    status = RMS$_NORMAL;   /* make error checking happy */
    do {
        sys$release(&ctx->idxrab);
        strncpy(found_keyword, ctx->idx_record, ctx->keyword_size);
        found_keyword[ctx->keyword_size] = '\0';
        chop(found_keyword);
        if ((ctx->idxrab.rab$l_rop == 0) &&   /* if exact match, */
            strcmp(found_keyword, query)) /* do exact compare */
            break;  /* no match */
        if ((ctx->idxrab.rab$l_rop == RAB$M_KGE) &&   /* if approx match, */
            strncmp(found_keyword, query, strlen(query))) /* approx compare */
            break;  /* no match */
        if (strlen(wild_query)) {   /* if wildcard match, */
            found_dsc.dsc$w_length = (short) strlen(found_keyword);
            wild_dsc.dsc$w_length = (short) strlen(wild_query);
            if ((str$match_wild(&found_dsc, &wild_dsc) & 1) == 0) {  /* wild compare */
                if (DEBUG)
                    swrite(ctx, "reject %s", ctx->idx_record);
                continue;  /* no match, try again */
            }
        }
        sprintf(found_field, "%0*d", ctx->field_size, field);
        if (strncmp(found_field, ctx->idx_record + ctx->keyword_size, ctx->field_size) != 0)
            continue;  /* this isn't the field we're looking for */
        strncpy(found_id, ctx->idx_record + ctx->keyword_size +
                ctx->field_size, ctx->id_size);
        found_id[ctx->id_size] = '\0';
        if (DEBUG)
            swrite(ctx, "found %s", ctx->idx_record);
        *result = (Result *) my_realloc(ctx, (Result *) *result, (size + 1) * sizeof(Result));
        (*result)[size].id = atoi(found_id);
        (*result)[size].field = atoi(found_field);
        (*result)[size].element = arg->element;
        size++;
    } while (((status = get_rec(&ctx->idxrab)) & 1) == SS$_NORMAL);

    if ((status != RMS$_EOF) && ((status & 1) == 0))
        qilog(ctx, True, "Find %s status = %d (%X)", ctx->idx_record, status, status);

    return size;
}


/* this routine makes sure that each result element has a corresponding
   query element.  It is much too easy for "query a* foo" to match "a*"
   in two (or more) fields and none for "foo".
*/
int validate_match(context *ctx, Result *results, int end, int matched, Arg *list)
{
    Arg *ptr;
    int ind;

    /* look for an indexed query without a corresponding result */
    for (ptr = list; ptr && NotRet(ptr); ptr = ptr->next)  /* all queries */
        if (ctx->fields[ptr->field].attrib & ATTR_INDEXED) {  /* this is indexed */
            for (ind = end - matched; ind < end; ind++)
                if (results[ind].element == ptr->element)
                    break;       /* a result matched this query */
            if (ind == end)      /* didn't find a match */
                return False;    /* for this query */
        }
    return True;
}


/* see if a query field matches the id */
int lookup(context *ctx, int id, Arg *arg)
{
    int status;
    char *cp, value[DATA_SIZE + 1], data[DATA_SIZE + 1];
    struct dsc$descriptor_s value_dsc, data_dsc;

    if (DEBUG)
        swrite(ctx, "lookup %d", id);

    /* anything to look for? */
    if ((arg->type & TYPE_VALUE) == 0)
        return False;  /* no, return no match */

    init_dsc(&value_dsc, value, sizeof(value)-1);
    init_dsc(&data_dsc, data, sizeof(data)-1);

    /* first, read the data record */
    ctx->datrab.rab$b_rac = RAB$C_KEY;
    ctx->datrab.rab$b_krf = PRIMARY_KEY;
    ctx->datrab.rab$b_ksz = ctx->id_size + ctx->field_size;  /* partial key size */
    ctx->datrab.rab$l_rop = RAB$M_KGE;             /* find any sequence number */
    sprintf(ctx->dat_key, "%0*d%0*d", ctx->id_size, id, ctx->field_size, arg->field);
    while ((status = get_rec(&ctx->datrab)) & 1) {
        sys$release(&ctx->datrab);
        ctx->datrab.rab$b_rac = RAB$C_SEQ;
        if (strncmp(ctx->dat_key, ctx->dat_record, ctx->id_size + ctx->field_size))
            break;                            /* finished with this id/field */  
        ctx->dat_record[ctx->datrab.rab$w_rsz] = '\0';  /* terminate string */   
        strcpy(data, cp=lc(ctx, ctx->dat_record + DAT_OH_SIZE));
        free(cp);
        data_dsc.dsc$w_length = (short) strlen(data);
        if (DEBUG)
            if (ctx->fields[arg->field].attrib & ATTR_ENCRYPTED)
                swrite(ctx, "lookup encrypted field %d", arg->field);
            else
                swrite(ctx, "Lookup %s", ctx->dat_record);
        /* force case for compare; there's no case-blind flag to match_wild */
        if (ctx->mode & WILD_MODE)
            sprintf(value, "*%s*", cp=lc(ctx, arg->value));
        else
            strcpy(value, cp=lc(ctx, arg->value));
        free(cp);
        value_dsc.dsc$w_length = (short) strlen(value);
        if (str$match_wild(&data_dsc, &value_dsc) & 1)
            return True;  /* one match is good enough */
    }
    if ((status != RMS$_EOF) && ((status & 1) == 0))
        qilog(ctx, True, "Lookup %s status = %d (%X)", ctx->dat_record, status, status);

    return False;
}
        

/* find the non-indexed query fields in the data file 
   and see if id will match them
 */
int find_non_indexed(context *ctx, int id, Arg *list)
{
    Arg *ptr;

    for (ptr = list; ptr && NotRet(ptr); ptr = ptr->next) {
        if (ctx->fields[ptr->field].attrib & ATTR_INDEXED)
            continue;                   /* already did indexed fields */
        if (!lookup(ctx, id, ptr))
            return False;               /* no match, you lose */
    }
    return True;                        /* all fields matched */
}


/* can the current login update field of user id? */
/* field -1 means ignore the field attribute */
int can_update(context *ctx, int field, int id)
{
    char proxy[DATA_SIZE + 1], *cp;
    int ind;

    /* if not login mode, don't look for hero or proxy */
    if (ctx->login_mode != MODE_LOGIN)
        return False;        /* not login mode */

    /* in login mode, hero has priority over self */
    if (get_field(ctx, ctx->login_id, HERO_FIELD, 0, True))
        return True;        /* OK to update */

    if (ctx->login_id == id)    /* now test for self */
        return ((field == -1) ? True : ctx->fields[field].attrib & ATTR_CHANGE);

    if (cp = get_field(ctx, id, PROXY_FIELD, 0, True))  /* get proxy for id */
        strcpy(proxy, cp);
    else
        return False;   /* No proxy, don't bother checking proxies held */

    for (ind = 0; ind < MAX_SEQ; ind++) {
        if ((cp = get_field(ctx, ctx->login_id, HOLDING_FIELD, ind, True)) == NULL)
            break;
        if (strcasecmp(proxy, cp) == 0)  /* holding a proxy owner? */
            return ((field == -1) ? True : ctx->fields[field].attrib & ATTR_CHANGE);
    }

    return False;   /* Can't update */
}


int is_hero(context *ctx)
{
    /* if not login mode, don't look for hero */
    if (ctx->login_mode != MODE_LOGIN)
        return False;        /* not login mode */

    /* return hero status */
    return (get_field(ctx, ctx->login_id, HERO_FIELD, 0, True) != NULL);
}


int compare(const void *a, const void *b)
{
    if (((Result *) a)->id < ((Result *) b)->id) return -1;
    if (((Result *) a)->id > ((Result *) b)->id) return 1;
    if (((Result *) a)->field < ((Result *) b)->field) return -1;
    if (((Result *) a)->field > ((Result *) b)->field) return 1;
    return 0;
}


/* display the requested fields associated with id */
void display_id(context *ctx, int id, Arg *list, int match)
{
    Arg *ptr;
    int request[MAX_FIELD], use_defaults = True;
    int max, ind, status, num, seq, attrib, proxy_owner;
    char data[DATA_SIZE + 1], field[20];

/* first we need to know whether there are any requested fields */

    for (ind = 0; ind < MAX_FIELD; ind++)
        request[ind] = False;
    for (ptr = list; ptr; ptr = ptr->next) {
        if (ptr->type & TYPE_RETURN)
            use_defaults = False;
        else if (strcasecmp(ptr->value, "all") == 0)
            for (ind = 0; ind < MAX_FIELD; ind++)
                request[ind] = True;    /* mark all as requested */
        else if (!use_defaults)
            if (ptr->field == -1)
                respond(ctx, RESP_ONCE, "507:Field %s does not exist.", ptr->value);
            else
                request[ptr->field] = True;
    }        

/* if use_defaults = true, the Default fields will be used
   otherwise, request[] contains the fields requested
*/

    /* are we a hero or a proxy owner? */
    proxy_owner = can_update(ctx, -1, id);

    /* find the max field name size */
    for (ind = 0, max = 0; ind < MAX_FIELD; ind++)
        if (ctx->fields[ind].name && (strlen(ctx->fields[ind].name) > max))
            max = strlen(ctx->fields[ind].name);

    ctx->datrab.rab$b_rac = RAB$C_KEY;
    ctx->datrab.rab$b_krf = PRIMARY_KEY;
    ctx->datrab.rab$b_ksz = ctx->id_size;               /* partial key size */
    ctx->datrab.rab$l_rop = RAB$M_KGE;             /* find all records */
    sprintf(ctx->dat_key, "%0*d", ctx->id_size, id);
    while ((status = get_rec(&ctx->datrab)) & 1) {
        sys$release(&ctx->datrab);
        ctx->dat_record[ctx->datrab.rab$w_rsz] = '\0';  /* terminate string */   
        ctx->datrab.rab$b_rac = RAB$C_SEQ;
        if (strncmp(ctx->dat_key, ctx->dat_record, ctx->id_size))
            break;                            /* finished with this id */  
        strncpy(field, ctx->dat_record + ctx->id_size, ctx->field_size);
        field[ctx->field_size] = '\0';
        num = atoi(field);
        strncpy(field, ctx->dat_record + ctx->id_size + ctx->field_size, SEQ_SIZE);
        field[SEQ_SIZE] = '\0';
        seq = atoi(field);
        strncpy(field, ctx->dat_record + ctx->id_size + ctx->field_size + SEQ_SIZE, ATTR_SIZE);
        field[ATTR_SIZE] = '\0';
        attrib = atoi(field);
        if (ctx->fields[num].attrib & ATTR_ENCRYPTED)
            strcpy(data, "(Encrypted)");
        else
            strcpy(data, ctx->dat_record + DAT_OH_SIZE);

        /* if we want to display this field
           AND
           we're authorized to display this field
           then display the field
        */
        if (
             (
              /* this field is requested OR */
              request[num] ||
              /* no fields were requested and this field is a default */
              /* (Default implies Public) */
              (use_defaults && (ctx->fields[num].attrib & ATTR_DEFAULT))
             )
             /* (this is a field we want to look at) AND */
             && 
             (
              /* the field def is public OR */
              (
               (ctx->fields[num].attrib & ATTR_PUBLIC) &&
               ((attrib & FIELD_ATTR_MASK) == FIELD_ATTR_NONE)
              ) ||
              /* the field is public OR */
              (
               (attrib & FIELD_ATTR_MASK) == FIELD_ATTR_PUBLIC
              ) ||
              /* the field def is local and requested from a local host OR */
              (
               (
                (ctx->fields[num].attrib & ATTR_LOCALPUB) &&
                ((attrib & FIELD_ATTR_MASK) == FIELD_ATTR_NONE)
               ) &&
               (is_local(ctx))
              ) ||
              /* the field is local and requested from a local host OR */
              (
               (
                (attrib & FIELD_ATTR_MASK) == FIELD_ATTR_LOCAL
               ) &&
               (is_local(ctx))
              ) ||
              /* the field can't be suppressed OR */
              (
               ctx->fields[num].attrib & ATTR_FORCEPUB
              ) ||
              /* this is the requestor's own entry OR */
              (id == ctx->login_id) ||
              /* the requestor is the proxy owner */
              proxy_owner
             )
           ) {
            respond(ctx, RESP_MULT, "200:%d:%*s: %s", match, max,
                    seq ? "" : ctx->fields[num].name, data);
        }
/* this could be considered a privacy hole if you can tell that a field instance exists
*       else if (DEBUG)
*           swrite(ctx, "Failed display check for field %s", ctx->fields[num].name);
*/
    }
    if ((status != RMS$_EOF) && ((status & 1) == 0))
        qilog(ctx, True, "Display_id %s status = %d (%X)", ctx->dat_record, status, status);

}



/*
   given an array of indexed fields that match indexed part of query,
   return the count of actual matches with the list in results[]
*/
int resolve(context *ctx, Result *results, int size, Arg *list)
{
    Arg *ptr;
    int ind, iq = 0, matches, count = 0;

    qsort(results, size, sizeof(Result), compare); /* sort the hits */
    for (ptr = list; ptr && NotRet(ptr); ptr = ptr->next) {
        if (ctx->fields[ptr->field].attrib & ATTR_INDEXED)
            iq++;  /* Get count of indexed queries (iq) */
    }

    if (DEBUG)
        for (ind = 0; ind < size; ind++)
            swrite(ctx, "Resolve element %d field %d id %d",
                    results[ind].element, results[ind].field, results[ind].id);

    /* find sequences of 'iq' matches of one id */
    for (ind = 1, matches = 1; ind < (size + 1); ind++) {
        if ((ind < size) && (results[ind-1].id == results[ind].id))
            matches++;
        else {
            if ((matches >= iq) &&          /* if everything matches */
                validate_match(ctx, results, ind, matches, list) &&
                find_non_indexed(ctx, results[ind-1].id, list)) {
                if (DEBUG)
                    swrite(ctx, "%d matches for %d clauses: id = %d",
                           matches, iq, results[ind-1].id);
                results[count++] = results[ind-1];  /* save the id */
            }
            matches = 1;
        }
    }
    return count;
}


/*
   return number of indexed fields
   ensure that all fields exist and have lookup attribute
*/
int validate_fields(context *ctx, Arg *list)
{
    int highest = 0, indexed = 0;
    Arg *ptr, *new;
    char *cp;

    for (ptr = list; ptr && NotRet(ptr); ptr = ptr->next) {
        if (ptr->name && (ptr->field == -1)) {
            respond(ctx, RESP_ONCE, "507:Field does not exist.");
            return 0;   /* force failure */
        }
        if (ptr->field == -1) {          /* null field name is 'name' field */
            ptr->field = atoi(NAME_FIELD);  /* so use name field */
            ptr->name = new_string(ctx, ctx->fields[ptr->field].name);
        }

        /* mirror the code in qi_build.c */
        if ((ptr->field == atoi(NAME_FIELD)) && (ptr->type & TYPE_VALUE)) {

/*   get_token has already removed the comma
 *          cp = strchr(ptr->value, ',');
 *          if (cp) strcpy(cp, cp+1);
 */

#if NAME_HACK
            cp = strchr(ptr->value, '\'');
            if (cp) strcpy(cp, cp+1);  /* squeeze out apostrophe */

/* now that hyphenated names are indexed both ways, this isn't necessary
 *          cp = strchr(ptr->value, '-');
 *          if (cp) {
 *              *cp = '\0';
 *              new = make_arg(ctx, ptr->name, ptr->field, cp + 1, ptr->type,
 *                             ptr->element + 1);
 *              new->next = ptr->next;
 *              ptr->next = new;
 *          }
 */
#endif
        }
        if (ctx->fields[ptr->field].attrib & ATTR_INDEXED)
            indexed++;
        if ((ctx->fields[ptr->field].attrib & ATTR_LOOKUP) == 0) {
            respond(ctx, RESP_ONCE, "504:Not authorized for requested search criteria.");
            return 0;   /* force failure */
        }
    }
    return indexed;
}


int query_cmd(char *cmd, context *ctx)
{
    int status, ind, iq = 0, hits, size = 0;
    Arg *list, *listp, *nick;
    Result *results = NULL;

    list = parse_cmd(cmd, ctx);       /* build query struct from cmd */

    if (validate_fields(ctx, list) == 0)
        respond(ctx, RESP_ONCE, "515:No indexed field in query.");
    else {
        for (listp = list; listp && NotRet(listp); listp = listp->next) {
            if ((ctx->fields[listp->field].attrib & ATTR_INDEXED) &&
                strcmp(listp->value, "*"))
                iq++;  /* Get count of indexed non-wildcard clauses (iq) */
        }
        for (listp = list; listp && NotRet(listp); listp = listp->next) {
            /* if there are other indexed clauses, don't do wildcards */
            if ((iq > 0) && (strcmp(listp->value, "*") == 0)) {
                if (listp->prev == (Arg *)0)  /* remove this clause from list */
                    list = listp->next;
                else {
                    listp->prev->next = listp->next;
                    if (listp->next)
                        listp->next->prev = listp->prev;
                }
                if (listp->name) free(listp->name);
                if (listp->value) free(listp->value);
                free(listp);
                continue;  /* *listp is gone, skip it */
            }
            size = find(ctx, listp, &results, size);  /* record ids that match query */
            if (listp->field == atoi(NAME_FIELD)) {
                nick = make_arg(ctx, listp->name, atoi(NICKNAME_FIELD),
                                listp->value, listp->type, listp->element);
                size = find(ctx, nick, &results, size);
                free(nick);
            }
        }
    }
    if (size == 0)  /* no index matches */
        respond(ctx, RESP_ONCE, "501:No matches to query.");
    else {
        hits = resolve(ctx, results, size, list);
        if (hits == 0)
            respond(ctx, RESP_ONCE, "501:No matches to query.");
        else if (hits > MAX_RECORDS) {
            respond(ctx, RESP_ONCE, "502:Too many matches to query.");
            qilog(ctx, False, "Too many matches (%d)", hits);
        }
        else {
            respond(ctx, RESP_MULT,"102:There %s %d %s to your request.",
                hits>1 ? "were" : "was", hits, hits>1 ? "matches" : "match");
            for (ind = 0; ind < hits; ind++)
                display_id(ctx, results[ind].id, list, ind+1);
            qilog(ctx, False, "Returned %d out of %d", hits, size);
            respond(ctx, RESP_MULT, "200:Ok.");
        }
    }

    free_args(list);
    free(results);
    return True;
}


void delete_index(context *ctx, int id, int field)
{
    int status;

    sprintf(ctx->idx_key, "%0*d%0*d", ctx->id_size, id, ctx->field_size, field);
    ctx->idxrab.rab$b_krf = SECONDARY_KEY;
    ctx->idxrab.rab$b_rac = RAB$C_KEY;
    ctx->idxrab.rab$l_rop = 0;            /* set up exact match */
    ctx->idxrab.rab$b_ksz = IDX_KEY2_SIZE;
    while ((status = get_rec(&ctx->idxrab)) & 1) {
        status = sys$delete(&ctx->idxrab);
        if (DEBUG)
            if (status & 1)
                swrite(ctx, "deleted index %s, status = %d (%X)",
                       ctx->idx_record, status, status);
            else
                swrite(ctx, "could not delete index %s, status = %d (%X)",
                       ctx->idx_key, status, status);
    };
    sys$release(&ctx->idxrab);
    ctx->idxrab.rab$b_krf = PRIMARY_KEY;  /* reset the krf */
}


void delete_field(context *ctx, int id, int field)
{
    int status;
    char seq[SEQ_SIZE + 1];

    ctx->datrab.rab$l_rop = RAB$M_KGE;
    ctx->datrab.rab$b_rac = RAB$C_KEY;
    ctx->datrab.rab$b_krf = PRIMARY_KEY;
    sprintf(ctx->dat_key, "%0*d%0*d", ctx->id_size, id, ctx->field_size, field);
    ctx->datrab.rab$b_ksz = strlen(ctx->dat_key);  /* partial key size */

    while ((status = get_rec(&ctx->datrab)) & 1) {
        ctx->dat_record[ctx->datrab.rab$w_rsz] = '\0';  /* terminate string */   
        ctx->datrab.rab$b_rac = RAB$C_SEQ;
        if (strncmp(ctx->dat_record, ctx->dat_key, ctx->datrab.rab$b_ksz))
            break;
        strncmp(seq, ctx->dat_record + ctx->id_size + ctx->field_size, SEQ_SIZE);
        if (DEBUG)
            if (ctx->fields[field].attrib & ATTR_ENCRYPTED)
                swrite(ctx, "delete/get encrypted field %d, status = %d (%X)",
                       field, status, status);
            else
                swrite(ctx, "delete/get field %s, status = %d (%X)",
                       ctx->dat_record, status, status);
        status = sys$delete(&ctx->datrab);
        if (DEBUG)
            swrite(ctx, "deleted field, status = %d (%X)", status, status);
    }
    sys$release(&ctx->datrab);
}


/* delete the records for a specific ID */
int delete_entry(context *ctx, int id)
{
    char field[20], *cp;
    int status;

    /* authorized to delete this entry? */
    if (can_update(ctx, -1, id))
        /* we can delete this entry */ ;
    else {
        respond(ctx, RESP_ONCE, "510:Not authorized to delete this entry.");
        return False;
    }
    if (DEBUG)
        swrite(ctx, "delete id %d", id);

    /* delete data fields */
    ctx->datrab.rab$l_rop = RAB$M_KGE;
    ctx->datrab.rab$b_rac = RAB$C_KEY;
    ctx->datrab.rab$b_krf = PRIMARY_KEY;
    sprintf(ctx->dat_key, "%0*d", ctx->id_size, id);
    ctx->datrab.rab$b_ksz = strlen(ctx->dat_key);  /* partial key size */

    while ((status = get_rec(&ctx->datrab)) & 1) {
        ctx->datrab.rab$b_rac = RAB$C_SEQ;
        ctx->dat_record[ctx->datrab.rab$w_rsz] = '\0';  /* terminate string */   
        if (strncmp(ctx->dat_record, ctx->dat_key, ctx->datrab.rab$b_ksz))
            break;
        strncpy(field, ctx->dat_record + ctx->id_size, ctx->field_size);
        status = sys$delete(&ctx->datrab);
        if ((DEBUG) && (status & 1))
            if (ctx->fields[atoi(field)].attrib & ATTR_ENCRYPTED)
                swrite(ctx, "deleted encrypted field %s, status = %d (%X)",
                       field, status, status);
            else
                swrite(ctx, "deleted field %s, status = %d (%X)",
                       ctx->dat_record, status, status);
    }
    sys$release(&ctx->datrab);
    if ((status != RMS$_EOF) && ((status & 1) == 0))
        qilog(ctx, True, "Delete_entry %s status = %d (%X)", ctx->dat_record, status, status);

    /* delete index records */
    ctx->idxrab.rab$b_krf = SECONDARY_KEY;
    ctx->idxrab.rab$l_rop = RAB$M_KGE;
    ctx->idxrab.rab$b_rac = RAB$C_KEY;
    sprintf(ctx->idx_key, "%0*d", ctx->id_size, id);
    ctx->idxrab.rab$b_ksz = strlen(ctx->idx_key);  /* partial key size */

    while ((status = get_rec(&ctx->idxrab)) & 1) {
        ctx->idxrab.rab$b_rac = RAB$C_SEQ;
        ctx->idx_record[ctx->idxrab.rab$w_rsz] = '\0';  /* terminate string */   
        if (strncmp(ctx->idx_record + ctx->keyword_size + ctx->field_size,
            ctx->idx_key, ctx->idxrab.rab$b_ksz))
            break;
        status = sys$delete(&ctx->idxrab);
        if ((DEBUG) && (status & 1))
            swrite(ctx, "deleted index %s, status = %d (%X)",
                   ctx->idx_record, status, status);
    }
    sys$release(&ctx->idxrab);
    if ((status != RMS$_EOF) && ((status & 1) == 0))
        qilog(ctx, True, "Delete_entry 2 %s status = %d (%X)", ctx->idx_record, status, status);

    ctx->idxrab.rab$b_krf = PRIMARY_KEY;  /* reset the krf */

    return True;
}


/* decrypt field */
char *decrypt_field(context *ctx, int id, Arg *ptr)
{
    /* if we're going to do multi-record encrypted fields, */
    /* value[] will need to be allocated dynamically */
    char *cp, *pwd, *value;

    value = (char *) check(ctx, calloc(DATA_SIZE + 1, sizeof(char)));

    if ((ctx->interactive) ||      /* interactive mode can't do encryption */
        ((ctx->fields[ptr->field].attrib & ATTR_ENCRYPTED) == 0)) {
        strcpy(value, ptr->value); /* pretend that it was decrypted */
        return value;
    }

    /* at this point our password field is likely deleted */
    /* so rely on a prior crypt_start */
    decrypt(value, ptr->value);

    /* new passwords mean new crypto keys */
    if ((ptr->field == atoi(PASSWORD_FIELD)) && (id == ctx->login_id))
        crypt_start(value);  /* remember our new password */

    return value;
}


/* add field value */
void add_field(context *ctx, int id, Arg *ptr)
{
    int status, seq = 0;
    char *cp, *cp2, value[MAX_INPUT + 1];

    strcpy(value, cp=decrypt_field(ctx, id, ptr));
    free(cp);
    for (cp = cp2 = value; cp2; cp = cp2 + 2) {
        if (cp2 = strstr(cp, "\\n"))
            *cp2 = '\0';
        sprintf(ctx->dat_record, "%0*d%0*d%0*d%0*d%s",
                ctx->id_size, id, ctx->field_size, ptr->field, SEQ_SIZE, seq++,
                ATTR_SIZE, FIELD_ATTR_CHANGED, cp);

        ctx->datrab.rab$w_rsz = strlen(ctx->dat_record);
        if ((status = sys$put(&ctx->datrab)) != RMS$_NORMAL) {
            respond(ctx, RESP_ONCE, "500:Add field failed with status %d.", status);
            qilog(ctx, True, "Add %s failed status %d", ctx->dat_record, status);
        }
        else
            if (DEBUG)
                if (ctx->fields[ptr->field].attrib & ATTR_ENCRYPTED)
                    swrite(ctx, "Add encrypted field %d, status = %d (%X)",
                           ptr->field, status, status);
                else
                    swrite(ctx, "Add field %s, status = %d (%X)",
                           ctx->dat_record, status, status);
        sys$release(&ctx->datrab);
    }
}


/* write an index record */
void write_index(context *ctx, char *str, int id, int field)
{
    int status;

    if (strlen(str) >= MIN_KEYWORD) {
        sprintf(ctx->idx_record, "%-*.*s%0*d%0*d",
                ctx->keyword_size, ctx->keyword_size, str,
                ctx->field_size, field, ctx->id_size, id);
        if (DEBUG)
            swrite(ctx, "Add index record %s", ctx->idx_record);
        ctx->idxrab.rab$w_rsz = strlen(ctx->idx_record);
        if (((status = sys$put(&ctx->idxrab)) & 1) == 0) {
            respond(ctx, RESP_ONCE, "500:Add index failed with status %d.", status);
            qilog(ctx, True, "Add %s failed status %d", ctx->idx_record, status);
        }
        sys$release(&ctx->idxrab);
    }
}


/* add field index */
void add_index(context *ctx, int id, Arg *ptr)
{
    char *cp, *cp2, *cp3, *cp4, *cp5, data[DATA_SIZE + 2];
    int line = 0;

    strcpy(data, ptr->value);
    for (cp = cp2 = data; cp2; cp = cp2 + 2) {  /* for each line of index */
        line++;
        if (cp2 = strstr(cp, "\\n"))
            *cp2 = '\0';
        strcpy(data, cp);

        /* special hack to omit indexing the email domain */
        if ((ptr->field == atoi(EMAIL_FIELD)) && (cp3 = strchr(data, '@')))
            *cp3 = '\0';

        /* only index the primary alias */
        if ((line > 1) && (ptr->field == atoi(ALIAS_FIELD)))
            return;

        for (cp3 = data; *cp3; cp3++) {    /* apply any special editing to names */
            if (*cp3 == ',') strcpy(cp3, cp3+1); /* remove commas */
#if NAME_HACK
            if (ptr->field == atoi(NAME_FIELD)) { /* only edit name field */
/*              if (*cp3 == '-')  *cp3 = ' '; /* index both hyphenated names */
                if (*cp3 == '\'') strcpy(cp3, cp3+1); /* squeeze out apostrophe */
                if (*cp3 == '/')  *cp3 = ' '; /* index both slashed names */
            }
#endif
            *cp3 = _tolower(*cp3);        /* indexes are always lowercase */
        }

        strcat(data, " ");              /* line ends with a space */
        cp3 = data;
        while(cp4 = strchr(cp3, ' ')) {  /* for each word of index */
            *cp4 = '\0';
            write_index(ctx, cp3, id, ptr->field); 
#if NAME_HACK
            if ((ptr->field == atoi(NAME_FIELD)) &&
                (cp5 = strchr(cp, '-'))) {
                *cp5 = '\0';
                write_index(ctx, cp, id, ptr->field);
                write_index(ctx, cp5 + 1, id, ptr->field);
            }
#endif
            cp3 = cp4 + 1;
        }
    }
}


/* make soundex entry for field */
void make_soundex(context *ctx, int id, Arg *ptr)
{
    char newrec[DATA_SIZE + 1], data[DATA_SIZE + 2], dest[SOUNDEX_SIZE + 1];
    Arg soundex_arg = { 0, TYPE_NAME | TYPE_EQUAL | TYPE_VALUE,
                        NULL, 0, NULL, (Arg *) 0, (Arg *) 0 };
    char *cp, *cp2, *cp3;

    soundex_arg.value = newrec;
    strcpy(data, ptr->value);
    for (cp = data; *cp; cp++) {
        if (iscntrl(*cp))  *cp = ' ';    /* convert tabs to spaces */
        if (*cp == ',') strcpy(cp, cp+1); /* remove commas */
#if NAME_HACK  /* assume NAME_FIELD here */
/*      if (*cp == '-')  *cp = ' '; /* index both hyphenated names */
        if (*cp == '\'') strcpy(cp, cp+1); /* squeeze out apostrophe */
#endif
        *cp = _tolower(*cp);
    }
    while ((strlen(data) > 0) &&
           (data[strlen(data)-1] == ' '))
        data[strlen(data)-1] = '\0';     /* remove trailing blanks */

    strcat(data, " ");                   /* line ends with a space */
    strncpy(newrec, "", sizeof(newrec)); /* init soundex value */
    cp = data;
    while(cp2 = strchr(cp, ' ')) {  /* break at space boundary */
        *cp2 = '\0';
        if (strlen(cp) >= MIN_KEYWORD) {
            strcat(newrec, soundex(dest, cp, SOUNDEX_SIZE));
            strcat(newrec, " ");
        }
#if NAME_HACK
        if ((ptr->field == atoi(NAME_FIELD)) &&
            (cp3 = strchr(cp, '-'))) {
            *cp3 = '\0';
            if (strlen(cp) >= MIN_KEYWORD) {
                strcat(newrec, soundex(dest, cp, SOUNDEX_SIZE));
                strcat(newrec, " ");
            }
            if (strlen(cp3) >= MIN_KEYWORD) {
                strcat(newrec, soundex(dest, cp3, SOUNDEX_SIZE));
                strcat(newrec, " ");
            }
        }
#endif
        cp = cp2 + 1;
    }

    soundex_arg.field = atoi(SOUNDEX_FIELD);

    if (ctx->fields[atoi(SOUNDEX_FIELD)].attrib & ATTR_INDEXED)
        add_index(ctx, id, &soundex_arg);
    add_field(ctx, id, &soundex_arg);
}


/* check to see if this field will be a duplicate */
int is_duplicate(context *ctx, Arg *ptr)
{
    char *cp, *cp2, *cp3, *cp4, data[DATA_SIZE + 2];
    int  status;

    if ((ctx->fields[ptr->field].attrib & ATTR_UNIQUE) == 0)
        return False;   /* No Unique attribute, say it's not duplicate */

    if ((ctx->fields[ptr->field].attrib & ATTR_INDEXED) == 0) {
        respond(ctx, RESP_ONCE, "500:Unique field %s is not Indexed", ptr->name);
        return True;    /* Not Indexed, fail now */
    }

    strcpy(data, ptr->value);
    for (cp = cp2 = data; cp2; cp = cp2 + 2) {  /* for each line of index */
        if (cp2 = strstr(cp, "\\n"))
            *cp2 = '\0';
        strcpy(data, cp);

        for (cp3 = data; *cp3; cp3++) {  /* apply any special editing to names */
            if (*cp3 == ',') strcpy(cp3, cp3+1); /* remove commas */
            *cp3 = _tolower(*cp3);       /* indexes are always lowercase */
        }

        strcat(data, " ");               /* line ends with a space */
        cp3 = data;
        ctx->idxrab.rab$b_rac = RAB$C_KEY;
        ctx->idxrab.rab$l_rop = 0;            /* exact match */
        ctx->idxrab.rab$b_ksz = ctx->keyword_size + ctx->field_size;
        while(cp4 = strchr(cp3, ' ')) {  /* for each word of index */
            *cp4 = '\0';
            if (strlen(cp3) >= MIN_KEYWORD) {
                sprintf(ctx->idx_key, "%-*.*s%0*d",
                        ctx->keyword_size, ctx->keyword_size, cp3,
                        ctx->field_size, ptr->field);
                status = get_rec(&ctx->idxrab);  /* will this be a duplicate? */
                if (DEBUG)
                    swrite(ctx, "Dup check of %s return %d (%X)",
                           ctx->idx_key, status, status);
                if (status == RMS$_NORMAL)
                    return True;         /* Duplicate */
                else if (status != RMS$_RNF)
                    qilog(ctx, True, "Is_duplicate status = %d (%X)", status, status);
            }
            cp3 = cp4 + 1;
        }
    }
    return False;   /* no duplicate found */
}


/* modify one field */
int modify_field(context *ctx, int id, Arg *ptr, int type)
{
    int index, ind, count, seq, int_attr, status;
    char char_attr[ATTR_SIZE + 1];

    /* we need at least "... field = " */
    if ((ptr->type & TYPE_EQUAL) == 0) {
        respond(ctx, RESP_ONCE, "500:Syntax error.");
        return False;
    }

    /* authorized to change this field? */
    if (can_update(ctx, ptr->field, id))
        /* we can change this entry */ ;
    else {
        respond(ctx, RESP_ONCE, "505:%s:you may not change this field.",
               ctx->fields[ptr->field].name);
        return False;
        }
    if (DEBUG)
        swrite(ctx, "modify type %d field %d (%s) to %s",
           ptr->type, ptr->field, ptr->name ? ptr->name : "", 
           ptr->value ? ptr->value : "");

    /* is the field defined? */
    if (ptr->field == -1) {
        respond(ctx, RESP_ONCE, "507:Field %s does not exist.",
               ptr->name ? ptr->name : ptr->value);
        return False;
    }

    /* if this is a SET, do it here */
    if (type == TYPE_SET) {
        for (index = 0, count = 0; index < MAX_FIELD_ATTRIBUTES; index++)
            if (ptr->value &&
                strncasecmp(ptr->value, field_attributes[index].name,
                            strlen(ptr->value)) == 0) {
                count++;
                ind = index;
            }
        if (count != 1) {
            respond(ctx, RESP_ONCE, "507:Attribute %s does not exist.",
                   ptr->value ? ptr->value : "");
            return False;
        }
        count = 0;
        for (seq = 0; seq < MAX_SEQ; seq++) {
            status = RMS$_RNF; /* default status for get_field */
            if (get_field(ctx, id, ctx->fields[ptr->field].number,
                          seq, False) == NULL)   /* get and lock record */
                break;  /* no more instances of this id/field */
            strncpy(char_attr, ctx->dat_record + DAT_KEY_SIZE, ATTR_SIZE);
            int_attr = atoi(char_attr) & ~field_attributes[ind].mask;  /* mask off bits */
            if (field_attributes[ind].value != FIELD_ATTR_CLEAR)
                int_attr |= field_attributes[ind].value | FIELD_ATTR_CHANGED;  /* set new attrib */
            sprintf(char_attr, "%0*d", ATTR_SIZE, int_attr);
            strncpy(ctx->dat_record + DAT_KEY_SIZE, char_attr, ATTR_SIZE);
            if ((status = sys$update(&ctx->datrab)) & 1)
                count++;
            if (DEBUG)
                swrite(ctx, "Update field attributes to %s status = %d (%X)",
                       ctx->dat_record, status, status);
        }
        sys$release(&ctx->datrab);
        return (count && (status == RMS$_RNF));
    }

    /* if the field is NAME field, and it has no value, fail */
    if ((ptr->field == atoi(NAME_FIELD)) &&
        (((ptr->type & TYPE_VALUE) == 0) ||
          (strlen(ptr->value) == 0))) {
        respond(ctx, RESP_ONCE, "500:Cannot delete required field %s", ptr->name);
        return False;
    }

    /* don't duplicate a field that should be unique */
    if (is_duplicate(ctx, ptr)) {
        respond(ctx, RESP_ONCE, "509:Duplicate unique record.");
        return False;
    }

    /* if the field is indexed, remove indexes */
    if (ctx->fields[ptr->field].attrib & ATTR_INDEXED)
        delete_index(ctx, id, ptr->field);

    /* if the field exists, remove it */
    delete_field(ctx, id, ptr->field);

    /* if the field is the name field, delete soundex also */
    if (ptr->field == atoi(NAME_FIELD)) {
        if (ctx->fields[atoi(SOUNDEX_FIELD)].attrib & ATTR_INDEXED)
            delete_index(ctx, id, atoi(SOUNDEX_FIELD));
        delete_field(ctx, id, atoi(SOUNDEX_FIELD));
    }

    /* add new field and its index */
    if ((ptr->type & TYPE_VALUE) && strlen(ptr->value)) {
        if (ctx->fields[ptr->field].attrib & ATTR_INDEXED)
            add_index(ctx, id, ptr);
        add_field(ctx, id, ptr);
    }

    /* if the field is the name field, create soundex */
    if (ptr->field == atoi(NAME_FIELD))
        make_soundex(ctx, id, ptr);

    /* if the field is our alias, change login_alias */
    if ((id == ctx->login_id) && (ptr->field == atoi(ALIAS_FIELD)))
        strcpy(ctx->login_alias, ptr->value);

    return True;
}


/* change the field(s) or change the field attributes given for a specific ID */
int modify_fields(context *ctx, int id, Arg *list, int type)
{
    Arg *ptr;
    int count = 0;

    if (DEBUG)
        swrite(ctx, "modify id %d", id);

    /* skip to the good part of the arg list */
    for (ptr = list; ptr && (ptr->type != type); ptr = ptr->next);

    for (ptr = ptr->next; ptr; ptr = ptr->next)
        if (modify_field(ctx, id, ptr, type)) {
            count++;
            respond(ctx, RESP_MULT, "200:%s changed.", ptr->name);
        }
    return count;
}


/* delete entry; almost the same as query_cmd */
int delete_cmd(char *cmd, context *ctx)
{
    int status, ind, hits, size = 0, count = 0;
    Arg *list, *listp, *nick;
    Result *results = NULL;

    list = parse_cmd(cmd, ctx);       /* build arg list from cmd */

    if (validate_fields(ctx, list) == 0)
        respond(ctx, RESP_ONCE, "515:No indexed field in delete.");
    else {
        for (listp = list; listp && NotRet(listp); listp = listp->next) {
            size = find(ctx, listp, &results, size);    /* record ids that match query */
            if (listp->field == atoi(NAME_FIELD)) {
                nick = make_arg(ctx, listp->name, atoi(NICKNAME_FIELD),
                                listp->value, listp->type, listp->element);
                size = find(ctx, nick, &results, size);
                free(nick);
            }
        }
    }
    if (size == 0)  /* no index matches */
        respond(ctx, RESP_ONCE, "501:No matches to query.");
    else {
        hits = resolve(ctx, results, size, list);
        if (hits == 0)
            respond(ctx, RESP_ONCE, "501:No matches to query.");
        else if (hits > MAX_DELETES) {
            respond(ctx, RESP_ONCE, "518:Too many entries selected by delete command.");
            qilog(ctx, False, "Too many selected (%d)", hits);
        }
        else {
            for (ind = 0; ind < hits; ind++)
                if (delete_entry(ctx, results[ind].id)) count++;
            respond(ctx, RESP_MULT, "200:%d entries deleted", count);
        }
    }

    free_args(list);
    free(results);
    return True;
}


/* change command; almost the same as query_cmd */
int change_cmd(char *cmd, context *ctx)
{
    int status, ind, hits, size = 0, count = 0;
    Arg *list, *listp, *nick;
    Result *results = NULL;

    list = parse_cmd(cmd, ctx);       /* build arg list from cmd */
    if (validate_fields(ctx, list) == 0)
        respond(ctx, RESP_ONCE, "515:No indexed field in query.");
    else {
        for (listp = list; listp && NotRet(listp); listp = listp->next) {
            size = find(ctx, listp, &results, size);  /* record ids that match query */
            if (listp->field == atoi(NAME_FIELD)) {
                nick = make_arg(ctx, listp->name, atoi(NICKNAME_FIELD),
                                listp->value, listp->type, listp->element);
                size = find(ctx, nick, &results, size);
                free(nick);
            }
        }
    }
    if (size == 0)  /* no index matches */
        respond(ctx, RESP_ONCE, "501:No matches to query.");
    else if ((listp == (Arg *) 0) ||
             (listp->type & (TYPE_MAKE | TYPE_SET) == 0))
       respond(ctx, RESP_ONCE, "525:Make or Set clause not found");
    else {
        hits = resolve(ctx, results, size, list);
        if (hits == 0)
            respond(ctx, RESP_ONCE, "501:No matches to query.");
        else if (hits > MAX_CHANGES) {
            respond(ctx, RESP_ONCE, "518:Too many entries selected by change command.");
            qilog(ctx, False, "Too many selected (%d)", hits);
        }
        else {
            for (ind = 0; ind < hits; ind++)
                count += modify_fields(ctx, results[ind].id, list, listp->type);
            respond(ctx, RESP_MULT, "200:%d entries/%d fields modified", 
                    hits, count);
        }
    }

    free_args(list);
    free(results);
    return True;
}


/* select alias and initialize next_id */
int select_cmd(char *cmd, context *ctx)
{
    int id;
    char *cp, field_str[10];
    Arg *list, arg;

    list = parse_cmd(cmd, ctx);       /* build query struct from cmd */
    if ((list == NULL) || (list->next) || ((list->type & TYPE_VALUE) == 0)) {
        free_args(list);
        respond(ctx, RESP_ONCE, "599:Syntax error");
        return True;
    }
    if (list->field == -1) {       /* "select nnnn" means select arbitrary ID */
        if (!isdigit(*list->value)) {
            free_args(list);
            respond(ctx, RESP_ONCE, "599:Syntax error");
            return True;
        }
        ctx->next_id = atoi(list->value);
        free_args(list);
        respond(ctx, RESP_MULT, "200:ID %d selected", ctx->next_id);
        return True;
    }
    /* here if "select field=value"  field must be unique (like alias) */
    if ((ctx->fields[list->field].attrib & ATTR_UNIQUE) == 0) {
        free_args(list);
        respond(ctx, RESP_ONCE, "500:Field must be configured as unique");
        return True;
    }
    if (ctx->next_id > 0) {               /* next_id already set up? */
        respond(ctx, RESP_MULT, "200:ID %d already selected", ctx->next_id);
        return True;
    }
    sprintf(field_str, "%02d", list->field);
    if ((cp = get_value(ctx, list->value, field_str, ID_FIELD, 0)) == NULL) {
        free_args(list);
        respond(ctx, RESP_ONCE, "500:%s does not exist", list->name);
        return True;
    }
    id = atoi(cp);
    if ((cp = get_field(ctx, id, CONTROL_FIELD, 0, True)) == NULL) {
        free_args(list);
        respond(ctx, RESP_ONCE, "500:Control field does not exist");
        return True;
    }
    ctx->next_id = atoi(cp);
    arg.field = atoi(CONTROL_FIELD);
    arg.name = new_string(ctx, ctx->fields[arg.field].name);
    arg.type = TYPE_EQUAL | TYPE_VALUE;
    arg.value = (char *) check(ctx, malloc(20));
    sprintf(arg.value, "%d", ctx->next_id + 1);
    modify_field(ctx, id, &arg, TYPE_MAKE);  /* update the control field */
    free_args(list);
    respond(ctx, RESP_MULT, "200:ID %d selected", ctx->next_id);
    return True;
}


/* create an entry */
int add_cmd(char *cmd, context *ctx)
{
    char *cp, sel_cmd[50];
    Arg *list, *ptr;

    if (ctx->next_id == -1) {             /* is there an ID ready to use? */
#ifdef DEFAULT_SELECT
        if (ctx->fields[atoi(ALIAS_FIELD)].name) {
            sprintf(sel_cmd, "select %s=%s", ctx->fields[atoi(ALIAS_FIELD)].name, DEFAULT_SELECT);
            select_cmd(sel_cmd, ctx);     /* select the default entry */
        }
#else
        respond(ctx, RESP_ONCE, "477:No entry selected");
#endif
        if (ctx->next_id == -1)            /* it didn't exist? */
            return True;
    }

    list = parse_cmd(cmd, ctx);       /* build add struct from cmd */
    for (ptr = list; ptr; ptr = ptr->next)
        if (modify_field(ctx, ctx->next_id, ptr, TYPE_MAKE))
            respond(ctx, RESP_MULT, "200:%s added.", ptr->name);
    respond(ctx, RESP_MULT, "200:Ok.");

    ctx->next_id = -1;
    free_args(list);
    return True;
}


void db_open(context *ctx)
{
    ctx->idxfab = cc$rms_fab;
    ctx->idxfab.fab$b_fac = FAB$M_GET | FAB$M_PUT | FAB$M_DEL;
    ctx->idxfab.fab$l_fna = INDEX_NAME;
    ctx->idxfab.fab$b_fns = strlen(INDEX_NAME);
    ctx->idxfab.fab$b_shr = FAB$M_SHRGET | FAB$M_SHRPUT | FAB$M_SHRUPD | FAB$M_SHRDEL;
#if EXEC_LOGICALS
    ctx->idxfab.fab$v_lnm_mode = PSL$C_EXEC;
#endif
    ctx->idxfab.fab$l_xab = (void *) &ctx->idxxab;

    ctx->idxrab = cc$rms_rab;
    ctx->idxrab.rab$l_fab = &ctx->idxfab;
    ctx->idxrab.rab$l_kbf = ctx->idx_key;
    ctx->idxrab.rab$b_ksz = IDX_KEY_SIZE;
    ctx->idxrab.rab$b_rac = RAB$C_KEY;
    ctx->idxrab.rab$w_usz = IDX_RECORD_SIZE;
    ctx->idxrab.rab$l_ubf = ctx->idx_record;

    ctx->idxxab = cc$rms_xabkey;
    ctx->idxxab.xab$l_nxt = (void *) &ctx->idxxab2;
    ctx->idxxab2 = cc$rms_xabkey;
    ctx->idxxab2.xab$b_ref = SECONDARY_KEY;

    ctx->datfab = cc$rms_fab;
    ctx->datfab.fab$b_fac = FAB$M_GET | FAB$M_PUT | FAB$M_UPD | FAB$M_DEL;
    ctx->datfab.fab$l_fna = DATA_NAME;
    ctx->datfab.fab$b_fns = strlen(DATA_NAME);
    ctx->datfab.fab$b_shr = FAB$M_SHRGET | FAB$M_SHRPUT | FAB$M_SHRUPD | FAB$M_SHRDEL;
#if EXEC_LOGICALS
    ctx->datfab.fab$v_lnm_mode = PSL$C_EXEC;
#endif

    ctx->datrab = cc$rms_rab;
    ctx->datrab.rab$l_fab = &ctx->datfab;
    ctx->datrab.rab$l_kbf = ctx->dat_key;
    ctx->datrab.rab$b_ksz = DAT_KEY_SIZE;
    ctx->datrab.rab$b_rac = RAB$C_KEY;
    ctx->datrab.rab$b_krf = PRIMARY_KEY;
    ctx->datrab.rab$w_usz = DAT_RECORD_SIZE;
    ctx->datrab.rab$l_ubf = ctx->dat_record;

    if (((ctx->db_status = sys$open(&ctx->idxfab)) & 7) != SS$_NORMAL)
        return;

    /* get field sizes from index info */
    ctx->id_size = ctx->idxxab2.xab$b_siz0;
    ctx->field_size = ctx->idxxab2.xab$b_siz1;
    ctx->keyword_size = ctx->idxxab.xab$b_siz0 - ctx->id_size - ctx->field_size;
    if (DEBUG)
        if ((ctx->keyword_size != KEYWORD_SIZE) ||
            (ctx->field_size != FIELD_SIZE) || (ctx->id_size != ID_SIZE))
            swrite(ctx, "Field sizes: Keyword = %d/%d, ID = %d/%d, Field = %d/%d",
                   KEYWORD_SIZE, ctx->keyword_size,
                   ID_SIZE, ctx->id_size, FIELD_SIZE, ctx->field_size);

    if (((ctx->db_status = sys$connect(&ctx->idxrab)) & 7) != SS$_NORMAL)
        return;

    if (ctx->idxfab.fab$b_org != FAB$C_IDX) {
        ctx->db_status = 0;
        return;
    }

    if (((ctx->db_status = sys$open(&ctx->datfab)) & 7) != SS$_NORMAL)
        return;

    if (((ctx->db_status = sys$connect(&ctx->datrab)) & 7) != SS$_NORMAL)
        return;

    if (ctx->datfab.fab$b_org != FAB$C_IDX) {
        ctx->db_status = 0;
        return;
    }
}


void db_close(context *ctx)
{
    sys$close(&ctx->idxfab);
    sys$close(&ctx->datfab);
}


/* wrapper for sys$get to handle locked records */
int get_rec(struct RAB *rab)
{
    int status;
    float delay = 0.1;  /* 100 ms delay between tries */

    while ((status = sys$get(rab)) == RMS$_RLK)
        lib$wait(&delay);
    return status;
}

