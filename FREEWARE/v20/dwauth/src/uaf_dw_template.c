#module UAF_DW_TEMPLATE "X-1"

/*
**++
**  FACILITY:	Authorization record maintenance utility
**
**  MODULE DESCRIPTION:
**
**		This module contains the routines for the managing
**		the modification template use by the DECwindows
**		AUTHORIZE utility.
**
**  AUTHORS:	L. Mark Pilant		CREATION DATE:  20-Apr-1993
**
**  MODIFICATION HISTORY:
**
**	X-1	LMP		L. Mark Pilant,		20-Apr-1993
**		Original version.
**
**--
*/

/*
**
**  INCLUDE FILES
**
*/

#include	string

#include	<decw$include:mrmappl>

#include	"uaf_header"

/*
**
** FORWARD ROUTINES.
**
*/

#pragma noinline (AUTHORIZE$MODIFY_TEMPLATE)

extern void AUTHORIZE$MODIFY_TEMPLATE (widget_id, tag, reason)

Widget				widget_id;
int				*tag;
XmAnyCallbackStruct		*reason;
{
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine is called to set up the template window for modifications
**	and later use by the DECwindows authorization utility.
**
**  FORMAL PARAMETERS:
**
**	None.
**
**  RETURN VALUE:
**
**	None.
**
**  SIDE EFFECTS:
**
**	None.
**
**--
*/

/* External routines. */

extern void		AUTHORIZE$SET_UP_ENTRY ();

/* Global references. */

globalref struct ITMDEF	(*uaf_r_template_info)[];	/* Modification template information */
globalref Widget	uaf_r_entry_window_widget;	/* Entry window widget info */

/* Local storage. */

unsigned int		status;				/* Routine exit status */

/* Debug information. */

#ifdef DEBUGGING
printf ("modify template\n");
#endif /* DEBUGGING */

/* Because the template information comes from the standard authorization
** entry window, check to see if the authorization entry window is currently
** managed.  If it is, query to see if this operation should continue. */

if (XtIsManaged (uaf_r_entry_window_widget))
    {
    XtUnmanageChild (uaf_r_entry_window_widget);
    }

/* Set up the authorization entry window using the template information. */

AUTHORIZE$SET_UP_ENTRY (uaf_r_template_info);

/* Make the entry window visible. */

XtManageChild (uaf_r_entry_window_widget);
}
