/* help_menu.h : help menu widgets and callbacks
**
** Written and Copyright (C) 1994 by Michael J. Gourlay
**
** NO WARRANTEES, EXPRESS OR IMPLIED.
*/
#ifndef _HELP_MENU__INCLUDED_
#define _HELP_MENU__INCLUDED_

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

/* --------------------------------------------------------------- */

/* create_help_menu: Create a help menu and its button
** returns the widget of the menu button
*/
extern Widget create_help_menu(Widget parent, Widget toplevel, Widget left_w);

#endif
