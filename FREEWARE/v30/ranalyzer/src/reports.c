/****************************************************************************/
/*									    */
/*  FACILITY:	Routine Analyzer					    */
/*									    */
/*  MODULE:	Report Generation					    */
/*									    */
/*  AUTHOR:	Steve Branam, Network Product Support Group, Digital	    */
/*		Equipment Corporation, Littleton, MA, USA.		    */
/*									    */
/*  DESCRIPTION: This module contains the routines for generating Routine   */
/*  Analyzer reports, for all output formats.				    */
/*									    */
/*  REVISION HISTORY:							    */
/*									    */
/*  V0.1-00 24-AUG-1994 Steve Branam					    */
/*									    */
/*	Original version.						    */
/*									    */
/****************************************************************************/

#include <stdio.h>
#include <ctype.h>
#include "ranalyzer.h"
#include "reports.h"

extern GLOBAL_DATABASE gGlobalDatabase;

int tree_greater_than(){return 1;} /* dummy */


/* dummy routines for currently unimplemented report formats		    */

void rpt_defined_rtf_hdr(){}
void rpt_defined_rtf_entry(){}
void rpt_defined_rtf_end(){}
void rpt_defined_winhelp_hdr(){}
void rpt_defined_winhelp_entry(){}
void rpt_defined_winhelp_end(){}
void rpt_defined_vmshelp_hdr(){}
void rpt_defined_vmshelp_entry(){}
void rpt_defined_vmshelp_end(){}

void rpt_undefined_rtf_hdr(){}
void rpt_undefined_rtf_entry(){}
void rpt_undefined_rtf_end(){}
void rpt_undefined_winhelp_hdr(){}
void rpt_undefined_winhelp_entry(){}
void rpt_undefined_winhelp_end(){}
void rpt_undefined_vmshelp_hdr(){}
void rpt_undefined_vmshelp_entry(){}
void rpt_undefined_vmshelp_end(){}

void rpt_calls_rtf_hdr(){}
void rpt_calls_rtf_entry(){}
void rpt_calls_rtf_end(){}
void rpt_calls_winhelp_hdr(){}
void rpt_calls_winhelp_entry(){}
void rpt_calls_winhelp_end(){}
void rpt_calls_vmshelp_hdr(){}
void rpt_calls_vmshelp_entry(){}
void rpt_calls_vmshelp_end(){}

void rpt_xref_rtf_hdr(){}
void rpt_xref_rtf_entry(){}
void rpt_xref_rtf_end(){}
void rpt_xref_winhelp_hdr(){}
void rpt_xref_winhelp_entry(){}
void rpt_xref_winhelp_end(){}
void rpt_xref_vmshelp_hdr(){}
void rpt_xref_vmshelp_entry(){}
void rpt_xref_vmshelp_end(){}

void rpt_tree_rtf_hdr(){}
void rpt_tree_rtf_entry(){}
void rpt_tree_rtf_end(){}
void rpt_tree_winhelp_hdr(){}
void rpt_tree_winhelp_entry(){}
void rpt_tree_winhelp_end(){}
void rpt_tree_vmshelp_hdr(){}
void rpt_tree_vmshelp_entry(){}
void rpt_tree_vmshelp_end(){}

void rpt_byfile_text_hdr(){}
void rpt_byfile_text_entry(){}
void rpt_byfile_text_end(){}
void rpt_byfile_rtf_hdr(){}
void rpt_byfile_rtf_entry(){}
void rpt_byfile_rtf_end(){}
void rpt_byfile_winhelp_hdr(){}
void rpt_byfile_winhelp_entry(){}
void rpt_byfile_winhelp_end(){}
void rpt_byfile_vmshelp_hdr(){}
void rpt_byfile_vmshelp_entry(){}
void rpt_byfile_vmshelp_end(){}

void rpt_file_text_hdr(){}
void rpt_file_text_entry(){}
void rpt_file_text_end(){}
void rpt_file_rtf_hdr(){}
void rpt_file_rtf_entry(){}
void rpt_file_rtf_end(){}
void rpt_file_winhelp_hdr(){}
void rpt_file_winhelp_entry(){}
void rpt_file_winhelp_end(){}
void rpt_file_vmshelp_hdr(){}
void rpt_file_vmshelp_entry(){}
void rpt_file_vmshelp_end(){}

void rpt_source_text_hdr(){}
void rpt_source_text_entry(){}
void rpt_source_text_end(){}
void rpt_source_rtf_hdr(){}
void rpt_source_rtf_entry(){}
void rpt_source_rtf_end(){}
void rpt_source_winhelp_hdr(){}
void rpt_source_winhelp_entry(){}
void rpt_source_winhelp_end(){}
void rpt_source_vmshelp_hdr(){}
void rpt_source_vmshelp_entry(){}
void rpt_source_vmshelp_end(){}

/*									    */
/* Formatter routines forward declarations.				    */
/*									    */

void rpt_defined_text_hdr();
void rpt_defined_text_entry();
void rpt_defined_text_end();
void rpt_defined_sdml_hdr();
void rpt_defined_sdml_entry();
void rpt_defined_sdml_end();
void rpt_defined_html_hdr();
void rpt_defined_html_entry();
void rpt_defined_html_end();
void rpt_defined_rtf_hdr();
void rpt_defined_rtf_entry();
void rpt_defined_rtf_end();
void rpt_defined_winhelp_hdr();
void rpt_defined_winhelp_entry();
void rpt_defined_winhelp_end();
void rpt_defined_vmshelp_hdr();
void rpt_defined_vmshelp_entry();
void rpt_defined_vmshelp_end();

void rpt_undefined_text_hdr();
void rpt_undefined_text_entry();
void rpt_undefined_text_end();
void rpt_undefined_sdml_hdr();
void rpt_undefined_sdml_entry();
void rpt_undefined_sdml_end();
void rpt_undefined_html_hdr();
void rpt_undefined_html_entry();
void rpt_undefined_html_end();
void rpt_undefined_rtf_hdr();
void rpt_undefined_rtf_entry();
void rpt_undefined_rtf_end();
void rpt_undefined_winhelp_hdr();
void rpt_undefined_winhelp_entry();
void rpt_undefined_winhelp_end();
void rpt_undefined_vmshelp_hdr();
void rpt_undefined_vmshelp_entry();
void rpt_undefined_vmshelp_end();

void rpt_calls_text_hdr();
void rpt_calls_text_entry();
void rpt_calls_text_end();
void rpt_calls_sdml_hdr();
void rpt_calls_sdml_entry();
void rpt_calls_sdml_end();
void rpt_calls_html_hdr();
void rpt_calls_html_entry();
void rpt_calls_html_end();
void rpt_calls_rtf_hdr();
void rpt_calls_rtf_entry();
void rpt_calls_rtf_end();
void rpt_calls_winhelp_hdr();
void rpt_calls_winhelp_entry();
void rpt_calls_winhelp_end();
void rpt_calls_vmshelp_hdr();
void rpt_calls_vmshelp_entry();
void rpt_calls_vmshelp_end();

void rpt_xref_text_hdr();
void rpt_xref_text_entry();
void rpt_xref_text_end();
void rpt_xref_sdml_hdr();
void rpt_xref_sdml_entry();
void rpt_xref_sdml_end();
void rpt_xref_html_hdr();
void rpt_xref_html_entry();
void rpt_xref_html_end();
void rpt_xref_rtf_hdr();
void rpt_xref_rtf_entry();
void rpt_xref_rtf_end();
void rpt_xref_winhelp_hdr();
void rpt_xref_winhelp_entry();
void rpt_xref_winhelp_end();
void rpt_xref_vmshelp_hdr();
void rpt_xref_vmshelp_entry();
void rpt_xref_vmshelp_end();

void rpt_tree_text_hdr();
void rpt_tree_text_entry();
void rpt_tree_text_end();
void rpt_tree_sdml_hdr();
void rpt_tree_sdml_entry();
void rpt_tree_sdml_end();
void rpt_tree_html_hdr();
void rpt_tree_html_entry();
void rpt_tree_html_end();
void rpt_tree_rtf_hdr();
void rpt_tree_rtf_entry();
void rpt_tree_rtf_end();
void rpt_tree_winhelp_hdr();
void rpt_tree_winhelp_entry();
void rpt_tree_winhelp_end();
void rpt_tree_vmshelp_hdr();
void rpt_tree_vmshelp_entry();
void rpt_tree_vmshelp_end();

