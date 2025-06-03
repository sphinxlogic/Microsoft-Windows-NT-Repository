
static char rcsid[] = "@(#)$Id: addr_util.c,v 5.5 1992/12/11 01:45:04 syd Exp $";

/*******************************************************************************
 *  The Elm Mail System  -  $Revision: 5.5 $   $State: Exp $
 *
 * 			Copyright (c) 1988-1992 USENET Community Trust
 * 			Copyright (c) 1986,1987 Dave Taylor
 *******************************************************************************
 * Bug reports, patches, comments, suggestions should be sent to:
 *
 *	Syd Weinstein, Elm Coordinator
 *	elm@DSI.COM			dsinc!elm
 *
 *******************************************************************************
 * $Log: addr_util.c,v $
 * Revision 5.5  1992/12/11  01:45:04  syd
 * remove sys/types.h include, it is now included by defs.h
 * and this routine includes defs.h or indirectly includes defs.h
 * From: Syd
 *
 * Revision 5.4  1992/11/26  00:46:13  syd
 * changes to first change screen back (Raw off) and then issue final
 * error message.
 * From: Syd
 *
 * Revision 5.3  1992/10/31  18:52:51  syd
 * Corrections to Unix date parsing and time zone storage
 * From: eotto@hvlpa.att.com
 *
 * Revision 5.2  1992/10/25  02:18:01  syd
 * fix found_year flag
 * From: Syd
 *
 * Revision 5.1  1992/10/03  22:58:40  syd
 * Initial checkin as of 2.4 Release at PL0
 *
 *
 ******************************************************************************/

/** This file contains addressing utilities 

**/

#include "headers.h"
#include "s_elm.h"

#include <sys/stat.h>
#ifdef PWDINSYS
#  include <sys/pwd.h>
#else
#  include <pwd.h>
#endif

#include <ctype.h>

#ifdef BSD 
#undef tolower
#undef toupper
#endif

char *get_full_name(), *get_alias_address(), *get_token();
char *strcpy(), *strcat(), *strncpy();


#define SKIP_WS(p) while (isspace(*p)) p++
#define SKIP_ALPHA(p) while (isalpha(*p)) p++
#define SKIP_DIGITS(p) while (isdigit(*p)) p++

static char *day_name[8] = {
    "sun", "mon", "tue", "wed", "thu", "fri", "sat", 0
};

static char *month_name[13] = {
    "jan", "feb", "mar", "apr",
    "may", "jun", "jul", "aug",
    "sep", "oct", "nov", "dec", 0
};

static int month_len[12] = {
    31, 28, 31, 30, 31, 30, 31,
    31, 30, 31, 30, 31 };

/* The following time zones are taken from a variety of sources.  They
 * are by no means exhaustive, but seem to include most of those
 * in common usage.  A comprehensive list is impossible, since the same
 * abbreviation is sometimes used to mean different things in different
 * parts of the world.
 */
static struct tzone {
    char *str;
    int offset; /* offset, in minutes, EAST of GMT */
} tzone_info[] = {
    /* the following are from rfc822 */
    "ut", 0, "gmt", 0,
    "est", -5*60, "edt", -4*60,
    "cst", -6*60, "cdt", -5*60,
    "mst", -7*60, "mdt", -6*60,
    "pst", -8*60, "pdt", -7*60,
    "z", 0, /* zulu time (the rest of the military codes are bogus) */

    /* these are also popular in Europe */
    "wet", 0*60, "wet dst", 1*60, /* western european */
    "met", 1*60, "met dst", 2*60, /* middle european */
    "eet", 2*60, "eet dst", 3*60, /* eastern european */
    "bst", 1*60, /* ??? british summer time (=+0100) */

    /* ... and Canada */
    "ast", -4*60, "adt", -3*60, /* atlantic */
    "nst", -3*60-30, "ndt", -2*60-30, /* newfoundland */
    "yst", -9*60, "ydt", -8*60, /* yukon */
    "hst", -10*60, /* hawaii (not really canada) */

    /* ... and Asia */
    "jst", 9*60, /* japan */
    "sst", 8*60, /* singapore */

    /* ... and the South Pacific */
    "nzst", 12*60, "nzdt", 13*60, /* new zealand */
    "wst", 8*60, "wdt", 9*60, /* western australia */
    /* there's also central and eastern australia, but they insist on using
     * cst, est, etc., which would be indistinguishable for the us zones */
     (char *) 0, 0
};

