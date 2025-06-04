#module UAF_DW_MAIN "X-3"

/*
**++
**  FACILITY:	Authorization record maintenance utility
**
**  MODULE DESCRIPTION:
**
**		This module contains the various DECwindows main window
**		support routines needed by the DECwindows AUTHORIZE utility.
**
**  AUTHORS:	L. Mark Pilant			CREATION DATE:  30-Oct-1992
**
**  MODIFICATION HISTORY:
**
**	X-3	LMP		L. Mark Pilant,		 5-JUN-1995  13:51
**		Fix ACCVIO bug resulting from from incorrect indirection.
**
**	X-2	LMP		L. Mark Pilant,		 5-MAR-1993  12:47
**		Convert to C from BLISS sources.
**
**	X-1	LMP		L. Mark Pilant,		30-Oct-1992
**		Original version.
**
**--
*/

/*
**
**  INCLUDE FILES
**
*/

#include ctype
#include stdlib

#include <decw$include:mrmappl>

#include "uaf_header"

/*
**
**  FORWARD ROUTINES
**
*/

#pragma noinline (AUTHORIZE$COMMAND_MENU)
#pragma noinline (AUTHORIZE$CREATE_ENTRY)
#pragma noinline (AUTHORIZE$DELETE_ENTRY)
#pragma noinline (AUTHORIZE$FILE_MENU)
#pragma noinline (AUTHORIZE$HELP_MENU)
#pragma noinline (AUTHORIZE$MAIN_PROCESS)
#pragma noinline (AUTHORIZE$MODIFY_ENTRY)
#pragma noinline (AUTHORIZE$RESET_ENTRY)
#pragma noinline (AUTHORIZE$SELECT_ENTRY)
#pragma noinline (AUTHORIZE$SET_DEFAULT_FILTER)
#pragma noinline (AUTHORIZE$SET_UP_GROUP)
#pragma noinline (AUTHORIZE$SET_UP_USERNAME)

extern void AUTHORIZE$COMMAND_MENU (widget_id, tag, reason)

Widget				widget_id;
int				*tag;
XmAnyCallbackStruct		*reason;
{
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine is called to process requests from the command menu list item.
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

extern void	AUTHORIZE$MODIFY_TEMPLATE ();

/* Global references. */

globalref int	uaf_l_main_operation;		/* Operation (i.e., create, modify, etc.) */

/* Local storage. */

unsigned int	status;				/* Routine exit status */
int		widget_number = *tag;

/* Debug information. */

#ifdef DEBUGGING
printf ("command menu\n");
#endif /* DEBUGGING */

/* Set the operation (main window widget number) */

uaf_l_main_operation = widget_number;

/* Call the appropriate routine, based upon the menu list item. */

switch (widget_number)
    {
    case uaf$c_cmd_modify_template:
	{
	AUTHORIZE$MODIFY_TEMPLATE (widget_id,
				   tag,
				   reason);
	break;
	}
    }
}

extern void AUTHORIZE$CREATE_ENTRY (widget_id, tag, reason)

