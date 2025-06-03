/* PROGRAM	Handle a VMS Help request from a WWW client	VMSHelpGate.c
**		===========================================
**		PROGRAM FOR CGI/1.0 SCRIPTS TO GENERATE HTML
**		RENDITIONS OF VMS HELP LIBRARY CONTENTS.
**		  Note that in the present absence of table
**		  support, PRE formatting is used to create
**		  multicolumn displays.  The tab expansion
**		  functions cannot take into account any
**		  numbering of anchors performed by the client,
**		  so the displays will not be as intented when
**		  this gateway is used by the WWW LineMode
**		  browser, or by Lynx when set to "Numbered
**		  Links" mode.  The PRE formatting assumes
**		  the client has an 80 column (or greater)
**		  display window in effect.
**			    FOR USE ON VMS.
**
** USAGE (called from a script, e.g., HelpGate.com):
**
**	$ VMSHelpGate :== $device:[directory]VMSHelpGate.exe
**	$ VMSHelpGate 				! cern server
**	$ VMSHelpGate method url protocol	! decthreads server.
**
**		The symbol WWW_PATH_INFO is set by the httpd and holds
**		the argument for the help library and module to be
**		returned to the client as HTML.  It should be of the
**		form:  /HELP[/@library][/topic[/subtopic...]]
**		with the following in the httpd configuration file to
**		set it up:
**
**		map	/help	/htbin/helpgate/HELP
**		map	/help/*	/htbin/helpgate/HELP/*
**		exec	/htbin/*	/HTTPD_Dir/*
**
**		If no "/@library" is specified, "/@SYS$HELP:HELPLIB"
**		is assumed.
**
**		The symbols WWW_HOST_ACRONYM for indicating the
**		host (e.g., WFEB) in the TITLE, and WWW_COPYRIGHT_STRING
**		for showing a copyright notice at the tops of displays,
**		should be set by the script.
**
** AUTHORS:
**	JFG	Jean-Francois Groff, CERN, Geneva	jfg@info.cern.ch
**	FM	Foteos Macrides				macrides@sci.wfeb.edu
**	DLJ	David L. Jones, Ohio State Univ.	vman+@osu.edu
**
** HISTORY:
**		Originally written by JFG as a standalone gateway.
**		Extensively modified by FM for use with HTTP/1.0 and
**		CGI scripts.
**
** 	JFG's HISTORY:
**	--------------
** 1.1  26 Feb 92 (JFG) Enabled strange topics
** 1.0	 7 Oct 91 (JFG) First release
** 0.4	 2 Oct 91 (JFG) Handle help summary. Definitive address format :
**			//node[:port]/HELP[/@library][/topic[/subtopic...]]
** 0.3	27 Sep 91 (JFG)	Created from h2h.c
**
** (c) CERN WorldWideWeb project 1990-1992. See Copyright.html for details
**
**	Current HISTORY:
**	----------------
** 2.0   1 May 94 (FM)	Polishing up my modifications, for general release.
**
**	DLJ mods:
** 	--------------------
**	 5 May 95	Convert to support DECthreads scripting system
**			(cgilib).  Replace printfs with cgi_printf.
**	 7 May 94	Replace WWW_HELP_DIR 'caching' with direct library 
**			access via VMS Librarian utility routines.
**
** 2.1   8 May 94 (FM)	Adopted DLJ's mods for CERN server too, with minor
**			fixes for spacing in PRE formatted sections, and bug
**			fix in lbr_close() of lbrio.c which caused ACCIO due
**			to bad argument in free() call.
**	11 May 94 (FM)  Added fixes from DLJ for Alphas.
**	27 Jun 94 (FM)  Fixes in anchor_strcpy() for current HELP library
**			rules and escaping of any lead colon.  Unescape
**			it in main() if still present.
**	DLJ mods:
** 	--------------------
**	 2 Jul 94	Make qualifers within a level anchors and include
**			in them with hrefs in generated menus.  This more
**			more closely mimics DCL HELP.  Use cleaner algorithm
**			to determine line-breaks in menus.
**
**	 3 Jul 94	Add hack to deal with bugs in mac-mosaicB6 and
**			<PRE> operations.
**
**	 31 Oct 94	Escape '<', '>', and '&' in help text as per HTML spec.
**			(No checks being done within anchors)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unixlib.h>

#include "lbrio.h"		/* direct library access  *DLJ* */
#define TAB_SIZE 16		/* 11 is what DCL HELP uses. */
#define LINE_WIDTH 80
/*
 * For DECthreads, include the cgilib.h
 */