translate_return(addr, ret_addr)
char *addr, *ret_addr;
{
	/** Return ret_addr to be the same as addr, but with the login 
            of the person sending the message replaced by '%s' for 
            future processing... 
	    Fixed to make "%xx" "%%xx" (dumb 'C' system!) 
	**/

	register int loc, loc2, iindex = 0;
	register char *remaining_addr;
	
/*
 *	check for RFC-822 source route: format @site:usr@site
 *	if found, skip to after the first : and then retry.
 *	source routes can be stacked
 */
	remaining_addr = addr;
	while (*remaining_addr == '@') {
	  loc = qchloc(remaining_addr, ':');
	  if (loc == -1)
	    break;

	  remaining_addr += loc + 1;
	}

	loc2 = qchloc(remaining_addr,'@');
	loc = qchloc(remaining_addr, '%');
	if ((loc < loc2) && (loc != -1))
	  loc2 = loc;

	if (loc2 != -1) {	/* ARPA address. */
	  /* algorithm is to get to '@' sign and move backwards until
	     we've hit the beginning of the word or another metachar.
	  */
	  for (loc = loc2 - 1; loc > -1 && remaining_addr[loc] != '!'; loc--)
	     ;
	}
	else {			/* usenet address */
	  /* simple algorithm - find last '!' */

	  loc2 = strlen(remaining_addr);	/* need it anyway! */

	  for (loc = loc2; loc > -1 && remaining_addr[loc] != '!'; loc--)
	      ;
	}
	
	/** now copy up to 'loc' into destination... **/

	while (iindex <= loc) {
	  ret_addr[iindex] = remaining_addr[iindex];
	  iindex++;
	}

	/** now append the '%s'... **/

	ret_addr[iindex++] = '%';
	ret_addr[iindex++] = 's';

	/*
	 *  and, finally, if anything left, add that
	 * however, just pick up the address part, we do
	 * not want any comments.  Thus stop copying at
	 * the first blank character.
	 */

	if ((loc = qchloc(remaining_addr,' ')) == -1)
	  loc = strlen(addr);
	while (loc2 < loc) {
	  ret_addr[iindex++] = remaining_addr[loc2++];
	  if (remaining_addr[loc2-1] == '%')	/* tweak for "printf" */
	    ret_addr[iindex++] = '%';
	}
	
	ret_addr[iindex] = '\0';
}