Widget				widget_id;
int				*tag;
XmAnyCallbackStruct		*reason;
{
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine is called when it is necessary to create a new authorization
**	entry.  It will be necesary to get the username (or UIC) from the main
**	window to determine which authorization entry is to be created.
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
extern void			AUTHORIZE$ITMLST_COPY ();
extern void			AUTHORIZE$ITMLST_UPDATE ();
extern void			AUTHORIZE$SET_UP_ENTRY ();

/* Global references. */

globalref struct ITMDEF		(*uaf_r_current_group_info)[];	/* Current group UAF info */
globalref struct ITMDEF		(*uaf_r_current_user_info)[];	/* Current user UAF info */
globalref Widget		uaf_r_entry_window_widget;	/* Entry window widget info */
globalref struct USRLSTDEF	uaf_r_master_list;		/* Complete user list (from the file) */
globalref Widget		uaf_r_widget_id_array[];	/* Array of Widget IDs in use */
globalref char			uaf_t_current_username[];	/* Current username */

/* Local storage. */

int				index;				/* General index */
unsigned int			status;				/* Routine exit status */
char				*temp_string;			/* Temp string pointer */
char				text_string [MAX_USERNAME];	/* Text string (from main window) storage */
struct USRLSTDEF		*user_entry;			/* Current user list entry */
int				widget_number = *tag;
short int			wildcard_name = 0;		/* Wildcard indicator */

/* Debug information. */

#ifdef DEBUGGING
printf ("create entry\n");
#endif /* DEBUGGING */

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

/* Note the current username for later use. */

strcpy (uaf_t_current_username, text_string);

#ifdef DEBUGGING
printf ("creating new user: %s\n", text_string);
#endif /* DEBUGGING */

/* If there are any wildcards, complain and abort the operation. */

if (wildcard_name != 0)
    {
    AUTHORIZE$DECW_ERROR (UAF$_NOWILDCARDS);
    return;
    }

/* See if the user already exists. */

if (TRUE (AUTHORIZE$FIND_USER_ENTRY (&uaf_r_master_list.uaf$l_usrlst_flink,
				     uaf_t_current_username,
				     FIND_NAME_EQUAL,
				     0,
				     0)))
    {

/* The user already exists.  Let the user know, and return. */

    AUTHORIZE$DECW_ERROR (UAF$_USEREXISTS);
    return;
    }

/* The user doesn't exist.  Set up a new entry using the selected group "default".
** The selected default may be a specific group default entry or the traditional
** default entry. */

AUTHORIZE$ITMLST_COPY (uaf_r_current_group_info,
		       &uaf_r_current_user_info,
		       0);

/* Update the username in the copied item list to reflect the username of the
** entry to create. */

AUTHORIZE$ITMLST_UPDATE (&uaf_r_current_user_info,
			 UAI$_USERNAME,
			 strlen (uaf_t_current_username),
			 uaf_t_current_username);

/* Set up the information in the authorization entry window. */

AUTHORIZE$SET_UP_ENTRY (uaf_r_current_user_info);

/* Make the entry window visible. */

XtManageChild (uaf_r_entry_window_widget);
}

extern void AUTHORIZE$DELETE_ENTRY (widget_id, tag, reason)

