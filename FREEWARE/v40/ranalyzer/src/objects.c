/****************************************************************************/
/*									    */
/*  FACILITY:	Routine Analyzer					    */
/*									    */
/*  MODULE:	Object Management Support				    */
/*									    */
/*  AUTHOR:	Steve Branam, Network Product Support Group, Digital	    */
/*		Equipment Corporation, Littleton, MA, USA.		    */
/*									    */
/*  DESCRIPTION: This module contains all support routines for managing the */
/*  object types used by Routine Analyzer. They are: source files; routine  */
/*  definitions; and routine references. These objects are all descendents  */
/*  of the List Entry generic type, which means that they are linkable via  */
/*  a list entry header. This allows them to be stored in arbitrary ordered */
/*  and unordered lists, stacks, and queues. Support routines are supplied  */
/*  for creating and destroying objects from the heap, and for comparing,   */
/*  finding, and sorting them.						    */
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
char *def_ident(
/* Formats an indentification string for a definition entry. The string	    */
/* includes the routine name and the source file name (if known).	    */

    DEFINITION
	    *aDef
	    /* (READ, BY ADDR):  					    */
	    /* Definition entry to identify.				    */

)	/* Returns formatted string ptr.  WARNING: The returned ptr is the  */
	/* address of a statically-defined string buffer, so the string	    */
	/* must be read from it immediately.				    */
	/*****************************************************************--*/

{
					    /* Trace name string buffer.    */
    static char	strbuf[MAX_ROUTINE_IDENT + 1];

    sprintf(strbuf, "%s - %s", def_name(aDef),
	(isdefined_routine(aDef) ?
	source_name(def_source(aDef)) : "External"));
    return strbuf;
}

/*************************************************************************++*/
LANGUAGE_TRANSLATION *new_lang(
/* Creates and initializes a new language translation record.		    */

    char    *aFileExt,
	    /* (READ, BY ADDR):						    */
	    /* Source file extension string.				    */

    SOURCE_LANGUAGE
	    vCode
	    /* (READ, BY VAL):						    */
	    /* Source language code.					    */

)	/* Returns ptr to initialized record.				    */
	/*****************************************************************--*/

{
    LANGUAGE_TRANSLATION		    /* Ptr to new record.	    */
	    *record;

    if ((record = (LANGUAGE_TRANSLATION *)
	    obj_alloc(sizeof(LANGUAGE_TRANSLATION),
		sizeof(LANGUAGE_TRANSLATION), OBJ_NAME_LANGTRANS)) != NULL) {
	inc_nlang();
	record->code   = vCode;
	record->fext   = new_str(aFileExt, MAX_FILE_NAME, OBJ_NAME_LANG_EXT);
	if (trace_obj_enabled()) {
	    trace_new_obj(record, OBJ_NAME_LANGTRANS, aFileExt, num_lang());
	}
    }
    return record;
}

/*************************************************************************++*/
LANGUAGE_TRANSLATION *free_lang(
/* Frees a source language translation record. The file extension string is */
/* not freed; it is assumed to be picked up by another ptr.		    */

    LANGUAGE_TRANSLATION
	    *aLangTrans
	    /* (DELETE, BY ADDR):					    */
	    /* Translation block to free.				    */

)	/* Returns NULL ptr.						    */
	/*****************************************************************--*/

{
    dec_nlang();
    if (trace_obj_enabled()) {
	trace_free_obj(aLangTrans, OBJ_NAME_LANGTRANS, lang_fext(aLangTrans),
	    num_lang());
    }
    return obj_free(aLangTrans, sizeof(LANGUAGE_TRANSLATION),
	OBJ_NAME_LANGTRANS);
}

/*************************************************************************++*/
SOURCEFILE *new_file(
/* Creates and initializes a new source file record.			    */

    char    *aSourceName,
	    /* (READ, BY ADDR):						    */
	    /* Source file name string.					    */

    int	    vTabSize
	    /* (READ, BY VAL):						    */
	    /* Source text tab size.					    */

)	/* Returns ptr to initialized record.				    */
	/*****************************************************************--*/

