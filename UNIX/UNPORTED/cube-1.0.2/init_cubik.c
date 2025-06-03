/*The following references were used:
   "The X Window System Programming And Applications with Xt
   OSF/MOTIF EDITION"
   by Douglas A Young 
   Prentice-Hall, 1990.
   ISBN 0-13-642786-3

   "Mastering Rubik's Cube"
   by Don Taylor
   An Owl Book; Holt, Rinehart and Winston, New York, 1980
   ISBN 0-03-059941-5

-------------------------------------------------------------------
Copyright (C) 1993 by Pierre A. Fleurant
Permission is granted to copy and distribute this program
without charge, provided this copyright notice is included
in the copy.
This Software is distributed on an as-is basis. There will be
ABSOLUTELY NO WARRANTY for any part of this software to work
correct. In no case will the author be liable to you for damages
caused by the usage of this software.
-------------------------------------------------------------------

  init_cubik
  This is a callback that creates a message box.
  The message box contains a pallette of colors 
  and 6 buttons (front, back, ...etc.) for each 
  side of the cube. 
   The user chooses a side which causes the main window's
  canvas to display the side. Then the user picks a color
  from the pallette. After this the uses clicks on the 
  canvas' cube subplane to init to this color.
   
  `OK' ends the operation.
 
  This is used with cube.c
 
                       popup(transientShell)
                        |
                       framework
                        |
               ---------|
               |        |
            message  command
                        |---------|
                        |         |
                      tiles      panel
		      
 */
#include "init_cubik.h"
#include "common.h"

void xcube_init_cubik(w, wdata, call_data) 
     Widget          w; 
     widget_data     *wdata;
     caddr_t         call_data;
{
  int        i,n;
  Widget     framework,message,command,tiles,panel;
  Arg        wargs[10];
  XmString   xmstr;

  /*
   * Create the dialog 
   */
  wdata->popup = XtCreatePopupShell("Init_Cube",transientShellWidgetClass,
			     w,NULL,0);


  framework = XtCreateManagedWidget("popframework", 
                                    xmFormWidgetClass, 
                                    wdata->popup, NULL, 0);

  /*
   * Count the text up to the first NULL string.
   */
  for(i=0; popup_str[i][0] != '\0'; i++)
    ;
  /*
   * Convert the string array to an XmString array and 
   * set it as the label text.
   */
  xmstr  = xs_str_array_to_xmstr(popup_str, i);

  n = 0;
  XtSetArg(wargs[n],  XmNmessageString, xmstr);n++;
  /* add a message */

  message = XtCreateManagedWidget("popmessage",
				  xmMessageBoxWidgetClass,
				  framework,wargs,n);
  /* Make the button say 'Done' instead of 'Ok' */
  xmstr = XmStringCreate("Done", XmSTRING_DEFAULT_CHARSET);
  n = 0;
  XtSetArg(wargs[n], XmNokLabelString, xmstr);     n++;
  XtSetValues(message, wargs, n);
  
  /* Add OK (Done) callback to pop down the dialog. */
  XtAddCallback(message, XmNokCallback, xcic_done_callback, wdata);
  
  /*
   * We won't be using the help and cancel widgets. Unmanage 'em.
   */
  XtUnmanageChild(XmMessageBoxGetChild (message,
					XmDIALOG_HELP_BUTTON));
  XtUnmanageChild(XmMessageBoxGetChild (message,
					XmDIALOG_CANCEL_BUTTON));
  
  /*
   * Create the column to hold pallette and side buttons (panel).
   */
  command = XtCreateManagedWidget("popcommand", 
                                  xmRowColumnWidgetClass, 
                                  framework, NULL, 0);
  
  /* add pallette */
  tiles = xss_create_pixmap_browser(command, 
				    seed_color,
				    6, /* sides */
				    set_fill_pattern, wdata->graph_pointer);
  /* add side buttons */
  panel = xss_create_label_wbutton(command, 
				   side_names,
				   6, /* sides */
				   set_cube_side, wdata);
  
  XtManageChild(tiles);
  XtManageChild(panel);

  /* fix pallette orientation */
  n = 0;
  XtSetArg(wargs[n], XmNorientation,    XmHORIZONTAL);     n++;
  XtSetValues(tiles, wargs, n);

  /* fix side names orientation */
  n = 0;
  XtSetArg(wargs[n], XmNorientation,    XmHORIZONTAL);     n++;
  XtSetValues(panel, wargs, n);

  /* specifiy attachments */
  /* message */
  n = 0;
  XtSetArg(wargs[n], XmNtopAttachment,    XmATTACH_FORM);     n++;
  XtSetArg(wargs[n], XmNleftAttachment,   XmATTACH_FORM);     n++;
  XtSetValues(message, wargs, n);

  /* command */
  n = 0;
  XtSetArg(wargs[n], XmNtopAttachment,    XmATTACH_WIDGET);   n++;
  XtSetArg(wargs[n], XmNtopWidget,        message);           n++;
  XtSetArg(wargs[n], XmNleftAttachment,   XmATTACH_FORM);     n++;
  XtSetValues(command, wargs, n);

  /* 
     Cube must be inited because we can't orient back from
     an arbitrary r-s-f angle.
   */
  init_cube(wdata->canvas, wdata, NULL);

  XtPopup(wdata->popup,XtGrabNone);
  
}

void xcic_done_callback(w, wdata, call_data) 
     Widget               w; 
     widget_data     *wdata;
     XmAnyCallbackStruct *call_data; 
{
  align_subfaces(w,wdata,call_data); /* update front[], left[],... */
  XtUnmanageChild(wdata->popup);
  XtDestroyWidget(wdata->popup);   
  /* reset */
  Fill_Pattern_Chosen = FALSE;
  Cube_Side_Chosen = FALSE;
}
