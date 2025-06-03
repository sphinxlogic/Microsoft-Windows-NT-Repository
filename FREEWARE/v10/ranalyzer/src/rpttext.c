/****************************************************************************/
/*									    */
/*  FACILITY:	Routine Analyzer					    */
/*									    */
/*  MODULE:	Text Report Formatting Routines				    */
/*									    */
/*  AUTHOR:	Steve Branam, Network Product Support Group, Digital	    */
/*		Equipment Corporation, Littleton, MA, USA.		    */
/*									    */
/*  DESCRIPTION: This module contains the routines for generating Routine   */
/*  Analyzer reports in plain text format.				    */
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

/*************************************************************************++*/
void rpt_defined_text_hdr(
/* Writes plain text formatted report header for defined routines.	    */

    FILE    *aRptFile
	    /* (READ, BY ADDR):						    */
	    /* Report output file. Must be opened by caller.		    */

)	/* No return value.						    */
	/*****************************************************************--*/

{
    fputs("Defined Routines Alphabetical\n", aRptFile);
    fputs("-----------------------------\n", aRptFile);
    fputs(
"Routine                   Defined In File      Line   Length Called Calls\n",
	aRptFile);
    fputs(
"------------------------- -------------------- ------ ------ ------ ------\n",
	aRptFile);
}

/*************************************************************************++*/
void rpt_defined_text_entry(
/* Writes plain text formatted routine entry for a defined routine.	    */

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
    fprintf(aRptFile, "%25s %20s %6ld %6ld %6ld %6ld\n",
	def_name(aDef), source_name(def_source(aDef)), def_begin(aDef),
	def_length(aDef), def_num_calls(aDef), def_num_callers(aDef));
}

/*************************************************************************++*/
void rpt_defined_text_end(
/* Writes plain text formatted report end for defined routines.		    */

    FILE    *aRptFile,
	    /* (READ, BY ADDR):						    */
	    /* Report output file. Must be opened by caller.		    */

    long    vTotalDef
	    /* (READ, BY VAL):						    */
	    /* Total number of defined routines.			    */

)	/* No return value.						    */
	/*****************************************************************--*/

{
    fputs("\n", aRptFile);
    fprintf(aRptFile,
	"TOTAL ROUTINES: %6ld   TOTAL DEF LINES: %6ld AVG LENGTH: %6ld\n",
	vTotalDef, total_rlength(), total_avglen());
}

/*************************************************************************++*/
void rpt_undefined_text_hdr(
/* Writes plain text formatted report header for undefined routines.	    */

    FILE    *aRptFile
	    /* (READ, BY ADDR):						    */
	    /* Report output file. Must be opened by caller.		    */

)	/* No return value.						    */
	/*****************************************************************--*/

{
    fputs("Undefined Routines Alphabetical\n", aRptFile);
    fputs("-------------------------------\n", aRptFile);
    fputs("Routine                   Called\n", aRptFile);
    fputs("------------------------- ------\n", aRptFile);
}

/*************************************************************************++*/
void rpt_undefined_text_entry(
/* Writes plain text formatted routine entry for a undefined routine.	    */

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
    fprintf(aRptFile, "%25s %6ld\n", def_name(aDef), def_num_callers(aDef));
}

/*************************************************************************++*/
void rpt_undefined_text_end(
/* Writes plain text formatted report end for undefined routines.	    */

    FILE    *aRptFile,
	    /* (READ, BY ADDR):						    */
	    /* Report output file. Must be opened by caller.		    */

    long    vTotalUndef
	    /* (READ, BY VAL):						    */
	    /* Total number of undefined routines.			    */

)	/* No return value.						    */
	/*****************************************************************--*/

{
    fputs("\n", aRptFile);
    fprintf(aRptFile, "TOTAL ROUTINES: %6ld\n", vTotalUndef);
}

/*************************************************************************++*/
void rpt_calls_text_hdr(
/* Writes plain text formatted report header for defined routine	    */
/* calls/callers table.							    */

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
    fprintf(aRptFile, "%s Calls/Caller Routines\n", def_name(aDef));
    fputs("------------------------\n", aRptFile);
    fputs("Calls Routine             Line   Caller Routine       Line\n",
	aRptFile);
    fputs("------------------------- ------ -------------------- ------\n",
	aRptFile);
}

