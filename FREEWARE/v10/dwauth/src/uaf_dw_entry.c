#module UAF_DW_ENTRY "X-2"

/*
**++
**  FACILITY:	Authorization record maintenance utility
**
**  MODULE DESCRIPTION:
**
**		This module contains the various DECwindows entry window
**		support routines needed by the DECwindows AUTHORIZE utility.
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

#include <decw$include:mrmappl>

#include "uaf_header"

/*
**
**  FORWARD ROUTINES
**
*/

#pragma noinline (AUTHORIZE$ENTRY_PROCESS)
#pragma noinline (AUTHORIZE$FINISH_ENTRY)
#pragma noinline (AUTHORIZE$GET_ENTRY)
#pragma noinline (AUTHORIZE$SET_UP_ENTRY)

extern void AUTHORIZE$ENTRY_PROCESS (widget_id, tag, reason)

Widget				widget_id;
int				*tag;
XmAnyCallbackStruct		*reason;
{
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine is called to initiate processing of an
**	authorization entry.
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
extern unsigned int		AUTHORIZE$GET_USER_INFO ();
extern void			AUTHORIZE$ITMLST_ADD_ITEM ();
extern void			AUTHORIZE$SET_UP_ENTRY ();
extern void			AUTHORIZE$SET_UP_USERNAME ();

/* Global references. */

globalref unsigned int		uaf_l_current_user_index;	/* Index of current user */
globalref unsigned int		uaf_l_current_user_max;		/* Count of entries in user list */
globalref Widget		uaf_r_access_pri_window_widget;	/* Primary access restriction window widget info */
globalref Widget		uaf_r_access_sec_window_widget;	/* Secondary access restriction window widget info */
globalref struct ITMDEF		(*uaf_r_current_user_info)[];	/* Current user UAF info */
globalref Widget		uaf_r_entry_window_widget;	/* Entry window widget info */
globalref struct ITMDEF		(*uaf_r_template_info)[];	/* Modification template information */
globalref char			uaf_t_current_username[];	/* Current username */

/* Local storage. */

int				index;				/* General index */
unsigned int			status;				/* Routine exit status */
int				widget_number = *tag;

/* Debug information. */

#ifdef DEBUGGING
printf ("entry process\n");
#endif /* DEBUGGING */

/* Call the appropriate routine, based upon the pushbutton. */

switch (widget_number)
    {
    case uaf$c_entry_cancel:
	{
	XtUnmanageChild (uaf_r_entry_window_widget);

/* If there is an access window up, tear it down. */

	if (XtIsManaged (uaf_r_access_pri_window_widget))
	    XtUnmanageChild (uaf_r_access_pri_window_widget);
	if (XtIsManaged (uaf_r_access_sec_window_widget))
	    XtUnmanageChild (uaf_r_access_sec_window_widget);
	break;
	}
    case uaf$c_entry_ok_next:
	{

/* If I am positioned to the last possible entry, don't attempt to go any
** further.  Otherwise finish up with the current entry and do the next one. */

	if (uaf_l_current_user_index < uaf_l_current_user_max)
	    {

/* Set up for the next user in the list. */

	    ++uaf_l_current_user_index;
	    AUTHORIZE$SET_UP_USERNAME (NULL,
				       uaf_l_current_user_index);

/* Get the authorization information. */

	    if (FALSE (AUTHORIZE$GET_USER_INFO (uaf_t_current_username,
						&uaf_r_current_user_info)))
		{

/* Since an error has occurred getting the authorization information for the
** next user, reset the username, note the error, and dismiss; leaving the
** existing window unchanged. */

		--uaf_l_current_user_index;
		AUTHORIZE$SET_UP_USERNAME (NULL,
					   uaf_l_current_user_index);
		AUTHORIZE$DECW_ERROR (UAF$_UAFREADERR);
		return;
		}

/* Now set up the information in the entry window. */

	    AUTHORIZE$SET_UP_ENTRY (uaf_r_current_user_info);
	    }
	XtManageChild (uaf_r_entry_window_widget);
	break;
	}

    case uaf$c_entry_ok_previous:
	{

/* If I am positioned to the first possible entry, don't attempt to go any
** further.  Otherwise finish up with the current entry and do the next one. */

	if (uaf_l_current_user_index > 1)
	    {

/* Set up for the previous user in the list. */

	    --uaf_l_current_user_index;
	    AUTHORIZE$SET_UP_USERNAME (NULL,
				       uaf_l_current_user_index);

/* Get the authorization information. */

	    if (FALSE (AUTHORIZE$GET_USER_INFO (uaf_t_current_username,
						&uaf_r_current_user_info)))
		{

/* Since an error has occurred getting the authorization information for the
** next user, reset the username, note the error, and dismiss; leaving the
** existing window unchanged. */

		++uaf_l_current_user_index;
		AUTHORIZE$SET_UP_USERNAME (NULL,
					   uaf_l_current_user_index);
		AUTHORIZE$DECW_ERROR (UAF$_UAFREADERR);
		return;
		}

/* Now set up the information in the entry window. */

	    AUTHORIZE$SET_UP_ENTRY (uaf_r_current_user_info);
	    }
	XtManageChild (uaf_r_entry_window_widget);
	break;
	}

    case uaf$c_entry_apply_template:
	{

/* Apply the template information to the current authorization entry.  This is
** done by taking each of the (template) item list entries and updating the
** current user information. */

	for (index = 0; (*uaf_r_template_info)[index].itm$w_itmcod != 0; index++)
	    {

/* Don't modify the username of the current entry.  This allows the user to
** put some meaningful information in the username field of the template
** authorization entry. */

	    if ((*uaf_r_template_info)[index].itm$w_itmcod != UAI$_USERNAME)
		AUTHORIZE$ITMLST_ADD_ITEM (&uaf_r_current_user_info,
					   (*uaf_r_template_info)[index].itm$w_itmcod,
					   (*uaf_r_template_info)[index].itm$w_bufsiz,
					   (*uaf_r_template_info)[index].itm$l_bufadr);
	    }

/* Use the updated information to set up the authorization window.  It would
** be possible to simply update those fields that changed, but doing it this
** way simplifies the code. */

	AUTHORIZE$SET_UP_ENTRY (uaf_r_current_user_info);
	XtManageChild (uaf_r_entry_window_widget);
	break;
	}
    }
}

extern void AUTHORIZE$FINISH_ENTRY (widget_id, tag, reason)

