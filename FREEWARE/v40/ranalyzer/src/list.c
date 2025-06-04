/****************************************************************************/
/*									    */
/*  FACILITY:	Generic Support Library					    */
/*									    */
/*  MODULE:	List Management						    */
/*									    */
/*  AUTHOR:	Steve Branam, Network Product Support Group, Digital	    */
/*		Equipment Corporation, Littleton, MA, USA.		    */
/*									    */
/*  DESCRIPTION: This module contains routines to manage doubly-linked	    */
/*  lists of application objects. It supports addition of entries to and    */
/*  removal of entries from lists, for both ordered and unordered lists, as */
/*  well as list searching.						    */
/*									    */
/*  REVISION HISTORY:							    */
/*									    */
/*  V0.1-00 24-AUG-1994 Steve Branam					    */
/*									    */
/*	Original version.						    */
/*									    */
/****************************************************************************/

#include <stdio.h>
#include "list.h"

/*************************************************************************++*/
void *insert_list_entry(
/* Adds an item to a list at a given insertion point.			    */

    LIST    *aList,
	    /* (MODIFY, BY ADDR):					    */
	    /* List to which item is to be added. The list pointers may be  */
	    /* updated, and the item count will be incremented.		    */

    LIST_ENTRY_HDR
	    *aPrevious,
	    /* (MODIFY, BY ADDR):					    */
	    /* Insertion point in list. Item is added following this one,   */
	    /* linked through flink field; if NULL ptr is passed here, item */
	    /* is added at head of list.				    */

    LIST_ENTRY_HDR
	    *aItem
	    /* (MODIFY, BY ADDR):					    */
	    /* Item to be added. It is linked to aPrevious through blink    */
	    /* field; if no aPrevious item is specified, blink is NULL.	    */

)	/* Returns aItem, ptr to added item.				    */
	/*****************************************************************--*/

{
    if (aPrevious == NULL) {

	/*+								    */
	/*  Item is to be added at head of list. First, link it to first    */
	/*  entry and clear its backlink. Next, if list is empty, make item */
	/*  last; otherwise, link the first entry back to it. Finally, make */
	/*  item first.							    */
	/*-								    */
	
					    /* Set item links.		    */
	set_entry_flink(aItem, list_first(aList));
	set_entry_blink(aItem, NULL);
	if (isempty_list(aList)) {	    /* Empty list, item is last.    */
	    set_list_last(aList, aItem);
	}
	else {				    /* Link first to item.	    */
	    set_entry_blink(list_first(aList), aItem);
	}
	set_list_first(aList, aItem);	    /* Item is first.		    */
    }
    else {

	/*+								    */
	/*  Item is to be added following an existing entry.  First, link   */
	/*  it to existing entries forward and back (aPrevious is the	    */
	/*  previous entry, and its forward link points to the next entry). */
	/*  Next, if previous entry is currently last, make this item last; */
	/*  otherwise, link the next entry back to this item.  Finally,	    */
	/*  link previous entry forward to this one.			    */
	/*-								    */

					    /* Set item links.		    */
	set_entry_flink(aItem, entry_flink(aPrevious));
	set_entry_blink(aItem, aPrevious);
	if (islast_entry(aPrevious)) {	    /* Previous entry used to be    */
	    set_list_last(aList, aItem);    /* last, now item is.	    */
	}
	else {				    /* Link next back to item.	    */
	    set_entry_blink(entry_flink(aPrevious), aItem);
	}
	set_entry_flink(aPrevious, aItem);  /* Link prev forward to item.   */
    }
    inc_list_entries(aList);		    /* Update entry count.	    */
    return aItem;
}

/*************************************************************************++*/
void *insert_ordered_entry(
/* Adds an item to a list according to the list order specified by a	    */
/* comparator routine.							    */

    LIST    *aList,
	    /* (MODIFY, BY ADDR):					    */
	    /* List to which item is to be added. The list pointers may be  */
	    /* updated, and the item count will be incremented.		    */

    LIST_ENTRY_HDR
	    *aItem,
	    /* (MODIFY, BY ADDR):					    */
	    /* Item to be added. Its links will be updated to point to	    */
	    /* other list entries.					    */

    int	    (* aComparator)()
	    /* (EXEC, BY ADDR):						    */
	    /* Routine that compares item to existing items in list. The    */
	    /* routine must have the following interface:		    */
	    /*								    */
	    /*	    int comparator(listitem, aItem)			    */
	    /*								    */
	    /* where listitem is a ptr to an item from the list, and aItem  */
	    /* is the item being added here. The comparator returns 0 if    */
	    /* they are equal, less than 0 if list_item should precede	    */
	    /* aItem in the list, and greater than 0 if list_item should    */
	    /* follow aItem in the list.				    */
	    

)	/* Returns aItem, ptr to added item.				    */
	/*****************************************************************--*/

{
    LIST_ENTRY_HDR			    /* Current record ptr.	    */
	    *listitem;

    /*+									    */
    /*	If the list is empty, just put this item at the front. Otherwise,   */
    /*	search for appropriate insertion point using the comparator	    */
    /*	routine. If the end of the list is reached without finding a	    */
    /*	greater entry, this item should be last, so append it to the list.  */
    /*	Otherwise, insert it in the list in front of the greater item.	    */
    /*-									    */

    if (isempty_list(aList)) {
	prepend_list_entry(aList, aItem);
    }
    else {				 
	for (listitem = list_first(aList);  /* Search list.		    */
	    listitem != NULL && (*aComparator)(listitem, aItem) < 0; 
	    listitem = entry_flink(listitem));
	if (listitem == NULL) {		    /* None greater.		    */
	    append_list_entry(aList, aItem);
	}
	else {				    /* Found greater entry.	    */
	    insert_list_entry(aList, entry_blink(listitem), aItem);
	}
    }
    return aItem;
}
        
