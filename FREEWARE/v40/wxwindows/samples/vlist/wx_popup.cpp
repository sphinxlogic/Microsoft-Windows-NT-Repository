/*
 * File:	wx_popup.cc
 * Purpose: Popup menu implementation
 * Author:  Sergey Krasnov
 * Created: 1995
 * Updated:
 * Copyright:
 */

#ifdef __GNUG__
#pragma implementation
#pragma interface
#endif
#include "wx_defs.h"
#include "wx_buttn.h"
#include "wx_utils.h"

// If not MS C++, don't include wx.h: we'll just include
// the minimum set of files.
// If MS C++, we'll use a precompiled header instead.
#if !defined(_MSC_VER) && !defined(wx_wxh)
#define wx_wxh
#endif

#include "wx.h"
#include "wx_cmdlg.h"

#include "wx_popup.h"
#include "srchdlg.h"

#define SEARCH       0
#define SEARCH_NEXT   1
#define SEARCH_PREV  2

void wxPopupMenuOkButton(wxButton& but, wxEvent& event);
void wxPopupMenuCancelButton(wxButton& but, wxEvent& event);
void wxPopupMenuSearchButton(wxButton& but, wxEvent& event);
void wxPopupMenuSearchNextButton(wxButton& but, wxEvent& event);
void wxPopupMenuSearchPrevButton(wxButton& but, wxEvent& event);


void wxPopupMenuOkButton(wxButton& but, wxEvent& event)
{
   wxPanel *panel = (wxPanel *)but.GetParent();
   while (!panel->IsKindOf(CLASSINFO(wxDialogBox)))
           panel = (wxPanel*) panel->GetParent() ;

   wxPopupMenu *dialog = (wxPopupMenu *)panel;
   dialog->SelectOk();
   dialog->Show(FALSE);
}

void wxPopupMenuCancelButton(wxButton& but, wxEvent& event)
{
    wxPanel *panel = (wxPanel *)but.GetParent();
    while (!panel->IsKindOf(CLASSINFO(wxDialogBox)))
            panel = (wxPanel*) panel->GetParent() ;

    wxPopupMenu *dialog = (wxPopupMenu *)panel;
    dialog->Show(FALSE);
}

void wxPopupMenuSearchButton(wxButton& but, wxEvent& event)
{
   wxPanel *panel = (wxPanel *)but.GetParent();
   while (!panel->IsKindOf(CLASSINFO(wxDialogBox)))
           panel = (wxPanel*) panel->GetParent() ;

   wxPopupMenu *dialog = (wxPopupMenu *)panel;
   dialog->OnSearch(SEARCH);
}

void wxPopupMenuSearchNextButton(wxButton& but, wxEvent& event)
{
   wxPanel *panel = (wxPanel *)but.GetParent();
   while (!panel->IsKindOf(CLASSINFO(wxDialogBox)))
           panel = (wxPanel*) panel->GetParent() ;

   wxPopupMenu *dialog = (wxPopupMenu *)panel;
   dialog->OnSearch(SEARCH_NEXT);
}

void wxPopupMenuSearchPrevButton(wxButton& but, wxEvent& event)
{
   wxPanel *panel = (wxPanel *)but.GetParent();
   while (!panel->IsKindOf(CLASSINFO(wxDialogBox)))
           panel = (wxPanel*) panel->GetParent() ;

   wxPopupMenu *dialog = (wxPopupMenu *)panel;
   dialog->OnSearch(SEARCH_PREV);
}

void wxPopupMenuProc(wxVirtListBox& list, wxCommandEvent& event)
{
    wxPopupMenu * popup = (wxPopupMenu *)list.wxWindow::GetParent();
    popup->ProcessCommand(event);
}

wxPopupMenu::wxPopupMenu(wxFrame * parent, wxFunction func,
                int x, int y, int width, int height,
                char * Title,
                CPFPCPI get_optx, char *garg,
                FPCPPI execf, char *earg[],
                int nrows, long style):
            GetOptx(get_optx), gArg(garg),
            ExecF(execf), eArg(earg),
            Style(style), nRows(nrows),
            Width(width), Height(height),
            wxDialogBox(parent, Title, TRUE, x, y, width, height,
                style)
{
    SearchStr[0] = 0;
    SearchDown = TRUE;
    IgnoreCase = FALSE;
    Callback(func);
    vListBox = NULL;
}

wxPopupMenu::~wxPopupMenu(void)
{
   	if (vListBox)
   		delete vListBox;
}