Widget				widget_id;
int				*tag;
XmAnyCallbackStruct		*reason;
{
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine is called to finish up the processing of an
**	authorization entry.
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

extern unsigned int		AUTHORIZE$ADD_LIST_ENTRY ();
extern unsigned int		AUTHORIZE$CRE_USER_INFO ();
extern void			AUTHORIZE$DECW_ERROR ();
extern unsigned int		AUTHORIZE$FIND_USER_ENTRY ();
extern unsigned int		AUTHORIZE$GET_ENTRY ();
extern void			AUTHORIZE$ITMLST_ADD_ITEM ();
extern void			AUTHORIZE$ITMLST_COMPARE ();
extern void			AUTHORIZE$ITMLST_DELETE ();
extern void			AUTHORIZE$ITMLST_MERGE ();
extern void			AUTHORIZE$ITMLST_UPDATE ();
extern void			AUTHORIZE$SET_UP_USERNAME ();
extern unsigned int		AUTHORIZE$SET_USER_INFO ();
extern unsigned int		AUTHORIZE$MATCH_NAME ();

/* Global references. */

globalref unsigned int		uaf_l_current_group_max;	/* Count of entries in group list */
globalref unsigned int		uaf_l_current_user_max;		/* Count of entries in user list */
globalref int			uaf_l_main_operation;		/* Operation (i.e., create, modify, etc.) */
globalref Widget		uaf_r_access_pri_window_widget;	/* Primary access restriction window widget info */
globalref Widget		uaf_r_access_sec_window_widget;	/* Secondary access restriction window widget info */
globalref struct ITMDEF		(*uaf_r_current_group_info)[];	/* Current group UAF info */
globalref struct ITMDEF		(*uaf_r_current_user_info)[];	/* Current user UAF info */
globalref Widget		uaf_r_entry_window_widget;	/* Entry window widget info */
globalref struct USRLSTDEF	uaf_r_group_list;		/* Group user name list head */
globalref struct USRLSTDEF	uaf_r_master_list;		/* Complete user list (from the file) */
globalref struct ITMDEF		(*uaf_r_template_info)[];	/* Modification template information */
globalref struct USRLSTDEF	uaf_r_user_list;		/* User name list head */
globalref Widget		uaf_r_widget_id_array[];	/* Array of Widget IDs in use */
globalref char			uaf_t_current_username[];	/* Current username */
globalref char			uaf_t_username_filter[sizeof uaf_r_user_list.uaf$t_usrlst_name];	/* Current username filter string */

/* Local storage. */

XmString			current_username;		/* Compound string for current username */
int				index;				/* General index */
unsigned char			*item_bufadr;			/* Item buffer address from item list entry */
unsigned int			item_bufsiz;			/* Item buffer size from item list entry */
unsigned int			item_code;			/* Item code from item list entry */
int				list_count;			/* Number of list entries passed before adding item */
struct USRLSTDEF		*new_entry;			/* User list entry just added */
int				new_entry_uic = 0;		/* UIC for new entry */
struct ITMDEF			(*new_user_info)[] = 0;		/* Item list for new information */
unsigned int			status;				/* Routine exit status */
struct USRLSTDEF		*user_entry;			/* Current user list entry */
int				widget_number = *tag;

/* Debug information. */

#ifdef DEBUGGING
printf ("finish entry\n");
#endif /* DEBUGGING */

/* Do any needed initialization. */

current_username = XmStringCreateSimple (uaf_t_current_username);

/* Look through all the item list entries for the current user, and see
** if a UAI$_UIC entry is present.  If it is, note the UIC for later. */

for (index = 0; (*uaf_r_current_user_info)[index].itm$w_itmcod != 0; index++)
    {

/* Set up information from item list entry. */

    item_code = (*uaf_r_current_user_info)[index].itm$w_itmcod;
    item_bufsiz = (*uaf_r_current_user_info)[index].itm$w_bufsiz;
    item_bufadr = (*uaf_r_current_user_info)[index].itm$l_bufadr;

/* If a UIC entry is found, note the UIC for later and trip out of the loop. */

    if (item_code == UAI$_UIC)
	{
	new_entry_uic = (*(unsigned int (*)[])item_bufadr)[0];
	break;
	}
    }

/* Based upon the operation being performed, do the necessary final steps
** to complete the operation started (i.e., create, modify, etc.) on the
** authorization entry. */

switch (uaf_l_main_operation)
    {

/* The modification template is complete. */

    case uaf$c_cmd_modify_template:
	{

/* Get the information from the (template) authorization entry window, and
** build an appropriate item list. */

	if (FALSE (AUTHORIZE$GET_ENTRY (&new_user_info,
					1)))
	    {
	    XtFree (current_username);
	    return;					/* Return with windows still up on error*/
	    }

/* Compare the new information with the existing template information, and
** build an item list with just the differences. */

	AUTHORIZE$ITMLST_COMPARE (new_user_info,
				  uaf_r_template_info,
				  &new_user_info);

/* The changed item list entries must now be added to the original template
** information.  This is done by taking each of the changed item list entries
** and adding it to the original template information in turn. */

	for (index = 0; (*new_user_info)[index].itm$w_itmcod != 0; index++)
	    {
	    AUTHORIZE$ITMLST_ADD_ITEM (&uaf_r_template_info,
				       (*new_user_info)[index].itm$w_itmcod,
				       (*new_user_info)[index].itm$w_bufsiz,
				       (*new_user_info)[index].itm$l_bufadr);
	    }

/* Delete the temporary (new user information) item list. */

	AUTHORIZE$ITMLST_DELETE (&new_user_info);
	break;
	}

/* A new entry is being created. */

    case uaf$c_main_create:
	{

/* Get the information from the authorization entry window, and build an
** appropriate item list. */

	if (FALSE (AUTHORIZE$GET_ENTRY (&uaf_r_current_user_info,
					0)))
	    {
	    XtFree (current_username);
	    return;					/* Return with windows still up on error*/
	    }

/* Merge the new user info item list with the current default info item
** list.  This will insure all information is present and accounted for. */

	AUTHORIZE$ITMLST_MERGE (uaf_r_current_user_info,
				uaf_r_current_group_info,
				&uaf_r_current_user_info);

/* Create a new authorization entry with the specified information. */

	if (FALSE (AUTHORIZE$CRE_USER_INFO (uaf_t_current_username,
					    uaf_r_current_user_info)))
	    {

/* Since an error has occurred creating the authorization information for the
** user, note the error and dismiss; leaving the existing window unchanged. */

	    AUTHORIZE$DECW_ERROR (UAF$_UAFWRITERR);
	    XtFree (current_username);
	    return;
	    }

/* Add the new entry to the master list. */

	AUTHORIZE$ADD_LIST_ENTRY (uaf_t_current_username,
				  new_entry_uic,
				  &uaf_r_master_list.uaf$l_usrlst_flink,
				  &new_entry);

/* If the newly created entry passes the filter check, add it to the use
** list. */

	if (TRUE (AUTHORIZE$MATCH_NAME (strlen (uaf_t_current_username),
					uaf_t_current_username,
					strlen (uaf_t_username_filter),
					uaf_t_username_filter)))
	    {

/* Locate the entry in the user list after which the new entry will be added. */

	    AUTHORIZE$FIND_USER_ENTRY (&uaf_r_user_list.uaf$l_usrlst_flink,
				       uaf_t_current_username,
				       FIND_NAME_LESS,
				       &list_count,
				       &user_entry);

/* USER_ENTRY now points to the "list head"; where the new entry is to be
** added.  Do it. */

	    AUTHORIZE$ADD_LIST_ENTRY (uaf_t_current_username,
				      NULL,
				      &user_entry->uaf$l_usrlst_flink,
				      &new_entry);
	    ++uaf_l_current_user_max;				/* One more user */
	    XmListAddItem (uaf_r_widget_id_array[uaf$c_main_user_list],
			   current_username,
			   list_count);

/* Set up the current username as the selected one. */

	    AUTHORIZE$SET_UP_USERNAME (uaf_t_current_username,
				       NULL);
	    }

/* If the newly created entry represents a new group (member number 177777)
** add it to the group user list. */

	for (index = 0; (*uaf_r_current_user_info)[index].itm$w_itmcod != 0; index++)
	    {

/* Set up information from item list entry. */

	    item_code = (*uaf_r_current_user_info)[index].itm$w_itmcod;
	    item_bufsiz = (*uaf_r_current_user_info)[index].itm$w_bufsiz;
	    item_bufadr = (*uaf_r_current_user_info)[index].itm$l_bufadr;

/* Look through all the item list entries for the current user, and see
** if a UAI$_UIC entry is present.  If it is, check to see if the member
** portion of the UIC is 177777 (octal). */

	    if (item_code == UAI$_UIC)
		{
		if ((*(unsigned short int (*)[])item_bufadr)[0] == 0177777)
		    {

/* Locate the entry in the group list after which the new entry will be added. */

		    AUTHORIZE$FIND_USER_ENTRY (&uaf_r_group_list.uaf$l_usrlst_flink,
					       uaf_t_current_username,
					       FIND_NAME_LESS,
					       &list_count,
					       &user_entry);

/* USER_ENTRY now points to the list head; where the new entry is to be
** added.  Do it. */

		    AUTHORIZE$ADD_LIST_ENTRY (uaf_t_current_username,
					      NULL,
					      &user_entry->uaf$l_usrlst_flink,
					      &new_entry);
		    ++uaf_l_current_group_max;				/* One more group */
		    XmListAddItem (uaf_r_widget_id_array[uaf$c_main_group_list],
				   uaf_t_current_username,
				   list_count);
		    }
		}
	    }
	break;
	}

/* An existing entry is being modified. */

    case uaf$c_main_modify:
	{

/* Get the information from the authorization entry window, and build an
** appropriate item list. */

	if (FALSE (AUTHORIZE$GET_ENTRY (&new_user_info,
					0)))
	    {
	    XtFree (current_username);
	    return;					/* Return with windows still up on error*/
	    }

/* Compare the new information with the current information, and build an item
** list with just the differences. */

	AUTHORIZE$ITMLST_COMPARE (new_user_info,
				  uaf_r_current_user_info,
				  &new_user_info);

/* Update the authorization entry with the specified information. */

	if (FALSE (AUTHORIZE$SET_USER_INFO (uaf_t_current_username,
					    new_user_info)))
	    {

/* Since an error has occurred setting the authorization information for the
** user, note the error and dismiss; leaving the existing window unchanged. */

	    AUTHORIZE$DECW_ERROR (UAF$_UAFWRITERR);
	    XtFree (current_username);
	    return;
	    }

/* Delete the temporary (new user information) item list. */

	AUTHORIZE$ITMLST_DELETE (&new_user_info);
	break;
	}
    }

/* Since I am finished with then authorization entry window, tear it down. */

XtUnmanageChild (uaf_r_entry_window_widget);

/* If there is an access window up, tear it down. */

if (XtIsManaged (uaf_r_access_pri_window_widget))
    XtUnmanageChild (uaf_r_access_pri_window_widget);
if (XtIsManaged (uaf_r_access_sec_window_widget))
    XtUnmanageChild (uaf_r_access_sec_window_widget);

XtFree (current_username);
}

