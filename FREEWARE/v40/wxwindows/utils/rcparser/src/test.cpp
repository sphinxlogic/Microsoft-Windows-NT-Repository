// ************************************************************************
// *********************** Test for rcParser Library **********************
// ***********************  not very polished, just to get the idea *******
//             Petr Smilauer <petrsm@entu.cas.cz>, 1 March 1994 ***********

#include <wx.h>

#include "rcobject.h"

class  TestApplication: public wxApp
{
    public:
  wxFrame *OnInit( void);

};

TestApplication  application;

// this is the frame with panel with the main information about parsed file
class wxTestFrame: public wxFrame
{
    public:
  wxTestFrame( wxFrame *parent, char *title, int x, int y, int w, int h);
 ~wxTestFrame() { if(rcf != 0) delete rcf; }
  void Create( wxFrame *parent, char *title, int x, int y, int w, int h);
  Bool OnClose( void);
  void OnMenuCommand( int id);
  void UpdatePanel( int nChange = 0); // if 1, means only dialog selection change
  void DisplayMenusOn( wxFrame *fr);

  rcFile    *rcf;
  wxPanel   *panel;
  wxListBox *wxlbDialogs,
            *wxlbControls;  // list of controls for the current dialog-box
  wxText    *X,      // current dialog attributes
            *Y,
            *W,
            *H,
            *Style,
            *Name;
  wxButton  *ShowMenu;
  int        NDialogs;
};

// this frame is only to demonstrate parsed menus and their styles
class wxSMFrame: public wxFrame
{
    public:
  wxSMFrame( wxFrame *parent):
    wxFrame( parent, "MENUS", 0, 0, 400, 80,
          wxSDI | wxTHICK_FRAME | wxSYSTEM_MENU | wxCAPTION | wxSTAY_ON_TOP) { }
  Bool OnClose( void) { return TRUE;}
};

// callback
void wxLBCallBack( wxObject &obj, wxEvent&)
// this is installed only for dialog boxes listbox: hence, we know what is
// going on...
{ wxListBox   *lbox  = (wxListBox *)&obj;
  wxTestFrame *frame = (wxTestFrame *)(lbox->GetGrandParent());

  frame->UpdatePanel( 1);
}

// callback for 'Menus' button.
void wxSMCallBack( wxObject &obj, wxEvent&)
{ wxButton    *but    = (wxButton *)&obj;
  wxTestFrame *frame  = (wxTestFrame *)(but->GetGrandParent());
  wxSMFrame   *Tframe = new wxSMFrame( frame);

  new wxPanel( Tframe); // just for dtr to be satisfied..?

  frame->DisplayMenusOn( Tframe);
  Tframe->Show( TRUE);
}

// update according the current application state
void wxTestFrame::UpdatePanel( int nChange)
{ static Bool fFirst = TRUE;

  if(fFirst)
  {
    fFirst = FALSE;
    panel->SetLabelPosition(wxVERTICAL);
    wxlbDialogs = new wxListBox( panel, wxLBCallBack, "Dialog Boxes");
    panel->NewLine();
    X = new wxText( panel, 0, "X:", "000"); panel->Tab(5);
    Y = new wxText( panel, 0, "Y:", "000"); panel->NewLine();
    W = new wxText( panel, 0, "W:", "000"); panel->Tab(5);
    H = new wxText( panel, 0, "H:", "000"); panel->NewLine();
    Style = new wxText( panel, 0, "Style:", "0x00000000"); panel->Tab(5);
    Name = new wxText( panel, 0, "Caption:", "_____________"); panel->NewLine();
    panel->Tab(150); new wxMessage( panel, "Controls"); panel->NewLine();
    wxlbControls = new wxListBox( panel, 0, 0, wxSINGLE, -1, -1, 590);
    panel->NewLine();
    ShowMenu = new wxButton( panel, wxSMCallBack, "Menus");
    panel->Fit();
    ShowMenu->Enable( FALSE);
  }
  if(rcf == 0)
    return;

  int    N,
         i;

  if(nChange == 0)  // i.e. we have read new RC file!
  { rcDialog *rcd;

    N = rcf->nDialogs();
    wxlbDialogs->Clear();
    for( i = 0 ; i < N ; ++i)
    {
      if( (rcd = (rcDialog *)rcf->GetIthDialog( i)) != 0)
        wxlbDialogs->Append( (char *)rcd->dlgName());
    }
    if(N > 0)
      wxlbDialogs->SetSelection( 0);
    NDialogs = N;
    wxlbControls->Clear();
    X->SetValue( " "); Y->SetValue( " "); W->SetValue( " "); H->SetValue( " ");
    Name->SetValue( " "); Style->SetValue( " ");
    ShowMenu->Enable( (rcf->nMenus() > 0));
  }
  if(NDialogs > 0)
  { rcDialog *rcd = (rcDialog *)rcf->GetIthDialog( wxlbDialogs->GetSelection());
    char      szBuf[16];

    if(rcd == 0)
      return;

    sprintf( szBuf, "%i", rcd->left());   X->SetValue( szBuf);
    sprintf( szBuf, "%i", rcd->top());    Y->SetValue( szBuf);
    sprintf( szBuf, "%i", rcd->width());  W->SetValue( szBuf);
    sprintf( szBuf, "%i", rcd->height()); H->SetValue( szBuf);
    sprintf( szBuf, "%lx", rcd->Style()); Style->SetValue( szBuf);
    Name->SetValue((char *)rcd->caption());

    N = rcd->nItems();
    wxlbControls->Clear();

    char szB[75];
    rcItem *rci;

    for( i = 0 ; i < N ; ++i)
    {
      rci = (rcItem *)rcd->getIthItem( i);
      sprintf( szB, "%s:\"%.20s\" %i(%s) [%3i,%3i] [%3i,%3i] %lx",
               rci->szItemType(), rci->caption(), rci->ID(),
               rci->szIdName(),  // NEW ONE 10 Mar 94
               rci->left(), rci->top(), rci->width(), rci->height(),
               rci->Style());
      wxlbControls->Append( szB);
    }
  }
}

