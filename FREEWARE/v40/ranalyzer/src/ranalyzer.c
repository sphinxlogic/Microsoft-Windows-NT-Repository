/****************************************************************************/
/*									    */
/*  FACILITY:	Routine Analyzer					    */
/*									    */
/*  MODULE:	Main Module						    */
/*									    */
/*  AUTHOR:	Steve Branam, Network Product Support Group, Digital	    */
/*		Equipment Corporation, Littleton, MA, USA.		    */
/*									    */
/*  DESCRIPTION: This is the main module for Routine Analyzer. It contains  */
/*  the main routine, command option handlers, and the main application	    */
/*  routines for processing product and source files.			    */
/*									    */
/*  REVISION HISTORY:							    */
/*									    */
/*  V1.0-00 27-JAN-1995 Steve Branam					    */
/*									    */
/*	Modified help logic when insufficient arguments supplied and	    */
/*	cleaned up help.						    */
/*									    */
/*  V0.1-00 24-AUG-1994 Steve Branam					    */
/*									    */
/*	Original version.						    */
/*									    */
/****************************************************************************/

#define MAIN_MODULE			    /* This is the main module.	    */
#include <ctype.h>
#include "ranalyzer.h"


extern language_element c_parser();
extern language_element bliss_parser();
extern language_element dcl_parser();
extern language_element text_parser();

/*************************************************************************++*/
PARSER get_parser(
/* Returns the parser function appropriate for the source language, based   */
/* on the file name extension.						    */

    char    *aSourceName,
	    /* (READ, BY ADDR):  					    */
	    /* Source file name string.					    */

    char    **aParserName
	    /* (WRITE, BY ADDR):  					    */
	    /* Parser name string ptr, set to parser name string.	    */

)	/* Returns ptr to parser function.				    */
	/*****************************************************************--*/

{
    KEYWORD_DEFINITION			    /* Current keyword definition.  */
	    *curkwdef;
    LANGUAGE_TRANSLATION		    /* Current language trans.	    */
	    *curtrans;
    char    *extstr;			    /* File extension ptr.	    */

    if (global_langtable() == NULL) {
	set_lang_table(list_entries(global_langlist()));
	for (curkwdef = global_langtable();
	    (curtrans = remove_lang()) != NULL;
	    curkwdef++) {
	    set_kwdef_keyword(curkwdef, lang_fext(curtrans));
	    set_kwdef_minlen(curkwdef, strlen(lang_fext(curtrans)));
	    set_kwdef_code(curkwdef, lang_code(curtrans));
	    free_lang(curtrans);
	}
    }


    /*+									    */
    /*	Scan back from end of file name string for file extension. If not   */
    /*	found, can't identify parser. Otherwise, locate end of extension    */
    /*	string and compare it to known file extensions.			    */
    /*-									    */

    for (extstr = &aSourceName[strlen(aSourceName)];
	extstr >= aSourceName && *extstr != FILE_EXT_SEPARATOR;
	extstr--);
    if (extstr < aSourceName) {
	printf("ERROR: No file extension specified for file %s\n", aSourceName);
	return NULL;
    }
    else {
	extstr++;
	switch (translate_keyword(extstr, global_langtable())) {
	case LANGUAGE_UNKNOWN:		    /* No matches on file type.	    */
	    printf(
	    "ERROR: Unable to identify source language for file %s\n",
		aSourceName);
	    return NULL;
	    break;
	case LANGUAGE_TEXT:
	    *aParserName = "text (no language structure)";
	    return text_parser;
	    break;
	case LANGUAGE_DCL:
	    *aParserName = "DCL";
	    return dcl_parser;
	    break;
	case LANGUAGE_CC:
	    *aParserName = "C";
	    return c_parser;
	    break;
	case LANGUAGE_BLISS:
	    *aParserName = "BLISS";
	    return bliss_parser;
	    break;
	}
    }
}

/*************************************************************************++*/
int analyze_file(
/* Analyze the code in a product source file, using the designated parser.  */
/* This adds routine definition entries for every routine defined in the    */
/* file, and for every unknown routine referenced in the file. Unknown	    */
/* routines are expected to be defined later in the file, in another file,  */
/* or in another product (i.e. a run-time library).			    */

    char    *aSourceName,
		/* (READ, BY ADDR):					    */
		/* Source file name string.				    */

    int	    vTabSize,
		/* (READ, BY ADDR):					    */
		/* Source file tab size.				    */

    PARSER  aParser,
		/* (READ, BY ADDR):					    */
		/* Source language parse routine, called to return the next */
		/* token from the source file.				    */

    char    *aLangName
		/* (READ, BY ADDR):					    */
		/* Language name string.				    */

)	/* Returns status indication:					    */
	/*  1	- File analyzed successfully.				    */
	/*  0   - File not analyzed successfully.			    */
	/*****************************************************************--*/