extern unsigned int AUTHORIZE$GET_ENTRY (itmlst, ignore_null_fields)

struct ITMDEF		(**itmlst)[];
int			ignore_null_fields;
{
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine is called to get the information from the authorization
**	entry window, and convert it to the appropriate ($xxxUAI) item list
**	format.
**
**  FORMAL PARAMETERS:
**
**	ITMLST			- Address of a pointer containing the pointer
**				  to the new item list
**	IGNORE_NULL_FIELDS	- = 0 to accept empty (zero length) fields
**				  = 1 to ignore empty (zero length) fields
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

/* External routines. */

extern unsigned int		AUTHORIZE$CONVERT_UIC ();
extern void			AUTHORIZE$DECW_ERROR ();
extern unsigned int		AUTHORIZE$GET_LIST_ENTRIES ();
extern void			AUTHORIZE$ITMLST_ADD_ITEM ();
extern void			AUTHORIZE$ITMLST_DELETE ();

/* Global references. */

globalref struct UAFLAG		uaf_r_flags;				/* Miscellaneous utility flags */
globalref struct FMTABL		uaf_r_item_table[];			/* Entry window info format table */
globalref Widget		uaf_r_widget_id_array[uaf$c_max_widget_code];	/* Array of Widget IDs in use */
globalref char			*uaf_t_no_date_time;			/* String to use when no date/time present */
globalref char			*uaf_t_pre_expired;			/* String to use when no pwd chg dat/time present */

/* Local storage. */

unsigned char			ascii_string[MAX_TEMP_STRING];		/* ASCII string of some form */
char				device_terminator = ':';		/* Device string terminator character */
int				index;					/* General index */
unsigned int			simple_desc[2];				/* Simple character descriptor */
unsigned int			status;					/* Routine exit status */
char				*temp_string;				/* Temp string pointer */
int				temp_string_length;			/* Length of temp string */

/* Debug information. */

#ifdef DEBUGGING
printf ("get entry info\n");
#endif /* DEBUGGING */

/* Delete any existing item list. */

AUTHORIZE$ITMLST_DELETE (itmlst);

/* Loop through each entry in the item table and get the corresponding
** widget value. */

for (index = 0; index < UAI$_MAX_ITEM_CODE; index++)
    {

/* Clear out any previous buffer contents by filling the target buffer with
** the appropriate pad character. */

    memset (ascii_string, uaf_r_item_table[index].item_b_pad_char, sizeof ascii_string);

/* If there is a widget-id for the item table entry, get the value (a
** string) associated with the widget. */

    if ((uaf_r_item_table[index].item_l_widget != 0) &&
	(uaf_r_widget_id_array[uaf_r_item_table[index].item_l_widget] != 0))
	{
	temp_string = XmTextGetString (uaf_r_widget_id_array[uaf_r_item_table[index].item_l_widget]);
	temp_string_length = strlen (temp_string);

/* Only attempt to decode the information if there was something specified
** in the field.  Otherwise, simply skip it. */

	if ((temp_string_length != 0) ||
	    (ignore_null_fields == 0))
	    {

/* Now that I have a string, it is time to decode it.  The method used to
** decode the information is determined by various flags set in the item
** table entry. */

/* A counted ASCII string.  This is done by placing the string length in
** the first byte of the target string, and then copying the actual string
** text. */

	    if (uaf_r_item_table[index].item_r_flag_overlay.item_r_flag_bits.item_v_flag_cstr)
		{

/* Make sure the string copy will not exceed the maximum size of a counted
** ASCII string (255 bytes). */

		if (temp_string_length > 255) return SS$_BADPARAM;
		ascii_string[0] = temp_string_length;

/* Copy the string text. */

		memmove (&ascii_string[1], temp_string, ascii_string[0]);
		++temp_string_length;			/* For count byte */
		}

/* A simple ASCII string.  This is done by first filling the target string
** with the appropriate pad character, and then copying the actual string. */

	    if (uaf_r_item_table[index].item_r_flag_overlay.item_r_flag_bits.item_v_flag_asc)
		{

/* Make sure the text string is not too large for the authorization record. */

		if (temp_string_length > uaf_r_item_table[index].item_w_byte_cnt) return SS$_BADPARAM;

/* Copy the string text. */

		memmove (ascii_string, temp_string, temp_string_length);
		}

/* A value to be converted from a string.  This is done by converting the
** numeric string to the corresponding binary value.  The resulting binary
** value is placed in the target "string" to make the logic a little more
** simple; by allowing a common code path. */

	    if (uaf_r_item_table[index].item_r_flag_overlay.item_r_flag_bits.item_v_flag_val)
		{
		(*(int *)ascii_string) = strtoul (temp_string, NULL, 10);
		temp_string_length = uaf_r_item_table[index].item_w_byte_cnt;	/* Byte count of converted value */
		}

/* A date string.  This is pretty simple, but there is the special case where
** the date string is "(none)".  In this case, the resulting value is zero. */

	    if (uaf_r_item_table[index].item_r_flag_overlay.item_r_flag_bits.item_v_flag_dat)
		{

/* Check for date/time specified.  If there is no date/time specified, simply
** do nothing.  This will result in a zero value (because of the buffer preset).
** Otherwise, convert the date. */

		if ((strcmp (temp_string, uaf_t_no_date_time) != 0) &&
		    (strcmp (temp_string, uaf_t_pre_expired) != 0))
		    {

/* Initialize a simple character descriptor for the temp string buffer. */

		    simple_desc[0] = temp_string_length;
		    simple_desc[1] = temp_string;

/* Convert the time string. */

		    CHECK_RETURN (SYS$BINTIM (simple_desc,
					      ascii_string));
		    }
		temp_string_length = QUADWORD;				/* Converted value is a quadword */
		}

/* An item requiring special handling. */

	    if (uaf_r_item_table[index].item_r_flag_overlay.item_r_flag_bits.item_v_flag_spc)
		{
		if (index == UAI$_CPUTIM)
		    {

/* Check for date/time specified.  If there is no date/time specified, simply
** do nothing.  This will result in a zero value (because of the buffer preset).
** Otherwise, convert the date. */

		    if (strcmp (temp_string, uaf_t_no_date_time) != 0)
			{

/* Decode the CPU time limit.  This is done as a standard delta time, and
** then the second longword of the time is dropped. */

/* Initialize a simple character descriptor for the temp string buffer. */

			simple_desc[0] = temp_string_length;
			simple_desc[1] = temp_string;

/* Convert the time string. */

			CHECK_RETURN (SYS$BINTIM (simple_desc,
						  ascii_string));
			}
		    temp_string_length = LONGWORD;				/* Converted value is a longword */
		    }
		if (index == UAI$_UIC)
		    {

/* Decode the UIC.  In the simple case, this could simply mean interpreting
** a comma separated pair of octal numbers.  However, in the interest of
** flexibility, a separate routine is called to use TPARSE to decode the
** UIC string.  This allows octal numbers or identifiers to be used in the
** UIC string. */

/* Convert the UIC string. */

		    CHECK_RETURN (AUTHORIZE$CONVERT_UIC (temp_string,
							 ascii_string));
		    temp_string_length = LONGWORD;				/* Converted value is a longword */
		    }
		}

/* If necessary, use the maximal item size instead of the actual size
** (from the entry window).  This is necessary for such things as the
** account field. */

	    if (uaf_r_item_table[index].item_r_flag_overlay.item_r_flag_bits.item_v_flag_max)
		temp_string_length = uaf_r_item_table[index].item_w_byte_cnt;

/* Add the information to the item list; if it is not the username.  The
** username item is not added because it is a formal input parameter to
** the routines that actually create or modify the user authorization
** information. */

	    if (index != UAI$_USERNAME)
		AUTHORIZE$ITMLST_ADD_ITEM (itmlst,
					   index,
					   temp_string_length,
					   ascii_string);
	    }

/* Free up any storage allocated for the text string. */

	XtFree (temp_string);
	}
    }

/* Since all the standard information has been processed, do the special
** case information.  These are usually bit masks (streams) or composite
** fields. */

/* Default device and directory. */

if (uaf_r_widget_id_array[uaf$c_entry_default] != 0)
    {

/* Get the default string. */

    temp_string = XmTextGetString (uaf_r_widget_id_array[uaf$c_entry_default]);
    temp_string_length = strlen (temp_string);

    if (temp_string_length == 0)
	{

/* If the default string is empty, and enpty fields are being accepted, create
** the appropriate item list entries; otherwise simply ignore the field.  These
** fields are all zero, indicating a null counted string, but still having the
** maximal size. */

	if (ignore_null_fields == 0)
	    {
	    memset (ascii_string, uaf_r_item_table[UAI$_DEFDEV].item_b_pad_char, sizeof ascii_string);	/* Clear out and pad */
	    AUTHORIZE$ITMLST_ADD_ITEM (itmlst,						/* ITEM_M_FLAG_MAX */
				       UAI$_DEFDEV,
				       MAX_DEFDEV,
				       ascii_string);
	    memset (ascii_string, uaf_r_item_table[UAI$_DEFDIR].item_b_pad_char, sizeof ascii_string);	/* Clear out and pad */
	    AUTHORIZE$ITMLST_ADD_ITEM (itmlst,						/* ITEM_M_FLAG_MAX */
				       UAI$_DEFDIR,
				       MAX_DEFDIR,
				       ascii_string);
	    }
	}
    else
	{

/* Loop through the  source string looking for the device (terminated by a colon. */

	memset (ascii_string, uaf_r_item_table[UAI$_DEFDEV].item_b_pad_char, sizeof ascii_string);	/* Clear out and pad */
	ascii_string[0] = 0;						/* Reset byte count */
	for (index = 0; index < temp_string_length; index++)
	    {
	    ascii_string[ascii_string[0] + 1] = temp_string[index];	/* Copy one character */
	    ++ascii_string[0];						/* One more character in count byte */
	    if (temp_string[index] == device_terminator)
		{

/* Add the default device name item.  This must have maximal size. */

		AUTHORIZE$ITMLST_ADD_ITEM (itmlst,					/* ITEM_M_FLAG_MAX */
					   UAI$_DEFDEV,
					   MAX_DEFDEV,
					   ascii_string);
		memset (ascii_string, uaf_r_item_table[UAI$_DEFDIR].item_b_pad_char, sizeof ascii_string);  /* Clear out and pad */
		ascii_string[0] = 0;					/* Reset byte count */
		}
	    }

/* At this point, the count byte will contain the number of characters in the
** directory string. */

	AUTHORIZE$ITMLST_ADD_ITEM (itmlst,						/* ITEM_M_FLAG_MAX */
				   UAI$_DEFDIR,
				   MAX_DEFDIR,
				   ascii_string);
	}

/* Free up any storage allocated for the text string. */

    XtFree (temp_string);
    }

/* Default privilege mask. */

if (uaf_r_flags.uaf_v_mod_def_privs)
    {
    memset (ascii_string, uaf_r_item_table[UAI$_DEF_PRIV].item_b_pad_char, sizeof ascii_string);	/* Clear out and pad */
    AUTHORIZE$GET_LIST_ENTRIES (uaf$c_entry_def_priv_list,
				ascii_string,
				sizeof ascii_string);
    AUTHORIZE$ITMLST_ADD_ITEM (itmlst,
			       UAI$_DEF_PRIV,
			       QUADWORD,
			       ascii_string);
    }

/* Login flags. */

if (uaf_r_flags.uaf_v_mod_login_flags)
    {
    memset (ascii_string, uaf_r_item_table[UAI$_FLAGS].item_b_pad_char, sizeof ascii_string);	/* Clear out and pad */
    AUTHORIZE$GET_LIST_ENTRIES (uaf$c_entry_login_flag_list,
				ascii_string,
				sizeof ascii_string);
    AUTHORIZE$ITMLST_ADD_ITEM (itmlst,
			       UAI$_FLAGS,
			       LONGWORD,
			       ascii_string);
    }

/* Primary days. */

if (uaf_r_flags.uaf_v_mod_prime_days)
    {
    memset (ascii_string, uaf_r_item_table[UAI$_PRIMEDAYS].item_b_pad_char, sizeof ascii_string);	/* Clear out and pad */
    AUTHORIZE$GET_LIST_ENTRIES (uaf$c_entry_primeday_list,
				ascii_string,
				sizeof ascii_string);

/* In the primary day mask, the primary days are 0 and the secondary days are
** 1.  This means the resulting bit stream must be complemented prior to adding
** it to the item list (for the authorization entry). */

    COMPLEMENT (BYTE, ascii_string);
    AUTHORIZE$ITMLST_ADD_ITEM (itmlst,
			       UAI$_PRIMEDAYS,
			       BYTE,
			       ascii_string);
    }

/* Authorized privileges. */

if (uaf_r_flags.uaf_v_mod_auth_privs)
    {
    memset (ascii_string, uaf_r_item_table[UAI$_PRIV].item_b_pad_char, sizeof ascii_string);	/* Clear out and pad */
    AUTHORIZE$GET_LIST_ENTRIES (uaf$c_entry_auth_priv_list,
				ascii_string,
				sizeof ascii_string);
    AUTHORIZE$ITMLST_ADD_ITEM (itmlst,
			       UAI$_PRIV,
			       QUADWORD,
			       ascii_string);
    }

/* Access restrictions. */

if (uaf_r_flags.uaf_v_mod_pri_local_access)
    {
    memset (ascii_string, uaf_r_item_table[UAI$_LOCAL_ACCESS_P].item_b_pad_char, sizeof ascii_string);	/* Clear out and pad */
    AUTHORIZE$GET_LIST_ENTRIES (uaf$c_access_pri_local_list,
				ascii_string,
				sizeof ascii_string);

/* In the local access mask, access is granted if the hour bit is clear and denied
** if the hour bit is set.  In other words, a bit stream of all zeros indicates
** full access.  This means the resulting bit stream must be complemented prior
** to adding it to the item list (for the authorization entry). */

    COMPLEMENT (ACCESS_SIZE, ascii_string);
    AUTHORIZE$ITMLST_ADD_ITEM (itmlst,
			       UAI$_LOCAL_ACCESS_P,
			       ACCESS_SIZE,
			       ascii_string);
    }

if (uaf_r_flags.uaf_v_mod_sec_local_access)
    {
    memset (ascii_string, uaf_r_item_table[UAI$_LOCAL_ACCESS_S].item_b_pad_char, sizeof ascii_string);	/* Clear out and pad */
    AUTHORIZE$GET_LIST_ENTRIES (uaf$c_access_sec_local_list,
				ascii_string,
				sizeof ascii_string);
    COMPLEMENT (ACCESS_SIZE, ascii_string);			/* See comments on above call */
    AUTHORIZE$ITMLST_ADD_ITEM (itmlst,
			       UAI$_LOCAL_ACCESS_S,
			       ACCESS_SIZE,
			       ascii_string);
    }

if (uaf_r_flags.uaf_v_mod_pri_batch_access)
    {
    memset (ascii_string, uaf_r_item_table[UAI$_BATCH_ACCESS_P].item_b_pad_char, sizeof ascii_string);	/* Clear out and pad */
    AUTHORIZE$GET_LIST_ENTRIES (uaf$c_access_pri_batch_list,
				ascii_string,
				sizeof ascii_string);
    COMPLEMENT (ACCESS_SIZE, ascii_string);			/* See comments on above call */
    AUTHORIZE$ITMLST_ADD_ITEM (itmlst,
			       UAI$_BATCH_ACCESS_P,
			       ACCESS_SIZE,
			       ascii_string);
    }

if (uaf_r_flags.uaf_v_mod_sec_batch_access)
    {
    memset (ascii_string, uaf_r_item_table[UAI$_BATCH_ACCESS_S].item_b_pad_char, sizeof ascii_string);	/* Clear out and pad */
    AUTHORIZE$GET_LIST_ENTRIES (uaf$c_access_sec_batch_list,
				ascii_string,
				sizeof ascii_string);
    COMPLEMENT (ACCESS_SIZE, ascii_string);			/* See comments on above call */
    AUTHORIZE$ITMLST_ADD_ITEM (itmlst,
			       UAI$_BATCH_ACCESS_S,
			       ACCESS_SIZE,
			       ascii_string);
    }

if (uaf_r_flags.uaf_v_mod_pri_dialup_access)
    {
    memset (ascii_string, uaf_r_item_table[UAI$_DIALUP_ACCESS_P].item_b_pad_char, sizeof ascii_string);	/* Clear out and pad */
    AUTHORIZE$GET_LIST_ENTRIES (uaf$c_access_pri_dialup_list,
				ascii_string,
				sizeof ascii_string);
    COMPLEMENT (ACCESS_SIZE, ascii_string);			/* See comments on above call */
    AUTHORIZE$ITMLST_ADD_ITEM (itmlst,
			       UAI$_DIALUP_ACCESS_P,
			       ACCESS_SIZE,
			       ascii_string);
    }

if (uaf_r_flags.uaf_v_mod_sec_dialup_access)
    {
    memset (ascii_string, uaf_r_item_table[UAI$_DIALUP_ACCESS_S].item_b_pad_char, sizeof ascii_string);	/* Clear out and pad */
    AUTHORIZE$GET_LIST_ENTRIES (uaf$c_access_sec_dialup_list,
				ascii_string,
				sizeof ascii_string);
    COMPLEMENT (ACCESS_SIZE, ascii_string);			/* See comments on above call */
    AUTHORIZE$ITMLST_ADD_ITEM (itmlst,
			       UAI$_DIALUP_ACCESS_S,
			       ACCESS_SIZE,
			       ascii_string);
    }

if (uaf_r_flags.uaf_v_mod_pri_remote_access)
    {
    memset (ascii_string, uaf_r_item_table[UAI$_REMOTE_ACCESS_P].item_b_pad_char, sizeof ascii_string);	/* Clear out and pad */
    AUTHORIZE$GET_LIST_ENTRIES (uaf$c_access_pri_remote_list,
				ascii_string,
				sizeof ascii_string);
    COMPLEMENT (ACCESS_SIZE, ascii_string);			/* See comments on above call */
    AUTHORIZE$ITMLST_ADD_ITEM (itmlst,
			       UAI$_REMOTE_ACCESS_P,
			       ACCESS_SIZE,
			       ascii_string);
    }

if (uaf_r_flags.uaf_v_mod_sec_remote_access)
    {
    memset (ascii_string, uaf_r_item_table[UAI$_REMOTE_ACCESS_S].item_b_pad_char, sizeof ascii_string);	/* Clear out and pad */
    AUTHORIZE$GET_LIST_ENTRIES (uaf$c_access_sec_remote_list,
				ascii_string,
				sizeof ascii_string);
    COMPLEMENT (ACCESS_SIZE, ascii_string);			/* See comments on above call */
    AUTHORIZE$ITMLST_ADD_ITEM (itmlst,
			       UAI$_REMOTE_ACCESS_S,
			       ACCESS_SIZE,
			       ascii_string);
    }

if (uaf_r_flags.uaf_v_mod_pri_network_access)
    {
    memset (ascii_string, uaf_r_item_table[UAI$_NETWORK_ACCESS_P].item_b_pad_char, sizeof ascii_string);	/* Clear out and pad */
    AUTHORIZE$GET_LIST_ENTRIES (uaf$c_access_pri_network_list,
				ascii_string,
				sizeof ascii_string);
    COMPLEMENT (ACCESS_SIZE, ascii_string);			/* See comments on above call */
    AUTHORIZE$ITMLST_ADD_ITEM (itmlst,
			       UAI$_NETWORK_ACCESS_P,
			       ACCESS_SIZE,
			       ascii_string);
    }

if (uaf_r_flags.uaf_v_mod_sec_network_access)
    {
    memset (ascii_string, uaf_r_item_table[UAI$_NETWORK_ACCESS_S].item_b_pad_char, sizeof ascii_string);	/* Clear out and pad */
    AUTHORIZE$GET_LIST_ENTRIES (uaf$c_access_sec_network_list,
				ascii_string,
				sizeof ascii_string);
    COMPLEMENT (ACCESS_SIZE, ascii_string);			/* See comments on above call */
    AUTHORIZE$ITMLST_ADD_ITEM (itmlst,
			       UAI$_NETWORK_ACCESS_S,
			       ACCESS_SIZE,
			       ascii_string);
    }

/* Primary password. */

if (uaf_r_widget_id_array[uaf$c_entry_password] != 0)
    {

/* Get the password string. */

    temp_string = XmTextGetString (uaf_r_widget_id_array[uaf$c_entry_password]);
    temp_string_length = strlen (temp_string);

    if (temp_string_length != 0)
	{

/* If a password string was specified, create the appropriate item list entry. */

	AUTHORIZE$ITMLST_ADD_ITEM (itmlst,
				   UAI$_PASSWORD,
				   temp_string_length,
				   temp_string);

/* Since the password should not remain visible, clear out the field. */

	XmTextSetString (uaf_r_widget_id_array[uaf$c_entry_password],
			 &NULL);
	}

/* Free up any storage allocated for the text string. */

    XtFree (temp_string);
    }

/* Secondary password. */

if (uaf_r_widget_id_array[uaf$c_entry_password2] != 0)
    {

/* Get the password string. */

    temp_string = XmTextGetString (uaf_r_widget_id_array[uaf$c_entry_password2]);
    temp_string_length = strlen (temp_string);

    if (temp_string_length != 0)
	{

/* If a password string was specified, create the appropriate item list entry. */

	AUTHORIZE$ITMLST_ADD_ITEM (itmlst,
				   UAI$_PASSWORD2,
				   temp_string_length,
				   temp_string);

/* Since the password should not remain visible, clear out the field. */

	XmTextSetString (uaf_r_widget_id_array[uaf$c_entry_password],
			 &NULL);
	}

/* Free up any storage allocated for the text string. */

    XtFree (temp_string);
    }

/* The item list is complete.  Exit stage right. */

return SS$_NORMAL;
}

