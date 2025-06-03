
static char rcsid[] = "@(#)$Id: mk_aliases.c,v 5.6 1992/12/11 02:10:24 syd Exp $";

/*******************************************************************************
 *  The Elm Mail System  -  $Revision: 5.6 $   $State: Exp $
 *
 *			Copyright (c) 1988-1992 USENET Community Trust
 *			Copyright (c) 1986,1987 Dave Taylor
 *******************************************************************************
 * Bug reports, patches, comments, suggestions should be sent to:
 *
 *	Syd Weinstein, Elm Coordinator
 *	elm@DSI.COM			dsinc!elm
 *
 *******************************************************************************
 * $Log: mk_aliases.c,v $
 * Revision 5.6  1992/12/11  02:10:24  syd
 * Make Elm complain about spaces that are not after commas in
 * alias and address lists
 * From: "Robert L. Howard" <robert.howard@matd.gatech.edu>
 *
 * Revision 5.5  1992/12/11  02:09:06  syd
 * Fix where the user creates a first new alias, then deletes it, the
 * alias stays on screen, but the file really will be empty if it was the
 * last alias, so the retry to delete gives 'cannot open ...file' messages
 * From: "Robert L. Howard" <robert.howard@matd.gatech.edu>
 *
 * Revision 5.4  1992/11/15  01:15:28  syd
 * The alias message_count isn't set to zero if the last alias has
 * been deleted from the alias table. As no aliases are reread from
 * the aliases database the message_count is left as it was before.
 *
 * Fixed that the function do_newalias() sometimes returns without freeing
 * the buffer allocated before. The patch adds these free calls.
 *
 * When you erroneously type a number in your folder elm asks you for
 * a new current message number. But now if you erase this one number
 * and leave the string empty elm will set the new current message to
 * the second message on our sun4! The patch adds a check for an empty
 * string and returns the current number if no number was entered.
 * From: vogt@isa.de (Gerald Vogt)
 *
 * Revision 5.3  1992/11/07  16:32:14  syd
 * comments should be allowed anywhere in the alias file.
 * From: "Robert L. Howard" <robert.howard@matd.gatech.edu>
 *
 * Revision 5.2  1992/10/11  01:46:35  syd
 * change dbm name to dbz to avoid conflicts with partial call
 * ins from shared librarys, and from mixing code with yp code.
 * From: Syd via prompt from Jess Anderson
 *
 * Revision 5.1  1992/10/03  22:41:36  syd
 * Initial checkin as of 2.4 Release at PL0
 *
 *
 ******************************************************************************/

/** Install a new set of aliases for the 'Elm' mailer. 

	This code is shared with newalias and elm so that
  it is easier to do updates while in elm.  The main routine
  here is do_newalias().  If the third argument is TRUE then
  we were called from elm.  That means that we will need to 
  sleep() between error messages....  The fourth arguement
  controls whether or not we should warn about missing aliases.text
  files.  For the newalias program that's an error, for elm it's
  ok.  Warning -- this last is a *hack* to get elm2.4b out the
  door -- do_newalias should really return an error value and
  the caller should generate the message.

**/

#include "headers.h"
#include "s_newalias.h"
#include <ctype.h>
#include "ndbz.h"

#ifdef BSD
#  include <sys/file.h>
#  undef tolower
#  undef toupper
#endif

#define group(string)		(index(string,',') != NULL)

int  buff_loaded;		/* for file input overlap... */
int  err_flag;			/* if errors, don't save!    */
int  al_count;			/* how many aliases so far?  */
char msg_buff[SLEN];		/* buffer for error messages */
DBZ  *aliases_hash;             /* current alias file        */
char *buffer;			/* alias line buffer         */
long buffer_size;		/* size of alias buffer      */
long file_offset = 0;		/* offset into file so far   */

extern int  is_system;		/* system file updating?     */

#ifdef DEBUG
extern FILE *debugfile;
extern int  debug;
#endif