{
    DEFINITION				    /* Current routine definition.  */
	    *curdef = NULL;		  
    DEFINITION				    /* Referenced routine	    */
	    *refdef;			    /* definition.		    */
    REFERENCE				    /* Current routine reference.   */
	    *curref;
    SOURCEFILE
	    *curfile;			    /* Current source file.	    */
    FILE    *sourcefile;		    /* Source file ptr.		    */
    char    element[MAX_ROUTINE_NAME + 1];  /* Language element buffer.	    */
		
    language_element			    /* Type of language element.    */
	    element_type;
    long    sourceline;		    /* Line number in source file.  */
		
    /*+									    */
    /*	Add a new source file entry to the source file list, then analyze   */
    /*	the language elements in the source file, adding definition and	    */
    /*	reference entries for routines found.				    */
    /*-									    */

					    /* Open source file.	    */
    if ((sourcefile = fopen(aSourceName, "r")) == NULL) {
	printf("ERROR: Unable to open source file %s for input\n",
	    aSourceName);
	return 0;
    }
    else {				    /* Create new source file entry */
					    /* and add it to list.	    */
	curfile = new_file(aSourceName, vTabSize);
	add_file(curfile);		    
	list_file_begin(curfile, aLangName);
	new_list_line(curfile);
					    /* Process next source language */
					    /* element.			    */
	while ((element_type = (*aParser)(sourcefile, curfile, element,
				    &sourceline)) != END_OF_SOURCE) {
	    switch(element_type) {
	    case PARSE_ERROR:

		    /*+							    */
		    /*	Parser believes some source construct is invalid,   */
		    /*	give up on this file.				    */
		    /*-							    */
	    
		    fclose(sourcefile);
		    return 0;
		    break;
	    case ROUTINE_DEF_BEGIN:

		    /*+							    */
		    /*	Start of a routine found: Get its definition entry  */
		    /*	for this file and save begin line number. Add a	    */
		    /*	source reference, passing this def as the caller as */
		    /*	well as the definition so that it will be inserted  */
		    /*	in alphabetical order.				    */
		    /*-							    */
					    
		    curdef = find_def(element, curfile);
		    set_def_begin(curdef, sourceline);
		    add_srcref(curfile, new_ref(sourceline, curdef, curdef));
		    list_def_begin(curfile, curdef);
		    break;
	    case ROUTINE_DEF_END:

		    /*+							    */
		    /*	End of a routine found: Save end line number and    */
		    /*	clear current definition context.		    */
		    /*-							    */

		    set_def_end(curdef, sourceline);
		    inc_source_routines(curfile);
		    inc_source_rlength(curfile, def_length(curdef));
		    inc_source_calls(curfile, def_num_calls(curdef));
		    list_def_end(curfile, curdef);
		    curdef = NULL;	    
		    break;
	    case ROUTINE_REF:

		    /*+							    */
		    /*	Routine reference found: Make sure this is inside a */
		    /*	routine definition (as opposed to a static data	    */
		    /*	initializer macro or some other construct that the  */
		    /*	parser has misconstrued as a routine call). Find    */
		    /*	the definition of the referenced routine, add a	    */
		    /*	reference entry for it to current routine being	    */
		    /*	defined, and add a caller entry to it for the	    */
		    /*	current routine.				    */
		    /*-							    */

		    if (curdef != NULL) {   
			refdef = find_def(element, NULL);
			curref = new_ref(sourceline, refdef, curdef);
			add_ref(curdef, curref);    
		        list_ref(curfile, curref);
			curref = new_ref(sourceline, refdef, curdef);
			add_caller(refdef, curref);
		    }
		    break;
	    }
	}

	/*+								    */
	/*  Parser has reached the end of the source file, close it and	    */
	/*  roll up all file counts to total product counters.		    */
	/*-								    */

	fclose(sourcefile);
	inc_total_comments(source_comments(curfile));
	inc_total_statements(source_statements(curfile));
	inc_total_mixed(source_mixed(curfile));
	inc_total_empty(source_empty(curfile));
	inc_total_routines(source_routines(curfile));
	inc_total_rlength(source_rlength(curfile));
	inc_total_calls(source_calls(curfile));
	list_file_end(curfile);
	return 1;
    }
}

/*************************************************************************++*/
int analyze_product(
/* Processes a product definition file. This file contains a line for the   */
/* product name, then one line for each product source file.  Analyzes the  */
/* source code in each product source file, then produces analysis reports  */
/* for the product.							    */

    char    *aProductName
		/* (READ, BY ADDR):					    */
		/* Product file name string.				    */
		
)	/* Returns status indication:					    */
	/*  1	- Product analyzed successfully.			    */
	/*  0   - Product not analyzed successfully.			    */
	/*****************************************************************--*/

{
    FILE    *productfile;		    /* Product file ptr.	    */
					    /* Product name buffer.	    */
    char    productname[MAX_PRODUCT_NAME + 1];  
    char    sourcebuf[MAX_FILE_NAME + 1];   /* Source file line buffer.	    */
    char    sourcename[MAX_FILE_NAME + 1];  /* Source file name buffer.	    */
    char    outputname[MAX_FILE_NAME + 1];  /* Output file name buffer.	    */
    PARSER  *parser;			    /* Parser function for source   */
					    /* language.		    */
    char    *parsername;		    /* Parser language name ptr.    */
    int	    tabsize;			    /* File tab size.		    */

    /*+									    */
    /*	Analyze each source file listed in the product definition file.	    */
    /*	Abort if the product file cannot be opened or an error occurs	    */
    /*	during analysis. The appropriate parser must be determined	    */
    /*	individually for each file.					    */
    /*-									    */

					    /* Open product file.	    */
    if ((productfile = fopen(aProductName, "r")) == NULL) {
	printf("ERROR: Unable to open product file %s for input\n",
	    aProductName);
	return 0;
    }
    else if (fgets(productname, sizeof(productname), productfile) == NULL) {
	printf("ERROR: Unable to read product name from file %s\n",
	    aProductName);
	return 0;
    }
    else {
	productname[strlen(productname) - 1] = '\0';
	set_product_name(productname);
	list_product_begin(aProductName);
					    /* Get next source file name.   */
	while (fgets(sourcebuf, sizeof(sourcebuf), productfile) != NULL) {

	    if (sscanf(sourcebuf, "%s %d", sourcename, &tabsize) < 2) {
		tabsize = TAB_SIZE;
	    }
					    /* Get parser for file type and */
					    /* analyze file.		    */
	    if ((parser = get_parser(sourcename, &parsername)) == NULL
		|| !analyze_file(sourcename, tabsize, parser, parsername)) {
		fclose(productfile);
		return 0;
	    }
	}
	fclose(productfile);
	list_analysis_complete(aProductName);
    }

    /*+									    */
    /*	Write alphabetical reports (ordered by routine name): defined and   */
    /*	undefined routines; calls/callers for defined routines; and call    */
    /*	trees for defined routines. Before the last report, duplicate	    */
    /*	references must be trimmed from the routine definitions so that	    */
    /*	minimal call trees can be generated (i.e. a routine will appear	    */
    /*	only once in a given level of a call tree).			    */
    /*-									    */

    if (rpt_html_enabled()) {
	assign_xreffiles();
	assign_byfilefiles();
    }
    if (rpt_defined_enabled())
	report_defined(make_filename(outfile_prefix(), OUTFILE_SUFFIX_DEFLIST,
	    report_filext(), outputname));
    if (rpt_undefined_enabled())
	report_undefined(make_filename(outfile_prefix(), OUTFILE_SUFFIX_UNDEFLIST,
	    report_filext(), outputname));
    if (rpt_calls_enabled())
	report_calls(make_filename(outfile_prefix(), OUTFILE_SUFFIX_CALLS,
	    report_filext(), outputname));
    discard_dup_refs();			    /* Trim duplicate refs.	    */
#if 0
    if (rpt_trees_enabled())
	report_call_trees(make_filename(outfile_prefix(), OUTFILE_SUFFIX_CALLTREES,
	    report_filext(), outputname));
#endif
    if (rpt_xrefs_enabled())
	report_xrefs(make_filename(outfile_prefix(), OUTFILE_SUFFIX_XREF,
	    report_filext(), outputname));

    /*+									    */
    /*	Sort the file definitions by source file and beginning line number  */
    /*	and write the defined routines in this order. Finally, report the   */
    /*	product files.							    */
    /*-									    */

    sort_file_order();
    if (rpt_byfile_enabled())
	report_by_file(make_filename(outfile_prefix(), OUTFILE_SUFFIX_BYFILE,
	    report_filext(), outputname));
    if (rpt_files_enabled())
	report_files(make_filename(outfile_prefix(), OUTFILE_SUFFIX_FILES,
	    report_filext(), outputname));
    if (rpt_source_enabled())
	report_source(OUTFILE_SUFFIX_SOURCE);

    list_product_end(aProductName);
    return 1;				    /* Successful completion	    */
}

