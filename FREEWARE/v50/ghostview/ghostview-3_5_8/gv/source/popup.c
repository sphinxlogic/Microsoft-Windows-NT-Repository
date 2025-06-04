/*
**
** popup.c
**
** Copyright (C) 1995, 1996, 1997 Johannes Plass
** 
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
** 
** Author:   Johannes Plass (plass@thep.physik.uni-mainz.de)
**           Department of Physics
**           Johannes Gutenberg-University
**           Mainz, Germany
**
*/

/*
#define MESSAGES
*/
#include "message.h"

#include "config.h"

#include <stdio.h>

#include "paths.h"
#include INC_X11(Intrinsic.h)
#include INC_X11(StringDefs.h)
#include INC_X11(IntrinsicP.h)
#include INC_X11(ShellP.h)

#include "types.h"
#include "popup.h"

/*##################################################################*/
/* cb_popdownPopup Callback */
/*##################################################################*/

void
cb_popdownPopup(w, client_data, call_data)
    Widget w;
    XtPointer client_data, call_data;
{
    BEGINMESSAGE(cb_popdownPopup)
    XtPopdown((Widget)client_data);
    ENDMESSAGE(cb_popdownPopup)
}

/*##################################################################*/
/* cb_popupPopup Callback */
/* Popup a window. */
/*##################################################################*/

void
cb_popupPopup(w, client_data, call_data)
    Widget w;
    XtPointer client_data, call_data;
{
    BEGINMESSAGE(cb_popupPopup)
    XtPopup((Widget)client_data,XtGrabNone);
    XRaiseWindow(XtDisplay((Widget)client_data), XtWindow((Widget)client_data));
    ENDMESSAGE(cb_popupPopup)
}
 
/*##################################################################*/
/* popup_positionPopup */
/*##################################################################*/

static int wmtdecor = 0;
static int wmbdecor = 0;
static int wmsdecor = 0;

void
popup_positionPopup(popup,ref_widget,mode,refx,refy)
   Widget popup;
   Widget ref_widget;
   int mode;
   int refx;
   int refy;
{
   Arg args[5];
   Cardinal n;
   Position posx,posy;
   Position posrefx,posrefy;
   Dimension width,height;
   Dimension screenwidth, screenheight;

   BEGINMESSAGE(popup_positionPopup)

   if (((ShellWidget)popup)->shell.popped_up) {
      INFMESSAGE(will not reposition popped up popup)
      ENDMESSAGE(popup_positionPopup)
      return;  
   }

   posrefx = (Position)(refx);
   posrefy = (Position)(refy);
   IIMESSAGE(posrefx,posrefy)

   screenwidth =  XtScreen(ref_widget)->width;
   screenheight = XtScreen(ref_widget)->height;
   IIMESSAGE(screenwidth,screenheight);

   width = popup->core.width;
   height = popup->core.height;
   IIMESSAGE(width,height);

   XtTranslateCoords(ref_widget,0,0,&posx,&posy);
   IIMESSAGE(posx,posy);

   if (mode==POPUP_POSITION_POS) {
      posx += posrefx + (Position)(wmsdecor);
      posy += posrefy + (Position)(wmtdecor);
   } else if (mode==POPUP_POSITION_CENTER) {
      Dimension w,h;
      w = ref_widget->core.width;
      h = ref_widget->core.height;
      posx += (Position)(w/2) - (Position)(width/2);
      posy += (Position)(h/2) - (Position)(height/2);
   } else if (mode==POPUP_POSITION_POS_CENTER) {
      posx += posrefx - (Position)(width/2);
      posy += posrefy - (Position)(height/2);
   }
   INFIIMESSAGE(after adding offsets,posx,posy)

   if (posx + (Position)width  >= (Position)screenwidth) 
       posx = ((Position)screenwidth)  - ((Position)width)  - ((Position)wmsdecor); 
   if (posx < 0) posx = 0;
   if (posy + (Position)height >= (Position)screenheight)
       posy = ((Position)screenheight) - ((Position)height) - ((Position)wmbdecor); 
   if (posy < 0) posy = 0;
   INFIIMESSAGE(after adjustment,posx,posy)

                                          n=0;
   XtSetArg(args[n], XtNx, posx);         n++;
   XtSetArg(args[n], XtNy, posy);         n++;
   XtSetValues(popup, args, n);

   ENDMESSAGE(popup_positionPopup)
}
