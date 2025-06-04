#module UAF_DW_IDENTIFIERS "X-1"

/*
**++
**  FACILITY:	Authorization record maintenance utility
**
**  MODULE DESCRIPTION:
**
**		This module contains the various support routines needed by
**		the DECwindows AUTHORIZE utility to manage identifiers.
**
**  AUTHORS:	L. Mark Pilant			CREATION DATE:   4-Jun-1993
**
**  MODIFICATION HISTORY:
**
**	X-1	LMP		L. Mark Pilant,		 4-Jun-1993
**		Original version.
**
**--
*/

/*
**
**  INCLUDE FILES
**
*/

#include <UICDEF>

#include ctype
#include stdlib

#include <decw$include:mrmappl>

#include "uaf_header"

/*
**
**  FORWARD ROUTINES
**
*/

#pragma noinline (AUTHORIZE$IDENTIFIERS)
#pragma noinline (AUTHORIZE$IDENTIFIERS_UPDATE)
#pragma noinline (AUTHORIZE$MODIFY_IDENTIFIER)
#pragma noinline (AUTHORIZE$SET_UP_ATTRIBUTES)
#pragma noinline (AUTHORIZE$SET_UP_IDENTIFIER)
#pragma noinline (AUTHORIZE$SET_UP_SYSTEM_ID)

extern void AUTHORIZE$IDENTIFIERS (widget_id, tag, reason)

