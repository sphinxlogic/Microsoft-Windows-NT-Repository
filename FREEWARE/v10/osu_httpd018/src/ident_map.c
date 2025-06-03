/*
 * This module manages the internal database of pathname (ident) translation
 * and screening information.
 *
 * Note the the define routine is not thread-safe.
 *
 * The suffix string is stored upcase and lookups are case insensitive.
 *
 *   int http_define_ident ( char *ident, int code, char *value );
 *	code:	1 - Add map entry.
 *		2 - Add pass (if match) entry.
 *		3 - Add fail (if match) entry.
 *		4 - Add redirect (if match) entry.
 *		5 - Add exec (htbin) entry.
 *		6 - Add usermap entry (userdir, loable mapping routines)
 *		7 - Add protect entry
 *
 *   int http_translate_ident 
 *	   ( char *ident, char *translation, int maxlen, access_info acc );
 *	ret val: 0	Translation failed (matched fail record).
 *		1	Translation successful, ident matched pass entry.
 *		2	Translation successful, return redirect.
 *		3	Translation successful, matched exec entry.
 *		4	Translation successful, matched userdir entry.
 *
 * Revision history:
 *    25-FEB-1994	Added piggyyback mapping to pass command.
 *     6-APR-1994	Added redirect and generic exec support.
 *     4-MAY-1994	Added support for userdir command.
 *    26-MAY-1994	Added protect option.
 *    30-JUN-1994	Unescape the URL.
 *    25-SEP-1994	Preserve case in URL as long as translation is
 *			less than 300 bytes.
 *    16-NOV-1994	Make failure on userdir look count as no-match and
 *			continue with next rule rather than failing.
 *    26-FEB-1995	Incorporate George Carrette's Dynamic mapping idea.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tutil.h"
#include "access.h"
int http_log_level, tlog_putlog();	/* Global variable, logging level */
/*
 * Define module-wide structures for the database.
 */
#include "ident_map.h"
static int rule_count = 0;
static info_ptr rule_list, last_rule;
map_callout http_translate_userdir;
int http_load_dynamic_rule(info_ptr blk, char *rtn, char *image, char *fspec);
/*********************************************************************/
/*
 * Define new entry for ident map.  Return value is 1 for success, 0 for
 * error.
 */
int http_define_ident ( char *ident, int code, char *value )
{
    info_ptr new;
    int val_len, i, j, status;
    static char *rulename[] = { "(invalid):", "(map):    ", "(pass):   ", 
	"(fail):   ", "(redirct):", "(exec):   ", "(usermap):", "(protect):" };
    /*
     * Allocate block and its string storage.  Pattern is upcased for
     * case insensitivity.
     */
    new = (info_ptr) malloc ( sizeof(struct info_blk) );
    new->next = (info_ptr) NULL;
    new->code = code;
    new->pat_len = strlen(ident);
    val_len = strlen ( value );
    new->val_len = val_len;
    new->pattern = malloc ( new->pat_len + 1 );
    tu_strupcase ( new->pattern, ident );
    new->value = malloc ( val_len + 1 );
    strcpy ( new->value, value );
    new->name = rulename[((code > 0) && (code < 8)) ? code : 0];
    /*
     * See if pattern has wildcard character.
     */
    if ( (new->pat_len > 0) && (new->pattern[new->pat_len-1] == '*') ) {
	new->wildcard = 1;
	--new->pat_len;			/* don't include '*' */
	/*
	 * See if value ends in '*', setting
	 */
	if ( val_len > 0 ) if ( new->value[val_len-1] == '*' ) {
	   /* Trim the last character and change wildcard status. */
	    --new->val_len;
	    new->value[val_len-1] = '\0';
	    new->wildcard = 2;
	}
    }
    else new->wildcard = 0;
    /*
     * Setup callout for code type 6.  Value for userdir is just a
     * directory name.  Value for usermap is prefixed with 
     * "(routine,image,file)"
     */
    if ( code == IDENT_MAP_RULE_USERMAP ) {
	if ( *value != '(' ) {
	    /* Old userdir rule */
	    new->usermap = http_translate_userdir;
	    new->name = "(userdir):";
	} else {
	    /*
	     * Parse image/routine information out of value.
	     * (routine,image,fspec).
	     */
	    char *routine, *image, *fspec;
	    routine = &new->value[1];
	    image = fspec = (char *) 0;
	    for ( i = 0; (routine[i] != ')'); i++ ) {
		if ( routine[i] == ',' ) {
		    routine[i] = '\0';
		    if ( !image ) image = &routine[i+1];
		    else if ( !fspec ) fspec = &routine[i+1];
		    else {
			tlog_putlog(0,"Extra argument in usermap ignored: !AZ!/",
			    &routine[i+1] );
			break;
		    }
		} else if ( !routine[i] ) {
		    tlog_putlog(0,"Internal error in ident_map!/" );
		    i = 0;
		    break;
		}
	    }
	    routine[i++] = '\0';
	    status = http_load_dynamic_rule ( new, routine, image, fspec );
	    /*
	     * Trim arguments from value.  We are garanteed string fits.
	     */
	    for ( j = 0; new->value[i]; j++ ); new->value[j] = new->value[i++];
	    new->val_len = tu_strlen ( new->value );
	}
    }
    /*
     * Insert at end of list.
     */
    if ( rule_count == 0 ) rule_list = new;
    else last_rule->next = new;
    last_rule = new;
    rule_count++;
    return 1;
}
/***************************************************************************/
/* Process ident against rules files:  Turn
 *
 * Return values:
 *	0	Translation failed (matched fail record).
 *	1	Translation successful, ident matched pass entry.
 *	2	Translation successful, return redirect.
 *	3	Translation successful, matched exec entry.
 *	4	Translation successful, matched userdir entry.
 */
