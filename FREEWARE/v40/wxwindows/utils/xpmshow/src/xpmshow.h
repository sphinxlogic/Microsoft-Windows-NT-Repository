/*
 * File:	xpmshow.h
 * Purpose:	XPM shower/converter
 * Author:	Julian Smart
 * Created:	1995
 * Updated:	
 * Copyright:	(c) 1995, AIAI, University of Edinburgh
 */

#ifdef __GNUG__
#pragma interface
#endif

// Define a new application type
class MyApp: public wxApp
{ public:
    wxFrame *OnInit(void);
};

#define XPMSHOW_TYPE_BMP    1
#define XPMSHOW_TYPE_XPM    2

// Structure representing an XPM or BMP bitmap on disk
class BitmapStruc: public wxObject
{
 public:
   int bitmapType;
   char *filename;
   wxBitmap *bitmap;
   BitmapStruc(int typ, char *bitmapFilename, wxBitmap *theBitmap);
   ~BitmapStruc(void);
};

// Define a new frame type
class MyFrame: public wxFrame
{ public:
   wxPanel *panel;
   wxListBox *listbox;
   wxMessage *bitmapMessage;
   wxList bitmapList;
   char *currentDir;
   
   MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h, long style);
   void OnMenuCommand(int id);
   Bool OnClose(void);

   void ScanDirectory(char *dir);
   void ClearFiles(void);
   BitmapStruc *CreateBitmapStruc(int typ, char *filename);
   void ShowFiles(void);
   void Convert(void);
   void RemoveFile(char *filename);
};

void XPMConvertFile(char *in, char *out);

// IDs for the menu quit command
#define XPMSHOW_SCAN    1
#define XPMSHOW_TEST    2
#define XPMSHOW_QUIT    10

void XPMShowConvert(wxButton& but, wxCommandEvent& ev);
void XPMBitmapList(wxListBox& lbox, wxCommandEvent& ev);