Widget				widget_id;
int				*tag;
XmAnyCallbackStruct		*reason;
{
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine is called to delete an authorization entry.
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
extern void			AUTHORIZE$DEL_LIST_ENTRY ();
extern unsigned int		AUTHORIZE$DEL_USER_INFO ();
extern unsigned int		AUTHORIZE$FIND_USER_ENTRY ();
extern void			AUTHORIZE$SET_UP_GROUP ();
extern void			AUTHORIZE$SET_UP_USERNAME ();

/* Global references. */

globalref unsigned int		uaf_l_current_group_max;	/* Count of entries in group list */
globalref unsigned int		uaf_l_current_user_max;		/* Count of entries in user list */
globalref struct USRLSTDEF	uaf_r_group_list;		/* Group user name list head */
globalref struct USRLSTDEF	uaf_r_master_list;		/* Complete user list (from the file) */
globalref struct USRLSTDEF	uaf_r_user_list;		/* User name list head */
globalref Widget		uaf_r_widget_id_array[];	/* Array of Widget IDs in use */
globalref char			uaf_t_current_username[];	/* Current username */

/* Local storage. */

XmString			current_username;		/* Current username compound string */
int				index;				/* General index */
int				list_count;			/* Number of list entries passed before adding item */
unsigned int			status;				/* Routine exit status */
char				*temp_string;			/* Temp string pointer */
char				text_string [MAX_USERNAME];	/* Text string (from main window) storage */
struct USRLSTDEF		*user_entry;			/* Current user list entry */
int				widget_number = *tag;
short int			wildcard_name = 0;		/* Wildcard indicator */

/* Debug information. */

#ifdef DEBUGGING
printf ("delete entry\n");
#endif /* DEBUGGING */

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

/* Note the current username for later use. */

strcpy (uaf_t_current_username, text_string);
current_username = XmStringCreateSimple (uaf_t_current_username);	/* After being upcased */

/* Make sure the user already exists. */

if (FALSE (AUTHORIZE$FIND_USER_ENTRY (&uaf_r_master_list.uaf$l_usrlst_flink,
				      &text_string,
				      FIND_NAME_EQUAL,
				      0,
				      0)))
    {

/* The user does not exist.  Let the user know, and return. */

    AUTHORIZE$DECW_ERROR (UAF$_NOSUCHUSER);
    XtFree (current_username);
    return;
    }

/* Delete the requested entry from the user authorization file. */

if (FALSE (AUTHORIZE$DEL_USER_INFO (uaf_t_current_username)))
    {

/* There was some error deleting the authorization record.  Let the user
** know and return. */

    AUTHORIZE$DECW_ERROR (UAF$_UAFWRITERR);
    XtFree (current_username);
    return;
    }

/* Since the authorization record has been successfully deleted, remove
** the internal information. */

/* Check the master list of users to find the entry to remove. */

if (TRUE (AUTHORIZE$FIND_USER_ENTRY (&uaf_r_master_list.uaf$l_usrlst_flink,
				     &text_string,
				     FIND_NAME_EQUAL,
				     0,
				     &user_entry)))
    AUTHORIZE$DEL_LIST_ENTRY (&user_entry->uaf$l_usrlst_flink);

/* Check the list of users to find the entry to remove. */

if (TRUE (AUTHORIZE$FIND_USER_ENTRY (&uaf_r_user_list.uaf$l_usrlst_flink,
				     &text_string,
				     FIND_NAME_EQUAL,
				     &list_count,
				     &user_entry)))
    {
    AUTHORIZE$DEL_LIST_ENTRY (&user_entry->uaf$l_usrlst_flink);
    --uaf_l_current_user_max;				/* One less user */
    XmListDeleteItem (uaf_r_widget_id_array[uaf$c_main_user_list],
		      current_username);
    }

/* Limit check the list entry number. */

if (list_count > uaf_l_current_user_max) list_count = uaf_l_current_user_max;

/* Since the username has been removed from the list, select the previous
** entry. */

AUTHORIZE$SET_UP_USERNAME (NULL,
			   list_count);

/* Check the list of groups to find the entry to remove. */

if (TRUE (AUTHORIZE$FIND_USER_ENTRY (&uaf_r_group_list.uaf$l_usrlst_flink,
				     &text_string,
				     FIND_NAME_EQUAL,
				     &list_count,
				     &user_entry)))
    {
    AUTHORIZE$DEL_LIST_ENTRY (&user_entry->uaf$l_usrlst_flink);
    --uaf_l_current_user_max;				/* One less user */
    XmListDeleteItem (uaf_r_widget_id_array[uaf$c_main_group_list],
		      current_username);
    }

/* Limit check the list entry number. */

if (list_count > uaf_l_current_group_max) list_count = uaf_l_current_group_max;

/* Since the username has been removed from the list, select the previous
** entry. */

AUTHORIZE$SET_UP_GROUP (NULL,
			list_count);

XtFree (current_username);
}

extern void AUTHORIZE$FILE_MENU (widget_id, tag, reason)

