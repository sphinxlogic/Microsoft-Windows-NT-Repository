/****************************************************************************/
/*									    */
/*  FACILITY:	Routine Analyzer					    */
/*									    */
/*  MODULE:	Main Header						    */
/*									    */
/*  AUTHOR:	Steve Branam, Network Product Support Group, Digital	    */
/*		Equipment Corporation, Littleton, MA, USA.		    */
/*									    */
/*  DESCRIPTION: This header file contains the general definitions and	    */
/*  nested header includes used by Routine Analyzer modules.		    */
/*									    */
/*  REVISION HISTORY:							    */
/*									    */
/*  V1.0-00 27-JAN-1995 Steve Branam					    */
/*									    */
/*	Modified brief help string and version ident.			    */
/*									    */
/*  V0.1-00 24-AUG-1994 Steve Branam					    */
/*									    */
/*	Original version.						    */
/*									    */
/****************************************************************************/

#include "globdb.h"			    /* Global database class.	    */

/*									    */
/* Miscellaneous constants.						    */
/*									    */

#define PROGRAM_IDENT	    "Routine Analyzer V1.0-00"
#define PROGRAM_COPYRIGHT   "Copyright (C) 1994 Digital Equipment Corporation"
#define PROGRAM_AUTHOR      "Written by Steve Branam, Network Product Support Group,\nDigital Equipment Corporation, Littleton, MA, USA\n"
#define PROGRAM_PARAMS	    "PARAMETERS: product_file [options]"
#define PROGRAM_HELP	    "        OR: %chelp for full help\n"
#define TAB_SIZE	    8		    /* Number of chars in TAB.	    */
#define MAX_PRODUCT_NAME    64		    /* Max length of product name.  */
#define MAX_ROUTINE_NAME    64		    /* Max length of routine name.  */
					    /* Max routine fullname.	    */
#define MAX_ROUTINE_IDENT   MAX_FILE_NAME + MAX_ROUTINE_NAME + 3    
#define MIN_TREE_SIZE	    5		    /* Minimum number of nodes to   */
					    /* generate separate call tree  */
					    /* for routine with callers.    */
#define MAX_TREE_DEPTH	    20		    /* Max call tree depth.	    */
#define DEF_MAX_CALLERS	    3		    /* Default value of max callers */
					    /* to allow subtree expansion.  */
#define MIN_MAX_CALLERS	    1		    /* Min value of max callers.    */
#define MAX_MAX_CALLERS	    10		    /* Max value of max callers.    */
#define DEF_MAX_HTML_BYFILE 200		    /* Default value of max tree    */
					    /* entries for HTML output.	    */
#define DEF_MAX_HTML_XREF   200		    /* Default value of max xref    */
					    /* entries for HTML output.	    */
#define FILE_EXT_SEPARATOR  '.'		    /* File name/extension sep.	    */

/*									    */
/* Report/listing format strings.					    */
/*									    */

#define LIST_LINE_NUMBER    "%8d: "
#define LIST_LINE_SPACER    "%8c  "

/*									    */
/* Report/listing output file name suffixes, to be appended to the output   */
/* file name prefix.							    */
/*									    */

#define OUTFILE_SUFFIX_DEFLIST	    "defined"
#define OUTFILE_SUFFIX_UNDEFLIST    "undefind"
#define OUTFILE_SUFFIX_CALLS	    "calls"
#define OUTFILE_SUFFIX_XREF	    "xref"
#define OUTFILE_SUFFIX_MOREXREF     "xrf%05d"
#define OUTFILE_SUFFIX_CALLTREES    "calltree"
#define OUTFILE_SUFFIX_MORETREES    "cat%05d"
#define OUTFILE_SUFFIX_BYFILE	    "byfile"
#define OUTFILE_SUFFIX_MOREBYFILE   "byf%05d"
#define OUTFILE_SUFFIX_FILES	    "files"
#define OUTFILE_SUFFIX_SOURCE	    "src%05d"
#define OUTFILE_SUFFIX_FIRSTSOURCE  "src00001"
#define OUTFILE_SUFFIX_LIST	    "analysis"

/*									    */
/* Report/listing output file extensions.				    */
/*									    */

#define OUTFILE_EXT_LIST	    ".lis"
#define OUTFILE_EXT_TEXT	    ".txt"
#ifdef MSDOS 
#define OUTFILE_EXT_SDML	    ".sdm"
#else
#define OUTFILE_EXT_SDML	    ".sdml"
#endif
#ifdef MSDOS 
#define OUTFILE_EXT_HTML	    ".htm"
#else
#define OUTFILE_EXT_HTML	    ".html"
#endif
#define OUTFILE_EXT_RTF		    ".doc"
#define OUTFILE_EXT_WINHELP	    ".hlp"
#define OUTFILE_EXT_VMSHELP	    ".hlp"

/*									    */
/* Report field sizes.							    */
/*									    */

#define RPT_HTML_FILE_FIELD_LEN	    23
#define RPT_HTML_ROUTINE_FIELD_LEN  23
#define RPT_HTML_ROUTINE_WIDE_LEN   48
#define RPT_HTML_ROUTINE_MEDIUM_LEN 40

/*									    */
/* HTML section subtitles.						    */
/*									    */

#define HTML_SECTION_FILES	"Source Files"
#define HTML_SECTION_DEFINED	"Defined Routines Alphabetical"
#define HTML_SECTION_UNDEFINED	"Undefined Routines Alphabetical"
#define HTML_SECTION_CALLS	"Defined Routines Calls/Callers"
#define HTML_SECTION_TREES	"Routine Call Trees (Selected Routines)"
#define HTML_SECTION_XREF	"Caller Cross Reference"
#define HTML_SECTION_BYFILE	"Defined Routines By File"
#define HTML_SECTION_SOURCE	"Source Code"


/*									    */
/* General-purpose macros.						    */
/*									    */

#define make_filename(pfx,fn,ext,buf) strcat(strcat(strcpy(buf, pfx), fn), ext)

/*									    */
/* Generic parser-related types.					    */
/*									    */

typedef	enum {
    PARSE_ERROR,
    END_OF_SOURCE,
    ROUTINE_DEF_BEGIN,
    ROUTINE_DEF_END,
    ROUTINE_REF
} language_element;

typedef language_element (*PARSER)();