int
get_alias(file, fromelm)
FILE *file;
int fromelm;
{
	/* load buffer with the next complete alias from the file.
	   (this can include reading in multiple lines and appending
	   them all together!)  Returns EOF after last entry in file.
	
	Lines that start with '#' are assumed to be comments and are
 	ignored.  White space as the first field of a line is taken
	to indicate that this line is a continuation of the previous. */

	static char mybuffer[LONG_STRING];
	int    done = 0, len;
	char   *s;

	/** get the first line of the entry... **/

	buffer[0] = '\0';			/* zero out line */
	len = 0;

	if (get_line(file, mybuffer, TRUE, fromelm) == -1) 
	    return(-1);
	strcpy(buffer, mybuffer);
	len = strlen(buffer);

	/** now read in the rest (if there is any!) **/

	do {
	  if (get_line(file, mybuffer, FALSE, fromelm) == -1) {
	      if (err_flag)
	          return(-1);
	      buff_loaded = 0;	/* force a read next pass! */
	      return(0);	/* okay. let's just hand 'buffer' back! */
	  }
	  done = (! whitespace(mybuffer[0]));
	  if (! done) {
	      for (s = mybuffer; *s && whitespace(*s); s++) ;
	      *--s = ' ';
	      len += strlen(s);
	      if (len >= buffer_size) {
	          sprintf(msg_buff, catgets(elm_msg_cat,
	                  NewaliasSet, NewaliasLineToLong,
	              "Line + continuations exceeds maximum length of %ld:"),
	              buffer_size);
	          error(msg_buff);
	          if (fromelm)
	              sleep(2);
	          sprintf(msg_buff, "%.40s", buffer);
	          error(msg_buff);
		  err_flag++;
	      } else
		  strcat(buffer, s);
	  }
	} while (! done);
	
	return(0);	/* no sweat! */
}

int
get_line(file, buffer, first_line, fromelm)
FILE *file;
char *buffer;
int  first_line, fromelm;
{
/*
 *	Read line from file.
 *	If first_line and buff_loaded, then just return!
 *	All comment and blank lines are just ignored (never passed back).
 */
	int len;

	if (first_line && buff_loaded) {
	  buff_loaded = 1;
	  return(0);
	}

	buff_loaded = 1;	/* we're going to get SOMETHING in the buffer */

	do {
       /*
	* We will just ignore any line that begins with comment (no
	* matter how long).
	*/
	  do {
	    len = mail_gets(buffer, LONG_STRING, file);
	  } while (buffer[0] == '#');

	  if (len > 0) {
	    if (buffer[len - 1] != '\n') {
	      if (fromelm) {
		  error(catgets(elm_msg_cat, NewaliasSet, NewaliasSplitShort,
		  "Line too long, split using continuation line format:"));
		  sleep(2);
		  sprintf(msg_buff, "%.40s", buffer);
	      }
	      else {
		  sprintf(msg_buff, catgets(elm_msg_cat,
			  NewaliasSet, NewaliasSplit,
		  "Line too long, split using continuation line format (starting line\nwith whitespace):\n%.40s\n"), buffer);
	      }
	      error(msg_buff);
	      err_flag++;
	      return(-1);
	    }
	  }
	  else {
	    return(-1);
	  }
	  no_ret(buffer);
/*
 *	If the buffer is zero length after returns are stripped (and
 *	len was > 0) we need to go ahead and get another line.
 */
	} while (strlen(buffer) == 0);

	return(0);
}

void
de_escape(the_string)
char *the_string;
{
	register char *s, *out;

	for (s = the_string, out = the_string; *s; s++) {
	    if (*s != '\\')
		*out++ = *s;
	    else
		*out++ = *++s;
	}
	*out = '\0';

	return;
}

void
add_to_hash_table(word, offset)
char *word;
long  offset;
{
	datum	key, value, ovalue;
	long	off;
	
	key.dptr = word;
	key.dsize = strlen(word);
	off = offset;
	value.dptr = (char *) &off;
	value.dsize = sizeof(off);

	ovalue = dbz_fetch(aliases_hash, key);
	if (ovalue.dptr != NULL) {
	    sprintf(msg_buff, catgets(elm_msg_cat,
	            NewaliasSet, NewaliasDupAlias,
		    "** Duplicate alias '%s' in file.  Multiples ignored."),
		word);
	    error(msg_buff);
	    return;
	}

	if (dbz_store(aliases_hash, key, value) < 0) {
	    sprintf(msg_buff, catgets(elm_msg_cat,
	            NewaliasSet, NewaliasErrWrite,
		    "** Error writing alias '%s'."), word);
	    error(msg_buff);
	    err_flag++;
	}
}

