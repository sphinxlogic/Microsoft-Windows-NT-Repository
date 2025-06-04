/****************************************************************************/
/*									    */
/*  FACILITY:	Routine Analyzer					    */
/*									    */
/*  MODULE:	Object Management Support Header			    */
/*									    */
/*  AUTHOR:	Steve Branam, Network Product Support Group, Digital	    */
/*		Equipment Corporation, Littleton, MA, USA.		    */
/*									    */
/*  DESCRIPTION: This header file contains all type definitions for the	    */
/*  object types used by Routine Analyzer. Member access routines (get/set  */
/*  values) and a number of object management routines are implemented here */
/*  as macros.								    */
/*									    */
/*  REVISION HISTORY:							    */
/*									    */
/*  V0.1-01 09-JAN-1994 Steve Branam                                        */  
/*                                                                          */
/*      Replace LANGUAGE_C with LANGUAGE_CC to avoid conflict with Unix     */
/*      C compiler predefined symbol.                                       */
/*									    */
/*  V0.1-00 24-AUG-1994 Steve Branam					    */
/*									    */
/*	Original version.						    */
/*									    */
/****************************************************************************/

#ifndef __OBJECTS_H
#define __OBJECTS_H

/*									    */
/* String object names.							    */
/*									    */

#define OBJ_NAME_FILENAME	"File name"
#define OBJ_NAME_LANG_EXT	"Language file extension"
#define OBJ_NAME_ROUTINENAME	"Routine name"
#define OBJ_NAME_AUTHORNAME	"Program author"

/****************************************************************************/
/*									    */
/* LIST ENTRY OBJECTS							    */
/*									    */
/*	This general class of objects can be strung together on a	    */
/*	doubly-linked list. There are several specific subtypes in this	    */
/*	general class:							    */
/*									    */
/*	Language Translation Record					    */
/*									    */
/*	    Contains all the information for a file extension-to-language   */
/*	    translation.						    */
/*									    */
/*	Source File Record						    */
/*									    */
/*	    Contains all the information for a single source file.	    */
/*									    */
/*	Routine Definition Record					    */
/*									    */
/*	    Contains all the information for the definition of a single	    */
/*	    routine.							    */
/*									    */
/*	Routine Reference Record					    */
/*									    */
/*	    Contains all the information for a given reference (call) to    */
/*	    a routine.							    */
/*									    */
/****************************************************************************/

/****************************************************************************/
/*									    */
/* Language translation record object type.				    */
/*									    */
/****************************************************************************/

#define OBJ_NAME_LANGTRANS	"LANGUAGE_TRANSLATION"

typedef enum {				    /* List of known languages.	    */
    LANGUAGE_UNKNOWN,
    LANGUAGE_TEXT,
    LANGUAGE_DCL,
    LANGUAGE_BLISS,
    LANGUAGE_CC
} SOURCE_LANGUAGE;

typedef struct lang_trans_record {
    LIST_ENTRY_HDR			    /* List linkage.		    */
	    entry_hdr;
    char    *fext;			    /* File extension string.	    */
    SOURCE_LANGUAGE			    /* Language code.		    */
	    code;
} LANGUAGE_TRANSLATION;

/*									    */
/* Language translation member access routines.				    */
/*									    */

#define lang_fext(l) ((l)->fext)
#define lang_code(l) ((l)->code)
#define set_lang_fext(l, s) ((l)->fext = s)
#define set_lang_code(l, x) ((l)->code = x)

/*									    */
/* Language translation management routines.				    */
/*									    */

#define add_lang(l) (enqueue_entry(global_langlist(), l))
#define remove_lang() (dequeue_entry(global_langlist()))

/****************************************************************************/
/*									    */
/* Source file record object type.					    */
/*									    */
/****************************************************************************/

#define OBJ_NAME_SOURCEFILE	"SOURCEFILE"

typedef struct source_record {
    LIST_ENTRY_HDR			    /* List linkage.		    */
	    entry_hdr;
    int	    seqnum;			    /* File sequence number.	    */
    char    *name;			    /* File name.		    */
    char    *author;			    /* Program author, that's me!   */
    int	    tabsize;			    /* Source text tab size.	    */
    long    lines;			    /* Total number of lines.	    */
    long    comments;			    /* Number comment-only lines.   */
    long    statements;			    /* Number statement-only lines. */
    long    mixed;			    /* Number mixed comment/	    */
					    /* statement lines.		    */
    long    routines;			    /* Number routines defined.	    */
    long    rlength;			    /* Length of all routine defs.  */
    long    calls;			    /* Number of routine calls made */
					    /* in this file.		    */
    LIST    reflist;			    /* List of routines defined in  */
					    /* this file, treated as	    */
					    /* callers.			    */
    int	    byfilefile;			    /* By-file report file number   */
					    /* for linking.		    */
} SOURCEFILE;

/*									    */
/* Source file record member access routines.				    */
/*									    */