#ifdef NOCGILIB
#define cgi_init(a,b)  1
#define cgi_info(a) getenv(strcpy(&cgi_info_buf[4], a)-4)
static char cgi_info_buf[64] = { 'W', 'W', 'W', '_' };
#define cgi_printf printf
#else
#include "cgilib.h"
#endif


/** Headers taken from h2h.c **/

/* Maximum line size for buffers */
#define LSIZE 256

/* Maximum command line size for VMS */
#define CSIZE 256

/* Maximum VMS file name size (note that it's actually 49.49) */
#define FSIZE 80

/* Maximum anchor size */
#define ASIZE 80


/** Function declarations **/

int strcasecomp(char *, char *);
int strncasecomp(char *, char *, int);
char *anchor_strcpy(char *, char *);
void show_copyright(void);
int hlp_to_html(lbr_index, char *);
int lis_to_html(lbr_index, char *);
lbr_index open_hlp(char *, char *);
static char *pre_start;		/* either "/n<PRE>" or "<PRE>/n" (macmosaic) */

/*****************************************************************************

main  : Retrieve information from VMS help libraries or cached modules
	and convert to HTML "on the fly".

Input :
	symbol WWW_PATH_INFO set by httpd : HT address

------------------------------------------------------------------------------*/

int main(int argc, char *argv[])
{
  char *arg;
  lbr_index hlp;		/* lbr control structure *DLJ* */
  char *help_file;
  char *query;
  char *s;
  char *cp;
  char *browser;		/* User-agent field, DLJ */
  int status;

  /* Initialize CGI library. */
  status = cgi_init ( argc, argv );
  if ( (status&1) == 0 ) exit ( status );
  s = cgi_info ( "HTTP_USER_AGENT" );
  /*
   * Hack to accomodate MacMosaic bug.  Mosaic 1.0.3 ignores the first
   * LF it sees after going into <PRE> mode while other browsers need the
   * line break before going into <PRE>.  Check for the faulty browser and
   * do the right thing.
   */
  pre_start = "\n<PRE>";
  if ( s ) {
	browser = malloc ( strlen(s) + 1 );
	strcpy ( browser, s );
	for ( cp = browser; *cp; cp++ ) if (isspace(*cp) ) {
	    *cp = '\0'; break;
	}
	/* fprintf(stderr,"browser: '%s'\n", browser ); */
	if ( strcmp ( browser, "MacMosaicB6" ) == 0 ) pre_start = "<PRE>\n";
  }

  /* output the content type */
  cgi_printf("Content-Type: text/html\n\n<HTML>\n");

  /* address must be of the form /HELP[/@library][/topic[/subtopic...]] */
  if ((arg=cgi_info("PATH_INFO")) == NULL ||
      (s = strtok (arg + 1, "/")) == NULL) {
      cgi_printf("Address should begin with /HELP arg=%d\n</HTML>\n", arg);
      exit(0);
  }
  if (strcasecomp (s, "HELP")) {
      cgi_printf("Address should begin with /HELP : /%s\n</HTML>\n", s ? s : "");
      exit(0);
  }

  help_file = strtok (NULL, "/");
  if (help_file && *help_file == '@'
      && help_file[1] ) {  /* Explicit help library specified */
    help_file++;  /* Skip @ */
    query = strtok (NULL, "/+");
  } else {  /* This was not a help library, but the beginning of the query */
    query = help_file;
    help_file = NULL;
  }
  /* Unescape lead colon, if present in query */
  if (query && strncasecomp(query, "%3a", 3) == 0) {
      query += 2;
      *query = ':';
  }

  /* The first word from the query is the module name (perhaps empty) */
  if (! (hlp = open_hlp (help_file, query))) {
    cgi_printf("Help library or module not found : %s/%s\n",
	       help_file ? help_file : "", query ? query : "");
    cgi_printf("</HTML>");
    return 2160;
  }
  if (query) {  /* Parse rest of address and process help file */
    while (s = strtok (NULL, "/+"))
      *(s - 1) = ' ';
    status = hlp_to_html(hlp, query);
  }
  else  /* Process library directory */
    status = lis_to_html(hlp, help_file);

  cgi_printf("</HTML>");
  exit(0);
  return(status);
}