int
build_address(to, full_to)
char *to, *full_to;
{
	/** loop on all words in 'to' line...append to full_to as
	    we go along, until done or length > len.  Modified to
	    know that stuff in parens are comments...Returns non-zero
	    if it changed the information as it copied it across...
	**/

	register int i, j, changed = 0, in_parens = 0, expanded_information = 0;
	register int k, l, eliminated = 0;
	char word[SLEN], next_word[SLEN], *ptr, buffer[SLEN];
	char new_to_list[VERY_LONG_STRING];
	char elim_list[SLEN], word_a[SLEN], next_word_a[SLEN];
	char *qstrpbrk(), *gecos;

	new_to_list[0] = '\0';

	i = get_word(to, 0, word);

	full_to[0] = '\0';

	elim_list[0] = '\0';

	/** Look for addresses to be eliminated from aliases **/
	while (i > 0) {

	  j = get_word(to, i, next_word);

	  if(word[0] == '(')
	    in_parens++;

	  if (in_parens) {
	    if(word[strlen(word)-1] == ')')
	      in_parens--;
	  }

	  else if (word[0] == '-'){
	    for (k=0; word[k]; word[k] = word[k+1],k++);
	    if (elim_list[0] != '\0')
	      strcat(elim_list, " ");
	    strcat(elim_list, word);
	  }
	  if ((i = j) > 0)
	    strcpy(word, next_word);
	}

	if (elim_list[0] != '\0')
	  eliminated++;

	i = get_word(to, 0, word);

	while (i > 0) {

	  j = get_word(to, i, next_word);

try_new_word:
	  if(word[0] == '(')
	    in_parens++;

	  if (in_parens) {
	    if(word[strlen(word)-1] == ')')
	      in_parens--;
	    strcat(full_to, " ");
	    strcat(full_to, word);
	  }

	  else if (word[0] == '-') {
	  }

	  else if (qstrpbrk(word,"!@:") != NULL) {
	    sprintf(full_to, "%s%s%s", full_to,
                    full_to[0] != '\0'? ", " : "", word);
	  }
	  else if ((ptr = get_alias_address(word, TRUE)) != NULL) {

	    /** check aliases for addresses to be eliminated **/
	    if (eliminated) {
	      k = get_word(strip_commas(ptr), 0, word_a);

	      while (k > 0) {
		l = get_word(ptr, k, next_word_a);
		if (in_list(elim_list, word_a) == 0)
		  sprintf(full_to, "%s%s%s", full_to,
			  full_to[0] != '\0' ? ", " : "", word_a);
		if ((k = l) > 0)
		  strcpy(word_a, next_word_a);
	      }
	    } else
	      sprintf(full_to, "%s%s%s", full_to, 
                      full_to[0] != '\0'? ", " : "", ptr);
	    expanded_information++;
	  }
	  else if (strlen(word) > 0) {
	    if (valid_name(word)) {
	      if (j > 0 && next_word[0] == '(')	/* already has full name */
		gecos = NULL;
	      else				/* needs a full name */
		gecos = get_full_name(word);
#if defined(INTERNET) & defined(USE_DOMAIN)
	      sprintf(full_to, "%s%s%s@%s%s%s%s",
		      full_to,
		      (full_to[0] != '\0'? ", " : ""),
		      word,
		      hostfullname,
		      (gecos ? " (" : ""),
		      (gecos ? gecos : ""),
		      (gecos ? ")" : ""));
#else /* INTERNET and USE_DOMAIN */
	      sprintf(full_to, "%s%s%s%s%s%s",
		      full_to,
		      (full_to[0] != '\0'? ", " : ""),
		      word,
		      (gecos ? " (" : ""),
		      (gecos ? gecos : ""),
		      (gecos ? ")" : ""));
#endif /* INTERNET and USE_DOMAIN */
	    } else if (check_only) {
	      printf(catgets(elm_msg_cat, ElmSet, ElmAliasUnknown,
			"(alias \"%s\" is unknown)\n\r"), word);
	      changed++;
	    }
	    else if (! isatty(fileno(stdin)) ) {	/* batch mode error! */
	      Raw(OFF);
	      fprintf(stderr, catgets(elm_msg_cat, ElmSet, ElmCannotExpandNoCR,
			"Cannot expand alias '%s'!\n"), word);
	      fprintf(stderr, catgets(elm_msg_cat, ElmSet, ElmUseCheckalias,
			"Use \"checkalias\" to find valid addresses!\n"));
	      dprint(1, (debugfile,
		      "Can't expand alias %s - bailing out of build_address\n", 
		      word));
	      leave(0);
	    }
	    else {
	      dprint(2,(debugfile,"Entered unknown address %s\n", word));
	      sprintf(buffer, catgets(elm_msg_cat, ElmSet, ElmUnknownAddress,
			"'%s' is an unknown address.  Replace with: "), 
			word);
	      word[0] = '\0';
	      changed++;

	      PutLine0(LINES, 0, buffer);
		
	      (void)optionally_enter(word, LINES, strlen(buffer), FALSE, FALSE);
	      clear_error();
	      if (strlen(word) > 0) {
	        dprint(3,(debugfile, "Replaced with %s in build_address\n", 
			 word));
		goto try_new_word;
	      }
	      else
		dprint(3,(debugfile, 
		    "Address removed from TO list by build_address\n"));
	      continue;
	    }
	  }

	  /* and this word to the new to list */
	  if(*new_to_list != '\0')
	    strcat(new_to_list, " ");
	  strcat(new_to_list, word);

	  if((i = j) > 0)
	    strcpy(word, next_word);
	}

	/* if new to list is different from original, update original */
	if (changed)
	  strcpy(to, new_to_list);

	return( expanded_information > 0 ? 1 : 0 );
}

