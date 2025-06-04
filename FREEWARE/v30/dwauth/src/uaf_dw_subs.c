#module UAF_DW_SUBS "X-2"

/*
**++
**  FACILITY:	Authorization record maintenance utility
**
**  MODULE DESCRIPTION:
**
**		This module contains the various DECwindows support routines
**		needed by the DECwindows AUTHORIZE utility.
**
**  AUTHORS:	L. Mark Pilant			CREATION DATE:  30-Oct-1992
**
**  MODIFICATION HISTORY:
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

#include <STSDEF>

#include <decw$include:mrmappl>

#include "uaf_header"

/*
**
**  FORWARD ROUTINES
**
*/

#pragma noinline (AUTHORIZE$DECW_ACTIVATE_PROC)
#pragma noinline (AUTHORIZE$DECW_BUTTON_PROC)
#pragma noinline (AUTHORIZE$DECW_CREATE_PROC)
#pragma noinline (AUTHORIZE$DECW_ERROR)
#pragma noinline (AUTHORIZE$DECW_ERROR_DISMISS)
#pragma noinline (AUTHORIZE$DECW_LIST_PROC)
#pragma noinline (AUTHORIZE$DECW_PULL_PROC)
#pragma noinline (AUTHORIZE$DECW_SET_VALUE)
#pragma noinline (AUTHORIZE$DECW_TOGGLE_PROC)
#pragma noinline (AUTHORIZE$GET_LIST_ENTRIES)
#pragma noinline (AUTHORIZE$GET_LIST_ENTRY_TEXT)
#pragma noinline (AUTHORIZE$GET_TOGGLE_BUTTONS)
#pragma noinline (AUTHORIZE$SELECT_LIST_ENTRIES)
#pragma noinline (AUTHORIZE$SET_TOGGLE_BUTTONS)
#pragma noinline (AUTHORIZE$UNMAP_WIDGET)
#pragma noinline (AUTHORIZE$WATCH_CURSOR_OFF)
#pragma noinline (AUTHORIZE$WATCH_CURSOR_ON)

extern void AUTHORIZE$DECW_ACTIVATE_PROC (widget_id, tag, reason)

Widget				widget_id;
int				*tag;
XmAnyCallbackStruct		*reason;
{
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This callback routine is invoked from the .UIL file to perform the
**	the actions required to support various widgets; usually pushbuttons
**	and menu lists.
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

extern void	AUTHORIZE$ACCESS_RESTRICTIONS ();
extern void	AUTHORIZE$ENTRY_PROCESS ();
extern void	AUTHORIZE$COMMAND_MENU ();
extern void	AUTHORIZE$FILE_MENU ();
extern void	AUTHORIZE$FILE_PROCESS ();
extern void	AUTHORIZE$FINISH_ENTRY ();
extern void	AUTHORIZE$HELP_MENU ();
extern void	AUTHORIZE$IDENTIFIERS ();
extern void	AUTHORIZE$MAIN_PROCESS ();

/* Local storage. */

unsigned int	status;				/* Routine exit status */
int		widget_number = *tag;

/* Debug information. */

#ifdef DEBUGGING
printf ("activate proc\n");
#endif /* DEBUGGING */

/* Call the appropriate routine, based upon the requested action. */

switch (widget_number)
    {

/* For the main menu bar, call the appropriate routines. */

    case uaf$c_ctl_open:
    case uaf$c_ctl_refresh:
    case uaf$c_ctl_quit:
	{
	AUTHORIZE$FILE_MENU (widget_id,
			     tag,
			     reason);
	break;
	}

    case uaf$c_cmd_list:
    case uaf$c_cmd_print:
    case uaf$c_cmd_modify_template:
	{
	AUTHORIZE$COMMAND_MENU (widget_id,
				tag,
				reason);
	break;
	}

    case uaf$c_hlp_about:
    case uaf$c_hlp_overview:
	{
	AUTHORIZE$HELP_MENU (widget_id,
			     tag,
			     reason);
	break;
	}

/* For the main window pushbuttons, call the appropriate routine. */

    case uaf$c_main_create:
    case uaf$c_main_modify:
    case uaf$c_main_reset:
    case uaf$c_main_select:
    case uaf$c_main_delete:
	{
	AUTHORIZE$MAIN_PROCESS (widget_id,
				tag,
				reason);
	break;
	}

/* For the identifier window. */

    case uaf$c_main_identifiers:
    case uaf$c_identifier_system:
    case uaf$c_identifier_grant:
    case uaf$c_identifier_modify:
    case uaf$c_identifier_remove:
    case uaf$c_identifier_ok:
    case uaf$c_identifier_cancel:
    case uaf$c_system_identifier_cancel:
	{
	AUTHORIZE$IDENTIFIERS (widget_id,
			       tag,
			       reason);
	break;
	}

/* For the entry modify window pushbuttons. */

    case uaf$c_entry_ok:
    case uaf$c_entry_rename:
    case uaf$c_entry_copy:
	{
	AUTHORIZE$FINISH_ENTRY (widget_id,
				tag,
				reason);
	break;
	}

    case uaf$c_entry_cancel:
    case uaf$c_entry_ok_next:
    case uaf$c_entry_ok_previous:
    case uaf$c_entry_apply_template:
	{
	AUTHORIZE$ENTRY_PROCESS (widget_id,
				 tag,
				 reason);
	break;
	}

/* For the access restriction window. */

    case uaf$c_entry_access_primary:
    case uaf$c_entry_access_secondary:
    case uaf$c_access_pri_ok:
    case uaf$c_access_pri_cancel:
    case uaf$c_access_sec_ok:
    case uaf$c_access_sec_cancel:
	{
	AUTHORIZE$ACCESS_RESTRICTIONS (widget_id,
				       tag,
				       reason);
	break;
	}

/* For the file selection window pushbuttons. */

    case uaf$c_file_open_file:
    case uaf$c_file_cancel:
	{
	AUTHORIZE$FILE_PROCESS (widget_id,
				tag,
				reason);
	break;
	}
    }
}