/*****************************************************************************

strcasecomp  : Case-insensitive comparison for strings of any length.
strncasecomp : Case-insensitive comparison for strings with count limit.

Return -1, 0, 1 equivalently to strcmp and strncmp

-----------------------------------------------------------------------------*/

/*	Strings of any length
**	---------------------
*/
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

/*	With count limit
**	----------------
*/
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


/*****************************************************************************

anchor_strcpy : Converts a string to a name suitable for use as an anchor
		and for comparisons, i.e. trim to its first word (consisting
		of alphanumeric characters plus '_', '=', '@', '-', '$', '/'
		and '.'), make the letters uppercase, and convert each '$'
		'/' and '.' to '_'.  If string begins with ':', escape it.

Inputs :
	char *dest : destination string
	char *src  : source string

Returns the end position of the destination string.

-----------------------------------------------------------------------------*/

char *anchor_strcpy(char *dest, char *src)
{
  char *end = dest;
  /* Trim to first word */
  for ( ; *end =
            isalnum (*src) || *src == '_' || *src == '=' || *src == '@' ||
	    *src == '-' ? _toupper(*src)
          : *src == '/' || *src == '$' || *src == '.' ? '_'
          : '\0' ;
       ++end, ++src);
  if (end == dest) {   /* src doesn't begin with a word or _=@-/$. */
    if (*src == ':') { /* If it begins with a colon, escape it     */
        *end++ = '%';
	*end++ = '3';
	*end++ = 'a';
	src++;
    }
    for ( ; *src ; ++src) {
	*end++ = _toupper (*src);
    }
    *end = 0;  /* Terminate the string */
  }
  return end;
}


/****************************************************************************

show_copyright : Outputs a copyright notice if the script has
		 set the symbol WWW_COPYRIGHT_STRING.

----------------------------------------------------------------------------*/

void show_copyright()
{
  char *cp;

  if ((cp=getenv("WWW_COPYRIGHT_STRING")) != NULL)
  	cgi_printf("%s\n", cp);

  return;
}


/*****************************************************************************

hlp_to_html : Reads in help library module and outputs desired info in HTML format.

Inputs :
	lbr_index hlp : pointer to the .HLP file to be scanned
	char *query : data to be found, as blank-separated keywords
	              NOTE : this string is altered (tokenized) by the function

Optional symbols set by script :
	WWW_HOST_ACRONYM : Acronym for host.  Prefixed to HTML title.

Status returned :
	 0 : Success
Unused	-1 : Help file not found
	-2 : Query not found
Unused	-3 : Empty query
	-4 : Structure error in help file (may create some text before that)
Unused	-5 : Write error to HTML file (may create some text before that)
Unused	-6 : Can't open HTML output file

-----------------------------------------------------------------------------*/