int http_translate_ident ( char *ident, char *translation, int maxlen, 
	access_info acc )
{
    info_ptr blk;
    int i, ident_len, length, status;
    char *defprot, *cmpbuf, *cp_buf, scratch_cmp[300];
    /*
     * Make initial translation the unescaped URL.  Cmpbuf holds upcased form 
     * of translation and will either point local buffer or user's buffer if 
     * translation grows too large.
     */
    tu_unescape  ( translation, ident, maxlen - 1 );
    ident_len = tu_strlen ( ident ) + 1;
    cmpbuf = (ident_len < sizeof(scratch_cmp)) ? scratch_cmp : translation;
    tu_strupcase ( cmpbuf, translation );
    acc->uic = 0;
    acc->prot_file = "";		/* No protection file */
    acc->cache_allowed = 1;		/* Caching enabled */
    defprot = "";			/* default protection file */
    /*
     * Process translation against rules.
     */
    for ( blk = rule_list; blk; blk = blk->next ) {
	int cmp;
	/*
	 * Either compare entire string or only up to wildcard position.
	 */
	cmp = tu_strncmp ( cmpbuf, blk->pattern, 
		blk->wildcard ? blk->pat_len : ident_len );
	if ( http_log_level > 10 ) tlog_putlog ( 9,
		"Rule test !AZ !AZ '!AZ' vs. '!AZ'!/",
		cmp ? "failure" : "SUCCESS", blk->name,	blk->pattern, 
		translation );

	if ( cmp == 0 ) {
	    /*
	     * We matched pattern, Take action based upon code.
	     */
	    switch ( blk->code ) {
		case IDENT_MAP_RULE_PASS:  /* Pass on ident match, possibly */
					   /* mapping result. */
		    if ( blk->val_len <= 0 ) return 1;

		case IDENT_MAP_RULE_MAP:		/* map */
		case IDENT_MAP_RULE_REDIRECT:		/* redirect */
		    /* Replace either all of translation or part with
		     *	new value
		     */
		    if ( blk->wildcard > 1 ) {
			/* 
			 * replace matching portion only.
			 */
			int replace_len, delta;
			delta = blk->val_len - blk->pat_len;
			if ( delta > 0 ) {
			    /* New value longer than current */
			    if ( ident_len + 1 + delta > maxlen ) return 0;
			    for ( i = ident_len; i >= blk->pat_len; --i )
				translation[i+delta] = translation[i];
			} else {
			    /*
			     * Easy case, new is smaller than old.
			     */
			    for ( i = blk->pat_len; i <= ident_len; i++ )
				translation[i+delta] = translation[i];
			
			}
			ident_len = ident_len + delta;
		        tu_strncpy ( translation, blk->value, blk->val_len );
		    } else {
			/* Replace entire thing. */
			tu_strnzcpy ( translation, blk->value, maxlen - 1 );
		        ident_len = tu_strlen ( translation ) + 1;
		    }
		    if ( http_log_level > 1 ) tlog_putlog ( 2,
			"ident_map: '!AZ' -> '!AZ'!/", ident, translation );
		    /*
		     * Either return success or try next rule.
		     * Don't upcase redirect values.
		     */
		    if ( blk->code == 4 ) return 2;	/* redirect */
		    if (ident_len >= sizeof(scratch_cmp)) cmpbuf = translation;
		    tu_strupcase ( cmpbuf, translation );
		    if ( blk->code == 2 ) return 1;	/* pass */
		    break;
		case IDENT_MAP_RULE_FAIL:
		    return IDENT_MAP_FAILED;	/* fail on ident match */

		case IDENT_MAP_RULE_EXEC:		/* exec */
		    /*
		     * Exec directive, append * pattern * value to translation.
		     */
		    i = tu_strlen ( translation );
		    translation[i++] = '*';		/* first delimiter */
		    tu_strnzcpy ( &translation[i], translation, /* blk->pattern, */
				blk->pat_len );
		    i += blk->pat_len;
		    translation[i++] = '*';
		    length = maxlen - i - 1;
		    if ( length > blk->val_len ) length = blk->val_len;
		    tu_strnzcpy ( &translation[i], blk->value,  length );
		    return 3;
		case IDENT_MAP_RULE_USERMAP:
		    /*
		     * userdir directive, replace ~username/ with login
		     * directory + translation + /
		     */
		    cp_buf = cmpbuf;
		    status = (*blk->usermap) (blk, translation, 
			maxlen, acc, &cp_buf, sizeof(scratch_cmp) );
		    cmpbuf = cp_buf;
		    if ( http_log_level > 1 ) tlog_putlog ( 2,
			"ident_map!AZ'!AZ' -> '!AZ'!/", blk->name, ident, 
				translation );
		    if ( status != IDENT_MAP_LOOP  ) return status;
		    break;
		case IDENT_MAP_RULE_PROTECT:
		    /*
		     * Access to the current ident is protected.  Return
		     * failure if protection check fails.
		     */
		    acc->prot_file = blk->value;
		    if ( *acc->prot_file == '\0' ) acc->prot_file = defprot;
		    break;
		case IDENT_MAP_RULE_DEFPROT:		/* defprot rule */
		    defprot = blk->value;
	    }
	}
    }
    return 0;		/* default to failure status */
}

