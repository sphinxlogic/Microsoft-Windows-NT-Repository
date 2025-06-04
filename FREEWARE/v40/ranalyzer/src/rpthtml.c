/****************************************************************************/
/*									    */
/*  FACILITY:	Routine Analyzer					    */
/*									    */
/*  MODULE:	HTML Report Formatting Routines				    */
/*									    */
/*  AUTHOR:	Steve Branam, Network Product Support Group, Digital	    */
/*		Equipment Corporation, Littleton, MA, USA.		    */
/*									    */
/*  DESCRIPTION: This module contains the routines for generating Routine   */
/*  Analyzer reports in HTML format, suitable for viewing with a World-Wide */
/*  Web browser.							    */
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
#include "reports.h"

#define calls_link_suffix()	(rpt_source_enabled()	? "</A>" : "")
#define xref_link_suffix()	(rpt_xrefs_enabled()	? "</A>" : "")
#define byfile_link_suffix()	(rpt_byfile_enabled()	? "</A>" : "")
#define source_link_suffix()	(rpt_source_enabled()	? "</A>" : "")

					/* Section navigation hyperlink	    */
					/* information.			    */
static HTML_SECTION mSections[] = {
    {NULL, "table_of_contents", NULL, NULL, NULL, NULL, NULL},
    {HTML_SECTION_FILES, "source_files_section",
	OUTFILE_SUFFIX_FILES, NULL,
	OUTFILE_SUFFIX_BYFILE, NULL,
	"file", NULL},
    {HTML_SECTION_BYFILE, "routines_by_file_section",
	OUTFILE_SUFFIX_BYFILE, OUTFILE_SUFFIX_MOREBYFILE,
	OUTFILE_SUFFIX_DEFLIST, OUTFILE_SUFFIX_FILES,
	"file", "routine", "f"},
    {HTML_SECTION_DEFINED, "defined_routines_section",
	OUTFILE_SUFFIX_DEFLIST, NULL,
	OUTFILE_SUFFIX_UNDEFLIST,
	OUTFILE_SUFFIX_BYFILE, "routine", NULL, NULL},
    {HTML_SECTION_UNDEFINED, "undefined_routines_section",
	OUTFILE_SUFFIX_UNDEFLIST, NULL,
	OUTFILE_SUFFIX_CALLS, OUTFILE_SUFFIX_DEFLIST,
	"routine", NULL, NULL},
    {HTML_SECTION_CALLS, "routine_calls_section",
	OUTFILE_SUFFIX_CALLS, NULL,
	OUTFILE_SUFFIX_XREF, OUTFILE_SUFFIX_UNDEFLIST,
	"routine", NULL, "c"},
    {HTML_SECTION_XREF, "caller_xref_section",
	OUTFILE_SUFFIX_XREF, OUTFILE_SUFFIX_MOREXREF,
	OUTFILE_SUFFIX_CALLTREES, OUTFILE_SUFFIX_CALLS,
	"routine", NULL, "x"},
    {HTML_SECTION_TREES, "routine_call_trees_section",
	OUTFILE_SUFFIX_CALLTREES, OUTFILE_SUFFIX_MORETREES,
	OUTFILE_SUFFIX_FIRSTSOURCE,
	OUTFILE_SUFFIX_XREF, "tree", NULL, "t"},
    {HTML_SECTION_SOURCE, "source_code_section",
	OUTFILE_SUFFIX_FIRSTSOURCE, OUTFILE_SUFFIX_SOURCE,
	NULL, OUTFILE_SUFFIX_CALLTREES,
	"file", NULL, "s"}
};

/*************************************************************************++*/
void assign_xreffiles(
/* Assigns the Xref file numbers for routine definitions.		    */

    /* No arguments.							    */

)	/* No return value.      					    */
	/*****************************************************************--*/

{
    int	    xreffile;			    /* Xref file number.	    */
    int	    lines;			    /* Number of lines of def/refs. */
    DEFINITION				    /* Current definition entry.    */
	    *curdef;
	    
    for (xreffile = lines = 0, curdef = list_first(global_deflist());
	curdef != NULL;
	curdef = next_entry(curdef)) {
	set_def_xreffile(curdef, xreffile);
	lines += def_num_callers(curdef) + def_num_calls(curdef) + 5;
	if (lines > max_html_xref()) {
	    lines = 0;
	    xreffile++;
	}
    }
}

/*************************************************************************++*/
void assign_byfilefiles(
/* Assigns the by-file file numbers for routine definitions.		    */

    /* No arguments.							    */

)	/* No return value.      					    */
	/*****************************************************************--*/

{
    int	    byfilefile;			    /* By-file file number.	    */
    int	    lines;			    /* Number of lines of def/refs. */
    SOURCEFILE				    /* Current file entry.	    */
	    *curfile;
	    
    for (byfilefile = lines = 0, curfile = list_first(global_filelist());
	curfile != NULL;
	curfile = next_entry(curfile)) {
	set_source_byfilefile(curfile, byfilefile);
	lines += source_routines(curfile) + 10;
	if (lines > max_html_byfile()) {
	    lines = 0;
	    byfilefile++;
	}
    }
}

/*************************************************************************++*/
char *link_url(
/* Formats an HTML link URL portion.					    */

    char    *aFileName
	    /* (READ, BY ADDR):  					    */
	    /* Report file name portion.				    */

)	/* Returns formatted string ptr.  WARNING: The returned ptr is the  */
	/* address of a statically-defined string buffer, so the string	    */
	/* must be read from it immediately.				    */
	/*****************************************************************--*/

{
					    /* Trace name string buffer.    */
    static char	strbuf[MAX_FILE_NAME + 60];

    if (url_prefix()) {
	sprintf(strbuf, "%s%s%s%s", url_prefix(), outfile_prefix(), aFileName,
	    OUTFILE_EXT_HTML);
    }
    else {
	sprintf(strbuf, "%s%s%s", outfile_prefix(), aFileName,
	    OUTFILE_EXT_HTML);
    }
    return strbuf;
}

#if 1
#define tree_link(d,f) xref_link(d,f)
#else
/*************************************************************************++*/
char *tree_link(
/* Formats a hyperlink to a routine's call tree if one exists.		    */

    DEFINITION
	    *aDef,
	    /* (READ, BY ADDR):						    */
	    /* Routine definition entry to link to.			    */

    int	    vSameFile
	    /* (READ, BY VAL):  					    */
	    /* Flag indicating whether this is link within same HTML file.  */

)	/* Returns formatted string ptr.  WARNING: The returned ptr is the  */
	/* address of a statically-defined string buffer, so the string	    */
	/* must be read from it immediately.				    */
	/*****************************************************************--*/