/*************************************************************************++*/
int cmdopt_set(
/* Common command line option handler to set simple toggle options.	    */

    int	    vToggleFlag
	    /* (READ, BY VAL):						    */
	    /* Toggle flag to set.					    */

)	/* Returns 1 to indicate successful processing of this option.	    */
	/*****************************************************************--*/

{
    set_option(vToggleFlag);
    return 1;
}

/*************************************************************************++*/
int cmdopt_clear(
/* Common command line option handler to clear simple toggle options.	    */

    int	    vToggleFlag
	    /* (READ, BY VAL):						    */
	    /* Toggle flag to clear.					    */

)	/* Returns 1 to indicate successful processing of this option.	    */
	/*****************************************************************--*/

{
    clear_option(vToggleFlag);
    return 1;
}

/*************************************************************************++*/
int cmdopt_trace(
/* Command line option handler to set specified traces.			    */

    int	    vOptCode,
	    /* (READ, BY VAL):						    */
	    /* Option keyword translation code, ignored by this routine.    */

    char    *aValStr
	    /* (READ, BY ADDR):						    */
	    /* Option value string, preceded by equal sign.		    */
    
)	/* Returns status code:						    */
	/*  1	- Successful processing of this option.			    */
	/*  0	- Report keywords missing.				    */
	/*****************************************************************--*/

{
					    /* Trace option keyword	    */
					    /* dispatch table.		    */
    static KEYWORD_DEFINITION keywords[] = {
	{"memalloc",	3, cmdopt_set,  TRACE_MEM_ENABLE},
	{"strings",	3, cmdopt_set,  TRACE_STR_ENABLE},
	{"objects",	3, cmdopt_set,  TRACE_OBJ_ENABLE},
	{"reports",	3, cmdopt_set,  TRACE_RPT_ENABLE},
	{"plevel",	3, cmdopt_set,  TRACE_PLEVEL_ENABLE},
	{"blevel",	3, cmdopt_set,  TRACE_BLEVEL_ENABLE},
	{"parser",	3, cmdopt_set,  TRACE_PARSER_ENABLE},
	{NULL,		0, NULL}	    /* End of table.		    */
    };

    if (*aValStr++ == CMDLINE_OPTION_SEPARATOR && *aValStr != '\0') {
	if (!process_keyword(aValStr, keywords)) {
	    printf("ERROR: Unable to process %ctrace option",
		CMDLINE_OPTION_SWITCH);
	    return 0;
	}
	else {
	    return 1;
	}
    }
    else {
	printf("ERROR: %ctrace option requires trace type keyword or list\n",
	    CMDLINE_OPTION_SWITCH);
	return 0;
    }
}

/*************************************************************************++*/
int cmdopt_log(
/* Command line option handler to set log file for stdout logging.	    */

    int	    vOptCode,
	    /* (READ, BY VAL):						    */
	    /* Option keyword translation code, ignored by this routine.    */

    char    *aValStr
	    /* (READ, BY ADDR):						    */
	    /* Option value string, preceded by equal sign.		    */
    
)	/* Returns status code:						    */
	/*  1	- Successful processing of this option.			    */
	/*  0	- Log file name missing, or file cannot be opened.	    */
	/*****************************************************************--*/

{
    /*+									    */
    /*	If a file name value has been supplied, reopen stdout on that file. */
    /*-									    */

    if (*aValStr++ == CMDLINE_OPTION_SEPARATOR && *aValStr != '\0') {
	if (freopen(aValStr, "w", stdout) != NULL) {
	    return 1;
	}
	else {
	    printf("ERROR: Unable to open log file %s for output\n", aValStr);
	    return 0;
	}
    }
    else {
	printf("ERROR: %clog option requires log file name\n",
	    CMDLINE_OPTION_SWITCH);
	return 0;
    }
}

/*************************************************************************++*/
int cmdopt_list(
/* Command line option handler to enable listing file. Attempts to create   */
/* listing file.							    */

    /* No arguments.							    */

)	/* Returns status code:						    */
	/*  1	- Successful processing of this option.			    */
	/*  0	- Unable to open listing file.				    */
	/*****************************************************************--*/

