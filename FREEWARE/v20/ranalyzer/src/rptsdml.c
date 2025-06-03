/****************************************************************************/
/*									    */
/*  FACILITY:	Routine Analyzer					    */
/*									    */
/*  MODULE:	SDML Report Formatting Routines				    */
/*									    */
/*  AUTHOR:	Steve Branam, Network Product Support Group, Digital	    */
/*		Equipment Corporation, Littleton, MA, USA.		    */
/*									    */
/*  DESCRIPTION: This module contains the routines for generating Routine   */
/*  Analyzer reports in SDML format, suitable for input to VAX DOCUMENT.    */
/*									    */
/*  REVISION HISTORY:							    */
/*									    */
/*  V0.1-00 24-AUG-1994 Steve Branam					    */
/*									    */
/*	Original version.						    */
/*									    */
/****************************************************************************/

#include <stdio.h>
#include "ranalyzer.h"

#define MAX_VALID_BREAK	    60			/* Lines between breaks.    */

static int mLastValidBreak;			/* Last source break line.  */

/*************************************************************************++*/
void rpt_defined_sdml_hdr(
/* Writes SDML-formatted report header for defined routines.		    */

    FILE    *aRptFile
	    /* (READ, BY ADDR):						    */
	    /* Report output file. Must be opened by caller.		    */

)	/* No return value.						    */
	/*****************************************************************--*/

{
    fputs("<TABLE>(Defined Routines Alphabetical\\defined_routines_table)\n", aRptFile);
    fputs("<TABLE_ATTRIBUTES>(MULTIPAGE\\WIDE)\n", aRptFile);
    fputs("<TABLE_SETUP>(6\\20\\20\\7\\5\\5)\n", aRptFile);
    fputs(
"<TABLE_HEADS>(Routine\\Defined In File\\Line\\Length\\# Calls\\Times Called)\n",
	aRptFile);
}

/*************************************************************************++*/
void rpt_defined_sdml_entry(
/* Writes SDML-formatted routine entry for a defined routine.		    */

    FILE    *aRptFile,
	    /* (READ, BY ADDR):						    */
	    /* Report output file. Must be opened by caller.		    */

    DEFINITION
	    *aDef
	    /* (READ, BY ADDR):						    */
	    /* Routine definition entry to report.			    */
    
)	/* No return value.						    */
	/*****************************************************************--*/

{
    fprintf(aRptFile, "<TABLE_ROW>(%s\\%s\\%ld\\%ld\\%ld\\%ld)\n",
	def_name(aDef), source_name(def_source(aDef)), def_begin(aDef),
	def_length(aDef), def_num_calls(aDef), def_num_callers(aDef));
}

/*************************************************************************++*/
void rpt_defined_sdml_end(
/* Writes SDML-formatted report end for defined routines.		    */

    FILE    *aRptFile,
	    /* (READ, BY ADDR):						    */
	    /* Report output file. Must be opened by caller.		    */

    long    vTotalDef
	    /* (READ, BY VAL):						    */
	    /* Total number of defined routines.			    */

)	/* No return value.						    */
	/*****************************************************************--*/

{
    fputs("<TABLE_ROW>(<SPAN>(6)<RULE>)\n", aRptFile);
    fprintf(aRptFile, "<TABLE_ROW>(<EMPHASIS>(TOTAL ROUTINES: %ld\\BOLD)\\\n",
	vTotalDef);
    fprintf(aRptFile, "            <EMPHASIS>(TOTAL DEF LINES: %ld\\BOLD)\\\n",
	total_rlength());
    fprintf(aRptFile, "            <EMPHASIS>(AVG LEN:\\BOLD)\\\n");
    fprintf(aRptFile, "            <EMPHASIS>(%ld\\BOLD)\\\\)\n",
	total_avglen());
    fputs("<ENDTABLE>\n", aRptFile);
}

/*************************************************************************++*/
void rpt_undefined_sdml_hdr(
/* Writes SDML-formatted report header for undefined routines.		    */

    FILE    *aRptFile
	    /* (READ, BY ADDR):						    */
	    /* Report output file. Must be opened by caller.		    */

)	/* No return value.						    */
	/*****************************************************************--*/