/*************************************************************************++*/
void rpt_calls_text_entry(
/* Writes plain text formatted routine entry for a defined routine	    */
/* calls/caller entry.							    */

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
    if (aCalled == NULL) {
	fprintf(aRptFile, "%25c %6c ", ' ', ' ');
    }
    else {
	fprintf(aRptFile, "%25s %6ld ", def_name(ref_definition(aCalled)),
	    ref_offset(aCalled));
    }
    if (aCaller == NULL) {
	fprintf(aRptFile, "\n");
    }
    else {
	fprintf(aRptFile, "%25s %6ld\n", def_name(ref_caller(aCaller)),
	    ref_offset(aCaller));
    }
}

/*************************************************************************++*/
void rpt_calls_text_end(
/* Writes plain text formatted report end for defined routine calls/caller  */
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
    fputs("\n", aRptFile);
    fprintf(aRptFile, "TOTAL CALLS: %6ld      ", def_num_calls(aDef));
    fprintf(aRptFile, "TOTAL CALLERS: %6ld\n\n", def_num_callers(aDef));
}

/*************************************************************************++*/
void rpt_xref_text_hdr(
/* Writes plain text-formatted report header for cross reference.	    */

    FILE    *aRptFile
	    /* (READ, BY ADDR):						    */
	    /* Report output file. Must be opened by caller.		    */

)	/* No return value.						    */
	/*****************************************************************--*/

{
    fputs("Caller Cross-Reference\n", aRptFile);
    fputs("----------------------\n", aRptFile);
    fputs("Routine                     Callers\n", aRptFile);
    fputs("--------------------------- -----------------------\n", aRptFile);
}

/*************************************************************************++*/
void rpt_xref_text_entry(
/* Writes plain text-formatted cross-reference sections for a routine.	    */

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
    REFERENCE				    /* Current caller ref.	    */
	    *caller;
					    /* Write section header.	    */
    fprintf(aRptFile, "%25s  Total callers: %ld\n", def_name(aDef),
	def_num_callers(aDef));

					    /* Write entry for each caller. */
    for (caller = list_first(def_callers(aDef));
	caller != NULL;
	caller = next_entry(caller)) {
	fprintf(aRptFile, "%25c %s\n", ' ', def_name(ref_caller(caller)));
    }
}

/*************************************************************************++*/
void rpt_xref_text_end(
/* Writes plain text-formatted report end for caller cross-reference.	    */

    FILE    *aRptFile,
	    /* (READ, BY ADDR):						    */
	    /* Report output file. Must be opened by caller.		    */

    long    vTotalDef
	    /* (READ, BY VAL):						    */
	    /* Total number of routines.				    */

)	/* No return value.						    */
	/*****************************************************************--*/

{
    fprintf(aRptFile, "\nTOTAL ROUTINES: %ld\n", vTotalDef);
}

/*************************************************************************++*/
void rpt_tree_text_hdr(
/* Writes plain text formatted report header for defined routine call tree. */

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
    fprintf(aRptFile, "%25s - Call Tree\n", def_ident(aDef));
    fprintf(aRptFile, "%25c - %25c - ---------\n", '-', '-');
}

/*************************************************************************++*/
void rpt_tree_text_entry(
/* Writes plain text formatted call tree line for a defined routine.	    */

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
	fprintf(aRptFile, "%s: %ld caller%s\n", def_name(ref_definition(aRef)),
	    def_num_callers(ref_definition(aRef)),
	    (def_num_callers(ref_definition(aRef)) == 1 ? "" : "s"));
    }
    else {
	fprintf(aRptFile, "%s%s%s%s%s\n", def_name(ref_definition(aRef)),
	    (needs_tree(ref_definition(aRef)) &&
		!tree_inline_disabled() ? " (Separate)" : ""),
	    (vExpanded ? " (Duplicate)" : ""),
	    (vRecursive ? " (Recursive)" : ""),
	    (isdefined_routine(ref_definition(aRef)) ? "" : " (External)"));
    }
}

/*************************************************************************++*/
void rpt_tree_text_end(
/* Writes plain text formatted section end for defined routine call tree.   */

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
    fputs("END OF TREE\n", aRptFile);
}