#define source_seq(src) ((src)->seqnum)
#define source_name(src) ((src)->name)
#define source_lines(src) ((src)->lines)
#define source_tabsize(src) ((src)->tabsize)
#define source_line(src) ((src)->lines + 1)
#define source_comments(src) ((src)->comments)
#define source_statements(src) ((src)->statements)
#define source_mixed(src) ((src)->mixed)
#define source_empty(src) ((src)->lines-(src)->mixed-(src)->comments-(src)->statements)
#define source_routines(src) ((src)->routines)
#define source_rlength(src) ((src)->rlength)
#define source_avglen(src) (source_routines(src)==0?0:((source_rlength(src) * 10)/source_routines(src) + 5)/ 10)
#define source_calls(src) ((src)->calls)
#define source_reflist(src) (&(src)->reflist)
#define source_byfilefile(src) ((src)->byfilefile)
#define set_source_byfilefile(src, f) ((src)->byfilefile = f)
#define inc_source_comments(src) ((src)->lines++,(src)->comments++)
#define inc_source_statements(src) ((src)->lines++,(src)->statements++)
#define inc_source_mixed(src) ((src)->lines++,(src)->mixed++)
#define inc_source_empty(src) ((src)->lines++)
#define inc_source_routines(src) ((src)->routines++)
#define inc_source_rlength(src, x) ((src)->rlength += x)
#define inc_source_calls(src, x) ((src)->calls += x)

/*									    */
/* Source file record management routines.				    */
/*									    */

#define add_srcref(s,r) (insert_ordered_entry(&(s)->reflist, r, compare_caller))

/****************************************************************************/
/*									    */
/* Routine definition record object type.				    */
/*									    */
/****************************************************************************/

typedef enum {
    NOT_YET_EVALUATED,
    TREE_REQUIRED,
    TREE_NOT_REQUIRED
} TREE_REQ;

#define OBJ_NAME_DEFINITION	"DEFINITION"

typedef struct definition_record {
    LIST_ENTRY_HDR			    /* List linkage.		    */
	    entry_hdr;
    char    *name;			    /* Routine name.		    */
    struct source_record
	    *source;			    /* Ptr to source file record.   */
    struct definition_record		    /* Ptr to root of call tree.    */
	    *rootdef;
    long    begin;			    /* Beginning line in source.    */
    long    end;			    /* Ending line in source.	    */
    LIST    refs;			    /* List of referenced routines. */
    LIST    callers;			    /* List of referencing	    */
					    /* routines.		    */
    TREE_REQ				    /* Call tree required flag.	    */
	    needs_tree;
    int	    treesize;			    /* Number of nodes in tree.	    */
    int	    xreffile;			    /* Xref report file number for  */
					    /* linking.			    */
} DEFINITION;

/*									    */
/* Routine definition record member access routines.			    */
/*									    */

#define def_name(def) ((def)->name)
#define def_source(def) ((def)->source)
#define def_root(def) ((def)->rootdef)
#define def_begin(def) ((def)->begin)
#define def_end(def) ((def)->end)
#define def_length(def) (def_end(def) - def_begin(def) + 1)
#define def_refs(def) (&(def)->refs)
#define def_callers(def) (&(def)->callers)
#define def_tree_required(def) ((def)->needs_tree)
#define def_tree_size(def) ((def)->treesize)
#define def_xreffile(def) ((def)->xreffile)
#define set_def_source(def, s) ((def)->source = s)
#define set_def_root(def, x) ((def)->rootdef = x)
#define set_def_begin(def, x) ((def)->begin = x)
#define set_def_end(def, x) ((def)->end = x)
#define set_def_tree_req(def, x) ((def)->needs_tree = x)
#define set_def_tree_size(def, x) ((def)->treesize = x)
#define set_def_xreffile(def, f) ((def)->xreffile = f)
#define def_num_callers(def) (list_entries(&(def)->callers))
#define def_num_calls(def) (list_entries(&(def)->refs))

/*									    */
/* Routine definition record management routines.			    */
/*									    */

#define add_ref(d, r) (append_list_entry(&(d)->refs, r))
#define push_ref(d, r) (push_entry(&(d)->refs, r))
#define pop_ref(d) (pop_entry(&(d)->refs))
#define add_caller(d,r) (insert_ordered_entry(&(d)->callers, r, compare_caller))
#define find_ref(d,r) (find_list_entry(&(d)->refs, r, compare_ref))
#define find_caller(d,r) (find_list_entry(&(d)->callers, r, compare_caller_name))
#define isdefined_routine(def) ((def)->source != NULL)
#define isalready_expanded(def, root) (!tree_inline_disabled() && isdefined_routine(def) && (def)->rootdef == root)

/****************************************************************************/
/*									    */
/* Routine reference record object type.				    */
/*									    */
/****************************************************************************/

#define OBJ_NAME_REFERENCE	"REFERENCE"

typedef struct reference_record {
    LIST_ENTRY_HDR			    /* List linkage.		    */
	    entry_hdr;
    struct definition_record		    /* Ptr to routine definition    */
	    *definition;		    /* record.			    */
    long    line;			    /* Line in source where called. */
    struct definition_record		    /* Ptr to routine definition    */
	    *caller;			    /* record of caller.	    */
} REFERENCE;

/*									    */
/* Routine reference record member access routines.			    */
/*									    */

#define ref_definition(ref) (ref->definition)
#define ref_name(ref) (def_name(ref_definition(ref)))
#define ref_line(ref) (ref->line)
#define ref_caller(ref) (ref->caller)
#define ref_offset(ref) (ref_line(ref) - def_begin(ref_caller(ref)) + 1)

/*									    */
/* Routine reference record management routines.			    */
/*									    */

#define isrecursive_ref(ref, root) (find_ref(root, ref) != NULL)

/****************************************************************************/
/*									    */
/* Forward references.							    */
/*									    */
/****************************************************************************/

REFERENCE *find_ref_in_tree();
int compare_ref();
int compare_file();
int compare_caller();
char *def_ident();

#endif /* #ifndef __OBJECTS_H */