{
    char    name[MAX_FILE_NAME + 1];		/* Listing file name.	    */

    set_option(LIST_ENABLE);
    if (open_list_file(make_filename(outfile_prefix(), OUTFILE_SUFFIX_LIST,
	    OUTFILE_EXT_LIST, name)) == NULL) {
	printf("ERROR: Unable to open listing file %s for output\n", name);
	return 0;
    }
    else {
	return 1;
    }
}

/*************************************************************************++*/
int cmdopt_outprefix(
/* Command line option handler to set report file output prefix.	    */

    int	    vOptCode,
	    /* (READ, BY VAL):						    */
	    /* Option keyword translation code, ignored by this routine.    */

    char    *aValStr
	    /* (READ, BY ADDR):						    */
	    /* Option value string, preceded by equal sign.		    */
    
)	/* Returns status code:						    */
	/*  1	- Successful processing of this option.			    */
	/*  0	- Prefix missing.					    */
	/*****************************************************************--*/

{
    if (*aValStr++ == CMDLINE_OPTION_SEPARATOR && *aValStr != '\0') {
	set_outfile_prefix(aValStr);
	return 1;
    }
    else {
	printf("ERROR: %coutprefix option requires prefix string\n",
	    CMDLINE_OPTION_SWITCH);
	return 0;
    }
}

/*************************************************************************++*/
int cmdopt_description(
/* Command line option handler to set the product description file name.    */

    int	    vOptCode,
	    /* (READ, BY VAL):						    */
	    /* Option keyword translation code, ignored by this routine.    */

    char    *aValStr
	    /* (READ, BY ADDR):						    */
	    /* Option value string, preceded by equal sign.		    */
    
)	/* Returns status code:						    */
	/*  1	- Successful processing of this option.			    */
	/*  0	- Option value out of range.				    */
	/*****************************************************************--*/

{
    if (*aValStr++ == CMDLINE_OPTION_SEPARATOR) {
	set_product_description(aValStr);
	return 1;
    }
    printf("ERROR: %cdescription option requires file name\n",
	CMDLINE_OPTION_SWITCH);
    return 0;
}

/*************************************************************************++*/
int cmdopt_separate(
/* Command line option handler to set list of routines for which separate   */
/* call trees MUST be generated if they call anything. The option value is  */
/* the name of a file containing a list of routine names, one per line.	    */

    int	    vOptCode,
	    /* (READ, BY VAL):						    */
	    /* Option keyword translation code, ignored by this routine.    */

    char    *aValStr
	    /* (READ, BY ADDR):						    */
	    /* Option value string, preceded by equal sign.		    */
    
)	/* Returns status code:						    */
	/*  1	- Successful processing of this option.			    */
	/*  0	- List file name missing, or file cannot be opened.	    */
	/*****************************************************************--*/

{
    FILE    *sepfile;			    /* Sep routine list file ptr.   */
    int	    rcount;			    /* Routine name count.	    */
    char    rname[MAX_ROUTINE_NAME * 2];    /* Routine name buffer.	    */
    char    **rlist;			    /* Ptr to routine name list.    */
    
    if (*aValStr++ == CMDLINE_OPTION_SEPARATOR && *aValStr != '\0') {
	if ((sepfile = fopen(aValStr, "r")) != NULL) {
						/* Count lines in file.	    */
	    for (rcount = 0; fgets(rname, sizeof(rname), sepfile) != NULL;
		rcount++);
					    /* Allocate routine name list   */
					    /* and terminate it.	    */
	    rlist = malloc((rcount + 1) * sizeof(char *));
	    rlist[rcount] = NULL;

	    /*+								    */
	    /*	Reset to beginning of file. For each line in file, get the  */
	    /*	routine name, stripping the ending newline. Allocate memory */
	    /*	for it, copy the name to the memory, and put the memory ptr */
	    /*	in the routine name list.				    */
	    /*-								    */

	    fseek(sepfile, 0, SEEK_SET); 
	    while (rcount-- > 0) {
		fgets(rname, sizeof(rname), sepfile);
		rname[strlen(rname) - 1] = '\0';
		rlist[rcount] = strcpy(malloc(strlen(rname) + 1), rname);
	    }
	    fclose(sepfile);
	    set_separate_list(rlist);	    /* Save list in global db.	    */
	    return 1;
	    
	}
	else {
	    printf("ERROR: Unable to open separate routine file %s for input\n",
		aValStr);
	    return 0;
	}
    }
    else {
	printf("ERROR: %cseparate option requires routine list file name\n",
	    CMDLINE_OPTION_SWITCH);
	return 0;
    }
}

/*************************************************************************++*/
int cmdopt_language(
/* Command line option handler to set list of source file extension	    */
/* language translations.						    */

    int	    vOptCode,
	    /* (READ, BY VAL):						    */
	    /* Option keyword translation code, ignored by this routine.    */

    char    *aValStr
	    /* (READ, BY ADDR):						    */
	    /* Option value string, preceded by equal sign.		    */
    
)	/* Returns status code:						    */
	/*  1	- Successful processing of this option.			    */
	/*  0	- Language file name missing, file cannot be opened, or	    */
	/*	  invalid language definition found.			    */
	/*****************************************************************--*/