void
add_to_table(data, aliases, lastn, firstn, comment, addresses)
FILE *data;
register char *aliases, *lastn, *firstn, *comment, *addresses;
{
	struct alias_rec	alias;
	register char	*s;
/*
 *	crack the information into an alias_rec structure, then add the entry
 *	each alias at a time to the dbz file.
 */
	alias.status = 0;
	alias.alias = 0;
/*
 *	loop over each alias in aliases (split at the ,)
 */
	while (aliases != NULL) {
	    if ((s = index(aliases, ',')) != NULL)
		*s++ = '\0';

	    alias.last_name = alias.alias + strlen(aliases) + 1;
	    alias.name = alias.last_name + strlen(lastn) + 1;
	    alias.comment = alias.name + strlen(lastn) + 1;
	    if (firstn)
		alias.comment += strlen(firstn) + 1;
	    if (comment)
	        alias.address = alias.comment + strlen(comment) + 1;
	    else
	        alias.address = alias.comment + 1;
	    alias.type = is_system ? SYSTEM : USER;
	    alias.type |= group(addresses) ? GROUP : PERSON;
	    alias.length = ((int) alias.address) + strlen(addresses) + 1;

/*
 *	write the entry to the data file, followed by its data
 */
	    fwrite((char *)&alias, sizeof(alias), 1, data);
	    fwrite(aliases, strlen(aliases) + 1, 1, data);
	    fwrite(lastn, strlen(lastn) + 1, 1, data);
	    if (firstn) {
		fwrite(firstn, strlen(firstn), 1, data);
		fwrite(" ", 1, 1, data);
	    }
	    fwrite(lastn, strlen(lastn) + 1, 1, data);
	    if (comment)
	        fwrite(comment, strlen(comment) + 1, 1, data);
	    else
	        fwrite("\0", 1, 1, data);
	    fwrite(addresses, strlen(addresses) + 1, 1, data);

	    add_to_hash_table(aliases, file_offset + sizeof(alias));
	    al_count++;

	    file_offset += alias.length + sizeof(alias);
	    aliases = s;
	    fflush(data);
	}

}	

