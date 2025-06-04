#module UAF_MISC "X-3"

/*
**++
**  FACILITY:	Authorization record maintenance utility
**
**  MODULE DESCRIPTION:
**
**		This module contains miscellaneous routines needed by the
**		DECwindows AUTHORIZE utility.
**
**  AUTHORS:	L. Mark Pilant		CREATION DATE:  12-Nov-1992
**
**  MODIFICATION HISTORY:
**
**	X-3	LMP		L. Mark Pilant,		 8-OCT-1993  15:43
**		Add AUTHORIZE$MATCH_NAME routine.
**
**	X-2	LMP		L. Mark Pilant,		 2-MAR-1993  15:40
**		Convert to C from BLISS sources.
**
**	X-1	LMP		L. Mark Pilant		12-Nov-1992
**		Original version.
**
**--
*/

/*
**
**  INCLUDE FILES
**
*/

#include <SSDEF>
#include <TPADEF>

#include "uaf_header"

/*
**
**  FORWARD ROUTINES
**
*/

#pragma noinline (AUTHORIZE$CONVERT_UIC)
#pragma noinline (AUTHORIZE$ITMLST_ADD_ITEM)
#pragma noinline (AUTHORIZE$ITMLST_COMPARE)
#pragma noinline (AUTHORIZE$ITMLST_COPY)
#pragma noinline (AUTHORIZE$ITMLST_DELETE)
#pragma noinline (AUTHORIZE$ITMLST_MERGE)
#pragma noinline (AUTHORIZE$ITMLST_UPDATE)
#pragma noinline (AUTHORIZE$MATCH_NAME)

extern unsigned int AUTHORIZE$CONVERT_UIC (uic_string, uic_value)

char				*uic_string;
unsigned int			*uic_value;
{
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine is called to convert a UIC string to a value.
**
**  FORMAL PARAMETERS:
**
**	UIC_STRING	- Pointer to the UIC string
**	UIC_VALUE	- Address of a cell to contain the converted UIC
**
**  RETURN VALUE:
**
**	Status from LIB$TPARSE.
**
**  SIDE EFFECTS:
**
**	None
**
**--
*/

/* External routines. */

#if defined (__ALPHA) || defined (__alpha)
extern			LIB$TABLE_PARSE ();
#else
extern			LIB$TPARSE ();
#endif /* defined (__ALPHA) || defined (__alpha) */

/* Global storage. */

globalref unsigned int	converted_uic;			/* Converted UIC value */
globalref unsigned char	uic_key[];			/* TPARSE key tables */
globalref unsigned char	uic_state[];			/* TPARSE state tables */
globalref struct tpadef	tparse_block;			/* TPARSE argument block */

/* Local storage. */

unsigned int		status;				/* Routine exit status */

/* Initialize needed storage. */

converted_uic = 0;
status = SS$_NORMAL;					/* Assume success */

/* In there is something to convert, do it.  Otherwise, simply skip the
** following which will allow a zero UIC value to be returned. */

if (strlen (uic_string) != 0)
    {
    tparse_block.tpa$l_stringcnt = strlen (uic_string);
    tparse_block.tpa$l_stringptr = uic_string;

/* On an OpenVMS/AXP system (and also OpenVMS/VAX Version 6) the parse
** routine is called LIB$TABLE_PARSE.  On an OpenVMS/VAX system the parse
** routine is called LIB$TPARSE.  Compile the appropriate source. */

#if defined (__ALPHA) || defined (__alpha)
    status = LIB$TABLE_PARSE (&tparse_block, uic_state, uic_key);
#else
    status = LIB$TPARSE (&tparse_block, uic_state, uic_key);
#endif /* defined (__ALPHA) || defined (__alpha) */
    }

/* Return the converted UIC if necessary. */

if (uic_value != 0) *uic_value = converted_uic;

return status;
}

extern void AUTHORIZE$ITMLST_ADD_ITEM (itmlst, item_code, entry_size, entry)