{
    static KEYWORD_DEFINITION languages[] = {
	{"text",	3, NULL, LANGUAGE_TEXT},
	{"dcl",		3, NULL, LANGUAGE_DCL},
	{"bliss",	3, NULL, LANGUAGE_BLISS},
	{"c",		1, NULL, LANGUAGE_CC},
	{NULL,          0, NULL}	    /* End of table.		    */
    };
    FILE    *langfile;			    /* Lang translation file ptr.   */
    char    translation[512];		    /* Translation string buffer.   */
    char    *fextbegin;			    /* File extension ptr.	    */
    char    *fextend;			    /* File extension end ptr.	    */
    SOURCE_LANGUAGE			    /* Language identification.	    */
	    langcode;
					    /* Check for equal sign and	    */
					    /* make sure there is a file.   */
    if (*aValStr++ == CMDLINE_OPTION_SEPARATOR && *aValStr != '\0') {
	if ((langfile = fopen(aValStr, "r")) != NULL) {
	
					    /* For each line, locate	    */
					    /* beginning of text.	    */
	    while (fgets(translation, sizeof(translation), langfile) != NULL) {
		translation[strlen(translation) - 1] = '\0';
		for (fextbegin = translation;
		    *fextbegin != '\0' && isspace(*fextbegin) &&
		    *fextbegin != CMDLINE_OPTION_COMMENT;
		    fextbegin++);
					    /* Ignore comment lines. Find   */
					    /* end of translation field and */
					    /* process translation.	    */
		if (*fextbegin != CMDLINE_OPTION_COMMENT) {
		    for (fextend = fextbegin;
			*fextend > ' ' && *fextend != CMDLINE_OPTION_SEPARATOR;
			fextend++);
		    if (*fextend != CMDLINE_OPTION_SEPARATOR
			|| !isalnum(*(fextend + 1))) {
			printf(
		    "ERROR: Invalid language definition in language file %s\n",
			    aValStr);
			printf(
		    "       Language name missing in definition %s\n",
			    fextbegin);
			fclose(langfile);
			return 0;
		    }
		    else {
			*fextend++ = '\0';
			if ((langcode = translate_keyword(fextend,
			    languages)) == LANGUAGE_UNKNOWN) {
			    printf(
		    "ERROR: Invalid language definition in language file %s\n",
			    aValStr);
			    printf("       Language %s unknown\n", fextend);
			    fclose(langfile);
			    return 0;
			}
			else {
			    add_lang(new_lang(fextbegin, langcode));
			}
		    }
		}
	    }
	    fclose(langfile);
	    return 1;
	}
	else {
	    printf("ERROR: Unable to open language file %s for input\n",
		aValStr);
	    return 0;
	}
    }
    else {
	printf("ERROR: %clanguage option requires language file name\n",
	    CMDLINE_OPTION_SWITCH);
	return 0;
    }
}

/*************************************************************************++*/
int cmdopt_urlprefix(
/* Command line option handler to set HTML URL prefix.			    */

    int	    vOptCode,
	    /* (READ, BY VAL):						    */
	    /* Option keyword translation code, ignored by this routine.    */

    char    *aValStr
	    /* (READ, BY ADDR):						    */
	    /* Option value string, preceded by equal sign.		    */
    
)	/* Returns status code:						    */
	/*  1	- Successful processing of this option.			    */
	/*  0	- Prefix missing.					    */
	/*****************************************************************--*/

{
    if (*aValStr++ == CMDLINE_OPTION_SEPARATOR && *aValStr != '\0') {
	set_url_prefix(aValStr);
	return 1;
    }
    else {
	printf("ERROR: %curlprefix option requires prefix string\n",
	    CMDLINE_OPTION_SWITCH);
	return 0;
    }
}

/*************************************************************************++*/
int cmdopt_callers(
/* Command line option handler to set maximum number of callers to allow    */
/* inline expansion of routine call subtrees.				    */

    int	    vOptCode,
	    /* (READ, BY VAL):						    */
	    /* Option keyword translation code, ignored by this routine.    */

    char    *aValStr
	    /* (READ, BY ADDR):						    */
	    /* Option value string, preceded by equal sign.		    */
    
)	/* Returns status code:						    */
	/*  1	- Successful processing of this option.			    */
	/*  0	- Option value out of range.				    */
	/*****************************************************************--*/

{
    int	    maxcallers;			    /* Option value.		    */

    if (*aValStr++ == CMDLINE_OPTION_SEPARATOR) {
	maxcallers = atoi(aValStr);
	if (maxcallers >= MIN_MAX_CALLERS && maxcallers <= MAX_MAX_CALLERS) {
	    set_max_callers(maxcallers);
	    return 1;
	}
    }
    printf("ERROR: %ccallers option requires value between %d and %d\n",
	CMDLINE_OPTION_SWITCH, MIN_MAX_CALLERS, MAX_MAX_CALLERS);
    return 0;
}

/*************************************************************************++*/
int cmdopt_depth(
/* Command line option handler to set maximum call tree expansion depth.    */

    int	    vOptCode,
	    /* (READ, BY VAL):						    */
	    /* Option keyword translation code, ignored by this routine.    */

    char    *aValStr
	    /* (READ, BY ADDR):						    */
	    /* Option value string, preceded by equal sign.		    */
    
)	/* Returns status code:						    */
	/*  1	- Successful processing of this option.			    */
	/*  0	- Option value out of range.				    */
	/*****************************************************************--*/

{
    int	    maxdepth;			    /* Option value.		    */

    if (*aValStr++ == CMDLINE_OPTION_SEPARATOR) {
	maxdepth = atoi(aValStr);
	if (maxdepth > 0 && maxdepth < MAX_TREE_DEPTH) {
	    set_max_tree_depth(maxdepth);
	    return 1;
	}
    }
    printf("ERROR: %cdepth option requires value between %d and %d\n",
	CMDLINE_OPTION_SWITCH, 1, MAX_TREE_DEPTH - 1);
    return 0;
}

/*************************************************************************++*/
int cmdopt_report(
/* Command line option handler to set specified report generation.	    */

    int	    vOptCode,
	    /* (READ, BY VAL):						    */
	    /* Option keyword translation code, ignored by this routine.    */

    char    *aValStr
	    /* (READ, BY ADDR):						    */
	    /* Option value string, preceded by equal sign.		    */
    
)	/* Returns status code:						    */
	/*  1	- Successful processing of this option.			    */
	/*  0	- Report keywords missing.				    */
	/*****************************************************************--*/