void
put_alias(data)
FILE *data;
{
/*
 *	parse the buffer into aliases, names, comments and addresses
 *	and then add the alias
 */
	register char *s, *aliases, *lastn, *firstn, *comment, *addresses, *out;
	int	in_quote = FALSE;

/*
 *	extract the alias name, its the part up to the first =, less the
 *	white space on the end.
 */
	aliases = buffer;
	if ((s = index(buffer, '=')) == NULL) {
	    sprintf(msg_buff, catgets(elm_msg_cat,
	            NewaliasSet, NewaliasNoFieldSep,
	            "Error - alias \"%.40s\" missing '=' field seperator."),
	        aliases);
	    error(msg_buff);
	    err_flag++;
	    return;
	}

	lastn = s + 1;
	while (--s >= buffer && whitespace(*s) ) ;
	*++s = '\0';

/*
 *	Now strip out any whitespace (and make sure it was
 *	legal whitespace).  Legal whitespace follows ','.
 */
	for (s = aliases, out = aliases; *s; s++) {
	    if (whitespace(*s)) {
	        if (*(out-1) != ',') {
	            err_flag++;			/* Keep going for now */
		    *out++ = *s;
	        }
	    }
	    else {
		*out++ = *s;
	    }
	}
	*out = '\0';

/*
 *	We have to delay reporting the error until the (legal)
 *	spaces are striped, otherwise aliases is all screwed
 *	up and doesn't display well at all.
 */
	if (err_flag) {
	    sprintf(msg_buff, catgets(elm_msg_cat,
	        NewaliasSet, NewaliasAliasWSNotAllowed,
	        "Error - whitespace in alias '%.30s' is not allowed."),
	        aliases);
	    error(msg_buff);
	    return;
	}


	/* verify the alias name is valid */
	for ( s = buffer ; *s != '\0' && (ok_alias_char(*s)||*s==',') ; ++s ) ;
	if ( *s != '\0' ) {
	    MCsprintf(msg_buff, catgets(elm_msg_cat,
	            NewaliasSet, NewaliasCharNotSupported,
		    "Error - character '%c' in alias '%s' is not supported."),
	        *s, aliases);
	    error(msg_buff);
	    err_flag++;
	    return;
	}
	
/*
 *	get the second field into "lastn" - putting stuff after ','
 *	into "comment".  skip over white space after = and before last =
 */
	while (*lastn != '\0' && whitespace(*lastn) )
	    lastn++;

	for (s = lastn; *s; s++) {
	    if (*s == '\\') {
		s++;
		continue;
		}

	    if (*s == '"')
		in_quote = !in_quote;

	    if (in_quote)
		continue;

	    if (*s == '=') {
		addresses = s + 1;
		break;
	    }
	}

	if (*s != '=') {
	    sprintf(msg_buff, catgets(elm_msg_cat,
	            NewaliasSet, NewaliasNoFieldSep,
		    "Error - alias \"%.40s\" missing '=' field seperator."),
		aliases);
	    error(msg_buff);
	    err_flag++;
	    return;
	}

	addresses = s + 1;			/* REDUNDANT !?!?! */
/*
 *	Remove trailing whitespace from second field
 */
	while (--s >= lastn && whitespace(*s) ) ;
	*++s = '\0';
/*
 *	now get anything after a comma (marks comment within name field)
 */
	for (s = lastn, comment = NULL; *s; s++) {
	    if (*s == '\\') {
		s++;
		continue;
		}

	    if (*s == '"')
		in_quote = !in_quote;

	    if (in_quote)
		continue;

	    if (*s == ',') {
		comment = s + 1;
		while (--s >= lastn && whitespace(*s) ) ;
	        *++s = '\0';		/* Trailing whitespace... */
		break;
	    }
	}

/*
 *	strip leading whitespace from comment
 */
	if (comment) {
	  while (*comment != '\0' && whitespace(*comment) )
	    comment++;
	}

/*
 *	remainder of line is the addresses, remove leading and
 *	trailing whitespace
 */
	while (*addresses != '\0' && whitespace(*addresses) )
	    addresses++;

	s = addresses + strlen(addresses);
	while (--s >= addresses && whitespace(*s) ) ;
	*++s = '\0';

/*
 *	Now strip out any whitespace (and make sure it was
 *	legal whitespace).  Legal whitespace follows ','.
 */
	for (s = addresses, out = addresses; *s; s++) {
	    if (whitespace(*s)) {
	        if (*(out-1) != ',') {
	            err_flag++;			/* Keep going for now */
		    *out++ = *s;
	        }
	    }
	    else {
		*out++ = *s;
	    }
	}
	*out = '\0';

/*
 *	We have to delay reporting the error until the (legal)
 *	spaces are striped, otherwise addresses is all screwed
 *	up and doesn't display well at all.
 */
	if (err_flag) {
	    sprintf(msg_buff, catgets(elm_msg_cat,
	        NewaliasSet, NewaliasAddressWSNotAllowed,
	        "Error - whitespace in address '%.30s' is not allowed."),
	        addresses);
	    error(msg_buff);
	    return;
	}

/*
 *	split the lastn field into firstn and lastn
 */
	for (s = lastn, firstn = NULL; *s; s++) {
	    if (*s == '\\') {
		s++;
		continue;
		}

	    if (*s == '"')
		in_quote = !in_quote;

	    if (in_quote)
		continue;

	    if (*s == ';') {
		firstn = s + 1;
		while (--s >= lastn && whitespace(*s) ) ;
		*++s = '\0';		/* Trailing whitespace... */
		break;
	    }
	}

/*
 *	strip leading whitespace from firstn
 */
	if (firstn) {
	    while (*firstn && whitespace(*firstn))
		firstn++;
	}

/*
 *	now remove 'escapes' from name/comment and aliases fields, in place
 */
	de_escape(lastn);
	if (firstn) {
	    de_escape(firstn);
	}
	if (comment) {
	    de_escape(comment);
	}
	de_escape(addresses);

	add_to_table(data, aliases, lastn, firstn, comment, addresses);
}

