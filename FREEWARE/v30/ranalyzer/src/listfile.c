/****************************************************************************/
/*									    */
/*  FACILITY:	Routine Analyzer					    */
/*									    */
/*  MODULE:	List File Management					    */
/*									    */
/*  AUTHOR:	Steve Branam, Network Product Support Group, Digital	    */
/*		Equipment Corporation, Littleton, MA, USA.		    */
/*									    */
/*  DESCRIPTION: This module contains list file management routines	    */
/*  for Routine Analyzer.						    */
/*									    */
/*  REVISION HISTORY:							    */
/*									    */
/*  V0.1-00 24-AUG-1994 Steve Branam					    */
/*									    */
/*	Original version.						    */
/*									    */
/****************************************************************************/

#include "ranalyzer.h"

/*************************************************************************++*/
void new_list_line(
/* Starts a new line in the list file.					    */

    SOURCEFILE
	    *aSourceRecord
		/* (READ, BY ADDR):					    */
		/* Source file information record.			    */

)	/* No return value.						    */
	/*****************************************************************--*/

{
    if (list_enabled()) {
	fprintf(list_file(), LIST_LINE_NUMBER, source_line(aSourceRecord));
    }
}

/*************************************************************************++*/
void list_char(
/* Writes a character to the listing file and maintains the line character  */
/* count.								    */

    char    vChar
	    /* (READ, BY VAL):						    */
	    /* Character to list.					    */

)	/* No return value.						    */
	/*****************************************************************--*/

{
    /*+									    */
    /*	If a listing file is being written, write the character to it. If   */
    /*	it is a tab, compute the next tab position. If it is a newline,	    */
    /*	reset the column position. Otherwise, just increment the column.    */
    /*-									    */

    if (list_enabled()) {
	fputc(vChar, list_file());
	if (vChar == '\t') {
	    set_list_column(((list_column() / TAB_SIZE) + 1) * TAB_SIZE);
	}
	else if (vChar == '\n') {
	    set_list_column(0);
	}
	else {
	    inc_list_column();
	}
    }
}

/*************************************************************************++*/
void restore_list_column(
/* Prints blanks out to the current listing column, restoring the column    */
/* position after a message has been printed.				    */

    /* No arguments.							    */

)	/* No return value.						    */
	/*****************************************************************--*/

{
    int	    count;			    /* Character counter.	    */

    /*+									    */
    /*	Write the listing file line number spacing margin. Next write a	    */
    /*	sufficient number of tabs to position it before the current column  */
    /*	position. Then write the remaining number of blanks to go from the  */
    /*	tab stop to the column position.				    */
    /*-									    */

    fprintf(list_file(), LIST_LINE_SPACER, ' ');
    for (count = list_column() / TAB_SIZE; count > 0; count--) {
	fputc('\t', list_file());
    }
    for (count = list_column() % TAB_SIZE; count > 0; count--) {
	fputc(' ', list_file());
    }
}

/*************************************************************************++*/
void list_file_begin(
/* Writes status messages indicating that analysis of a source file is	    */
/* beginning.								    */

    SOURCEFILE	*aSourceFile,
		/* (READ, BY ADDR):					    */
		/* Source file information record.			    */

    char	*aLangName
		/* (READ, BY ADDR):					    */
		/* Source file language name.				    */

)	/* No return value.						    */
	/*****************************************************************--*/

{
    if (list_enabled()) {
	fprintf(list_file(), "FILE %s analysis started, language is %s\n",
	    source_name(aSourceFile), aLangName);
    }
    if (log_enabled()) {
	printf("FILE %s analysis started, language is %s\n",
	    source_name(aSourceFile), aLangName);
    }
}

/*************************************************************************++*/
void list_file_end(
/* Writes status messages indicating that analysis of a source file is	    */
/* complete.								    */

    SOURCEFILE	*aSourceFile
		/* (READ, BY ADDR):					    */
		/* Source file information record.			    */

)	/* No return value.						    */
	/*****************************************************************--*/

{
    if (list_enabled()) {
	fprintf(list_file(), "\nFILE %s analysis complete\n",
	    source_name(aSourceFile));
	fprintf(list_file(),
	    "     %ld lines, %ld routines (avg len %ld), %ld calls\n\n",
	    source_lines(aSourceFile), source_routines(aSourceFile),
	    source_avglen(aSourceFile), source_calls(aSourceFile));
    }
    if (log_full_enabled()) {
	printf("FILE %s analysis complete\n", source_name(aSourceFile));
	printf("     %ld lines, %ld routines (avg len %ld), %ld calls\n\n",
	    source_lines(aSourceFile), source_routines(aSourceFile),
	    source_avglen(aSourceFile), source_calls(aSourceFile));
    }
}

/*************************************************************************++*/
void list_product_begin(
/* Writes status messages indicating that analysis of a product file is	    */
/* beginning.								    */

    char    *aProductName
		/* (READ, BY ADDR):					    */
		/* Product file name string.				    */

)	/* No return value.						    */
	/*****************************************************************--*/

