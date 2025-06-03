/****************************************************************************/
/*									    */
/*  FACILITY:	Generic Support Library					    */
/*									    */
/*  MODULE:	List Management						    */
/*									    */
/*  AUTHOR:	Steve Branam, Network Product Support Group, Digital	    */
/*		Equipment Corporation, Littleton, MA, USA.		    */
/*									    */
/*  DESCRIPTION: This header file contains all type definitions for the	    */
/*  generic list object types used by Routine Analyzer. Member access	    */
/*  routines (get/set values) and a number of object management routines    */
/*  are implemented here as macros.					    */
/*									    */
/*  REVISION HISTORY:							    */
/*									    */
/*  V0.1-00 24-AUG-1994 Steve Branam					    */
/*									    */
/*	Original version.						    */
/*									    */
/****************************************************************************/

/****************************************************************************/
/*									    */
/* List object type.							    */
/*									    */
/****************************************************************************/

typedef struct {
    long    entries;			    /* Number of items in list.	    */
    struct list_entry_hdr		    /* Ptr to first item in list.   */
	    *head;
    struct list_entry_hdr		    /* Ptr to last item in list.    */
	    *tail;
} LIST;

/*									    */
/* List object member access routines.					    */
/*									    */

#define list_entries(l) ((l)->entries)
#define list_first(l) ((l)->head)
#define list_last(l) ((l)->tail)
#define inc_list_entries(l) ((l)->entries++)
#define dec_list_entries(l) ((l)->entries--)
#define set_list_first(l, p) ((l)->head = p)
#define set_list_last(l, p) ((l)->tail = p)

/*									    */
/* List object management routines.					    */
/*									    */

#define append_list_entry(l, e) insert_list_entry((l), list_last(l), e)
#define prepend_list_entry(l, e) insert_list_entry((l), NULL, e)
#define copy_list(s, d) ((d)->head=(s)->head,(d)->tail=(s)->tail,(d)->entries=(s)->entries)
#define init_list(l) ((l)->head=NULL, (l)->tail=NULL, (l)->entries=0)
#define isempty_list(l) ((l)->head == NULL)

/*									    */
/* Treat a list as a stack: last-in, first-out.				    */
/*									    */

#define push_entry(s, e) append_list_entry(s, e)
#define pop_entry(s) remove_list_entry(s, list_last(s))

/*									    */
/* Treat a list as a queue: first-in, first-out.			    */
/*									    */

#define enqueue_entry(s, e) append_list_entry(s, e)
#define dequeue_entry(s) remove_list_entry(s, list_first(s))

/****************************************************************************/
/*									    */
/* List entry header object. Application list entry objects are expected to */
/* define the following as the first element of their structures:	    */
/*									    */
/*	LIST_ENTRY_HDR   entry_hdr;					    */
/*									    */
/****************************************************************************/

typedef struct list_entry_hdr {
    struct list_entry_hdr		    /* Ptr to next record in list.  */
	    *flink;
    struct list_entry_hdr		    /* Ptr to previous record.	    */
	    *blink;
} LIST_ENTRY_HDR;

/*									    */
/* Entry header object member access routines.				    */
/*									    */

#define entry_blink(e) ((e)->blink)
#define entry_flink(e) ((e)->flink)
#define next_entry(e) ((e) == NULL ? NULL : entry_flink(&((e)->entry_hdr)))
#define prev_entry(e) ((e) == NULL ? NULL : entry_blink(&((e)->entry_hdr)))
#define set_entry_flink(e, p) (e->flink = p)
#define set_entry_blink(e, p) (e->blink = p)

/*									    */
/* Entry header object management routines.				    */
/*									    */

#define isfirst_entry(e) (entry_blink(e) == NULL)
#define islast_entry(e) (entry_flink(e) == NULL)
#define isbegin_of_list(e) (prev_entry(e) == NULL)
#define isend_of_list(e) (next_entry(e) == NULL)