void rpt_byfile_text_hdr();
void rpt_byfile_text_entry();
void rpt_byfile_text_end();
void rpt_byfile_sdml_hdr();
void rpt_byfile_sdml_entry();
void rpt_byfile_sdml_end();
void rpt_byfile_html_hdr();
void rpt_byfile_html_entry();
void rpt_byfile_html_end();
void rpt_byfile_rtf_hdr();
void rpt_byfile_rtf_entry();
void rpt_byfile_rtf_end();
void rpt_byfile_winhelp_hdr();
void rpt_byfile_winhelp_entry();
void rpt_byfile_winhelp_end();
void rpt_byfile_vmshelp_hdr();
void rpt_byfile_vmshelp_entry();
void rpt_byfile_vmshelp_end();

void rpt_file_text_hdr();
void rpt_file_text_entry();
void rpt_file_text_end();
void rpt_file_sdml_hdr();
void rpt_file_sdml_entry();
void rpt_file_sdml_end();
void rpt_file_html_hdr();
void rpt_file_html_entry();
void rpt_file_html_end();
void rpt_file_rtf_hdr();
void rpt_file_rtf_entry();
void rpt_file_rtf_end();
void rpt_file_winhelp_hdr();
void rpt_file_winhelp_entry();
void rpt_file_winhelp_end();
void rpt_file_vmshelp_hdr();
void rpt_file_vmshelp_entry();
void rpt_file_vmshelp_end();

void rpt_source_text_hdr();
void rpt_source_text_entry();
void rpt_source_text_end();
void rpt_source_sdml_hdr();
void rpt_source_sdml_entry();
void rpt_source_sdml_end();
void rpt_source_html_hdr();
void rpt_source_html_entry();
void rpt_source_html_end();
void rpt_source_rtf_hdr();
void rpt_source_rtf_entry();
void rpt_source_rtf_end();
void rpt_source_winhelp_hdr();
void rpt_source_winhelp_entry();
void rpt_source_winhelp_end();
void rpt_source_vmshelp_hdr();
void rpt_source_vmshelp_entry();
void rpt_source_vmshelp_end();

/*									    */
/* Report formatting dispatch tables.					    */
/*									    */

#define report_hdr_routine(t) ((t)[report_format()][0])
#define report_entry_routine(t) ((t)[report_format()][1])
#define report_end_routine(t) ((t)[report_format()][2])

typedef void (*FORMATTER)();

static FORMATTER rpt_defined_routines[][3] = {
    {rpt_defined_text_hdr, rpt_defined_text_entry, rpt_defined_text_end},
    {rpt_defined_sdml_hdr, rpt_defined_sdml_entry, rpt_defined_sdml_end},
    {rpt_defined_html_hdr, rpt_defined_html_entry, rpt_defined_html_end},
    {rpt_defined_rtf_hdr,  rpt_defined_rtf_entry,  rpt_defined_rtf_end},
    {rpt_defined_winhelp_hdr, rpt_defined_winhelp_entry, rpt_defined_winhelp_end},
    {rpt_defined_vmshelp_hdr, rpt_defined_vmshelp_entry, rpt_defined_vmshelp_end}
};

static FORMATTER rpt_undefined_routines[][3] = {
    {rpt_undefined_text_hdr, rpt_undefined_text_entry, rpt_undefined_text_end},
    {rpt_undefined_sdml_hdr, rpt_undefined_sdml_entry, rpt_undefined_sdml_end},
    {rpt_undefined_html_hdr, rpt_undefined_html_entry, rpt_undefined_html_end},
    {rpt_undefined_rtf_hdr,  rpt_undefined_rtf_entry,  rpt_undefined_rtf_end},
    {rpt_undefined_winhelp_hdr, rpt_undefined_winhelp_entry, rpt_undefined_winhelp_end},
    {rpt_undefined_vmshelp_hdr, rpt_undefined_vmshelp_entry, rpt_undefined_vmshelp_end}
};

static FORMATTER rpt_calls_routines[][3] = {
    {rpt_calls_text_hdr, rpt_calls_text_entry, rpt_calls_text_end},
    {rpt_calls_sdml_hdr, rpt_calls_sdml_entry, rpt_calls_sdml_end},
    {rpt_calls_html_hdr, rpt_calls_html_entry, rpt_calls_html_end},
    {rpt_calls_rtf_hdr,  rpt_calls_rtf_entry,  rpt_calls_rtf_end},
    {rpt_calls_winhelp_hdr, rpt_calls_winhelp_entry, rpt_calls_winhelp_end},
    {rpt_calls_vmshelp_hdr, rpt_calls_vmshelp_entry, rpt_calls_vmshelp_end}
};

static FORMATTER rpt_xref_routines[][3] = {
    {rpt_xref_text_hdr, rpt_xref_text_entry, rpt_xref_text_end},
    {rpt_xref_sdml_hdr, rpt_xref_sdml_entry, rpt_xref_sdml_end},
    {rpt_xref_html_hdr, rpt_xref_html_entry, rpt_xref_html_end},
    {rpt_xref_rtf_hdr,  rpt_xref_rtf_entry,  rpt_xref_rtf_end},
    {rpt_xref_winhelp_hdr, rpt_xref_winhelp_entry, rpt_xref_winhelp_end},
    {rpt_xref_vmshelp_hdr, rpt_xref_vmshelp_entry, rpt_xref_vmshelp_end}
};

static FORMATTER rpt_tree_routines[][3] = {
    {rpt_tree_text_hdr, rpt_tree_text_entry, rpt_tree_text_end},
    {rpt_tree_sdml_hdr, rpt_tree_sdml_entry, rpt_tree_sdml_end},
    {rpt_tree_html_hdr, rpt_tree_html_entry, rpt_tree_html_end},
    {rpt_tree_rtf_hdr,  rpt_tree_rtf_entry,  rpt_tree_rtf_end},
    {rpt_tree_winhelp_hdr, rpt_tree_winhelp_entry, rpt_tree_winhelp_end},
    {rpt_tree_vmshelp_hdr, rpt_tree_vmshelp_entry, rpt_tree_vmshelp_end}
};

static FORMATTER rpt_byfile_routines[][3] = {
    {rpt_byfile_text_hdr, rpt_byfile_text_entry, rpt_byfile_text_end},
    {rpt_byfile_sdml_hdr, rpt_byfile_sdml_entry, rpt_byfile_sdml_end},
    {rpt_byfile_html_hdr, rpt_byfile_html_entry, rpt_byfile_html_end},
    {rpt_byfile_rtf_hdr,  rpt_byfile_rtf_entry,  rpt_byfile_rtf_end},
    {rpt_byfile_winhelp_hdr, rpt_byfile_winhelp_entry, rpt_byfile_winhelp_end},
    {rpt_byfile_vmshelp_hdr, rpt_byfile_vmshelp_entry, rpt_byfile_vmshelp_end}
};

static FORMATTER rpt_file_routines[][3] = {
    {rpt_file_text_hdr, rpt_file_text_entry, rpt_file_text_end},
    {rpt_file_sdml_hdr, rpt_file_sdml_entry, rpt_file_sdml_end},
    {rpt_file_html_hdr, rpt_file_html_entry, rpt_file_html_end},
    {rpt_file_rtf_hdr,  rpt_file_rtf_entry,  rpt_file_rtf_end},
    {rpt_file_winhelp_hdr, rpt_file_winhelp_entry, rpt_file_winhelp_end},
    {rpt_file_vmshelp_hdr, rpt_file_vmshelp_entry, rpt_file_vmshelp_end}
};

static FORMATTER rpt_source_routines[][3] = {
    {rpt_source_text_hdr, rpt_source_text_entry, rpt_source_text_end},
    {rpt_source_sdml_hdr, rpt_source_sdml_entry, rpt_source_sdml_end},
    {rpt_source_html_hdr, rpt_source_html_entry, rpt_source_html_end},
    {rpt_source_rtf_hdr,  rpt_source_rtf_entry,  rpt_source_rtf_end},
    {rpt_source_winhelp_hdr, rpt_source_winhelp_entry, rpt_source_winhelp_end},
    {rpt_source_vmshelp_hdr, rpt_source_vmshelp_entry, rpt_source_vmshelp_end}
};

