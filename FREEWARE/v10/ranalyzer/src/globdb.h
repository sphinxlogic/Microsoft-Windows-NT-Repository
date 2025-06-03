/****************************************************************************/
/*									    */
/*  FACILITY:	Routine Analyzer					    */
/*									    */
/*  MODULE:	Global Database Header					    */
/*									    */
/*  AUTHOR:	Steve Branam, Network Product Support Group, Digital	    */
/*		Equipment Corporation, Littleton, MA, USA.		    */
/*									    */
/*  DESCRIPTION: This header file contains the definitions for the global   */
/*  database used by Routine Analyzer. The global database contains all	    */
/*  program-permanant global data. Member access routines (get/set values)  */
/*  and a number of general management routines are implemented here as	    */
/*  macros.								    */
/*									    */
/*  REVISION HISTORY:							    */
/*									    */
/*  V0.1-00 24-AUG-1994 Steve Branam					    */
/*									    */
/*	Original version.						    */
/*									    */
/****************************************************************************/

#ifndef __GLOBDB_H			    /* If not already included...   */
#define __GLOBDB_H
#include <stdio.h>
#include "list.h"			    /* Generic list class.	    */
#include "cmdopt.h"
#include "objects.h"

/*									    */
/* Report format types.							    */
/*									    */

typedef enum {
    FORMAT_TEXT,
    FORMAT_SDML,
    FORMAT_HTML,
    FORMAT_RTF,
    FORMAT_WINHELP,
    FORMAT_VMSHELP
} report_output_format;

/*									    */
/* Object type.								    */
/*									    */

typedef struct {
    long    nobjects;			    /* Current number of objects.   */
    long    nobjbytes;			    /* Current number of bytes.	    */
    long    nstrings;			    /* Current number of strings.   */
    long    nlang;			    /* Current number of language   */
					    /* translations.		    */
    long    nfiles;			    /* Current number of files.	    */
    long    ndefs;			    /* Current number of defs.	    */
    long    nrefs;			    /* Current number of refs.	    */
    char    *productname;		    /* Product name string.	    */
    char    *outprefix;			    /* Output file name prefix.	    */
    char    *productdesc;		    /* Description file name.	    */
    char    *urlprefix;			    /* HTML URL prefix.		    */
    FILE    *listfile;			    /* Listing file ptr.	    */
    char    **separatelist;		    /* List of separate routines.   */
    int	    listcolumn;			    /* List output column.	    */
    long    flags;			    /* Program enable flags.	    */
    report_output_format		    /* Report format.		    */
	    rptformat;
    int	    maxcallers;			    /* Maximum number of callers    */
					    /* allowed for expanded call    */
					    /* subtree.			    */
    int	    maxhtmlbyfile;		    /* Max lines in HTML by-file.   */
    int	    maxhtmlxref;		    /* Max lines in HTML XREF file. */
    int	    maxtreedepth;		    /* Max tree expansion depth.    */
    long    lines;			    /* Total number of lines.	    */
    long    comments;			    /* Number comment-only lines.   */
    long    statements;			    /* Number statement-only lines. */
    long    mixed;			    /* Number mixed comment/	    */
					    /* statement lines.		    */
    long    routines;			    /* Number routines defined in   */
					    /* source files.		    */
    long    rlength;			    /* Length of all routine	    */
					    /* defintion code.		    */
    long    calls;			    /* Number of routine calls made */
					    /* in all files.		    */
    LIST    langlist;			    /* List of lang translations.   */
    LIST    filelist;			    /* List of source file records. */
    LIST    deflist;			    /* List of routine defintions.  */
    KEYWORD_DEFINITION			    /* Language translation table.  */
	    *langtable;
} GLOBAL_DATABASE;

/*									    */
/* Miscellaneous constants for the object.				    */
/*									    */

#define MAX_FILE_NAME	    256		    /* Max length of file name.	    */

/*									    */
/* Global flags.							    */
/*									    */

#define TRACE_MEM_ENABLE	(1 << 0)    /* Trace enables.		    */
#define TRACE_STR_ENABLE	(1 << 1)
#define TRACE_OBJ_ENABLE	(1 << 2)
#define TRACE_RPT_ENABLE	(1 << 3)
#define TRACE_PLEVEL_ENABLE	(1 << 4)
#define TRACE_BLEVEL_ENABLE	(1 << 5)
#define TRACE_PARSER_ENABLE	(1 << 6)