extern void AUTHORIZE$DECW_BUTTON_PROC (widget_id, user_data, event)

Widget				widget_id;
int				user_data;	/* Unused */
XButtonEvent			*event;
{
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This callback routine is invoked through a ButtonPress event in the
**	authorization entry window.
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

globalref Widget	uaf_r_entry_popup_menu_widget;	/* Entry window popup menu */
globalref Widget	uaf_r_entry_window_widget;	/* Entry window widget info */
globalref Widget	uaf_r_id_popup_menu_widget;	/* Identifier window popup menu */
globalref Widget	uaf_r_identifier_window_widget;	/* Identifier window widget info */

/* Local storage. */

unsigned int		status;				/* Routine exit status */

/* Debug information. */

#ifdef DEBUGGING
printf ("button proc\n");
#endif /* DEBUGGING */

/* Only events related to mouse button 3 are to be processed by this routine. */

if (event->button != Button3) return;

/* If the event occurs in the authorization entry window, bring up the
** appropriate popup menu. */

if (widget_id == uaf_r_entry_window_widget)
    {
    XmMenuPosition (uaf_r_entry_popup_menu_widget,
		    event);
    XtManageChild (uaf_r_entry_popup_menu_widget);
    }

/* if the event occurs in the identifier window, bring up the appropriate
** popup menu. */

if (widget_id == uaf_r_identifier_window_widget)
    {
    XmMenuPosition (uaf_r_id_popup_menu_widget,
		    event);
    XtManageChild (uaf_r_id_popup_menu_widget);
    }
}

extern void AUTHORIZE$DECW_CREATE_PROC (widget_id, tag, reason)

Widget				widget_id;
int				*tag;
XmAnyCallbackStruct		*reason;
{
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This callback routine is invoked to "register" the IDs for the various
**	widgets created (in the .UIL file).
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

unsigned int		status;				/* Routine exit status */
int			widget_number = *tag;

/* Debug information. */

#ifdef DEBUGGING
printf ("create proc\n");
#endif /* DEBUGGING */

/* Save the widget id of the created widget. */

uaf_r_widget_id_array[widget_number] = widget_id;
}

extern void AUTHORIZE$DECW_ERROR (error_code)

