#module UAF_DW_ACCESS "X-1"

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
**	X-1	LMP		L. Mark Pilant,		15-Jun-1993
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

#pragma noinline (AUTHORIZE$ACCESS_RESTRICTIONS)
#pragma noinline (AUTHORIZE$SET_UP_ACCESS)

extern void AUTHORIZE$ACCESS_RESTRICTIONS (widget_id, tag, reason)

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
**	{@description or none@}
**
**  SIDE EFFECTS:
**
**	{@description or none@}
**
**--
*/

/* External routines. */

extern unsigned int		AUTHORIZE$GET_USER_INFO ();

/* Global references. */

globalref unsigned int		uaf_l_current_user_index;	/* Index of current user */
globalref unsigned int		uaf_l_current_user_max;		/* Count of entries in user list */
globalref Widget		uaf_r_access_pri_window_widget;	/* Primary access restriction window widget info */
globalref Widget		uaf_r_access_sec_window_widget;	/* Secondary access restriction window widget info */
globalref struct ITMDEF		(*uaf_r_current_user_info)[];	/* Current user UAF info */
globalref Widget		uaf_r_entry_window_widget;	/* Entry window widget info */
globalref struct UAFLAG		uaf_r_flags;			/* Miscellaneous utility flags */
globalref struct ITMDEF		(*uaf_r_template_info)[];	/* Modification template information */
globalref char			uaf_t_current_username[];	/* Current username */

/* Local storage. */

int				index;				/* General index */
int				widget_number = *tag;

/* Debug information. */

#ifdef DEBUGGING
printf ("access restrictions\n");
#endif /* DEBUGGING */

/* Determine what to do based upon the user actions. */

switch (widget_number)
    {
    case uaf$c_entry_access_primary:
	{

/* Make the primary access restriction window visible. */

	XtManageChild (uaf_r_access_pri_window_widget);
	break;
	}

    case uaf$c_entry_access_secondary:
	{

/* Make the secondary access restriction window visible. */

	XtManageChild (uaf_r_access_sec_window_widget);
	break;
	}

    case uaf$c_access_pri_ok:
	{

/* Dismiss the primary access restriction window. */

	XtUnmanageChild (uaf_r_access_pri_window_widget);

/* Note the state of the access list modification flags remain unchanged.  Thus,
** if no changes were made, an entry is not added to the item list. */

	break;
	}
    case uaf$c_access_pri_cancel:
	{

/* Dismiss the primary access restriction window. */

	XtUnmanageChild (uaf_r_access_pri_window_widget);

/* Reset all the access list modification flags to prevent the values from being
** changed by an item list entry. */

	uaf_r_flags.uaf_v_mod_pri_local_access = 0;
	uaf_r_flags.uaf_v_mod_pri_batch_access = 0;
	uaf_r_flags.uaf_v_mod_pri_dialup_access = 0;
	uaf_r_flags.uaf_v_mod_pri_remote_access = 0;
	uaf_r_flags.uaf_v_mod_pri_network_access = 0;
	break;
	}

    case uaf$c_access_sec_ok:
	{

/* Dismiss the secondary access restriction window. */

	XtUnmanageChild (uaf_r_access_sec_window_widget);

/* Note the state of the access list modification flags remain unchanged.  Thus,
** if no changes were made, an entry is not added to the item list. */

	break;
	}
    case uaf$c_access_sec_cancel:
	{

/* Dismiss the secondary access restriction window. */

	XtUnmanageChild (uaf_r_access_sec_window_widget);

/* Reset all the access list modification flags to prevent the values from being
** changed by an item list entry. */

	uaf_r_flags.uaf_v_mod_sec_local_access = 0;
	uaf_r_flags.uaf_v_mod_sec_batch_access = 0;
	uaf_r_flags.uaf_v_mod_sec_dialup_access = 0;
	uaf_r_flags.uaf_v_mod_sec_remote_access = 0;
	uaf_r_flags.uaf_v_mod_sec_network_access = 0;
	break;
	}
    }
}

extern void AUTHORIZE$SET_UP_ACCESS (itmlst, window_code)

