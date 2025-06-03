/****************************************************************************/
/*									    */
/*  FACILITY:	Routine Analyzer					    */
/*									    */
/*  MODULE:	Report Generation Header				    */
/*									    */
/*  AUTHOR:	Steve Branam, Network Product Support Group, Digital	    */
/*		Equipment Corporation, Littleton, MA, USA.		    */
/*									    */
/*  DESCRIPTION: This header file contains report macros.		    */
/*									    */
/*  REVISION HISTORY:							    */
/*									    */
/*  V0.1-00 24-AUG-1994 Steve Branam					    */
/*									    */
/*	Original version.						    */
/*									    */
/****************************************************************************/

#define isfirst_defining_file(f) (prev_defining_file(f) == NULL)
#define islast_defining_file(f) (next_defining_file(f) == NULL)
#define isfirst_tree(f) (prev_tree(f) == NULL)
#define islast_tree(f) (next_tree(f) == NULL)
#define isfirst_defined(f) (prev_defined(f) == NULL)
#define islast_defined(f) (next_defined(f) == NULL)
#define isfirst_undefined(f) (prev_undefined(f) == NULL)
#define islast_undefined(f) (next_undefined(f) == NULL)

typedef enum {
    MAIN_SECTION,
    SOURCE_FILES_SECTION,
    ROUTINES_BY_FILE_SECTION,
    DEFINED_ROUTINES_SECTION,
    UNDEFINED_ROUTINES_SECTION,
    CALLS_CALLERS_SECTION,
    CALLER_XREF_SECTION,
    CALL_TREES_SECTION,
    SOURCE_CODE_SECTION
} HTML_SECTION_TYPE;

typedef struct {
    char    *name;			    /* Section name.		    */
    char    *anchor;			    /* Section anchor name.	    */
    char    *suffix;			    /* Section file suffix.	    */
    char    *moresuffix;		    /* Section file continuation    */
					    /* suffix.			    */
    char    *nextsuffix;		    /* Next section file suffix.    */
    char    *prevsuffix;		    /* Previous section file	    */
					    /* suffix.			    */
    char    *objstr;			    /* Section object name string.  */
    char    *subobjstr;			    /* Subsection object name	    */
					    /* string.			    */
    char    *subanchor;			    /* Subsection anchor prefix.    */
} HTML_SECTION;

#define section_name(t, x) (t)[x].name
#define section_anchor(t, x) (t)[x].anchor
#define section_suffix(t, x) (t)[x].suffix
#define section_moresuffix(t, x) (t)[x].moresuffix
#define section_nextsuffix(t, x) (t)[x].nextsuffix
#define section_prevsuffix(t, x) (t)[x].prevsuffix
#define section_objstr(t, x) (t)[x].objstr
#define section_subobjstr(t, x) (t)[x].subobjstr
#define section_subanchor(t, x) (t)[x].subanchor
#define ismain_section(s) (s == MAIN_SECTION)
#define issource_section(s) (s == SOURCE_CODE_SECTION)

/*									    */
/* Forward declarations.						    */
/*									    */

DEFINITION *next_tree();
DEFINITION *prev_tree();
SOURCEFILE *next_defining_file();
SOURCEFILE *prev_defining_file();
