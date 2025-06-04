/*
 * File:	wx_item.h
 * Purpose:	Declares panel items (controls/widgets) for X
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "@(#)wx_item.h	1.2 5/9/94" */

#ifndef wx_itemh
#define wx_itemh

#ifdef __GNUG__
#pragma interface
#endif

#include "wx_defs.h"
#include "wx_win.h"
#include "wb_item.h"

#ifdef IN_CPROTO
typedef       void    *wxItem ;
#else

// General item class
class wxItem: public wxbItem
{
  DECLARE_ABSTRACT_CLASS(wxItem)

 public:
    wxItem(void);
   ~wxItem(void);
#ifdef wx_motif
   // Each item is a form/rowcol widget, optional label widget, plus a specific
   // widget
   Widget formWidget;
   Widget labelWidget;
   int rowNumber;
   int colNumber;
   XtTranslations oldTranslations; // Save translations
                                   // when toggling to UI edit mode
   Bool canAddEventHandler;        // Can we add an event handler to this item?
   Bool inSetValue;                // If we're in SetValue, don't issue a callback.
#endif
#ifdef wx_x
   // Store the actual label (may be a label with the accelerators
   // stripped out). We'll want to delete in the destructor.
   char *actualLabel;
#endif

   void GetSize(int *width, int *height);
   void GetPosition(int *x, int *y);
   void SetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO);
   // Avoid compiler warning
   void SetSize(int w, int h) { wxbItem::SetSize(w, h); }
   void SetFocus(void);
   void SetLabel(char *label);
   char *GetLabel(void);

   Bool Show(Bool show);
   void Enable(Bool enable);
   float GetCharHeight(void);
   float GetCharWidth(void);

   virtual void SetBackgroundColour(wxColour& col)
          { backColour = col; ChangeColour(); }
   virtual void SetLabelColour(wxColour& col)
          { labelColour = col ; ChangeColour(); }
   virtual void SetButtonColour(wxColour& col)
          { buttonColour = col ; ChangeColour(); }
   virtual void ChangeColour(void);

   void GetTextExtent (const char *string,
                            float *x,
                            float *y,
                            float *descent,
                            float *externalLeading = NULL,
                            wxFont * fontparm = NULL, Bool use16 = FALSE);

#ifdef wx_motif
   void RemoveTranslations(Bool remove = TRUE); // FALSE restores again
   inline void SetCanAddEventHandler(Bool can) { canAddEventHandler = can; }
   inline Bool CanAddEventHandler(void) { return canAddEventHandler; }
#endif
};

#endif // IN_CPROTO
#endif // wx_itemh