{
    fputs("<TABLE>(External Routines Alphabetical\\external_routines_table)\n", aRptFile);
    fputs("<TABLE_ATTRIBUTES>(MULTIPAGE)\n", aRptFile);
    fputs("<TABLE_SETUP>(2\\20)\n", aRptFile);
    fputs("<TABLE_HEADS>(Routine\\Times Called)\n", aRptFile);
}

/*************************************************************************++*/
void rpt_undefined_sdml_entry(
/* Writes SDML-formatted routine entry for a undefined routine.		    */

    FILE    *aRptFile,
	    /* (READ, BY ADDR):						    */
	    /* Report output file. Must be opened by caller.		    */

    DEFINITION
	    *aDef
	    /* (READ, BY ADDR):						    */
	    /* Routine definition entry to report.			    */
    
)	/* No return value.						    */
	/*****************************************************************--*/

{
    fprintf(aRptFile, "<TABLE_ROW>(%s\\%ld)\n",
	def_name(aDef), def_num_callers(aDef));
}

/*************************************************************************++*/
void rpt_undefined_sdml_end(
/* Writes SDML-formatted report end for undefined routines.		    */

    FILE    *aRptFile,
	    /* (READ, BY ADDR):						    */
	    /* Report output file. Must be opened by caller.		    */

    long    vTotalUndef
	    /* (READ, BY VAL):						    */
	    /* Total number of undefined routines.			    */

)	/* No return value.						    */
	/*****************************************************************--*/

{
    fputs("<TABLE_ROW>(<SPAN>(2)<RULE>)\n", aRptFile);
    fprintf(aRptFile, "<TABLE_ROW>(<EMPHASIS>(TOTAL ROUTINES: %ld\\BOLD)\\)\n",
	vTotalUndef);
    fputs("<ENDTABLE>\n", aRptFile);
}

/*************************************************************************++*/
void rpt_calls_sdml_hdr(
/* Writes SDML-formatted report header for defined routine calls/callers    */
/* table.								    */

    FILE    *aRptFile,
	    /* (READ, BY ADDR):						    */
	    /* Report output file. Must be opened by caller.		    */

    DEFINITION
	    *aDef
	    /* (READ, BY ADDR):						    */
	    /* Routine definition entry to report.			    */
    
)	/* No return value.						    */
	/*****************************************************************--*/

{
    fprintf(aRptFile,
    "<TABLE>(%s Calls/Caller Routines)\n", def_ident(aDef));
    fputs("<TABLE_ATTRIBUTES>(MULTIPAGE)\n", aRptFile);
    fputs("<TABLE_SETUP>(4\\20\\5\\20)\n", aRptFile);
    fputs("<TABLE_HEADS>(Calls Routine\\Line\\Caller Routine\\Line)\n",
	aRptFile);
}

/*************************************************************************++*/
void rpt_calls_sdml_entry(
/* Writes SDML-formatted calls/caller entry for a defined routine.	    */

    FILE    *aRptFile,
	    /* (READ, BY ADDR):						    */
	    /* Report output file. Must be opened by caller.		    */

    REFERENCE
	    *aCalled,
	    /* (READ, BY ADDR):						    */
	    /* Called routine reference entry to report. If NULL is passed, */
	    /* only a caller is being reported.				    */
    
    REFERENCE
	    *aCaller
	    /* (READ, BY ADDR):						    */
	    /* Caller routine reference entry to report. If NULL is passed, */
	    /* only a called routine is being reported.			    */
    
)	/* No return value.						    */
	/*****************************************************************--*/

{
    fprintf(aRptFile, "<TABLE_ROW>(");
    if (aCalled == NULL) {
	fprintf(aRptFile, "\\\\");
    }
    else {
	fprintf(aRptFile, "%s\\%ld\\", def_name(ref_definition(aCalled)),
	    ref_offset(aCalled));
    }
    if (aCaller == NULL) {
	fprintf(aRptFile, "\\)\n");
    }
    else {
	fprintf(aRptFile, "%s\\%ld)\n", def_name(ref_caller(aCaller)),
	    ref_offset(aCaller));
    }
}

/*************************************************************************++*/
void rpt_calls_sdml_end(
/* Writes SDML-formatted report end for defined routine calls/caller	    */
/* table.								    */

    FILE    *aRptFile,
	    /* (READ, BY ADDR):						    */
	    /* Report output file. Must be opened by caller.		    */

    DEFINITION
	    *aDef
	    /* (READ, BY ADDR):						    */
	    /* Routine definition entry to report.			    */
    
)	/* No return value.						    */
	/*****************************************************************--*/