{
    SOURCEFILE				    /* Ptr to new record.	    */
	    *record;

    if ((record = (SOURCEFILE *) obj_alloc(sizeof(SOURCEFILE),
	    sizeof(SOURCEFILE), OBJ_NAME_SOURCEFILE)) != NULL) {
	inc_nfiles();
	record->tabsize = vTabSize;
	record->seqnum	= num_files();
	record->name	= new_str(aSourceName, MAX_FILE_NAME,
			    OBJ_NAME_FILENAME);
	record->author	= new_str(PROGRAM_AUTHOR, sizeof(PROGRAM_AUTHOR),
			    OBJ_NAME_AUTHORNAME);
	if (trace_obj_enabled()) {
	    trace_new_obj(record, OBJ_NAME_SOURCEFILE, aSourceName,
		num_files());
	}
    }
    return record;
}

/*************************************************************************++*/
SOURCEFILE *free_file(
/* Frees a source file record.						    */

    SOURCEFILE
	    *aSourceFile
	    /* (DELETE, BY ADDR):					    */
	    /* Source file to free.					    */

)	/* Returns NULL ptr.						    */
	/*****************************************************************--*/

{
    dec_nfiles();
    if (trace_obj_enabled()) {
	trace_free_obj(aSourceFile, OBJ_NAME_SOURCEFILE,
	    source_name(aSourceFile), num_files());
    }
    free_str(source_name(aSourceFile));
    return obj_free(aSourceFile, sizeof(SOURCEFILE), OBJ_NAME_SOURCEFILE);
}

/*************************************************************************++*/
int compare_file(
/* Compares two file information records for ordering by file name. Entries */
/* are sorted alphabetically.						    */

    SOURCEFILE
	    *aSourceFile1,
	    /* (READ, BY ADDR):						    */
	    /* First record to compare.					    */

    SOURCEFILE
	    *aSourceFile2
	    /* (READ, BY ADDR):						    */
	    /* Second record to compare.				    */
	    
)	/* Returns status value indicating comparison results:		    */
	/*    0	- File names are equal.					    */
	/*  < 0 - File name of aSourceFile1 is less than name of	    */
	/*	  aSourceFile2.						    */
	/*  > 0 - File name of aSourceFile1 is greater than name of	    */
	/*	  aSourceFile2.						    */
	/*****************************************************************--*/

{
    return strcmp(source_name(aSourceFile1), source_name(aSourceFile2));
}

/*************************************************************************++*/
DEFINITION *new_def(
/* Creates and initializes a new routine definition record.		    */

    char    *aName,
	    /* (READ, BY ADDR):						    */
	    /* Routine name string.					    */

    SOURCEFILE
	    *aSourceRecord
	    /* (READ, BY ADDR):						    */
	    /* Source file record.					    */

)	/* Returns ptr to initialized record.				    */
	/*****************************************************************--*/

{
    DEFINITION				    /* Ptr to new record.	    */
	    *record;

    if ((record = (DEFINITION *) obj_alloc(sizeof(DEFINITION),
	    sizeof(DEFINITION), OBJ_NAME_DEFINITION)) != NULL) {
	inc_ndefs();
	record->name = new_str(aName, MAX_ROUTINE_NAME, OBJ_NAME_ROUTINENAME);
	set_def_source(record, aSourceRecord);
	if (trace_obj_enabled()) {
	    trace_new_obj(record, OBJ_NAME_DEFINITION, aName, num_defs());
	    if (aSourceRecord != NULL) {
		printf("       Source file %s @ %lxh\n",
		    source_name(aSourceRecord), aSourceRecord);
	    }
	    else {
		puts("       No source file (routine call, not definition)");
	    }
	}
    }
    return record;
}

/*************************************************************************++*/
DEFINITION *free_def(
/* Frees a routine definition record.					    */

    DEFINITION
	    *aDef
	    /* (DELETE, BY ADDR):					    */
	    /* Definition to free.					    */

)	/* Returns NULL ptr.						    */
	/*****************************************************************--*/

{
    dec_ndefs();
    if (trace_obj_enabled()) {
	trace_free_obj(aDef, OBJ_NAME_DEFINITION, def_ident(aDef),
	    num_defs());
    }
    return obj_free(aDef, sizeof(DEFINITION), OBJ_NAME_DEFINITION);
}