{
    if (list_enabled()) {
	fprintf(list_file(), "%s %s\n", PROGRAM_IDENT, PROGRAM_COPYRIGHT);
	fprintf(list_file(), "\nPRODUCT %s analysis started\n\n", aProductName);
    }
    if (log_enabled()) {
	printf("%s %s\n", PROGRAM_IDENT, PROGRAM_COPYRIGHT);
        printf("PRODUCT %s\n", product_name());
	printf("        (in file %s) analysis started\n\n", aProductName);
    }
}

/*************************************************************************++*/
void list_analysis_complete(
/* Writes status messages indicating that analysis of all source files is   */
/* complete.								    */

    char    *aProductName
		/* (READ, BY ADDR):					    */
		/* Product file name string.				    */

)	/* No return value.						    */
	/*****************************************************************--*/

{
    if (list_enabled()) {
	fprintf(list_file(), "\nPRODUCT %s all source file analysis complete\n",
	    aProductName);
	fprintf(list_file(),
    "        %ld files, %ld lines, %ld routines (avg len %ld), %ld calls\n\n",
	    total_files(), total_lines(), total_routines(), total_avglen(),
	    total_calls());
    }
    if (log_full_enabled()) {
	printf("\nPRODUCT %s all source file analysis complete\n",
	    aProductName);
	printf(
    "        %ld files, %ld lines, %ld routines (avg len %ld), %ld calls\n\n",
	    total_files(), total_lines(), total_routines(), total_avglen(),
	    total_calls());
    }
}

/*************************************************************************++*/
void list_product_end(
/* Writes status messages indicating that analysis and reporting of a	    */
/* product is complete.							    */

    char    *aProductName
		/* (READ, BY ADDR):					    */
		/* Product file name string.				    */

)	/* No return value.						    */
	/*****************************************************************--*/

{
    if (list_enabled()) {
	fprintf(list_file(),
	    "\nPRODUCT %s all analysis and reporting complete\n", aProductName);
    }
    if (log_full_enabled()) {
	printf("PRODUCT %s all analysis and reporting complete\n",
	aProductName);
    }
}

/*************************************************************************++*/
void list_def_begin(
/* Writes status messages indicating that the beginning of a routine	    */
/* definition has been found.						    */

    SOURCEFILE	*aSourceFile,
		/* (READ, BY ADDR):					    */
		/* Source file information record.			    */

    DEFINITION	*aRoutineDef
		/* (READ, BY ADDR):					    */
		/* Routine definition record.				    */

)	/* No return value.						    */
	/*****************************************************************--*/

{
    if (list_enabled()) {
	fprintf(list_file(), "\nBEGIN %s in %s at line %ld/%ld\n",
	    def_name(aRoutineDef), source_name(aSourceFile),
	    def_begin(aRoutineDef), source_line(aSourceFile));
	restore_list_column();
    }
    if (log_def_enabled()) {
	printf("BEGIN %s in %s at line %ld\n", def_name(aRoutineDef),
	    source_name(aSourceFile), source_line(aSourceFile));
    }
}

/*************************************************************************++*/
void list_def_end(
/* Writes status messages indicating that the end of a routine definition   */
/* has been found.							    */

    SOURCEFILE	*aSourceFile,
		/* (READ, BY ADDR):					    */
		/* Source file information record.			    */

    DEFINITION	*aRoutineDef
		/* (READ, BY ADDR):					    */
		/* Routine definition record.				    */

)	/* No return value.						    */
	/*****************************************************************--*/

{
    if (list_enabled()) {
	fprintf(list_file(),
	    "\nEND %s in %s at line %ld\n    %ld lines, %ld calls\n",
	    def_name(aRoutineDef), source_name(aSourceFile),
	    source_line(aSourceFile), def_length(aRoutineDef),
	    def_num_calls(aRoutineDef));
	restore_list_column();
    }
    if (log_def_enabled() && !log_brief_enabled()) {
	printf("END   %s in %s at line %ld\n      %ld lines, %ld calls\n\n",
	    def_name(aRoutineDef), source_name(aSourceFile),
	    source_line(aSourceFile), def_length(aRoutineDef),
	    def_num_calls(aRoutineDef));
    }
}

/*************************************************************************++*/
void list_ref(
/* Writes status messages indicating that a routine reference (call) has    */
/* been found.								    */

    SOURCEFILE	*aSourceFile,
		/* (READ, BY ADDR):					    */
		/* Source file information record.			    */

    REFERENCE	*aRoutineRef
		/* (READ, BY ADDR):					    */
		/* Routine reference record.				    */

)	/* No return value.						    */
	/*****************************************************************--*/

{
    if (list_enabled()) {
	fprintf(list_file(), "\nCALL %s in %s at line %ld, line %ld of %s\n",
	    ref_name(aRoutineRef), source_name(aSourceFile),
	    source_line(aSourceFile), ref_offset(aRoutineRef),
	    def_name(ref_caller(aRoutineRef)));
	restore_list_column();
    }
    if (log_ref_enabled()) {
	printf("CALL  %s in %s at line %ld, line %ld of %s\n",
	    ref_name(aRoutineRef), source_name(aSourceFile),
	    source_line(aSourceFile), ref_offset(aRoutineRef),
	    def_name(ref_caller(aRoutineRef)));
    }
}