Widget				widget_id;
int				*tag;
XmAnyCallbackStruct		*reason;
{
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine is called to initiate processing of identifiers
**	for an authorization entry.
**
**  FORMAL PARAMETERS:
**
**	{@subtags@}
**
**  RETURN VALUE:
**
**	None
**
**  SIDE EFFECTS:
**
**	{@description or none@}
**
**--
*/

/* External routines. */

extern void			AUTHORIZE$DECW_ERROR ();
extern unsigned int		AUTHORIZE$FIND_USER_ENTRY ();
extern void			AUTHORIZE$IDENTIFIERS_UPDATE ();
extern void			AUTHORIZE$MODIFY_IDENTIFIER ();
extern void			AUTHORIZE$SET_UP_ATTRIBUTES ();
extern void			AUTHORIZE$SET_UP_IDENTIFIER ();
extern void			AUTHORIZE$SET_UP_SYSTEM_ID ();

/* Global references. */

globalref unsigned int		uaf_l_current_user_uic;		/* Current user UIC */
globalref Widget		uaf_r_identifier_window_widget;	/* Identifier window widget info */
globalref struct USRLSTDEF	uaf_r_master_list;		/* Complete user list (from the file) */
globalref Widget		uaf_r_system_id_window_widget;	/* System identifier window widget info */
globalref Widget		uaf_r_widget_id_array[];	/* Array of Widget IDs in use */

/* Local storage. */

unsigned int			id_attributes;			/* Identifier attributes */
char				**identifier;			/* Identifier selected */
int				index;				/* General index */
unsigned int			status;				/* Routine exit status */
char				*temp_string;			/* Temp string pointer */
char				text_string [MAX_USERNAME];	/* Text string (from main window) storage */
struct USRLSTDEF		*user_entry;			/* Current user list entry */
int				widget_number = *tag;
short int			wildcard_name = 0;		/* Wildcard indicator */

/* Debug information. */

#ifdef DEBUGGING
printf ("identifier processing\n");
#endif /* DEBUGGING */

/* Based upon the widget number, determine what should be done. */

switch (widget_number)
    {
    case uaf$c_main_identifiers:
	{

/* Get the supplied username from the main window, convert it to all upper case
** characters and see if there are wildcards present. */

	temp_string = XmTextGetString (uaf_r_widget_id_array[uaf$c_main_username]);
	strcpy (text_string, temp_string);					/* Copy the widget text */
	for (index = 0; index < strlen (text_string); index++)
	    {
	    text_string[index] = toupper (text_string[index]);			/* Upcase character */
	    if (text_string[index] == '*') wildcard_name = 1;			/* Check for */
	    if (text_string[index] == '%') wildcard_name = 1;			/*   wildcards */
	    }
	XtFree (temp_string);

/* Since the name is now uppercased, reflect it in the main window. */

	XmTextSetString (uaf_r_widget_id_array[uaf$c_main_username],
			 text_string);

/* If there are any wildcards, complain and abort the operation. */

	if (wildcard_name != 0)
	    {
	    AUTHORIZE$DECW_ERROR (UAF$_NOWILDCARDS);
	    return;
	    }

/* Make sure the user already exists. */

	if (FALSE (AUTHORIZE$FIND_USER_ENTRY (&uaf_r_master_list.uaf$l_usrlst_flink,
					      &text_string,
					      FIND_NAME_EQUAL,
					      0,
					      &user_entry)))
	    {

/* The user does not exist.  Let the user know, and return. */

	    AUTHORIZE$DECW_ERROR (UAF$_NOSUCHUSER);
	    return;
	    }

/* Fill the identifier list with those held by the user. */

	uaf_l_current_user_uic = user_entry->uaf$l_usrlst_own_id;	/* Save for later */
	AUTHORIZE$IDENTIFIERS_UPDATE (uaf_l_current_user_uic);

/* Select the first identifier in the list. */

	AUTHORIZE$SET_UP_IDENTIFIER (NULL,
				     1);

/* Make the identifier window visible. */

	XtManageChild (uaf_r_identifier_window_widget);
	break;
	}

    case uaf$c_identifier_system:
	{

/* A list of the system identifiers is needed.  Build it and then make
** it visible. */

	AUTHORIZE$SET_UP_SYSTEM_ID ();
	XtManageChild (uaf_r_system_id_window_widget);
	break;
	}

    case uaf$c_system_identifier_cancel:
	{

/* Simply tear down the system identifier window without doing anything. */

	XtUnmanageChild (uaf_r_system_id_window_widget);
	break;
	}

    case uaf$c_identifier_modify:
    case uaf$c_identifier_grant:
    case uaf$c_identifier_remove:
	{

/* An identifier's attributes are being modified.  Get the attribute list
** state and set the apptoprizte flags. */

	AUTHORIZE$MODIFY_IDENTIFIER (widget_id,
				     tag,
				     reason);
	break;
	}

    case uaf$c_identifier_ok:
	{

/* The "OK" action is the same as a modify, except the window will be torn
** down upon completion. */

/* Tear down the identifier window. */

	AUTHORIZE$MODIFY_IDENTIFIER (widget_id,
				     tag,
				     reason);
	XtUnmanageChild (uaf_r_identifier_window_widget);

/* If necessary, tear down the system identifier window. */

	if (XtIsManaged (uaf_r_system_id_window_widget))
	    XtUnmanageChild (uaf_r_system_id_window_widget);
	break;
	}

    case uaf$c_identifier_cancel:
	{

/* Simply tear down the identifier window without doing anything. */

	XtUnmanageChild (uaf_r_identifier_window_widget);

/* If necessary, tear down the system identifier window. */

	if (XtIsManaged (uaf_r_system_id_window_widget))
	    XtUnmanageChild (uaf_r_system_id_window_widget);
	break;
	}
    }
}

extern void AUTHORIZE$IDENTIFIERS_UPDATE (uic)