/*************************************************************************++*/
void trace_def(
/* Traces an action on a definition entry.				    */

    DEFINITION
	    *aDef,
	    /* (READ, BY ADDR):  					    */
	    /* Definition entry to trace.				    */

    char    *aAction
	    /* (READ, BY ADDR):  					    */
	    /* Action string describing what is being done to entry.	    */

)	/* No return value.      					    */
	/*****************************************************************--*/

{
    if (trace_obj_enabled()) {
	printf("TRACE: %s %s %s @ %lxh\n", aAction, OBJ_NAME_DEFINITION,
	    def_ident(aDef), aDef);
    }
}

/*************************************************************************++*/
int compare_def(
/* Compares two routine definition entries for ordering by routine, file    */
/* names, and line number (some languages allow multiple definition of the  */
/* same routine name within different scopes in the same module). Entries   */
/* are sorted alphabetically, with a special provision. If either or both   */
/* routine is currently undefined (i.e. the source file is not yet known),  */
/* they will be considered equal, since either 1) they are both forward	    */
/* references to an as yet undefined routine, or 2) one is the actual	    */
/* definition, while the other is just a reference, in which case the	    */
/* source information from the defined one is copied to the undefined one.  */

    DEFINITION
	    *aDef1,
	    /* (MODIFY, BY ADDR):					    */
	    /* First entry to compare. The source information may be	    */
	    /* updated.							    */

    DEFINITION
	    *aDef2
	    /* (MODIFY, BY ADDR):					    */
	    /* Second entry to compare. The source information may be	    */
	    /* updated.		   					    */
	    
)	/* Returns status value indicating comparison results:		    */
	/*    0	- Routine, file names, and line numbers are equal.	    */
	/*  < 0 - Routine/file name/line of aDef1 less than aDef2.	    */
	/*  > 0 - Routine/file name/line of aDef1 greater than aDef2.	    */
	/*****************************************************************--*/

{
    int	    cmpstat;			    /* Comparison status.	    */

    /*+									    */
    /*	Compare routine names. If they are unequal, no further comparison   */
    /*	is needed. Otherwise, if both routines are defined, compare their   */
    /*	files names and line numbers to determine final comparison status.  */
    /*-									    */

    if ((cmpstat = ustrncmp(def_name(aDef1), def_name(aDef2),
	max(strlen(def_name(aDef1)), strlen(def_name(aDef2))))) == 0) {
	if (isdefined_routine(aDef1) && isdefined_routine(aDef2)) {
	    if ((cmpstat = compare_file(def_source(aDef1), def_source(aDef2)))
		== 0) {
		cmpstat = def_begin(aDef1) - def_begin(aDef2);
	    }
	}				    
	else {

	    /*+								    */
	    /*	One or both is undefined, assume either one is the actual   */
	    /*	definition and the other is actually a reference to it, or  */
	    /*	both are actually just references. In either case they are  */
	    /*	equal in that they represent the same routine. Copy the	    */
	    /*	source information from the defined one to the undefined    */
	    /*	one so they will both be truly equal.			    */
	    /*								    */
		    
	    cmpstat = 0;
	    if (isdefined_routine(aDef2)) { /* aDef2 is the defined one.    */
		set_def_source(aDef1, def_source(aDef2));
	    }
	    else {			    /* Vice versa (or both undef).  */
		set_def_source(aDef2, def_source(aDef1));
	    }
	}				    
    }
    return cmpstat;
}

/*************************************************************************++*/
int compare_def_file(
/* Compares two routine definition entries for ordering by file name and    */
/* line number, i.e. by the order in which they occurred in the files.	    */
/* Entries are sorted alphabetically, with the provision that undefined	    */
/* routines sort before defined ones.					    */

    DEFINITION
	    *aDef1,
	    /* (READ, BY ADDR):						    */
	    /* First entry to compare.					    */

    DEFINITION
	    *aDef2
	    /* (READ, BY ADDR):						    */
	    /* Second entry to compare.					    */
	    
)	/* Returns status value indicating comparison results:		    */
	/*    0	- File and line numbers are equal.			    */
	/*  < 0 - File/line of aDef1 less than aDef2.			    */
	/*  > 0 - File/line of aDef1 greater than aDef2.		    */
	/*****************************************************************--*/

