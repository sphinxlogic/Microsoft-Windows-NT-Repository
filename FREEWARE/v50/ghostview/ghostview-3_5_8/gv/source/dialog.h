/*
**
** dialog.h
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


#ifndef	_GV_DIALOG_H_
#define	_GV_DIALOG_H_

#define DIALOG_BUTTON_DONE   (1<<0)
#define DIALOG_BUTTON_CANCEL (1<<1)

extern void cb_popdownDialogPopup (
#if NeedFunctionPrototypes
   Widget,
   XtPointer,
   XtPointer
#endif
);

extern void cb_popupDialogPopup (
#if NeedFunctionPrototypes
   Widget,
   XtPointer,
   XtPointer
#endif
);

extern void action_preferDialogPopupButton (
#if NeedFunctionPrototypes
    Widget,
    XEvent *,
    String *,
    Cardinal *
#endif
);

extern void makeDialogPopup (
#if NeedFunctionPrototypes
#endif
);

extern void DialogPopupSetButton (
#if NeedFunctionPrototypes
   int,
   String,
   XtCallbackProc
#endif
);

extern void DialogPopupClearText (
#if NeedFunctionPrototypes
#endif
);

extern void DialogPopupSetText (
#if NeedFunctionPrototypes
    String
#endif
);

extern String DialogPopupGetText (
#if NeedFunctionPrototypes
#endif
);

extern void DialogPopupSetPrompt (
#if NeedFunctionPrototypes
    String
#endif
);

extern void DialogPopupSetMessage (
#if NeedFunctionPrototypes
    String
#endif
);

#endif	/* _GV_DIALOG_H_ */