{
    static char	link[MAX_FILE_NAME + 64];   /* Link string buffer.	    */
    char    outputname[MAX_FILE_NAME + 1];  /* Report file name buffer.	    */

    if (!rpt_trees_enabled()) {		    /* Linking is valid only if	    */
	return "";			    /* report generated.	    */
    }
    else {
	if (!vSameFile) {
	    if (def_treefile(aDef)) {
		sprintf(outputname, OUTFILE_SUFFIX_MORETREES,
		    def_treefile(aDef));
	    }
	    else {
		strcpy(outputname, OUTFILE_SUFFIX_CALLTREES);
	    }
	}
    					    /* If routine needs a separate  */
					    /* tree or is expanded within   */
					    /* some other tree, make link.  */
	if (needs_tree(aDef) || def_root(aDef) != NULL) {
	    sprintf(link, "<A HREF=\"%s#t%lx\">%s</A>",
		(vSameFile ? "" : link_url(outputname)),
#if 0	    
		(needs_tree(aDef) ? aDef : def_root(aDef)),
		(needs_tree(aDef) ? "Tree" : "Root"));
#else
		aDef, "Tree");
#endif
	    return link;
	}
	else {				    /* Otherwise, no tree exists.   */
	    return "No tree";
	}
    }
}
#endif

/*************************************************************************++*/
char *source_link_prefix(
/* Formats the prefix for a hyperlink to a routine's source code.	    */

    DEFINITION
	    *aDef,
	    /* (READ, BY ADDR):						    */
	    /* Routine definition entry to link to.			    */

    int	    vSameFile
	    /* (READ, BY VAL):  					    */
	    /* Flag indicating whether this is link within same HTML file.  */

)	/* Returns formatted string ptr.  WARNING: The returned ptr is the  */
	/* address of a statically-defined string buffer, so the string	    */
	/* must be read from it immediately.				    */
	/*****************************************************************--*/

{
    static char	link[MAX_FILE_NAME + 64];   /* Link string buffer.	    */
    char    linkname[MAX_FILE_NAME + 1];    /* Source file name.	    */

    if (!rpt_source_enabled()) {	    /* Linking is valid only if	    */
	return "";			    /* report generated.	    */
    }
    else {
	if (!vSameFile) {
	    sprintf(linkname, OUTFILE_SUFFIX_SOURCE,
		source_seq(def_source(aDef)));
	}
	sprintf(link, "<A HREF=\"%s#d%lx\">",
	    (vSameFile ? "" : link_url(linkname)), aDef);
	return link;
    }
}

/*************************************************************************++*/
char *source_link(
/* Formats a hyperlink to a routine's source code.			    */

    DEFINITION
	    *aDef,
	    /* (READ, BY ADDR):						    */
	    /* Routine definition entry to link to.			    */

    int	    vSameFile
	    /* (READ, BY VAL):  					    */
	    /* Flag indicating whether this is link within same HTML file.  */

)	/* Returns formatted string ptr.  WARNING: The returned ptr is the  */
	/* address of a statically-defined string buffer, so the string	    */
	/* must be read from it immediately.				    */
	/*****************************************************************--*/

{
    static char	link[MAX_FILE_NAME + 64];   /* Link string buffer.	    */

    if (!rpt_source_enabled()) {	    /* Linking is valid only if	    */
	return "";			    /* report generated.	    */
    }
    else if (isdefined_routine(aDef)) {
	sprintf(link, "%sSource%s", source_link_prefix(aDef, vSameFile),
	    source_link_suffix());
	return link;
    }
    else {
	return "No source";
    }
}

/*************************************************************************++*/
char *sourcefile_link_prefix(
/* Formats the prefix for a hyperlink to a file's source code.		    */

    SOURCEFILE
	    *aSourceFile
	    /* (READ, BY ADDR):						    */
	    /* Source file entry to link to.				    */

)	/* Returns formatted string ptr.  WARNING: The returned ptr is the  */
	/* address of a statically-defined string buffer, so the string	    */
	/* must be read from it immediately.				    */
	/*****************************************************************--*/

{
    static char	link[MAX_FILE_NAME + 64];   /* Link string buffer.	    */
    char    linkname[MAX_FILE_NAME + 1];    /* Source file name.	    */

    if (!rpt_source_enabled()) {	    /* Linking is valid only if	    */
	return "";			    /* report generated.	    */
    }
    else {
	sprintf(linkname, OUTFILE_SUFFIX_SOURCE, source_seq(aSourceFile));
	sprintf(link, "<A HREF=\"%s\">", link_url(linkname), aSourceFile);
	return link;
    }
}

/*************************************************************************++*/
char *calls_link_prefix(
/* Formats the prefix for a hyperlink to a routine's calls/callers.	    */

    DEFINITION
	    *aDef,
	    /* (READ, BY ADDR):						    */
	    /* Routine definition entry to link to.			    */

    int	    vSameFile
	    /* (READ, BY VAL):  					    */
	    /* Flag indicating whether this is link within same HTML file.  */

)	/* Returns formatted string ptr.  WARNING: The returned ptr is the  */
	/* address of a statically-defined string buffer, so the string	    */
	/* must be read from it immediately.				    */
	/*****************************************************************--*/

{
    static char	link[MAX_FILE_NAME + 64];   /* Link string buffer.	    */

    if (!rpt_calls_enabled()) {		    /* Linking is valid only if	    */
	return "";			    /* report generated.	    */
    }
    else {
	sprintf(link, "<A HREF=\"%s#c%lx\">",
	    (vSameFile ? "" : link_url(OUTFILE_SUFFIX_CALLS)), aDef);
	return link;
    }
}

/*************************************************************************++*/
char *xref_link_prefix(
/* Formats the prefix for a hyperlink to a routine's caller		    */
/* cross-references.							    */

    DEFINITION
	    *aDef,
	    /* (READ, BY ADDR):						    */
	    /* Routine definition entry to link to.			    */

    int	    vSameFile
	    /* (READ, BY VAL):  					    */
	    /* Flag indicating whether this is link within same HTML file.  */

)	/* Returns formatted string ptr.  WARNING: The returned ptr is the  */
	/* address of a statically-defined string buffer, so the string	    */
	/* must be read from it immediately.				    */
	/*****************************************************************--*/

{
    static char	link[MAX_FILE_NAME + 64];   /* Link string buffer.	    */
    char    outputname[MAX_FILE_NAME + 1];  /* Report file name buffer.	    */

    if (!rpt_xrefs_enabled()) {		    /* Linking is valid only if	    */
	return "";			    /* report generated.	    */
    }
    else {
	if (!vSameFile) {
	    if (def_xreffile(aDef)) {
		sprintf(outputname, OUTFILE_SUFFIX_MOREXREF,
		    def_xreffile(aDef));
	    }
	    else {
		strcpy(outputname, OUTFILE_SUFFIX_XREF);
	    }
	}
	sprintf(link, "<A HREF=\"%s#x%lx\">",
	    (vSameFile ? "" : link_url(outputname)), aDef);
	return link;
    }
}

/*************************************************************************++*/
char *xref_link(
/* Formats a hyperlink to a routine's caller cross-references.		    */

    DEFINITION
	    *aDef,
	    /* (READ, BY ADDR):						    */
	    /* Routine definition entry to link to.			    */

    int	    vSameFile
	    /* (READ, BY VAL):  					    */
	    /* Flag indicating whether this is link within same HTML file.  */

)	/* Returns formatted string ptr.  WARNING: The returned ptr is the  */
	/* address of a statically-defined string buffer, so the string	    */
	/* must be read from it immediately.				    */
	/*****************************************************************--*/

{
    static char	link[MAX_FILE_NAME + 64];   /* Link string buffer.	    */

    if (!rpt_xrefs_enabled()) {		    /* Linking is valid only if	    */
	return "";			    /* report generated.	    */
    }
    else {
	sprintf(link, "%sGoto%s", xref_link_prefix(aDef, vSameFile),
	    xref_link_suffix());
	return link;
    }
}