{
    fputs("<TABLE_ROW>(<SPAN>(4)<RULE>)\n", aRptFile);
    fprintf(aRptFile, "<TABLE_ROW>(<EMPHASIS>(TOTAL CALLS: %ld\\BOLD)\\\\\n",
	def_num_calls(aDef));
    fprintf(aRptFile, "            <EMPHASIS>(TOTAL CALLERS: %ld\\BOLD)\\)\n",
	def_num_callers(aDef));
    fputs("<ENDTABLE>\n\n", aRptFile);
}

/*************************************************************************++*/
void rpt_xref_sdml_hdr(
/* Writes SDML-formatted report header for cross reference.		    */

    FILE    *aRptFile
	    /* (READ, BY ADDR):						    */
	    /* Report output file. Must be opened by caller.		    */

)	/* No return value.						    */
	/*****************************************************************--*/

{
}

/*************************************************************************++*/
void rpt_xref_sdml_entry(
/* Writes SDML-formatted cross-reference sections for a routine.	    */

    FILE    *aRptFile,
	    /* (READ, BY ADDR):						    */
	    /* Report output file. Must be opened by caller.		    */

    DEFINITION
	    *aDef,
	    /* (READ, BY ADDR):						    */
	    /* Routine definition entry to report.			    */

    int	    vFirst
	    /* (READ, BY VAL):						    */
	    /* Flag indicating this is first entry being reported.	    */
    
)	/* No return value.						    */
	/*****************************************************************--*/

{
    REFERENCE				    /* Current caller ref.	    */
	    *caller;
    char    fullname[MAX_ROUTINE_IDENT + 1];/* Routine fullname buffer.	    */
					    /* Routine name folding buffer. */
    char    rname[RPT_HTML_ROUTINE_WIDE_LEN + 1]; 
    int	    rpos;			    /* Pos in routine name to print.*/

    fprintf(aRptFile, "<FIGURE>(%s Cross Reference)\n", def_ident(aDef));
#if 0
    fputs("<FIGURE_ATTRIBUTES>(KEEP\\WIDE)\n", aRptFile);
#endif    
    fputs("<FIGURE_ATTRIBUTES>(MULTIPAGE)", aRptFile);
    fputs("<LINE_ART>(WIDE)\n", aRptFile);
    fputs("CALLERS:", aRptFile);
    if (def_num_callers(aDef) == 0) {
	fputs(" No callers\n", aRptFile);
    }
    else {
	fprintf(aRptFile, "\n\n%3c%ld caller%s\n", ' ',
	    def_num_callers(aDef), (def_num_callers(aDef) == 1 ? "" : "s"));

					    /* Write entry for each caller. */
	for (caller = list_first(def_callers(aDef));
	    caller != NULL;
	    caller = next_entry(caller)) {
	    strcpy(fullname, def_ident(ref_caller(caller)));
	    
					    /* Get first part of rout name. */
	    rpos = fold_string(fullname, 0, NULL, rname,
		RPT_HTML_ROUTINE_WIDE_LEN);
	    fprintf(aRptFile, "%8c+ %-48s\n", ' ', rname);
	    while (rpos < strlen(fullname)) {/* Write rest of routine name.  */
		rpos = fold_string(fullname, rpos, "    ", rname,
		    RPT_HTML_ROUTINE_WIDE_LEN);
		fprintf(aRptFile, "%8c%s\n", ' ', rname);
	    }
	}
    }
    if (isdefined_routine(aDef)) {
	fputs("\nCALL TREE:", aRptFile);
	if (def_num_calls(aDef) == 0) {
	    fputs(" No calls\n", aRptFile);
	}
	else {
	    fputs("\n\n", aRptFile);
	}
    }
    if (def_num_calls(aDef) == 0) {
	fputs("<ENDLINE_ART>\n", aRptFile);
	fputs("<ENDFIGURE>\n\n", aRptFile);
    }
        
#if 0					    /* Write section header.	    */
    fprintf(aRptFile, "<TABLE>(%s Callers\\WIDE)\n", def_ident(aDef));
    fputs("<TABLE_SETUP>(2\\20)\n", aRptFile);
    fputs("<TABLE_HEADS>(Routine\\Callers)\n", aRptFile);
    if (def_num_callers(aDef) == 0) {
	fprintf(aRptFile, "<TABLE_ROW>(%s\\- No callers)\n", def_ident(aDef));
    }
    else {				    /* Write entry for each caller. */
	fprintf(aRptFile, "<TABLE_ROW>(%s\\<LIST>(UNNUMBERED)\n",
	    def_ident(aDef));
	for (caller = list_first(def_callers(aDef));
	    caller != NULL;
	    caller = next_entry(caller)) {
	    fprintf(aRptFile, "<LE>%s\n", def_ident(ref_caller(caller)));
	}
	fputs("<ENDLIST>)\n", aRptFile);
    }
    fputs("<ENDTABLE>\n", aRptFile);
#endif
}