extern void AUTHORIZE$SET_UP_ENTRY (itmlst)

struct ITMDEF			(*itmlst)[];
{
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine is called to set up the information in the authorization
**	entry window.  The information is obtained from the supplied item list.
**
**  FORMAL PARAMETERS:
**
**	ITMLST		- Address of the item list
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

/* External routines. */

extern void			AUTHORIZE$SELECT_LIST_ENTRIES ();
extern void			AUTHORIZE$SET_UP_ACCESS ();

/* Global references. */

globalref Widget		uaf_r_access_pri_window_widget;	/* Primary access restriction window widget info */
globalref Widget		uaf_r_access_sec_window_widget;	/* Secondary access restriction window widget info */
globalref struct UAFLAG		uaf_r_flags;			/* Miscellaneous utility flags */
globalref struct FMTABL		uaf_r_item_table[];		/* Entry window info format table */
globalref Widget		uaf_r_widget_id_array[uaf$c_max_widget_code];	/* Array of Widget IDs in use */
globalref char			*uaf_t_no_date_time;		/* String to use when no date/time present */
globalref char			*uaf_t_pre_expired;		/* String to use when no pwd chg dat/time present */

/* Local storage. */

unsigned int			date_time[2];			/* Temp date time value */
char				default_dev[MAX_DEFDEV] = "";	/* Default device string */
char				default_dir[MAX_DEFDIR] = "";	/* Default directory string */
int				index;				/* General index */
int				index2;				/* Second general index */
unsigned char			*item_bufadr;			/* Item buffer address from item list entry */
unsigned int			item_bufsiz;			/* Item buffer size from item list entry */
unsigned int			item_code;			/* Item code from item list entry */
unsigned int			simple_desc[2];			/* Simple character descriptor */
unsigned int			status;				/* Routine exit status */
char				temp_string[MAX_TEMP_STRING];	/* Temporary string storage */
int				temp_string_length;		/* Length of temp string (built) */
int				widget_number;			/* Widget number for item list entry */

/* Debug information. */

#ifdef DEBUGGING
printf ("set up entry\n");
#endif /* DEBUGGING */

/* Before setting up the new information in the authorization entry window,
** clear out any stale information. */

for (index = 0; index < UAI$_MAX_ITEM_CODE; index++)
    {

/* If there is a widget-id for the item table entry, reset the associated
** text string. */

    if ((uaf_r_item_table[index].item_l_widget != 0) &&
	(uaf_r_widget_id_array[uaf_r_item_table[index].item_l_widget] != 0))
	XmTextSetString (uaf_r_widget_id_array[uaf_r_item_table[index].item_l_widget],
			 &NULL);
    }

/* Reset the default device & directory string. */

XmTextSetString (uaf_r_widget_id_array[uaf$c_entry_default],
		 &NULL);

/* Reset the various lists.  This is done by deselecting all the list entries. */

XmListDeselectAllItems (uaf_r_widget_id_array[uaf$c_entry_def_priv_list]);
XmListDeselectAllItems (uaf_r_widget_id_array[uaf$c_entry_auth_priv_list]);
XmListDeselectAllItems (uaf_r_widget_id_array[uaf$c_entry_login_flag_list]);
XmListDeselectAllItems (uaf_r_widget_id_array[uaf$c_entry_primeday_list]);

XmListDeselectAllItems (uaf_r_widget_id_array[uaf$c_access_pri_local_list]);
XmListDeselectAllItems (uaf_r_widget_id_array[uaf$c_access_pri_batch_list]);
XmListDeselectAllItems (uaf_r_widget_id_array[uaf$c_access_pri_dialup_list]);
XmListDeselectAllItems (uaf_r_widget_id_array[uaf$c_access_pri_remote_list]);
XmListDeselectAllItems (uaf_r_widget_id_array[uaf$c_access_pri_network_list]);

XmListDeselectAllItems (uaf_r_widget_id_array[uaf$c_access_sec_local_list]);
XmListDeselectAllItems (uaf_r_widget_id_array[uaf$c_access_sec_batch_list]);
XmListDeselectAllItems (uaf_r_widget_id_array[uaf$c_access_sec_dialup_list]);
XmListDeselectAllItems (uaf_r_widget_id_array[uaf$c_access_sec_remote_list]);
XmListDeselectAllItems (uaf_r_widget_id_array[uaf$c_access_sec_network_list]);

/* If the item list pointer is zero, trip out now. */

if (itmlst == 0) return;

/* Loop through the item list entries and set the appropriate information in
** the authorization entry window. */

for (index = 0; (*itmlst)[index].itm$w_itmcod != 0; index++)
    {
   
/* Get the information from the current item list entry. */

    item_code = (*itmlst)[index].itm$w_itmcod;
    item_bufsiz = (*itmlst)[index].itm$w_bufsiz;
    item_bufadr = (*itmlst)[index].itm$l_bufadr;

/* Depending on the type of information (i.e., value, counted string, etc.)
** do some limit checking.  There are two possible cases: too much information
** supplied, and not enough information supplied.  The former case would most
** likely occur with some form of ASCII text and the latter would most likely
** occur with some form of value.
**
** If the limit check fails, return an SS$_BADPARAM error.  This also indicates
** the uaf_r_item_table probably needs to be updated; i.e., an internal error. */

    if ((uaf_r_item_table[item_code].item_r_flag_overlay.item_r_flag_bits.item_v_flag_cstr) ||
	(uaf_r_item_table[item_code].item_r_flag_overlay.item_r_flag_bits.item_v_flag_asc))
	{
	if (item_bufsiz > uaf_r_item_table[item_code].item_w_byte_cnt) LIB$SIGNAL (UAF$_INTERNAL1);
	}
    else
	{
	if (item_bufsiz < uaf_r_item_table[item_code].item_w_byte_cnt) LIB$SIGNAL (UAF$_INTERNAL1);
	}

/* Get the widget number corresponding to the widget associated with the information.
** This may be zero, which usually indicates special processing is needed. */

    widget_number = uaf_r_item_table[item_code].item_l_widget;

/* Clear out any previous buffer contents.  This also assumes the typical C
** character string storage is terminated by a null (\0) character. */

    memset (temp_string, NULL, sizeof temp_string);

/* Initialize a simple character descriptor for the temp string buffer. */

    simple_desc[0] = (sizeof temp_string) - 1;		/* -1 for ASCIZ string */
    simple_desc[1] = temp_string;

/* Determine how the information in the item list entry is to be formatted.
** This is done by examining the flags associated with the format table entry. */

/* A counted ASCII string.
**
** For this case, the first byte of the buffer contains the number of
** ASCII characters to copy.
**
*/

    if (uaf_r_item_table[item_code].item_r_flag_overlay.item_r_flag_bits.item_v_flag_cstr)
	{

/* Make sure the temp storage area is not exceeded when copying the text. */

	temp_string_length = item_bufadr[0];			/* Assume it will all fit (-1 for count) */

/* If the size of the supplied buffer is greater than the internal storage,
** truncate the supplied information to fit. */

	if (temp_string_length > sizeof temp_string)
	    temp_string_length = (sizeof temp_string) - 1;

/* Copy the appropriate amount of text, minus the count byte. */

	memmove (temp_string, &item_bufadr[1], temp_string_length);
	}

/* A simple ASCII string. */

    if (uaf_r_item_table[item_code].item_r_flag_overlay.item_r_flag_bits.item_v_flag_asc)
	{

/* Copy the string up to the first occurrance of the pad character, the
** input string is exhausted, or the target buffer is exceeded. */

	for (temp_string_length = 0; temp_string_length < (sizeof temp_string) - 1; temp_string_length++)
	    {
	    if (temp_string_length == item_bufsiz) break;
	    if (item_bufadr[temp_string_length] == uaf_r_item_table[item_code].item_b_pad_char) break;
	    temp_string[temp_string_length] = item_bufadr[temp_string_length];
	    }
	}

/* A value to be converted to a string. */

    if (uaf_r_item_table[item_code].item_r_flag_overlay.item_r_flag_bits.item_v_flag_val)
	{

/* Format the value.  Because the buffer is zeroed (above) prior to the FAO call,
** the formatting results in an ASCIZ string. */

	SYS$FAO (&uaf_r_item_table[item_code].item_l_fao_ctrs_len,
		 simple_desc,
		 simple_desc,
		 (*(int (*)[])item_bufadr)[0]);
	}

/* Format a date/time quadword.  This may be an absolute or delta time. */

    if ((uaf_r_item_table[item_code].item_r_flag_overlay.item_r_flag_bits.item_v_flag_dat) &&
	(item_bufsiz >= 8))	/* Must be at least a quadword...otherwise skip */
	{

/* If the quadword time value is zero, substitute a reasonable string instead of
** the usual "1858" date :-).  Otherwise, format the time.*/

	if ((*(int (*)[])item_bufadr)[0] == 0 && (*(int (*)[])item_bufadr)[1] == 0)
	    {

/* If the zero time value is for a password change date (primary or secondary)
** change the string used to "(pre-expired)" rather than "(none)". */

	    if ((item_code == UAI$_PWD_DATE) ||
		(item_code == UAI$_PWD2_DATE))
		strcpy (temp_string, uaf_t_pre_expired);
	    else
		strcpy (temp_string, uaf_t_no_date_time);
	    }
	else
	    SYS$ASCTIM (simple_desc,
			simple_desc,
			item_bufadr,
			NULL);
	}

/* Handle those items which require special formatting; but nothing more in the
** way of additional processing. */

    if (uaf_r_item_table[item_code].item_r_flag_overlay.item_r_flag_bits.item_v_flag_spc)
	{
	if (item_code == UAI$_CPUTIM)
	    {

/* Format the CPU time limit.  This requires special processing because the CPU
** time limit is stored as the low longword of a delta time.  In order to format
** it as a real delta time, set the date/time quadword to -1 before adding in the
** supplied buffer information (the remaining longword). */

	    memset (date_time, 0xFF, sizeof date_time);
	    date_time[0] = (*(int (*)[])item_bufadr)[0];

/* If the CPU time limit is zero, substitute a reasonable string instead of some
** random time string. */

	    if (date_time[0] == 0)
		strcpy (temp_string, uaf_t_no_date_time);
	    else
		SYS$ASCTIM (simple_desc,
			    simple_desc,
			    date_time,
			    NULL);
	    }
	if (item_code == UAI$_UIC)
	    {

/* Format the UIC.  This is done as a special case to allow the item table to be
** used for input as well as output.  Because the buffer is zeroed (above) prior
** to the FAO call, the formatting results in an ASCIZ string. */

	    SYS$FAO (&uaf_r_item_table[item_code].item_l_fao_ctrs_len,
		     simple_desc,
		     simple_desc,
		     (*(int (*)[])item_bufadr)[0]);
	    }
	}

/* If there is a widget number associated with the UAI item code, simply set the
** string in the entry window. */

    if (uaf_r_widget_id_array[widget_number] != 0) XmTextSetString (uaf_r_widget_id_array[widget_number], temp_string);

/* There are several authorization items that require special processing.  Mostly
** because they cannot be represented as simple values or strings.  See if any are
** present and do the appropriate processing. */

/* If a default privilege entry is seen, set the state of all the various default
** privilege list entries. */

    if (item_code == UAI$_DEF_PRIV)
	AUTHORIZE$SELECT_LIST_ENTRIES (uaf$c_entry_def_priv_list,
				       item_bufadr,
				       item_bufsiz);

/* If a default device entry is seen, copy the device name for later. */

    if (item_code == UAI$_DEFDEV) strcpy (default_dev, temp_string);

/* If a default directory entry is seen, copy the directory name for later. */

    if (item_code == UAI$_DEFDIR) strcpy (default_dir, temp_string);

/* If a login flag entry is seen, set the state of all the various login flag
** list entries. */

    if (item_code == UAI$_FLAGS)
	AUTHORIZE$SELECT_LIST_ENTRIES (uaf$c_entry_login_flag_list,
				       item_bufadr,
				       item_bufsiz);

/* If a primary day entry is seen, set the state of all the various primary day
** list entries. */

    if (item_code == UAI$_PRIMEDAYS)
	{

/* In the primary day mask, the primary days are 0 and the secondary days are
** 1.  This means the resulting bit stream must be complemented prior to selecting
** the items in the list. */

	COMPLEMENT (item_bufsiz, item_bufadr);
	AUTHORIZE$SELECT_LIST_ENTRIES (uaf$c_entry_primeday_list,
				       item_bufadr,
				       item_bufsiz);
	}

/* If an authorized privilege entry is seen, set the state of all the various
** authorized privilege list entries. */

    if (item_code == UAI$_PRIV)
	AUTHORIZE$SELECT_LIST_ENTRIES (uaf$c_entry_auth_priv_list,
				       item_bufadr,
				       item_bufsiz);
    }

/* Since the default device and default directory must be set up in the entry
** window in a particular order, the display of this information has been
** deferred until now.
**
** If there is either a default device or a default directory, display the
** information at this time. */

temp_string[0] = '\0';		/* Null string to start with */

if (strlen (default_dev) > 0) strcat (temp_string, default_dev);
if (strlen (default_dir) > 0) strcat (temp_string, default_dir);

if ((strlen (temp_string) > 0) && (uaf_r_widget_id_array[uaf$c_entry_default] != 0))
    XmTextSetString (uaf_r_widget_id_array[uaf$c_entry_default],
		     temp_string);

/* If the primary access restriction window is managed, set up the lists. */

if (XtIsManaged (uaf_r_access_pri_window_widget))
    AUTHORIZE$SET_UP_ACCESS (itmlst,
			     1);

/* If the secondary access restriction window is managed, set up the lists. */

if (XtIsManaged (uaf_r_access_sec_window_widget))
    AUTHORIZE$SET_UP_ACCESS (itmlst,
			     2);

/* Note no list modifications performed as yet.  This must be done after
** the list have been set up by AUTHORIZE$SELECT_LIST_ENTRIES (otherwise
** AUTHORIZE$SELECT_LIST_ENTRIES would cause the list modification flag
** to get set. */

uaf_r_flags.uaf_v_mod_def_privs = 0;
uaf_r_flags.uaf_v_mod_login_flags = 0;
uaf_r_flags.uaf_v_mod_prime_days = 0;
uaf_r_flags.uaf_v_mod_auth_privs = 0;
uaf_r_flags.uaf_v_mod_pri_local_access = 0;
uaf_r_flags.uaf_v_mod_pri_batch_access = 0;
uaf_r_flags.uaf_v_mod_pri_dialup_access = 0;
uaf_r_flags.uaf_v_mod_pri_remote_access = 0;
uaf_r_flags.uaf_v_mod_pri_network_access = 0;
uaf_r_flags.uaf_v_mod_sec_local_access = 0;
uaf_r_flags.uaf_v_mod_sec_batch_access = 0;
uaf_r_flags.uaf_v_mod_sec_dialup_access = 0;
uaf_r_flags.uaf_v_mod_sec_remote_access = 0;
uaf_r_flags.uaf_v_mod_sec_network_access = 0;
}
