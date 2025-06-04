
#include "wx_defs.h"
#include "wx_dialg.h"
#include "wx_utils.h"
#include "wx_frame.h"
#include "wx_panel.h"
#include "wx_item.h"
#include "wx_buttn.h"
#include "wx_check.h"
#include "wx_txt.h"
#include "wx_rbox.h"

// If not MS C++, don't include wx.h: we'll just include
// the minimum set of files.
// If MS C++, we'll use a precompiled header instead.
#if !defined(_MSC_VER) && !defined(wx_wxh)
#define wx_wxh
#endif

#include "wx.h"

#include "srchdlg.h"

                    
#define MESSAGE    "What:"
#define CAPTION    "Search"
                       
extern void wxSplitMessage(Const char *message, wxList *messageList, wxPanel *panel);
extern void wxCentreMessage(wxList *messageList);
                       
class wxSearchDialogBox: public wxDialogBox
{
public:
   wxText *textItem;
    wxRadioBox * choice;
    wxCheckBox * caseCheck;
    wxButton * search;
    static char *textAnswer;
    int buttonPressed;                  
    Bool& SearchDown;
    Bool& IgnoreCase;                                  
          
    wxSearchDialogBox(Bool& ignoreCase, Bool& searchDown, char * default_value,
           wxFrame *parent, char *caption, Bool modal, int x, int y, 
                       int w, int h, long type);
   Bool OnClose(void)
   {
           return TRUE;
   } 
   Bool Execute(void)
   {
       Show(TRUE);
       return buttonPressed == wxOK;
   }
};


char *wxSearchDialogBox::textAnswer = NULL;

void wxSearchDialogSearchButton(wxButton& but, wxEvent& event)
{
  wxPanel *panel = (wxPanel *)but.GetParent();
  while (!panel->IsKindOf(CLASSINFO(wxDialogBox)))
    panel = (wxPanel*) panel->GetParent() ;

  wxSearchDialogBox *dialog = (wxSearchDialogBox *)panel;
  if (dialog->textItem)
  {
    if (dialog->textAnswer)
      delete[] dialog->textAnswer;
    dialog->textAnswer = copystring(dialog->textItem->GetValue());
  }

  dialog->IgnoreCase = dialog->caseCheck->GetValue();
  dialog->SearchDown = dialog->choice->GetSelection();
  dialog->buttonPressed = wxOK;
  dialog->Show(FALSE);
}
  
void wxSearchDialogCancelButton(wxButton& but, wxEvent& event)
{
  wxSearchDialogBox *dialog = (wxSearchDialogBox *)but.GetParent();
  // There is a possibility that buttons belong to a sub panel.
  // So, we must search the dialog.
  while (!dialog->IsKindOf(CLASSINFO(wxDialogBox)))
    dialog = (wxSearchDialogBox*) ((wxPanel*)dialog)->GetParent() ;

  dialog->Show(FALSE);
}

void text_proc(wxText& but, wxCommandEvent& event)
{
  wxSearchDialogBox *dialog = (wxSearchDialogBox *)but.GetParent();
  while (!dialog->IsKindOf(CLASSINFO(wxDialogBox)))
    dialog = (wxSearchDialogBox*) ((wxPanel*)dialog)->GetParent() ; 
  dialog->search->Enable(*(dialog->textItem->GetValue()));  
}
                                                                                                                               

wxSearchDialogBox::wxSearchDialogBox(Bool& ignoreCase, Bool& searchDown, char *default_value,
           wxFrame *parent, char *caption, Bool modal, int x, int y, 
                       int w, int h, long type):
        IgnoreCase(ignoreCase), SearchDown(searchDown),                                                                 
       wxDialogBox(parent, caption, modal, x, y, w, h,
           wxCAPTION|wxSYSTEM_MENU|wxTHICK_FRAME)
{                                                                             
   textItem = NULL;   
    buttonPressed = wxCANCEL;      

#if (!defined(wx_xview) && USE_PANEL_IN_PANEL)
   wxPanel *panel = new wxPanel(this) ;
#else
   // Until sub panels work in XView mode
   wxPanel *panel = this ;
#endif    
//     panel->label_position = wxVERTICAL;
   wxList messageList;
   wxSplitMessage(MESSAGE, &messageList, this);

   NewLine();

   textItem = new wxText(this, (wxFunction)&text_proc, NULL, default_value, -1, -1, 320);
   NewLine();

   char *choice_strings[2];
   choice_strings[0] = "Up";
    choice_strings[1] = "Down";
          
    choice = new wxRadioBox(panel, NULL, "Direction",         
                                 -1, -1, -1, -1, 2, choice_strings);                       
    choice->SetSelection(searchDown);                                                     
                          
    caseCheck = new wxCheckBox(panel, NULL, "Ignore Case");
    caseCheck->SetValue(ignoreCase);                       
                          
    NewLine();                                                     
   search = new wxButton(panel, (wxFunction)&wxSearchDialogSearchButton, "Search");
   (void)new wxButton(panel, (wxFunction)&wxSearchDialogCancelButton, "Cancel");

   search->SetDefault();                 
   search->Enable(*(this->textItem->GetValue()));  
   textItem->SetFocus();
   

#if (!defined(wx_xview) && USE_PANEL_IN_PANEL)
   panel->Fit() ;
#endif
    Fit();
   if ((x < 0) && (y < 0))
       Centre(wxBOTH);
   else if (x < 0)
       Centre(wxHORIZONTAL);
   else if (y < 0)
       Centre(wxVERTICAL);
}

char *wxGetSearchString(Bool& searchDown, Bool& ignoreCase,
           char *default_value,  wxFrame *parent, int x, int y)            
{                                                                              
// Do not use ABSOLUTE until subpanels work with this mode!

#if USE_PANEL_IN_PANEL
   wxSearchDialogBox *dialog = new wxSearchDialogBox(ignoreCase, searchDown, default_value,
               parent, CAPTION, TRUE, 
               x, y, -1, -1, DEFAULT_DIALOG_STYLE);
#else
   wxSearchDialogBox *dialog = new wxSearchDialogBox(ignoreCase, searchDown, default_value,
               parent, CAPTION, TRUE, 
               x, y, 500, 160, 0);
#endif
   if (dialog->Execute()) {
       delete dialog;
       return wxSearchDialogBox::textAnswer;      
    }  
   else { 
       delete dialog;
       return NULL;
    }  
}
        
