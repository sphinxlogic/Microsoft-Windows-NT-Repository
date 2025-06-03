/****************************************************************************/
/*									    */
/*  FACILITY:	Generic Support Library					    */
/*									    */
/*  MODULE:	Command Line Keyword and Option Processing Header	    */
/*									    */
/*  AUTHOR:	Steve Branam, Network Product Support Group, Digital	    */
/*		Equipment Corporation, Littleton, MA, USA.		    */
/*									    */
/*  DESCRIPTION: This header contains definitions used in the processing of */
/*  program command line arguments, given a dispatch table of keywords and  */
/*  handlers.								    */
/*									    */
/*  REVISION HISTORY:							    */
/*									    */
/*  V0.1-00 24-AUG-1994 Steve Branam					    */
/*									    */
/*	Original version.						    */
/*									    */
/*  V0.2-00 23-SEP-1994 Steve Branam					    */
/*									    */
/*	Addded keyword translation code.				    */
/*									    */
/****************************************************************************/

#ifndef __CMDOPT_H
#define __CMDOPT_H

#ifdef VAXC
#define CMDLINE_OPTION_SWITCH	    '/'	    /* VMS option switch char.	    */
#else
#define CMDLINE_OPTION_SWITCH	    '-'	    /* Non-VMS option switch char.  */
#endif
#define CMDLINE_OPTION_SEPARATOR    '='	    /* Separates option from value. */
#define CMDLINE_OPTION_COMMENT	    '!'	    /* Rest of option is comment.   */
#define CMDLINE_OPTVAL_SEPARATOR    ','	    /* Separates values in list.    */
#define CMDLINE_HELP_SWITCH	    '#'	    /* Switch char to show help.    */
#define KEYWORD_LIST_SEPARATOR      ','	    /* Separates values in list.    */

#define max(x, y) (x > y ? x : y)

/*									    */
/* Keyword definition object. Keyword dispatch tables are composed of these */
/* objects.								    */
/*									    */

typedef struct {
    char    *keyword;			    /* Keyword string.		    */
    int	    minlen;			    /* Minimum required command	    */
					    /* line argument length.	    */
    int	    (*handler)();		    /* Keyword handler ptr.	    */
    int	    code;			    /* Translation code.	    */
} KEYWORD_DEFINITION;

/*									    */
/* Keyword definition object member access routines.			    */
/*									    */

#define kwdef_keyword(d) ((d)->keyword)
#define kwdef_minlen(d) ((d)->minlen)
#define kwdef_handler(d) ((d)->handler)
#define kwdef_code(d) ((d)->code)
#define set_kwdef_keyword(d, s) ((d)->keyword = s)
#define set_kwdef_minlen(d, x) ((d)->minlen = x)
#define set_kwdef_handler(d, h) ((d)->handler = h)
#define set_kwdef_code(d, x) ((d)->code = x)

/*									    */
/* Forward declarations.						    */
/*									    */

int ustrncmp();
int process_options();
int process_keyword();
int process_options_file();
int translate_keyword();

#endif /* #ifndef __CMDOPT_H */