#define LIST_ENABLE		(1 << 10)    /*	Program options.	    */
#define LOG_SILENT_ENABLE	(1 << 11)
#define LOG_BRIEF_ENABLE	(1 << 12)
#define LOG_DEF_ENABLE		(1 << 13)
#define LOG_REF_ENABLE		(1 << 14)
#define RPT_TEXT_ENABLE		(1 << 15)
#define TREE_INLINE_DISABLE	(1 << 16)
#define RPT_DEFINED_DISABLE	(1 << 17)
#define RPT_UNDEFINED_DISABLE	(1 << 18)
#define RPT_CALLS_DISABLE	(1 << 19)
#define RPT_TREES_DISABLE	(1 << 20)
#define RPT_XREFS_DISABLE	(1 << 21)
#define RPT_BYFILE_DISABLE	(1 << 22)
#define RPT_FILES_DISABLE	(1 << 23)
#define RPT_SOURCE_DISABLE	(1 << 24)

/*									    */
/* Member access routines.						    */
/*									    */

#define gDb gGlobalDatabase		    /* Abbreviate name for macros.  */

#define num_objects()	(gDb.nobjects)
#define num_objbytes()	(gDb.nobjbytes)
#define num_strings()	(gDb.nstrings)
#define num_lang()	(gDb.nlang)
#define num_files()	(gDb.nfiles)
#define num_defs()	(gDb.ndefs)
#define num_refs()	(gDb.nrefs)
#define	product_name() (gDb.productname)
#define	product_description() (gDb.productdesc)
#define	outfile_prefix() (gDb.outprefix ? gDb.outprefix : "")
#define	url_prefix() (gDb.urlprefix)
#define	list_file() (gDb.listfile)
#define separate_list() (gDb.separatelist)
#define	list_column() (gDb.listcolumn)
#define	report_format() (gDb.rptformat)
#define max_callers() (gDb.maxcallers)
#define max_html_byfile() (gDb.maxhtmlbyfile)
#define max_html_xref() (gDb.maxhtmlxref)
#define max_tree_depth() (gDb.maxtreedepth)
#define total_files() (list_entries(&gDb.filelist))
#define total_lines() (gDb.lines)
#define total_comments() (gDb.comments)
#define total_statements() (gDb.statements)
#define total_mixed() (gDb.mixed)
#define total_empty() (gDb.lines - gDb.mixed - gDb.comments - gDb.statements)
#define total_routines() (gDb.routines)
#define total_rlength() (gDb.rlength)
#define total_avglen() (total_routines()==0?0:((total_rlength() * 10)/total_routines() + 5)/ 10)
#define total_calls() (gDb.calls)
#define global_langtable() (gDb.langtable)
#define global_langlist() (&gDb.langlist)
#define global_filelist() (&gDb.filelist)
#define global_deflist() (&gDb.deflist)
#define inc_nobjects()	(gDb.nobjects++)
#define dec_nobjects()	(gDb.nobjects--)
#define inc_nobjbytes(x)	(gDb.nobjbytes += x)
#define dec_nobjbytes(x)	(gDb.nobjbytes -= x)
#define inc_nstrings()	(gDb.nstrings++)
#define dec_nstrings()	(gDb.nstrings--)
#define inc_nlang()	(gDb.nlang++)
#define dec_nlang()	(gDb.nlang--)
#define inc_nfiles()	(gDb.nfiles++)
#define dec_nfiles()	(gDb.nfiles--)
#define inc_ndefs()	(gDb.ndefs++)
#define dec_ndefs()	(gDb.ndefs--)
#define inc_nrefs()	(gDb.nrefs++)
#define dec_nrefs()	(gDb.nrefs--)
#define	set_lang_table(n) (gDb.langtable=obj_alloc(sizeof(KEYWORD_DEFINITION)*(n+1), sizeof(KEYWORD_DEFINITION)*(n+1),"Language translation table"));
#define	set_product_name(s) (gDb.productname = new_str(s, MAX_PRODUCT_NAME,"Product name"))
#define	set_product_description(s) (gDb.productdesc = new_str(s, MAX_FILE_NAME, "Description file"))
#define	set_url_prefix(s) (gDb.urlprefix = new_str(s, MAX_FILE_NAME, "URL prefix"))
#define	set_outfile_prefix(s) (gDb.outprefix = new_str(s, MAX_FILE_NAME, "Output file prefix"))
#define set_separate_list(l) (gDb.separatelist = l)
#define	inc_list_column() (gDb.listcolumn++)
#define	set_list_column(x) (gDb.listcolumn = x)
#define	set_report_format(x) (gDb.rptformat = x)
#define set_max_callers(x) (gDb.maxcallers = x)
#define set_max_html_byfile(x) (gDb.maxhtmlbyfile = x)
#define set_max_html_xref(x) (gDb.maxhtmlxref = x)
#define set_max_tree_depth(x) (gDb.maxtreedepth = x)
#define inc_total_comments(x) (gDb.lines += x, gDb.comments += x)
#define inc_total_statements(x) (gDb.lines += x, gDb.statements += x)
#define inc_total_mixed(x) (gDb.lines += x, gDb.mixed += x)
#define inc_total_empty(x) (gDb.lines += x)
#define inc_total_routines(x) (gDb.routines += x)
#define inc_total_rlength(x) (gDb.rlength += x)
#define inc_total_calls(x) (gDb.calls += x)