int
do_newalias(inputname, dataname, fromelm, textwarn)
char *inputname, *dataname;
int fromelm, textwarn;
{
	FILE *in, *data;

/*
 *	try and allocate a big buffer (larger than a 64k segment...
 *		if it succeeds, mark dbz to run in-core
 *		if not, allocate a smaller buffer
 */
	if ((buffer = malloc(20 * VERY_LONG_STRING)) == NULL) {
	    if ((buffer = malloc(2 * VERY_LONG_STRING)) == NULL) {
	        error(catgets(elm_msg_cat, NewaliasSet, NewaliasNoAlloc,
	                "Unable to allocate space for alias buffer!"));
	        return(-1);
	    }
	    buffer_size = 2 * VERY_LONG_STRING;
	    dbz_incore(FALSE);
	}
	else {
	    buffer_size = 20 * VERY_LONG_STRING;
	    dbz_incore(TRUE);
	}

	if ((in = fopen(inputname,"r")) == NULL) {
	    if ( textwarn )
	    {
	        sprintf(msg_buff, catgets(elm_msg_cat,
	            NewaliasSet, NewaliasNoOpenIn,
	            "Couldn't open %s for input!"), inputname);
	        error(msg_buff);
	    }
	    free(buffer);
	    return(-1);
	}

	if ((aliases_hash = dbz_fresh(dataname, 4999, 0, 0)) == NULL) {
	    MCsprintf(msg_buff, catgets(elm_msg_cat,
	            NewaliasSet, NewaliasNoOpendbz,
		    "Couldn't open %s.pag or %s.dir for output!"),
	        dataname, dataname);
	    error(msg_buff);
	    free(buffer);
	    return(-1);
	}

	if ((data = fopen(dataname, "w")) == NULL) {
	    sprintf(msg_buff, catgets(elm_msg_cat,
	            NewaliasSet, NewaliasNoOpenOut,
	            "Couldn't open %s for output!"), dataname);
	    error(msg_buff);
	    free(buffer);
	    return(-1);
	}

	buff_loaded = 0; 	/* file buffer empty right now! */
	al_count = 0;
	err_flag = 0;

	while (get_alias(in, fromelm) != -1) {
	    put_alias(data);
	    if (err_flag) break;
	}

	if (err_flag) {
	    if (fromelm) sleep(2);
	    error(catgets(elm_msg_cat, NewaliasSet, NewaliasNoSave,
	            "** Not saving tables!  Please fix and re-run!"));
	    free(buffer);
	    return(-1);
	}
	else {
	    dbz_close(aliases_hash);
	    fclose(data);
	    fclose(in);
	    free(buffer);
	
	    if (al_count == 0) {
	        delete_alias_files(dataname, fromelm);
	    }

	    return(al_count);
	}
	/*NOTREACHED*/
}

delete_alias_files(dataname, fromelm)
char *dataname;
int fromelm;
{
/*
 *	This routine remove all the alias hash and data files.
 *	This is called from do_newalias() when there are no user
 *	aliases to be kept.
 */

	char fname[SLEN];

	if (unlink(dataname)) {
	  sprintf(msg_buff,
		catgets(elm_msg_cat, NewaliasSet, NewaliasCouldntDeleteData,
		"Could not delete alias data file %s!"), fname);
	  error(msg_buff);
	  if (fromelm) sleep(2);
	}

	sprintf(fname,"%s.dir", dataname);
	if (unlink(fname)) {
	  sprintf(msg_buff,
		catgets(elm_msg_cat, NewaliasSet, NewaliasCouldntDeleteHash,
		"Could not delete alias hash file %s!"), fname);
	  error(msg_buff);
	  if (fromelm) sleep(2);
	}

	sprintf(fname,"%s.pag", dataname);
	if (unlink(fname)) {
	  sprintf(msg_buff,
		catgets(elm_msg_cat, NewaliasSet, NewaliasCouldntDeleteHash,
		"Could not delete alias hash file %s!"), fname);
	  error(msg_buff);
	  if (fromelm) sleep(2);
	}

}