{
    int	    cmpstat;			    /* Comparison status.	    */

    /*+									    */
    /*	If both routines are defined, compare file names and line numbers.  */
    /*	Otherwise, undefined one is less than defined one (or both are	    */
    /*	undefined, and entries are considered equal).			    */
    /*-									    */

    if (isdefined_routine(aDef1) && isdefined_routine(aDef2)) {
	if ((cmpstat = compare_file(def_source(aDef1), def_source(aDef2)))
	    == 0) {
	    cmpstat = def_begin(aDef1) - def_begin(aDef2);
	}
    }
    else if (isdefined_routine(aDef2)) {    /* aDef2 is the defined one.    */
	cmpstat = -1;
    }
    else if (isdefined_routine(aDef1)) {    /* Vice versa.		    */
	cmpstat = 1;
    }
    else {				    /* Both undefined.		    */
	cmpstat = 0;
    }
    return cmpstat;
}

/*************************************************************************++*/
DEFINITION *find_def(
/* Searches the global routine list for a routine definition entry from the */
/* specified source file. If an entry is not found, creates and initializes */
/* a new one and returns it.						    */

    char    *aName,
	    /* (READ, BY ADDR):						    */
	    /* Routine name string to find.				    */

    SOURCEFILE
	    *aSourceRecord
	    /* (READ, BY ADDR):						    */
	    /* Source file record to find. If a NULL ptr is passed, any	    */
	    /* routine definition from any file will satisfy the request.   */
	    /* In any case, if a definition entry must be created, the	    */
	    /* value passed as aSourceRecord will be used to initialize the */
	    /* definition source file.					    */


)	/* Returns ptr to found (and possibly created) record.		    */
	/*****************************************************************--*/

{
    DEFINITION				    /* Ptr to search entry.	    */
	    *searchdef;
    DEFINITION				    /* Ptr to found entry.	    */
	    *curdef;

    /*+									    */
    /*	Create a temporary search entry and use it to search the definition */
    /*	list for a match. If a match is found, discard the search entry and */
    /*	return the found entry. Otherwise, add the search entry to the list */
    /*	and return it as the "found" entry. This guarantees that this	    */
    /*	routine will always "find" an entry.				    */
    /*-									    */

    searchdef = new_def(aName, aSourceRecord);
    if ((curdef = find_ordered_entry(global_deflist(), searchdef,
		    compare_def)) != NULL) {
	free_def(searchdef);		    /* Found match, return it.	    */
	return curdef;
    }
    else {				    /* Match not found, use search  */
	add_def(searchdef);		    /* entry.			    */
	return searchdef;
    }
}

/*************************************************************************++*/
REFERENCE *new_ref(
/* Creates and initializes a new routine reference record.		    */

    long    vLine,
	    /* (READ, BY VAL):						    */
	    /* Line number in source file where routine is called.	    */

    DEFINITION
	    *aRefDef,
	    /* (READ, BY ADDR):						    */
	    /* Refenced routine's definition record.			    */

    DEFINITION
	    *aCallerDef
	    /* (READ, BY ADDR):						    */
	    /* Caller's definition record.				    */

)	/* Returns ptr to initialized record.				    */
	/*****************************************************************--*/

{
    REFERENCE				    /* Ptr to new record.	    */
	    *record;

    if ((record = (REFERENCE *) obj_alloc(sizeof(REFERENCE), sizeof(REFERENCE),
	    OBJ_NAME_REFERENCE)) != NULL) {
	inc_nrefs();
	record->definition = aRefDef;
	record->line       = vLine;
	record->caller     = aCallerDef;
	if (trace_obj_enabled()) {
	    trace_new_obj(record, OBJ_NAME_REFERENCE, def_name(aRefDef), 
		num_refs());
	    printf("       Definition is @ %lxh, caller is %s @ %lxh\n",
		aRefDef, (aCallerDef == NULL ? "(none)" : def_name(aCallerDef)),
		(aCallerDef == NULL ? NULL : aCallerDef));
	}
    }
    return record;
}

/*************************************************************************++*/
REFERENCE *free_ref(
/* Frees a routine reference record.					    */

    REFERENCE
	    *aRef
	    /* (DELETE, BY ADDR):					    */
	    /* Reference to free.					    */

)	/* Returns NULL ptr.						    */
	/*****************************************************************--*/

