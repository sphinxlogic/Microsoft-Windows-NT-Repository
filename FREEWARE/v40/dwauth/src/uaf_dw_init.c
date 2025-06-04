#module UAF_DW_INIT "X-1"

/*
**++
**  FACILITY:	Authorization record maintenance utility
**
**  MODULE DESCRIPTION:
**
**		This module contains the main processing routine  for the
**		DECwindows AUTHORIZE utility.
**
**  AUTHORS:	L. Mark Pilant		CREATION DATE:  25-Nov-1992
**
**  MODIFICATION HISTORY:
**
**	X-1	LMP		L. Mark Pilant,		6-Apr-1993
**		Original version.
**
**--
*/

/*
**
**  INCLUDE FILES
**
*/

#include	<JPIDEF>

#include	string

#include	<decw$include:mrmappl>
#include	<decw$include:DECspecific>
#include	<DECw$cursor>

#include	"uaf_header"

/*
**
** FORWARD ROUTINES.
**
*/

#pragma noinline (AUTHORIZE$DECW_INIT)
#pragma noinline (AUTHORIZE$DECW_MAIN_LOOP)

extern unsigned int AUTHORIZE$DECW_INIT ()

{
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine initializes the DECwindows environment for the
**	DECwindows authorization utility.
**
**  FORMAL PARAMETERS:
**
**	None.
**
**  RETURN VALUE:
**
**	SS$_NORMAL if successful, error code otherwise.
**
**  SIDE EFFECTS:
**
**	None.
**
**--
*/

/* Required constants. */

#define	ARG_COUNT	1				/* Number of X arguments in list */

/* External routines. */

extern void		AUTHORIZE$DECW_ACTIVATE_PROC ();
extern void		AUTHORIZE$DECW_BUTTON_PROC ();
extern void		AUTHORIZE$DECW_CREATE_PROC ();
extern void		AUTHORIZE$DECW_LIST_PROC ();
extern void		AUTHORIZE$DECW_PULL_PROC ();
extern void		AUTHORIZE$DECW_TOGGLE_PROC ();

/* Global references. */

globalref Widget	uaf_r_access_pri_window_widget;	/* Primary access restriction window widget info */
globalref Widget	uaf_r_access_sec_window_widget;	/* Secondary access restriction window widget info */
globalref XtAppContext	uaf_r_app_context;		/* Application context */
globalref Display	*uaf_r_entry_display;		/* Display id for authorization entry window */
globalref Widget	uaf_r_entry_popup_menu_widget;	/* Entry window popup menu */
globalref Widget	uaf_r_entry_top_level_widget;	/* Entry window widget shell info */
globalref Widget	uaf_r_entry_window_widget;	/* Entry window widget info */
globalref Display	*uaf_r_file_display;		/* Display id for file selection window */
globalref Widget	uaf_r_file_top_level_widget;	/* File selction widget shell info */
globalref Widget	uaf_r_file_window_widget;	/* File selection widget info */
globalref Widget	uaf_r_id_popup_menu_widget;	/* Identifier window popup menu */
globalref Widget	uaf_r_identifier_window_widget;	/* Identifier window widget info */
globalref Display	*uaf_r_main_display;		/* Display id for main window */
globalref Widget	uaf_r_main_top_level_widget;	/* Main window widget shell info */
globalref Widget	uaf_r_main_window_widget;	/* Main window widget info */
globalref MrmHierarchy	uaf_r_mrm_hierarchy;		/* Hierarchy info for .UID file(s) */
globalref Widget	uaf_r_system_id_window_widget;	/* System identifier window widget info */
globalref Cursor	uaf_r_wait_cursor_entry;	/* Entry window wait cursor id */
globalref Cursor	uaf_r_wait_cursor_main;		/* Main window wait cursor id */
globalref Widget	uaf_r_widget_id_array[];	/* Array of Widget IDs in use */

/* Local storage. */

#define			app_class   "DECW$AUTHORIZE"
#define			app_name    "Authorize Utility"

Arg			args[ARG_COUNT];		/* Genral X argument list */
MrmType			*class;				/* Widget class */
unsigned int		status;				/* Routine exit status */
char			*uid_filename_vec[] =		/* Heirachy file list. */
			    {"decw$authorize.uid"	/* There is only one UID file for */
			    };				/* this application. */
int			uid_filename_num = (sizeof uid_filename_vec / sizeof uid_filename_vec [0]);

/* Mrm bindings */

static MrmRegisterArg	mrm_resource_list[] =				/* Resource list for Mrm callbacks */
	{
	 {"activate_proc", (caddr_t) AUTHORIZE$DECW_ACTIVATE_PROC},	/* Button callback */
	 {"create_proc", (caddr_t) AUTHORIZE$DECW_CREATE_PROC},		/* Widget creation callback */
	 {"list_proc", (caddr_t) AUTHORIZE$DECW_LIST_PROC},		/* List creation callback */
	 {"pull_proc", (caddr_t) AUTHORIZE$DECW_PULL_PROC},		/* */
	 {"toggle_proc", (caddr_t) AUTHORIZE$DECW_TOGGLE_PROC}		/* */
	};
static int		mrm_resource_list_count = (sizeof mrm_resource_list / sizeof mrm_resource_list[0]);

/* Initialize the resource manager. */

MrmInitialize ();

/* Initialize the X tool kit. */

XtToolkitInitialize ();

/* Create an application context. */

uaf_r_app_context = XtCreateApplicationContext ();

/* Create the display and top level widget for the main window. */

uaf_r_main_display = XtOpenDisplay (uaf_r_app_context,
				    NULL,
				    app_name,
				    app_class,
				    NULL,
				    NULL,
				    &NULL,
				    NULL);
if (uaf_r_main_display == 0) return UAF$_INITDISPLAY;

uaf_r_main_top_level_widget = XtAppCreateShell (app_name,
						app_class,
						applicationShellWidgetClass,
						uaf_r_main_display,
						NULL,
						NULL);
if (uaf_r_main_top_level_widget == 0) return UAF$_INITDISPLAY;

/* Create the display and top level widget for the entry window. */

uaf_r_entry_display = XtOpenDisplay (uaf_r_app_context,
				     NULL,
				     app_name,
				     app_class,
				     NULL,
				     NULL,
				     &NULL,
				     NULL);
if (uaf_r_entry_display == 0) return UAF$_INITDISPLAY;

uaf_r_entry_top_level_widget = XtCreatePopupShell (app_name,
						   topLevelShellWidgetClass,
						   uaf_r_main_top_level_widget,
						   NULL,
						   NULL);
if (uaf_r_entry_top_level_widget == 0) return UAF$_INITDISPLAY;

/* Create the display and top level widget for the file selection window. */

uaf_r_file_display = XtOpenDisplay (uaf_r_app_context,
				    NULL,
				    app_name,
				    app_class,
				    NULL,
				    NULL,
				    &NULL,
				    NULL);
if (uaf_r_file_display == 0) return UAF$_INITDISPLAY;

uaf_r_file_top_level_widget = XtCreatePopupShell (app_name,
						  topLevelShellWidgetClass,
						  uaf_r_main_top_level_widget,
						  NULL,
						  NULL);
if (uaf_r_file_top_level_widget == 0) return UAF$_INITDISPLAY;

/* Open the hierarchy file. */

CHECK_RETURN (MrmOpenHierarchy (uid_filename_num,
				uid_filename_vec,
				NULL,
				&uaf_r_mrm_hierarchy));

/* Register the resources. */

CHECK_RETURN (MrmRegisterNames (mrm_resource_list,
				mrm_resource_list_count));

/* Fetch the main window widget. */

CHECK_RETURN (MrmFetchWidget (uaf_r_mrm_hierarchy,
			      "mainwindow",
			      uaf_r_main_top_level_widget,
			      &uaf_r_main_window_widget,
			      &class));

/* Fetch the identifier window widget. */

CHECK_RETURN (MrmFetchWidget (uaf_r_mrm_hierarchy,
			      "identifier_window",
			      uaf_r_main_top_level_widget,
			      &uaf_r_identifier_window_widget,
			      &class));

/* Fetch the identifier popup menu widget. */

CHECK_RETURN (MrmFetchWidget (uaf_r_mrm_hierarchy,
			      "identifier_popup_menu",
			      uaf_r_entry_top_level_widget,
			      &uaf_r_id_popup_menu_widget,
			      &class));

/* Extablish an event handler for the identifier window. */

XtAddEventHandler (uaf_r_identifier_window_widget,
		   ButtonPressMask,
/*		   False, */
		   True,
		   (caddr_t) AUTHORIZE$DECW_BUTTON_PROC,
		   NULL);

/* Fetch the system identifier window widget. */

CHECK_RETURN (MrmFetchWidget (uaf_r_mrm_hierarchy,
			      "system_identifier_window",
			      uaf_r_main_top_level_widget,
			      &uaf_r_system_id_window_widget,
			      &class));

/* Fetch the entry window widget. */

CHECK_RETURN (MrmFetchWidget (uaf_r_mrm_hierarchy,
			      "entry_window",
			      uaf_r_entry_top_level_widget,
			      &uaf_r_entry_window_widget,
			      &class));

/* Fetch the entry popup menu widget. */

CHECK_RETURN (MrmFetchWidget (uaf_r_mrm_hierarchy,
			      "entry_popup_menu",
			      uaf_r_entry_top_level_widget,
			      &uaf_r_entry_popup_menu_widget,
			      &class));

/* Extablish an event handler for the entry window. */

XtAddEventHandler (uaf_r_entry_window_widget,
		   ButtonPressMask,
/*		   False, */
		   True,
		   (caddr_t) AUTHORIZE$DECW_BUTTON_PROC,
		   NULL);

/* Fetch the primary access restriction window widget. */

CHECK_RETURN (MrmFetchWidget (uaf_r_mrm_hierarchy,
			      "pri_access_restriction_window",
			      uaf_r_entry_top_level_widget,
			      &uaf_r_access_pri_window_widget,
			      &class));

/* Fetch the secondary access restriction window widget. */

CHECK_RETURN (MrmFetchWidget (uaf_r_mrm_hierarchy,
			      "sec_access_restriction_window",
			      uaf_r_entry_top_level_widget,
			      &uaf_r_access_sec_window_widget,
			      &class));

/* Fetch the file selection window widget. */

CHECK_RETURN (MrmFetchWidget (uaf_r_mrm_hierarchy,
			      "file_selection_window",
			      uaf_r_file_top_level_widget,
			      &uaf_r_file_window_widget,
			      &class));

/* Create the wait (watch) cursor. */

uaf_r_wait_cursor_main = DXmCreateCursor (uaf_r_main_top_level_widget,
					  decw$c_wait_cursor);
uaf_r_wait_cursor_entry = DXmCreateCursor (uaf_r_entry_top_level_widget,
					   decw$c_wait_cursor);

/* If the INITIAL_OPEN symbol is defined, disable the OPEN option in
** the file menu.  This is because the various $xxxUAI utilities do not
** know how to deal with a file other than SYS$SYSTEM:SYSUAF.DAT. */

#ifdef INITIAL_OPEN
XtSetArg (args[0],
	  XmNsensitive,
	  0);	/* false */
XtSetValues (uaf_r_widget_id_array[uaf$c_ctl_open],
	     args,
	     ARG_COUNT);
#endif /* INITIAL_OPEN */

/* Return to caller, all set. */

return SS$_NORMAL;
}

extern void AUTHORIZE$DECW_MAIN_LOOP ()

{
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine makes the main window visible and sits in a loop
**	processing X events.
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

/* Global references. */

globalref XtAppContext	uaf_r_app_context;		/* Application context */
globalref Widget	uaf_r_main_top_level_widget;	/* Main window widget shell info */
globalref Widget	uaf_r_main_window_widget;	/* Main window widget info */

/* Manage the main part of the display and realize everything.  This will bring
** up the DECwindows interface. */

XtManageChild (uaf_r_main_window_widget);
XtRealizeWidget (uaf_r_main_top_level_widget);

/* Sit in a loop processing the X-events. */

XtAppMainLoop (uaf_r_app_context);

/* Should never get here. */
}