/*************************************************************************++*/
int tree_size(
/* Computes the size (number of nodes) of a routine's call tree. The	    */
/* current implementation is only an approximation, showing a minimal	    */
/* tree size, since it only looks down one level; deep trees will throw it  */
/* off.									    */

    DEFINITION
	    *aDef
	    /* (MODIFY, BY ADDR):  					    */
	    /* Definition to check. The tree size will be cached in this    */
	    /* definition.						    */

)	/* Returns size of tree.					    */
	/*****************************************************************--*/

{
    int	    nodes;			    /* Node counter.		    */
    REFERENCE				    /* Current reference entry.     */
	    *called;
    DEFINITION				    /* Definition for current	    */
	    *curdef;			    /* reference entry.		    */

    if (def_tree_size(aDef) != 0) {	    /* Already been computed.	    */
	return def_tree_size(aDef);
    }
    else if (!needs_tree(aDef)) {	    /* No tree for routine.	    */
	return 0;
    }
    else {				    /* Check called routines.	    */
	for (nodes = 0, called = list_first(def_refs(aDef));
	     called != NULL; nodes++, called = next_entry(called)) {
	    if (needs_tree(ref_definition(called))) {
		nodes += def_num_calls(ref_definition(called));
	    }
	}
	set_def_tree_size(aDef, nodes);
	return nodes;
    }
}

/*************************************************************************++*/
int fold_string(
/* Folds a string at the non-alphanumeric nearest the right margin. A	    */
/* whitespace is removed; anything else is left in front of the fold.	    */

    char    *aSrc,
	    /* (READ, BY ADDR):  					    */
	    /* Source string to be folded.				    */

    int	    vPos,
	    /* (READ, BY VAL):						    */
	    /* Position in source string where characters should be copied  */
	    /* from. When repeatedly folding a long string, this should	    */
	    /* be the return value from a previous call to fold_string. It  */
	    /* is assumed to be less than the length of aSrc.		    */

    char    *aPrefix,
	    /* (READ, BY VAL):						    */
	    /* Optional prefix string to be written to aDest before	    */
	    /* characters from aSrc. Must be shorter than vLength, or it    */
	    /* will be ignored. If NULL ptr is passed, no prefix is used.   */

    char    *aDest,
	    /* (WRITE, BY ADDR):					    */
	    /* Destination buffer into which string is to be folded. It is  */
	    /* assumed to be at least vLength + 1 characters long.	    */

    int	    vLength
	    /* (READ, BY VAL):						    */
	    /* Maximum length of string to be written into aDest.	    */

)	/* Returns position in aStr where copying stopped. It will be equal */
	/* to the length of aSrc when all characters have been copied from  */
	/* aSrc.  This may be used as the vPos value on a successive call   */
	/* to fold_string.						    */
	/*****************************************************************--*/

{
    int	    curlen = 0;			    /* Length accumulator.	    */
    int	    remlen;			    /* Remaining length of string.  */
    int	    fold;			    /* Point at which to fold it.   */
    int	    spaceflag = 0;		    /* Flag: fold at space.	    */

    if (aPrefix != NULL && (curlen = strlen(aPrefix)) < vLength) {
	strcpy(aDest, aPrefix);		    /* Copy prefix first.	    */
    }

    remlen = strlen(&aSrc[vPos]);
    if (curlen + remlen <= vLength) {	    /* Remainder of string fits	    */
	strcpy(&aDest[curlen], &aSrc[vPos]);/* without folding.		    */
	return vPos + remlen;
    }
    else {				    /* Need to fold it, find fold   */
					    /* point.			    */
	for (fold = vPos + vLength - curlen - 1;
	    fold > vPos && isalnum(aSrc[fold]);
	    fold--);
	if (fold == vPos) {		    /* Fold point not found, fold   */
	    fold = vPos + vLength - 1;	    /* it at right margin.	    */
	}
	else if (!(spaceflag = isspace(aSrc[fold]))) {
					    /* Folding at non-alphanum,	    */
	    fold++;			    /* non-space, fold after it.    */
	}
					    /* Copy folded part of string.  */
	strncpy(&aDest[curlen], &aSrc[vPos], fold - vPos);
	aDest[fold - vPos] = '\0';
	if (spaceflag) {		    /* Remove whitespace at fold.   */
	    fold++;
	}
	return fold;			    /* Next fold will start after   */
    }					    /* this one.		    */
}

/*************************************************************************++*/
char *report_filext(
/* Returns the file extension string to be used for report files, based on  */
/* the type of report output requested.					    */

    /* No arguments.							    */

)	/* Returns ptr to file name extension string.			    */
	/*****************************************************************--*/

{
    if (rpt_sdml_enabled())	return OUTFILE_EXT_SDML;
    if (rpt_html_enabled())	return OUTFILE_EXT_HTML;
    if (rpt_rtf_enabled())	return OUTFILE_EXT_RTF;
    if (rpt_winhelp_enabled())	return OUTFILE_EXT_WINHELP;
    if (rpt_vmshelp_enabled())	return OUTFILE_EXT_VMSHELP;
    return OUTFILE_EXT_TEXT;		    /* Default output format.	    */
}

/*************************************************************************++*/
SOURCEFILE *next_defining_file(
/* Returns next source file containing routine definitions.		    */

    SOURCEFILE
	    *aSourceFile
	    /* (READ, BY ADDR):  					    */
	    /* Source file to check.					    */

)	/* Returns ptr to source file, or NULL if one not found.	    */
	/*****************************************************************--*/

{
    SOURCEFILE				    /* Current file being checked.  */
	    *curfile;
	    
    for (curfile = next_entry(aSourceFile); /* Scan rest of file list.	    */
	curfile != NULL;
	curfile = next_entry(curfile)) {
	if (source_routines(curfile) > 0) { 
	    return curfile;		    /* Found another with defs.	    */
	}
    }
    return NULL;			    /* No more with defs.	    */
}

/*************************************************************************++*/
SOURCEFILE *prev_defining_file(
/* Returns previous source file containing routine definitions.		    */

    SOURCEFILE
	    *aSourceFile
	    /* (READ, BY ADDR):  					    */
	    /* Source file to check.					    */

)	/* Returns ptr to source file, or NULL if one not found.	    */
	/*****************************************************************--*/

{
    SOURCEFILE				    /* Current file being checked.  */
	    *curfile;
	    
    for (curfile = prev_entry(aSourceFile); /* Scan prior file list.	    */
	curfile != NULL;
	curfile = prev_entry(curfile)) {
	if (source_routines(curfile) > 0) { 
	    return curfile;		    /* Found another with defs.	    */
	}
    }
    return NULL;			    /* No prior with defs.	    */
}

/*************************************************************************++*/
int isfirst_in_file(
/* Determines whether a definition entry is the first one in its source	    */
/* file, assuming the definitions are currently sorted in file order.	    */

    DEFINITION
	    *aDef
	    /* (READ, BY ADDR):  					    */
	    /* Definition to check.					    */

)	/* Returns flag indicating whether definition is first:		    */
	/*  1 - Definition is first in file.				    */
	/*  0 - Definition is not first in file.			    */
	/*****************************************************************--*/

{
    DEFINITION				    /* Ptr to prev def.		    */
	    *prevdef;

    if ((prevdef = prev_entry(aDef)) != NULL) {
	return (def_source(aDef) != def_source(prevdef));
    }
    return 1;
}

/*************************************************************************++*/
int islast_in_file(
/* Determines whether a definition entry is the last one in its source	    */
/* file, assuming the definitions are currently sorted in file order.	    */

    DEFINITION
	    *aDef
	    /* (READ, BY ADDR):  					    */
	    /* Definition to check.					    */

)	/* Returns flag indicating whether definition is last:		    */
	/*  1 - Definition is lasst in file.				    */
	/*  0 - Definition is not last in file.				    */
	/*****************************************************************--*/

{
    DEFINITION				    /* Ptr to next def.		    */
	    *nextdef;

    if ((nextdef = next_entry(aDef)) != NULL) {
	return (def_source(aDef) != def_source(nextdef));
    }
    return 1;
}