unsigned int		error_code;
{
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine is called to pop up an appropriate dialog box, based
**	upon the specified error code.
**
**  FORMAL PARAMETERS:
**
**	ERROR_CODE		- error code to interrogate
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

/* Required constants. */

#define	ARG_COUNT	4				/* Number of X arguments in list */

/* External routines. */

extern void		AUTHORIZE$DECW_ERROR_DISMISS ();

/* Global references. */

globalref Widget	uaf_r_error_dialog_widget;	/* Error dialog box widget */
globalref Widget	uaf_r_main_window_widget;	/* Main window widget info */

/* Local storage. */

Arg			args[ARG_COUNT];		/* Genral X argument list */
XtCallbackRec		callback_list[2];		/* Callback list */
XmString		compound_string;		/* General compound string */
char			message_text[MAX_TEMP_STRING];	/* Message test storage */
short int		message_text_length;		/* Length of message text */
unsigned int		simple_desc[2];			/* Simple descriptor */
unsigned int		status;				/* Routine exit status */

/* If the supplied error code indicates some failure, pop up an appropriate
** dialog box.  Otherwise, simply return. */

if ($VMS_STATUS_SEVERITY (error_code) != STS$K_SUCCESS)
    {

/* Get the text associated with the error code. */

    simple_desc[0] = sizeof message_text;
    simple_desc[1] = &message_text;

    SYS$GETMSG (error_code,
		&message_text_length,
		&simple_desc,
		1,					/* Include only message text */
		0);
    message_text[message_text_length] = '\0';		/* Terminate the string ASCIZ fashion */

/* Set up the callback list. */

    callback_list[0].callback = (caddr_t) AUTHORIZE$DECW_ERROR_DISMISS;
    callback_list[0].closure = 0;
    callback_list[1].callback = NULL;			/* Terminate the callback list */
    callback_list[1].closure = 0;

/* Set up the necessary arguments for the dialog box creation.  These are
** common to all the dialog boxes created. */

    XtSetArg (args[0],
	      XmNcancelCallback,
	      callback_list);
    XtSetArg (args[1],
	      XmNokCallback,
	      callback_list);
    compound_string = XmStringCreateSimple (message_text);
    XtSetArg (args[2],
	      XmNmessageString,
	      compound_string);
    XtSetArg (args[3],
	      XmNdialogStyle,
	      XmDIALOG_FULL_APPLICATION_MODAL);


/* Based upon the severity of the error code, do the necessary popup. */

    switch ($VMS_STATUS_SEVERITY (error_code))
	{
	case STS$K_WARNING:
	    {
	    uaf_r_error_dialog_widget = XmCreateWarningDialog (uaf_r_main_window_widget,
							       "Warning_box",
							       args,
							       ARG_COUNT);
	    break;
	    }

	case STS$K_ERROR:
	case STS$K_SEVERE:
	    {
	    uaf_r_error_dialog_widget = XmCreateErrorDialog (uaf_r_main_window_widget,
							     "Error_box",
							     args,
							     ARG_COUNT);
	    break;
	    }

	case STS$K_INFO:
	    {
	    uaf_r_error_dialog_widget = XmCreateInformationDialog (uaf_r_main_window_widget,
								   "Informational_box",
								   args,
								   ARG_COUNT);
	    break;
	    }
	}

/* Make the dialog box visible. */

    XtManageChild (uaf_r_error_dialog_widget);

    XtFree (compound_string);
    }
}

extern void AUTHORIZE$DECW_ERROR_DISMISS ()

{
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine is called to dismiss the "error" dialog box.
**
**  FORMAL PARAMETERS:
**
**	None
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

/* Global references. */

globalref Widget	uaf_r_error_dialog_widget;	/* Error dialog box widget */

XtUnmanageChild (uaf_r_error_dialog_widget);
}

extern void AUTHORIZE$DECW_LIST_PROC (widget_id, tag, list)

