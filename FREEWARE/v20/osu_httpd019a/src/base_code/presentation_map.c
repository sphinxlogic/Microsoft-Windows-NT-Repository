/*
 * This module manages the internal database of presentation information from
 * the rules file.
 * Note the the define routine is not thread-safe.
 *
 * The representation string is stored upcase and lookups are case insensitive.
 *
 *   int http_define_presentation ( char *rep, char *presentation );
 *
 *   int http_get_presentation ( char *rep, char **presentation );
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "tutil.h"
/*
 * Define module-wide structures for the database.
 */
struct pinfo_blk {
    struct pinfo_blk *next_hash;
    char *rep;				/* MIME content-type representation */
    char *presentation;			/* Command string for CONVERT */
};
typedef struct pinfo_blk *pinfo_ptr;

static int hash_table_size = 0;
static int hash_mask, hash_table_count;
#define INITIAL_HASH_TABLE_SIZE 128
#define NULL_PTR (pinfo_ptr) 0

static pinfo_ptr *hash_table;		/* Pointer to pinfo_blk pointer (array)*/

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
/* Insert prepared pinfo block into hash table.
 * Insert new blocks at end of hash chain so that gets return them
 * in the same order added.
 */
static int insert_pinfo ( pinfo_ptr blk )
{
    int ndx;

    ndx = hash_name ( blk->rep );
#ifdef DEBUG
    printf("/presentation_map/ rep '%s' hashes to %d (entry=%d)\n",
	blk->rep, ndx, hash_table[ndx] );
#endif
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
	pinfo_ptr cur;
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
    pinfo_ptr *old_table, cur, nxt;
    int i;

    hash_table_count = 0;
    if ( hash_table_size == 0 ) {
	/*
	 * Make initial allocation for table.
	 */
	hash_table_size = INITIAL_HASH_TABLE_SIZE;
	hash_table = (pinfo_ptr *) malloc 
			( hash_table_size * sizeof(pinfo_ptr) );
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
	hash_table = (pinfo_ptr *) malloc 
		( hash_table_size  * sizeof(pinfo_ptr) );
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
		insert_pinfo ( cur );
	    }
	}
	free ( old_table );
    }
    return hash_table_count;
}
/***************************************************************************/
/*
 * Lookup presentation command for a representation. 
 */
int http_get_presentation ( char *rep, char **presentation )
{
    pinfo_ptr cur;
    char key[96];

    if ( hash_table_size == 0 ) return 0;    /* no entries */
    /*
     * Make upcase version of representation and make initial key.
     */
    tu_strnzcpy ( key, rep, sizeof(key)-1 );
    tu_strupcase ( key, key );
    /*
     * Search hash table chain for ndx'th occurrence of match.
     */
    for ( cur = hash_table[hash_name(key)];
	(cur != NULL_PTR); cur = cur->next_hash ) {
	if ( tu_strncmp ( key, cur->rep, sizeof(key) ) == 0 ) {
	    *presentation = cur->presentation;
	    return 1;
	}
    }
    *presentation = "";
    return 0;
}
/***************************************************************************/
/*
 * Define new entry for presentation map.  Return value is 1 for success, 0 for
 * error.  Duplicates re not allowed.
 */
int http_define_presentation ( char *rep, char *presentation )
{
    pinfo_ptr blk;
    int i, status;
    char *existing;
    /*
     * Initialize if first call.
     */
    if ( hash_table_size == 0 ) expand_hash_table();
    else if ( hash_table_count > hash_table_size ) expand_hash_table();
    /*
     * Attempt to find existing definition and return error if found.
     */
    if ( http_get_presentation ( rep, &existing ) == 1 ) return 0;
    /*
     * Allocate a block.
     */
    blk = (pinfo_ptr) malloc ( sizeof(struct pinfo_blk) );
    if ( !blk ) return 0;
    /*
     * Allocate strings for the field values and copy key (upcased).
     */
    blk->rep = malloc ( strlen(rep) + 1 );
    if ( !blk->rep ) return 0;
    else {
	for ( i = 0; rep[i] != '\0'; i++ ) 
		blk->rep[i] = _toupper(rep[i]);
	blk->rep[i] = '\0';
    }

    blk->presentation = malloc ( strlen(presentation) + 1 );
    if ( !blk->presentation ) return 0;
    else strcpy ( blk->presentation, presentation );
    /*
     * Add to hash table.
     */
    insert_pinfo ( blk );
    return 1;
}