// parses menu spefication, creating wxWindows menus on the fly (recursively)
wxMenu *ParseMenu( rcMenuPopup *pop)
{ rcMenuItem    *rcm;
  wxMenu        *menu;
  char          *szName = pop->GetName();
  register int   i,
                 N = pop->NChilds();


  menu = new wxMenu();
  menu->SetTitle( szName);
  for( i = 0 ; i < N ; ++i)
  {
    rcm    = pop->rcGetIthChild( i);

    szName = rcm->GetName();
    if(rcm->IsPopup())
      menu->Append( 0, szName, ParseMenu( (rcMenuPopup *)rcm));
    else // normal item
      if(rcm->IsSeparator())
        menu->AppendSeparator();
      else
      { int ID = rcm->GetID();

        menu->Append( ID, szName);
        if(rcm->IsChecked())
          menu->Check( ID, TRUE);
        if(rcm->IsGrayed())
          menu->Enable( ID, FALSE);
      }
  }
  return menu;
}

// make menus demo to happen
void  wxTestFrame::DisplayMenusOn( wxFrame *fr)
{ int        NM,
             i;
  wxMenuBar *bar = new wxMenuBar;

  NM = rcf->nMenus();
  if(NM <= 0)
    return;
  if(NM == 1) // one menu defined: could be rather frequent situation!
  { rcMenuPopup *pop = rcf->GetIthMenu( 0);
    rcMenuItem  *rcm;

    NM = pop->NChilds();
    for( i = 0 ; i < NM ; ++i)
    { char *szName;

      rcm    = pop->rcGetIthChild( i);
      szName = rcm->GetName();
      if(rcm->IsPopup())  // BUT SHOULD BE ALWAYS!
        bar->Append( ParseMenu( (rcMenuPopup *)rcm), szName);
    }
  }
  else
  { rcMenuPopup *pop;

    for( i = 0 ; i < NM ; ++i)  // here NM has the original value!
    { char *szName;

      pop = rcf->GetIthMenu( i);
      szName = pop->GetName();
      bar->Append( ParseMenu( pop), szName);
    }
  }
  fr->SetMenuBar( bar);
}

// my frame ctr
wxTestFrame::wxTestFrame( wxFrame *parent, char *title, int x, int y, int w, int h)
 :   wxFrame( parent, title, x, y, w, h)
{
  rcf      = 0;
  NDialogs = 0;
}

void wxTestFrame::Create( wxFrame *parent, char *title, int x, int y, int w, int h)
{
  wxFrame::Create( parent, title, x, y, w, h);
}

Bool wxTestFrame::OnClose( void)
{
  return TRUE;
}

// application's menu
void wxTestFrame::OnMenuCommand( int id)
{ char *szFileName,
       *szFlags;

  switch( id)
  {
    case 1:
      if( (szFileName = wxFileSelector( "Select RC file to parse", 0, 0, "rc",
                                        "*.rc", wxOPEN, this)) != 0)
      {
        if( rcf != 0)
          delete rcf;
        rcf = new rcFile;

        if( (szFlags = wxGetTextFromUser( "Enter -I parameters for parser",
                                          "Enter parser options", "",
                                          this)) == 0)
          szFlags = "";
        UpdatePanel();
        rcf->Read( szFileName, szFlags);
        UpdatePanel();
      }
      break;
    case 2:
      OnClose();
      delete this;
      break;
  }
}


wxFrame* TestApplication::OnInit( void)
{
  wxTestFrame *wxfWindow = new wxTestFrame( 0, "RCParser Demo", 20, 20, 620, 420);
  wxfWindow->panel = new wxPanel( wxfWindow, 0, 0, 600, 420);

  wxMenu *wxmFile = new wxMenu;
  wxmFile->Append( 1, "Parse RC file");
  wxmFile->Append( 2, "Exit");
  wxMenuBar *wxmbBar = new wxMenuBar;
  wxmbBar->Append( wxmFile, "File");
  wxfWindow->SetMenuBar( wxmbBar);

  wxfWindow->UpdatePanel();
  wxfWindow->Fit();
  wxfWindow->Show( TRUE);

  return wxfWindow;

}
