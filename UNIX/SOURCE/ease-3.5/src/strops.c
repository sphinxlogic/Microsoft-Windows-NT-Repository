#ifdef FLUKE
# ifndef LINT
    static char RCSid[] = "@(#)FLUKE  $Header: /home/kreskin/u0/barnett/Src/Ease/ease/src/RCS/strops.c,v 3.4 1991/10/15 17:02:04 barnett Exp $";
# endif LINT
#endif FLUKE

/*
 *	strops.c   -- Contains string operation routines used for constructing
 *		      definitions in cf format.
 *
 *	author	   -- James S. Schoner, Purdue University Computing Center,
 *				        West Lafayette, Indiana  47907
 *
 *	date	   -- July 9, 1985
 *
 *	Copyright (c) 1985 by Purdue Research Foundation
 *
 *	All rights reserved.
 *
 * $Log: strops.c,v $
 * Revision 3.4  1991/10/15  17:02:04  barnett
 * Detect if (one_or_more) next ($2) error
 *
 * Revision 3.3  1991/09/09  16:33:23  barnett
 * Minor bug fix release
 *
 * Revision 3.2  1991/05/16  10:45:25  barnett
 * Better support for System V machines
 * Support for machines with read only text segments
 *
 * Revision 3.1  1991/02/25  22:09:52  barnett
 * Fixed some portability problems
 *
 * Revision 3.0  1991/02/22  18:50:27  barnett
 * Added support for HP/UX and IDA sendmail.
 *
 * Revision 2.1  1990/01/30  15:52:55  jeff
 * Added SunOS/Ultrix/IDA extensions  Jan 24, 1989 Bruce Barnett
 *
 * Revision 2.0  88/06/15  14:42:55  root
 * Baseline release for net posting. ADR.
 */

#include "fixstrings.h"
#include <stdio.h>
#ifdef __svr4__
#include <string.h>
#else
#include <strings.h>
#endif
#include <ctype.h>
#include "symtab.h"

#define MAXTOKPOS   99		/* maximum number of token positions */
#define MAXNAME	    1024	/* maximum length of an identifier   */

extern struct he *LookupSymbol ();
extern char       MakeMac ();
extern void	  FatalError (),
		  PrintError (),
		  ErrorReport ();
extern char *	  malloc ();

short  Rformat = FALSE;			/* class read format flag	  */
short number_of_fields = 0;		/* number of fields on this line */
#ifdef	DATA_RW
static char   *Ptok   = "$  ";		/* positional token structure     */
static char   *Cfield = "$= ";		/* class reference structure	  */
#else
static char   *Ptok   = NULL;		/* positional token structure     */
static char   *Cfield = NULL;		/* class reference structure	  */
#endif
static char   *Ofield = "$-";		/* one token match structure	  */
static char   *Zfield = "$*";		/* zero or more tokens structure  */
static char   *Pfield = "$+";		/* one or more tokens structure	  */

/*
 *  FLUKE jps 25-apr-86
 *
 *  Add the three new $%x, $%y, and $!x match operators that Sun introduced
 *  with release 3.0.
 */

#ifdef	DATA_RW
static char   *Mfield = "$% ";		/*    match in specified YP map */
static char   *Nfield = "$! ";		/* no match in specified YP map */
static char   *Mtest  = "$? ";		/* conditional macro test string  */
#else
static char   *Mfield = NULL;		/*    match in specified YP map */
static char   *Nfield = NULL;		/* no match in specified YP map */
static char   *Mtest  = NULL;		/* conditional macro test string  */
#endif

#ifndef	DATA_RW
/* This section of code is for those machines with split text and data
 * areas - where string constants are not modifiable
 */
#define nil (char *)0

char *	  Strdup(string) /* a clone of the strdup() routine */
char	*string;
{
    char *s;
    s=malloc(strlen(string));
    if (!s) return (nil);
    (void) sprintf(s,"%s",string);
    return (s);
}
void InitStrOps()
{
	if(!(Ptok = Strdup("$  ")) || !(Cfield = Strdup("$= ")) ||
	   !(Mfield = Strdup("$% ")) || !(Nfield = Strdup("$! ")) ||
	   !(Mtest = Strdup("$? ")))
	   	FatalError("Out of memory in InitStrOps()", (char *)NULL);
}
#endif