/* Return smallest i such that table[i] is a prefix of str.  Return -1 if not
 * found.
 */
static int prefix(table, str)
char **table;
char *str;
{
    int i;

    for (i=0;table[i];i++)
	if (strncmp(table[i],str,strlen(*table))==0)
	    return i;
    return -1;
}

/*
Quoting from RFC 822:
     5.  DATE AND TIME SPECIFICATION

     5.1.  SYNTAX

     date-time   =  [ day "," ] date time        ; dd mm yy
						 ;  hh:mm:ss zzz

     day         =  "Mon"  / "Tue" /  "Wed"  / "Thu"
		 /  "Fri"  / "Sat" /  "Sun"

     date        =  1*2DIGIT month 2DIGIT        ; day month year
						 ;  e.g. 20 Jun 82

     month       =  "Jan"  /  "Feb" /  "Mar"  /  "Apr"
		 /  "May"  /  "Jun" /  "Jul"  /  "Aug"
		 /  "Sep"  /  "Oct" /  "Nov"  /  "Dec"

     time        =  hour zone                    ; ANSI and Military

     hour        =  2DIGIT ":" 2DIGIT [":" 2DIGIT]
						 ; 00:00:00 - 23:59:59

     zone        =  "UT"  / "GMT"                ; Universal Time
						 ; North American : UT
		 /  "EST" / "EDT"                ;  Eastern:  - 5/ - 4
		 /  "CST" / "CDT"                ;  Central:  - 6/ - 5
		 /  "MST" / "MDT"                ;  Mountain: - 7/ - 6
		 /  "PST" / "PDT"                ;  Pacific:  - 8/ - 7
		 /  1ALPHA                       ; Military: Z = UT;
						 ;  A:-1; (J not used)
						 ;  M:-12; N:+1; Y:+12
		 / ( ("+" / "-") 4DIGIT )        ; Local differential
						 ;  hours+min. (HHMM)
*/

/* Translate a symbolic timezone name (e.g. EDT or NZST) to a number of
 * minutes *east* of gmt (if the local time is t, the gmt equivalent is
 * t - tz_lookup(zone)).
 * Return 0 if the timezone is not recognized.
 */
static int tz_lookup(str)
char *str;
{
    struct tzone *p; 

    for (p = tzone_info; p->str; p++) {
	if (strcmp(p->str,str)==0) return p->offset;
    }
    dprint(5,(debugfile,"unknown time zone %s\n",str));
    return 0;
}

/* The following routines, get_XXX(p,...), expect p to point to a string
 * of the appropriate syntax.  They return decoded values in result parameters,
 * and return p updated to point past the parsed substring (also stripping
 * trailing whitespace).
 * Return 0 on syntax errors.
 */


/* Parse a time: hours ':' minutes [ ':' seconds ] WS
 * Check that 0<=hours<24, 0<=minutes,seconds<60.
 * Also allow the syntax "digit digit digit digit" with implied ':' in the
 * middle.
 * Convert to minutes and seconds, with results in (*m,*s).
 */