int hlp_to_html(lbr_index hlp, char *query)
{
  int subqual;	     /* True if level defines qualifiers (DLJ) */
  int dlen;	     /* 'Display' length, chars on current display line */
  char carcon[TAB_SIZE*2];
  char *sep = " ";   /* Authorized keyword separators in the query */
  char line[LSIZE];  /* Input buffer */
  char out[LSIZE];   /* Output buffer */
  /* Possible states of the scanning algorithm */
  enum { PARSE, TEXT, MENU, DONE } state;
  int depth;   /* Current depth in the help tree */
  char *key;   /* Currently searched keyword from the query */
  int prefix;  /* Prefix number on a line of the help file, indicating depth */
  char *title; /* Pointer to the rest of a title line after depth prefix */
  char anchor[ASIZE];  /* Anchor name built from menu title and item */
  char *menu_item;     /* Pointer to menu item within anchor name */
  char *cp, *s, *t;  /* Temporary string pointers */
  int i = 1, len;

  /* Initial things to send out */

  state = PARSE;  /* Searching for the query's keywords */
  depth = 1;
  subqual = 0;
  key = strtok (query, sep); /* Read first keyword from the query */
  anchor_strcpy (key, key);  /* Normalise key to help matches */

  while (1==(1&lbr_fgets (line, LSIZE, hlp)) ) {
    if (isdigit (*line)) {  /* This is a title line */
      prefix = strtol (line, &title, 10);
      while (isspace (*title))  /* Find the real beginning of the title */
	title++;
      s = title;
      while (*(++s) != '\n');  /* Find the real end of the title */
      while (isspace (*(--s)));
      *(++s) = '\0';  /* Trim title to a clean string */

      switch (state) {
      case PARSE:  /* Check title against query */
	menu_item = anchor_strcpy (anchor, title);
	/* Is this a match ? */
	if (prefix == depth && !strncmp(anchor, key, strlen(key))) {
	  if (depth == 1) {  /* Root match */
	    /* Start the HTML title with the first word of 'title' */
	    cgi_printf ("<TITLE>%s Help %.*s",
	    		    (cp=getenv("WWW_HOST_ACRONYM")) ? cp : "",
	    		    menu_item - anchor, title);
	  } else {  /* Deep match : continue HTML title in the same way */
	    cgi_printf(" %.*s", menu_item - anchor, title);
	  }
	  if (key = strtok (NULL, sep)) {  /* More levels to descend */
	    depth++;
	    anchor_strcpy (key, key); /* Normalise key to help matches */
	  }
	  else {  /* Query fully parsed */
	    cgi_printf("</TITLE>\n");  /* End the HTML title */
	    show_copyright();  /* show copyright notice if set by script */
	    cgi_printf("<H1>%s</H1>", title);  /* Print full heading */
	    *menu_item++ = '/';  /* Append field separator to anchor */
	    *menu_item = '\0';  /* Ready for anchor completion */
	    state = TEXT;
	    cgi_printf(pre_start);  /* Will copy help text verbatim */
	  }
	}
	break;

      case TEXT:
	cgi_printf("\n</PRE>\n");  /* Help text is finished, folks ! */
	if (prefix <= depth) {
	  /* We reached the next item at this depth or it was the last one */
	  /* That's all, folks ! */
	  state = DONE;
	  break;
	} else if (prefix == depth + 1) {  /* This item has a menu */
	  state = MENU;
	  depth++;
	  cgi_printf("<H3>Additional information available:</H3>%s",pre_start);
	  i = 1;
	  dlen = 0;
	} else {  /* The help file skipped the next depth */
	  lbr_close (hlp);
	  cgi_printf("Help file corrupted.\n");
	  return(-4);
	}
	/* No break here : flow through case MENU if state transition */
	/* (Yeah, you may find this ugly...) */

      case MENU:
	if (prefix < depth) {  /* Seen all menu items */
	  cgi_printf("\n</PRE>\n");
	  /* That's all, folks ! */
	  state = DONE;
	} else if (prefix == depth) {  /* Next menu item */
	  anchor_strcpy (menu_item, title);  /* Append item to anchor prefix */
	  len = strlen(title);		/* printable chars */

	  if ( 0 == dlen ) strcpy ( carcon, "" );
	  else {
	     int j = 0;
	     carcon[j++] = ' ';
	     for ( dlen++; dlen%TAB_SIZE; dlen++ ) carcon[j++] = ' ';
	     carcon[j] = '\0';
	     if ( subqual || ( (dlen + len) >= LINE_WIDTH ) ) {
		strcpy ( carcon, "\n" );
		dlen = 0; i = 1;
	     }
	  }
	  dlen += len;
	  subqual = 0;		/* new level started */
	  cgi_printf("%s<A HREF=\"%s\">%s</A\n>", carcon, anchor, title );

	} /* if prefix > depth, there's a submenu : ignore it. */
	break;
	
      case DONE:  /* Shouldn't happen here */
	fprintf (stderr, "Internal error : invalid state DONE.\n");
	break;

      }  /* End of switch(state) */
    }  /* End of title line processing */

    else { /* This is a text line */
      if (state == TEXT && *line != '!') { /* It's not a comment ('!') */
        if (*line == '/') {
	     /* Make qualifier header bold  and make it an anchor.  Anchor
	      * name is only the qualifier name itself. */
	     char tag[80]; int j;
	     strncpy ( tag, line+1, sizeof(tag)-2 ); tag[sizeof(tag)-1] = '\0';
	     for ( j = 0; tag[j]; j++ ) if ( isspace(tag[j]) ||
		( tag[j] == '\n' ) ) { tag[j] = '\0'; break; }
	     for ( j = 0; line[j] && (line[j] != '\n'); j++); line[j] = '\0';
	     cgi_printf("<b><A NAME=\"%s\">%s</A></b>", tag, line);
	} else {
	    /* Copy text line to HTML file, escaping '<', '>', and '&' */
    	   char *start, *curp, testc;
	   for ( start = curp = line; (testc=(*curp)); curp++ ) {
		if ( (testc == '<') || (testc == '>') || (testc == '&') ) {
		    *curp = '\0';
		    cgi_printf( (testc=='<') ? "%s&lt;" :
			((testc=='>') ? "%s&gt;" : "%s&amp;"), start );
		    start = curp+1;
		}
	    }
	    cgi_printf("%s", start);
	}
      }
	else if ( (state == MENU) && (*line == '/') && (prefix == depth) ) {
	    /*
	     * The help module has qualifier definitions that don't go to the
	     * next level.  Add these to the menu, starting them on a fresh
	     * line.  Reference will be using intra-module syntax.
	     */
	    char tag[80]; int j;
	     strncpy ( tag, line+1, sizeof(tag)-2 ); tag[sizeof(tag)-1] = '\0';
	     for ( j = 0; tag[j]; j++ ) if ( isspace(tag[j]) ||
		( tag[j] == '\n' ) ) { tag[j] = '\0'; break; }
	     for ( j = 0; line[j] && (line[j] != '\n'); j++); line[j] = '\0';

	    len = strlen ( line );
	    if ( 0 == dlen ) strcpy ( carcon, "" );
	    else {
		j = 0;
		carcon[j++] = ' ';
		for ( dlen++; dlen%TAB_SIZE; dlen++ ) carcon[j++] = ' ';
		carcon[j] = '\0';
		if ( (subqual==0) || ( dlen + len >= LINE_WIDTH ) ) {
		    strcpy ( carcon, "\n" );
		    dlen = 0;
		}
	    }
	    subqual = 1;
	    dlen += len;
	    cgi_printf ( "%s<A HREF=\"%s#%s\">%s</A\n>", 
			carcon, anchor, tag, line );
	}
    }  /* End of text line processing */

    if (state == DONE) {  /* Have we finished yet ? */
      lbr_close (hlp);
      cgi_printf("\n");
      return 0;  /* success */
    }

  }  /* EOF reached on help file */
  lbr_close (hlp);
  if (state != PARSE) {  /* EOF reached while outputting HTML */
    cgi_printf("\n</PRE>\n");
    return 0;  /* success */
  } else {  /* key not found */
    if (depth > 1)  /* Partial match : end the title cleanly */
      cgi_printf("</TITLE>\n");
    cgi_printf("\"%s\" not found in help file.\n", key ? key : "");
    return(-2);
  }
}