Widget				widget_id;
int				*tag;
XmAnyCallbackStruct		*reason;
{
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine is called to process requests from the file menu list item.
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

extern unsigned int		AUTHORIZE$BUILD_MASTER_LIST ();
extern void 			AUTHORIZE$BUILD_USER_LIST ();
extern void			AUTHORIZE$DECW_ERROR ();
extern void			AUTHORIZE$WATCH_CURSOR_ON ();
extern void			AUTHORIZE$WATCH_CURSOR_OFF ();

/* Global references. */

globalref Widget		uaf_r_file_window_widget;	/* File selection widget info */
globalref Widget		uaf_r_main_top_level_widget;	/* Main window widget shell info */
globalref Widget		uaf_r_main_window_widget;	/* Main window widget info */
globalref Cursor		uaf_r_wait_cursor_main;		/* Main window wait cursor id */
globalref unsigned short int	uaf_w_uai_context[2];		/* $xxxUAI context cell */

/* Local storage. */

unsigned int			status;				/* Routine exit status */
int				widget_number = *tag;

/* Debug information. */

#ifdef DEBUGGING
printf ("file menu\n");
#endif /* DEBUGGING */

/* Based upon the widget number, determine what should be done. */

switch (widget_number)
    {
    case uaf$c_ctl_open:
	{

/* Bring up the file selection box. */

	XtManageChild (uaf_r_file_window_widget);
	break;
	}
    case uaf$c_ctl_quit:
	{

/* All through with the session. */

	SYS$EXIT (SS$_NORMAL);
	break;
	}
    case uaf$c_ctl_refresh:
	{

/* If there is no authorization file open, indicated by the uaf_w_uai_context
** cell being something other than -1, there is nothing to refresh. */

	if (uaf_w_uai_context[0] != 0xFFFF)
	    {

/* Since building the user list may take some time, set up the wait cursor. */

	    AUTHORIZE$WATCH_CURSOR_ON (uaf_r_main_top_level_widget,
				       uaf_r_main_window_widget,
				       uaf_r_wait_cursor_main);

/* Rebuild the in memory user lists from the authorization file. */

	    if (FALSE (AUTHORIZE$BUILD_MASTER_LIST ()))
		AUTHORIZE$DECW_ERROR (UAF$_UAFREADERR);
	    AUTHORIZE$BUILD_USER_LIST ();

/* Reset the cursor. */

	    AUTHORIZE$WATCH_CURSOR_OFF (uaf_r_main_top_level_widget,
					uaf_r_main_window_widget);
	    }
	break;
	}
    }
}

extern void AUTHORIZE$HELP_MENU (widget_id, tag, reason)

Widget				widget_id;
int				*tag;
XmAnyCallbackStruct		*reason;
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
**	None
**
**  SIDE EFFECTS:
**
**	{@description or none@}
**
**--
*/

/* Local storage. */

unsigned int			status;				/* Routine exit status */
int				widget_number = *tag;

/* Debug information. */

#ifdef DEBUGGING
printf ("help menu\n");
#endif /* DEBUGGING */
}

extern void AUTHORIZE$MAIN_PROCESS (widget_id, tag, reason)

Widget				widget_id;
int				*tag;
XmAnyCallbackStruct		*reason;
{
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine is called to process requests via the main window
**	pushbuttons.
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

extern void	AUTHORIZE$CREATE_ENTRY ();
extern void	AUTHORIZE$DELETE_ENTRY ();
extern void	AUTHORIZE$MODIFY_ENTRY ();
extern void	AUTHORIZE$RESET_ENTRY ();
extern void	AUTHORIZE$SELECT_ENTRY ();

/* Global references. */

globalref int	uaf_l_main_operation;		/* Operation (i.e., create, modify, etc.) */

/* Local storage. */

unsigned int	status;				/* Routine exit status */
int		widget_number = *tag;

/* Debug information. */

#ifdef DEBUGGING
printf ("main process\n");
#endif /* DEBUGGING */

/* Set the operation (main window widget number) */

uaf_l_main_operation = widget_number;

/* Call the appropriate routine, based upon the pushbutton. */

switch (widget_number)
    {
    case uaf$c_main_create:
	{
	AUTHORIZE$CREATE_ENTRY (widget_id,
				tag,
				reason);
	break;
	}
    case uaf$c_main_modify:
	{
	uaf_l_main_operation = widget_number;		/* Set the operation (main window widget number) */
	AUTHORIZE$MODIFY_ENTRY (widget_id,
				tag,
				reason);
	break;
	}
    case uaf$c_main_reset:
	{
	uaf_l_main_operation = widget_number;		/* Set the operation (main window widget number) */
	AUTHORIZE$RESET_ENTRY (widget_id,
			       tag,
			       reason);
	break;
	}
    case uaf$c_main_select:
	{
	uaf_l_main_operation = widget_number;		/* Set the operation (main window widget number) */
	AUTHORIZE$SELECT_ENTRY (widget_id,
				tag,
				reason);
	break;
	}
    case uaf$c_main_delete:
	{
	uaf_l_main_operation = widget_number;		/* Set the operation (main window widget number) */
	AUTHORIZE$DELETE_ENTRY (widget_id,
				tag,
				reason);
	break;
	}
    }
}