int				uic;
{
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine is called to get the list of identifiers held by
**	the supplied UIC.  These are then placed in the identifier list.
**
**  FORMAL PARAMETERS:
**
**	UIC		- UIC for which the held identifiers will be
**			  obtained
**
**  RETURN VALUE:
**
**	None
**
**  SIDE EFFECTS:
**
**	{@description or none@}
**
**--
*/

/* Global references. */

globalref Widget		uaf_r_widget_id_array[];	/* Array of Widget IDs in use */

/* Local storage. */

unsigned int			context = 0;			/* $FIND_HELD context */
unsigned int			holder[2];			/* Holder for $FIND_HELD */
XmString			id_name;			/* Compound string for identifier name */
char				id_name_buf[MAX_ID_SIZE];	/* Storage for identifier name */
unsigned int			id_name_desc[2];		/* Simple descriptor for identifier name */
unsigned short int		id_name_len;			/* Identifier name length */
unsigned int			identifier;			/* Identifier found by $FIND_HELD */
unsigned int			status;				/* Routine exit status */

/* Debug information. */

#ifdef DEBUGGING
printf ("identifiers update\n");
#endif /* DEBUGGING */

/* Initialize needed storage. */

id_name_desc[0] = sizeof id_name_buf;
id_name_desc[1] = &id_name_buf;

holder[0] = uic;
holder[1] = 0;

/* Delete all the entries currently in the identifier list. */

XmListDeleteAllItems (uaf_r_widget_id_array[uaf$c_identifier_list]);

/* Loop getting all the identifiers held by the specified holder id. */

while (TRUE (SYS$FIND_HELD (&holder,
			    &identifier,
			    NULL,
			    &context)))
    {

/* Convert the identifier from a value to a text string. */

    SYS$IDTOASC (identifier,
		 &id_name_len,
		 &id_name_desc,
		 NULL,
		 NULL,
		 NULL);
    id_name_buf[id_name_len] = '\0';		/* Terminate the name */

/* Add the identifier to the list. */

    id_name = XmStringCreateSimple (id_name_buf);
    XmListAddItem (uaf_r_widget_id_array[uaf$c_identifier_list],
		   id_name,
		   0);
    XtFree (id_name);
    }
}

extern void AUTHORIZE$MODIFY_IDENTIFIER (widget_id, tag, reason)