/*****************************************************************************

lis_to_html : Reads current library module index and outputs menu in HTML format.

Inputs :
	lbr_index dir : pointer to the library to be scanned
	char *lib_name : name of the help library (for HTML title)

Optional symbols set by script :
	WWW_HOST_ACRONYM : Acronym for host.  Prefixed to HTML title.

Status returned :
	 0 : Success
	-2 : Structure error in .LIS file

-----------------------------------------------------------------------------*/

int lis_to_html(lbr_index dir, char *lib_name)
{
  char line[LSIZE];  /* Input buffer */
  char spacing[24];	/* Holds output chars that move cursoro to next col */
  char *entry;  /* Pointer to current directory entry */
  char *cp, *at_prefix;
  int dlen;
  int i = 1, len, j, extra_cols, fill;
  
  if (lib_name)  /* Help library specified */
    at_prefix = "HELP/../@"; /* Use a relative path trick */
  else {  /* Plain HELP request */
    at_prefix = "";
    lib_name = "HELP";  /* Root level help */
  }
    
  /* Initial things to send out */
  cgi_printf("<TITLE>%s Help Library ",
  		 (cp=getenv("WWW_HOST_ACRONYM")) ? cp : "");
  cgi_printf("%s", lib_name);
  cgi_printf(" Contents</TITLE>\n");
  show_copyright();  /* show copyright notice if set by script */
  cgi_printf("<H1>%s</H1>%s", lib_name, pre_start);

  /* Now scan the entries */
  i = 1;			/* Current column, 1..5  */
  entry = line;
  extra_cols = 0;
  dlen = 0;
  fill = 0;
  while ( 1==(1&lbr_read_directory (entry, LSIZE, dir)) ) {
    len = strlen(entry);	/* length of entry */
    if ( 0 == dlen ) strcpy ( spacing, "" );
    else {
	j = 0;
	spacing[j++] = ' ';
	for ( dlen++; dlen%TAB_SIZE; dlen++ ) spacing[j++] = ' ';
	spacing[j] = '\0';
	if ( dlen + len >= LINE_WIDTH ) {
	    strcpy ( spacing, "\n" );
	    dlen = 0;
	}
    }
    dlen += len;
    cgi_printf ( "%s<A HREF=\"%s%s/%s\">%s</A\n>", 
		spacing, at_prefix, lib_name, entry, entry );
  }  /* End of entry processing */

  lbr_close (dir);
  cgi_printf("\n</PRE>\n");  /* End the pseudo-table cleanly */
  return 0;  /* success */
}