extern void AUTHORIZE$MODIFY_ENTRY (widget_id, tag, reason)

Widget				widget_id;
int				*tag;
XmAnyCallbackStruct		*reason;
{
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine is called when it is necessary to modify an existing
**	authorization entry.  It will be necesary to get the username (or
**	UIC) from the main window to determine which authorization entries
**	are to be affected.
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
extern void			AUTHORIZE$GET_USER_INFO ();
extern void			AUTHORIZE$SET_UP_ENTRY ();

/* Global references. */

globalref struct ITMDEF		(*uaf_r_current_user_info)[];	/* Current user UAF info */
globalref Widget		uaf_r_entry_window_widget;	/* Entry window widget info */
globalref struct USRLSTDEF	uaf_r_master_list;		/* Complete user list (from the file) */
globalref Widget		uaf_r_widget_id_array[];	/* Array of Widget IDs in use */
globalref char			uaf_t_current_username[];	/* Current username */

/* Local storage. */

int				index;				/* General index */
unsigned int			status;				/* Routine exit status */
char				*temp_string;			/* Temp string pointer */
char				text_string [MAX_USERNAME];	/* Text string (from main window) storage */
struct USRLSTDEF		*user_entry;			/* Current user list entry */
int				widget_number = *tag;
short int			wildcard_name = 0;		/* Wildcard indicator */

/* Debug information. */

#ifdef DEBUGGING
printf ("modify entry\n");
#endif /* DEBUGGING */

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

/* Note the current username for later use. */

strcpy (uaf_t_current_username, text_string);

/* Make sure the user exists. */

if (FALSE (AUTHORIZE$FIND_USER_ENTRY (&uaf_r_master_list.uaf$l_usrlst_flink,
				      uaf_t_current_username,
				      FIND_NAME_EQUAL,
				      0,
				      0)))
    {

/* The user does not exist.  Let the user know, and return. */

    AUTHORIZE$DECW_ERROR (UAF$_NOSUCHUSER);
    return;
    }

/* Since an existing entry is being modified, set the initial state of all the
** information in the window. */

AUTHORIZE$GET_USER_INFO (uaf_t_current_username,
			 &uaf_r_current_user_info);
AUTHORIZE$SET_UP_ENTRY (uaf_r_current_user_info);

/* Make the entry window visible. */

XtManageChild (uaf_r_entry_window_widget);
}

extern void AUTHORIZE$RESET_ENTRY (widget_id, tag, reason)

Widget				widget_id;
int				*tag;
XmAnyCallbackStruct		*reason;
{
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine is called when it is necessary to reset the main window
**	to an initial state.
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

extern void		AUTHORIZE$BUILD_USER_LIST ();
extern void		AUTHORIZE$SET_DEFAULT_FILTER ();
extern void		AUTHORIZE$SET_UP_USERNAME ();
extern void		AUTHORIZE$WATCH_CURSOR_ON ();
extern void		AUTHORIZE$WATCH_CURSOR_OFF ();

/* Global references. */

globalref Widget	uaf_r_main_top_level_widget;	/* Main window widget shell info */
globalref Widget	uaf_r_main_window_widget;	/* Main window widget info */
globalref Cursor	uaf_r_wait_cursor_main;		/* Main window wait cursor id */
globalref char		*uaf_t_default_filter;		/* Default username filter */

/* Local storage. */

unsigned int		status;				/* Routine exit status */
int			widget_number = *tag;

/* Debug information. */

#ifdef DEBUGGING
printf ("reset entry\n");
#endif /* DEBUGGING */

/* Since building the user list may take some time, set up the wait cursor. */

AUTHORIZE$WATCH_CURSOR_ON (uaf_r_main_top_level_widget,
			   uaf_r_main_window_widget,
			   uaf_r_wait_cursor_main);

/* Set up the default username filter. */

AUTHORIZE$SET_DEFAULT_FILTER (uaf_t_default_filter);

/* Get a list of the user's and groups. */

AUTHORIZE$BUILD_USER_LIST ();

/* If possible, set up for the first username in the list. */

AUTHORIZE$SET_UP_USERNAME (NULL,
			   1);

/* Reset the cursor. */

AUTHORIZE$WATCH_CURSOR_OFF (uaf_r_main_top_level_widget,
			    uaf_r_main_window_widget);
}

