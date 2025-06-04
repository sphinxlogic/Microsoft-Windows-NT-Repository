/*
 * File:    	wb_scrol.h
 * Purpose: 	Scrollbar items
 * Author:      Sergey Krasnov (ksa@orgland.ru)
 * Created: 	1995
 * Updated:
 * Copyright:
 */

/* sccsid[] = "%W% %G%" */

#ifndef wxb_scrolh
#define wxb_scrolh

#ifdef __GNUG__
#pragma interface
#endif

#include "wx_defs.h"
#include "wx_panel.h"
#include "wx_item.h"

#ifdef IN_CPROTO
typedef       void    *wxbScrollBar ;
#else

WXDLLEXPORT extern Constdata char *wxScrollBarNameStr;

// Scrollbar item
class WXDLLEXPORT wxbScrollBar: public wxItem
{
public:
    wxbScrollBar(void);
    wxbScrollBar(wxPanel *panel, wxFunction func,
                int x = -1, int y = -1, int width = -1, int height = -1,
                long style = wxHORIZONTAL, Constdata char *name = wxScrollBarNameStr);
    ~wxbScrollBar(void);

    virtual void SetValue(int viewStart) = 0;
    virtual int GetValue(void) = 0;

    void Command(wxCommandEvent& event);
    void ProcessCommand(wxCommandEvent& event);
};

#endif // IN_CPROTO
#endif // wxb_scrolh