/*************************************************************************++*/
char *byfile_link_prefix(
/* Formats the prefix for a hyperlink to a file's routine listing.	    */

    SOURCEFILE
	    *aSourceFile,
	    /* (READ, BY ADDR):						    */
	    /* File entry to link to.					    */

    int	    vSameFile
	    /* (READ, BY VAL):  					    */
	    /* Flag indicating whether this is link within same HTML file.  */

)	/* Returns formatted string ptr.  WARNING: The returned ptr is the  */
	/* address of a statically-defined string buffer, so the string	    */
	/* must be read from it immediately.				    */
	/*****************************************************************--*/

{
    static char	link[MAX_FILE_NAME + 64];   /* Link string buffer.	    */

    if (!rpt_byfile_enabled()) {	    /* Linking is valid only if	    */
	return "";			    /* report generated.	    */
    }
    else {
	sprintf(link, "<A HREF=\"%s#f%lx\">",
	    (vSameFile ? "" : link_url(OUTFILE_SUFFIX_BYFILE)), aSourceFile);
	return link;
    }
}

/*************************************************************************++*/
void rpt_html_section_nav(
/* Writes a set of common HTML section navigation links to the report file. */
/* These links go to the table of contents, and the next and previous	    */
/* sections.								    */

    FILE    *aRptFile,
	    /* (READ, BY ADDR):						    */
	    /* Report output file. Must be opened by caller.		    */

    HTML_SECTION_TYPE
	    vSection,
	    /* (READ, BY VAL):  					    */
	    /* Section number to write.					    */

    int	    vLast
	    /* (READ, BY VAL):						    */
	    /* Flag indicating this is last entry in section.		    */

)	/* No return value.      					    */
	/*****************************************************************--*/

{
    char    fname[MAX_FILE_NAME + 1];	    /* File name buffer.	    */

    fprintf(aRptFile, "Go to: <A HREF=\"%s#%s\">Contents</a>",
	link_url(OUTFILE_SUFFIX_FILES),
	section_anchor(mSections, MAIN_SECTION));
    if (section_nextsuffix(mSections, vSection) != NULL) {
	fprintf(aRptFile, "; <A HREF=\"%s\">Next section</a>",
	    link_url(section_nextsuffix(mSections, vSection)));
    }
    if (section_prevsuffix(mSections, vSection) != NULL) {
	fprintf(aRptFile, "; <A HREF=\"%s\">Previous section</a>",
	    link_url(section_prevsuffix(mSections, vSection)));
    }
}

/*************************************************************************++*/
void rpt_html_section_title(
/* Writes a common HTML section title report file.			    */

    FILE    *aRptFile,
	    /* (READ, BY ADDR):						    */
	    /* Report output file. Must be opened by caller.		    */

    char    *aSubTitle
	    /* (READ, BY ADDR):  					    */
	    /* Optional subtitle string to add to title. If NULL is passed, */
	    /* only main title will be written.				    */

)	/* No return value.      					    */
	/*****************************************************************--*/

{
    fprintf(aRptFile,
	"<TITLE>%s Source File Analysis%s%s</TITLE>\n", product_name(),
	(aSubTitle == NULL ? "" : " - "),
	(aSubTitle == NULL ? "" : aSubTitle));
}	

/*************************************************************************++*/
void rpt_html_section_hdr(
/* Writes a common HTML section header to the report file. This includes a  */
/* title, a level-one header, and optionally navigation links to other	    */
/* sections.								    */

    FILE    *aRptFile,
	    /* (READ, BY ADDR):						    */
	    /* Report output file. Must be opened by caller.		    */

    HTML_SECTION_TYPE
	    vSection
	    /* (READ, BY VAL):  					    */
	    /* Section number to write.					    */

)	/* No return value.      					    */
	/*****************************************************************--*/

{
    FILE    *dscfile;			    /* Description file.	    */
    char    dscbuf[256];		    /* Description file line	    */
					    /* buffer.			    */
    
    rpt_html_section_title(aRptFile, 
	(ismain_section(vSection) ? NULL : section_name(mSections, vSection)));

    if (ismain_section(vSection)) {	    /* Write main header.	    */
	fprintf(aRptFile, "<H1>%s Source File Analysis</H1>\n",
	    product_name());

	if (product_description() != NULL) {
	    if ((dscfile = fopen(product_description(), "r")) != NULL) {
		while (fgets(dscbuf, sizeof(dscbuf), dscfile) != NULL) {
		    fputs(dscbuf, aRptFile);
		}
		fclose(dscfile);
	    }
	    else {
		printf(
		    "WARNING: Unable to open description file %s for input\n",
		    product_description());
	    }
	}

	fputs("<A NAME=\"table_of_contents\">\n", aRptFile);
	fputs("<H2>Contents</H2></A>\n", aRptFile);
	fputs("The following sections contain information that was\n",
	    aRptFile);
	fputs("generated by automated analysis of the\n", aRptFile);
	fprintf(aRptFile, "%s source files.<P>\n", product_name());

	fputs("<UL>\n", aRptFile);
	if (rpt_files_enabled())
	    fputs("<LI> <A HREF=\"#source_files_section\">Source Files</A>\n",
		aRptFile);
	if (rpt_byfile_enabled())
	    fprintf(aRptFile, "<LI> <A HREF=\"%s\">Defined Routines By File</A>\n",
		link_url(OUTFILE_SUFFIX_BYFILE));
	if (rpt_defined_enabled())
	    fprintf(aRptFile, "<LI> <A HREF=\"%s\">Defined Routines Alphabetical</A>\n",
		link_url(OUTFILE_SUFFIX_DEFLIST));
	if (rpt_undefined_enabled())
	    fprintf(aRptFile, "<LI> <A HREF=\"%s\">External Routines Alphabetical</A>\n",
		link_url(OUTFILE_SUFFIX_UNDEFLIST));
	if (rpt_calls_enabled())
	    fprintf(aRptFile, "<LI> <A HREF=\"%s\">Defined Routines Calls/Callers</A>\n",
		link_url(OUTFILE_SUFFIX_CALLS));
	if (rpt_xrefs_enabled())
	    fprintf(aRptFile, "<LI> <A HREF=\"%s\">Caller Cross Reference</A>\n",
		link_url(OUTFILE_SUFFIX_XREF));
	if (rpt_trees_enabled())
	    fprintf(aRptFile, "<LI> <A HREF=\"%s\">Routine Call Trees</A>\n",
		link_url(OUTFILE_SUFFIX_CALLTREES));
	if (rpt_source_enabled())
	    fprintf(aRptFile, "<LI> <A HREF=\"%s\">Source Code</A>\n",
		link_url(OUTFILE_SUFFIX_FIRSTSOURCE));
	fputs("</UL>\n", aRptFile);

	fputs("For information on the software used to generate this\n", aRptFile);
	fputs("analysis, contact Steve Branam, Network Products\n", aRptFile);
	fputs("Support Group, Digital Equipment Corporation,\n", aRptFile);
	fputs("Littleton, MA, USA.<P>\n", aRptFile);
	fputs("\n<HR>\n<HR>\n\n", aRptFile);
    }
    else {				    /* Write section header.	    */
	fprintf(aRptFile, "<A NAME=\"%s\">\n",
	    section_anchor(mSections, vSection));
	fprintf(aRptFile, "<H1>%s</H1></a>\n",
	    section_name(mSections, vSection));
	rpt_html_section_nav(aRptFile, vSection, 0);
	fputs("</A>.<P>\n", aRptFile);
    }
}