static char *
get_time(p,m,s)
char *p;
int *m, *s;
{
    int hours, minutes, seconds;

    /* hour */
    if (!isdigit(*p)) {
	dprint(5,(debugfile,"missing time: %s\n",p));
	return 0;
    }
    hours = atoi(p);
    SKIP_DIGITS(p);
    if (*p++ != ':') {
	/* perhaps they just wrote hhmm instead of hh:mm */
	minutes = hours % 60;
	hours /= 60;
    }
    else {
	if (hours<0 || hours>23) {
	    dprint(5,(debugfile,"ridiculous hour: %d\n",hours));
	    return 0;
	}
	minutes = atoi(p);
	if (minutes<0 || minutes>59) {
	    dprint(5,(debugfile,"ridiculous minutes: %d\n",minutes));
	    return 0;
	}
    }
    SKIP_DIGITS(p);
    if (*p == ':') {
	p++;
	seconds = atoi(p);
	if (seconds<0 || seconds>59) {
	    dprint(5,(debugfile,"ridiculous seconds: %d\n",seconds));
	    return 0;
	}
	SKIP_DIGITS(p);
    }
    else seconds = 0;
    minutes += hours*60;
    SKIP_WS(p);
    *m = minutes;
    *s = seconds;
    return p;
}

/* Parse a year: ['1' '9'] digit digit WS
 */
static char *
get_year(p, result)
char *p;
int *result;
{
    int year;

    if (!isdigit(*p)) {
	dprint(5,(debugfile,"missing year: %s\n",p));
	return 0;
    }
    year = atoi(p);
    /* be nice and allow xx and 19xx */
    if (year>69 && year <= 99)
	year += 1900;
    else if (year > 0 && year < 70)
	year += 2000;
    if (year<1900 || year>2099) {
	dprint(5,(debugfile,"ridiculous year %d\n",year));
	return 0;
    }
    SKIP_DIGITS(p);
    SKIP_WS(p);
    *result = year;
    return p;
}

/* Parse a Unix date from which the leading week-day has been stripped.
 * The syntax is "Jun 21 06:45:44 CDT 1989" with timezone optional.
 * i.e., month day time [ zone ] year
 * where day::=digit*, year and time are as defined above,
 * and month and zone are alpha strings starting with a known 3-char prefix.
 * The month has already been processed by the caller, so we just skip over
 * a leading alpha* WS.
 *
 * Unlike the preceding routines, the result is not an updated pointer, but
 * simply 1 for success and 0 for failure.
 */
static int
get_unix_date(p,y,d,m,s,t)
char *p;
int *y, *d, *m, *s, *t;
{
    char time_zone[STRING];

    SKIP_ALPHA(p);
    SKIP_WS(p);
    if (!isdigit(*p)) return 0;
    *d = atoi(p);  /* check the value for sanity after we know the month */
    SKIP_DIGITS(p);
    SKIP_WS(p);
    p = get_time(p,m,s);
    if (!p) return 0;
    if (isalpha(*p)) {
	get_word(p,0,time_zone);
	*t = tz_lookup(time_zone);
	SKIP_ALPHA(p);
	SKIP_WS(p);
    }
    else if (*p == '+') {
	p++;
	*t = atoi(p);
	SKIP_DIGITS(p);
	SKIP_WS(p);
    }
    else if (*p == '-') {
	p++;
	*t = - atoi(p);
	SKIP_DIGITS(p);
	SKIP_WS(p);
    }
    else *t = 0;
    p = get_year(p,y);
    if (!p) return 0;
    return 1;
}

int
real_from(buffer, entry)
char *buffer;
struct header_rec *entry;
{
	/***** Returns true iff 's' has at least the seven 'from' fields,
	       8 or 9 indicates time zone, 10+ include remote from host.
	       Initialize the date and from entries in the record 
	       and also the message received date/time if 'entry'
	       is not NULL.  *****/

	struct header_rec temp_rec, *rec_ptr;
	char junk[STRING], timebuff[STRING], holding_from[SLEN];
	char time_zone[STRING];
	char mybuf[BUFSIZ], *p, *q, *t;
        int mday, month, year, minutes, seconds, tz, i, found_year;
        long gmttime;

	/* set rec_ptr according to whether the data is to be returned
	 * in the second argument */
	rec_ptr = (entry == NULL ? &temp_rec : entry);

	rec_ptr->time_sent = 0;
	rec_ptr->tz_offset = 0;
	rec_ptr->time_zone[0] = '\0';
	timebuff[0] = '\0';
	junk[0] = '\0';
	year = 0;

