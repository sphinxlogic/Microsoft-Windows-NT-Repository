/* warp_menu.c : warp menu widgets and callbacks
**
** Written and Copyright (C) 1994 by Michael J. Gourlay
**
** NO WARRANTEES, EXPRESS OR IMPLIED.
*/

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#ifndef VMS
#include <X11/Xaw/MenuButton.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/SmeBSB.h>
#include <X11/Xaw/SmeLine.h>
#else
#include <X11Xaw/MenuButton.h>
#include <X11Xaw/SimpleMenu.h>
#include <X11Xaw/SmeBSB.h>
#include <X11Xaw/SmeLine.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#include "mjg_dialog.h"
#include "sequence.h"
#include "warp_menu.h"

/* =============================================================== */
/* --------------------------------------------------------------- */

/* create_warp_menu: Create a warp menu and its button
** returns the widget of the menu button
*/
Widget
create_warp_menu(Widget parent, Widget toplevel, Widget left_w)
{
  Widget warp_menu_button;
  Widget warp_menu;
  Widget   wm_ssfn_sme;            /* set sequence file name */
  Widget   wm_ssns_sme;            /* set sequence number of steps */
  Widget   wm_seq_sme;             /* warp sequence */
  dialog_apdx_t *wm_ssfn_daP;
  dialog_apdx_t *wm_ssns_daP;

  /* ========================= */
  /* Create the Dialog widgets */
  /*  - - - - - - - - - - - - - - - - -  */
  /* Create the "set sequence file name" dialog */
  wm_ssfn_daP = create_mjg_dialog(toplevel, "ssfn");
  wm_ssfn_daP->client_data = NULL;
  wm_ssfn_daP->callback = set_sequence_file_name_cb;

  /*  - - - - - - - - - - - - - - - - -  */
  /* Create the "set sequence number of steps" dialog */
  wm_ssns_daP = create_mjg_dialog(toplevel, "ssns");
  wm_ssns_daP->client_data = NULL;
  wm_ssns_daP->callback = set_sequence_num_frames_cb;

  /* ----------------------------- */
  /* Create a simple menu for Command */
  warp_menu = XtVaCreatePopupShell("warp_menu", simpleMenuWidgetClass,
    parent, NULL);

    /* - - - - - - - - - - - - - - - - - - - -  */
    /* Create a menu entry for "set sequence file name" */
    wm_ssfn_sme = XtVaCreateManagedWidget("wm_ssfn_sme", smeBSBObjectClass,
      warp_menu, NULL);
    XtAddCallback(wm_ssfn_sme, XtNcallback, popup_dialog_cb, wm_ssfn_daP);

    wm_ssfn_daP->button = wm_ssfn_sme;

    /* - - - - - - - - - - - - - - - - - - - -  */
    /* Create a menu entry for "set sequence number of steps" */
    wm_ssns_sme = XtVaCreateManagedWidget("wm_ssns_sme", smeBSBObjectClass,
      warp_menu, NULL);
    XtAddCallback(wm_ssns_sme, XtNcallback, popup_dialog_cb, wm_ssns_daP);

    wm_ssns_daP->button = wm_ssns_sme;

    /* - - - - - - - - - - - - - - - - - - - -  */
    /* Create a menu entry for "warp sequence" */
    wm_seq_sme = XtVaCreateManagedWidget("wm_seq_sme", smeBSBObjectClass,
      warp_menu, NULL);
    XtAddCallback(wm_seq_sme, XtNcallback, warp_sequence_cb, NULL);

  /* ------------------------- */
  /* Create a warp menu button */
  warp_menu_button = XtVaCreateManagedWidget("warp_menu_button",
    menuButtonWidgetClass, parent, XtNmenuName, "warp_menu",
    XtNfromHoriz, left_w, NULL);

  return(warp_menu_button);
}