{
					    /* Report option keyword	    */
					    /* dispatch table.		    */
    static KEYWORD_DEFINITION keywords[] = {
	{"defined",	3, cmdopt_clear,    RPT_DEFINED_DISABLE},
	{"undefined",	3, cmdopt_clear,    RPT_UNDEFINED_DISABLE},
	{"calls",	3, cmdopt_clear,    RPT_CALLS_DISABLE},	    
	{"trees",	3, cmdopt_clear,    RPT_TREES_DISABLE},	    
	{"xrefs",	3, cmdopt_clear,    RPT_XREFS_DISABLE},	    
	{"files",	3, cmdopt_clear,    RPT_FILES_DISABLE},	    
	{"byfile",	3, cmdopt_clear,    RPT_BYFILE_DISABLE},	    
	{"source",	3, cmdopt_clear,    RPT_SOURCE_DISABLE},	    
	{NULL,		0, NULL}	    /* End of table.		    */
    };

    if (*aValStr++ == CMDLINE_OPTION_SEPARATOR && *aValStr != '\0') {
    
					    /* Disable all reports, then    */
					    /* enable only those specified  */
	set_option( RPT_DEFINED_DISABLE |   /* by keyword.		    */
		     RPT_UNDEFINED_DISABLE |
		     RPT_CALLS_DISABLE |
		     RPT_TREES_DISABLE |
		     RPT_XREFS_DISABLE |
		     RPT_BYFILE_DISABLE |
		     RPT_FILES_DISABLE |
		     RPT_SOURCE_DISABLE);
	if (!process_keyword(aValStr, keywords)) {
	    printf("ERROR: Unable to process %creport option",
		CMDLINE_OPTION_SWITCH);
	    return 0;
	}
	else {
	    return 1;
	}
    }
    else {
	printf("ERROR: %creport option requires report type keyword or list\n",
	    CMDLINE_OPTION_SWITCH);
	return 0;
    }
}

/*************************************************************************++*/
int cmdopt_noreport(
/* Command line option handler to suppress specified report generation.	    */

    int	    vOptCode,
	    /* (READ, BY VAL):						    */
	    /* Option keyword translation code, ignored by this routine.    */

    char    *aValStr
	    /* (READ, BY ADDR):						    */
	    /* Option value string, preceded by equal sign.		    */
    
)	/* Returns status code:						    */
	/*  1	- Successful processing of this option.			    */
	/*  0	- Report keywords missing.				    */
	/*****************************************************************--*/

{
					    /* No-report option keyword	    */
					    /* dispatch table.		    */
    static KEYWORD_DEFINITION keywords[] = {
	{"defined",	3, cmdopt_set,    RPT_DEFINED_DISABLE},
	{"undefined",	3, cmdopt_set,    RPT_UNDEFINED_DISABLE},
	{"calls",	3, cmdopt_set,    RPT_CALLS_DISABLE},	    
	{"trees",	3, cmdopt_set,    RPT_TREES_DISABLE},	    
	{"xrefs",	3, cmdopt_set,    RPT_XREFS_DISABLE},	    
	{"files",	3, cmdopt_set,    RPT_FILES_DISABLE},	    
	{"byfile",	3, cmdopt_set,    RPT_BYFILE_DISABLE},	    
	{"source",	3, cmdopt_set,    RPT_SOURCE_DISABLE},	    
	{NULL,		0, NULL}	    /* End of table.		    */
    };

    if (*aValStr++ == CMDLINE_OPTION_SEPARATOR && *aValStr != '\0') {
	if (!process_keyword(aValStr, keywords)) {
	    printf("ERROR: Unable to process %cnoreport option",
		CMDLINE_OPTION_SWITCH);
	    return 0;
	}
	else {
	    return 1;
	}
    }
    else {
	printf(
	    "ERROR: %cnoreport option requires report type keyword or list\n",
	    CMDLINE_OPTION_SWITCH);
	return 0;
    }
}

/*************************************************************************++*/
int cmdopt_fmt_kwhandler(
/* Common keyword handler for format command line option.		    */

    report_output_format
    	    vFormat
	    /* (READ, BY VAL):						    */
	    /* Format code.						    */

)	/* Returns status flag:						    */
	/*  1	- Successful processing of this option.			    */
	/*  0	- Conflicting format option specified.			    */
	/*****************************************************************--*/

{
    if (rpt_text_enabled() || report_format() != FORMAT_TEXT) {
	puts("ERROR: Conflicting format options specififed");
	return 0;
    }
    else {
	switch (vFormat) {
	case FORMAT_TEXT:
	    set_option(RPT_TEXT_ENABLE);	/* Text has special flag.   */
	case FORMAT_SDML:
	case FORMAT_HTML:
	    set_report_format(vFormat);
	    break;
	case FORMAT_RTF:
	case FORMAT_WINHELP:
	case FORMAT_VMSHELP:
puts("Sorry, the RTF, WINHELP, and VMSHELP formats are not yet implemented");
return 0;
	    break;
	}
        return 1;
    }
}

/*************************************************************************++*/
int cmdopt_format(
/* Command line option handler to set report output format.		    */

    int	    vOptCode,
	    /* (READ, BY VAL):						    */
	    /* Option keyword translation code, ignored by this routine.    */

    char    *aValStr
	    /* (READ, BY ADDR):						    */
	    /* Option value string, preceded by equal sign.		    */
    
)	/* Returns status code:						    */
	/*  1	- Successful processing of this option.			    */
	/*  0	- Format keyword missing.				    */
	/*****************************************************************--*/

