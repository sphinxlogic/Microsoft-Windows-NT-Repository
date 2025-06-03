/*
 * This module manages the internal database of suffix information.
 * Note that the define routine is not thread-safe.
 *
 * The suffix string is stored upcase and lookups are case insensitive.
 *
 *   int http_define_suffix ( char *suffix, char *rep, char *encoding,
 *		char *quality );
 *
 *   int http_get_suffix_info ( char *suffix, int ndx,
 *	char **rep, char *encoding, float *quality );
 *
 *   int http_match_suffix ( char *suffix, char *client_accept,
 *	    char **rep, char **encoding );
 *
 * Revised: 12-OCT-1994		Convert encodings to lower case.
 * Revised: 17-DEC-1994		Ignore arguments on accepts list (;-delimited).
 * Revised: 28-FEB-1995		Remove ctype reference.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tutil.h"
/*
 * Define module-wide structures for the database.
 */
struct sinfo_blk {
    struct sinfo_blk *next_hash;
    char *suffix;			/* suffix string, including period */
    char *rep;				/* MIME content-type representation */
    char *encoding;			/* Data transfer type */
    float quality;
};
typedef struct sinfo_blk *sinfo_ptr;

static int hash_table_size = 0;
static int hash_mask, hash_table_count;
#define INITIAL_HASH_TABLE_SIZE 128
#define NULL_PTR (sinfo_ptr) 0

static sinfo_ptr *hash_table;		/* Pointer to sinfo_blk pointer (array)*/

/*****************************************************************************/
/*
 * Define hash function for determining initial hash table entry.
 * We assume string has been upcased and that hash table is valid.
 */
static int hash_name ( char *name )
{
    int hash, i;
    char *ptr;

    for ( hash = 0, ptr = name; *ptr; ptr++ ) {
	i = (unsigned) *ptr;
	hash = hash_mask & (i*577 ^ hash*3);
    }
    return hash;
}
/*********************************************************************/
/* Insert prepared sinfo block into hash table.
 * Insert new blocks at end of hash chain so that gets return them
 * in the same order added.
 */
static int insert_sinfo ( sinfo_ptr blk )
{
    int ndx;

    ndx = hash_name ( blk->suffix );
    /* printf("/suffix_map/ suffix '%s' hashes to %d (entry=%d)\n",
	blk->suffix, ndx, hash_table[ndx] ); */
    blk->next_hash = NULL_PTR;
    if ( hash_table[ndx] == NULL_PTR ) {
	/*
	 * First block in chain.
	 */
        hash_table[ndx] = blk;
     } else {
	/*
	 * Advance blk to last one.
	 */
	sinfo_ptr cur;
	for ( cur = hash_table[ndx]; cur->next_hash != NULL_PTR;
		cur = cur->next_hash );
	cur->next_hash = blk;
     }
    hash_table_count++;
    return hash_table_count;
}
/****************************************************************************/
/* Initialize or grow hash table. */
static int expand_hash_table()
{
    sinfo_ptr *old_table, cur, nxt;
    int i;

    hash_table_count = 0;
    if ( hash_table_size == 0 ) {
	/*
	 * Make initial allocation for table.
	 */
	hash_table_size = INITIAL_HASH_TABLE_SIZE;
	hash_table = (sinfo_ptr *) malloc 
			( hash_table_size * sizeof(sinfo_ptr) );
	hash_mask = hash_table_size - 1;
	for ( i = 0; i < hash_table_size; i++ ) hash_table[i] = NULL_PTR;
    } else {
	int old_size;
	old_size = hash_table_size;
	old_table = hash_table;
        /*
	 * Allocate and zero new table.
	 */
	hash_table_size = hash_table_size * 2;
	hash_table = (sinfo_ptr *) malloc 
		( hash_table_size  * sizeof(sinfo_ptr) );
	for ( i = 0; i < hash_table_size; i++ ) hash_table[i] = NULL_PTR;
	hash_mask = hash_table_size - 1;
	/*
	 * Scan all entries in old table.
	 */
	for (i=0; i < old_size; i++) if ( old_table[i] != NULL_PTR ) {
	    /*
	     * Scan chain of blocks at this hash value and re-insert.
	     */
	    for ( cur = old_table[i]; cur != NULL_PTR; cur = nxt ) {
		nxt = cur->next_hash;
		insert_sinfo ( cur );
	    }
	}
	free ( old_table );
    }
    return hash_table_count;
}
/***************************************************************************/
/*
 * Define new entry for suffix map.  Return value is 1 for success, 0 for
 * error.
 */