int wxPopupMenu::Execute()
{
#define DEF_LISTBOX_WIDTH  250

   int dlg_w, dlg_h;
   int listbox_w, listbox_h;
   int button_x, button_y, button_w, button_h;

   if (GetOptx(gArg, 1) == NULL) {
       wxMessageBox("No options selected");
       return wxERROR;
   }

   GetClientSize(&dlg_w, &dlg_h);

   if (Width < 0 || Height < 0)
   {
       if (Width < 0)
           dlg_w = 800;
       if (Height < 0) {
           if (nRows > -1)
               dlg_h = 1280;
           else
               dlg_h = 340;
       }
       SetClientSize(dlg_w, dlg_h);
   }

   /* Estimating the size of button */
   search = new wxButton(this, (wxFunction)&wxPopupMenuSearchButton, "Search");
   search->GetSize(&button_w, &button_h);

   if (dlg_h < 5 * button_w + 6 * PANEL_VSPACING) {
       dlg_h = 5 * button_w + 6 * PANEL_VSPACING;
       SetClientSize(dlg_w, dlg_h);
    }

    /* Setting the width of dialog for Width = -1 */
    if (Width < 0) {
           dlg_w = DEF_LISTBOX_WIDTH + 3 * PANEL_HSPACING  + button_w;
       SetClientSize(dlg_w, dlg_h);
    }

   listbox_w = dlg_w - 3 * PANEL_HSPACING - button_w;
   listbox_h = dlg_h - 2 * PANEL_VSPACING;


   vListBox = new wxVirtListBox(this, (wxFunction)&wxPopupMenuProc, (CPFPCPI)GetOptx, gArg,
                       0 , nRows, wxSINGLE|wxNEEDED_SB,
                       PANEL_HSPACING, PANEL_VSPACING,
                       listbox_w, listbox_h);

   vListBox->GetSize(&listbox_w, &listbox_h);
   button_x = listbox_w + 2 * PANEL_HSPACING;
   button_y = PANEL_VSPACING;

   wxButton *ok = new wxButton(this, (wxFunction)&wxPopupMenuOkButton, "OK",
           button_x, button_y, button_w, button_h);
   button_y += button_h + PANEL_VSPACING;
   (void)new wxButton(this, (wxFunction)&wxPopupMenuCancelButton, "Cancel",
        button_x, button_y, button_w, button_h);
   button_y += button_h + PANEL_VSPACING;
   search->SetSize(button_x, button_y, button_w, button_h);
   button_y += button_h + PANEL_VSPACING;
    next = new wxButton(this, (wxFunction)&wxPopupMenuSearchNextButton, "Next",
        button_x, button_y, button_w, button_h);
   button_y += button_h + PANEL_VSPACING;
    prev = new wxButton(this, (wxFunction)&wxPopupMenuSearchPrevButton, "Prev",
        button_x, button_y, button_w, button_h);

   ok->SetDefault();
    next->Enable(FALSE);
    prev->Enable(FALSE);

   if (Height < 0 || Width < 0)
       Fit();

   Show(TRUE);
   return GetSelection();
}


void wxPopupMenu::SelectOk()
{
   if (ExecF && (GetSelection() != wxERROR))
       (*ExecF)(eArg, GetSelection());
}

void wxPopupMenu::SetSelection(int n)
{
   vListBox->SetSelection(n);
}

void wxPopupMenu::Fit(void) {
   int dlg_w, dlg_h, w, h;
   int listbox_w, listbox_h;
   int button_x, button_y, button_w, button_h;

   vListBox->GetSize(&listbox_w, &listbox_h);
   prev->GetSize(&button_w, &button_h);
   prev->GetPosition(&button_x, &button_y);

   dlg_w = button_x + button_w + PANEL_HSPACING;
   dlg_h = wxMax(PANEL_VSPACING + listbox_h, button_y + button_h) + PANEL_VSPACING;
    GetClientSize(&w, &h);
    if (w != dlg_w || h != dlg_h)
       SetClientSize(dlg_w, dlg_h);
}

void wxPopupMenu::OnSearch(int SearchType)
{

    if (vListBox->GetSelection() == wxERROR)
           return;
    int Selection = Search(SearchType);
    if (Selection != wxERROR) {
       if (Selection < vListBox->GetViewStart() ||
               Selection >= vListBox->GetViewStart() + vListBox->GetRowsNumber())
           vListBox->SetViewStart(Selection);
       vListBox->SetSelection(Selection);
    }
    next->Enable(SearchStr[0]);
    prev->Enable(SearchStr[0]);
}


int wxPopupMenu::Search(int SearchType)
{
   if (SearchType == SEARCH) {
       int x, y;
	   search->GetPosition(&x, &y);
	   ClientToScreen(&x, &y);
       char * _SearchStr = wxGetSearchString(SearchDown, IgnoreCase,
                       SearchStr, NULL, x, y);
       if (!_SearchStr || !*_SearchStr)
           return wxERROR;
       else
           strcpy(SearchStr, _SearchStr);
   }

   int ret =  vListBox->SearchItem(SearchStr,
       (SearchType == SEARCH_PREV) ? !SearchDown : SearchDown, !IgnoreCase);

   if (ret == wxERROR)
       ::wxBell();
   return ret;
}
void wxPopupMenu::ProcessCommand (wxCommandEvent & event)
{
    if (wxNotifyEvent (event, TRUE))
        return;

    wxFunction fun = callback;
    if (fun)
        (void) (*(fun)) (*this, event);

    wxNotifyEvent (event, FALSE);
}