/*************************************************************************++*/
void rpt_html_subsection_hdr(
/* Writes a common HTML subsection header to the report file.  This	    */
/* includes navigation links to other sections and subsections.		    */

    FILE    *aRptFile,
	    /* (READ, BY ADDR):						    */
	    /* Report output file. Must be opened by caller.		    */

    HTML_SECTION_TYPE
	    vSection,
	    /* (READ, BY VAL):  					    */
	    /* Section number to write.					    */

    void    *aNext,
	    /* (READ, BY ADDR):						    */
	    /* Next subsection object (of any type).			    */

    void    *aPrev,
	    /* (READ, BY ADDR):						    */
	    /* Previous subsection object (of any type).		    */

    char    *aFileSuffix,
	    /* (READ, BY ADDR):						    */
	    /* Optional filename suffix for linking across continuation	    */
	    /* files. If NULL is passed, next/prev links will be within	    */
	    /* current file. Otherwise, next or prev link (whichever one is */
	    /* NULL) is within a different file.			    */

    int	    vFileNumber
	    /* (READ, BY VAL):						    */
	    /* File number for linking across continuation files.	    */
	    /* If this is zero and aFileSuffix is non-NULL, the file name   */
	    /* will come from the mSections table. If it is non-zero and    */
	    /* aFileSuffix is non-NULL, aFileSuffix will be used with the   */
	    /* number to form the name.					    */

)	/* No return value.      					    */
	/*****************************************************************--*/

{
    char    fname[MAX_FILE_NAME + 1];	    /* File name buffer.	    */

    rpt_html_section_nav(aRptFile, vSection, (aNext == NULL));
    if (aFileSuffix != NULL) {
	if (vFileNumber) {
	    sprintf(fname, aFileSuffix, vFileNumber);
	}
	else {
	    strcpy(fname, section_suffix(mSections, vSection));
	}
    }
    fprintf(aRptFile, "; <A HREF=\"%s\">Beginning of section</a>",
	link_url(section_suffix(mSections, vSection)));

    if (aNext != NULL) {
	fprintf(aRptFile, "; <A HREF=\"%s#%s%lx\">Next %s in section</a>",
	    (aFileSuffix != NULL ? link_url(fname) : ""),
	    section_subanchor(mSections, vSection), aNext,
	    section_objstr(mSections, vSection));
    }

    if (aPrev != NULL) {
	fprintf(aRptFile, "; <A HREF=\"%s#%s%lx\">Previous %s in section</a>",
	    (aFileSuffix != NULL ? link_url(fname) : ""),
	    section_subanchor(mSections, vSection), aPrev,
	    section_objstr(mSections, vSection));
    }
    fputs(".<P>\n", aRptFile);
}

/*************************************************************************++*/
int rpt_html_continue(
/* Continues an HTML report in a new output file, adding links between the  */
/* files.                                                                   */

    FILE    **aRptFile,
	    /* (MODIFY, BY ADDR):  					    */
	    /* Report file ptr. The current file will be linked and closed  */
	    /* through this ptr, and the new file will be opened through    */
	    /* it, leaving it set to the new file information.		    */
    
    HTML_SECTION_TYPE
	    vSection,
	    /* (READ, BY VAL):  					    */
	    /* Section number being reported.				    */

    void    *aCurrent,
	    /* (READ, BY ADDR):						    */
	    /* Current subsection object (of any type) to be reported.	    */

    int	    vCurFileNumber,
	    /* (READ, BY VAL):						    */
	    /* Current file number being ended.				    */
    
    int	    vNextFileNumber
	    /* (READ, BY VAL):						    */
	    /* Next file number to be started.				    */

)	/* Returns vNextFileNumber, next file number started if successful, */
	/* or 0 if continuation failed.					    */
	/*****************************************************************--*/

{
    char    rptname[MAX_FILE_NAME + 1];	    /* Report file name prefix.	    */
    char    outputname[MAX_FILE_NAME + 1];  /* Report file name buffer.	    */

					    /* Write ending link to next    */
					    /* object and close file.	    */
    rpt_html_subsection_hdr(*aRptFile, vSection, aCurrent, NULL,
	section_moresuffix(mSections, vSection), vNextFileNumber);
    fclose(*aRptFile);

					    /* Open next file.		    */
    sprintf(rptname, section_moresuffix(mSections, vSection), vNextFileNumber);
    if ((*aRptFile = fopen(make_filename(outfile_prefix(), rptname,
	report_filext(), outputname), "w")) == NULL) {
	printf("ERROR: Unable to open %s for report output\n", rptname);
	return 0;
    }
    else {
	if (log_enabled()) {
	    printf("       Continuing report section in file %s\n", outputname);
	}
					    /* Write HTML title, starting   */
					    /* link to previous object.	    */
	rpt_html_section_title(*aRptFile, section_name(mSections, vSection));
	rpt_html_subsection_hdr(*aRptFile, vSection, NULL,
	    entry_blink((LIST_ENTRY_HDR *) aCurrent),
	    section_moresuffix(mSections, vSection), vCurFileNumber);
	return vNextFileNumber;
    }
}

/*************************************************************************++*/
void rpt_defined_html_hdr(
/* Writes HTML-formatted report header for defined routines.		    */

    FILE    *aRptFile
	    /* (READ, BY ADDR):						    */
	    /* Report output file. Must be opened by caller.		    */

)	/* No return value.						    */
	/*****************************************************************--*/

{
    rpt_html_section_hdr(aRptFile, DEFINED_ROUTINES_SECTION);
    fputs("<PRE>\n", aRptFile);
    fputs(
"========================================================================\n",
	aRptFile);
    fputs(
"                                                           #      Times\n",
	aRptFile);
    fputs(
"Routine                                     Line  Length   Calls  Called\n",
	aRptFile);
    fputs(
"------------------------------------------------------------------------\n",
	aRptFile);
}

/*************************************************************************++*/
void rpt_defined_html_entry(
/* Writes HTML-formatted routine entry for a defined routine.		    */

    FILE    *aRptFile,
	    /* (READ, BY ADDR):						    */
	    /* Report output file. Must be opened by caller.		    */

    DEFINITION
	    *aDef,
	    /* (READ, BY ADDR):						    */
	    /* Routine definition entry to report.			    */
    
    int	    vFirst,
	    /* (READ, BY VAL):						    */
	    /* Flag indicating whether this is first entry in table.	    */

    int	    vLast
	    /* (READ, BY VAL):						    */
	    /* Flag indicating whether this is last entry in table.	    */
    
)	/* No return value.						    */
	/*****************************************************************--*/