	/* From <user> <weekday> <month> <day> <hr:min:sec> [TZ1 [TZ2]] <year> [remote from sitelist] */

	if ((i = get_word(buffer, 0, junk)) < 0)	/* skip From */
	  return(FALSE);

	if (!equal("From", junk))
	  return(FALSE);

	if ((i = get_word(buffer, 5, holding_from)) < 0)
	  return(FALSE);
	buffer += 5 + strlen(holding_from);

	if ((i = get_word(buffer, 0, junk)) < 0)	/* skip day of week */
	  return(FALSE);
	buffer += i;

	if ((i = get_word(buffer, 0, timebuff)) < 0)	/* get month */
	  return(FALSE);
	buffer += i;

	if ((i = get_word(buffer, 0, junk)) < 0)	/* get day */
	  return(FALSE);
	strcat(timebuff, " ");
	strcat(timebuff, junk);
	buffer += i;

	if ((i = get_word(buffer, 0, junk)) < 0)	/* get hr:mm:ss */
	  return(FALSE);
	strcat(timebuff, " ");
	strcat(timebuff, junk);
	buffer += i;

	found_year = FALSE;
 	if ((i = get_word(buffer, 0, time_zone)) >= 0) {	/* get tz1? */
	  strcat(timebuff, " ");
 	  tz = strlen(time_zone);
 	  if ((*time_zone == '+' || *time_zone == '-') && tz == 5) {
 								/* numeric tz */
 		strcat(timebuff, time_zone);
 	  } else if ((*time_zone == '+' || *time_zone == '-') && tz == 3) {
							/* numeric tz 2 digits long */
 		strcat(timebuff, time_zone);
		strcat(timebuff, "00");			/* make it 4 digits long */
 
 	  } else if (isdigit(*time_zone)) {
 		  year = atoi(time_zone);
 
     		  /* be nice and allow xx and 19xx */
     		  if (year>69 && year <= 99)
 		  	year += 1900;
     	  	  else if (year > 0 && year < 70)
 		  	year += 2000;
     	  	  if (year<1900 || year>2099) {
 		  	found_year = FALSE;
     		  } else {
 		  	found_year = TRUE;
 		        strcat(timebuff, time_zone);
 		  }
 		  /* No time_zone found */
 		  time_zone[0] = '\0';
 
 	  } else if (strlen(time_zone) <= 4) {	
 								/* alpha tz */
 		  strcat(timebuff, time_zone);
 	  }
	  buffer += i;
	}

	if (found_year == FALSE) {
	  if ((i = get_word(buffer, 0, junk)) >= 0) {	/* get tz2? */
	    strcat(timebuff, " ");
	    if (isdigit(*junk)) {
		    found_year = TRUE;
		    strcat(timebuff, junk);
	    } else if (strlen(junk) <= 4)	/* alpha tz */
		    strcat(timebuff, junk);
	    buffer += i;
	  }
	}

	if (found_year == FALSE) {
	  if ((i = get_word(buffer, 0, junk)) < 0)	/* get year */
	    return(FALSE);
	  strcat(timebuff, " ");
	  strcat(timebuff, junk);
	  buffer += i;
	}

 	if (*time_zone) {
         	/* convert time_zone to lower case */
         	for (p=mybuf, q=mybuf+sizeof mybuf, t = time_zone; 
 		     *t && p<q; p++, t++) {
 	  	   *p = isupper(*t) ? tolower(*t) : *t;
         	}
 		*p = 0;
 		p = mybuf;
 		strncpy(rec_ptr->time_zone, p, sizeof(rec_ptr->time_zone) - 1);
 	}
 
	/** now process the info gotten out of the record! **/

	strncpy(rec_ptr->from, holding_from, STRING-1);
	rec_ptr->from[STRING-1] = '\0';

        /* first get time into lower case */
        for (p=mybuf, q=mybuf+sizeof mybuf, t = timebuff; 
	     *t && p<q; p++, t++) {
	  *p = isupper(*t) ? tolower(*t) : *t;
        }
	*p = 0;
	p = mybuf;