struct ITMDEF			(**itmlst)[];
int				item_code;
int				entry_size;
unsigned char			*entry;
{
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine is called to add an entry to a dynamic item list.  To
**	do this, a buffer is allocated an the item list extended so the new
**	entry may be added to the end of the item list.
**
**	This routine assumes the buffer information is disjoint from the
**	actual item lust.  This allows just the item list to be freed up
**	without having to diddle with all the individual buffers.
**
**  FORMAL PARAMETERS:
**
**	ITMLST		- Address of a pointer containing the pointer to
**			  the dynamic item list
**	ITEM_CODE	- Item code for the new item list entry
**	ENTRY_SIZE	- Number of bytes for the new item list entry
**	ENTRY		- Pointer to the buffer containing the information
**			  for the new item list entry
**
**  RETURN VALUE:
**
**	None
**
**  SIDE EFFECTS:
**
**	None
**
**--
*/

/* External routines. */

extern void		AUTHORIZE$ITMLST_DELETE ();
extern void		AUTHORIZE$ITMLST_COPY ();

/* Local storage. */

int			itmlst_size = 0;		/* Size of the item list to free up */
int			itmlst_index = 0;		/* Index into item list */
struct ITMDEF		(*new_itmlst)[] = 0;		/* New allocated item list */
unsigned int		status;				/* Routine exit status */

/* Copy the input item list, and request enough room to be added for the
** new item list entry. */

AUTHORIZE$ITMLST_COPY (*itmlst,
		       &new_itmlst,
		       itm$c_length);

/* Figure out where the current item list is terminated. */

if (*itmlst != 0)
    {
    while ((**itmlst)[itmlst_index].itm$w_itmcod != 0) itmlst_index++;
    }

/* Allocate storage for the new item list entry buffer.  Also fill the
** other parts of the new item list entry.  */

(*new_itmlst)[itmlst_index].itm$w_itmcod = item_code;
(*new_itmlst)[itmlst_index].itm$w_bufsiz = entry_size;
(*new_itmlst)[itmlst_index].itm$l_bufadr = malloc (entry_size);

/* If the allocation fails, free up the memory allocated for the new item
** list, and signal a failure status. */

if ((*new_itmlst)[itmlst_index].itm$l_bufadr == 0)
    {
    AUTHORIZE$ITMLST_DELETE (&new_itmlst);		/* Delete the copy */
    free (new_itmlst);
    LIB$SIGNAL (UAF$_NOPROCMEM);
    return;
    }

/* Copy the information from the supplied buffer to the newly allocated buffer. */

memmove ((*new_itmlst)[itmlst_index].itm$l_bufadr, entry, entry_size);

/* Since the new item list is complete, delete the original item list. */

if (*itmlst != 0) AUTHORIZE$ITMLST_DELETE (itmlst);

/* Return a pointer to the new item list. */

*itmlst = new_itmlst;
}

extern void AUTHORIZE$ITMLST_COMPARE (first_itmlst, second_itmlst, itmlst_out)