/*************************************************************************++*/
DEFINITION *next_tree(
/* Returns next definition that needs a call tree.			    */

    DEFINITION
	    *aDef
	    /* (READ, BY ADDR):  					    */
	    /* Definition to check.					    */

)	/* Returns ptr to definition, or NULL if none found.		    */
	/*****************************************************************--*/

{
    DEFINITION				    /* Current def being checked.   */
	    *curdef;
	    
    for (curdef = next_entry(aDef);	    /* Scan rest of def list.	    */
	curdef != NULL;
	curdef = next_entry(curdef)) {
	if (needs_tree(curdef)) { 
	    return curdef;		    /* Found another needing tree.  */
	}
    }
    return NULL;			    /* No more needing tree.	    */
}

/*************************************************************************++*/
DEFINITION *prev_tree(
/* Returns previous definition that needs a call tree.			    */

    DEFINITION
	    *aDef
	    /* (READ, BY ADDR):  					    */
	    /* Definition to check.					    */

)	/* Returns ptr to definition, or NULL if none found.		    */
	/*****************************************************************--*/

{
    DEFINITION				    /* Current def being checked.   */
	    *curdef;
	    
    for (curdef = prev_entry(aDef);	    /* Scan prior def list.	    */
	curdef != NULL;
	curdef = prev_entry(curdef)) {
	if (needs_tree(curdef)) { 
	    return curdef;		    /* Found another needing tree.  */
	}
    }
    return NULL;			    /* No prior needing tree.	    */
}

/*************************************************************************++*/
DEFINITION *next_defined(
/* Returns next definition for a defined routine.			    */

    DEFINITION
	    *aDef
	    /* (READ, BY ADDR):  					    */
	    /* Definition to check.					    */

)	/* Returns ptr to definition, or NULL if none found.		    */
	/*****************************************************************--*/

{
    DEFINITION				    /* Current def being checked.   */
	    *curdef;
	    
    for (curdef = next_entry(aDef);	    /* Scan rest of def list.	    */
	curdef != NULL;
	curdef = next_entry(curdef)) {
	if (isdefined_routine(curdef)) { 
	    return curdef;		    /* Found another defined one.  */
	}
    }
    return NULL;			    /* No more.			    */
}

/*************************************************************************++*/
DEFINITION *prev_defined(
/* Returns previous definition of a defined routine.			    */

    DEFINITION
	    *aDef
	    /* (READ, BY ADDR):  					    */
	    /* Definition to check.					    */

)	/* Returns ptr to definition, or NULL if none found.		    */
	/*****************************************************************--*/

{
    DEFINITION				    /* Current def being checked.   */
	    *curdef;
	    
    for (curdef = prev_entry(aDef);	    /* Scan prior def list.	    */
	curdef != NULL;
	curdef = prev_entry(curdef)) {
	if (isdefined_routine(curdef)) { 
	    return curdef;		    /* Found another defined one.   */
	}
    }
    return NULL;			    /* No prior one.		    */
}

/*************************************************************************++*/
DEFINITION *next_undefined(
/* Returns next definition for an undefined routine.			    */

    DEFINITION
	    *aDef
	    /* (READ, BY ADDR):  					    */
	    /* Definition to check.					    */

)	/* Returns ptr to definition, or NULL if none found.		    */
	/*****************************************************************--*/

{
    DEFINITION				    /* Current def being checked.   */
	    *curdef;
	    
    for (curdef = next_entry(aDef);	    /* Scan rest of def list.	    */
	curdef != NULL;
	curdef = next_entry(curdef)) {
	if (!isdefined_routine(curdef)) { 
	    return curdef;		    /* Found another undefined one. */
	}
    }
    return NULL;			    /* No more.			    */
}

/*************************************************************************++*/
DEFINITION *prev_undefined(
/* Returns previous definition of an undefined routine.			    */

    DEFINITION
	    *aDef
	    /* (READ, BY ADDR):  					    */
	    /* Definition to check.					    */

)	/* Returns ptr to definition, or NULL if none found.		    */
	/*****************************************************************--*/

{
    DEFINITION				    /* Current def being checked.   */
	    *curdef;
	    
    for (curdef = prev_entry(aDef);	    /* Scan prior def list.	    */
	curdef != NULL;
	curdef = prev_entry(curdef)) {
	if (!isdefined_routine(curdef)) { 
	    return curdef;		    /* Found another undefined one. */
	}
    }
    return NULL;			    /* No prior one.		    */
}

/*************************************************************************++*/
int separate_tree(
/* Determines whether or not the user has requested a separate call tree    */
/* for a routine.							    */

    DEFINITION
	    *aDef
	    /* (READ, BY ADDR):  					    */
	    /* Routine definition to check.				    */

)	/* Returns status flag:						    */
	/*  1 - User has requested separate tree for this routine.	    */
	/*  0 - User has not requested separate tree for this routine.	    */
	/*****************************************************************--*/

{
    char    **rlist;			    /* Routine name list ptr.	    */

    if ((rlist = separate_list()) == NULL) {    
	return 0;			    /* No list to worry about.	    */
    }
    else {				    /* Otherwise, check each name   */
	while (*rlist != NULL) {	    /* in list.			    */
	    if (strcmp(*rlist++, def_name(aDef)) == 0) {
		return 1;		    /* Found it, separate tree.	    */
	    }
	}
	return 0;			    /* Routine is not in list.	    */
    }
}

/*************************************************************************++*/
int needs_tree(
/* Determines whether or not a routine definition needs a separate call	    */
/* tree. See the description of report_call_trees for details.		    */

    DEFINITION
	    *aDef
	    /* (MODIFY, BY ADDR):					    */
	    /* Routine definition entry to check. If the routine has not    */
	    /* already been checked, the results of the			    */
	    /* evalution are cached in the definition.			    */

)	/* Returns flag indicating whether or not tree is needed:	    */
	/*  1	- Separate tree is needed.				    */
	/*  0	- Separate tree is not needed, routine should be expanded   */
	/*	  in callers.						    */
	/*****************************************************************--*/

{
    int	    status;			    /* Evaluation status.	    */
    DEFINITION				    /* Current definition ptr.	    */
	    *curdef;

    curdef = next_entry(aDef);
    if (def_tree_required(aDef) != NOT_YET_EVALUATED) {
	return (def_tree_required(aDef) == TREE_REQUIRED);
    }
    else if (def_num_calls(aDef) == 0) {    /* Does not call anything, no   */
	status = 0;			    /* tree to worry about.	    */
    }
    else if (tree_inline_disabled()) {	    /* User says separate tree for  */
	status = 1;			    /* all routines.		    */
    }
    else if (separate_tree(aDef)) {	    /* User says make this one	    */
	status = 1;			    /* separate.		    */
    }
    else if (!isend_of_list(aDef) &&
	     strcmp(def_name(aDef), def_name(curdef)) == 0) {
	status = 1;			    /* Multiply-defined routine,    */
	curdef = aDef;			    /* all versions of it need a    */
	for (curdef = aDef;		    /* tree.			    */
	    curdef != NULL && strcmp(def_name(aDef), def_name(curdef)) == 0;
	    curdef = next_entry(curdef)) {
	    set_def_tree_req(curdef, TREE_REQUIRED);
	}
    }
    else if (def_num_callers(aDef) >= 1 &&   
	    def_num_callers(aDef) <= max_callers()) {
	status = 0;			    /* Too few callers for separate */
    }					    /* tree.			    */
    else if (!tree_greater_than(aDef, MIN_TREE_SIZE)) {
	status = 0;			    /* Tree is trivial.		    */
    }
    else {
	status = 1;			    /* Generate separate tree!	    */
    }
    if (status) {			    /* Cache evaluation result.	    */
	set_def_tree_req(aDef, TREE_REQUIRED);
    }
    else {
	set_def_tree_req(aDef, TREE_NOT_REQUIRED);
    }
    return status;
}

/*************************************************************************++*/
void report_defined(
/* Reports the defined routines in alphabetical order to a file.	    */

    char    *aFileName
	    /* (READ, BY ADDR):						    */
	    /* Report output file name.					    */

)	/* No return value.						    */
	/*****************************************************************--*/

