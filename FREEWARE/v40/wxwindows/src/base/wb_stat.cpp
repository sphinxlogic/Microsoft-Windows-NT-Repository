#ifdef __GNUG__
#pragma implementation
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx_panel.h"
#endif

#if USE_EXTENDED_STATICS

#include <wb_stat.h>
#include <wx_dcpan.h>
#include <math.h>

wxbStaticItem::wxbStaticItem(void)
   {
     x_draw = xpos = 0;
     y_draw = ypos = 0;
     w_draw = width = 0;
     h_draw = height = 0;
     panel = NULL;
     dc = NULL;
     isShow = TRUE;
   }

wxbStaticItem::wxbStaticItem(wxPanel *the_panel,int x, int y, int w, int h)
   {
     x_draw = xpos = x;
     y_draw = ypos = y;
     w_draw = width = w;
     h_draw = height = h;
     panel = the_panel;
     isShow = TRUE;
     dc = panel -> GetPanelDC();
   }

wxbStaticItem::~wxbStaticItem(void)
   {
   }

void wxbStaticItem::SetSize(int x,int y,int w,int h, int)
   {
     Bool showed = IsShow();
     if (showed) Show(FALSE);

     if (x >= 0) {
        if (!x) x = 1;
     	x_draw -= xpos - x;
     	xpos = x;
     }

     if (y >= 0) {
        if (!y) y = 1;
     	y_draw -= ypos - y;
     	ypos = y;
     }

     if (w >= 0) {
        if (!w) w = 1;
     	w_draw -= width - w;
        width = w;
     }

     if (h >= 0) {
        if (!h) h = 1;
     	h_draw -= height - h;
        height = h;
     }

     if (showed) Show(TRUE);
   }

#endif