struct ITMDEF			first_itmlst[];
struct ITMDEF			second_itmlst[];
struct ITMDEF			(**itmlst_out)[];
{
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine is called to compare two item lists and produce a third
**	item list with only the differences; using the first item list as the
**	source for the new item list.
**
**  FORMAL PARAMETERS:
**
**	FIRST_ITMLST	- Address of the first item list to compare
**	SECOND_ITMLST	- Address of the second item list to compare
**	ITMLST_OUT	- Address of a pointer containing the pointer to the
**			  new item list
**
**  RETURN VALUE:
**
**	None
**
**  SIDE EFFECTS:
**
**	None
**
**--
*/

/* External routines. */

extern void		AUTHORIZE$ITMLST_ADD_ITEM ();
extern void		AUTHORIZE$ITMLST_COPY ();
extern void		AUTHORIZE$ITMLST_DELETE ();

/* Local storage. */

short int		add_to_output;			/* Simple flag */
int			first_itmlst_index;		/* Index into first item list */
struct ITMDEF		(*new_itmlst)[] = 0;		/* New item list */
struct ITMDEF		(*second_itmlst_copy)[] = 0;	/* Local copy of the second item list */
int			second_itmlst_index;		/* Index into second item list */
unsigned int		status;				/* Routine exit status */

/* Before starting, copy the second item list.  This will allow the item entries
** to be flagged (by setting the item code to -1) as being processed.  This means
** it is possible to transfer any remaining item entries from the second item list
** to the output item list. */

AUTHORIZE$ITMLST_COPY (second_itmlst,
		       &second_itmlst_copy,
		       0);

/* Compare the item list entries in each of the two input item lists, and
** produce a third item list containing the differences (using the first item
** list as the source).
**
** Because there is no assumed order in either of the input item lists, the
** comparison takes a little more time (but not a whole lot of effort).  The
** comparison is done by taking each item list entry from the first list (in
** turn) and checking to see if it exists in the ssecond item list.  If it
** does not exist, or it exists and the data is different, add it to the third
** item list. */

/* Loop through each item list entry in the first item list. */

for (first_itmlst_index = 0; first_itmlst[first_itmlst_index].itm$w_itmcod != 0; first_itmlst_index++)
    {
    add_to_output = 1;				/* Assume entry will be added to output list */

/* Loop through each item list entry in the second item list. */

    for (second_itmlst_index = 0; (*second_itmlst_copy)[second_itmlst_index].itm$w_itmcod != 0; second_itmlst_index++)
	{

/* Look for an item code match.  If the item codes match, check the item entry
** size and finally the buffer contents. */

	if (first_itmlst[first_itmlst_index].itm$w_itmcod == (*second_itmlst_copy)[second_itmlst_index].itm$w_itmcod)
	    {

/* Flag this item entry as being processed. */

	    (*second_itmlst_copy)[second_itmlst_index].itm$w_itmcod = -1;

/* If the buffer sizes do not match, trip out early since the buffer contents
** cannot possibly match. */

	    if (first_itmlst[first_itmlst_index].itm$w_bufsiz != (*second_itmlst_copy)[second_itmlst_index].itm$w_bufsiz) break;

/* Since the buffer sizes match, check the buffer contents.  If the buffer
** contents match, do not add the item list entry to the third item list. */

	    if (memcmp (first_itmlst[first_itmlst_index].itm$l_bufadr,
			(*second_itmlst_copy)[second_itmlst_index].itm$l_bufadr,
			first_itmlst[first_itmlst_index].itm$w_bufsiz) == 0)
		add_to_output = 0;			/* Do not add entry to output list */

/* Since the item codes from the two item list entries matched, there is no
** need to continue on in the second item list.  Trip out now. */

	    break;
	    }
	}

/* At this point, the second item list has been searched for the entry from the
** first item list.  Either a match has been found, a similar entry with different
** data has been found, or the entry has not been found.
**
** If the entry has not been found or it is different, add it to the third item
** list. */

    if (add_to_output != 0)
	AUTHORIZE$ITMLST_ADD_ITEM (&new_itmlst,
				   first_itmlst[first_itmlst_index].itm$w_itmcod,
				   first_itmlst[first_itmlst_index].itm$w_bufsiz,
				   first_itmlst[first_itmlst_index].itm$l_bufadr);
    }

/* Since the entire first item list has been processed, scan through all the
** entries of the second item list looking for any item entries not processed.
** These are then added to the output item list.
**
** Note the USERNAME item entry is not copied, if it is present.  This is not
** done because $SETUAI does not know to ignore it; and will return a bad
** parameter error status if it is present. */

for (second_itmlst_index = 0; (*second_itmlst_copy)[second_itmlst_index].itm$w_itmcod != 0; second_itmlst_index++)
    {
    if ((*second_itmlst_copy)[second_itmlst_index].itm$w_itmcod != 0xffff &&
	(*second_itmlst_copy)[second_itmlst_index].itm$w_itmcod != UAI$_USERNAME)
	{
	AUTHORIZE$ITMLST_ADD_ITEM (&new_itmlst,
				   (*second_itmlst_copy)[second_itmlst_index].itm$w_itmcod,
				   (*second_itmlst_copy)[second_itmlst_index].itm$w_bufsiz,
				   (*second_itmlst_copy)[second_itmlst_index].itm$l_bufadr);
	}
    }

/* Now delete the copy of the second item list. */

AUTHORIZE$ITMLST_DELETE (&second_itmlst_copy);

/* If there is an existing dynamic (output) item list, delete it. */

if (*itmlst_out != 0) AUTHORIZE$ITMLST_DELETE (itmlst_out);

/* If no new item list was created, because the item lists were the same,
** create a null item list. */

if (new_itmlst == 0)
    {
    new_itmlst = calloc (1, LONGWORD);		/* Single longword item list terminator */
    if (new_itmlst == 0) LIB$SIGNAL (UAF$_NOPROCMEM);
    }

/* Return the address of the new item list. */

*itmlst_out = new_itmlst;
}

