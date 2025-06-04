/*
 * File:	dialged.h
 * Purpose:	Dialog editor for wxWindows
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "%W% %G%" */

#ifdef __GNUG__
#pragma interface
#endif

#ifndef dialogedh
#define dialogedh

#include "wx_plist.h"
#include "wx_resed.h"

class MyChild;

// Define a new application
class MyApp: public wxApp
{
  public:
    MyApp(void);
    wxFrame *OnInit(void);
};

extern MyApp myApp;

extern wxFrame *GetMainFrame(void);

#endif