{
    void    (*report_hdr)();		    /* Ptr to report header	    */
					    /* routine.			    */
    void    (*report_entry)();		    /* Ptr to report line routine.  */
    void    (*report_end)();		    /* Ptr to report end routine.   */
    FILE    *rptfile;			    /* Report file ptr.		    */
    DEFINITION				    /* Current routine entry.	    */
	    *curdef;
    long    total;			    /* Total defined routines.	    */

    /*+									    */
    /*	Select appropriate set of output formatter routines. Then open file */
    /*	and write report header. Next, for each defined routine (i.e. each  */
    /*	routine for which the source file is known), write a report line.   */
    /*	Finally, write report trailer and close file.			    */
    /*-									    */
    
    report_hdr   = report_hdr_routine(rpt_defined_routines);
    report_entry = report_entry_routine(rpt_defined_routines);
    report_end   = report_end_routine(rpt_defined_routines);

    if ((rptfile = fopen(aFileName, "w")) == NULL) {
	printf("ERROR: Unable to open %s for report output\n", aFileName);
    }
    else {
	if (list_enabled()) {
	    fprintf(list_file(), "\nREPORT Defined routines in file %s\n",
		aFileName);
	}
	if (log_enabled()) {
	    printf("REPORT Defined routines in file %s\n", aFileName);
	}
	(*report_hdr)(rptfile);		    /* Write header.		    */
	for (total = 0, curdef = list_first(global_deflist()); curdef != NULL;
	    curdef = next_entry(curdef)) {
	    if (isdefined_routine(curdef)) {
					    /* Write entry line.	    */
		if (trace_rpt_enabled()) {
		    printf("TRACE: Report defined routine %s @ %lxh\n",
			def_name(curdef), curdef);
		}
		(*report_entry)(rptfile, curdef, (total == 0),
		    islast_defined(curdef));
		total++;
	    }
	    else if (trace_rpt_enabled()) {
		printf(
		"TRACE: Report defined, skipping undefined routine %s @ %lxh\n",
		    def_name(curdef), curdef);
	    }
	}
	(*report_end)(rptfile, total);	    /* Write trailer.		    */
	fclose(rptfile);
	if (list_enabled()) {
	    fprintf(list_file(), "       %ld Defined routines\n", total);
	}
	if (log_full_enabled()) {
	    printf("       %ld Defined routines\n\n", total);
	}
    }
}

/*************************************************************************++*/
void report_undefined(
/* Reports the undefined routines in alphabetical order to a file.	    */

    char    *aFileName
	    /* (READ, BY ADDR):						    */
	    /* Report output file name.					    */

)	/* No return value.						    */
	/*****************************************************************--*/

{
    void    (*report_hdr)();		    /* Ptr to report header	    */
					    /* routine.			    */
    void    (*report_entry)();		    /* Ptr to report line routine.  */
    void    (*report_end)();		    /* Ptr to report end routine.   */
    FILE    *rptfile;			    /* Report file ptr.		    */
    DEFINITION				    /* Current routine entry.	    */
	    *curdef;
    long    total;			    /* Total undefined routines.    */

    /*+									    */
    /*	Select appropriate set of output formatter routines. Then open file */
    /*	and write report header. Next, for each undefined routine (i.e.	    */
    /*	each routine for which the source file is not known), write a	    */
    /*	report line.  Finally, write report trailer and close file.	    */
    /*-									    */

    report_hdr   = report_hdr_routine(rpt_undefined_routines);
    report_entry = report_entry_routine(rpt_undefined_routines);
    report_end   = report_end_routine(rpt_undefined_routines);

    if ((rptfile = fopen(aFileName, "w")) == NULL) {
	printf("ERROR: Unable to open %s for report output\n", aFileName);
    }
    else {
	if (list_enabled()) {
	    fprintf(list_file(), "\nREPORT External routines in file %s\n",
		aFileName);
	}
	if (log_enabled()) {
	    printf("REPORT External routines in file %s\n", aFileName);
	}
	(*report_hdr)(rptfile);		    /* Write header.		    */
	for (total = 0, curdef = list_first(global_deflist()); curdef != NULL;
	    curdef = next_entry(curdef)) {
	    if (!isdefined_routine(curdef)) {
					    /* Write entry line.	    */
		if (trace_rpt_enabled()) {
		    printf("TRACE: Report external routine %s @ %lxh\n",
			def_name(curdef), curdef);
		}
		(*report_entry)(rptfile, curdef, (total == 0),
		    islast_undefined(curdef));
		total++;
	    }
	    else if (trace_rpt_enabled()) {
		printf(
		"TRACE: Report external, skipping defined routine %s @ %lxh\n",
		    def_name(curdef), curdef);
	    }
	}
	(*report_end)(rptfile, total);	    /* Write trailer.		    */
	fclose(rptfile);
	if (list_enabled()) {
	    fprintf(list_file(), "       %ld External routines\n", total);
	}
	if (log_full_enabled()) {
	    printf("       %ld External routines\n\n", total);
	}
    }
}

/*************************************************************************++*/
void report_calls(
/* Reports the calls and callers of the defined routines in alphabetical    */
/* order to a file.							    */

    char    *aFileName
	    /* (READ, BY ADDR):						    */
	    /* Report output file name.					    */

)	/* No return value.						    */
	/*****************************************************************--*/

{
    void    (*report_hdr)();		    /* Ptr to section header	    */
					    /* routine.			    */
    void    (*report_entry)();		    /* Ptr to report line routine.  */
    void    (*report_end)();		    /* Ptr to section end routine.  */
    FILE    *rptfile;			    /* Report file ptr.		    */
    DEFINITION				    /* Current routine entry.	    */
	    *curdef;
    REFERENCE				    /* Current caller ref.	    */
	    *caller;
    REFERENCE				    /* Current called ref.	    */
	    *called;
    long    total;			    /* Total defined routines.	    */

    /*+									    */
    /*	Select appropriate set of output formatter routines and open file.  */
    /*	For each defined routine, write a header section. For each	    */
    /*	reference and caller in a routine definition, write a report line   */
    /*	(they are output in side-by-side pairs until both lists are	    */
    /*	exhausted, although there is no relationship implied by the	    */
    /*	pairing). Then write a trailer section for the defined routine.	    */
    /*	Last, close the file.						    */
    /*-									    */

    report_hdr   = report_hdr_routine(rpt_calls_routines);
    report_entry = report_entry_routine(rpt_calls_routines);
    report_end   = report_end_routine(rpt_calls_routines);

    if ((rptfile = fopen(aFileName, "w")) == NULL) {
	printf("ERROR: Unable to open %s for report output\n", aFileName);
    }
    else {
	if (list_enabled()) {
	    fprintf(list_file(),
		"\nREPORT Defined routine calls/callers in file %s\n",
		aFileName);
	}
	if (log_enabled()) {
	    printf("REPORT Defined routine calls/callers in file %s\n",
		aFileName);
	}
	for (total = 0, curdef = list_first(global_deflist()); curdef != NULL;
	    curdef = next_entry(curdef)) {
	    if (isdefined_routine(curdef)) {
					    /* Write header section.	    */
		if (trace_rpt_enabled()) {
		    printf(
			"TRACE: Report calls/callers for routine %s @ %lxh\n",
			def_name(curdef), curdef);
		}
		(*report_hdr)(rptfile, curdef);
		for (called = list_first(def_refs(curdef)),
		     caller = list_first(def_callers(curdef));
		     called != NULL || caller != NULL;
		     caller = next_entry(caller), called = next_entry(called)) {
					    /* Write calls/caller line.	    */
		    (*report_entry)(rptfile, called, caller);
		}			    /* Write trailer section.	    */
		(*report_end)(rptfile, curdef);
		total++;
	    }
	    else if (trace_rpt_enabled()) {
		printf(
	"TRACE: Report calls/callers, skipping external routine %s @ %lxh\n",
		    def_name(curdef), curdef);
	    }
	}
	fclose(rptfile);
	if (list_enabled()) {
	    fprintf(list_file(), "       %ld Defined routines\n", total);
	}
	if (log_full_enabled()) {
	    printf("       %ld Defined routines\n\n", total);
	}
    }
}

/*************************************************************************++*/
void report_xrefs(
/* Reports caller cross references for all routines (defined and undefined) */
/* in alphabetical order to a file.					    */

    char    *aFileName
	    /* (READ, BY ADDR):						    */
	    /* Report output file name.					    */

)	/* No return value.						    */
	/*****************************************************************--*/