{
    char    fullname[MAX_ROUTINE_IDENT + 1];/* Routine fullname buffer.	    */
					    /* Routine name folding buffer. */
    char    rname[RPT_HTML_ROUTINE_MEDIUM_LEN + 1]; 
    int	    pos;			    /* Pos in routine name to	    */
					    /* print.			    */

    strcpy(fullname, def_ident(aDef));
					    /* Get first part of rout name. */
    pos = fold_string(fullname, 0, NULL, rname, RPT_HTML_ROUTINE_MEDIUM_LEN);
					    
    fprintf(aRptFile, "%-40s ", rname);
#if 0
    fprintf(aRptFile,"%s%7ld%s %7ld ",
	source_link_prefix(aDef, 0), def_begin(aDef), source_link_suffix(),
	def_length(aDef));
#else
    fprintf(aRptFile,"%7ld %7ld ", def_begin(aDef), def_length(aDef));
#endif
    fprintf(aRptFile,"%s%7ld%s %s%7ld%s\n",
	(vLast ? "<A NAME=\"defined_routines_section_last\">" : ""),
	def_num_calls(aDef), (vLast ? "</A>" : ""), xref_link_prefix(aDef, 0),
	def_num_callers(aDef), xref_link_suffix());
	
    while (pos < strlen(fullname)) {
	pos = fold_string(fullname, pos, "  ", rname,
	    RPT_HTML_ROUTINE_MEDIUM_LEN);
	fprintf(aRptFile, "%s\n", rname);
    }
}

/*************************************************************************++*/
void rpt_defined_html_end(
/* Writes HTML-formatted report end for defined routines.		    */

    FILE    *aRptFile,
	    /* (READ, BY ADDR):						    */
	    /* Report output file. Must be opened by caller.		    */

    long    vTotalDef
	    /* (READ, BY VAL):						    */
	    /* Total number of defined routines.			    */

)	/* No return value.						    */
	/*****************************************************************--*/

{
    fputs(
"-------------------------------------------------------------------------\n",
	aRptFile);
    fprintf(aRptFile, "TOTAL: %ld routines\n", vTotalDef);
    fputs(
"=========================================================================\n",
	aRptFile);
    fputs("</PRE>\n<HR>\n<HR>\n", aRptFile);
}

/*************************************************************************++*/
void rpt_undefined_html_hdr(
/* Writes HTML-formatted report header for undefined routines.		    */

    FILE    *aRptFile
	    /* (READ, BY ADDR):						    */
	    /* Report output file. Must be opened by caller.		    */

)	/* No return value.						    */
	/*****************************************************************--*/

{
    rpt_html_section_hdr(aRptFile, UNDEFINED_ROUTINES_SECTION);
    fputs("<PRE>\n", aRptFile);
    fputs(
"================================================\n",
	aRptFile);
    fputs(
"                                          Times\n",
	aRptFile);
    fputs(
"Routine                                   Called\n",
	aRptFile);
    fputs(
"------------------------------------------------\n",
	aRptFile);
}

/*************************************************************************++*/
void rpt_undefined_html_entry(
/* Writes HTML-formatted routine entry for a undefined routine.		    */

    FILE    *aRptFile,
	    /* (READ, BY ADDR):						    */
	    /* Report output file. Must be opened by caller.		    */

    DEFINITION
	    *aDef,
	    /* (READ, BY ADDR):						    */
	    /* Routine definition entry to report.			    */
    
    int	    vFirst,
	    /* (READ, BY VAL):						    */
	    /* Flag indicating whether this is first entry in table.	    */

    int	    vLast
	    /* (READ, BY VAL):						    */
	    /* Flag indicating whether this is last entry in table.	    */

)	/* No return value.						    */
	/*****************************************************************--*/

{
    char    fullname[MAX_ROUTINE_IDENT + 1];/* Routine fullname buffer.	    */
					    /* Routine name folding buffer. */
    char    rname[RPT_HTML_ROUTINE_MEDIUM_LEN + 1]; 
    int	    pos;			    /* Pos in routine name to	    */
					    /* print.			    */

    strcpy(fullname, def_name(aDef));
					    /* Get first part of rout name. */
    pos = fold_string(fullname, 0, NULL, rname, RPT_HTML_ROUTINE_MEDIUM_LEN);
					    
    fprintf(aRptFile,"%s%-40s%s %s%7ld%s\n",
	(vLast ? "<A NAME=\"undefined_routines_section_last\">" : ""),
	rname, (vLast ? "</A>" : ""), xref_link_prefix(aDef, 0),
	def_num_callers(aDef), xref_link_suffix());
	
    while (pos < strlen(fullname)) {
	pos = fold_string(fullname, pos, "  ", rname,
	    RPT_HTML_ROUTINE_MEDIUM_LEN);
	fprintf(aRptFile, "%s\n", rname);
    }
}

/*************************************************************************++*/
void rpt_undefined_html_end(
/* Writes HTML-formatted report end for undefined routines.		    */

    FILE    *aRptFile,
	    /* (READ, BY ADDR):						    */
	    /* Report output file. Must be opened by caller.		    */

    long    vTotalUndef
	    /* (READ, BY VAL):						    */
	    /* Total number of undefined routines.			    */

)	/* No return value.						    */
	/*****************************************************************--*/

{
    fputs(
"------------------------------------------------\n",
	aRptFile);
    fprintf(aRptFile, "TOTAL: %ld routines\n", vTotalUndef);
    fputs(
"================================================\n",
	aRptFile);
    fputs("</PRE>\n<HR>\n<HR>\n", aRptFile);
}

/*************************************************************************++*/
void rpt_calls_html_hdr(
/* Writes HTML-formatted report header for defined routine calls/callers    */
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
    rpt_html_section_hdr(aRptFile, CALLS_CALLERS_SECTION);
    fputs("<HR>\n\n", aRptFile);

    fprintf(aRptFile,
    "<TABLE>(%s Calls/Caller Routines)\n", def_ident(aDef));
    fputs("<TABLE_ATTRIBUTES>(MULTIPAGE)\n", aRptFile);
    fputs("<TABLE_SETUP>(4\\20\\5\\20)\n", aRptFile);
    fputs("<TABLE_HEADS>(Calls Routine\\Line\\Caller Routine\\Line)\n",
	aRptFile);
}

