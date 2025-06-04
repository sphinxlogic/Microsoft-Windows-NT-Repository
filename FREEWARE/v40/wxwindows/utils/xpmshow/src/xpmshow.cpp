/*
 * File:	xpmshow.cc
 * Purpose:	XPM shower/converter
 * Author:	Julian Smart
 * Created:	1995
 * Updated:	
 * Copyright:	(c) 1995, AIAI, University of Edinburgh
 */

static const char sccsid[] = "%W% %G%";

#ifdef __GNUG__
#pragma implementation
#pragma interface
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx.h"
#endif

#include "xpmshow.h"

#include "wxxpm.xpm"
wxBitmap *theDummyBitmap = NULL;

// Frame
MyFrame *myFrame = NULL;

// A macro needed for some compilers (AIX) that need 'main' to be defined
// in the application itself.
IMPLEMENT_WXWIN_MAIN

// This statement initializes the whole application and calls OnInit
MyApp myApp;

// `Main program' equivalent, creating windows and returning main app frame
wxFrame *MyApp::OnInit(void)
{
  int n = 1;
  char *inputFile = NULL;
  char *outputFile = NULL;
  
  // Read input/output files, if any
  if (argc > 1)
  {
    if (argv[1][0] != '-')
    {
      inputFile = copystring(argv[1]);
      n ++;

      if (argc > 2)
      {
        if (argv[2][0] != '-')
        {
          outputFile = copystring(argv[2]);
          n ++;
        }
      }
    }
  }

  if (inputFile)
  {
    XPMConvertFile(inputFile, outputFile);
    return NULL;
  }


  // Create the main frame window
  MyFrame *frame = new MyFrame(NULL, "XPM Show", 50, 50, 600, 600, wxSDI | wxDEFAULT_FRAME);
  myFrame = frame;

  // Make a menubar
  wxMenu *file_menu = new wxMenu;

  file_menu->Append(XPMSHOW_SCAN, "&Scan for bitmap files");
//  file_menu->Append(XPMSHOW_TEST, "&Test");
  file_menu->Append(XPMSHOW_QUIT, "E&xit");
  wxMenuBar *menu_bar = new wxMenuBar;
  menu_bar->Append(file_menu, "&File");
  frame->SetMenuBar(menu_bar);

  theDummyBitmap = new wxBitmap(wxxpm_xpm);

  frame->panel = new wxPanel(frame, 0, 0, 500, 500);
  frame->panel->SetLabelPosition(wxVERTICAL);
  frame->listbox = new wxListBox(frame->panel, (wxFunction)XPMBitmapList, "Bitmaps", wxSINGLE, -1, -1, 250, 150);
  frame->bitmapMessage = new wxMessage(frame->panel, theDummyBitmap);
  frame->panel->NewLine();
  (void)new wxButton(frame->panel, (wxFunction)XPMShowConvert, "Convert");

  // Give it an icon
#ifdef wx_msw
  frame->SetIcon(new wxIcon("xpmshow_icn"));
#endif
#ifdef wx_x
//  frame->SetIcon(new wxIcon(wxxpm_xpm));
#endif

  char buf[400];
  (void)wxGetWorkingDirectory(buf, 400);
  myFrame->ScanDirectory(buf);
  myFrame->ShowFiles();

  frame->panel->Fit();
  frame->Fit();
  
  // Show the frame
  frame->Show(TRUE);

  // Return the main frame window
  return frame;
}

// My frame constructor
MyFrame::MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h, long style):
  wxFrame(frame, title, x, y, w, h, style)
{
  currentDir = NULL;
  panel = NULL;
  listbox = NULL;
  bitmapMessage = NULL;
}

Bool MyFrame::OnClose(void)
{
  return TRUE;
}

// Intercept menu commands
void MyFrame::OnMenuCommand(int id)
{
  switch (id)
  {
    case XPMSHOW_QUIT:
    {
      if (myFrame->OnClose())
        delete this;
      break;
    }
    case XPMSHOW_SCAN:
    {
      char *f = wxFileSelector("Scan a directory", NULL, NULL, NULL, "*.*");
      if (f)
      {
        char *p = copystring(PathOnly(f));
        ScanDirectory(p);
        delete[] p;
        ShowFiles();
        bitmapMessage->SetLabel(theDummyBitmap);
      }
    }
    case XPMSHOW_TEST:
    {
//      XpmWriteFileFromXpmImage(NULL, NULL, NULL);
      break;
    }
    break;
  }
}