	month = prefix(month_name, p);
	get_unix_date(p, &year, &mday, &minutes, &seconds, &tz);
	month_len[1] = (year % 4) ? 28 : 29;
	if (mday <= 0 || mday > month_len[month]) {
	  dprint(5, (debugfile, "ridiculous day %d of month %d\n", mday, month));
	}

        gmttime = year - 1970;		 /* make base year */
	/* now we have days adjusted for leap years */
        gmttime = gmttime * 365 + (gmttime + 1) / 4;  
        for (i = 0; i < month; i++)
  		gmttime += month_len[i];
        gmttime += mday - 1;		/* and now to the day */
        gmttime *= 24 * 60;			/* convert to minutes */
        gmttime += minutes - tz;
        rec_ptr->time_sent = gmttime * 60 + seconds;	/* now unix seconds since 1/1/70 00:00 GMT */
	rec_ptr->received_time = rec_ptr->time_sent;
	rec_ptr->tz_offset = tz * 60;

	return(year != 0);
}

forwarded(buffer, entry)
char *buffer;
struct header_rec *entry;
{
	/** Change 'from' and date fields to reflect the ORIGINATOR of 
	    the message by iteratively parsing the >From fields... 
	    Modified to deal with headers that include the time zone
	    of the originating machine... **/

	char machine[SLEN], buff[SLEN], holding_from[SLEN];
	int len;

	machine[0] = holding_from[0] = '\0';

	sscanf(buffer, "%*s %s", holding_from);

	/* after skipping over From and address, process rest as date field */

	while (!isspace(*buffer)) buffer++;	/* skip From */
	SKIP_WS(buffer);

	while (*buffer) {
	  len = len_next_part(buffer);
	  if (len > 1) {
	    buffer += len;
	  } else {
	    if (isspace(*buffer))
	      break;
	    buffer++;
	  }
	}
	SKIP_WS(buffer);

	parse_arpa_date(buffer, entry);

	/* the following fix is to deal with ">From xyz ... forwarded by xyz"
	   which occasionally shows up within AT&T.  Thanks to Bill Carpenter
	   for the fix! */

	if (strcmp(machine, holding_from) == 0)
	  machine[0] = '\0';

	if (machine[0] == '\0')
	  strcpy(buff, holding_from[0] ? holding_from : "anonymous");
	else
	  sprintf(buff,"%s!%s", machine, holding_from);

	strncpy(entry->from, buff, STRING-1);
	entry->from[STRING-1] = '\0';
}

/* Parse an rfc822 (with extensions) date.  Return 1 on success, 0 on failure.
 */
parse_arpa_date(string, entry)
char *string;
struct header_rec *entry;
{
    char buffer[BUFSIZ], *p, *q;
    int mday, month, year, minutes, seconds, tz, i;
    long gmttime;

    /* first get everything into lower case */
    for (p=buffer, q=buffer+sizeof buffer; *string && p<q; p++, string++) {
	*p = isupper(*string) ? tolower(*string) : *string;
    }
    *p = 0;
    p = buffer;
    SKIP_WS(p);

    if (prefix(day_name,p)>=0) {
	/* accept anything that *starts* with a valid day name */
	/* also, don't check whether it's right! */

	SKIP_ALPHA(p);
	SKIP_WS(p);

	if (*p==',') {
	    p++;
	    SKIP_WS(p);
	}
	/* A comma is required here, but we'll be nice guys and look the other
	 * way if it's missing.
	 */
    }

    /* date */

    /* day of the month */
    if (!isdigit(*p)) {
	/* Missing day.  Maybe this is a Unix date?
	 */
	month = prefix(month_name,p);
	if (month >= 0 &&
	    get_unix_date(p, &year, &mday, &minutes, &seconds, &tz)) {
		goto got_date;
	}
	dprint(5,(debugfile,"missing day: %s\n",p));
	return 0;
    }
    mday = atoi(p);  /* check the value for sanity after we know the month */
    SKIP_DIGITS(p);
    SKIP_WS(p);