{
    dec_nrefs();
    if (trace_obj_enabled()) {
	trace_free_obj(aRef, OBJ_NAME_REFERENCE,
	    def_ident(ref_definition(aRef)), num_refs());
    }
    return obj_free(aRef, sizeof(REFERENCE), OBJ_NAME_DEFINITION);
}

/*************************************************************************++*/
void trace_ref(
/* Traces an action on a reference entry.				    */

    REFERENCE
	    *aRef,
	    /* (READ, BY ADDR):  					    */
	    /* Reference entry to trace.				    */

    char    *aAction
	    /* (READ, BY ADDR):  					    */
	    /* Action string describing what is being done to entry.	    */

)	/* No return value.      					    */
	/*****************************************************************--*/

{
    if (trace_obj_enabled()) {
	printf("TRACE: %s %s %s @ %lxh\n", aAction, OBJ_NAME_REFERENCE,
	    def_ident(ref_definition(aRef)), aRef);
    }
}

/*************************************************************************++*/
int compare_caller(
/* Compares two reference entries for ordering by caller routine. Entries   */
/* are sorted alphabetically, and by line number within the same caller.    */

    REFERENCE
	    *aRef1,
	    /* (READ, BY ADDR):						    */
	    /* First entry to compare.					    */

    REFERENCE
	    *aRef2
	    /* (READ, BY ADDR):						    */
	    /* Second entry to compare. 				    */
	    
)	/* Returns status value indicating comparison results:		    */
	/*    0	- Callers are equal.					    */
	/*  < 0 - Caller from aRef1 is less than aRef2.			    */
	/*  > 0 - Caller from aRef1 is greater than aRef2.		    */
	/*****************************************************************--*/

{
    int	    cmpstat;			    /* Comparison status.	    */

    /*+									    */
    /*	If the caller names are the same, compare the line numbers.	    */
    /*-									    */

    if ((cmpstat = compare_def(ref_caller(aRef1), ref_caller(aRef2))) == 0) {
	cmpstat = ref_offset(aRef1) - ref_offset(aRef2);
    }
    return cmpstat;
}

/*************************************************************************++*/
int compare_caller_name(
/* Compares two reference entries for equality by caller routine name.	    */
/* Entries are not sorted by this routine.				    */

    REFERENCE
	    *aRef1,
	    /* (READ, BY ADDR):						    */
	    /* First entry to compare.					    */

    REFERENCE
	    *aRef2
	    /* (READ, BY ADDR):						    */
	    /* Second entry to compare. 				    */
	    
)	/* Returns status value indicating comparison results:		    */
	/*    0	- Callers are equal.					    */
	/*   -1 - Callers are not equal.				    */
	/*****************************************************************--*/

{
    /*+									    */
    /*	Compare the caller names.					    */
    /*-									    */

    if (strcmp(def_name(ref_caller(aRef1)), def_name(ref_caller(aRef2))) != 0) {
	return -1;
    }
    else {
	return 0;
    }
}

/*************************************************************************++*/
int compare_ref(
/* Compares two reference entries for equality by routine definition.	    */
/* Entries are not sorted by this routine.				    */

    REFERENCE
	    *aRef1,
	    /* (READ, BY ADDR):						    */
	    /* First entry to compare.					    */

    REFERENCE
	    *aRef2
	    /* (READ, BY ADDR):						    */
	    /* Second entry to compare. 				    */
	    
)	/* Returns status value indicating comparison results:		    */
	/*    0	- Definitions are equal.				    */
	/*   -1 - Definitions are not equal.				    */
	/*****************************************************************--*/

{
    /*+									    */
    /*	Compare the definitions.					    */
    /*-									    */

    if (compare_def(ref_definition(aRef1), ref_definition(aRef2)) != 0) {
	return -1;
    }
    else {
	return 0;
    }
}

/*************************************************************************++*/
REFERENCE *find_ref_in_tree(
/* Find the first reference to a routine in a call tree.		    */

    DEFINITION
	    *aTreeRoot,
	    /* (READ, BY ADDR):						    */
	    /* Routine definition entry that is root of this call tree.	    */

    char    *aName
	    /* (READ, BY ADDR):						    */
	    /* Routine name to search for.				    */

)	/* Returns ptr to found reference, or NULL if no matching reference */
	/* found.							    */
	/*****************************************************************--*/