/*************************************************************************++*/
void rpt_xref_sdml_end(
/* Writes SDML-formatted report end for caller cross-reference.		    */

    FILE    *aRptFile,
	    /* (READ, BY ADDR):						    */
	    /* Report output file. Must be opened by caller.		    */

    long    vTotalDef
	    /* (READ, BY VAL):						    */
	    /* Total number of routines.				    */

)	/* No return value.						    */
	/*****************************************************************--*/

{
}

/*************************************************************************++*/
void rpt_tree_sdml_hdr(
/* Writes SDML-formatted report header for defined routine call tree.	    */

    FILE    *aRptFile,
	    /* (READ, BY ADDR):						    */
	    /* Report output file. Must be opened by caller.		    */

    DEFINITION
	    *aDef
	    /* (READ, BY ADDR):						    */
	    /* Routine definition entry to report.			    */
    
)	/* No return value.						    */
	/*****************************************************************--*/

{
}

/*************************************************************************++*/
void rpt_tree_sdml_entry(
/* Writes SDML-formatted call tree line for a defined routine.		    */

    FILE    *aRptFile,
	    /* (READ, BY ADDR):						    */
	    /* Report output file. Must be opened by caller.		    */

    REFERENCE
	    *aRef,
	    /* (READ, BY ADDR):						    */
	    /* Reference to routine definition entry to report.       	    */

    int	    vLevel,
	    /* (READ, BY VAL):						    */
	    /* Nesting level, used to space indentation.		    */

    int	    vExpanded,
	    /* (READ, BY VAL):						    */
	    /* Flag indicating whether or not routine has already been	    */
	    /* expanded in this call tree.				    */

    int	    vRecursive
	    /* (READ, BY VAL):						    */
	    /* Flag indicating whether or not routine is called		    */
	    /* recursively.						    */

)	/* No return value.						    */
	/*****************************************************************--*/

{
    int	    lcount;			    /* Level print count.	    */

    fputs("   ", aRptFile);
    for (lcount = vLevel; lcount > 1; lcount--) {
	fputs("|   ", aRptFile);
    }
    if (lcount > 0) {
	if (isend_of_list(aRef)) {
	    fputs("+   ", aRptFile);
	}
	else {
	    fputs("|   ", aRptFile);
	}
    }
    if (vLevel == 0) {
	fprintf(aRptFile, "%s\n", def_name(ref_definition(aRef)));
    }
					    /* If routine is not going to   */
					    /* be expanded here, format it  */
					    /* in italics and add an	    */
    else if (vExpanded || vRecursive ||	    /* explanation code.	    */
	(needs_tree(ref_definition(aRef)) && !tree_inline_disabled()) ||
	!isdefined_routine(ref_definition(aRef))) {
	fprintf(aRptFile, "<EMPHASIS>(%s\\italic) (%s%s%s%s)\n",
	    def_name(ref_definition(aRef)),
	    (vExpanded ? "Duplicate" : ""),
	    (vRecursive ? "Recursive" : ""),
	    (needs_tree(ref_definition(aRef)) && !vRecursive && !vExpanded
		&& !tree_inline_disabled() ? "Separate" : ""),
	    (!isdefined_routine(ref_definition(aRef)) ? "External" : ""));
    }
    else if (vLevel == max_tree_depth()	    /* Reached expansion limit?	    */
	&& def_num_calls(ref_definition(aRef)) > 0) {
	fprintf(aRptFile, "<EMPHASIS>(%s\\italic) (Separate)\n",
	    def_name(ref_definition(aRef)));
    }
    else {				    /* Otherwise, format plain.	    */
	fprintf(aRptFile, "%s\n", def_name(ref_definition(aRef)));
    }
}