/*************************************************************************++*/
void *remove_list_entry(
/* Removes an item from a list.						    */

    LIST    *aList,
	    /* (MODIFY, BY ADDR)					    */
	    /* List from which item is to be removed. The list pointers may */
	    /* be updated, and the item count will be decremented.	    */

    LIST_ENTRY_HDR
	    *aItem
	    /* (MODIFY, BY ADDR)					    */
	    /* Item to be removed.					    */

)	/* Returns aItem, ptr to removed item.				    */
	/*****************************************************************--*/

{
    /*+									    */
    /*	First break and reform links in front of item. If item is the first */
    /*	entry, make the next first; otherwise, link previous entry forward  */
    /*	to next one. Next break and reform links following entry. If item   */
    /*	is the last entry, make the previous one last; otherwise, link next */
    /*	entry back to previous one. Finally, stomp the links in item so	    */
    /*	they can't accidentally be followed and update the entry count.	    */
    /*-									    */

    if (aItem == NULL) {		    /* Unspecified entry cannot be  */
	return NULL;			    /* removed.			    */
    }
    if (isfirst_entry(aItem)) {		    /* Make next entry first?	    */
	set_list_first(aList, entry_flink(aItem));
    }
    else {				    /* Or link previous to next.    */
	set_entry_flink(entry_blink(aItem), entry_flink(aItem));
    }
    if (islast_entry(aItem)) {		    /* Make previous entry last?    */
	set_list_last(aList, entry_blink(aItem));
    }
    else {				    /* Or link next to previous.    */
	set_entry_blink(entry_flink(aItem), entry_blink(aItem));
    }
    set_entry_flink(aItem, NULL);	    /* Stomp links in this entry.   */
    set_entry_blink(aItem, NULL);
    dec_list_entries(aList);		    /* Update entry count.	    */
    return aItem;
}

/*************************************************************************++*/
LIST_ENTRY_HDR *find_list_entry(
/* Searches for an item in a list without regard to any ordering.	    */

    LIST    *aList,
	    /* (READ, BY ADDR):						    */
	    /* List to search.						    */

    LIST_ENTRY_HDR
	    *aItem,
	    /* (READ, BY ADDR): 					    */
	    /* Item containing information be located. Note that any item   */
	    /* that meets the comparison will satisfy the search.	    */

    int	    (* aComparator)()
	    /* (EXEC, BY ADDR):						    */
	    /* Routine that compares item to existing items in list. See    */
	    /* the aComparator parameter of routine insert_ordered_entry    */
	    /* for a description.					    */

)	/* Returns ptr to found entry if found, or NULL if not found.	    */
	/*****************************************************************--*/

{
    LIST_ENTRY_HDR			    /* Current record ptr.	    */
	    *listitem;
    int	    cmpstat;			    /* Comparison status.	    */

    /*+									    */
    /*	If the list is empty, matching item cannot be in it. Otherwise,	    */
    /*	scan list for matching item. If end of list found, entry is not in  */
    /*	it.								    */
    /*-									    */

					    /* Ignore missing item, and	    */
					    /* don't search empty list.	    */
    if (aItem == NULL || isempty_list(aList)) {
    	return NULL;
    }
    else {				   
	for (listitem = list_first(aList);  /* Scan list for match.	    */
	    listitem != NULL &&
	    (cmpstat = (*aComparator)(listitem, aItem)) != 0; 
	    listitem = entry_flink(listitem)) {
	}
	if (cmpstat == 0) {
	    return listitem;		    /* Entry found, return it.	    */
	}
	else {
	    return NULL;		    /* Entry not found.		    */
	}
    }
}
        
/*************************************************************************++*/
LIST_ENTRY_HDR *find_ordered_entry(
/* Searches for an item in an ordered list according to the order specified */
/* by a comparator routine.						    */

    LIST    *aList,
	    /* (READ, BY ADDR):						    */
	    /* List to search.						    */

    LIST_ENTRY_HDR
	    *aItem,
	    /* (READ, BY ADDR): 					    */
	    /* Item containing information be located. Note that any item   */
	    /* that meets the comparison will satisfy the search.	    */

    int	    (* aComparator)()
	    /* (EXEC, BY ADDR):						    */
	    /* Routine that compares item to existing items in list. See    */
	    /* the aComparator parameter of routine insert_ordered_entry    */
	    /* for a description.					    */

)	/* Returns ptr to found entry if found, or NULL if not found.	    */
	/*****************************************************************--*/

{
    LIST_ENTRY_HDR			    /* Current record ptr.	    */
	    *listitem;
    int	    cmpstat;			    /* Comparison status.	    */

    /*+									    */
    /*	If the list is empty, matching item cannot be in it. Otherwise,	    */
    /*	scan list for matching item or next greater one.  If end of list or */
    /*	greater entry found, entry is not in it.			    */
    /*-									    */

					    /* Ignore missing item, and	    */
					    /* don't search empty list.	    */
    if (aItem == NULL || isempty_list(aList)) {
	return NULL;
    }
    else {				   
	for (listitem = list_first(aList);  /* Scan list for match.	    */
	    listitem != NULL &&
	    (cmpstat = (*aComparator)(listitem, aItem)) < 0; 
	    listitem = entry_flink(listitem)) {
	}
	if (cmpstat == 0) {
	    return listitem;		    /* Entry found, return it.	    */
	}
	else {
	    return NULL;		    /* Entry not found.		    */
	}
    }
}
        