extern void AUTHORIZE$ITMLST_COPY (itmlst_in, itmlst_out, additional_size)

struct ITMDEF			itmlst_in[];
struct ITMDEF			(**itmlst_out)[];
int				additional_size;
{
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine is called to copy an item list.  This is done by allocating
**	a new item listand needed buffers and then copying the information in
**	the original buffers.
**
**  FORMAL PARAMETERS:
**
**	ITMLST_IN	- Address of the item list to copy
**	ITMLST_OUT	- Address of a pointer containing the pointer to the
**			  new item list
**	ADDITIONAL_SIZE	- Number of additional bytes to add to the new item list
**
**  RETURN VALUE:
**
**	None
**
**  SIDE EFFECTS:
**
**	None
**
**--
*/

/* External routines. */

extern void	AUTHORIZE$ITMLST_DELETE ();

/* Local storage. */

int		itmlst_size = 0;		/* Size of item list to copy */
int		itmlst_index = 0;		/* Index into item list */
struct ITMDEF	(*new_itmlst)[];		/* New allocated item list */
unsigned int	status;				/* Routine exit status */

/* Special case when there is no input item list.  In this case, the
** output item list will be created with the terminator (item code 0)
** as the only thing in the new item list. */

if (itmlst_in == 0)
    {
    itmlst_size = 4;						/* For the terminator longword */
    itmlst_size += additional_size;				/* Any additional space as requested */

/* Allocate and clear a new item list. */

    new_itmlst = calloc (1, itmlst_size);
    if (new_itmlst == 0) LIB$SIGNAL (UAF$_NOPROCMEM);

/* If there is an existing dynamic (output) item list, delete it. */

    if (*itmlst_out != 0) AUTHORIZE$ITMLST_DELETE (itmlst_out);

/* Return the address of the new item list. */

    *itmlst_out = new_itmlst;

/* Exit stage left. */

    return;
    }

/* Figure out how big the input item list is. */

for (itmlst_index = 0; itmlst_in[itmlst_index].itm$w_itmcod != 0; itmlst_index++)
    itmlst_size += itm$c_length;
itmlst_size += 4;						/* Account for the terminator longword. */

/* If there is a need for additional bytes to be added to the new item
** list, add in the required number of bytes. */

itmlst_size += additional_size;

/* Allocate the main body of the new item list.  This is done with calloc
** instead of malloc because any additional space must be cleared. */

new_itmlst = calloc (1, itmlst_size);
if (new_itmlst == 0) LIB$SIGNAL (UAF$_NOPROCMEM);

/* Copy the main body of the item list. */

memmove (new_itmlst, itmlst_in, itmlst_size - additional_size);

/* Now look at each item list entry, and copy the buffer information. */

itmlst_index = 0;						/* Reset the item list index */

for (itmlst_index = 0; itmlst_in[itmlst_index].itm$w_itmcod != 0; itmlst_index++)
    {

/* If the buffer size of the item list entry is zero, simply set the buffer
** address to be that of the item list entry.  This avoids any potential
** problems with bufer address validation; even through the buffer size is
** zero. */

    if (itmlst_in[itmlst_index].itm$w_bufsiz == 0)
	(*new_itmlst)[itmlst_index].itm$l_bufadr = &(*new_itmlst)[itmlst_index];
    else
	{

/* Othersize, allocate a new buffer and copy the contents. */

	(*new_itmlst)[itmlst_index].itm$l_bufadr = malloc (itmlst_in[itmlst_index].itm$w_bufsiz);

/* If the allocation fails, run through the new item list and free up any
** memory allocated to buffers.  These buffers can be identified by having
** an address that is not the item list entry address and is not the same
** as the input item list entry buffer address.  In the former case, there
** is nothing to deallocate, and in the latter case, a new buffer has not
** yet been allocated (which also means the list processing can terminate. */

	if ((*new_itmlst)[itmlst_index].itm$l_bufadr == 0)
	    {
	    for (itmlst_index = 0; itmlst_in[itmlst_index].itm$w_itmcod != 0; itmlst_index++)
		{

/* If the buffer address in the input and output item lists is the same,
** list processing can stop. */

		if (itmlst_in[itmlst_index].itm$l_bufadr ==
		    (*new_itmlst)[itmlst_index].itm$l_bufadr) break;

/* See if there is anything to deallocate. */

		if ((*new_itmlst)[itmlst_index].itm$l_bufadr !=
		    &(*new_itmlst)[itmlst_index]) free ((*new_itmlst)[itmlst_index].itm$l_bufadr);

		}

/* Deallocate the main body of the new item list. */

	    free (new_itmlst);

/* Signal a failure status. */

	    LIB$SIGNAL (UAF$_NOPROCMEM);
	    }

/* Copy the actual buffer information. */

	memmove ((*new_itmlst)[itmlst_index].itm$l_bufadr, itmlst_in[itmlst_index].itm$l_bufadr,
							   itmlst_in[itmlst_index].itm$w_bufsiz);
	}
    }

/* If there is an existing dynamic (output) item list, delete it. */

if (*itmlst_out != 0) AUTHORIZE$ITMLST_DELETE (itmlst_out);

/* Return the address of the new item list. */

*itmlst_out = new_itmlst;
}

