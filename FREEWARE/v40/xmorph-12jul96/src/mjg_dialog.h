/* my_dialog.h : My version of the Athena Dialog
**
** This dialog automatically adds the "okay" and "cancel" command
** buttons, registers callbacks for them to popdown the dialog, and adds
** the action to the Text widget so that pressing "Return" calls the
** "okay" callback.  Also, I provide for a callback that the "okay"
** callback calls after popping down the dialog.
**
** This translation should be added to the application's resources:
** Dialog.Text.translations:      #override <Key>Return: mjg_okay()
**
** The user only has to create the mjg_dialog by calling
** create_mjg_dialog() and setting the dialog_apdx.callback and
** dialog_apdx.button members.  The rest is taken care of by these
** routines.  The user may also set the dialog_apdx.client_data member
** to provide for client_data for the dialog_apdx.callback.
** That callback is also given the address of the dialog_apdx as the
** call_data.
**
** Written and Copyright (C) 1994 by Michael J. Gourlay
**
** NO WARRANTEES, EXPRESS OR IMPLIED.
*/


#ifndef _MJG_DIALOG_H__INCLUDED_
#define _MJG_DIALOG_H__INCLUDED_

#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#include <X11/Shell.h>

#ifndef VMS
#include <X11/Xaw/Dialog.h>
#include <X11/Xaw/Text.h>
#include <X11/Xaw/Command.h>
#else
#include <X11Xaw/Dialog.h>
#include <X11Xaw/Text.h>
#include <X11Xaw/Command.h>
#endif

/* The Athena widgets are particularly lame about dialogs
** so I add this appendix of members to their dialog widget
** You'd think that people from MIT would do better.
*/
typedef struct dialog_apdx_ {
  Widget    toplevel;
  Widget    button;
  Widget    shell;
  Widget    dialog;
  Widget    text;
  Widget    okay;
  Widget    cancel;
  void      (*callback)();
  XtPointer client_data;
  int       type;
} dialog_apdx_t;

extern dialog_apdx_t *dialog_apdx_of_widget(Widget w);

/* popup_dialog_cb : callback to popup dialog */
extern void popup_dialog_cb(Widget w, XtPointer client_data, XtPointer call_data);

extern void mjg_dialog_okay(Widget w, XEvent *event, String *params, Cardinal *num_params);

extern dialog_apdx_t *create_mjg_dialog(Widget parent, char *name);

#endif /* _MJG_DIALOG_H__INCLUDED_ */