Widget				widget_id;
int				*tag;
XmAnyCallbackStruct		*reason;
{
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine is called when it is necessary to modify the attributes
**	of an identifier.  The identifier name is obtained from the identifier
**	window and the attributes from the attribute list.  This information
**	is then used with the holder record.
**
**  FORMAL PARAMETERS:
**
**	{@subtags@}
**
**  RETURN VALUE:
**
**	None
**
**  SIDE EFFECTS:
**
**	{@description or none@}
**
**--
*/

/* The following is a mask to clear all id attributes available to a
** holder of an identifier.  This must track the definition of the
** symbol KGB$M_VALID_HOLDER_ATTRIB in the module [LOADSS]RDBSHR.B32. */

#define			id_attrib_clear_mask	0x1f

/* External routines. */

extern void		AUTHORIZE$GET_LIST_ENTRIES ();
extern void		AUTHORIZE$IDENTIFIERS_UPDATE ();
extern void		AUTHORIZE$SET_UP_ATTRIBUTES ();
extern void		AUTHORIZE$SET_UP_IDENTIFIER ();

/* Global references. */

globalref unsigned int	uaf_l_current_user_uic;		/* Current user UIC */
globalref Widget	uaf_r_widget_id_array[];	/* Array of Widget IDs in use */

/* Local storage. */

XmString		compound_string;		/* General compound string */
unsigned int		holder[2];			/* Holder for $xxx_HOLDER */
unsigned int		identifier;			/* Identifier's value */
unsigned int		identifier_attributes = 0;	/* Identifier's attributes to set */
int			index;				/* General index */
unsigned int		simple_desc[2];			/* Simple character descriptor */
unsigned int		status;				/* Routine exit status */
char			*temp_string;			/* Temp string pointer */
char			text_string [MAX_USERNAME];	/* Text string (from main window) storage */
int			widget_number = *tag;

/* Debug information. */

#ifdef DEBUGGING
printf ("modify identifier\n");
#endif /* DEBUGGING */

/* Get the supplied identifier name from the identifier window and convert
** it to all upper case characters. */

temp_string = XmTextGetString (uaf_r_widget_id_array[uaf$c_identifier_name]);
strcpy (text_string, temp_string);					/* Copy the widget text */
for (index = 0; index < strlen (text_string); index++)
    text_string[index] = toupper (text_string[index]);			/* Upcase character */
XtFree (temp_string);

/* Since the name is now uppercased, reflect it in the identifier window. */

XmTextSetString (uaf_r_widget_id_array[uaf$c_identifier_name],
		 text_string);

/* Get the state of the attributes in the identifier attribute list. 
** This doesn't really need to be done for the remove, but it is easier
** to make this call in the main line as it does affect most other
** operations. */

AUTHORIZE$GET_LIST_ENTRIES (uaf$c_identifier_attrib_list,
			    &identifier_attributes,
			    sizeof identifier_attributes);

/* Convert the text identifier name to a value; for $xxx_HOLDER. */

simple_desc[0] = strlen (text_string);
simple_desc[1] = text_string;

SYS$ASCTOID (&simple_desc,
	     &identifier,
	     NULL);						/* Don't need the attributes here */

/* Set up the holder information. */

holder[0] = uaf_l_current_user_uic;
holder[1] = 0;

/* Based upon the widget number, determine what should be done. */

switch (widget_number)
    {
    case uaf$c_identifier_modify:
	{

/* Because it is possible for the user to specify the granting of attributes
** the identifier does not currently have enabled, explicitly modify the
** identifier to enable the attributes being granted.  This will allow the
** $ADD_HOLDER to proceed as expected.  This is done quietly, unless an error
** is encountered.
**
** It would be possible to pop up a warning dialog box to query the user to
** determine whether or not this should be done...but that is more work :-). */

	SYS$MOD_IDENT (identifier,
		       identifier_attributes,
		       NULL,
		       NULL,
		       NULL);

/* Modify the holder record as appropriate. Clear all existing attributes
** held before setting the new attributes. */

	SYS$MOD_HOLDER (identifier,
			&holder,
			identifier_attributes,
			id_attrib_clear_mask);
	break;
	}

    case uaf$c_identifier_grant:
	{

/* Because it is possible for the user to specify the granting of attributes
** the identifier does not currently have enabled, explicitly modify the
** identifier to enable the attributes being granted.  This will allow the
** $ADD_HOLDER to proceed as expected.  This is done quietly, unless an error
** is encountered.
**
** It would be possible to pop up a warning dialog box to query the user to
** determine whether or not this should be done...but that is more work :-). */

	SYS$MOD_IDENT (identifier,
		       identifier_attributes,
		       NULL,
		       NULL,
		       NULL);

/* Add a new holder record for the specified identifier. */

	SYS$ADD_HOLDER (identifier,
			&holder,
			identifier_attributes);

/* Add the new identifier to the list.  This is done by simply rebuilding the
** list; rather than adding the single item.  Because the usual case is only
** holding a handful of identifiers, this is not a big performance issue.
**
** However, this is an area for a performance tweek, should an account hold
** a large number of identifiers. */

	AUTHORIZE$IDENTIFIERS_UPDATE (uaf_l_current_user_uic);

/* Select the currently entered identifier. */

	AUTHORIZE$SET_UP_IDENTIFIER (text_string,
				     NULL);
	break;
	}

    case uaf$c_identifier_remove:
	{

/* Remove a holder record for the specified identifier. */

	SYS$REM_HOLDER (identifier,
		        &holder);

/* Since the entry is to be removed from the identifier list, do it directly
** (and simply) rather than rebuilding the entire list. */

	compound_string = XmStringCreateSimple (text_string);
	XmListDeleteItem (uaf_r_widget_id_array[uaf$c_identifier_list],
			  compound_string);
	XtFree (compound_string);
	break;
	}
    }
}

extern void AUTHORIZE$SET_UP_ATTRIBUTES (identifier)