extern void AUTHORIZE$ITMLST_DELETE (itmlst)

struct ITMDEF			(**itmlst)[];
{
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine is called free up a dynamic item list.  This is done by
**	first traversing the item list, freeing up the memory for any of the
**	buffers.  Then the actual item list is deallocated.
**
**  FORMAL PARAMETERS:
**
**	ITMLST		- Address of a pointer containing the pointer to the
**			  item list to delete
**
**  RETURN VALUE:
**
**	None
**
**  SIDE EFFECTS:
**
**	The item list pointer is cleared.
**
**--
*/

/* Local storage. */

int		itmlst_index;			/* Index into item list */

/* Initialize needed storage. */

itmlst_index = 0;

/* If there is an item list to delete, do it.  Otherwise simply return. */

if (*itmlst != 0)
    {
    for (itmlst_index = 0; (**itmlst)[itmlst_index].itm$w_itmcod != 0; itmlst_index++)
	{

/* See if there is anything to deallocate. */

	if ((**itmlst)[itmlst_index].itm$l_bufadr != 0) free ((**itmlst)[itmlst_index].itm$l_bufadr);
	}

/* Deallocate the main body of the new item list. */

    free (*itmlst);

/* Note there is now no dynamic item list. */

    *itmlst = 0;
    }
}

extern void AUTHORIZE$ITMLST_MERGE (first_itmlst, second_itmlst, itmlst_out)