Widget				widget_id;
int				*tag;
XmListCallbackStruct		*list;
{
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This callback routine is invoked to obtain the information about
**	an item selected from one of the main window list boxes.
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

extern void			AUTHORIZE$DECW_ACTIVATE_PROC ();
extern void			AUTHORIZE$DECW_ERROR ();
extern unsigned int		AUTHORIZE$GET_USER_INFO ();
extern void			AUTHORIZE$SET_UP_ATTRIBUTES ();

/* Global references. */

globalref unsigned int		uaf_l_current_user_index;				/* Index of current user */
globalref struct ITMDEF		(*uaf_r_current_group_info)[];				/* Current group UAF info */
globalref struct UAFLAG		uaf_r_flags;						/* Miscellaneous utility flags */
globalref Widget		uaf_r_widget_id_array[uaf$c_max_widget_code];		/* Array of Widget IDs in use */
globalref char			uaf_t_current_groupname[];				/* Current group name */

/* Local storage. */

XmPushButtonCallbackStruct	*pb_callback;			/* Callback structure for simulated pushbutton activate */
unsigned int			status;				/* Routine exit status */
int				widget_number = *tag;

/* Compound string component storage. */

XmStringCharSet			*cs_char_set;
XmStringContext			*cs_context;
XmStringDirection		*cs_direction;
Boolean				*cs_separator;
char				**cs_text;

/* Debug information. */

#ifdef DEBUGGING
printf ("list proc\n");
#endif /* DEBUGGING */

/* Get the actual text from the list element selected.  This will be either a
** username or group name (depending upon the list).  This assumes the text
** will be the first segment. */

XmStringInitContext (&cs_context,
		     list->item);
XmStringGetNextSegment (cs_context,
			&cs_text,
			&cs_char_set,
			&cs_direction,
			&cs_separator);
XmStringFreeContext (cs_context);

/* Use the widget ID to determine whether the item selection is for the user
**or group list box. */

if (widget_id == uaf_r_widget_id_array[uaf$c_main_user_list])
    {

/* For the user list, take the selected list item text, and set up the username
** to use for the operation. */

    XmTextSetString (uaf_r_widget_id_array[uaf$c_main_username],
		     cs_text);

/* Note the current position in the username list for later. */

    uaf_l_current_user_index = list->item_position;

/* If the list selection was via a double click, make it appear as though
** the user clicked on the modify push button. */

    if (list->reason == XmCR_DEFAULT_ACTION)
	{
	pb_callback = malloc (sizeof *pb_callback);	/* Allocate fake pushbutton callback structure */
	pb_callback->reason = XmCR_ACTIVATE;		/* Build fake pushbutton callback structure */
	pb_callback->event = NULL;
	pb_callback->click_count = 2;			/* For double click */
	AUTHORIZE$DECW_ACTIVATE_PROC (uaf_r_widget_id_array[uaf$c_main_modify],
				      &uaf$c_main_modify,
				      pb_callback);
	free (pb_callback);
	}
    }

if (widget_id == uaf_r_widget_id_array[uaf$c_main_group_list])
    {

/* For the group list, set up the gurrent group information. */

    strcpy (uaf_t_current_groupname, cs_text);

/* Because the current group is not changed all that often, get the authorization
** information.  This will allow the itemlist to be copied, rather than reading
** the information all over when a new authorization entry is created. */

    if (FALSE (AUTHORIZE$GET_USER_INFO (uaf_t_current_groupname,
					&uaf_r_current_group_info)))
	{

/* If there is a failure reading the authorization information, note the failure. */

	AUTHORIZE$DECW_ERROR (UAF$_UAFREADERR);
	return;
	}
    }

if (widget_id == uaf_r_widget_id_array[uaf$c_identifier_list])
    {

/* For the identifier list, take the selected list item text, and set up the
** identifier to use for the operation. */

    XmTextSetString (uaf_r_widget_id_array[uaf$c_identifier_name],
		     cs_text);

/* Set up the identifier's attributes correctly. */

    AUTHORIZE$SET_UP_ATTRIBUTES (cs_text);
    }

if (widget_id == uaf_r_widget_id_array[uaf$c_system_identifier_list])
    {

/* For the system identifier list, take the selected item text, and set up the
** identifier to use for the operation in the identifier window. */

    XmTextSetString (uaf_r_widget_id_array[uaf$c_identifier_name],
		     cs_text);
    }

/* The following check are rather simple.  They simply set an appropriate
** flag if the associated list is modified.  These flags are used later to
** determine whether or not to get the list infomation. */

if (widget_id == uaf_r_widget_id_array[uaf$c_entry_login_flag_list])
    uaf_r_flags.uaf_v_mod_login_flags = 1;		/* Note an entry as been selected or deselected */

if (widget_id == uaf_r_widget_id_array[uaf$c_entry_auth_priv_list])
    uaf_r_flags.uaf_v_mod_auth_privs = 1;		/* Note an entry as been selected or deselected */

if (widget_id == uaf_r_widget_id_array[uaf$c_entry_def_priv_list])
    uaf_r_flags.uaf_v_mod_def_privs = 1;		/* Note an entry as been selected or deselected */

/* Access restriction lists. */

if (widget_id == uaf_r_widget_id_array[uaf$c_access_pri_local_list])
    uaf_r_flags.uaf_v_mod_pri_local_access = 1;		/* Note an entry as been selected or deselected */

if (widget_id == uaf_r_widget_id_array[uaf$c_access_pri_batch_list])
    uaf_r_flags.uaf_v_mod_pri_batch_access = 1;		/* Note an entry as been selected or deselected */

if (widget_id == uaf_r_widget_id_array[uaf$c_access_pri_dialup_list])
    uaf_r_flags.uaf_v_mod_pri_dialup_access = 1;	/* Note an entry as been selected or deselected */

if (widget_id == uaf_r_widget_id_array[uaf$c_access_pri_remote_list])
    uaf_r_flags.uaf_v_mod_pri_remote_access = 1;	/* Note an entry as been selected or deselected */

if (widget_id == uaf_r_widget_id_array[uaf$c_access_pri_network_list])
    uaf_r_flags.uaf_v_mod_pri_network_access = 1;	/* Note an entry as been selected or deselected */

if (widget_id == uaf_r_widget_id_array[uaf$c_access_sec_local_list])
    uaf_r_flags.uaf_v_mod_sec_local_access = 1;		/* Note an entry as been selected or deselected */

if (widget_id == uaf_r_widget_id_array[uaf$c_access_sec_batch_list])
    uaf_r_flags.uaf_v_mod_sec_batch_access = 1;		/* Note an entry as been selected or deselected */

if (widget_id == uaf_r_widget_id_array[uaf$c_access_sec_dialup_list])
    uaf_r_flags.uaf_v_mod_sec_dialup_access = 1;	/* Note an entry as been selected or deselected */

if (widget_id == uaf_r_widget_id_array[uaf$c_access_sec_remote_list])
    uaf_r_flags.uaf_v_mod_sec_remote_access = 1;	/* Note an entry as been selected or deselected */

if (widget_id == uaf_r_widget_id_array[uaf$c_access_sec_network_list])
    uaf_r_flags.uaf_v_mod_sec_network_access = 1;	/* Note an entry as been selected or deselected */
}