/*****************************************************************************

open_hlp : Opens desired module from specified library. Extracts it from
           .HLB help library if necessary. If no module is specified, opens
	   (and perhaps creates) .LIS library directory file.

Inputs :
	char *help_lib : library name
	char *module : name of the module to extract

Returns lbr_index pointing to control stucture.

----------------------------------------------------------------------------*/

lbr_index open_hlp(char *help_lib, char *module)
{
    lbr_index lptr;
    int status, searching;
    char *lib_name;
    char fallback[256];

  /* First, create the relevant (unique) file name */
  if ( help_lib ) {
      lib_name = help_lib;
      strcpy ( fallback, "HELPLIB" );
      if ( strcmp(help_lib,"HELPLIB") == 0 ) strcpy ( fallback, "HLP$LIBRARY");
  } else {
      /* Try helplib first */
      lib_name = "HELPLIB";
      strcpy ( fallback, "HLP$LIBRARY" );
  }

  /*
   * Search specified library plus default libraries.
   */
  for ( searching = 1; searching; ) {
        status = lbr_open ( lib_name, "SYS$HELP:.HLB", &lptr );
	if ( 1 == (status&1) ) {
	    /* See if module in library */
	    if ( module ) {
		status = lbr_set_module ( lptr, module );
		if ( 1 == (status&1) ) searching = 0;	/* Found a match */
		else lbr_close ( lptr );		/* not this library */
	    } 
	    else searching = 0;		/* Assume directory scan */
	    
	}
        if ( searching ) {
	    /*
	     * Library open failed, set up libname for next try.
	     */
	    if ( strcmp(fallback,"HELPLIB") == 0 ) {
		strcpy ( fallback, "HLP$LIBRARY" );
		lib_name = "HELPLIB";
	    } else {
		lib_name = getenv(fallback);
		if ( !lib_name ) break;		/* no more fallbacks */
		sprintf(fallback,"HLP$LIBRARY_%d", searching++ );
	    }
	}
    }
    /*
     * Breaking out of loop (searching non-zero) means module not found.
     */
   return searching ? (lbr_index) 0 : lptr;		/* DLJ */
}