{
					    /* Format option keyword	    */
					    /* dispatch table.		    */
    static KEYWORD_DEFINITION keywords[] = {
	{"text",        3, cmdopt_fmt_kwhandler,    FORMAT_TEXT},
	{"sdml",        3, cmdopt_fmt_kwhandler,    FORMAT_SDML},
	{"html",        3, cmdopt_fmt_kwhandler,    FORMAT_HTML},
	{"rtf",		3, cmdopt_fmt_kwhandler,    FORMAT_RTF},
	{"winhelp",	3, cmdopt_fmt_kwhandler,    FORMAT_WINHELP},
	{"vmshelp",	3, cmdopt_fmt_kwhandler,    FORMAT_VMSHELP},
	{NULL,		0, NULL}	    /* End of table.		    */
    };

    if (*aValStr++ == CMDLINE_OPTION_SEPARATOR && *aValStr != '\0') {
	if (!process_keyword(aValStr, keywords)) {
	    printf("ERROR: Unable to process %cformat option",
		CMDLINE_OPTION_SWITCH);
	    return 0;
	}
	else {
	    return 1;
	}
    }
    else {
	printf("ERROR: %cformat option requires report format keyword\n",
	    CMDLINE_OPTION_SWITCH);
	return 0;
    }
}

/*************************************************************************++*/
int cmdopt_htmlbyfile(
/* Command line option handler to set maximum number of entries of HTML	    */
/* output to allow per by-file file.					    */

    int	    vOptCode,
	    /* (READ, BY VAL):						    */
	    /* Option keyword translation code, ignored by this routine.    */

    char    *aValStr
	    /* (READ, BY ADDR):						    */
	    /* Option value string, preceded by equal sign.		    */
    
)	/* Returns status code:						    */
	/*  1	- Successful processing of this option.			    */
	/*  0	- Value missing.					    */
	/*****************************************************************--*/

{
    if (*aValStr++ == CMDLINE_OPTION_SEPARATOR) {
	set_max_html_byfile(atoi(aValStr));
	return 1;
    }
    printf("ERROR: %chtmlbyfile option requires value\n",
	CMDLINE_OPTION_SWITCH);
    return 0;
}

/*************************************************************************++*/
int cmdopt_htmlxref(
/* Command line option handler to set maximum number of entries of HTML	    */
/* output to allow per call xref file.					    */

    int	    vOptCode,
	    /* (READ, BY VAL):						    */
	    /* Option keyword translation code, ignored by this routine.    */

    char    *aValStr
	    /* (READ, BY ADDR):						    */
	    /* Option value string, preceded by equal sign.		    */
    
)	/* Returns status code:						    */
	/*  1	- Successful processing of this option.			    */
	/*  0	- Value missing.					    */
	/*****************************************************************--*/

{
    if (*aValStr++ == CMDLINE_OPTION_SEPARATOR) {
	set_max_html_xref(atoi(aValStr));
	return 1;
    }
    printf("ERROR: %chtmlxref option requires value\n",
	CMDLINE_OPTION_SWITCH);
    return 0;
}

/*************************************************************************++*/
int cmdopt_author(
/* Command line option handler to show program author for posterity (gee,   */
/* I'm embarrassed!).							    */

    /* No arguments.							    */

)	/* Returns 0 to indicate that this option is totally bogus!
	/*****************************************************************--*/

{
    printf("%s %s\n%s\n", PROGRAM_IDENT, PROGRAM_COPYRIGHT, PROGRAM_AUTHOR);
    return 0;
}

/*************************************************************************++*/
void show_help(
/* Prints help information to stdout and EXITS PROGRAM.			    */

    /* No arguments.							    */

)	/* No return value.      					    */
	/*****************************************************************--*/

{
    printf("%s %s\n\n", PROGRAM_IDENT, PROGRAM_COPYRIGHT);
    puts(PROGRAM_PARAMS);
    printf(PROGRAM_HELP, CMDLINE_OPTION_SWITCH);
    puts(
"\nWhere: product_file is the product definition file listing the source files");
    puts(
"          to be analyzed.");
    puts(
"       options are the following (they may be abbreviated):");
#if 0 /* This help is too long and is incomplete! */
    printf(
"          %chelp - Show this full help and exit.\n", CMDLINE_OPTION_SWITCH);
    printf(
"          %clist - Create listing file <output_prefix>%s%s\n",
	CMDLINE_OPTION_SWITCH, OUTFILE_SUFFIX_LIST, OUTFILE_EXT_LIST);
    printf(
"          %csilent - Disable stdout logging.\n", CMDLINE_OPTION_SWITCH);
    printf(
"          %cbrief - Brief stdout logging.\n", CMDLINE_OPTION_SWITCH);
    printf(
"          %coutprefix=output_prefix - Output file pathname and filename prefix;\n",
	CMDLINE_OPTION_SWITCH);
    puts(
"             suffixes and extensions will be added to this prefix to create");
    puts(
"             full output file names.");
    printf(
"          %cdefinition - Log routine definitions.\n", CMDLINE_OPTION_SWITCH);
    printf(
"          %creference - Log routine references.\n", CMDLINE_OPTION_SWITCH);
    printf(
"          %cseparate=sep_file - Name of file containing routine names for which\n             separate trees must always be generated if they call anything.\n",
	CMDLINE_OPTION_SWITCH);
    printf(
"          %clanguage - Name of file containing language definitions of the\n            form <ext>=<language>, where <ext> is a file type extension, and\n            <language> is one of the keywords \"c\", \"bliss\", or \"text\".\n",
	CMDLINE_OPTION_SWITCH);
    printf(
"          %cnoinline - Generate separate call trees for all routines that call\n              anything.\n",
	CMDLINE_OPTION_SWITCH);
    printf(
"          %ctext - Format reports as plain text files (default).\n",
	CMDLINE_OPTION_SWITCH);
    printf(
"          %csdml - Format reports as Standard Digital Markup Language for\n              VAX Document.\n",
	CMDLINE_OPTION_SWITCH);
    printf(
"          %chtml - Format reports as Hyper Text Markup Language for World-\n              Wide Web browsers.\n",
	CMDLINE_OPTION_SWITCH);
    printf(
"          %ccallers=N - Set max callers for inline subtrees to N\n              (range %d-%d, default %d).\n",
	CMDLINE_OPTION_SWITCH, MIN_MAX_CALLERS, MAX_MAX_CALLERS,
	DEF_MAX_CALLERS);
#else
    printf("          %chelp                       %cauthor\n",
	CMDLINE_OPTION_SWITCH, CMDLINE_OPTION_SWITCH);
    printf("          %coptions=opt_file           %ctrace=debug_list\n",
	CMDLINE_OPTION_SWITCH, CMDLINE_OPTION_SWITCH);
    printf("          %clog=log_file               %clist\n",
	CMDLINE_OPTION_SWITCH, CMDLINE_OPTION_SWITCH);
    printf("          %csilent                     %cbrief\n",
	CMDLINE_OPTION_SWITCH, CMDLINE_OPTION_SWITCH);
    printf("          %coutprefix=outfile_prefix   %curlprefix=url_prefix\n",
	CMDLINE_OPTION_SWITCH, CMDLINE_OPTION_SWITCH);
    printf("          %cformat=report_format       %cseparate=sep_file\n",
	CMDLINE_OPTION_SWITCH, CMDLINE_OPTION_SWITCH);
    printf("          %clanguage=lang_file         %cdescription=desc_file\n",
	CMDLINE_OPTION_SWITCH, CMDLINE_OPTION_SWITCH);
    printf("          %cdefinition                 %creference\n",
	CMDLINE_OPTION_SWITCH, CMDLINE_OPTION_SWITCH);
    printf("          %ccallers=n                  %cdepth=n\n",
	CMDLINE_OPTION_SWITCH, CMDLINE_OPTION_SWITCH);
    printf("          %creport=report_list         %cnoreport=report_list\n",
	CMDLINE_OPTION_SWITCH, CMDLINE_OPTION_SWITCH);
    printf("          %chtmlbyfile=n               %chtmlxref=n\n",
	CMDLINE_OPTION_SWITCH, CMDLINE_OPTION_SWITCH);
    printf("          %cnoinline\n",
	CMDLINE_OPTION_SWITCH);
#endif
	
    if (list_enabled()) {		    /* Close the listing file if    */
	fclose(list_file());		    /* one was created.		    */
    }
    
    exit(0);				    /* End the program!		    */
}