/*************************************************************************++*/
void rpt_tree_sdml_end(
/* Writes SDML-formatted section end for defined routine call tree.	    */

    FILE    *aRptFile,
	    /* (READ, BY ADDR):						    */
	    /* Report output file. Must be opened by caller.		    */

    DEFINITION
	    *aDef
	    /* (READ, BY ADDR):						    */
	    /* Routine definition entry to report.			    */
    
)	/* No return value.						    */
	/*****************************************************************--*/

{
    fputs("   END OF TREE\n", aRptFile);
    fputs("<ENDLINE_ART>\n", aRptFile);
    fputs("<ENDFIGURE>\n\n", aRptFile);
}

/*************************************************************************++*/
void rpt_byfile_sdml_hdr(
/* Writes SDML-formatted report header for defined routines by file table.  */

    FILE    *aRptFile,
	    /* (READ, BY ADDR):						    */
	    /* Report output file. Must be opened by caller.		    */

    SOURCEFILE
	    *aSourceFile
	    /* (READ, BY ADDR):						    */
	    /* Source file entry to report.				    */
    
)	/* No return value.						    */
	/*****************************************************************--*/

{
    fprintf(aRptFile, "<TABLE>(%s Routines)\n", source_name(aSourceFile));
    fputs("<TABLE_ATTRIBUTES>(MULTIPAGE)\n", aRptFile);
    fputs("<TABLE_SETUP>(5\\20\\7\\5\\5)\n", aRptFile);
    fputs("<TABLE_HEADS>(Routine\\Line\\Length\\# Calls\\Times Called)\n",
	aRptFile);
}

/*************************************************************************++*/
void rpt_byfile_sdml_entry(
/* Writes SDML-formatted entry for a defined routine by file.		    */

    FILE    *aRptFile,
	    /* (READ, BY ADDR):						    */
	    /* Report output file. Must be opened by caller.		    */

    DEFINITION
	    *aDef
	    /* (READ, BY ADDR):						    */
	    /* Routine definition entry to report.			    */
    
)	/* No return value.						    */
	/*****************************************************************--*/

{
    fprintf(aRptFile, "<TABLE_ROW>(%s\\%ld\\%ld\\%ld\\%ld)\n", def_name(aDef),
	def_begin(aDef), def_length(aDef), def_num_calls(aDef),
	def_num_callers(aDef));
}

/*************************************************************************++*/
void rpt_byfile_sdml_end(
/* Writes SDML-formatted report end for defined routine by file table.	    */

    FILE    *aRptFile,
	    /* (READ, BY ADDR):						    */
	    /* Report output file. Must be opened by caller.		    */

    SOURCEFILE
	    *aSourceFile
	    /* (READ, BY ADDR):						    */
	    /* Source file entry to report.				    */
    
)	/* No return value.						    */
	/*****************************************************************--*/

{
    fputs("<TABLE_ROW>(<SPAN>(5)<RULE>)\n", aRptFile);
    fprintf(aRptFile, "<TABLE_ROW>(<EMPHASIS>(TOTAL: %ld ROUTINES\\BOLD)\\\\\n",
	source_routines(aSourceFile));
    fprintf(aRptFile, "            <EMPHASIS>(%ld AVG\\BOLD)\\\\)\n",
	source_avglen(aSourceFile));
    fputs("<ENDTABLE>\n\n", aRptFile);
}

/*************************************************************************++*/
void rpt_file_sdml_hdr(
/* Writes SDML-formatted report header for source files.		    */

    FILE    *aRptFile
	    /* (READ, BY ADDR):						    */
	    /* Report output file. Must be opened by caller.		    */

)	/* No return value.						    */
	/*****************************************************************--*/

