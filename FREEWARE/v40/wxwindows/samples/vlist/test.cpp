// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx.h"
#endif

#if !USE_VLBOX
#error You must set USE_VLBOX to 1 in wx_setup.h to compile this demo.
#endif

#include "wx_popup.h"

static int nItems  = 0;
char * Item(char * arg, int n)
{
    static char sz[128];
    if (n < nItems) {
        if (n == 30)
            sprintf(sz, "Item %d (horizontal scrollbar should appear!", n);
        else
            sprintf(sz, "Item %d", n);
        return sz;
    }
    return NULL;
}

void OnExit(char ** ear, int n)
{
   char sz[128];
   sprintf(sz, "Item %d selected", n);
   wxMessageBox(sz, "Popup exit");
}

void SelectionProc(wxPopupMenu& popup, wxCommandEvent& event)
{
    wxFrame * aframe = (wxFrame *) popup.GetParent();
    aframe->SetStatusText(Item(NULL, event.commandInt));
}

// A macro needed for some compilers (AIX) that need 'main' to be defined
// in the application itself.
IMPLEMENT_WXWIN_MAIN

class MyApp: public wxApp
{ public:
    wxFrame *OnInit(void);
};

class MyFrame: public wxFrame
{ public:
    MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h);
    void OnMenuCommand(int id);
};

// ID for the menu commands
#define TEST_QUIT       1
#define POPUP_15        2
#define POPUP_20        3
#define POPUP_95        4
#define POPUP_10000     5

MyApp myApp;
MyFrame *frame;

wxFrame *MyApp::OnInit(void)
{
    frame = new MyFrame(NULL, "Virtual listbox test app", 50, 50, 400, 300);

    wxMenu *menu_quit = new wxMenu;
    menu_quit->Append(TEST_QUIT, "Quit");

    wxMenu *menu_popup = new wxMenu;
    menu_popup->Append(POPUP_15, "15 Items");
    menu_popup->Append(POPUP_20, "20 Items");
    menu_popup->Append(POPUP_95, "95 Items");
    menu_popup->Append(POPUP_10000, "10000 Items");

    wxMenuBar *menu_bar = new wxMenuBar;
    menu_bar->Append(menu_quit, "Quit");
    menu_bar->Append(menu_popup, "Popup");
    frame->SetMenuBar(menu_bar);
    frame->CreateStatusLine();
//  wxPanel  panel = new wxPanel(frame, 0, 0, 400, 250);

//    panel->SetLabelPosition(wxHORIZONTAL) ;
    frame->Show(TRUE);

    return frame;
}

MyFrame::MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h):
    wxFrame(frame, title, x, y, w, h)
{}

void MyFrame::OnMenuCommand(int id)
{
    switch (id) {
        case TEST_QUIT:
            delete this;
            return;
        case POPUP_15:
            nItems = 15;
            break;
        case POPUP_20:
            nItems = 20;
            break;
        case POPUP_95:
            nItems = 95;
            break;
        case POPUP_10000:
            nItems = 10000;
            break;
        default:
            return;
    }
    wxPopupMenu * menu = new wxPopupMenu(frame, (wxFunction)&SelectionProc,
           100, 200, -1, -1, "MyPopupTest", Item, NULL,
           OnExit, NULL, 20);
    int selection = menu->Execute();
    delete menu;
}