/***************************************************************************/
/* Convert /~username/.... specification to /device/home-dir/... according
 * to information in the user authorization file.  The sysuaf file may need to
 * be modified to granted read access.
 *
 * Return values:
 *	-1	Translation failed (username not found)
 *	4	Translation successful.
 */
int http_translate_userdir ( info_ptr blk, char *translation, int maxlen,
	access_info acc, char **case_preserved, int cp_len )
{
    int i, user_len, login_len, delta, status, length;
    int http_user_information();
    char login[128];
    /*
     * Parse username out of translation (part after pattern).
     */
    for ( i = blk->pat_len; (i < maxlen) && translation[i] &&
	  (translation[i] != '/'); i++ );
    user_len = i - blk->pat_len;
    /*
     * Search remainder of translation buffer for parent directory references
     * (/../) and disallow.
     */
    for ( ; translation[i]; i++ ) if ( translation[i] == '/' ) {
	if ( 0 == strncmp ( &translation[i], "/../", 4 ) ) return IDENT_MAP_LOOP;
    }
    /*
     * Fetch information about user.  Block other activity and set privilege.
     */
    status = http_user_information ( &translation[blk->pat_len], user_len, 
		&acc->uic, login, sizeof(login) );
    if ( (status&1) == 0 ) return IDENT_MAP_LOOP;
    /*
     * Calculate length of login directory + userdir path and make gap
     * in translation string to accomodate it.
     */
    login_len = tu_strlen ( login );
    if ( blk->val_len == 0 ) {
	/* userdir is null, shorten login_len to exclude extra '/' */
	login_len--;
    }
    
    user_len += blk->pat_len;
    delta = login_len + blk->val_len - user_len;

    length = tu_strlen ( translation );
    if ( delta > 0 ) {
	/* New value longer than current */
	if ( length+delta > maxlen ) delta = 0;
	for ( i = length; i > 0; --i )
		translation[i+delta] = translation[i];
    } else {
	/* New value smaller than /~username. */
	for ( i = user_len; i <= length; i++ ) 
		translation[i+delta] = translation[i];
    }
    /*
     * Load users device, directory and subdirectory into gap.
     */
    tu_strncpy ( translation, login, login_len );
    tu_strncpy ( &translation[login_len], blk->value, blk->val_len );
    /*
     * Do additional fixup of user portion.
     */
    tu_strupcase ( &translation[login_len], &translation[login_len] );

    return IDENT_MAP_OK_USERMAP;
}