struct ITMDEF			first_itmlst[];
struct ITMDEF			second_itmlst[];
struct ITMDEF			(**itmlst_out)[];
{
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine is called to merge two item lists and produce a third item
**	list containing all the entries from the first item list and those entries
**	from the second item list not present in the first item list.
**
**  FORMAL PARAMETERS:
**
**	FIRST_ITMLST	- Address of the first item list to compare
**	SECOND_ITMLST	- Address of the second item list to compare
**	ITMLST_OUT	- Address of a pointer containing the pointer to the
**			  new item list
**
**  RETURN VALUE:
**
**	None
**
**  SIDE EFFECTS:
**
**	None
**
**--
*/

/* External routines. */

extern void		AUTHORIZE$ITMLST_ADD_ITEM ();
extern void		AUTHORIZE$ITMLST_COPY ();
extern void		AUTHORIZE$ITMLST_DELETE ();

/* Local storage. */

struct ITMDEF		(*first_itmlst_copy)[] = 0;	/* Local copy of the first item list */
int			first_itmlst_index;		/* Index into first item list */
struct ITMDEF		(*second_itmlst_copy)[] = 0;	/* Local copy of the second item list */
int			second_itmlst_index;		/* Index into second item list */

/* Before starting, copy the item lists.  This will allow a new item list
** to be built using the first item list as a starting point.   It will
** also allow the item entries in the (copied) second itemlist to be
** flagged (by setting the item code to -1) as being processed.  This means
** it is possible to transfer any remaining item entries from the second
** item list to the new item list. */


AUTHORIZE$ITMLST_COPY (first_itmlst,
		       &first_itmlst_copy,
		       0);
AUTHORIZE$ITMLST_COPY (second_itmlst,
		       &second_itmlst_copy,
		       0);

/* Compare the item list entries in each of the two input item lists.  Flag
** those common entries (in the second item list) to avoid using them.
** After both lists have been processed, take the unflagged entries and add
** them to the new list.
**
** Note: The process of traversing these lists is fairly brute force; an inner
** loop (to traverse the second list entries) within an outer loop (to traverse
** the first list entries).  This does add a bit of overhead.
**
** A more elegant solution would be to have a three phase approach in the outer
** loop.  The first phase (pass) would find the maximum item code seen for all
** entries in the first item list.  This would allow an appropaitely sized bit
** stream to be created.  The second phase (pass) would set a bit in the bit
** stream (corresponding to the item code) indicating the item code was in use
** in the first item list.  The third phase (third and successive passes) would
** simply have to check to see if the bit corresponding to the item code from
** the second item list was set in the bit stream.  If it is, the item list
** entry is present in the first item list and should be ignored.
**
** In addition to having the first two phases size and populate the bit stream,
** it would be possible to flag the item list entries in the second item list
** at the same time; so no effort was wasted. */

/* Loop through each item list entry in the first item list. */

for (first_itmlst_index = 0; (*first_itmlst_copy)[first_itmlst_index].itm$w_itmcod != 0; first_itmlst_index++)
    {

/* Loop through each item list entry in the second item list. */

    for (second_itmlst_index = 0; (*second_itmlst_copy)[second_itmlst_index].itm$w_itmcod != 0; second_itmlst_index++)
	{

/* Look for an item code match.  If the item codes match, check the item entry
** size and finally the buffer contents. */

	if ((*first_itmlst_copy)[first_itmlst_index].itm$w_itmcod == (*second_itmlst_copy)[second_itmlst_index].itm$w_itmcod)
	    {

/* Flag this item entry as being processed. */

	    (*second_itmlst_copy)[second_itmlst_index].itm$w_itmcod = -1;

/* Since the item codes from the two item list entries matched, there is no
** need to continue on in the second item list.  Trip out now. */

	    break;
	    }
	}
    }

/* Since the entire first item list has been processed, scan through all the
** entries of the second item list looking for any item entries not processed.
** These are then added to the new item list.
**
** Note the USERNAME item entry is not copied, if it is present.  This is not
** done because $SETUAI does not know to ignore it; and will return a bad
** parameter error status if it is present. */

for (second_itmlst_index = 0; (*second_itmlst_copy)[second_itmlst_index].itm$w_itmcod != 0; second_itmlst_index++)
    {
    if ((*second_itmlst_copy)[second_itmlst_index].itm$w_itmcod != 0xffff &&
	(*second_itmlst_copy)[second_itmlst_index].itm$w_itmcod != UAI$_USERNAME)
	{
	AUTHORIZE$ITMLST_ADD_ITEM (&first_itmlst_copy,
				   (*second_itmlst_copy)[second_itmlst_index].itm$w_itmcod,
				   (*second_itmlst_copy)[second_itmlst_index].itm$w_bufsiz,
				   (*second_itmlst_copy)[second_itmlst_index].itm$l_bufadr);
	}
    }

/* Now delete the copy of the second item list. */

AUTHORIZE$ITMLST_DELETE (&second_itmlst_copy);

/* If there is an existing dynamic (output) item list, delete it. */

if (*itmlst_out != 0) AUTHORIZE$ITMLST_DELETE (itmlst_out);

/* Return the address of the new item list. */

*itmlst_out = first_itmlst_copy;
}