/*************************************************************************++*/
void rpt_calls_html_entry(
/* Writes HTML-formatted calls/caller entry for a defined routine.	    */

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
void rpt_calls_html_end(
/* Writes HTML-formatted report end for defined routine calls/caller	    */
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
void rpt_xref_html_hdr(
/* Writes HTML-formatted report header for cross reference.		    */

    FILE    *aRptFile
	    /* (READ, BY ADDR):						    */
	    /* Report output file. Must be opened by caller.		    */

)	/* No return value.						    */
	/*****************************************************************--*/

{
    rpt_html_section_hdr(aRptFile, CALLER_XREF_SECTION);
    fputs("<HR>\n\n", aRptFile);
}

/*************************************************************************++*/
void rpt_xref_html_entry(
/* Writes HTML-formatted cross-reference sections for a routine.	    */

    FILE    *aRptFile,
	    /* (READ, BY ADDR):						    */
	    /* Report output file. Must be opened by caller.		    */

    DEFINITION
	    *aDef,
	    /* (READ, BY ADDR):						    */
	    /* Routine definition entry to report.			    */

    int	    vFirst,
	    /* (READ, BY VAL):						    */
	    /* Flag indicating this is first entry being reported.	    */

    int	    vLast
	    /* (READ, BY VAL):						    */
	    /* Flag indicating this is last entry being reported.	    */
    
)	/* No return value.						    */
	/*****************************************************************--*/

{
    REFERENCE				    /* Current caller ref.	    */
	    *caller;
    char    fullname[MAX_ROUTINE_IDENT + 1];/* Routine fullname buffer.	    */
					    /* Routine name folding buffer. */
    char    rname[RPT_HTML_ROUTINE_WIDE_LEN + 1]; 
    int	    rpos;			    /* Pos in routine name to print.*/

    fprintf(aRptFile, "<H2><A NAME=\"x%lx\">%s</A> - ", aDef, def_name(aDef));
    if (isdefined_routine(aDef)) {
	fprintf(aRptFile, "%s%s%s", source_link_prefix(aDef, 0),
	    source_name(def_source(aDef)), source_link_suffix());
    }
    else {
	fputs("External", aRptFile);
    }
    fputs("</H2><H3>Callers", aRptFile);
    if (def_num_callers(aDef) == 0) {
	fputs(": No callers</H3>\n<PRE>\n", aRptFile);
    }
    else {
	fputs("</H3>\n<PRE>\n", aRptFile);
	fprintf(aRptFile, "%ld caller%s\n",
	    def_num_callers(aDef), (def_num_callers(aDef) == 1 ? "" : "s"));

					    /* Write entry for each caller. */
	for (caller = list_first(def_callers(aDef));
	    caller != NULL;
	    caller = next_entry(caller)) {
	    strcpy(fullname, def_ident(ref_caller(caller)));
	    
						/* Get first part of rout name. */
	    rpos = fold_string(fullname, 0, NULL, rname,
		RPT_HTML_ROUTINE_WIDE_LEN);
	    fprintf(aRptFile, "%5c+ %-48s %s\n", ' ', rname,
		xref_link(ref_caller(caller),
		    (def_xreffile(aDef) == def_xreffile(ref_caller(caller)))));
	    while (rpos < strlen(fullname)) {   /* Write rest of routine name.  */
		rpos = fold_string(fullname, rpos, "    ", rname,
		    RPT_HTML_ROUTINE_WIDE_LEN);
		fprintf(aRptFile, "%5c%s\n", ' ', rname);
	    }
	}
    }
    if (isdefined_routine(aDef)) {
	fputs("</PRE><H3>Call Tree", aRptFile);
	if (def_num_calls(aDef) == 0) {
	    fputs(": No calls", aRptFile);
	}
	fputs("</H3>\n", aRptFile);
    }
    if (def_num_calls(aDef) != 0) {
	fputs("<PRE>\n", aRptFile);
    }
    else {
	fputs("<HR>\n", aRptFile);
    }
}

/*************************************************************************++*/
void rpt_xref_html_end(
/* Writes HTML-formatted report end for caller cross-reference.		    */

    FILE    *aRptFile,
	    /* (READ, BY ADDR):						    */
	    /* Report output file. Must be opened by caller.		    */

    long    vTotalDef
	    /* (READ, BY VAL):						    */
	    /* Total number of routines.				    */

)	/* No return value.						    */
	/*****************************************************************--*/

{
    fputs("----------------------------------------------------\n", aRptFile);
    fprintf(aRptFile, "TOTAL: %ld routines\n", vTotalDef);
    fputs("====================================================\n", aRptFile);
    fputs("</PRE>\n<HR>\n<HR>\n", aRptFile);
}

/*************************************************************************++*/
void rpt_tree_html_hdr(
/* Writes HTML-formatted report header for defined routine call tree.	    */

    FILE    *aRptFile,
	    /* (READ, BY ADDR):						    */
	    /* Report output file. Must be opened by caller.		    */

    DEFINITION
	    *aDef,
	    /* (READ, BY ADDR):						    */
	    /* Routine definition entry to report.			    */
    
    int	    vFirst,
	    /* (READ, BY VAL):						    */
	    /* Flag indicating whether this is first tree.		    */

    int	    vLast
	    /* (READ, BY VAL):						    */
	    /* Flag indicating whether this is last tree.		    */
    
)	/* No return value.						    */
	/*****************************************************************--*/

{
    if (vFirst) {
	rpt_html_section_hdr(aRptFile, CALL_TREES_SECTION);
	fputs("<HR>\n\n", aRptFile);
    }

    fprintf(aRptFile, "<H2><A NAME=\"t%lx\">%s</a> %sCall Tree%s</H2>\n",
	aDef, def_ident(aDef),
	(vLast ? "<A NAME=\"routine_call_trees_section_last\">" : ""),
	(vLast ? "</A>" : ""));
    rpt_html_subsection_hdr(aRptFile, CALL_TREES_SECTION,
	(vLast ? NULL : next_tree(aDef)),
	(vFirst ? NULL : prev_tree(aDef)), NULL, 0);
    fputs("<PRE>\n", aRptFile);
}

/*************************************************************************++*/
void rpt_tree_html_entry(
/* Writes HTML-formatted call tree line for a defined routine.		    */

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
	fprintf(aRptFile, "%c   ", (isend_of_list(aRef) ? '+' : '|'));
    }
    if (vLevel == 0) {
#if 0
	fprintf(aRptFile,"%s%s%s: ",
	    source_link_prefix(ref_definition(aRef), 0),
	    def_name(ref_definition(aRef)), source_link_suffix());
	fprintf(aRptFile, "%s%ld caller%s%s\n",
	    xref_link_prefix(ref_definition(aRef), 0),
	    def_num_callers(ref_definition(aRef)),
	    (def_num_callers(ref_definition(aRef)) == 1 ? "" : "s"),
	    xref_link_suffix());
#else
	fprintf(aRptFile,"%s\n", def_name(ref_definition(aRef)));
#endif
    }
					    /* If routine is not going to   */
					    /* be expanded here, add an	    */
    else if (vExpanded || vRecursive ||	    /* explanation code.	    */
	!isdefined_routine(ref_definition(aRef))) {
	fprintf(aRptFile, "%s (%s%s%s)",
	    def_name(ref_definition(aRef)),
	    (vExpanded ? "Duplicate" : ""),
	    (vRecursive ? "Recursive" : ""),
	    (!isdefined_routine(ref_definition(aRef)) ? "External" : ""));
    }
#if 0
					    /* Separate tree gets link.	    */
    else if (needs_tree(ref_definition(aRef)) && !tree_inline_disabled()) {
	fprintf(aRptFile, "%s %s(Separate)%s\n",
	    def_name(ref_definition(aRef)),
	    xref_link_prefix(ref_definition(aRef), 0), xref_link_suffix());
    }
#else
    else if (needs_tree(ref_definition(aRef)) && !tree_inline_disabled()
	|| vLevel == max_tree_depth()
	    && def_num_calls(ref_definition(aRef)) > 0) {
	fprintf(aRptFile, "%s (Separate)", def_name(ref_definition(aRef)));
    }
#endif
#if 0
    else if (def_root(ref_definition(aRef)) == NULL) {
					    /* This is the first inline	    */
					    /* expansion of a routine, make */
					    /* it an anchor.		    */
	fprintf(aRptFile, "<A NAME=\"t%lx\">%s</a>", ref_definition(aRef),
	    def_name(ref_definition(aRef)));
    }
#endif
    else {				    /* Otherwise, format plain.	    */
	fprintf(aRptFile, "%s", def_name(ref_definition(aRef)));
    }
    if (vLevel > 0) {
	fprintf(aRptFile, " %s\n", xref_link(ref_definition(aRef), 0));
    }
}

/*************************************************************************++*/
void rpt_tree_html_end(
/* Writes HTML-formatted section end for defined routine call tree.	    */

    FILE    *aRptFile,
	    /* (READ, BY ADDR):						    */
	    /* Report output file. Must be opened by caller.		    */

    DEFINITION
	    *aDef,
	    /* (READ, BY ADDR):						    */
	    /* Routine definition entry to report.			    */

    int	    vFirst,
	    /* (READ, BY VAL):						    */
	    /* Flag indicating whether this is first tree.		    */

    int	    vLast
	    /* (READ, BY VAL):						    */
	    /* Flag indicating whether this is last tree.		    */
    
)	/* No return value.						    */
	/*****************************************************************--*/

{
    fputs("END OF TREE\n", aRptFile);
    fputs("</PRE>\n<HR>\n\n", aRptFile);
}

/*************************************************************************++*/
void rpt_byfile_html_hdr(
/* Writes HTML-formatted report header for defined routines by file table.  */

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
    if (vFirst) {
	rpt_html_section_hdr(aRptFile, ROUTINES_BY_FILE_SECTION);
	fputs("<HR>\n\n", aRptFile);
    }

    fprintf(aRptFile, "<H2><A NAME=\"f%lx\">%s</a> %sRoutines%s</H2>\n",
	aSourceFile, source_name(aSourceFile),
	(vLast ? "<A NAME=\"routines_by_file_section_last\">" : ""),
	(vLast ? "</A>" : ""));
    rpt_html_subsection_hdr(aRptFile, ROUTINES_BY_FILE_SECTION,
	(vLast ? NULL : next_defining_file(aSourceFile)),
	(vFirst ? NULL : prev_defining_file(aSourceFile)), NULL, 0);
    fputs("<PRE>\n", aRptFile);
    fputs("=======================================================\n",
	aRptFile);
    fputs("                                          #      Times\n",
	aRptFile);
    fputs("Routine                    Line  Length   Calls  Called\n",
	aRptFile);
    fputs("-------------------------------------------------------\n",
	aRptFile);
}

/*************************************************************************++*/
void rpt_byfile_html_entry(
/* Writes HTML-formatted entry for a defined routine by file.		    */

    FILE    *aRptFile,
	    /* (READ, BY ADDR):						    */
	    /* Report output file. Must be opened by caller.		    */

    DEFINITION
	    *aDef,
	    /* (READ, BY ADDR):						    */
	    /* Routine definition entry to report.			    */

    int	    vFirst,
	    /* (READ, BY VAL):						    */
	    /* Flag indicating whether this is first entry in table.	    */

    int	    vLast
	    /* (READ, BY VAL):						    */
	    /* Flag indicating whether this is last entry in table.	    */
    
)	/* No return value.						    */
	/*****************************************************************--*/

{
    char    anchor[32]; 		    /* Anchor name buffer.	    */
					    /* Routine name folding buffer. */
    char    rname[RPT_HTML_FILE_FIELD_LEN + 1]; 
    int	    pos;			    /* Pos in routine name to	    */
					    /* print.			    */

    if (vLast) {			    /* Form anchor name.	    */
	sprintf(anchor, "<A NAME=\"f%lx_last\">", def_source(aDef));
    }
					    /* Get first part of rout name. */
    pos = fold_string(def_name(aDef), 0, NULL, rname,
	RPT_HTML_ROUTINE_FIELD_LEN);
#if 0					    
    fprintf(aRptFile, "%s%-23s%s %s ",
	(vLast ? anchor : ""), rname, (vLast ? "</A>" : ""),
	tree_link(aDef, 0));
#else
    fprintf(aRptFile, "%s%-23s%s ",
	(vLast ? anchor : ""), rname, (vLast ? "</A>" : ""));
#endif
    fprintf(aRptFile,"%s%7ld%s %7ld ",
	source_link_prefix(aDef, 0), def_begin(aDef), source_link_suffix(),
	def_length(aDef));
    fprintf(aRptFile,"%s%7ld%s ",
	calls_link_prefix(aDef, 0), def_num_calls(aDef), calls_link_suffix());
    fprintf(aRptFile,"%s%7ld%s\n",
	xref_link_prefix(aDef, 0), def_num_callers(aDef), xref_link_suffix());
	
    while (pos < strlen(def_name(aDef))) {
	pos = fold_string(def_name(aDef), pos, "  ", rname,
	    RPT_HTML_ROUTINE_FIELD_LEN);
	fprintf(aRptFile, "%s\n", rname);
    }
}

/*************************************************************************++*/
void rpt_byfile_html_end(
/* Writes HTML-formatted report end for defined routine by file table.	    */

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
    fputs("-------------------------------------------------------\n",
	aRptFile);
    fprintf(aRptFile, "TOTAL: %5ld ROUTINES           %7ld AVG\n",
	source_routines(aSourceFile), source_avglen(aSourceFile));
    fputs("=======================================================\n",
	aRptFile);
    fprintf(aRptFile, "</PRE>\n<HR>\n%s\n", (vLast ? "<HR>\n" : ""));
}

/*************************************************************************++*/
void rpt_file_html_hdr(
/* Writes HTML-formatted report header for source files.		    */

    FILE    *aRptFile
	    /* (READ, BY ADDR):						    */
	    /* Report output file. Must be opened by caller.		    */

)	/* No return value.						    */
	/*****************************************************************--*/

{
    /*+									    */
    /*	This file is the main analysis HTML file, so first write the main   */
    /*	document header, then the section header.			    */
    /*-									    */

    rpt_html_section_hdr(aRptFile, MAIN_SECTION);    
    rpt_html_section_hdr(aRptFile, SOURCE_FILES_SECTION);    
    
    fputs("<PRE>\n", aRptFile);
    fputs(
"===============================================================================\n",
	aRptFile);
    fputs(
"                                 Com-    State-   Rou-              Avg   #\n",
	aRptFile);
    fputs(
"File                      Lines  mented    ment   tines  Length     Len   Calls\n",
	aRptFile);
    fputs(
"-------------------------------------------------------------------------------\n",
	aRptFile);
}

/*************************************************************************++*/
void rpt_file_html_entry(
/* Writes HTML-formatted source file entry.				    */

    FILE    *aRptFile,
	    /* (READ, BY ADDR):						    */
	    /* Report output file. Must be opened by caller.		    */

    SOURCEFILE
	    *aSourceFile,
	    /* (READ, BY ADDR):						    */
	    /* Source file entry to report.				    */

    int	    vFirst,
	    /* (READ, BY VAL):						    */
	    /* Flag indicating whether this is first entry in table.	    */

    int	    vLast
	    /* (READ, BY VAL):						    */
	    /* Flag indicating whether this is last entry in table.	    */
    
)	/* No return value.						    */
	/*****************************************************************--*/

{
					    /* File name folding buffer.    */
    char    fname[RPT_HTML_FILE_FIELD_LEN + 1]; 
    int	    pos;			    /* Pos in file name to print.   */

    /*+									    */
    /*	The file name may have to be folded over multiple lines. Embed a    */
    /*	hyperlink to the routines table for this file, and an anchor to the */
    /*	line if it is the last entry in this table.			    */
    /*-									    */

					    /* Get first part of file name. */
    pos = fold_string(source_name(aSourceFile), 0, NULL, fname,
	RPT_HTML_FILE_FIELD_LEN);

    fprintf(aRptFile, "%s%-23s%s %s%7ld%s %7ld %7ld ",
	(vLast ? "<A NAME=\"source_files_section_last\">" : ""),
	fname, (vLast ? "</A>" : ""), sourcefile_link_prefix(aSourceFile),
	source_lines(aSourceFile), source_link_suffix(),
	source_comments(aSourceFile) + source_mixed(aSourceFile),
	source_statements(aSourceFile) + source_mixed(aSourceFile));
    fprintf(aRptFile, "%s%7ld%s %7ld %7ld %7ld\n",
	(source_routines(aSourceFile) > 0 ? byfile_link_prefix(aSourceFile, 0)
	    : ""),
	source_routines(aSourceFile),
	(source_routines(aSourceFile) > 0 ? byfile_link_suffix() : ""),
	source_rlength(aSourceFile),
	source_avglen(aSourceFile), source_calls(aSourceFile));

    while (pos < strlen(source_name(aSourceFile))) {
	pos = fold_string(source_name(aSourceFile), pos, "  ", fname,
	    RPT_HTML_FILE_FIELD_LEN);
	fprintf(aRptFile, "%s\n", fname);
    }
}

/*************************************************************************++*/
void rpt_file_html_end(
/* Writes HTML-formatted report end for source files.			    */

    FILE    *aRptFile,
	    /* (READ, BY ADDR):						    */
	    /* Report output file. Must be opened by caller.		    */

    long    vTotalFiles
	    /* (READ, BY VAL):						    */
	    /* Total number of source files.				    */

)	/* No return value.						    */
	/*****************************************************************--*/

{
    fputs(
"-------------------------------------------------------------------------------\n",
	aRptFile);
	
    fprintf(aRptFile,
    "TOTAL: %7ld files    %7ld %7ld %7ld %7ld %7ld %7ld %7ld\n",
	vTotalFiles, total_lines(), 
	total_comments() + total_mixed(), total_statements() + total_mixed(),
	total_routines(), total_rlength(), total_avglen(), total_calls());
	
    fputs(
"===============================================================================\n",
	aRptFile);
    fputs("</PRE>\n<HR>\n<HR>\n\n", aRptFile);
}

/*************************************************************************++*/
void rpt_source_html_hdr(
/* Writes HTML-formatted report header for annotated source file.	    */

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

    if (vFirst) {
	rpt_html_section_hdr(aRptFile, SOURCE_CODE_SECTION);
	fputs("<HR>\n\n", aRptFile);
    }

    rpt_html_section_title(aRptFile, source_name(aSourceFile));
    fprintf(aRptFile, "<H2><A NAME=\"s%lx\">%s</a> Source Code</H2>\n",
	aSourceFile, source_name(aSourceFile));
    rpt_html_subsection_hdr(aRptFile, SOURCE_CODE_SECTION,
	(vLast ? NULL : next_entry(aSourceFile)),
	(vFirst ? NULL : prev_entry(aSourceFile)), OUTFILE_SUFFIX_SOURCE,
	source_seq(aSourceFile));
    fputs(
"<H3><A NAME=\"routines_in_this_file\">Routines In This File (Alphabetical)</a></H3>\n",
	aRptFile);
    
    fputs("<PRE>\n", aRptFile);
    if (source_routines(aSourceFile) == 0) {
	fputs("   None.\n", aRptFile);
    }
    else {
	fputs(" Line Name\n----- ----\n", aRptFile);
	for (curref = list_first(source_reflist(aSourceFile));
	    curref != NULL;
	    curref = next_entry(curref)) {
	    fprintf(aRptFile, "%5d %s%s%s\n",
		def_begin(ref_definition(curref)),
		source_link_prefix(ref_definition(curref), 1),
		def_name(ref_definition(curref)), source_link_suffix());
	}
	fputs("</PRE>\n", aRptFile);
    }
    
    fputs("<HR>\n<PRE>BEGINNING OF FILE\n</PRE>\n<HR>\n", aRptFile);
    fputs("<PRE>\n", aRptFile);
}

/*************************************************************************++*/
void rpt_source_html_entry(
/* Writes HTML-formatted entry for an annotated source line.		    */

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

    int	    vLine
	    /* (READ, BY VAL):						    */
	    /* Source file line number.					    */
    
)	/* No return value.						    */
	/*****************************************************************--*/

{
    DEFINITION				    /* Next definition in file.	    */
	    *nextdef;
	    
    if (aDef != NULL && def_begin(aDef) == vLine) {
	fprintf(aRptFile, "</PRE><HR>\n<A NAME=\"d%lx\">ROUTINE</A> ", aDef);
	fprintf(aRptFile, "%s%s%s. Go to: \n",
	    xref_link_prefix(aDef, 0), def_name(aDef), xref_link_suffix());
	if ((nextdef = next_entry(aDef)) != NULL) {
	    fprintf(aRptFile, "%sNext routine in file%s; ",
		source_link_prefix(nextdef, 1), source_link_suffix());
	}
#if 0
	fprintf(aRptFile,
"%s; %s; <A HREF=\"#routines_in_this_file\">Routines in this file</A>.<P>\n<PRE>\n",
	    xref_link(aDef, 0), tree_link(aDef, 0));
#else
	fputs(
"<A HREF=\"#routines_in_this_file\">Routines in this file</A>.<P>\n<PRE>\n",
	    aRptFile);
#endif
    }
    fprintf(aRptFile, "%6d: ", vLine);
    for (; *aSrcLine != '\0'; aSrcLine++) {
	switch (*aSrcLine) {
	case '>': fputs("&gt;", aRptFile); break;
	case '<': fputs("&lt;", aRptFile); break;
	case '&': fputs("&amp;", aRptFile); break;
	default: fputc(*aSrcLine, aRptFile);
	}
    }
    if (aDef != NULL && def_end(aDef) == vLine) {
	fprintf(aRptFile,
	"</PRE>\nEND %s. Go to: %sBeginning of routine%s.<P>\n<HR>\n<PRE>\n",
	    def_name(aDef), source_link_prefix(aDef, 1), source_link_suffix());
    }
}

/*************************************************************************++*/
void rpt_source_html_end(
/* Writes HTML-formatted report end for annotated source file.		    */

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
    fputs("<HR>\nEND OF FILE\n\n", aRptFile);
    fprintf(aRptFile, "TOTAL: %ld routines, %ld Avg Length\n",
	source_routines(aSourceFile), source_avglen(aSourceFile));
    fprintf(aRptFile, "</PRE>\n<HR>\n%s\n", (vLast ? "<HR>\n" : ""));
    rpt_html_subsection_hdr(aRptFile, SOURCE_CODE_SECTION,
	(vLast ? NULL : next_entry(aSourceFile)),
	(vFirst ? NULL : prev_entry(aSourceFile)), OUTFILE_SUFFIX_SOURCE,
	source_seq(aSourceFile));
}