int http_define_suffix ( char *suffix, char *rep, char *encoding,
	char *quality )
{
    sinfo_ptr blk;
    int i;
    /*
     * Initialize if first call.
     */
    if ( hash_table_size == 0 ) expand_hash_table();
    else if ( hash_table_count > hash_table_size ) expand_hash_table();
    /*
     * Allocate a block.
     */
    blk = (sinfo_ptr) malloc ( sizeof(struct sinfo_blk) );
    if ( !blk ) return 0;
    /*
     * Allocate strings for the field values and copy.
     */
    blk->suffix = malloc ( strlen(suffix) + 1 );
    if ( !blk->suffix ) return 0;
    else {
	tu_strupcase ( blk->suffix, suffix );
    }

    blk->rep = malloc ( strlen(rep) + 1 );
    if ( !blk->rep ) return 0;
    else strcpy ( blk->rep, rep );

    blk->encoding = malloc ( strlen(encoding) + 1 );
    if ( !blk->encoding ) return 0;
    else {
	tu_strlowcase ( blk->encoding, encoding );
    }

    if ( *quality ) blk->quality = atof ( quality );
    else blk->quality = 1.0;
    /*
     * Add to hash table.
     */
    insert_sinfo ( blk );
    return 1;
}
/***************************************************************************/
/*
 * Lookup suffix info.  Since multiple occurrences can occur, specify
 * secondary values with the ndx parameter.
 */
static int http_get_suffix_info ( char *suffix, int ndx,
	char **rep, char **encoding, float *quality )
{
    sinfo_ptr cur, hit;
    char key[96];
    if ( hash_table_size == 0 ) return 0;    /* no entries */
    /*
     * Make upcase version of suffix and make initial key.
     */

    tu_strnzcpy ( key, suffix, sizeof(key)-1 );
    tu_strupcase ( key, key );
    hit = NULL_PTR;
    /*
     * Search hash table chain for ndx'th occurrence of match.
     */
    for ( cur = hash_table[hash_name(key)];
	(cur != NULL_PTR) && (ndx >= 0); cur = cur->next_hash ) {
	if ( tu_strncmp ( key, cur->suffix, sizeof(key) ) == 0 ) {
	    --ndx;
	    hit = cur;
	}
    }

    if ( ndx >= 0 ) return 0;
    else {
	*rep = hit->rep;
	*encoding = hit->encoding;
	*quality = hit->quality;
	return hit->next_hash ? 1 : 3;
    }
}
/*****************************************************************************/
/* Http_match_suffix finds best match between representions available to
 * suffix and capabilities of client.  We return pointers to the representation
 * encoding strings to send in the response header.  
 *
 * Return value will be 1 if match found, 0 if no match and 2 if match
 * was against a wildcard string in the client request.
 */
int http_match_suffix ( 
	char *suffix, 			/* Suffix (e.g. .gif) in URL */
	char *client_accept,		/* Comma-separated list of fields from
					 * accept: header lines. */
	char **rep, 			/* Representation string */
	char **encoding )
{
    int i, status, j, match_fail;
    float qual, high_quality, wildcard_quality;
    char *p, *r, *e, *wild_r, *wild_e, c;
    /*
     * Set default rep and encoding to static values.
     */
    *rep = "text/plain";
    *encoding = "8BIT";
    wildcard_quality = high_quality = -1.0;
    /*
     * Loop through representations available to suffix.
     */
    for ( i=0; (status = http_get_suffix_info(suffix, i, &r, &e, &qual)); i++) {
	if ( qual <= high_quality ) continue;
	/*
	 * Scan the accept list
	 */
	for ( j  = 0, p = client_accept; *p; p++ ) {
	    c = *p;
	    if ( (c == ',') || (c == ';') ) {
		/*
		 * End of field, see if still in match state.
		 */
		if ( r[j] == 0 ) {  /* match */
		    high_quality = qual;
		    *rep = r;
		    *encoding = e;
		    break;
		}
		j = 0;
		if ( c == ';' ) {
		    /* skip to next comma */
		    do p++; while ( (*p != ',') && *p );
		}
	    } else if ( c == '*' ) {
		/*
		 * Save info on highest wildcard match.
		 */
		if ( qual > wildcard_quality ) {
		    wild_e = e;
		    wild_r = r;
		    wildcard_quality = qual;
		}
		/* Skip to next field in list */
		j = 0;
		do p++; while ( (*p != ',') && *p );

	    } else if ( c != r[j] ) {
		/*
		 * Match failed, skip to next comma or EOF.
		 */
		do p++; while ( (*p != ',') && *p );
		j = 0;

	    } else j++;		/* prepare to test next character */
	}
	if ( status == 3 ) { i++; break; }	/* no more suffix defs */
    }
    /*
     * Check results of scan.
     */
    if ( i == 0 ) return 1;	/* representation unknown, take defaults */

    if ( high_quality < 0 ) {
	/*
	 * We couldn't find the desired rep. in the clients accepted
	 * representation list.  See if it matched a wildcard.
	 */
	if ( r[j] == 0 ) {	/* Last field matched */
	    *rep = r;
	    *encoding = e;
	    return 1;
	} else if ( wildcard_quality > 0 ) {
	    /*
	     * Fallback to rep with highest wildcard match.
	     */
	    *rep = wild_r;
	    *encoding = wild_e;
	    return 2;
	}
	return 0;   /* No matches to suffix */
    }
    return 1;
}