/*
 *	ConvOpt () -- Convert an Ease option identifier (optid) by returning a
 *		      string representation of the cf format.  
 *
 */
char *
ConvOpt (optid) 
register enum opts optid;
{
	switch (optid) {
		case opt_A  :	return ("A");
		case opt_a  :	return ("a");
		case opt_B  :	return ("B");
		case opt_b  :	return ("b");
		case d_opt_b:	return ("b");
		case opt_C  :	return ("C");
		case opt_c  :	return ("c");
		case opt_D  :	return ("D");
		case opt_d  :	return ("d");
		case opt_e  :
		case e_opt_e:	return ("e");
		case opt_F  :	return ("F");
		case opt_f  :	return ("f");
		case opt_g  :	return ("g");
		case opt_h  :	return ("h");	/* SunOS Maxhops */
		case opt_H  :	return ("H");
		case opt_I  :   return ("I");	/* HP/UX */
		case opt_i  :
		case d_opt_i:	return ("i");
		case opt_L  :	return ("L");
		case opt_m  :
		case e_opt_m:	return ("m");
		case opt_N  :	return ("N");
		case opt_n  :	return ("n");
		case opt_o  :	return ("o");
		case opt_P  :	return ("P");
		case e_opt_p:	return ("p");
		case opt_Q  :	return ("Q");
		case opt_q  :	return ("q");
		case d_opt_q:	return ("q");
		case opt_r  :	return ("r");
		case opt_R  :	return ("R");
		case opt_S  :	return ("S");
		case opt_s  :	return ("s");
		case opt_T  :	return ("T");
		case opt_t  :	return ("t");
		case opt_u  :	return ("u");
		case opt_v  :	return ("v");
		case opt_W  :	return ("W");
		case e_opt_w:	return ("w");
		case opt_x  :	return ("x");
		case opt_X  :	return ("X");
		case opt_y  :	return ("y");
		case opt_Y  :	return ("Y");
		case opt_z  :	return ("z");
		case opt_Z  :	return ("Z");
		case e_opt_z:	return ("z");
		case opt_SL  :	return ("/");
		default     :	FatalError ("Bad case in ConvOpt ()", (char *) NULL);
	}
	/*NOTREACHED*/
}


/*
 *	ConvFlg () -- Convert an Ease mailer flag identifier (flgid) by 
 *		      string representation of the cf format.  
 *
 */
char *
ConvFlg (flgid)
register enum flgs flgid;	/* flag identifier */
{
	switch (flgid) {
		case flg_f:	return ("f");
		case flg_r:	return ("r");
		case flg_S:	return ("S");
		case flg_n:	return ("n");
		case flg_l:	return ("l");
		case flg_s:	return ("s");
		case flg_m:	return ("m");
		case flg_F:	return ("F");
		case flg_D:	return ("D");
		case flg_M:	return ("M");
		case flg_x:	return ("x");
		case flg_P:	return ("P");
		case flg_u:	return ("u");
		case flg_h:	return ("h");
		case flg_H:	return ("H");
		case flg_A:	return ("A");
		case flg_U:	return ("U");
		case flg_e:	return ("e");
		case flg_X:	return ("X");
		case flg_L:	return ("L");
		case flg_p:	return ("p");
		case flg_I:	return ("I");
		case flg_C:	return ("C");
		case flg_E:	return ("E");
		case flg_R:	return ("R");
		case flg_V:	return ("V");	/* IDA */
		case flg_B:	return ("B");	/* IDA */
		default   :	FatalError ("Bad case in ConvFlg ()", (char *) NULL);
	}
	/*NOTREACHED*/
}


/*
 *	ConvMat () -- Convert an Ease mailer attribute (mat) by returning a
 *		      string representation of the cf format.  
 *
 */