void MyFrame::ScanDirectory(char *dir)
{
  if (currentDir)
    delete[] currentDir;
  currentDir = copystring(dir);
  
  wxBeginBusyCursor();
  ClearFiles();

  char buf[400];
  strcpy(buf, dir);
#ifdef wx_msw
  strcat(buf, "\\");
#else
  strcat(buf, "/");
#endif
  strcat(buf, "*.bmp");

  char *f;
  if (f = wxFindFirstFile(buf, 0))
  {
    BitmapStruc *bm = CreateBitmapStruc(XPMSHOW_TYPE_BMP, f);
    if (bm)
      bitmapList.Append(bm);
    while (f = wxFindNextFile())
    {
      BitmapStruc *bm = CreateBitmapStruc(XPMSHOW_TYPE_BMP, f);
      if (bm)
        bitmapList.Append(bm);
    }
  }
  // Now do xpm files
  strcpy(buf, dir);
#ifdef wx_msw
  strcat(buf, "\\");
#else
  strcat(buf, "/");
#endif
  strcat(buf, "*.xpm");
  if (f = wxFindFirstFile(buf, 0))
  {
    BitmapStruc *bm = CreateBitmapStruc(XPMSHOW_TYPE_XPM, f);
    if (bm)
      bitmapList.Append(bm);
    while (f = wxFindNextFile())
    {
      BitmapStruc *bm = CreateBitmapStruc(XPMSHOW_TYPE_XPM, f);
      if (bm)
        bitmapList.Append(bm);
    }
  }
  wxEndBusyCursor();
}

void MyFrame::ClearFiles(void)
{
  wxNode *node = bitmapList.First();
  while (node)
  {
    BitmapStruc *struc = (BitmapStruc *)node->Data();
    delete struc;
    delete node;
    node = bitmapList.First();
  }
}

BitmapStruc *MyFrame::CreateBitmapStruc(int typ, char *filename)
{
  wxBitmap *bm = NULL;
  if (typ == XPMSHOW_TYPE_BMP)
  {
    bm = new wxBitmap(filename, wxBITMAP_TYPE_BMP);
  }
  else if (typ == XPMSHOW_TYPE_XPM)
  {
    bm = new wxBitmap(filename, wxBITMAP_TYPE_XPM);
  }

  if (bm && bm->Ok())
  {
    BitmapStruc *struc = new BitmapStruc(typ, filename, bm);
    return struc;
  }
  else
  {
    if (bm)
      delete bm;
    return NULL;
  }
}

void MyFrame::ShowFiles(void)
{
  listbox->Clear();
  wxNode *node = bitmapList.First();
  while (node)
  {
    BitmapStruc *struc = (BitmapStruc *)node->Data();
    listbox->Append(wxFileNameFromPath(struc->filename), (char *)struc);
    node = node->Next();
  }
}

void MyFrame::Convert(void)
{
  int sel = listbox->GetSelection();
  if (sel > -1)
  {
    BitmapStruc *struc = (BitmapStruc *)listbox->wxListBox::GetClientData(sel);
    char buf[400];
    strcpy(buf, struc->filename);
    wxStripExtension(buf);
    
    if (struc->bitmapType == XPMSHOW_TYPE_XPM)
    {
#ifdef wx_x
      wxMessageBox("XPM->BMP not implemented under X, sorry.", "Message", wxOK);
      return;
#endif
#ifdef wx_msw
      strcat(buf, ".BMP");
#else
      strcat(buf, ".bmp");
#endif
      if (wxFileExists(buf))
      {
        if (wxNO == wxMessageBox("Overrite existing BMP file?", "Message", wxYES_NO))
                return;
      }    
      wxBeginBusyCursor();
      Bool success = struc->bitmap->SaveFile(buf, wxBITMAP_TYPE_BMP);
      wxEndBusyCursor();
      if (!success)
        wxMessageBox("Could not write bitmap file.", "Message", wxOK);
      else
      {
        // Add to listbox
        BitmapStruc *bm = CreateBitmapStruc(XPMSHOW_TYPE_BMP, buf);
        if (bm)
        {
          RemoveFile(buf);
          bitmapList.Append(bm);
          listbox->Append(wxFileNameFromPath(bm->filename), (char *)bm);
        }
      }
    }
    else
    {
#ifdef wx_msw
      strcat(buf, ".XPM");
#else
      strcat(buf, ".xpm");
#endif
      if (wxFileExists(buf))
      {
        if (wxNO == wxMessageBox("Overrite existing XPM file?", "Message", wxYES_NO))
          return;
      }
      wxBeginBusyCursor();
      Bool success = struc->bitmap->SaveFile(buf, wxBITMAP_TYPE_XPM);
      wxEndBusyCursor();
      if (!success)
        wxMessageBox("Could not write XPM file.", "Message", wxOK);
      else
      {
        // Add to listbox
        BitmapStruc *bm = CreateBitmapStruc(XPMSHOW_TYPE_XPM, buf);
        if (bm)
        {
          RemoveFile(buf);
          bitmapList.Append(bm);
          listbox->Append(wxFileNameFromPath(bm->filename), (char *)bm);
        }
      }
    }
  } 
}