{
    void report_tree();

    void    (*report_hdr)();		    /* Ptr to report header	    */
					    /* routine.			    */
    void    (*report_entry)();		    /* Ptr to report line routine.  */
    void    (*report_entry_tree)();	    /* Ptr to report line routine.  */
    void    (*report_end)();		    /* Ptr to report end routine.   */
    void    (*report_end_tree)();	    /* Ptr to report end routine.   */
    FILE    *rptfile;			    /* Report file ptr.		    */
    DEFINITION				    /* Current routine entry.	    */
	    *curdef;
    DEFINITION				    /* Temporary def to hold call   */
	    *treedef;			    /* tree.			    */
    REFERENCE				    /* Temporary ref to current	    */
	    *curref;			    /* definition.		    */
    long    total;			    /* Total defined routines.	    */
    int	    xreffile = 0;		    /* Xref report file number.	    */

    /*+									    */
    /*	Select appropriate set of output formatter routines. Then open file */
    /*	and write report header. Next, for each routine, write a report	    */
    /*	line.  Finally, write report trailer and close file.		    */
    /*									    */

    report_hdr   = report_hdr_routine(rpt_xref_routines);
    report_entry = report_entry_routine(rpt_xref_routines);
    report_entry_tree = report_entry_routine(rpt_tree_routines);
    report_end_tree = report_end_routine(rpt_tree_routines);
    report_end   = report_end_routine(rpt_xref_routines);

    if ((rptfile = fopen(aFileName, "w")) == NULL) {
	printf("ERROR: Unable to open %s for report output\n", aFileName);
    }
    else {
	if (list_enabled()) {
	    fprintf(list_file(),
		"\nREPORT Caller cross references in file %s\n",
		aFileName);
	}
	if (log_enabled()) {
	    printf("REPORT Caller cross references in file %s\n", aFileName);
	}
	(*report_hdr)(rptfile);		    /* Write header.		    */
	for (total = 0, curdef = list_first(global_deflist());
	    curdef != NULL;
	    curdef = next_entry(curdef)) {
	    if (rpt_html_enabled() && def_xreffile(curdef) != xreffile) {
		xreffile = rpt_html_continue(&rptfile, CALLER_XREF_SECTION,
				curdef, xreffile, def_xreffile(curdef));
	    }
					    /* Write entry line.	    */
	    if (trace_rpt_enabled()) {
		printf("TRACE: Report caller xref for routine %s @ %lxh\n",
		    def_name(curdef), curdef);
	    }
	    (*report_entry)(rptfile, curdef, !total, isend_of_list(curdef));

	    if (def_num_calls(curdef) != 0) {

	    /******* The following code adapted from report_call_trees ********/

		curref  = new_ref(0, curdef, NULL);
		if (trace_rpt_enabled()) {
		    printf("TRACE: Report call tree for routine %s @ %lxh\n",
			def_name(curdef), curdef);
		}
		(*report_entry_tree)(rptfile, curref, 0, 0);
		free_ref(curref);
					    
		/*+							    */
		/* Generate the subtree. Create a temporary def to serve as */
		/* the tree root and set its tree root to the current	    */
		/* definition as a context value. This is used in detection */
		/* of duplicate and recursive expansions. Use the ref list  */
		/* of this tree to build a stack of caller refs. Push a ref */
		/* for the current routine to this stack, reflecting the    */
		/* fact that it is the first caller, then report the tree.  */
		/* Then pop the reference and free the root definition.	    */
		/*-							    */

		treedef = new_def(def_name(curdef), def_source(curdef));
		set_def_root(treedef, curdef);
		push_ref(treedef, new_ref(0, curdef, NULL));
		report_tree(rptfile, report_entry_tree, curdef, 1, treedef);
		free_ref(pop_ref(treedef));
		free_def(treedef);
					    /* Write trailer section.	    */
		(*report_end_tree)(rptfile, curdef, (total == 0), 1);

	    /****** The preceding code adapted from report_call_trees *********/

	    }
	    total++;
	}
	(*report_end)(rptfile, total);	    /* Write trailer.		    */
	fclose(rptfile);
	if (list_enabled()) {
	    fprintf(list_file(), "       %ld Routines\n", total);
	}
	if (log_full_enabled()) {
	    printf("       %ld Routines\n\n", total);
	}
    }
}

/*************************************************************************++*/
void report_tree(
/* Recursively reports one line for each routine in the call tree. The	    */
/* recursion is depth first (i.e. expanding each referenced routine before  */
/* showing the next at the same level). The recursion nesting level	    */
/* controls indentation of the called routine names.			    */

    FILE    *aRptFile,
	    /* (READ, BY ADDR):						    */
	    /* Report output file. Must be opened by caller.		    */

    void    (*aReportEntry)(),
	    /* (EXEC, BY ADDR):						    */
	    /* Report line formatter routine.				    */

    DEFINITION
	    *aDef,
	    /* (MODIFY, BY ADDR):					    */
	    /* Routine definition entry to report. The tree root for the    */
	    /* definition will be set to aTreeRoot.			    */
    
    int	    vLevel,
	    /* (READ, BY VAL):						    */
	    /* Recursion nesting level. Level 0 is the top level.	    */

    DEFINITION
	    *aTreeRoot
	    /* (MODIFY, BY ADDR):					    */
	    /* Routine definition entry that acts as root of this call	    */
	    /* tree.  References to the routines in the call tree will be   */
	    /* added to this definition. Its root field is used as the	    */
	    /* context value for this tree expansion.			    */
    
)	/* No return value.						    */
	/*****************************************************************--*/

{
    REFERENCE				    /* Current reference entry.     */
	    *called;
    DEFINITION				    /* Definition for current	    */
	    *curdef;			    /* reference entry.		    */
    int	    expanded;			    /* Flag indicating routine	    */
					    /* has already been reported.   */
    int	    recursive;			    /* Flag indicating routine is   */
					    /* called recursively.	    */
    char    errmsg[256];		    /* Error message buffer.	    */

    /*+									    */
    /*	For each referenced routine, report the routine. If the routine has */
    /*	not already been expanded in this tree, is defined, and does not    */
    /*	have a separate tree (and we have not exceeded max tree depth),	    */
    /*	generate its subtree recursively.				    */
    /*-									    */

    for (called = list_first(def_refs(aDef));
	 called != NULL; called = next_entry(called)) {
	 
					    /* Get current def ptr and find */
					    /* out expanded/recursive	    */
	curdef    = ref_definition(called); /* conditions.		    */
	recursive = isrecursive_ref(called, aTreeRoot);
	expanded  = (!recursive && !needs_tree(curdef) &&
		    isalready_expanded(curdef, def_root(aTreeRoot)));

	(*aReportEntry)(aRptFile, called, vLevel, expanded, recursive);

					    /* Mark def as part of tree.    */
	set_def_root(curdef, def_root(aTreeRoot));
	
	if (!expanded && !recursive &&
	    isdefined_routine(curdef) && !needs_tree(curdef)) {
	    if (vLevel < max_tree_depth()) {

		/*+							    */
		/*  A subtree needs to be generated. Push a reference to    */
		/*  the current routine to the call stack for recursion	    */
		/*  detection at lower levels, do the lower level subtree,  */
		/*  and discard the added reference.			    */
		/*-							    */

		push_ref(aTreeRoot, new_ref(0, curdef, NULL));
		report_tree(aRptFile, aReportEntry, curdef, vLevel + 1,
		    aTreeRoot);
		free_ref(pop_ref(aTreeRoot));
	    }
	    else if (vLevel == MAX_TREE_DEPTH) {
		sprintf(errmsg,
	    "\nWARNING: %s tree depth exceeds %d levels, halting expansion\n",
		    def_name(aTreeRoot), MAX_TREE_DEPTH);
		fputs(errmsg, aRptFile);
		if (list_enabled()) {
		    fputs(errmsg, list_file());
		}
		puts(errmsg);
		break;
	    }
	}
    }
}