char *
ConvMat (mat)
register enum mats mat;		/* mailer attribute flag */
{
	switch (mat) {
		case mat_path		: return ("P");
		case mat_flags		: return ("F");
		case mat_sender		: return ("S");
		case mat_recipient	: return ("R");
		case mat_argv		: return ("A");
		case mat_eol		: return ("E");
		case mat_maxsize	: return ("M");
		default			: FatalError ("Bad case in ConvMat ()", (char *) NULL);
	}
	/*NOTREACHED*/
}


/*
 *	MacScan () -- Scan a string (pstring) for macros, replacing the Ease
 *		      form with the one-character form required by cf format.
 *
 */
char *
MacScan (pstring)
char *pstring;		/* macro expandable string */
{
	register char *searchptr;	/* string search pointer 	*/
	register char *bptr, *eptr;	/* macro begin and end pointers */
	char macname [MAXNAME];		/* macro name buffer		*/
	int	quote = 0;			/* flag for detecting a quote() function */

	if ((searchptr = pstring) == NULL)
		return ((char *) NULL);
	while (*searchptr != '\0') 	/* find and rewrite all macros  */
		if (*searchptr == '\\') {
			searchptr = searchptr + 2;
			continue;
		} else if (*searchptr++ == '$') {
		    if (*searchptr == '{') {
			if (sscanf (searchptr + 1, "quote(%[^)])", macname) == 1) {
			    /* a quote(macro) sequence */
			        quote++;
			} else if (sscanf (searchptr + 1, "%[^}]", macname) != 1) {
				PrintError ("Invalid macro format: %s", searchptr + 1);
				return ((char *) NULL);
			} 
			if (quote) {
			    quote=0;
			    *searchptr++='!'; /* insert a quote */
			    /* insert the macro letter */
			    *searchptr++ = MakeMac (LookupSymbol (macname),
						    (unsigned) ID_MACRO);
			    /* now looking at quote(   */
			    /* must skip over everything until )}, and then rewrite
			       the rest of the macro */
			    bptr = eptr = searchptr; 
			    while (*eptr && *eptr != ')') /* skip to quote */
			      eptr++;
			    while (*eptr && *eptr != '}') /* skip past } */
			      eptr++;
			    eptr++;
			    do	/* copy everything from eptr to end of line */
			      *bptr++ = *eptr;
			    while (*eptr++ != '\0');
			} else {
			    *searchptr++ = MakeMac (LookupSymbol (macname),
						    (unsigned) ID_MACRO);
			    bptr = eptr = searchptr;
			    while (*eptr++ != '}')	/* delete until end of {name} */
			      /* empty */ ;
			    do	/* copy rest of line */
			      *bptr++ = *eptr;
			    while (*eptr++ != '\0');
			}
		    } else if (isupper(*searchptr)){ /* $ not followed by { */
			/* macro name might be one character */
			/* but or might be more than one.    */
			if (sscanf (searchptr, "%[A-Z]", macname) != 1) {
				PrintError ("Invalid macro format: %s", searchptr + 1);
				return ((char *) NULL);
			} 
			*searchptr++ = MakeMac (LookupSymbol (macname),
						(unsigned) ID_MACRO);
			bptr = eptr = searchptr;
			while (isupper(*eptr))	/* delete old macro chars */
				eptr++;
			do
				*bptr++ = *eptr;
			while (*eptr++ != '\0');
		    } 
		} /* end of (if char == '$' ) */
	return (pstring);
}


/*
 *	MakeRStr () -- Construct and return a pointer to a class read string 
 *		       using the filename fname and read format rformat.  
 *
 */
char *
MakeRStr (fname, rformat)
char *fname,			/* file name for class read */
     *rformat;			/* format for class read    */
{
	register char *res;	/* resultant read string    */

	Rformat = TRUE;		/* set read format flag     */
	if (rformat == NULL)
		return (fname);
	res = (char *) realloc (fname, strlen (fname) + strlen (rformat) + 2);
	if (res == NULL)
		FatalError ("System out of string space in MakeRStr ()", (char *) NULL);
	res = strcat (res, " ");	/* construct read string */
	res = strcat (res, rformat);
	free (rformat);
	return (res);
}