/*************************************************************************++*/
main(
/* Program main routine.						    */

    int	    vArgc,
	    /* (READ, BY VAL):						    */
	    /* Number of program argument strings in aArgv.		    */

    char    *aArgv[]
	    /* (READ, BY ADDR):						    */
	    /* List of program argument strings.			    */

)	/* Returns system success code.					    */
	/*****************************************************************--*/

{
					    /* Main program command line    */
					    /* argument options dispatch    */
					    /* table.			    */
    static KEYWORD_DEFINITION options[] = {
	{"options",     3, process_options_file},
	{"help",	3, show_help},
	{"trace",	3, cmdopt_trace},
	{"log",         3, cmdopt_log},	    
	{"list",        3, cmdopt_list},    
	{"silent",      3, cmdopt_set,	LOG_SILENT_ENABLE},  
	{"brief",       3, cmdopt_set,	LOG_BRIEF_ENABLE},   
	{"outprefix",   3, cmdopt_outprefix},
	{"format",	3, cmdopt_format},
	{"description", 3, cmdopt_description},
	{"definition",  3, cmdopt_set,	LOG_DEF_ENABLE},
	{"reference",   3, cmdopt_set,	LOG_REF_ENABLE},
	{"separate",    3, cmdopt_separate},
	{"language",	3, cmdopt_language},
	{"noinline",    3, cmdopt_set,	TREE_INLINE_DISABLE},
	{"urlprefix",   3, cmdopt_urlprefix},
	{"callers",	3, cmdopt_callers},
	{"depth",	3, cmdopt_depth},
	{"report",	3, cmdopt_report},
	{"noreport",    3, cmdopt_noreport},
	{"htmlbyfile",	5, cmdopt_htmlbyfile},
	{"htmlxref",	5, cmdopt_htmlxref},
	{"author",	3, cmdopt_author},
	{NULL,          0, NULL}	    /* End of table.		    */
    };

    /*+									    */
    /*	Make sure enough required arguments were specified, then process    */
    /*	the optional arguments and analyze the product files. If no	    */
    /*	arguments, show brief help. If first argument is an option, show    */
    /*	full help regardless of which options were specified. Note that	    */
    /*	showing full help terminates the program without any further	    */
    /*	processing of the command line, even if all the arguments are ok.   */
    /*-									    */
    
    if (vArgc < 2) {
	puts(PROGRAM_PARAMS);
        printf(PROGRAM_HELP, CMDLINE_OPTION_SWITCH);
    }
    else if (*aArgv[1] == CMDLINE_OPTION_SWITCH) {
	    show_help();
    }
    else {
					    /* Disable these reports by	    */
					    /* default.			    */
	set_option(RPT_CALLS_DISABLE | RPT_TREES_DISABLE);
	
	set_max_callers(DEF_MAX_CALLERS);
	set_max_html_byfile(DEF_MAX_HTML_BYFILE);
	set_max_html_xref(DEF_MAX_HTML_XREF);
	set_max_tree_depth(MAX_TREE_DEPTH + 1);
	if (process_options(vArgc, aArgv, 2, options)) {
	    add_lang(new_lang("DAT", LANGUAGE_TEXT));
	    add_lang(new_lang("TXT", LANGUAGE_TEXT));
	    add_lang(new_lang("COM", LANGUAGE_DCL));
	    add_lang(new_lang("C",   LANGUAGE_CC));
	    add_lang(new_lang("H",   LANGUAGE_CC));
	    add_lang(new_lang("BLI", LANGUAGE_BLISS));
	    add_lang(new_lang("REQ", LANGUAGE_BLISS));
	    add_lang(new_lang("R32", LANGUAGE_BLISS));
	    analyze_product(aArgv[1]);
	    if (list_enabled()) {
		fclose(list_file());
	    }
	}
    }
}