#if 0
/*************************************************************************++*/
void report_call_trees(
/* Reports the call trees for the defined routines in alphabetical order to */
/* a file. The report attempts to minimize the number of trivial trees	    */
/* generated and maximize tree depth, while limiting the amount of	    */
/* repetitive subtree expansion. To meet this goal, a separate call tree    */
/* will not be generated in the following cases:			    */
/*									    */
/*	1) a routine that calls no other routines (this eliminates empty    */
/*	   call trees);							    */
/*									    */
/*	2) a routine that calls one or more other routines, but is called   */
/*	   from only 1-x other routines, where x is a user-settable limit   */
/*	   (this eliminates low-usage routines that can be better shown	    */
/*	   as subtrees of their callers);				    */
/*									    */
/*	3) a routine that is called from more than x other routines, but    */
/*	   that has a total tree expansion of only 1-4 nodes (this	    */
/*	   eliminates very simple call trees that can be easily shown as    */
/*	   subtrees of their callers).					    */
/*									    */
/* In these cases, any call relationships will be shown in the call trees   */
/* of any caller. There is one special exception to cases 2 and 3, where a  */
/* routine is multiply-defined. This indicates multiple, alternate	    */
/* implementations of the same routine, possibly with different call	    */
/* structures. Until the product is linked, it cannot be determined which   */
/* version of the routine will be called; therefore a separate tree will be */
/* generated for each version of the routine.				    */
/*									    */
/* In all other cases, a separate call tree will be generated for a	    */
/* routine. Specifically, this means routines that are not called from any  */
/* other, and frequently-used routines with complex call trees. There are   */
/* also two user-selectable overrides to this behavior. The "separate"	    */
/* option allows the user to specify a file containing a list of routine    */
/* names for which separate trees must be generated if they call anything.  */
/* The "noinline" option forces separate trees for all routines that call   */
/* anything.								    */

    char    *aFileName
	    /* (READ, BY ADDR):						    */
	    /* Report output file name.					    */

)	/* No return value.						    */
	/*****************************************************************--*/

{
    void    (*report_hdr)();		    /* Ptr to section header	    */
					    /* routine.			    */
    void    (*report_entry)();		    /* Ptr to report line routine.  */
    void    (*report_end)();		    /* Ptr to section end routine.  */
    FILE    *rptfile;			    /* Report file ptr.		    */
    DEFINITION				    /* Current routine entry.	    */
	    *curdef;
    DEFINITION				    /* Temporary def to hold call   */
	    *treedef;			    /* tree.			    */
    int	    lastdef;			    /* Flag indicating last def	    */
					    /* with separate call tree.     */
    REFERENCE				    /* Temporary ref to current	    */
	    *curref;			    /* definition.		    */
    long    total;			    /* Total defined routines.	    */
    int	    treefile = 0;		    /* Tree report file number.	    */

    /*+									    */
    /*	Select appropriate set of output formatter routines and open file.  */
    /*	For each defined routine that warrants a separate call tree, write  */
    /*	a header section. Then do a depth-first recursive traversal of the  */
    /*	routine references and write a report line for each reference.	    */
    /*	Then write a trailer section for the defined routine.  Last, close  */
    /*	the file.							    */
    /*-									    */

    report_hdr   = report_hdr_routine(rpt_tree_routines);
    report_entry = report_entry_routine(rpt_tree_routines);
    report_end   = report_end_routine(rpt_tree_routines);

    if ((rptfile = fopen(aFileName, "w")) == NULL) {
	printf("ERROR: Unable to open %s for report output\n", aFileName);
    }
    else {
	if (list_enabled()) {
	    fprintf(list_file(),
		"\nREPORT Defined routine call trees in file %s\n",
		aFileName);
	}
	if (log_enabled()) {
	    printf("REPORT Defined routine call trees in file %s\n",
		aFileName);
	}
	for (total = 0, curdef = list_first(global_deflist()); curdef != NULL;
	    curdef = next_entry(curdef)) {
	    if (needs_tree(curdef)) {	    /* If routine needs tree, write */
					    /* header section and toplevel  */
					    /* line.			    */
		if (rpt_html_enabled() && def_treefile(curdef) != treefile) {
		    treefile = rpt_html_continue(&rptfile, CALL_TREE_SECTION,
				    curdef, treefile, def_treefile(curdef));
		}
		lastdef = islast_tree(curdef);
		(*report_hdr)(rptfile, curdef, (total == 0), lastdef);
		curref  = new_ref(0, curdef, NULL);
		if (trace_rpt_enabled()) {
		    printf("TRACE: Report call tree for routine %s @ %lxh\n",
			def_name(curdef), curdef);
		}
		(*report_entry)(rptfile, curref, 0, 0);
		free_ref(curref);
					    
		/*+							    */
		/* Generate the subtree. Create a temporary def to serve as */
		/* the tree root and set its tree root to the current	    */
		/* definition as a context value. This is used in detection */
		/* of duplicate and recursive expansions. Use the ref list  */
		/* of this tree to build a stack of caller refs. Push a ref */
		/* for the current routine to this stack, reflecting the    */
		/* fact that it is the first caller, then report the tree.  */
		/* Then pop the reference and free the root definition.	    */
		/*-							    */

		treedef = new_def(def_name(curdef), def_source(curdef));
		set_def_root(treedef, curdef);
		push_ref(treedef, new_ref(0, curdef, NULL));
		report_tree(rptfile, report_entry, curdef, 1, treedef);
		free_ref(pop_ref(treedef));
		free_def(treedef);
					    /* Write trailer section.	    */
		(*report_end)(rptfile, curdef, (total == 0), lastdef);
		total++;
	    }
	    else if (trace_rpt_enabled()) {
		printf("TRACE: Report call tree, skipping routine %s @ %lxh\n",
		    def_name(curdef), curdef);
	    }

	}
	fclose(rptfile);
	if (list_enabled()) {
	    fprintf(list_file(), "       %ld Trees generated\n", total);
	}
	if (log_full_enabled()) {
	    printf("       %ld Trees generated\n\n", total);
	}
    }
}
#endif

/*************************************************************************++*/
void report_by_file(
/* Reports the defined routines in the order in which they occurred in the  */
/* source files to a file. The files themselves are ordered alphabetically. */
/* Since this report is driven by the list of defined routines, if a file   */
/* contains no routine definitions, it will not be reported here.	    */

    char    *aFileName
	    /* (READ, BY ADDR):						    */
	    /* Report output file name.					    */

)	/* No return value.						    */
	/*****************************************************************--*/

{
    void    (*report_hdr)();		    /* Ptr to section header	    */
					    /* routine.			    */
    void    (*report_entry)();		    /* Ptr to report line routine.  */
    void    (*report_end)();		    /* Ptr to section end routine.  */
    FILE    *rptfile;			    /* Report file ptr.		    */
    DEFINITION				    /* Current routine entry.	    */
	    *curdef;
    SOURCEFILE				    /* Current file entry.	    */
	    *curfile;
    long    total;			    /* Total defined routines.	    */
    int	    totfiles;			    /* Total number of files.	    */
    int	    lastfile;			    /* Flag indicating last file    */
					    /* with defs.		    */
    int	    byfilefile = 0;		    /* By-file report file number.	    */

    /*+									    */
    /*	Select appropriate set of output formatter routines and open file.  */
    /*	For each source file, write a header section. For each routine in   */
    /*	that file, write a report list. Then write a trailer section for    */
    /*	the file. Last, close the file.					    */
    /*-									    */

    report_hdr   = report_hdr_routine(rpt_byfile_routines);
    report_entry = report_entry_routine(rpt_byfile_routines);
    report_end   = report_end_routine(rpt_byfile_routines);

    if ((rptfile = fopen(aFileName, "w")) == NULL) {
	printf("ERROR: Unable to open %s for report output\n", aFileName);
    }
    else {
	if (list_enabled()) {
	    fprintf(list_file(),
		"\nREPORT Routines by source file in file %s\n",
		aFileName);
	}
	if (log_enabled()) {
	    printf("REPORT Routines by source file in file %s\n", aFileName);
	}
					    /* Skip past undefined	    */
					    /* routines.		    */
	for (curdef = list_first(global_deflist());
	    curdef != NULL && !isdefined_routine(curdef);
	    curdef = next_entry(curdef)) {
	    if (trace_rpt_enabled()) {
		printf(
		"TRACE: Report file, skipping external routine %s @ %lxh\n",
		    def_name(curdef), curdef);
	    }
	}
					    /* Report each file's routines. */
	for (total = totfiles = 0,
		curdef != NULL ? curfile = def_source(curdef) : NULL;
	    curdef != NULL;
	    curdef != NULL ? curfile = def_source(curdef) : NULL) {
	    lastfile = islast_defining_file(curfile);
	    
	    if (rpt_html_enabled()
		&& source_byfilefile(curfile) != byfilefile) {
		byfilefile = rpt_html_continue(&rptfile,
				ROUTINES_BY_FILE_SECTION, curfile, byfilefile,
				source_byfilefile(curfile));
	    }
					    /* Write header section.	    */
	    (*report_hdr)(rptfile, curfile, (totfiles == 0), lastfile);
	    if (trace_rpt_enabled()) {
		printf("TRACE: Report routines in file %s @ %lxh\n",
		    source_name(curfile), curfile);
	    }
	    do {
					    /* Write entry line.	    */
		if (trace_rpt_enabled()) {
		    printf("TRACE: Report file %s routine %s @ %lxh\n",
			source_name(def_source(curdef)), def_name(curdef),
			curdef);
		}
		(*report_entry)(rptfile, curdef, isfirst_in_file(curdef),
		    islast_in_file(curdef));
		total++;
		curdef = next_entry(curdef);
	    } while (curdef != NULL && def_source(curdef) == curfile);

					    /* Write trailer section.	    */
	    (*report_end)(rptfile, curfile, (totfiles == 0), lastfile);
	    totfiles++;
	}
	fclose(rptfile);
	if (list_enabled()) {
	    fprintf(list_file(), "       %ld Defined routines in %d files\n",
		total, totfiles);
	}
	if (log_full_enabled()) {
	    printf("       %ld Defined routines in %d files\n\n", total,
		totfiles);
	}
    }
}