    /* month name */
    month = prefix(month_name,p);
    if (month < 0) {
	dprint(5,(debugfile,"missing month: %s\n",p));
	return 0;
    }
    SKIP_ALPHA(p);
    SKIP_WS(p);

    /* year */
    if (!(p = get_year(p,&year))) return 0;

    /* time */
    if (!(p = get_time(p,&minutes,&seconds))) return 0;

    /* zone */
    for (q=p; *q && !isspace(*q); q++) continue;
    *q = 0;
    if (*p=='-' || *p=='+') {
	char sign = *p++;

	if (isdigit(*p)) {
	    for (i=0; i<4; i++) {
		if (!isdigit(p[i])) {
		    dprint(5,(debugfile,"ridiculous numeric timezone: %s\n",p));
		    return 0;
		}
		p[i] -= '0';
	    }
	    tz = p[0]*10 + p[1];
	    tz *= 60;
	    tz += p[2]*10 + p[3];
	    sprintf(entry->time_zone, "%c%2.2d%2.2d", sign, tz / 60, tz % 60);
	    if (sign=='-') tz = -tz;
	}
	else {
	    /* some brain-damaged dates use a '-' before a symbolic time zone */
	    SKIP_WS(p);
	    strncpy(entry->time_zone, p, sizeof(entry->time_zone) - 1);
	    tz = tz_lookup(p);
	}
    }
    else {
	tz = tz_lookup(p);
	strncpy(entry->time_zone, p, sizeof(entry->time_zone) - 1);
    }

got_date:
    month_len[1] = (year%4) ? 28 : 29;	/* account for leap year */
    if (mday<=0 || mday>month_len[month]) {
	dprint(5,(debugfile,"ridiculous day %d of month %d\n",mday,month));
	return 0;
    }

    gmttime = year - 1970;		/* make base year */
    gmttime = gmttime * 365 + (gmttime + 1) / 4;  /* now we have days adjusted for leap years */
    for (i = 0; i < month; i++)
	gmttime += month_len[i];
    gmttime += mday - 1;		/* and now to the day */
    gmttime *= 24 * 60;			/* convert to minutes */
    gmttime += minutes - tz;
    gmttime *= 60;
    gmttime += seconds;

    entry->time_sent = gmttime;		/* now unix seconds since 1/1/70 00:00 GMT */
    entry->tz_offset = tz * 60;		/* and the tz for reconversion to local zone */

    return 1;
}

fix_arpa_address(address)
char *address;
{
	/** Given a pure ARPA address, try to make it reasonable.

	    This means that if you have something of the form a@b@b make 
            it a@b.  If you have something like a%b%c%b@x make it a%b@x...
	**/

	register int host_count = 0, i;
	char     hosts[MAX_HOPS][NLEN];	/* array of machine names */
	char     *host, *addrptr;

	/*  break down into a list of machine names, checking as we go along */
	
	addrptr = (char *) address;

	while ((host = get_token(addrptr, "%@", 2)) != NULL) {
	  for (i = 0; i < host_count && ! equal(hosts[i], host); i++)
	      ;

	  if (i == host_count) {
	    strcpy(hosts[host_count++], host);
	    if (host_count == MAX_HOPS) {
	       dprint(2, (debugfile, 
           "Can't build return address - hit MAX_HOPS in fix_arpa_address\n"));
	       error(catgets(elm_msg_cat, ElmSet, ElmCantBuildRetAddr,
			"Can't build return address - hit MAX_HOPS limit!"));
	       return(1);
	    }
	  }
	  else 
	    host_count = i + 1;
	  addrptr = NULL;
	}

	/** rebuild the address.. **/

	address[0] = '\0';

	for (i = 0; i < host_count; i++)
	  sprintf(address, "%s%s%s", address, 
	          address[0] == '\0'? "" : 
	 	    (i == host_count - 1 ? "@" : "%"),
	          hosts[i]);

	return(0);
}