void MyFrame::RemoveFile(char *filename)
{
  BitmapStruc *toDelete = NULL;
  
  wxNode *node = bitmapList.First();
  while (node)
  {
    BitmapStruc *struc = (BitmapStruc *)node->Data();
    if (strcmp(struc->filename, filename) == 0)
    {
      toDelete = struc;
      delete struc;
      delete node;
      node = NULL;
    }
    else node = node->Next();
  }
  // Delete from listbox
  int id = 0;
  while (TRUE && (id < 999))
  {
    if (listbox->wxListBox::GetClientData(id) == (char *)toDelete)
    {
      listbox->Delete(id);
      return;
    }
    id ++;
  }
}

void XPMConvertFile(char *in, char *out)
{
  if (!wxFileExists(in))
  {
    wxMessageBox("Cannot open file.", "Warning", wxOK);
    return;
  }
  int len = strlen(in);
  if (len < 3)
    return;
    
  // Find out what type the file is
  char ext[4];
  ext[0] = wxToLower(in[len-3]);
  ext[1] = wxToLower(in[len-2]);
  ext[2] = wxToLower(in[len-1]);
  ext[3] = 0;
  if (strcmp(ext, "bmp") == 0)
  {
    if (!out)
    {
      char buf[400];
      strcpy(buf, in);
      wxStripExtension(buf);
      strcat(buf, ".xpm");
      out = buf;
    }
    wxBitmap *bitmap = new wxBitmap(in, wxBITMAP_TYPE_BMP);
    Bool success = FALSE;
    if (bitmap && bitmap->Ok())
    {
      Bool success = bitmap->SaveFile(out, wxBITMAP_TYPE_XPM);
      delete bitmap;
    }
  }
  else if (strcmp(ext, "xpm") == 0)
  {
#ifdef wx_x
    (void)wxMessageBox("XPM->BMP not implemented under X, sorry.", "Message", wxOK);
    return;
#endif
    if (!out)
    {
      char buf[400];
      strcpy(buf, in);
      wxStripExtension(in);
      strcat(buf, ".bmp");
      out = buf;
    }
    wxBitmap *bitmap = new wxBitmap(in, wxBITMAP_TYPE_XPM);
    if (bitmap && bitmap->Ok())
    {
      Bool success = bitmap->SaveFile(out, wxBITMAP_TYPE_BMP);
      delete bitmap;
    }
  }
  else
  {
    wxMessageBox("Unrecognised format.", "Message", wxOK);
  }
}


void XPMShowConvert(wxButton& but, wxCommandEvent& ev)
{
  myFrame->Convert();
}

void XPMBitmapList(wxListBox& lbox, wxCommandEvent& ev)
{
  int sel = myFrame->listbox->GetSelection();
  if (sel > -1)
  {
    BitmapStruc *struc = (BitmapStruc *)myFrame->listbox->wxListBox::GetClientData(sel);
    myFrame->bitmapMessage->SetLabel(struc->bitmap);
  }
}

BitmapStruc::BitmapStruc(int typ, char *bitmapFilename, wxBitmap *theBitmap)
{
  bitmapType = typ;
  filename = copystring(bitmapFilename);
  bitmap = theBitmap;
}

BitmapStruc::~BitmapStruc(void)
{
  delete[] filename;
  delete bitmap;
}