/*									    */
/* Program trace flag access routines.					    */
/*									    */

#define trace_mem_enabled() (gDb.flags & TRACE_MEM_ENABLE)
#define trace_str_enabled() (gDb.flags & TRACE_STR_ENABLE)
#define trace_obj_enabled() (gDb.flags & TRACE_OBJ_ENABLE)
#define trace_rpt_enabled() (gDb.flags & TRACE_RPT_ENABLE)
#define trace_plevel_enabled() (gDb.flags & TRACE_PLEVEL_ENABLE)
#define trace_blevel_enabled() (gDb.flags & TRACE_BLEVEL_ENABLE)
#define trace_parser_enabled() (gDb.flags & TRACE_PARSER_ENABLE)

/*									    */
/* Program option flag access routines.					    */
/*									    */

#define	set_option(x) (gDb.flags |= (x))
#define	clear_option(x) (gDb.flags &= ~(x))
#define	list_enabled() (gDb.flags & LIST_ENABLE)
#define	log_silent_enabled() (gDb.flags & LOG_SILENT_ENABLE)
#define	log_brief_enabled() (gDb.flags & LOG_BRIEF_ENABLE && !log_silent_enabled())
#define	log_def_enabled() (gDb.flags & LOG_DEF_ENABLE && !log_silent_enabled())
#define	log_ref_enabled() (gDb.flags & LOG_REF_ENABLE && !log_silent_enabled())
#define	log_enabled() (!log_silent_enabled())
#define	log_full_enabled() (!log_silent_enabled() && !log_brief_enabled())
#define	rpt_text_enabled() (gDb.flags & RPT_TEXT_ENABLE)
#define	rpt_sdml_enabled() (gDb.rptformat == FORMAT_SDML)
#define	rpt_html_enabled() (gDb.rptformat == FORMAT_HTML)
#define	rpt_rtf_enabled() (gDb.rptformat == FORMAT_RTF)
#define	rpt_winhelp_enabled() (gDb.rptformat == FORMAT_WINHELP)
#define	rpt_vmshelp_enabled() (gDb.rptformat == FORMAT_VMSHELP)
#define	tree_inline_disabled() (gDb.flags & TREE_INLINE_DISABLE)
#define	rpt_defined_enabled() !(gDb.flags & RPT_DEFINED_DISABLE)
#define	rpt_undefined_enabled() !(gDb.flags & RPT_UNDEFINED_DISABLE)
#define	rpt_calls_enabled() !(gDb.flags & RPT_CALLS_DISABLE)
#define	rpt_trees_enabled() !(gDb.flags & RPT_TREES_DISABLE)
#define	rpt_xrefs_enabled() !(gDb.flags & RPT_XREFS_DISABLE)
#define	rpt_files_enabled() !(gDb.flags & RPT_FILES_DISABLE)
#define	rpt_byfile_enabled() !(gDb.flags & RPT_BYFILE_DISABLE)
#define	rpt_source_enabled() !(gDb.flags & RPT_SOURCE_DISABLE)


/*									    */
/* General management routines.						    */
/*									    */

#define	open_list_file(n) (gDb.listfile = fopen(n, "w"))
#define add_file(f) (insert_ordered_entry(&gDb.filelist, f, compare_file))
#define add_def(d) (insert_ordered_entry(&gDb.deflist, d, compare_def))

/*									    */
/* Global database allocation/declaration. The database resides in the main */
/* module, and is external to all other modules.			    */
/*									    */

#ifdef MAIN_MODULE
GLOBAL_DATABASE				    /* Allocate global database.    */
	gGlobalDatabase;
#else
extern GLOBAL_DATABASE			    /* Declare global database.	    */
	gGlobalDatabase;
#endif

#endif					    /* If not already included.	    */