extern void AUTHORIZE$DECW_PULL_PROC (widget_id, tag, reason)

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

unsigned int	status;				/* Routine exit status */
int		widget_number = *tag;

/* Debug information. */

#ifdef DEBUGGING
printf ("pull proc\n");
#endif /* DEBUGGING */
}

extern void AUTHORIZE$DECW_SET_VALUE (widget_id, tag, reason)

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

unsigned int	status;				/* Routine exit status */
int		widget_number = *tag;

/* Debug information. */

#ifdef DEBUGGING
printf ("set value\n");
#endif /* DEBUGGING */
}

extern void AUTHORIZE$DECW_TOGGLE_PROC (widget_id, tag, reason)

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

unsigned int	status;				/* Routine exit status */
int		widget_number = *tag;

/* Debug information. */

#ifdef DEBUGGING
printf ("toggle proc\n");
#endif /* DEBUGGING */
}

extern AUTHORIZE$GET_LIST_ENTRIES (widget_number, bit_stream, max_byte_count)

int				widget_number;
unsigned char			bit_stream[];
int				max_byte_count;
{
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine is called to get the selected set of list entries
**	and set the corresponding bits in a bit stream.  It assumes the
**	target bit stream has beein initially set to zero.
**
**  FORMAL PARAMETERS:
**
**	WIDGET_NUMBER		- Widget number
**	BIT_STREAM		- Pointer to a bit stream (set of bytes)
**	MAX_BYTE_COUNT		- Maximum number of bytes in the bit stream
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

/* Global references. */

globalref Widget	uaf_r_widget_id_array[uaf$c_max_widget_code];	/* Array of Widget IDs in use */

/* Local storage. */

int			index;			/* General index */
int			bit_position;		/* Bit in byte in bit stream */
int			buffer_position;	/* Byte in bit stream */
int			(*position_list)[];	/* List of entries selected */
int			position_count = 0;	/* Number of entries selected */
unsigned int		status;			/* Routine exit status */

/* Debug information. */

#ifdef DEBUGGING
printf ("get list entries\n");
#endif /* DEBUGGING */

/* Find out which entries have been selected.  If no entries have been
** selected, simply return; assuming the bit stream has been properly
** initialized. */

if (TRUE (XmListGetSelectedPos (uaf_r_widget_id_array[widget_number],
				&position_list,
				&position_count)))
    {

/* Loop through each entry in the position list, and set the corresponding
** bit in the bit stream. */

    for (index = 0; index < position_count; index++)
	{

/* Determine which bit in appropriate byte in the bit stream should be set. */

	buffer_position = ((*position_list)[index] - 1) / 8;
	bit_position = ((*position_list)[index] - 1) - (buffer_position * 8);

/* Range check the buffer position. */

	if (buffer_position > max_byte_count) LIB$SIGNAL (UAF$_INTERNAL2);

/* Set the bit. */

	bit_stream[buffer_position] = bit_stream[buffer_position] | (1 << bit_position);
	}

/* Release the memory for the position list. */

    XtFree (position_list);
    }
}