extern void AUTHORIZE$ITMLST_UPDATE (itmlst, item_code, entry_size, entry)

struct ITMDEF			(**itmlst)[];
int				item_code;
int				entry_size;
unsigned char			*entry;
{
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine is called to update an entry in a dynamic item list.
**	If an entry with the specified item code already exists, it is replaced
**	with the information supplied (to this routine).  If it does not exist,
**	it is added to the item list.
**
**  FORMAL PARAMETERS:
**
**	ITMLST		- Address of a pointer containing the pointer to
**			  the dynamic item list
**	ITEM_CODE	- Item code for the new item list entry
**	ENTRY_SIZE	- Number of bytes for the new item list entry
**	ENTRY		- Pointer to the buffer containing the information
**			  for the new item list entry
**
**  RETURN VALUE:
**
**	None
**
**  SIDE EFFECTS:
**
**	None
**
**--
*/

/* External routines. */

extern void	AUTHORIZE$ITMLST_ADD_ITEM ();

/* Local storage. */

int		itmlst_index;			/* Index into item list */
unsigned int	status;				/* Routine exit status */

/* Initialize needed storage. */

itmlst_index = 0;						/* Set initial index */

/* Traverse the item list looking for the specified item code. */

for (itmlst_index = 0;; itmlst_index++)
    {

/* If the end of the item list is reached, the entry did not exist.  Add a new
** entry to the end of the existing item list. */

    if ((**itmlst)[itmlst_index].itm$w_itmcod == 0)
	{
	AUTHORIZE$ITMLST_ADD_ITEM (itmlst, item_code, entry_size, entry);
	return;
	}

/* If an entry was found with the specified item code, trip out of the loop now. */

    if ((**itmlst)[itmlst_index].itm$w_itmcod == item_code) break;
    }

/* At this point, the index is pointing to the entry with the desired item code.
** deallocate the existing buffer, if any, allocate a new buffer and copy the
** supplied information.  If the allocation failes, signal a failure status. */

if ((**itmlst)[itmlst_index].itm$l_bufadr != 0) free ((**itmlst)[itmlst_index].itm$l_bufadr);	/* Free up original */
(**itmlst)[itmlst_index].itm$l_bufadr = malloc (entry_size);					/* Allocate a new */
if ((**itmlst)[itmlst_index].itm$l_bufadr == 0) LIB$SIGNAL (UAF$_NOPROCMEM);			/* Abort on failure */
memmove ((**itmlst)[itmlst_index].itm$l_bufadr, entry, entry_size);				/* Move contents */
(**itmlst)[itmlst_index].itm$w_bufsiz = entry_size;						/* Set new size */
}

extern unsigned int AUTHORIZE$MATCH_NAME (candidate_length, candidate_string, pattern_length, pattern_string)

unsigned short int		candidate_length;
char				*candidate_string;
unsigned short int		pattern_length;
char				*pattern_string;
{
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine is a jacket for STR$MATCH_WILD.  Originally, this utility
**	used FMG$MATCH_NAME, but that was not generally available.  STR$MATCH_WILD
**	does the same thing but the interface is much more cumbersome in C, so
**	I am using this jacket routine to make things a little easier and cleaner.
**
**  FORMAL PARAMETERS:
**
**	CANDIDATE_LENGTH	- Number of bytes in the candidate string
**	CANDIDATE_STRING	- Pointer to the candidate string text
**	PATTERN_LENGTH		- Number of bytes in the pattern string
**	PATTERN_STRING		- Pointer to the pattern string text
**
**  RETURN VALUE:
**
**	Status from STR$MATCH_WILD
**
**  SIDE EFFECTS:
**
**	None
**
**--
*/

/* Local storage. */

int		candidate_desc[2];		/* Simple candidate string descriptor */
int		pattern_desc[2];		/* Simple pattern string descriptor */

/* Initialize needed storage. */

candidate_desc[0] = candidate_length;
candidate_desc[1] = candidate_string;
pattern_desc[0] = pattern_length;
pattern_desc[1] = pattern_string;

/* Return the comparison status. */

return STR$MATCH_WILD (candidate_desc, pattern_desc);
}