char				*identifier;
{
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine is called to set the list of attributes for an
**	identifier (held by the current UIC).  If an identifier is not
**	specified the currently selected identifier will be used.
**
**  FORMAL PARAMETERS:
**
**	IDENTIFIER	- pointer to the identifier name string
**
**  RETURN VALUE:
**
**	None
**
**  SIDE EFFECTS:
**
**	{@description or none@}
**
**--
*/

/* External routines. *.

extern void		AUTHORIZE$SELECT_LIST_ENTRIES ();

/* Global references. */

globalref unsigned int	uaf_l_current_user_uic;		/* Current user UIC */
globalref Widget	uaf_r_widget_id_array[];	/* Array of Widget IDs in use */

/* Local storage. */

unsigned int		context = 0;			/* $FIND_HELD context */
unsigned int		holder[2];			/* Holder for $FIND_HELD */
char			*id;				/* Name of identifier for attributes */
unsigned int		id_attributes;			/* Identifier attributes */
char			id_name_buf[MAX_ID_SIZE];	/* Storage for identifier name */
unsigned int		id_name_desc[2];		/* Simple descriptor for identifier name */
unsigned short int	id_name_len;			/* Identifier name length */
unsigned int		identifier2;			/* Identifier found by $FIND_HELD */
int			index;				/* General index */
int			(*position_list)[];		/* List of entries selected */
int			position_count = 0;		/* Number of entries selected */
unsigned int		status;				/* Routine exit status */

/* Debug information. */

#ifdef DEBUGGING
printf ("set up identifier attributes\n");
#endif /* DEBUGGING */

/* Initialize needed storage. */

id_name_desc[0] = sizeof id_name_buf;
id_name_desc[1] = &id_name_buf;

holder[0] = uaf_l_current_user_uic;
holder[1] = 0;

/* Get the name of the identifier to use.  This will either be the one
** specified or the currently selected identifier. */

if (identifier != 0)
    id = identifier;
else
    {

/* Find out which entry has been selected.  There should only be one. */

    XmListGetSelectedPos (uaf_r_widget_id_array[uaf$c_identifier_list],
			  &position_list,
			  &position_count);

/* Release the memory for the position list. */

    XtFree (position_list);
    }

/* Loop getting all the identifiers held by the specified holder id. */

while (TRUE (SYS$FIND_HELD (&holder,
			    &identifier2,
			    &id_attributes,
			    &context)))
    {

/* Convert the identifier from a value to a text string. */

    SYS$IDTOASC (identifier2,
		 &id_name_len,
		 &id_name_desc,
		 NULL,
		 NULL,
		 NULL);
    id_name_buf[id_name_len] = '\0';		/* Terminate the name */

/* If the identifier found matches the supplied identifier, set the state
** of the attributes in the list appropriately. */

    if (strcmp (id, id_name_buf) == 0)
	{
	AUTHORIZE$SELECT_LIST_ENTRIES (uaf$c_identifier_attrib_list,
				       &id_attributes,
				       sizeof id_attributes);
	SYS$FINISH_RDB (&context);					/* Finish up */
	break;
	}
    }
}

extern void AUTHORIZE$SET_UP_IDENTIFIER (identifier, position)

char				*identifier;
int				position;
{
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	{@tbs@}
**
**  FORMAL PARAMETERS:
**
**	{@subtags@}
**
**  RETURN VALUE:
**
**	{@description or none@}
**
**  SIDE EFFECTS:
**
**	{@description or none@}
**
**--
*/

/* Global references. */

globalref Widget	uaf_r_widget_id_array[uaf$c_max_widget_code];	/* Array of Widget IDs in use */

/* Local storage. */

XmString		compound_string;		/* General compound string */
unsigned int		status;				/* Routine exit status */
char			*temp_string;			/* Temp string pointer */

/* Debug information. */

#ifdef DEBUGGING
printf ("set up identifier\n");
#endif /* DEBUGGING */

/* If the set up is being done by identifier, set the position in the list
** to the specified identifier.  Otherwise, set the position in the list
** to the specified (numeric) position.  In addition, select the list item
** to simulate a mouse button click. */

if (identifier != 0)
    {

/* See if the specified identifier exists in the list.  If it is in the list,
** select it. */

    compound_string = XmStringCreateSimple (identifier);
    if (XmListItemExists (uaf_r_widget_id_array[uaf$c_identifier_list],
			  compound_string))
	{
	XmListSetItem (uaf_r_widget_id_array[uaf$c_identifier_list],
		       compound_string);
	XmListSelectItem (uaf_r_widget_id_array[uaf$c_identifier_list],
			  compound_string,
			  1);
	}
    XtFree (compound_string);
    }
else
    {

/* Otherwise, get the position in the list and use it. */

    XmListSetPos (uaf_r_widget_id_array[uaf$c_identifier_list],
		  position);
    XmListSelectPos (uaf_r_widget_id_array[uaf$c_identifier_list],
		     position,
		     1);
    }
}

