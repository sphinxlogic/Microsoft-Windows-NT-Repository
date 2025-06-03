/****************************************************************************/
/*									    */
/*  FACILITY:	Routine Analyzer					    */
/*									    */
/*  MODULE:	Text Language "Parser"					    */
/*									    */
/*  AUTHOR:	Steve Branam, Network Product Support Group, Digital	    */
/*		Equipment Corporation, Littleton, MA, USA.		    */
/*									    */
/*  DESCRIPTION: This module contains the dummy "parser" for text files. No */
/*  actual parsing is done, since there is no language structure, but the   */
/*  normal source file management takes place.				    */
/*									    */
/*  REVISION HISTORY:							    */
/*									    */
/*  V0.1-00 27-SEP-1994 Steve Branam					    */
/*									    */
/*	Original version.						    */
/*									    */
/****************************************************************************/

#include <stdio.h>
#include <ctype.h>
#include "ranalyzer.h"
#include "parser.h"

/*************************************************************************++*/
language_element text_parser(
/* "Parses" text source language statements. This is just a dummy parser,   */
/* it does not recognize any actual syntax.				    */

    FILE    *aSourceFile,
		/* (READ, BY ADDR):					    */
		/* Source file containing text. Must be opened by caller.   */

    SOURCEFILE
	    *aSourceRecord,
		/* (READ, BY ADDR):					    */
		/* Source file information record.			    */

    char    *aElement,
		/* (WRITE, BY ADDR):					    */
		/* String buffer that will receive the recognized source    */
		/* language element.					    */

    long    *aSourceLine
		/* (WRITE, BY ADDR):					    */
		/* Buffer that will receive the line number of aElement.    */

)	/* Returns one of the following values indicating the type of	    */
	/* element output in aElement:					    */
	/*      PARSE_ERROR	    - An error was detected in the input    */
	/*			      stream.				    */
	/*	END_OF_SOURCE	    - The normal end of file was found.	    */
	/*****************************************************************--*/

{
    int	    ch;				    /* Input character.		    */

    do {
	ch = fgetc(aSourceFile);
	list_char(ch);
	if (ch == '\n') {
	    inc_source_statements(aSourceRecord);
	    new_list_line(aSourceRecord);
	}
    } while (ch != EOF);
    return END_OF_SOURCE;
}