extern unsigned int AUTHORIZE$GET_LIST_ENTRY_TEXT (widget_number, position, entry_text)

int				widget_number;
int				position;
char				**entry_text;
{
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine is called to get the text associated with a list entry
**	at a specified position in the list.  Because the performance is not
**	very good, this routine should only be used where absolutely necessary.
**
**  FORMAL PARAMETERS:
**
**	WIDGET_NUMBER		- Widget number
**	POSITION		- Index of the entry for which the text is to be
**				  obtained.  This is biased such that the first
**				  identifier in the list is position 1; NOT 0.
**	ENTRY_TEXT		- Pointer to the character string storage to
**				  contain the text
**
**  RETURN VALUE:
**
**	SS$_NORMAL	if all succeeded normally
**	SS$_BADPARAM	if specified entry is not in the list
**
**  SIDE EFFECTS:
**
**	None
**
**--
*/

/* Global references. */

globalref Widget	uaf_r_widget_id_array[uaf$c_max_widget_code];	/* Array of Widget IDs in use */

/* Local storage. */

Arg			args[2];		/* Genral X argument list */
unsigned int		status;			/* Routine exit status */
int			table_entry_count;	/* Number of entries in the table */

/* Compound string component storage. */

XmStringTable		cstring_table;		/* Compound string table */
XmStringCharSet		*cs_char_set;
XmStringContext		*cs_context;
XmStringDirection	*cs_direction;
Boolean			*cs_separator;
char			**cs_text;

/* Debug information. */

#ifdef DEBUGGING
printf ("get list entry text\n");
#endif /* DEBUGGING */

/* Get the string table containing all the list entries. */

XtSetArg (args[0],
	  XmNitems,
	  &cstring_table);
XtSetArg (args[1],
	  XmNitemCount,
	  &table_entry_count);
XtGetValues (uaf_r_widget_id_array[uaf$c_identifier_list],
	     args,
	     2); 

/* Range check the position argument. */

if (position > table_entry_count) return SS$_BADPARAM;

/* Get the actual text from the list element (compound string) selected.  This
** assumes the text will be the first segment. */

XmStringInitContext (&cs_context,
		     cstring_table[position - 1]);					/* Note -1 bias */
XmStringGetNextSegment (cs_context,
			&cs_text,
			&cs_char_set,
			&cs_direction,
			&cs_separator);
XmStringFreeContext (cs_context);

/* Copy the text string back to the caller's buffer. */

strcpy (entry_text, cs_text);

/* That's it. */

return SS$_NORMAL;
}

extern unsigned int AUTHORIZE$GET_TOGGLE_BUTTONS (base_widget_number, max_widget_number, bit_stream, max_byte_count)

int				base_widget_number;
int				max_widget_number;
unsigned char			bit_stream[];
int				max_byte_count;
{
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine is called to get the state of a set of toggle buttons and
**	set the corresponding bits in a bit stream.  It assumes the target bit
**	stream has beein initially set to zero.
**
**  FORMAL PARAMETERS:
**
**	BASE_WIDGET_NUMBER	- Widget number for bit 0
**	MAX_WIDGET_NUMBER	- Last valid (bit) widget number
**	BIT_STREAM		- Pointer to a bit stream (set of bytes)
**	MAX_BYTE_COUNT		- Maximum number of bytes in the bit stream
**
**  RETURN VALUE:
**
**	SS$_NORMAL if successful
**	SS$_IVBUFLEN if more toggle buttons than storage
**
**  SIDE EFFECTS:
**
**	None
**
**--
*/

/* Global references. */

globalref Widget	uaf_r_widget_id_array[uaf$c_max_widget_code];		/* Array of Widget IDs in use */

/* Local storage. */

int			bit_index;			/* Bitstream index */
int			buffer_index;			/* General buffer index */
unsigned int		status;				/* Routine exit status */

/* Debug information. */

#ifdef DEBUGGING
printf ("get toggle buttons\n");
#endif /* DEBUGGING */

/* Loop through each toggle button and set the state of the corresponding bit
** in the bit stream. */

for (bit_index = 0; (base_widget_number + bit_index) < max_widget_number; bit_index++)
    {
    buffer_index = bit_index / 8;
    if (uaf_r_widget_id_array[base_widget_number + bit_index] != 0)
	{
	if (TRUE (XmToggleButtonGetState (uaf_r_widget_id_array[base_widget_number + bit_index])))
	    {
	    if (buffer_index >= max_byte_count) return SS$_IVBUFLEN;	/* Error if out of bounds */
	    bit_stream[buffer_index] = bit_stream[buffer_index] | (1 << (bit_index - (buffer_index * 8)));
	    }
	}
    }

return SS$_NORMAL;
}