{
    fputs("<TABLE>(Source Files Alphabetical\\source_files_table)\n", aRptFile);
    fputs("<TABLE_ATTRIBUTES>(MULTIPAGE\\WIDE)\n", aRptFile);
    fputs("<TABLE_SETUP>(8\\20\\6\\5\\6\\4\\6\\4)\n", aRptFile);
    fputs(
"<TABLE_HEADS>(File\\Lines\\Com- mented\\State- ment\\Rou- tines\\Length\\Avg Len\\# Calls)\n",
	aRptFile);
}

/*************************************************************************++*/
void rpt_file_sdml_entry(
/* Writes SDML-formatted source file entry.				    */

    FILE    *aRptFile,
	    /* (READ, BY ADDR):						    */
	    /* Report output file. Must be opened by caller.		    */

    SOURCEFILE
	    *aSourceFile
	    /* (READ, BY ADDR):						    */
	    /* Source file entry to report.				    */
    
)	/* No return value.						    */
	/*****************************************************************--*/

{
    fprintf(aRptFile, "<TABLE_ROW>(%s\\%ld\\%ld\\%ld\\%ld\\%ld\\%ld\\%ld)\n",
	source_name(aSourceFile), source_lines(aSourceFile),
	source_comments(aSourceFile) + source_mixed(aSourceFile),
	source_statements(aSourceFile) + source_mixed(aSourceFile),
	source_routines(aSourceFile), source_rlength(aSourceFile),
	source_avglen(aSourceFile), source_calls(aSourceFile));
}

/*************************************************************************++*/
void rpt_file_sdml_end(
/* Writes SDML-formatted report end for source files.			    */

    FILE    *aRptFile,
	    /* (READ, BY ADDR):						    */
	    /* Report output file. Must be opened by caller.		    */

    long    vTotalFiles
	    /* (READ, BY VAL):						    */
	    /* Total number of source files.				    */

)	/* No return value.						    */
	/*****************************************************************--*/

{
    fputs("<TABLE_ROW>(<SPAN>(8)<RULE>)\n", aRptFile);
    fprintf(aRptFile, "<TABLE_ROW>(<EMPHASIS>(TOTAL: %ld files\\BOLD)\\\n",
	vTotalFiles);
    fprintf(aRptFile, "            <EMPHASIS>(%ld\\BOLD)\\\n",
	total_lines());
    fprintf(aRptFile, "            <EMPHASIS>(%ld\\BOLD)\\\n",
	total_comments() + total_mixed());
    fprintf(aRptFile, "            <EMPHASIS>(%ld\\BOLD)\\\n",
	total_statements() + total_mixed());
    fprintf(aRptFile, "            <EMPHASIS>(%ld\\BOLD)\\\n",
	total_routines());
    fprintf(aRptFile, "            <EMPHASIS>(%ld\\BOLD)\\\n",
	total_rlength());
    fprintf(aRptFile, "            <EMPHASIS>(%ld\\BOLD)\\\n",
	total_avglen());
    fprintf(aRptFile, "            <EMPHASIS>(%ld\\BOLD))\n",
	total_calls());
    fputs("<ENDTABLE>\n", aRptFile);
}

/*************************************************************************++*/
void rpt_source_sdml_hdr(
/* Writes SDML-formatted report header for annotated source file.	    */

    FILE    *aRptFile,
	    /* (READ, BY ADDR):						    */
	    /* Report output file. Must be opened by caller.		    */

    SOURCEFILE
	    *aSourceFile,
	    /* (READ, BY ADDR):						    */
	    /* Source file entry to report.				    */

    int	    vFirst,
	    /* (READ, BY VAL):						    */
	    /* Flag indicating whether this is first table.		    */

    int	    vLast
	    /* (READ, BY VAL):						    */
	    /* Flag indicating whether this is last table.		    */
    
)	/* No return value.						    */
	/*****************************************************************--*/