/*************************************************************************++*/
void report_files(
/* Reports the source files in alphabetical order to a file.		    */

    char    *aFileName
	    /* (READ, BY ADDR):						    */
	    /* Report output file name.					    */

)	/* No return value.						    */
	/*****************************************************************--*/

{
    void    (*report_hdr)();		    /* Ptr to report header	    */
					    /* routine.			    */
    void    (*report_entry)();		    /* Ptr to report line routine.  */
    void    (*report_end)();		    /* Ptr to report end routine.   */
    FILE    *rptfile;			    /* Report file ptr.		    */
    SOURCEFILE				    /* Current file entry.	    */
	    *curfile;
    int	    total;			    /* Total defined routines.	    */

    /*+									    */
    /*	Select appropriate set of output formatter routines. Open file and  */
    /*	write a header section. For each source file, write a report line.  */
    /*	Then write a trailer section and close the file.		    */
    /*-									    */

    report_hdr   = report_hdr_routine(rpt_file_routines);
    report_entry = report_entry_routine(rpt_file_routines);
    report_end   = report_end_routine(rpt_file_routines);

    if ((rptfile = fopen(aFileName, "w")) == NULL) {
	printf("ERROR: Unable to open %s for report output\n", aFileName);
    }
    else {
	if (list_enabled()) {
	    fprintf(list_file(),
		"\nREPORT Source files in file %s\n", aFileName);
	}
	if (log_enabled()) {
	    printf("REPORT Source files in file %s\n", aFileName);
	}

	(*report_hdr)(rptfile);		    /* Write header.		    */
	for (total = 0, curfile = list_first(global_filelist());
	    curfile != NULL;
	    curfile = next_entry(curfile)) {
					    /* Write entry line.	    */
	    if (trace_rpt_enabled()) {
		printf("TRACE: Report source file %s @ %lxh\n",
		    source_name(curfile), curfile);
	    }
	    (*report_entry)(rptfile, curfile, isbegin_of_list(curfile),
		isend_of_list(curfile));
	    total++;
	}
	(*report_end)(rptfile, total);	    /* Write trailer.		    */
	fclose(rptfile);
	if (list_enabled()) {
	    fprintf(list_file(), "       %d files\n", total);
	}
	if (log_full_enabled()) {
	    printf("       %d files\n\n", total);
	}
    }
}

/*************************************************************************++*/
void report_source(
/* Reports annotated source files with line numbers. The sources are	    */
/* reported in separate files.						    */

    char    *aFileName
	    /* (READ, BY ADDR):						    */
	    /* Report output file name format string.			    */

)	/* No return value.						    */
	/*****************************************************************--*/

{
    void    (*report_hdr)();		    /* Ptr to section header	    */
					    /* routine.			    */
    void    (*report_entry)();		    /* Ptr to report line routine.  */
    void    (*report_end)();		    /* Ptr to section end routine.  */
    char    rptname[MAX_FILE_NAME + 1];	    /* Report file name prefix.	    */
    char    outputname[MAX_FILE_NAME + 1];  /* Report file name buffer.	    */
    char    srcline[MAX_FILE_NAME * 2];	    /* Source line buffer.	    */
    FILE    *rptfile;			    /* Report file ptr.		    */
    FILE    *srcfile;			    /* Source file ptr.		    */
    DEFINITION				    /* Current routine entry.	    */
	    *curdef;
    SOURCEFILE				    /* Current file entry.	    */
	    *curfile;
    long    line;			    /* Current line number in file. */
    long    total;			    /* Total source lines.	    */
    int	    totfiles;			    /* Total number of files.	    */

    /*+									    */
    /*	Select appropriate set of output formatter routines.  For each	    */
    /*	source file, open source and report files and write a header	    */
    /*	section.  Write each line in the file, merging with the routine	    */
    /*	definitions.  Then write a trailer section for the file and close   */
    /*	the source and report files.					    */
    /*-									    */

    report_hdr   = report_hdr_routine(rpt_source_routines);
    report_entry = report_entry_routine(rpt_source_routines);
    report_end   = report_end_routine(rpt_source_routines);

    if (list_enabled()) {
	fputs("\nREPORT Annotated source files\n", list_file());
    }
    if (log_enabled()) {
	puts("REPORT Annotated source files");
    }
					    /* Skip past undefined	    */
					    /* routines.		    */
    for (curdef = list_first(global_deflist());
	curdef != NULL && !isdefined_routine(curdef);
	curdef = next_entry(curdef)) {
	if (trace_rpt_enabled()) {
	    printf(
	    "TRACE: Report sources, skipping external routine %s @ %lxh\n",
		def_name(curdef), curdef);
	}
    }
					    /* Report each file's contents. */
    for (total = totfiles = 0, curfile = list_first(global_filelist());
	curfile != NULL; curfile = next_entry(curfile)) {

	line = 0;
	sprintf(rptname, aFileName, source_seq(curfile));
	if ((srcfile = fopen(source_name(curfile), "r")) == NULL) {
	    printf("ERROR: Unable to open %s for input\n",
		source_name(curfile));
	}
	else if ((rptfile = fopen(make_filename(outfile_prefix(), rptname,
	        report_filext(), outputname), "w")) == NULL) {
	    printf("ERROR: Unable to open %s for report output\n", rptname);
	}
	else {				    /* Write header section.	    */
	    (*report_hdr)(rptfile, curfile, (totfiles == 0),
		isend_of_list(curfile));
	    if (trace_rpt_enabled()) {
		printf("TRACE: Report annotated source in file %s @ %lxh\n",
		    source_name(curfile), curfile);
	    }
					    /* Report lines through last    */
					    /* routine defined in this	    */
					    /* file.			    */
	    while (curdef != NULL && def_source(curdef) == curfile) {
		while (line < def_end(curdef)) {
					    /* Copy source line.	    */
		    fgets(srcline, sizeof(srcline), srcfile);
		    (*report_entry)(rptfile, curdef, srcline, ++line,
			source_tabsize(curfile));
		    total++;
		}
		curdef = next_entry(curdef);
	    }
					    /* Report rest of file.	    */
	    while (fgets(srcline, sizeof(srcline), srcfile) != NULL) {
		(*report_entry)(rptfile, NULL, srcline, ++line,
		    source_tabsize(curfile));
		total++;
	    }
					    /* Write trailer section.	    */
	    (*report_end)(rptfile, curfile, (totfiles == 0),
		isend_of_list(curfile));
	    totfiles++;
	    fclose(srcfile);
	    fclose(rptfile);
	}
    }
    if (list_enabled()) {
	fprintf(list_file(), "       %ld Lines in %d files\n",
	    total, totfiles);
    }
    if (log_full_enabled()) {
	printf("       %ld Lines in %d files\n\n", total, totfiles);
    }
}