extern void AUTHORIZE$SELECT_ENTRY (widget_id, tag, reason)

Widget				widget_id;
int				*tag;
XmAnyCallbackStruct		*reason;
{
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine is called when it is necessary to select a new set of
**	authorization entries.  These will be reflected in the user and group
**	lists.
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

extern void		AUTHORIZE$BUILD_USER_LIST ();
extern void		AUTHORIZE$SET_DEFAULT_FILTER ();
extern void		AUTHORIZE$SET_UP_USERNAME ();
extern void		AUTHORIZE$WATCH_CURSOR_ON ();
extern void		AUTHORIZE$WATCH_CURSOR_OFF ();

/* Global references. */

globalref Widget	uaf_r_main_top_level_widget;	/* Main window widget shell info */
globalref Widget	uaf_r_main_window_widget;	/* Main window widget info */
globalref Cursor	uaf_r_wait_cursor_main;		/* Main window wait cursor id */
globalref Widget	uaf_r_widget_id_array[];	/* Array of Widget IDs in use */

/* Local storage. */

int			index;				/* General index */
char			*temp_string;			/* Temp string pointer */
char			text_string [MAX_USERNAME];	/* Text string (from main window) storage */
int			widget_number = *tag;

/* Debug information. */

#ifdef DEBUGGING
printf ("select entry\n");
#endif /* DEBUGGING */

/* Get the supplied filter string from the main window and convert it to all upper
** case characters. */

temp_string = XmTextGetString (uaf_r_widget_id_array[uaf$c_main_filter]);
strcpy (text_string, temp_string);					/* Copy the widget text */
for (index = 0; index < sizeof text_string; index++)
    text_string[index] = toupper (text_string[index]);			/* Upcase character */

XmTextSetString (uaf_r_widget_id_array[uaf$c_main_filter],		/* Reflect upcasing */
		 text_string);
XtFree (temp_string);

/* Since building the user list may take some time, set up the wait cursor. */

AUTHORIZE$WATCH_CURSOR_ON (uaf_r_main_top_level_widget,
			   uaf_r_main_window_widget,
			   uaf_r_wait_cursor_main);

/* Set up the new filter and build the new lists using the supplied filter. */

AUTHORIZE$SET_DEFAULT_FILTER (text_string);
AUTHORIZE$BUILD_USER_LIST ();
AUTHORIZE$SET_UP_USERNAME (NULL,
			   1);

/* Reset the cursor. */

AUTHORIZE$WATCH_CURSOR_OFF (uaf_r_main_top_level_widget,
			    uaf_r_main_window_widget);
}

extern void AUTHORIZE$SET_DEFAULT_FILTER (filter)

char				*filter;
{
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine is called to set up the default username filter
**	string.  This string may contain wildcards.
**
**  FORMAL PARAMETERS:
**
**	FILTER		- pointer to the filter text
**
**  RETURN VALUE:
**
**	None.
**
**  SIDE EFFECTS:
**
**	{@description or none@}
**
**--
*/

/* Global references. */

globalref Widget	uaf_r_widget_id_array[];		/* Array of Widget IDs in use */
globalref char		uaf_t_username_filter[];		/* Current username filter string */

/* Local storage. */

unsigned int		status;				/* Routine exit status */

/* Debug information. */

#ifdef DEBUGGING
printf ("set default filter\n");
#endif /* DEBUGGING */

/* Set up the filter in the main window. */

XmTextSetString (uaf_r_widget_id_array[uaf$c_main_filter],
		 filter);

/* Stash a copy of the new default filter. */

strcpy (uaf_t_username_filter, filter);
}

extern void AUTHORIZE$SET_UP_GROUP (group, position)

char				*group;
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
**	None
**
**  SIDE EFFECTS:
**
**	{@description or none@}
**
**--
*/