{
    REFERENCE				    /* Current routine reference.   */
	    *curref;

    mLastValidBreak = 0;
    fprintf(aRptFile, "<DEFINE_SYMBOL>(source_%d_name\\%s)\n",
	source_seq(aSourceFile), source_name(aSourceFile));
    fprintf(aRptFile, "<FIGURE>(%s Source Code\\source_%d_figure)\n",
	source_name(aSourceFile), source_seq(aSourceFile));
    fputs("<FIGURE_ATTRIBUTES>(MULTIPAGE\\WIDE)\n", aRptFile);
    fputs("<LINE_ART>(WIDE)\n", aRptFile);
    fputs("ROUTINES IN THIS FILE (Alphabetical)\n\n", aRptFile);
    if (source_routines(aSourceFile) == 0) {
	fputs("   None.\n", aRptFile);
    }
    else {
	fputs(" Line Name\n----- ----\n", aRptFile);
	for (curref = list_first(source_reflist(aSourceFile));
	    curref != NULL;
	    curref = next_entry(curref)) {
	    fprintf(aRptFile, "%5d %s\n",
		def_begin(ref_definition(curref)),
		def_name(ref_definition(curref)));
	}
    }
    
    fputs("\nBEGINNING OF FILE\n\n", aRptFile);
}

/*************************************************************************++*/
void rpt_source_sdml_entry(
/* Writes SDML-formatted entry for an annotated source line.		    */

    FILE    *aRptFile,
	    /* (READ, BY ADDR):						    */
	    /* Report output file. Must be opened by caller.		    */

    DEFINITION
	    *aDef,
	    /* (READ, BY ADDR):						    */
	    /* Routine definition entry to report.			    */

    char    *aSrcLine,
	    /* (READ, BY ADDR):						    */
	    /* Source file line contents.				    */

    int	    vLine,
	    /* (READ, BY VAL):						    */
	    /* Source file line number.					    */
    
    int	    vTabSize
	    /* (READ, BY VAL):						    */
	    /* Source text tab size.					    */
    
)	/* No return value.						    */
	/*****************************************************************--*/

{
    DEFINITION				    /* Next definition in file.	    */
	    *nextdef;
    int	    column;			    /* Output column.		    */
    int	    count;			    /* Tab expansion counter.	    */
	    
    if (aDef != NULL && def_begin(aDef) == vLine) {
	fputs("<VALID_BREAK>\n", aRptFile);
	mLastValidBreak = vLine;
	fprintf(aRptFile, "\nBEGIN ROUTINE %s.\n\n", def_name(aDef));
    }
    fprintf(aRptFile, "%6d: ", vLine);
					    /* Expand tabs on output since  */
					    /* SDML treats TAB as a space.  */
    for (column = 0; *aSrcLine != '\0'; aSrcLine++) {
	if (*aSrcLine == '\t') {
	    for (count = vTabSize - column % vTabSize;
		count > 0;
		count--, column++) {
		fputc(' ', aRptFile);
	    }
	}
	else if (*aSrcLine == '\f') {
	    fputs("\n<VALID_BREAK>", aRptFile);
	    mLastValidBreak = vLine;
	}
	else {
	    switch (*aSrcLine) {
	    case '>': fputs("<LITERAL>(>)", aRptFile); break;
	    case '<': fputs("<LITERAL>(<)", aRptFile); break;
	    default: fputc(*aSrcLine, aRptFile);
	    }
	    column++;
	}
    }
    if (aDef != NULL && def_end(aDef) == vLine) {
	fprintf(aRptFile, "\nEND %s.\n\n", def_name(aDef));
    }
    else if (vLine - mLastValidBreak > MAX_VALID_BREAK) {
	fputs("<VALID_BREAK>\n", aRptFile);
	mLastValidBreak = vLine;
    }
}

/*************************************************************************++*/
void rpt_source_sdml_end(
/* Writes SDML-formatted report end for annotated source file.		    */

    FILE    *aRptFile,
	    /* (READ, BY ADDR):						    */
	    /* Report output file. Must be opened by caller.		    */

    SOURCEFILE
	    *aSourceFile,
	    /* (READ, BY ADDR):						    */
	    /* Source file entry to report.				    */
    
    int	    vFirst,
	    /* (READ, BY VAL):						    */
	    /* Flag indicating whether this is first table.		    */

    int	    vLast
	    /* (READ, BY VAL):						    */
	    /* Flag indicating whether this is last table.		    */
    
)	/* No return value.						    */
	/*****************************************************************--*/

{
    fputs("\nEND OF FILE\n\n", aRptFile);
    fprintf(aRptFile, "TOTAL: %ld routines, %ld Avg Length\n",
	source_routines(aSourceFile), source_avglen(aSourceFile));
    fputs("<ENDLINE_ART>\n", aRptFile);
    fputs("<ENDFIGURE>\n\n", aRptFile);
}