{
    REFERENCE				    /* Current reference being	    */
	    *curref;			    /* checked at this level.	    */
    REFERENCE				    /* Reference found in a	    */
	    *foundref;			    /* subtree.			    */

    /*+									    */
    /*	For each routine referenced at this level, see if it matches the    */
    /*	name. If not, recursively search that routine's subtree for a match */
    /*	and return the match if found. If no match is found at this level   */
    /*	or any sublevels, no such reference exists in this tree.	    */
    /*-									    */

    for (curref = list_first(def_refs(aTreeRoot)); curref != NULL;
	curref = next_entry(curref)) {
	if (strcmp(def_name(ref_definition(curref)), aName) == 0) {
	    return curref;		    /* Found it at this level!	    */
	}
	else if ((foundref = find_ref_in_tree(ref_definition(curref), aName))
	    != NULL) {
	    return foundref;		    /* Found it in a subtree!	    */
	}
    }
    return NULL;			    /* Did not find it anywhere.    */
}

/*************************************************************************++*/
void discard_dup_refs(
/* Discards any duplicate reference entries in any definition ref and	    */
/* caller lists. This leaves at most one reference for each routine in any  */
/* list.								    */

    /* No arguments.							    */

)	/* No return value.						    */
	/*****************************************************************--*/

{
    DEFINITION					/* Current routine entry.   */
	    *curdef;
    REFERENCE					/* Current routine	    */
	    *curref;				/* reference.		    */
    LIST    curlist;				/* Current list being	    */
						/* trimmed.		    */

    for (curdef = list_first(global_deflist()); curdef != NULL;
	curdef = next_entry(curdef)) {

	/*+								    */
	/*  First, if the current routine is defined, trim its reference    */
	/*  list. To do this, move the reference list to a temporary list,  */
	/*  clearing the definition's list. For each reference in the	    */
	/*  temporary list, see if a matching entry already exists in the   */
	/*  definition's list. If so, this is a duplicate, discard it;	    */
	/*  otherwise, add it to the definition's list. This regrows the    */
	/*  definition's list back with only one reference to each routine. */
	/*-								    */

	if (isdefined_routine(curdef)) {
	    trace_def(curdef, "Trim refs");
	    copy_list(def_refs(curdef), &curlist);
	    init_list(def_refs(curdef));
	    while ((curref = dequeue_entry(&curlist)) != NULL) {
		if (find_ref(curdef, curref)) {		
		    free_ref(curref);		/* Duplicate, discard it.   */
		}
		else {
		    add_ref(curdef, curref);	/* New one, keep it.	    */
		    trace_ref(curref, "Keep ref");
		}
	    }
	}

	/*+								    */
	/*  Now, regardless of whether or not the routine is defined, trim  */
	/*  its caller list, using the same procedure.			    */
	/*-								    */

	trace_def(curdef, "Trim callers");
						/* Move list to temporary.  */
	copy_list(def_callers(curdef), &curlist);
	init_list(def_callers(curdef));
	while ((curref = dequeue_entry(&curlist)) != NULL) {
	    if (find_caller(curdef, curref)) {
		free_ref(curref);		/* Duplicate, discard it.   */
	    }
	    else {
		add_caller(curdef, curref);	/* New one, keep it.	    */
		trace_ref(curref, "Keep caller");
	    }
	}
    }
}

/*************************************************************************++*/
void sort_file_order(
/* Resorts the global definition list in order by file, then routine within */
/* file.								    */

    /* No arguments.							    */

)	/* No return value.						    */
	/*****************************************************************--*/

{
    DEFINITION					/* Current routine entry.   */
	    *curdef;
    LIST    templist;				/* Temporary list.	    */

    /*									    */
    /*	Perform an insertion sort on the global definition list.  To do	    */
    /*	this, move the definition list to a temporary list, clearing the    */
    /*	definition list. For each definition, dequeue it from the temporary */
    /*	list and insert it back into the definition list according the	    */
    /*	file/routine/line ordering.					    */
    /*-									    */

    copy_list(global_deflist(), &templist);
    init_list(global_deflist());
    while ((curdef = dequeue_entry(&templist)) != NULL) {
	trace_def(curdef, "Sort file order");
	insert_ordered_entry(global_deflist(), curdef, compare_def_file);
    }
}