extern void AUTHORIZE$SET_UP_SYSTEM_ID ()

{
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine is called to set up the system identifier list.
**
**  FORMAL PARAMETERS:
**
**	None.
**
**  RETURN VALUE:
**
**	None
**
**  SIDE EFFECTS:
**
**	{@description or none@}
**
**--
*/

/* External routines. */

extern void			AUTHORIZE$WATCH_CURSOR_ON ();
extern void			AUTHORIZE$WATCH_CURSOR_OFF ();

/* Global references. */

globalref Widget		uaf_r_identifier_window_widget;	/* Identifier window widget info */
globalref Widget		uaf_r_main_top_level_widget;	/* Main window widget shell info */
globalref Widget		uaf_r_main_window_widget;	/* Main window widget info */
globalref Cursor		uaf_r_wait_cursor_main;		/* Main window wait cursor id */
globalref Widget		uaf_r_widget_id_array[uaf$c_max_widget_code];	/* Array of Widget IDs in use */

/* Local storage. */

unsigned int			context = 0;			/* $IDTOASC context */
XmString			id_name;			/* Identifier name compound string */
char				id_name_buf[MAX_ID_SIZE];	/* Storage for identifier name */
unsigned int			id_name_desc[2];		/* Simple descriptor for identifier name */
unsigned short int		id_name_len;			/* Identifier name length */
unsigned int			status;				/* Routine exit status */
char				*temp_string;			/* Temp string pointer */

/* Debug information. */

#ifdef DEBUGGING
printf ("set up system identifiers\n");
#endif /* DEBUGGING */

/* Initialize needed storage. */

id_name_desc[0] = sizeof id_name_buf;
id_name_desc[1] = &id_name_buf;

/* Since building the identifier list may take some time, set up the wait cursor. */

AUTHORIZE$WATCH_CURSOR_ON (uaf_r_main_top_level_widget,
			   uaf_r_main_window_widget,
			   uaf_r_wait_cursor_main);
AUTHORIZE$WATCH_CURSOR_ON (uaf_r_main_top_level_widget,
			   uaf_r_identifier_window_widget,
			   uaf_r_wait_cursor_main);

/* Delete all the entries currently in the system identifier list. */

XmListDeleteAllItems (uaf_r_widget_id_array[uaf$c_system_identifier_list]);

/* Loop, getting all the defined identifiers. */

while (TRUE (SYS$IDTOASC (UIC$K_MATCH_ALL,
			  &id_name_len,
			  &id_name_desc,
			  NULL,
			  NULL,
			  &context)))
    {

/* An identifier has been found.  Add it to the list. */

    id_name_buf[id_name_len] = '\0';		/* Terminate the name */
    id_name = XmStringCreateSimple (id_name_buf);
    XmListAddItem (uaf_r_widget_id_array[uaf$c_system_identifier_list],
		   id_name,
		   0);
    XtFree (id_name);
    }

/* Reset the cursor. */

AUTHORIZE$WATCH_CURSOR_OFF (uaf_r_main_top_level_widget,
			    uaf_r_main_window_widget);
AUTHORIZE$WATCH_CURSOR_OFF (uaf_r_main_top_level_widget,
			    uaf_r_identifier_window_widget);
}