/*
 *	ListAppend () -- Append string list2 to string list1 using the 
 *			 separator sep.  A pointer to the newly constructed
 *			 string is returned.
 *
 */
char *
ListAppend (list1, list2, sep)
char *list1,			/* first string 	*/
     *list2,			/* second string  	*/
     *sep;			/* string separator	*/
{
	register char *res;	/* resultant string	*/

	res = (char *) malloc (strlen (list1) + strlen (list2) + strlen (sep) + 1);
	if (res == NULL)
		FatalError ("System out of string space in ListAppend ()", (char *) NULL);
	res = strcpy (res, list1);
	if (list1 != NULL)	/* use separator if first string not null */
		res = strcat (res, sep);
	res = strcat (res, list2);
	return (res);
}


/*
 *	MakeCond () -- Construct a macro conditional string in cf format.  The
 *		       conditional is based on the macro testmac, with an "if
 *		       set" result ifstring, which may contain an optional 
 *		       "if not set" result string appended to it.
 *
 */
char *
MakeCond (testmac, ifstring)
struct he *testmac;		/* macro for conditional testing 	     */
char 	  *ifstring; 		/* "if macro set" result string(s)  	     */
{
	register char *res;	/* resultant conditional string		     */

	Mtest[2] = MakeMac (testmac, (unsigned) ID_MACRO); /* get one-char macro rep */
	res = (char *) malloc (strlen (ifstring) + 6);
	if (res == NULL)
		FatalError ("System out of string space in MakeCond ()", (char *) NULL);
	res = strcpy (res, Mtest);
	res = strcat (res, ifstring);		/* build result part	  */
	res = strcat (res, "$.");		/* end of conditional     */
	free (ifstring);
	return (res);
}


/*
 *	MakePosTok () -- Construct and return a positional token string 
 *			 representation from the parameter num.
 *
 */
char *
MakePosTok (num)
register int num;	        /* numerical value of positional token */
{
    char buf[100];
	if (num > MAXTOKPOS) {
		ErrorReport ("Positional token too large.\n");
		return ((char *) NULL);
	} else {
	    if (num > number_of_fields) {
		sprintf(buf," $%d used in right hand side of rule, but maximum positional parameter is %d\n", num, number_of_fields);
		ErrorReport(buf);
	    }
		if (num > 9) {	/* two-digit positional token */
			Ptok[1] = '0' + (num / 10);
			Ptok[2] = '0' + (num % 10);
			Ptok[3] = '\0';
		} else {
			Ptok[1] = '0' + num;
			Ptok[2] = '\0';
		}
	return (Ptok);
	}
}


/*
 *	Bracket () -- Construct and return a cf string form of the 
 *		      canonicalization of the string identifier passed in
 *		      the string parameter psb if dflag is true, else
 *		      simply bracket the identifier without dollar signs
 *		      for numeric hostname specifications.
 *
 */
char *
Bracket (psb, dflag)
char *psb;			/* identifier to be canonicalized */
short dflag;			/* dollar flag 			  */
{
	register char *res;	/* resultant cf form 		  */
	register short extra;	/* extra space needed for malloc  */
	
	extra = dflag ? 5 : 3;
	res = (char *) malloc (strlen (psb) + extra);
	if (res == NULL)
		FatalError ("System out of string space in Bracket ()", (char *) NULL);
	if (dflag)
		res = strcpy (res, "$[");
	else
		res = strcpy (res, "[");
	res = strcat (res, psb);
	if (dflag)
		res = strcat (res, "$");
	res = strcat (res, "]");
	return (res);
}
/*
 *	DbmParen () -- Construct and return a cf string form of the 
 *		      dbm reference of the IDA database identifier passed in
 *		      the string parameter db_psb, looking up the string identifier
 *		      passed in the string parameter lup_psb.
 *
 */
