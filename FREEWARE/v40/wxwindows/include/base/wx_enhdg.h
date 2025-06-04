/*
 * File:	wx_enhdg.h
 * Purpose:	wxEnhancedDialogBox
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "@(#)wx_enhdg.h	1.2 5/9/94" */

#ifndef wx_enhdgh
#define wx_enhdgh

#ifdef __GNUG__
#pragma interface
#endif

#include "wx_defs.h"
#include "wx_setup.h"

#if USE_ENHANCED_DIALOG
#include "wx_item.h"
#include "wx_check.h"
#include "wx_messg.h"
#include "wx_dialg.h"

#ifdef IN_CPROTO
typedef	void	*wxEnhDialogBox;
#else

WXDLLEXPORT extern Constdata char *wxEnhDialogNameStr;

// Enhanced Dialog styles
// Command area placment
#define wxBOTTOM_COMMANDS       0x00000000
#define wxRIGHT_COMMANDS        0x00040000
#define wxMASK_COMMANDS         0x00040000
// Status Area
#define wxSTATUS_FOOTER         0x00000000
#define wxNO_STATUS_FOOTER      0x00080000
#define wxMASK_STATUS           0x00080000
// Cancel Button/Pushpin Emulation
#define wxNO_CANCEL_BUTTON      0x00000000
#define wxCANCEL_BUTTON_FIRST   0x00100000
#define wxCANCEL_BUTTON_LAST    0x00200000
#define wxCANCEL_BUTTON_SECOND  0x00300000
#define wxMASK_CANCEL           0x00300000


#ifdef wx_motif
#define       wxENH_WINDOW_DEFAULT   (wxCAPTION)
#define       wxENH_DEFAULT   (wxBOTTOM_COMMANDS|wxSTATUS_FOOTER|wxNO_CANCEL_BUTTON)
#elif defined(wx_xview)
# define wxENH_WINDOW_DEFAULT 0
# define wxENH_DEFAULT   (wxBOTTOM_COMMANDS|wxSTATUS_FOOTER|wxNO_CANCEL_BUTTON)
#elif defined(wx_msw)
# define wxENH_WINDOW_DEFAULT 0
# define wxENH_DEFAULT   (wxRIGHT_COMMANDS|wxSTATUS_FOOTER|wxCANCEL_BUTTON_SECOND)
#else
#error "Only Motif, XView and MS-Windows/Windows-NT platforms are currently supported"
#endif

// Enhanced Dialog boxes
class WXDLLEXPORT wxEnhDialogBox: public wxDialogBox
{
  DECLARE_DYNAMIC_CLASS(wxEnhDialogBox)

 public:
  wxPanel *panel ;
#ifndef wx_xview
  wxPanel *pinPanel;
#endif
  long style ;
  Bool pinned ;
  wxCheckBox *pinCheck ;
  wxMessage *statusText ;
  wxPanel *userPanel;
  wxPanel *secondaryPanel;
  wxPanel *cmdPanel ;
  wxPanel *statusPanel;
  wxFunction unshow ;
  int userSpacing ;
  int maxWidth;
  int maxHeight ;
  long enhStyle;

  wxEnhDialogBox(void);
  
  // 0,0,10,10 to be sure that resize works.
  wxEnhDialogBox(wxFrame *frame, Const char *title, Bool modal = FALSE,
              wxFunction fun = NULL,int space = -1 ,
              int x = 0, int y = 0,
              int width = 10, int height = 10,
	      long wstyle = wxENH_WINDOW_DEFAULT, long style = wxENH_DEFAULT, Constdata char *name = wxEnhDialogNameStr );
  ~wxEnhDialogBox();

  Bool Create(wxFrame *frame, Const char *title, Bool modal = FALSE,
              wxFunction fun = NULL,int space = -1 ,
              int x = 0, int y = 0,
              int width = 10, int height = 10,
	      long wstyle = wxENH_WINDOW_DEFAULT, long style = wxENH_DEFAULT , Constdata char *name = wxEnhDialogNameStr );
  void SetStatus(char *label=NULL) ;
  wxButton* AddCmd(char *label,wxFunction fun=NULL,int tag = 0) ;
  wxButton* AddCmd(wxBitmap *bitmap,wxFunction fun=NULL,int tag = 0) ;
  wxButton *GetCmd(int number) ;
  void SetPin(Bool flag) ;
  Bool Show(Bool show,Bool flag) ;
  // Avoid compiler warning
  Bool Show(Bool show) { return Show(show, FALSE); }
  void PreFit(void);	 // Get a secondary wxPanel
  void PrimaryFit(void); // Internal use only
  void Fit(void);	 // Final Fitting
} ;

#endif // IN_CPROTO
#endif // USE_ENHANCED_DIALOG
#endif // wx_enhdgh