/* Global references. */

globalref Widget	uaf_r_widget_id_array[uaf$c_max_widget_code];		/* Array of Widget IDs in use */

/* Local storage. */

XmString		compound_string;		/* General compound string */
unsigned int		status;				/* Routine exit status */

/* Debug information. */

#ifdef DEBUGGING
printf ("set up group\n");
#endif /* DEBUGGING */

/* If the set up is being done by group name, set the position in the list
** to the specified group name.  Otherwise, set the position in the list
** to the specified (numeric) position.  In addition, select the list item
** to simulate a mouse button click. */

if (group != 0)
    {

/* See if the specified name exists in the list.  (This is done by checking
** the actual list rather than the in-memory queue.  Either method would work,
** but I wanted to practice with Motif.)  If it is in the list, select it. */

    compound_string = XmStringCreateSimple (group);
    if (XmListItemExists (uaf_r_widget_id_array[uaf$c_main_group_list],
			  compound_string))
	{
	XmListSetItem (uaf_r_widget_id_array[uaf$c_main_group_list],
		       compound_string);
	XmListSelectItem (uaf_r_widget_id_array[uaf$c_main_group_list],
			  compound_string,
			  1);
	}
    XtFree (compound_string);
    }
else
    {

/* Otherwise, get the first entry in the list and use it. */

    XmListSetPos (uaf_r_widget_id_array[uaf$c_main_group_list],
		  position);
    XmListSelectPos (uaf_r_widget_id_array[uaf$c_main_group_list],
		     position,
		     1);
    }
}

extern void AUTHORIZE$SET_UP_USERNAME (username, position)

char				*username;
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
**	None
**
**  SIDE EFFECTS:
**
**	{@description or none@}
**
**--
*/

/* Global references. */

globalref unsigned int	uaf_l_current_user_index;			/* Index of current user */
globalref Widget	uaf_r_widget_id_array[uaf$c_max_widget_code];	/* Array of Widget IDs in use */
globalref char		uaf_t_current_username[];			/* Current username */

/* Local storage. */

XmString		compound_string;		/* General compound string */
char			*temp_string;			/* Temp string pointer */

/* Debug information. */

#ifdef DEBUGGING
printf ("set up username\n");
#endif /* DEBUGGING */

/* If the set up is being done by username, set the position in the list
** to the specified username.  Otherwise, set the position in the list
** to the specified (numeric) position.  In addition, select the list item
** to simulate a mouse button click. */

if (username != 0)
    {

/* See if the specified name exists in the list.  (This is done by checking
** the actual list rather than the in-memory queue.  Either method would work,
** but I wanted to practice with Motif.)  If it is in the list, select it. */

    compound_string = XmStringCreateSimple (username);
    if (XmListItemExists (uaf_r_widget_id_array[uaf$c_main_user_list],
			  compound_string))
	{
	uaf_l_current_user_index = XmListItemPos (uaf_r_widget_id_array[uaf$c_main_user_list],
						  compound_string);
	XmListSetItem (uaf_r_widget_id_array[uaf$c_main_user_list],
		       compound_string);
	XmListSelectItem (uaf_r_widget_id_array[uaf$c_main_user_list],
			  compound_string,
			  1);
	}
    XtFree (compound_string);
    }
else
    {

/* Otherwise, get the position in the list and use it. */

    uaf_l_current_user_index = position;				/* Get specified position */
    if (uaf_l_current_user_index == 0) uaf_l_current_user_index = 1;	/* Default to 1 */
    XmListSetPos (uaf_r_widget_id_array[uaf$c_main_user_list],
		  uaf_l_current_user_index);
    XmListSelectPos (uaf_r_widget_id_array[uaf$c_main_user_list],
		     uaf_l_current_user_index,
		     1);
    }

/* Since the current username in the main window has been set, get the value
** back and set it up in memory.  This will be used later. */

temp_string = XmTextGetString (uaf_r_widget_id_array[uaf$c_main_username]);
strcpy (uaf_t_current_username, temp_string);				/* Copy the widget text */
XtFree (temp_string);
}