char *
DbmParen (db_psb, lup_psb)
char *db_psb;			/* identifier of data base */
char *lup_psb;			/* identifier being looked up */
{
	register char *res;	/* resultant cf form 		  */
	
	res = (char *) malloc (strlen (lup_psb) + 7);
	if (res == NULL)
		FatalError ("System out of string space in DbmParen ()", (char *) NULL);
	res = strcpy (res, "$(x ");
    *(res+2) = *db_psb;
	res = strcat (res, lup_psb);
	res = strcat (res, " $)");
	return (res);
}


/*
 *	MakeRSCall () -- Construct and return a cf string form of a call
 *			 to a ruleset (cid), which would pass to it the
 *			 remainder of a rewriting address (rwaddr).
 *
 */
char *
MakeRSCall (cid, rwaddr)
register struct he *cid;	/* called ruleset identifier	     */
register char *rwaddr;		/* remainder of rewriting address    */
{
	register char *res;	/* resultant cf string for the call  */
	
	if (!ISRULESET(cid->idtype)) {	/* check validity of ruleset */
		PrintError ("Undefined ruleset identifier: %s", cid->psb);
		return ((char *) NULL);
	}
	/*
	 * FLUKE jps - 8-may-86 - boost string size by 1 to accomodate space
	 * character.
	 */
	res = (char *) malloc (strlen (cid->idval.rsn) + strlen (rwaddr) + 4);
	if (res == NULL)
		FatalError ("System out of string space in MakeRSCall ()", (char *) NULL);
	res = strcpy (res, "$>");	/* construct the call string */
	res = strcat (res, cid->idval.rsn);
	res = strcat (res, " ");  /* FLUKE jps - work around sendmail bug:
				   * whitespace is needed to separate tokens:
				   * for example:  $>30$D will confuse
				   * sendmail, but $>30 $D is OK.
				   */
	res = strcat (res, rwaddr);
	return (res);
}


/*
 *	MakeField () -- Construct and return the cf string format for a
 *			field variable.  The match count (count), an optional
 *			class (class), and a match repetition flag (fstar)
 *			are used to determine what type of field string to
 *			construct.
 *
 *  FLUKE jps 25-apr-86 - Modified to add a fourth parameter "isYPmap".  This
 *  supports Sun's new Yellow Pages match patterns added in release 3.0.
 *
 */
char *
    MakeField (count, class, fstar, isYPmap)
register int count;		/* match count (0 or 1) */
register struct he *class;	/* optional class type  */
register short fstar;		/* repetition flag	*/
register short isYPmap;		/* "class" name is really a YP map name */
{
	switch (count) {
		case 0:	  if (class == NULL)	/* any token is valid */
				if (fstar)
					return (Zfield);
				else {
					ErrorReport ("Invalid field type.\n");
					return ((char *) NULL);
				}
			  else {		/* match 0 from class or map */
				if (isYPmap) {
				    /*  "class" is a misnomer here; it's really
				     *  a simple macro name for a YP map.
				     *  FLUKE jps 25-apr-86
				     */
				    Nfield[2] = MakeMac (class, (unsigned) ID_MACRO);
				    return (Nfield);
				} else {
				Cfield[1] = '~';
				Cfield[2] = MakeMac (class, (unsigned) ID_CLASS);
				return (Cfield);
			  }
			  }
		case 1:	  if (class == NULL)	/* any token is valid */
				if (fstar)
					return (Pfield);
				else
					return (Ofield);
			  else {		/* match 1 from class or map */
				if (isYPmap) {
				    /*  "class" is a misnomer here; it's really
				     *  a simple macro name for a YP map.
				     *  FLUKE jps 25-apr-86
				     */
				    Mfield[2] = MakeMac (class, (unsigned) ID_MACRO);
				    return (Mfield);
				} else {
				Cfield[1] = '=';
				Cfield[2] = MakeMac (class, (unsigned) ID_CLASS);
				return (Cfield);
				}
			  }
		default:  ErrorReport ("Invalid field type.\n");
	}
	/*NOTREACHED*/
}