struct ITMDEF			(*itmlst)[];
int				window_code;
{
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine is called to set up the information in the various
**	access restriction lists.
**
**  FORMAL PARAMETERS:
**
**	ITMLST		- Pointer to the dynamic item list
**	WINDOW_CODE	- = 1 to affect primary access window,
**			  = 2 to affect secondary access window
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

/* Global references. */

globalref Widget		uaf_r_access_pri_window_widget;	/* Primary access restriction window widget info */
globalref Widget		uaf_r_access_sec_window_widget;	/* Secondary access restriction window widget info */

/* Local storage. */

int				index;				/* General index */
unsigned char			*item_bufadr;			/* Item buffer address from item list entry */
unsigned int			item_bufsiz;			/* Item buffer size from item list entry */
unsigned int			item_code;			/* Item code from item list entry */

/* Debug information. */

#ifdef DEBUGGING
printf ("set up access\n");
#endif /* DEBUGGING */

/* Loop through the item list entries and set the appropriate information in
** the authorization entry window. */

for (index = 0; (*itmlst)[index].itm$w_itmcod != 0; index++)
    {
   
/* Get the information from the current item list entry. */

    item_code = (*itmlst)[index].itm$w_itmcod;
    item_bufsiz = (*itmlst)[index].itm$w_bufsiz;
    item_bufadr = (*itmlst)[index].itm$l_bufadr;

/* Check for various access restriction entries.  If seen, set the state of all
** the various local access list entries.  First check the item codes affecting
** the primary access restriction window, then check the item codes affecting
** the secondary access restriction window. */

    if (window_code == 1)			/* Primary window */
	{
	if (item_code == UAI$_LOCAL_ACCESS_P)
	    {

/* In the local access mask, access is granted if the hour bit is clear and denied
** if the hour bit is set.  In other words, a bit stream of all zeros indicates
** full access.  This means the resulting bit stream must be complemented prior
** to selecting the items in the list. */

	    COMPLEMENT (item_bufsiz, item_bufadr);
	    AUTHORIZE$SELECT_LIST_ENTRIES (uaf$c_access_pri_local_list,
					   item_bufadr,
					   item_bufsiz);
	    }
	if (item_code == UAI$_BATCH_ACCESS_P)
	    {
	    COMPLEMENT (item_bufsiz, item_bufadr);			/* See comments on above call */
	    AUTHORIZE$SELECT_LIST_ENTRIES (uaf$c_access_pri_batch_list,
					   item_bufadr,
					   item_bufsiz);
	    }
	if (item_code == UAI$_DIALUP_ACCESS_P)
	    {
	    COMPLEMENT (item_bufsiz, item_bufadr);			/* See comments on above call */
	    AUTHORIZE$SELECT_LIST_ENTRIES (uaf$c_access_pri_dialup_list,
					   item_bufadr,
					   item_bufsiz);
	    }
	if (item_code == UAI$_REMOTE_ACCESS_P)
	    {
	    COMPLEMENT (item_bufsiz, item_bufadr);			/* See comments on above call */
	    AUTHORIZE$SELECT_LIST_ENTRIES (uaf$c_access_pri_remote_list,
					   item_bufadr,
					   item_bufsiz);
	    }
	if (item_code == UAI$_NETWORK_ACCESS_P)
	    {
	    COMPLEMENT (item_bufsiz, item_bufadr);			/* See comments on above call */
	    AUTHORIZE$SELECT_LIST_ENTRIES (uaf$c_access_pri_network_list,
					   item_bufadr,
					   item_bufsiz);
	    }
	}

    if (window_code == 1)			/* Secondary window */
	{
	if (item_code == UAI$_LOCAL_ACCESS_S)
	    {

/* In the local access mask, access is granted if the hour bit is clear and denied
** if the hour bit is set.  In other words, a bit stream of all zeros indicates
** full access.  This means the resulting bit stream must be complemented prior
** to selecting the items in the list. */

	    COMPLEMENT (item_bufsiz, item_bufadr);			/* See comments on above call */
	    AUTHORIZE$SELECT_LIST_ENTRIES (uaf$c_access_sec_local_list,
					   item_bufadr,
					   item_bufsiz);
	    }
	if (item_code == UAI$_BATCH_ACCESS_S)
	    {
	    COMPLEMENT (item_bufsiz, item_bufadr);			/* See comments on above call */
	    AUTHORIZE$SELECT_LIST_ENTRIES (uaf$c_access_sec_batch_list,
					   item_bufadr,
					   item_bufsiz);
	    }
	if (item_code == UAI$_DIALUP_ACCESS_S)
	    {
	    COMPLEMENT (item_bufsiz, item_bufadr);			/* See comments on above call */
	    AUTHORIZE$SELECT_LIST_ENTRIES (uaf$c_access_sec_dialup_list,
					   item_bufadr,
					   item_bufsiz);
	    }
	if (item_code == UAI$_REMOTE_ACCESS_S)
	    {
	    COMPLEMENT (item_bufsiz, item_bufadr);			/* See comments on above call */
	    AUTHORIZE$SELECT_LIST_ENTRIES (uaf$c_access_sec_remote_list,
					   item_bufadr,
					   item_bufsiz);
	    }
	if (item_code == UAI$_NETWORK_ACCESS_S)
	    {
	    COMPLEMENT (item_bufsiz, item_bufadr);			/* See comments on above call */
	    AUTHORIZE$SELECT_LIST_ENTRIES (uaf$c_access_sec_network_list,
					   item_bufadr,
					   item_bufsiz);
	    }
	}
    }
}
