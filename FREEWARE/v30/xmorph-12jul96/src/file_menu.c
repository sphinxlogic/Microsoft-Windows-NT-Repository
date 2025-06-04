/* file_menu.c : file menu widgets and callbacks
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
#include "mesh_cb.h"
#include "image_cb.h"
#include "file_menu.h"

/* =============================================================== */
/* --------------------------------------------------------------- */

/* create_file_menu: Create a file menu and its button
** returns the widget of the menu button
*/
Widget
create_file_menu(Widget parent, Widget toplevel, Widget left_w)
{
  Widget file_menu_button;
  Widget file_menu;
  Widget   fm_osi_sme, fm_odi_sme, fm_osm_sme, fm_odm_sme;
  Widget   fm_ssm_sme, fm_sdm_sme;
  Widget   fm_quit_sme;
  Widget   fm_sep_sme; /* menu item separator */
  dialog_apdx_t *fm_osi_daP, *fm_odi_daP;
  dialog_apdx_t *fm_osm_daP, *fm_odm_daP;
  dialog_apdx_t *fm_ssm_daP, *fm_sdm_daP;

  /* ========================= */
  /* Create the Dialog widgets */
  /*  - - - - - - - - - - - - - - - - -  */
  /* Create the Open Source Image dialog */
  fm_osi_daP = create_mjg_dialog(toplevel, "osi");
  fm_osi_daP->client_data = (XtPointer)1;
  fm_osi_daP->callback = load_img_cb;

  /* - - - - - - - - - - - - - - - - - - - -  */
  /* Create the Open Destination Image dialog */
  fm_odi_daP = create_mjg_dialog(toplevel, "odi");
  fm_odi_daP->client_data = (XtPointer)2;
  fm_odi_daP->callback = load_img_cb;

  /*  - - - - - - - - - - - - - - - - -  */
  /* Create the Open Source Mesh dialog */
  fm_osm_daP = create_mjg_dialog(toplevel, "osm");
  fm_osm_daP->client_data = (XtPointer)1;
  fm_osm_daP->callback = load_mesh_cb;

  /* - - - - - - - - - - - - - - - - - - - -  */
  /* Create the Open Destination Mesh dialog */
  fm_odm_daP = create_mjg_dialog(toplevel, "odm");
  fm_odm_daP->client_data = (XtPointer)2;
  fm_odm_daP->callback = load_mesh_cb;

  /*  - - - - - - - - - - - - - - - - -  */
  /* Create the Save Source Mesh dialog */
  fm_ssm_daP = create_mjg_dialog(toplevel, "ssm");
  fm_ssm_daP->client_data = (XtPointer)1;
  fm_ssm_daP->callback = save_mesh_cb;

  /*  - - - - - - - - - - - - - - - - -  */
  /* Create the Save Destination Mesh dialog */
  fm_sdm_daP = create_mjg_dialog(toplevel, "sdm");
  fm_sdm_daP->client_data = (XtPointer)2;
  fm_sdm_daP->callback = save_mesh_cb;


  /* ----------------------------- */
  /* Create a simple menu for File */
  file_menu = XtVaCreatePopupShell("file_menu", simpleMenuWidgetClass, parent,
    NULL);

    /* - - - - - - - - - - - - - - - - - - - -  */
    /* Create a menu entry for "open source image" */
    fm_osi_sme = XtVaCreateManagedWidget("fm_osi_sme", smeBSBObjectClass,
      file_menu, NULL);
    XtAddCallback(fm_osi_sme, XtNcallback, popup_dialog_cb, fm_osi_daP);

    fm_osi_daP->button = fm_osi_sme;

    /* - - - - - - - - - - - - - - - - - - - -  */
    /* Create a menu entry for "open destination image" */
    fm_odi_sme = XtVaCreateManagedWidget("fm_odi_sme", smeBSBObjectClass,
      file_menu, NULL);
    XtAddCallback(fm_odi_sme, XtNcallback, popup_dialog_cb, fm_odi_daP);

    fm_odi_daP->button = fm_odi_sme;

    /* - - - - - - - - - - - - - -  */
    /* Create a menu item separator */
    fm_sep_sme = XtVaCreateManagedWidget("fm_sep_sme", smeLineObjectClass,
      file_menu, NULL);

    /* - - - - - - - - - - - - - - - - - - - -  */
    /* Create a menu entry for "open source mesh" */
    fm_osm_sme = XtVaCreateManagedWidget("fm_osm_sme", smeBSBObjectClass,
      file_menu, NULL);
    XtAddCallback(fm_osm_sme, XtNcallback, popup_dialog_cb, fm_osm_daP);

    fm_osm_daP->button = fm_osm_sme;

    /* - - - - - - - - - - - - - - - - - - - -  */
    /* Create a menu entry for "open destination mesh" */
    fm_odm_sme = XtVaCreateManagedWidget("fm_odm_sme", smeBSBObjectClass,
      file_menu, NULL);
    XtAddCallback(fm_odm_sme, XtNcallback, popup_dialog_cb, fm_odm_daP);

    fm_odm_daP->button = fm_odm_sme;

    /* - - - - - - - - - - - - - -  */
    /* Create a menu item separator */
    fm_sep_sme = XtVaCreateManagedWidget("fm_sep_sme", smeLineObjectClass,
      file_menu, NULL);

    /* - - - - - - - - - - - - - - - - - - - -  */
    /* Create a menu entry for "save source mesh" */
    fm_ssm_sme = XtVaCreateManagedWidget("fm_ssm_sme", smeBSBObjectClass,
      file_menu, NULL);
    XtAddCallback(fm_ssm_sme, XtNcallback, popup_dialog_cb, fm_ssm_daP);

    fm_ssm_daP->button = fm_ssm_sme;

    /* - - - - - - - - - - - - - - - - - - - -  */
    /* Create a menu entry for "save destination mesh" */
    fm_sdm_sme = XtVaCreateManagedWidget("fm_sdm_sme", smeBSBObjectClass,
      file_menu, NULL);
    XtAddCallback(fm_sdm_sme, XtNcallback, popup_dialog_cb, fm_sdm_daP);

    fm_sdm_daP->button = fm_sdm_sme;

    /* - - - - - - - - - - - - - -  */
    /* Create a menu item separator */
    fm_sep_sme = XtVaCreateManagedWidget("fm_sep_sme", smeLineObjectClass,
      file_menu, NULL);

    /* - - - - - - - - - - - - - -  */
    /* Create a menu item separator */
    fm_sep_sme = XtVaCreateManagedWidget("fm_sep_sme", smeLineObjectClass,
      file_menu, NULL);

    /* - - - - - - - - - - - - - - - - - - - -  */
    /* Create a menu entry for "quit" */
    fm_quit_sme = XtVaCreateManagedWidget("fm_quit_sme", smeBSBObjectClass,
      file_menu, NULL);
    XtAddCallback(fm_quit_sme, XtNcallback, exit, NULL);

  /* ------------------------- */
  /* Create a file menu button */
  if(left_w != NULL) {
    file_menu_button = XtVaCreateManagedWidget("file_menu_button",
      menuButtonWidgetClass, parent, XtNmenuName, "file_menu",
      XtNfromHoriz, left_w, NULL);
  } else {
    file_menu_button = XtVaCreateManagedWidget("file_menu_button",
      menuButtonWidgetClass, parent, XtNmenuName, "file_menu", NULL);
  }

  return(file_menu_button);
}