extern void AUTHORIZE$SELECT_LIST_ENTRIES (widget_number, bit_stream, byte_count)

int				widget_number;
unsigned char			bit_stream[];
int				byte_count;
{
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine is called to select a set of list entries to
**	correspond to the state of a set of bits in a bit stream.
**	In addition, the list is positioned such that the first item
**	of the list is also the first visible item of the list.
**
**  FORMAL PARAMETERS:
**
**	WIDGET_NUMBER		- Widget number
**	BIT_STREAM		- Pointer to a bit stream (set of bytes)
**	BYTE_COUNT		- Number of bytes in the bit stream
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

/* Global references. */

globalref Widget	uaf_r_widget_id_array[uaf$c_max_widget_code];	/* Array of Widget IDs in use */

/* Local storage. */

int			bit_index;			/* Bitstream index */
int			buffer_index;			/* General buffer index */
unsigned int		status;				/* Routine exit status */

/* Debug information. */

#ifdef DEBUGGING
printf ("select list entries\n");
#endif /* DEBUGGING */

/* Deselect all items in the list to start off. */

XmListDeselectAllItems (uaf_r_widget_id_array[widget_number]);

/* Loop through each byte in the supplied buffer looking at the state of each
** bit.  Select the list entries corresponding to the state of the bits in the
** bit stream. */

for (buffer_index = 0; buffer_index < byte_count; buffer_index++)	/* Each byte */
    {
    for (bit_index = 0; bit_index < 8; bit_index++)			/* Each bit in the byte */
	{

/* If the bit is set, set the selection state of the corresponding list entry. */

	if ((bit_stream[buffer_index] & (1 << bit_index)) != 0)
	    XmListSelectPos (uaf_r_widget_id_array[widget_number],
			     (buffer_index * 8) + bit_index + 1,
			     1);
	}
    }

/* Set the position to the first entry of the list. */

XmListSetPos (uaf_r_widget_id_array[widget_number],
	      1);

}

extern void AUTHORIZE$SET_TOGGLE_BUTTONS (bit_stream, byte_count, base_widget_number, max_widget_number)

unsigned char			bit_stream[];
int				byte_count;
int				base_widget_number;
int				max_widget_number;
{
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine is called to set the state of a set of toggle buttons to
**	correspond to the state of a set of bits in a bit stream.
**
**  FORMAL PARAMETERS:
**
**	BIT_STREAM		- Pointer to a bit stream (set of bytes)
**	BYTE_COUNT		- Number of bytes in the bit stream
**	BASE_WIDGET_NUMBER	- Widget number for bit 0
**	MAX_WIDGET_NUMBER	- Last valid (bit) widget number
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

/* Global references. */

globalref Widget	uaf_r_widget_id_array[uaf$c_max_widget_code];		/* Array of Widget IDs in use */

/* Local storage. */

int			bit_index;			/* Bitstream index */
int			buffer_index;			/* General buffer index */
unsigned int		status;				/* Routine exit status */

/* Debug information. */

#ifdef DEBUGGING
printf ("set toggle buttons\n");
#endif /* DEBUGGING */

/* Loop through each byte in the supplied buffer looking at the state of each
** bit.  This is used to set the state of the corresponding toggle button.
** This is done until all the bytes in the buffer are taken care of or until
** the last defined widget number is encountered. */

for (buffer_index = 0; buffer_index < byte_count; buffer_index++)	/* Each byte */
    {
    for (bit_index = 0; bit_index < 8; bit_index++)			/* Each bit in the byte */
	{

/* If I am within the range of widgets, set the toggle button to the
** appropriate state.  Otherwise simple skip it. */

	if (((buffer_index * 8) + base_widget_number + bit_index) < max_widget_number)
	    {

/* Make sure there is a widget number for the bit. */

	    if (uaf_r_widget_id_array[base_widget_number + (buffer_index * 8) + bit_index] != 0)
		{

/* Test the state of the bit, and set the state of the corresponding toggle button. */

		if ((bit_stream[buffer_index] & (1 << bit_index)) != 0)
		    XmToggleButtonSetState (uaf_r_widget_id_array[base_widget_number + (buffer_index * 8) + bit_index],
					    1,
					    0);
		else
		    XmToggleButtonSetState (uaf_r_widget_id_array[base_widget_number + (buffer_index * 8) + bit_index],
					    0,
					    0);
		}
	    }
	}
    }
}

extern void AUTHORIZE$UNMAP_WIDGET (widget_id)

Widget				widget_id;
{
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine is called to ummap a widget.  Special handling is needed
**	if the widget exists as an icon; as opposed to a notmal window.
**
**  FORMAL PARAMETERS:
**
**	WIDGET_ID	- id of widget to unmap
**
**  RETURN VALUE:
**
**	SS$_NORMAL
**
**  SIDE EFFECTS:
**
**	None
**
**--
*/

/* Local storage. */

XUnmapEvent	synth_unmap_event;

/* Debug information. */

#ifdef DEBUGGING
printf ("unmap widget\n");
#endif /* DEBUGGING */

/* First off, unmap the window. */

XUnmapWindow (XtDisplay (widget_id), XtWindow (widget_id));

/* Next, build a synthetic UnmapNotify event to the root window. */

synth_unmap_event.type = UnmapNotify;					/* Type is UnmapNotify */
synth_unmap_event.event = DefaultRootWindow (XtDisplay (widget_id));	/* To root window */
synth_unmap_event.window = XtWindow (widget_id);			/* Destination window */
synth_unmap_event.from_configure = False;

/* Lastly, send it. */
XSendEvent (XtDisplay (widget_id),
	    DefaultRootWindow (XtDisplay (widget_id)),
	    False,
	    (SubstructureRedirectMask | SubstructureNotifyMask),
	    (XEvent *) &synth_unmap_event);

/* Make sure it happens immediately. */

XFlush (XtDisplay (widget_id));
}

extern void AUTHORIZE$WATCH_CURSOR_OFF (top_level_widget_id, widget_id)

Widget				top_level_widget_id;
Widget				widget_id;
{
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine is called to set the cursor back to the normal (default)
**	cursor.
**
**  FORMAL PARAMETERS:
**
**	TOP_LEVEL_WIDGET_ID	- id of top level widget
**	WIDGET_ID		- id of widget
**
**  RETURN VALUE:
**
**	SS$_NORMAL
**
**  SIDE EFFECTS:
**
**	None
**
**--
*/

/* Debug information. */

#ifdef DEBUGGING
printf ("watch cursor off\n");
#endif /* DEBUGGING */

/* Remove the wait (watch) cursor. */

XUndefineCursor (XtDisplay (top_level_widget_id),
		 XtWindow (widget_id));
XFlush (XtDisplay (top_level_widget_id));	/* Now */
}

extern void AUTHORIZE$WATCH_CURSOR_ON (top_level_widget_id, widget_id, wait_cursor)

Widget				top_level_widget_id;
Widget				widget_id;
Cursor				wait_cursor;
{
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine is called to set the cursor to the wait (watch) cursor.
**
**  FORMAL PARAMETERS:
**
**	TOP_LEVEL_WIDGET_ID	- id of top level widget
**	WIDGET_ID		- id of widget
**
**  RETURN VALUE:
**
**	SS$_NORMAL
**
**  SIDE EFFECTS:
**
**	None
**
**--
*/

/* Debug information. */

#ifdef DEBUGGING
printf ("watch cursor on\n");
#endif /* DEBUGGING */

/* Set the wait (watch) cursor. */

XDefineCursor (XtDisplay (top_level_widget_id),
	       XtWindow (widget_id),
	       wait_cursor);
XFlush (XtDisplay (top_level_widget_id));	/* Now */
}
