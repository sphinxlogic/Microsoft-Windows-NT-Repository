/*
 * File:	wx_text.cc
 * Purpose:	wxTextWindow implementation
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	August 1994
 * RCS_ID:      $Id: wx_text.cc,v 1.1 1994/08/14 21:59:17 edz Exp $
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
	Last change:  JS   25 May 97   11:17 am
 */

/* static const char sccsid[] = "%W% %G%"; */

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx_setup.h"
#include "wx_list.h"
#include "wx_text.h"
#include "wx_panel.h"
#include "wx_gdi.h"
#include "wx_utils.h"
#include "wx_main.h"
#include "wx_mem.h"
#endif

#include "wx_clipb.h"
#include "wx_privt.h"

#if USE_IOSTREAMH
#include <iostream.h>
#else
#include <iostream>
#endif

#include <fstream.h>
#include <stdlib.h>

#ifndef GNUWIN32
#include <shellapi.h>
#endif

#if WIN95 && !defined(GNUWIN32)
#include <richedit.h>
#endif

#if CTL3D
#include "ctl3d.h"
#endif

#include <sys/types.h>
#include <sys/stat.h>
#if defined(__BORLANDC__) && !defined(WIN32)
#include <alloc.h>
#else
#ifndef GNUWIN32
#include <malloc.h>
#endif
#define farmalloc malloc
#define farfree free
#endif
#include <windowsx.h>

extern char wxCanvasClassName[];

#if (!EDITABLE_TEXT_WINDOW)
class wxTextWnd : public wxSubWnd
{
public:
  wxStringList lines;
  int no_lines;
  int current_line;

  int cxChar;
  int cyChar;

  int cxClient;
  int cyClient;

  wxTextWnd(wxWnd *parent, wxWindow *wx_win,
             int x, int y, int width, int height, DWORD style);
  ~wxTextWnd();

  void CalcNewSize(int x, int y);

  void OnCreate2(void);
  BOOL OnPaint(void);
  void OnSize(int x, int y, UINT);

  void WriteText(char *text);

};
#endif // EDITABLE_TEXT_WINDOW

wxTextWindow::wxTextWindow(void)
{
  file_name = NULL;
  wxWinType = wxTYPE_HWND;
  handle = NULL;
  oldTextProc = 0;
}

wxTextWindow::wxTextWindow(wxWindow *parent, int x, int y, int width, int height,
                           long style, Constdata char *name):
  wxbTextWindow(parent, x, y, width, height, style, name)

{
  Create(parent, x, y, width, height, style, name);
}

#if !defined(APIENTRY)	// NT defines APIENTRY, 3.x not
#define APIENTRY FAR PASCAL
#endif
#ifdef WIN32
#define _EXPORT /**/
#else
#define _EXPORT _export
typedef signed short int SHORT ;
#endif

#ifdef STRICT
static WNDPROC TextProc = NULL;
// static WNDPROC OldTextProc = NULL;
#define FIXWNDPROC      WNDPROC
#else
static FARPROC TextProc = NULL;
// static FARPROC OldTextProc = NULL;
#define FIXWNDPROC      FARPROC
#endif

#if __WATCOMC__ && defined(__WINDOWS_386__)
#define TEXTPROCINT short
#define WNDPROCCAST     (TEXTPROCINT (pascal*)())
#else
#define TEXTPROCINT int
#ifdef GNUWIN32
// #define WNDPROCCAST (long unsigned)
#define WNDPROCCAST (long int (*) (void*, unsigned int, unsigned int, long int))
#else
#define WNDPROCCAST
#endif     
#endif

wxList *wxTextWinList=NULL;
wxTextWindow *wxTextWinFromHandle(HWND hWnd)
{
  if (!wxTextWinList)
    return NULL;

  wxNode *node = wxTextWinList->Find((long)hWnd);
  if (!node)
    return NULL;
  return (wxTextWindow *)node->Data();
}
BOOL wxCDown,wxSDown;
WORD WX_CHAR,WX_KEYDOWN,WX_KEYUP;

void wxTextWindow::Synch()
{
	MSG current_msg;
	while(::PeekMessage(&current_msg,NULL,WX_KEYDOWN , WX_KEYUP, PM_REMOVE))
	{
		::TranslateMessage(&current_msg);
		::DispatchMessage(&current_msg);
	}
}
void wxTextWindow::Char(WPARAM wParam,BOOL isASCII)
{
  	int id;
    	Bool tempControlDown = FALSE;
    	// If 1 -> 26, translate to CTRL plus a letter.
    	id = wParam;
	if (isASCII)
	{
    		/*if ((id > 0) && (id < 27))
    		{
	      		tempControlDown = TRUE;
      			id = id + 96;
    		} */
	}
	else if ((id = wxCharCodeMSWToWX(wParam)) == 0)
      		id = -1;
  	if (id > -1)
  	{
    		wxKeyEvent event(wxEVENT_TYPE_CHAR);
    		if (wxSDown)
      			event.shiftDown = TRUE;
    		if (wxCDown || tempControlDown)
      			event.controlDown = TRUE;
    		event.eventObject =(wxWindow *) this;
    		event.keyCode = id;
		event.x=event.y=(float)0;
		OnChar(event);
	}
    	return ;
}

LRESULT APIENTRY _EXPORT wxTextProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	wxTextWindow *wnd = wxTextWinFromHandle(hWnd);
	if (!wnd)
	  return FALSE;
	  
	if(message==WX_CHAR)
		return CallWindowProc(WNDPROCCAST wnd->oldTextProc, hWnd,WM_CHAR, wParam, lParam);
	if(message==WX_KEYDOWN)
		return CallWindowProc(WNDPROCCAST wnd->oldTextProc, hWnd,WM_KEYDOWN, wParam, lParam);
	if(message==WX_KEYUP)
		return CallWindowProc(WNDPROCCAST wnd->oldTextProc, hWnd,WM_KEYUP, wParam, lParam);
	if (wnd)
	{
		wnd->last_msg = message;
		wnd->last_wparam = wParam;
		wnd->last_lparam = lParam;
		switch(message)
		{
		case WM_KEYDOWN:
			{
			if (wParam == VK_SHIFT)
				wxSDown = TRUE;
			else if (wParam == VK_CONTROL)
				wxCDown = TRUE;
			// Avoid duplicate messages to OnChar
			else if ((wParam != VK_ESCAPE) && (wParam != VK_SPACE) &&
                                 (wParam != VK_RETURN) && (wParam != VK_BACK))
				{
				wnd->Char(wParam);
				}
			else break;
			return 0;
			}
		case WM_KEYUP:
		{
			if (wParam == VK_SHIFT)
				wxSDown = FALSE;
			else if (wParam == VK_CONTROL)
				wxCDown = FALSE;
			else break;
			return 0;
		}
		case WM_CHAR: // Always an ASCII character
		{
			wnd->Char(wParam,TRUE);
			return 0;
		}
#ifndef GNUWIN32
                case WM_DROPFILES:
                {
                  HANDLE hFilesInfo = (HANDLE)wParam;
                  POINT dropPoint;
                  DragQueryPoint(hFilesInfo, (LPPOINT) &dropPoint);

                  // Get the total number of files dropped
                  WORD gwFilesDropped = (WORD)DragQueryFile ((HDROP)hFilesInfo,
				   (UINT)-1,
                                   (LPSTR)0,
                                   (UINT)0);

                  char **files = new char *[gwFilesDropped];
                  int wIndex;
                  for (wIndex=0; wIndex < (int)gwFilesDropped; wIndex++)
                  {
                    DragQueryFile (hFilesInfo, wIndex, (LPSTR) wxBuffer, 1000);
                    files[wIndex] = copystring(wxBuffer);
                  }
                  DragFinish (hFilesInfo);
                  wnd->GetEventHandler()->OnDropFiles(gwFilesDropped, files, dropPoint.x, dropPoint.y);

                  int i;
                  for (i = 0; i < (int)gwFilesDropped; i++)
                    delete[] files[i];
                  delete[] files;
                  break;  
                }
#endif
		default:
		  return CallWindowProc(WNDPROCCAST wnd->oldTextProc, hWnd, message, wParam, lParam);
		}
	}
	return CallWindowProc(WNDPROCCAST wnd->oldTextProc, hWnd, message, wParam, lParam);
}

void wxTextWindow::OnChar(wxKeyEvent& event)
{
	BOOL CF,SF;
	HWND hWnd=GetHWND();
	WPARAM wParamUpDown=(WPARAM)event.keyCode;
	LPARAM lParamUpDown=(LPARAM)1;  // 1 key (no repeats)
	WPARAM wParamChar=(WPARAM)event.keyCode;
	LPARAM lParamChar=(LPARAM)1;  // 1 key (no repeats)
	Bool IsVirtual;
	int id= wxCharCodeWXToMSW((int)wParamUpDown,&IsVirtual);
	if(IsVirtual!=0)
	{
		wParamUpDown=id;
		wParamChar=id;
/*
		switch (id)
		{
                // Uncommenting these disables the DELETE key. 12/11/95
//		  case 46:
//		    wParamUpDown = VK_DECIMAL;
//		    break;
		  default:
		    break;
		}
//		lParam+=0x41000000;
*/
	}
	/*if(event.controlDown && wParam>96)	\\prawdopodobnie ktorys ze znakow <32
		wParam-=96;*/
	CF=SF=FALSE;
	if(event.controlDown !=wxCDown)
	{
		if(event.controlDown)
			PostMessage(hWnd,WX_KEYDOWN,VK_CONTROL,lParamUpDown);
		else
			PostMessage(hWnd,WX_KEYUP,VK_CONTROL,lParamUpDown);
		CF=TRUE;
	}
	if(event.shiftDown!=wxSDown)
	{
		if(event.shiftDown)
			PostMessage(hWnd,WX_KEYDOWN,VK_SHIFT,lParamUpDown);
		else
			PostMessage(hWnd,WX_KEYUP,VK_SHIFT,lParamUpDown);
		SF=TRUE;
	}
	if (IsVirtual)
  	  PostMessage(hWnd,WX_KEYDOWN,wParamUpDown,lParamUpDown);
	if(!IsVirtual)
          PostMessage(hWnd,WX_CHAR,wParamChar,lParamChar);
	if (IsVirtual)
          PostMessage(hWnd,WX_KEYUP,wParamUpDown,lParamUpDown);
	if(SF)
		if(!event.shiftDown)
			PostMessage(hWnd,WX_KEYDOWN,VK_SHIFT,lParamUpDown);
		else
			PostMessage(hWnd,WX_KEYUP,VK_SHIFT,lParamUpDown);
	if(CF)
		if(!event.controlDown)
			PostMessage(hWnd,WX_KEYDOWN,VK_CONTROL,lParamUpDown);
		else
			PostMessage(hWnd,WX_KEYUP,VK_CONTROL,lParamUpDown);
	//CallWindowProc((int (pascal*)())wnd->oldTextProc, hWnd, last_msg, last_wparam, last_lparam);
	return;
}

Bool wxTextWindow::Create(wxWindow *parent, int x, int y, int width, int height,
                           long style, Constdata char *name)
{
  SetName(name);
  window_parent = parent;
  file_name = NULL;
  insertionPoint = 0;
  windowStyle = style;
#if !EDITABLE_TEXT_WINDOW
  if (windowStyle & wxNATIVE_IMPL)
#else
  if (TRUE)
#endif
    wxWinType = wxTYPE_HWND;
  else
    wxWinType = wxTYPE_XWND;

  if (parent->IsKindOf(CLASSINFO(wxPanel)))
    ((wxPanel *)parent)->GetValidPosition(&x, &y);

#if !EDITABLE_TEXT_WINDOW
  if (windowStyle & wxNATIVE_IMPL)
#else
  if (TRUE)
#endif
  {
#if defined(WIN32) || WIN95
    HGLOBAL globalHandle = 0;
#else
    // Use global memory (1K initially but can grow). Normally the instance
    // handle is passed on window creation, the sole purpose being that
    // at offset 6 is a near pointer to the local heap. If this heap
    // pointer is zero then CreateWindow() thinks it needs to create
    // a new one and does so. By allocating some (zeroed) global
    // memory and passing to CreateWindow() it gets tricked into creating
    // a new heap for each edit control, up to nearly 64K...

    globalHandle = 0;

    if ((wxGetOsVersion() != wxWINDOWS_NT) && (wxGetOsVersion() != wxWIN95))
      globalHandle = GlobalAlloc(GHND, 1024);
#endif

    windows_id = (int)NewControlId();

    long msStyle = ES_MULTILINE | ES_WANTRETURN | // | ES_LEFT 
               WS_VISIBLE | WS_CHILD | WS_TABSTOP |
               WS_VSCROLL;
    if (windowStyle & wxBORDER)
      msStyle |= WS_BORDER;

    if (windowStyle & wxREADONLY)
      msStyle |= ES_READONLY;

    if (windowStyle & wxHSCROLL)
      msStyle |= (WS_HSCROLL | ES_AUTOHSCROLL) ;

#if WIN95 && !defined(GNUWIN32)
    HWND edit = CreateWindowEx(0, "RichEdit", 0,
#else
    HWND edit = CreateWindowEx(0, "EDIT", 0,
#endif
                 msStyle,
                 0, 0, 0, 0, parent->GetHWND(), (HMENU)windows_id,
                 globalHandle ? globalHandle : wxhInstance, NULL);

    if (edit == 0)
    {
      WXTRACE("wxTextWindow::Create: edit control could not be created!\n");
    }
    
    if (edit)
    {
      if (!wxTextWinList)
      {
        wxTextWinList=new wxList;
        WX_KEYDOWN=RegisterWindowMessage("WX_KEYDOWN");
        WX_CHAR=RegisterWindowMessage("WX_CHAR");
        WX_KEYUP=RegisterWindowMessage("WX_KEYUP");
        TextProc = (FIXWNDPROC) MakeProcInstance((FARPROC) wxTextProc,
    		                               globalHandle);
      }

/*
    if (OldTextProc == NULL) {
//      OldTextProc = (WNDPROC)GetWindowLong(edit,GWL_WNDPROC);
      TextProc = (WNDPROC) MakeProcInstance((FARPROC) wxTextProc,
    		                               globalHandle);
      wxTextWinList=new wxList;
      WX_KEYDOWN=RegisterWindowMessage("WX_KEYDOWN");
      WX_CHAR=RegisterWindowMessage("WX_CHAR");
      WX_KEYUP=RegisterWindowMessage("WX_KEYUP");
    }
*/
    oldTextProc = (FIXWNDPROC)GetWindowLong(edit,GWL_WNDPROC);

    // Defined in windowsx.h as a SetWindowLong.
    (void) SubclassWindow(edit, TextProc);
    }

    wxTextWinList->Append((long)edit, this);

    ms_handle = edit;

    // Bypass the default 32K limit (now 64K)
    SendMessage(edit, EM_LIMITTEXT, 0, 0);

    // Set tab stops every 4 (default is 8).. units in 1/4 characters!
    WORD wTabSpacing = 4 * 4;
    SendMessage((HWND)edit, EM_SETTABSTOPS, 1, (LPARAM)&wTabSpacing);

    // Initialize
    SetWindowText((HWND)edit, "");

#if CTL3D
    // We don't really want to make it a 3D control. It seems to do
    // something strange to scrolling if it's not a child of a panel:
    // you get horizontal lines.
//    Ctl3dSubclassCtl(edit);
#endif
    SetSize(x, y, width, height);
  }
#if !EDITABLE_TEXT_WINDOW
  else
  {
    DWORD ms_flags = WS_HSCROLL | WS_VSCROLL | WS_CHILD | WS_VISIBLE;
    if (style & wxBORDER)
      ms_flags |= WS_BORDER;
    handle = (char *)new wxTextWnd((wxWnd *)parent->handle, this, x, y, width, height, ms_flags);
  }
#endif

  if (parent) parent->AddChild(this);
  if (parent->IsKindOf(CLASSINFO(wxPanel)))
    ((wxPanel *)parent)->AdvanceCursor(this);

  return TRUE;
}

wxTextWindow::~wxTextWindow(void)
{
  if (wxTextWinList)
  {
    wxTextWinList->DeleteObject(this);
  }

  if (file_name)
    delete[] file_name;

  if (font)
    font->ReleaseResource();
}

void wxTextWindow::SetFont(wxFont *theFont)
{
  if (font)
    font->ReleaseResource();

  font = theFont;
  if (font)
    font->UseResource();

#if !EDITABLE_TEXT_WINDOW
  if (windowStyle & wxNATIVE_IMPL)
#else
  if (TRUE)
#endif
  {
    HWND hWnd = GetHWND();
    if (font)
    {
      font->RealizeResource();
      if (font->GetResourceHandle())
        SendMessage(hWnd,WM_SETFONT, (WPARAM)font->GetResourceHandle(),0L);
    }
    else
        SendMessage(hWnd,WM_SETFONT, (WPARAM)0,0L);
  }
}

Bool wxTextWindow::LoadFile(char *file)
{
  if (!file || !FileExists(file))
    return FALSE;

  if (file_name)
    delete[] file_name;

  file_name = copystring(file);

  Clear();

// ITA change - ios::nocreate is not defined in MSL, or ANSI draft standard
// for that matter
#if !defined(__MWERKS__)
  ifstream input( file, ios::nocreate | ios::in);
#else
  ifstream input;
  input.open( file, ios::in);
#endif

  if (!input.bad())
  {
#if !EDITABLE_TEXT_WINDOW
    if (windowStyle & wxNATIVE_IMPL)
#else
    if (TRUE)
#endif
    {
      // Previously a SETSEL/REPLACESEL call-pair were done to insert
      // line by line into the control. Apart from being very slow this
      // was limited to 32K of text by the external interface presenting
      // positions as signed shorts. Now load in one chunk...
      // Note use of 'farmalloc' as in Borland 3.1 'size_t' is 16-bits...

      struct stat stat_buf;
      if (stat(file, &stat_buf) < 0)
        return FALSE;
//      char *tmp_buffer = (char*)farmalloc(stat_buf.st_size+1);
      // This may need to be a bigger buffer than the file size suggests,
      // if it's a UNIX file. Give it an extra 1000 just in case.
      char *tmp_buffer = (char*)farmalloc((size_t)(stat_buf.st_size+1+1000));
      long no_lines = 0;
      long pos = 0;
      while (!input.eof() && input.peek() != EOF)
      {
        input.getline(wxBuffer, 500);
	int len = strlen(wxBuffer);
	wxBuffer[len] = 13;
	wxBuffer[len+1] = 10;
	wxBuffer[len+2] = 0;
	strcpy(tmp_buffer+pos, wxBuffer);
	pos += strlen(wxBuffer);
	no_lines++;
      }

//      SendMessage((HWND)ms_handle, WM_SETTEXT, 0, (LPARAM)tmp_buffer);
      SetWindowText((HWND)ms_handle, tmp_buffer);
      SendMessage((HWND)ms_handle, EM_SETMODIFY, FALSE, 0L);
      farfree(tmp_buffer);
    }
#if !EDITABLE_TEXT_WINDOW
    else
    {
      wxTextWnd *text_win = (wxTextWnd *)handle;
      while (!input.eof() && input.peek() != EOF)
      {
        input.getline(wxBuffer, 500);
        text_win->lines.Add(wxBuffer);
        text_win->no_lines ++;
      }
      if (text_win->no_lines > 0)
        text_win->current_line = text_win->no_lines - 1;
      else text_win->current_line = 0;

      RECT rect;
      GetClientRect(text_win->handle, &rect);
      text_win->OnSize(rect.right, rect.bottom, 0);
      InvalidateRgn(text_win->handle, NULL, TRUE);
      UpdateWindow(text_win->handle);
    }
#endif
    return TRUE;
  }
  return FALSE;
}

// If file is null, try saved file name first
// Returns TRUE if succeeds.
Bool wxTextWindow::SaveFile(char *file)
{
  if (!file)
    file = file_name;
  if (!file)
    return FALSE;
  if (file_name) delete[] file_name;
  file_name = copystring(file);

  ofstream output(file);

#if !EDITABLE_TEXT_WINDOW
  if (windowStyle & wxNATIVE_IMPL)
#else
  if (TRUE)
#endif
  {
    // This will only save 64K max
    unsigned long nbytes = SendMessage(ms_handle, WM_GETTEXTLENGTH, 0, 0);
    char *tmp_buffer = (char*)farmalloc((size_t)(nbytes+1));
    SendMessage(ms_handle, WM_GETTEXT, (WPARAM)(nbytes+1), (LPARAM)tmp_buffer);
    char *pstr = tmp_buffer;

    if (!output.bad())
    {
	// Convert \r\n to just \n
	while (*pstr)
	{
		if (*pstr != '\r')
			output << *pstr;
		pstr++;
	}
    }

    farfree(tmp_buffer);
    SendMessage((HWND)ms_handle, EM_SETMODIFY, FALSE, 0L);

    return TRUE;
  }
#if !EDITABLE_TEXT_WINDOW
  else
  {
    if (!output.bad())
    {
     wxTextWnd *text_win = (wxTextWnd *)handle;
     wxNode *node = text_win->lines.First();
     while (node)
     {
       char *s = (char *)node->Data();
       if (s)
         output << s << "\n";
       node = node->Next();
     }
     return TRUE;
    }
  }
#endif
  return FALSE;
}

void wxTextWindow::WriteText(char *text)
{
  Synch();
#if !EDITABLE_TEXT_WINDOW
  if (windowStyle & wxNATIVE_IMPL)
#else
  if (TRUE)
#endif
  {
/* Appears to cause a memory bug (ouch, there goes an afternoon.)
    // Covert \n to \r\n
    char *newtext = new char[strlen(text)*2];
    char *dst = newtext;
    while (*text)
    {
      if (*text == '\n')
        *dst++ = '\r';
      *dst++ = *text++;
    }
    *dst++ = 0;
    SendMessage((HWND)ms_handle, EM_REPLACESEL, 0, (LPARAM)newtext);
    delete [] newtext;
*/
    // Covert \n to \r\n
    int len = strlen(text);
    char *newtext = new char[(len*2)+1];
    int i = 0;
    int j = 0;
    while (i < len)
    {
      if (text[i] == '\n')
      {
        newtext[j] = '\r';
        j ++;
      }
      newtext[j] = text[i];
      i ++;
      j ++;
    }
    newtext[j] = 0;
    SendMessage((HWND)ms_handle, EM_REPLACESEL, 0, (LPARAM)newtext);
    delete[] newtext;
  }
#if !EDITABLE_TEXT_WINDOW
  else
  {
    char *the_text = copystring(text);  // Necessary in case text points to
                                        // wxBuffer

    wxTextWnd *text_win = (wxTextWnd *)handle;
    int len = strlen(the_text);

    Bool text_end = FALSE;

    int i = 0;
    while (!text_end)
    {
      int j = 0;
      Bool eol = FALSE;
      wxNode *current_node = text_win->lines.Nth(text_win->current_line);
      char *s = (char *)current_node->Data();
      int old_line_length = strlen(s);
      strcpy(wxBuffer, s);

      while (!eol && !text_end)
      {
        if (i == len)
        {
          wxBuffer[j+old_line_length] = 0;
          text_end = TRUE;
        }
        else
        {
          char ch = the_text[i];

          if (ch == '\n' || (j+old_line_length) > 490)
          {
            eol = TRUE;
            wxBuffer[j+old_line_length] = 0;
            if ((j + old_line_length) > 490)
            {
              i --; j --;
            }
          }
          else
          {
            wxBuffer[j+old_line_length] = ch;
          }
          i ++;
          j ++;
        }
      }
      delete[] s;
      current_node->SetData((wxObject *)copystring(wxBuffer));

      HDC dc = GetDC(text_win->handle);
      SetTextColor(dc, GetSysColor(COLOR_WINDOWTEXT));
      SetBkColor(dc, GetSysColor(COLOR_WINDOW));
      HFONT oldFont = 0;
      if (font)
      {
        font->RealizeResource();
        if (font->GetResourceHandle())
          oldFont = ::SelectObject(dc, font->GetResourceHandle());
      }

      int x = (text_win->cxChar) * (1 - text_win->xscroll_position);
      int y = (text_win->cyChar) * (text_win->current_line - text_win->yscroll_position);
      TextOut(dc, x, y, wxBuffer, strlen(wxBuffer));

      if (font && oldFont)
        ::SelectObject(dc, oldFont);
        
      ReleaseDC(text_win->handle, dc);

      if (eol)
      {
        text_win->current_line ++;
        text_win->no_lines ++;
        text_win->lines.Add("");

        RECT rect;
        GetClientRect(text_win->handle, &rect);
        text_win->CalcNewSize(rect.right, rect.bottom);

        if (y >= (rect.bottom - text_win->cyChar))
          text_win->OnVScroll(SB_BOTTOM, 0, NULL);

//        (void)wxYield();
      }
    }
    delete[] the_text;
  }
#endif
}

void wxTextWindow::SetSize(int x, int y, int w, int h, int sizeFlags)
{
  int currentX, currentY;
  GetPosition(&currentX, &currentY);
  if (x == -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    x = currentX;
  if (y == -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    y = currentY;

  int currentW,currentH;
  GetSize(&currentW, &currentH);
  if (w == -1)
    w = currentW ;
  if (h == -1)
    h = currentH ;

#if !EDITABLE_TEXT_WINDOW
  if (windowStyle & wxNATIVE_IMPL)
#else
  if (TRUE)
#endif
  {
    MoveWindow(ms_handle, x, y, w, h, (BOOL)TRUE);
  }
#if !EDITABLE_TEXT_WINDOW
  else
  {
    wxWnd *wnd = (wxWnd *)handle;
    if (wnd)
      MoveWindow(wnd->handle, x, y, w, h, (BOOL)TRUE);
  }
#endif
  GetEventHandler()->OnSize(w, h);
}

void wxTextWindow::Clear(void)
{
  Synch();
#if !EDITABLE_TEXT_WINDOW
  if (windowStyle & wxNATIVE_IMPL)
#else
  if (TRUE)
#endif
  {
//    SendMessage((HWND)ms_handle, WM_SETTEXT, 0, (LPARAM)"");
    SetWindowText((HWND)ms_handle, "");
  }
#if !EDITABLE_TEXT_WINDOW
  else
  {
    wxTextWnd *text_win = (wxTextWnd *)handle;
    wxNode *node = text_win->lines.First();
    while (node)
    {
      char *s = (char *)node->Data();
      delete[] s;
      delete node;
      node = text_win->lines.First();
    }
    text_win->lines.Add("");
    text_win->no_lines = 1;
    text_win->current_line = 0;

    RECT rect;
    GetClientRect(text_win->handle, &rect);
    text_win->OnSize(rect.right, rect.bottom, 0);
    InvalidateRgn(text_win->handle, NULL, TRUE);
  }
#endif
}

Bool wxTextWindow::Modified(void)
{
  Synch();
#if !EDITABLE_TEXT_WINDOW
  if (windowStyle & wxNATIVE_IMPL)
#else
  if (TRUE)
#endif
    return (Bool)SendMessage((HWND)ms_handle, EM_GETMODIFY, 0, 0);
#if !EDITABLE_TEXT_WINDOW
  else
    return FALSE;
#endif
}

// Makes 'unmodified'
void wxTextWindow::DiscardEdits(void)
{
#if !EDITABLE_TEXT_WINDOW
  if (windowStyle & wxNATIVE_IMPL)
#else
  if (TRUE)
#endif
  {
    SendMessage((HWND)ms_handle, EM_SETMODIFY, FALSE, 0L);
  }
}

char *wxTextWindow::GetContents(void)
{
  Synch();
#if !EDITABLE_TEXT_WINDOW
  if (windowStyle & wxNATIVE_IMPL)
#else
  if (TRUE)
#endif
  {
    int length = GetWindowTextLength((HWND)ms_handle);
    char *s = new char[length+1];
    GetWindowText((HWND)ms_handle, s, length+1);
    return s;
  }
#if !EDITABLE_TEXT_WINDOW
  else
  {
    wxTextWnd *text_win = (wxTextWnd *)handle;
    // Count size of buffer required
    int i = 0;
    wxNode *node = text_win->lines.First();
    while (node)
    {
      char *s = (char *)node->Data();
      i += strlen(s) + 1; // Add one for a newline
      node = node->Next();
    }
    char *buf = new char[i+1];
    i = 0;
    node = text_win->lines.First();
    while (node)
    {
      char *s = (char *)node->Data();
      int len = strlen(s);
      int j;
      for (j = 0; j < len; j++)
      {
        buf[i] = s[j];
        i ++;
      }
      buf[i] = '\n';
      i ++;
    
      node = node->Next();
    }
    buf[i] = 0;
  
    return buf;
  }
#endif
}

/*
 * Some of the following functions are yet to be implemented
 *
 */
 
int wxTextWindow::GetNumberOfLines(void)
{
  Synch();
#if !EDITABLE_TEXT_WINDOW
  if (windowStyle & wxNATIVE_IMPL)
#else
  if (TRUE)
#endif
    return (int)SendMessage((HWND)ms_handle, EM_GETLINECOUNT, (WPARAM)0, (LPARAM)0);
#if !EDITABLE_TEXT_WINDOW
  else
  {
    wxTextWnd *text_win = (wxTextWnd *)handle;
    return text_win->lines.Number();
  }
#endif
}

void wxTextWindow::SetInsertionPoint(long pos)
{
  Synch();
#if !EDITABLE_TEXT_WINDOW
  if (windowStyle & wxNATIVE_IMPL)
#else
  if (TRUE)
#endif
  {
    HWND hWnd = GetHWND();
#if WIN95 && !defined(GNUWIN32)
    CHARRANGE range;
    range.cpMin = pos;
    range.cpMax = pos;
    SendMessage(hWnd, EM_EXSETSEL, 0, (LPARAM) &range);
    SendMessage(hWnd, EM_SCROLLCARET, (WPARAM)0, (LPARAM)0);
#else
#ifdef WIN32
    SendMessage(hWnd, EM_SETSEL, pos, pos);
    SendMessage(hWnd, EM_SCROLLCARET, (WPARAM)0, (LPARAM)0);
#else
    SendMessage(hWnd, EM_SETSEL, 0, MAKELPARAM(pos, pos));
#endif
#endif
    char *nothing = "";
    SendMessage(hWnd, EM_REPLACESEL, 0, (LPARAM)nothing);
  }
  insertionPoint = pos;
}

void wxTextWindow::SetInsertionPointEnd(void)
{
  long pos = GetLastPosition();
  SetInsertionPoint(pos);
}

long wxTextWindow::GetInsertionPoint(void)
{
  Synch();
#if !EDITABLE_TEXT_WINDOW
  if (windowStyle & wxNATIVE_IMPL)
#else
  if (TRUE)
#endif
  {
#if WIN95 && !defined(GNUWIN32)
    CHARRANGE range;
    range.cpMin = 0;
    range.cpMax = 0;
    SendMessage(GetHWND(), EM_EXGETSEL, 0, (LPARAM) &range);
    return range.cpMin;
#else
    DWORD Pos=(DWORD)SendMessage(GetHWND(), EM_GETSEL, 0, 0L);
    return Pos&0xFFFF;
#endif
  }
  return 0;
//  return insertionPoint;
}

long wxTextWindow::GetLastPosition(void)
{
#if !EDITABLE_TEXT_WINDOW
  if (windowStyle & wxNATIVE_IMPL)
#else
  if (TRUE)
#endif
  {
    HWND hWnd = GetHWND();
    
    // Will always return a number > 0 (according to docs)
    int noLines = (int)SendMessage(hWnd, EM_GETLINECOUNT, (WPARAM)0, (LPARAM)0L);

    // This gets the char index for the _beginning_ of the last line
    int charIndex = (int)SendMessage(hWnd, EM_LINEINDEX, (WPARAM)(noLines-1), (LPARAM)0L);
    
    // Get number of characters in the last line. We'll add this to the character
    // index for the last line, 1st position.
    int lineLength = (int)SendMessage(hWnd, EM_LINELENGTH, (WPARAM)charIndex, (LPARAM)0L);

    return (long)(charIndex + lineLength);
  }
  return 0;
}

long wxTextWindow::XYToPosition(long x, long y)
{
  Synch();
#if !EDITABLE_TEXT_WINDOW
  if (windowStyle & wxNATIVE_IMPL)
#else
  if (TRUE)
#endif
  {
    HWND hWnd = GetHWND();
    
    // This gets the char index for the _beginning_ of this line
    int charIndex = (int)SendMessage(hWnd, EM_LINEINDEX, (WPARAM)y, (LPARAM)0);
    return (long)(x + charIndex);
  }
  return 0;
}

void wxTextWindow::PositionToXY(long pos, long *x, long *y)
{
#if !EDITABLE_TEXT_WINDOW
  if (windowStyle & wxNATIVE_IMPL)
#else
  if (TRUE)
#endif
  {
    HWND hWnd = GetHWND();

    // This gets the line number containing the character
    int lineNo = (int)SendMessage(hWnd, EM_LINEFROMCHAR, (WPARAM)pos, (LPARAM)0);
    // This gets the char index for the _beginning_ of this line
    int charIndex = (int)SendMessage(hWnd, EM_LINEINDEX, (WPARAM)lineNo, (LPARAM)0);
    // The X position must therefore be the different between pos and charIndex
    *x = (long)(pos - charIndex);
    *y = (long)lineNo;
  }
}

void wxTextWindow::ShowPosition(long pos)
{
#if !EDITABLE_TEXT_WINDOW
  if (windowStyle & wxNATIVE_IMPL)
#else
  if (TRUE)
#endif
  {
    HWND hWnd = GetHWND();
    
    // To scroll to a position, we pass the number of lines and characters
    // to scroll *by*. This means that we need to:
    // (1) Find the line position of the current line.
    // (2) Find the line position of pos.
    // (3) Scroll by (pos - current).
    // For now, ignore the horizontal scrolling.

    // Is this where scrolling is relative to - the line containing the caret?
    // Or is the first visible line??? Try first visible line.
    int currentLineLineNo1 = (int)SendMessage(hWnd, EM_LINEFROMCHAR, (WPARAM)-1, (LPARAM)0L);

    int currentLineLineNo = (int)SendMessage(hWnd, EM_GETFIRSTVISIBLELINE, (WPARAM)0, (LPARAM)0L);

    int specifiedLineLineNo = (int)SendMessage(hWnd, EM_LINEFROMCHAR, (WPARAM)pos, (LPARAM)0L);
    
    int linesToScroll = specifiedLineLineNo - currentLineLineNo;

/*
    wxDebugMsg("Caret line: %d; Current visible line: %d; Specified line: %d; lines to scroll: %d\n",
      currentLineLineNo1, currentLineLineNo, specifiedLineLineNo, linesToScroll);
*/

    if (linesToScroll != 0)
      (void)SendMessage(hWnd, EM_LINESCROLL, (WPARAM)0, (LPARAM)MAKELPARAM(linesToScroll, 0));
  }
}

int wxTextWindow::GetLineLength(long lineNo)
{
  Synch();
#if !EDITABLE_TEXT_WINDOW
  if (windowStyle & wxNATIVE_IMPL)
#else
  if (TRUE)
#endif
  {
    long charIndex = XYToPosition(0, lineNo);
    HWND hWnd = GetHWND();
    int len = (int)SendMessage(hWnd, EM_LINELENGTH, (WPARAM)charIndex, (LPARAM)0);
    return len;
  }
#if !EDITABLE_TEXT_WINDOW
  else
  {
    wxTextWnd *text_win = (wxTextWnd *)handle;
    wxNode *node = text_win->lines.Nth((int)lineNo);
    if (node)
    {
      char *s = (char *)node->Data();
      return strlen(s);
    }
  }
#endif
  return -1;
}

int wxTextWindow::GetLineText(long lineNo, char *buf)
{
  Synch();
#if !EDITABLE_TEXT_WINDOW
  if (windowStyle & wxNATIVE_IMPL)
#else
  if (TRUE)
#endif
  {
    HWND hWnd = GetHWND();
    *(WORD *)buf = 128;
    int noChars = (int)SendMessage(hWnd, EM_GETLINE, (WPARAM)lineNo, (LPARAM)buf);
    buf[noChars] = 0;
    return noChars;
  }
#if !EDITABLE_TEXT_WINDOW
  else
  {
    wxTextWnd *text_win = (wxTextWnd *)handle;
    wxNode *node = text_win->lines.Nth((int)lineNo);
    if (node)
    {
      char *s = (char *)node->Data();
      strcpy(buf, s);
      return strlen(s);
    }
  }
#endif
  buf[0] = 0;
  return -1;
}

void wxTextWindow::Replace(long from, long to, char *value)
{
  Synch();
#if !EDITABLE_TEXT_WINDOW
  if (windowStyle & wxNATIVE_IMPL)
#else
  if (TRUE)
#endif
  {
#if USE_CLIPBOARD
    HWND hWnd = GetHWND();
    long fromChar = from;
    long toChar = to;
    
    // Set selection and remove it
#ifdef WIN32
    SendMessage(hWnd, EM_SETSEL, fromChar, toChar);
#else
    SendMessage(hWnd, EM_SETSEL, (WPARAM)0, (LPARAM)MAKELONG(fromChar, toChar));
#endif
    SendMessage(hWnd, WM_CUT, (WPARAM)0, (LPARAM)0);

    if (value)
    {
      // Now replace with 'value', by pasting.
      wxSetClipboardData(wxCF_TEXT, (wxObject *)value, 0, 0);

      // Paste into edit control
      SendMessage(hWnd, WM_PASTE, (WPARAM)0, (LPARAM)0L);
    }
#endif
  }
}

void wxTextWindow::Remove(long from, long to)
{
  Synch();
#if !EDITABLE_TEXT_WINDOW
  if (windowStyle & wxNATIVE_IMPL)
#else
  if (TRUE)
#endif
  {
    HWND hWnd = GetHWND();
    long fromChar = from;
    long toChar = to;
    
    // Cut all selected text
#ifdef WIN32
    SendMessage(hWnd, EM_SETSEL, fromChar, toChar);
#else
    SendMessage(hWnd, EM_SETSEL, (WPARAM)0, (LPARAM)MAKELONG(fromChar, toChar));
#endif
    SendMessage(hWnd, WM_CUT, (WPARAM)0, (LPARAM)0);
  }
}

void wxTextWindow::SetSelection(long from, long to)
{
  Synch();
#if !EDITABLE_TEXT_WINDOW
  if (windowStyle & wxNATIVE_IMPL)
#else
  if (TRUE)
#endif
  {
    HWND hWnd = GetHWND();
    long fromChar = from;
    long toChar = to;
    // if from and to are both -1, it means
    // (in wxWindows) that all text should be selected.
    // This translates into Windows convention
    if ((from == -1) && (to == -1))
    {
      fromChar = 0;
      toChar = -1;
    }
    
#ifdef WIN32
    SendMessage(hWnd, EM_SETSEL, (WPARAM)fromChar, (LPARAM)toChar);
    SendMessage(hWnd, EM_SCROLLCARET, (WPARAM)0, (LPARAM)0);
#else
    // WPARAM is 0: selection is scrolled into view
    SendMessage(hWnd, EM_SETSEL, (WPARAM)0, (LPARAM)MAKELONG(fromChar, toChar));
#endif
  }
}

void wxTextWindow::SetEditable(Bool editable)
{
#if !EDITABLE_TEXT_WINDOW
  if (windowStyle & wxNATIVE_IMPL)
#else
  if (TRUE)
#endif
  {
    HWND hWnd = GetHWND();
    SendMessage(hWnd, EM_SETREADONLY, (WPARAM)!editable, (LPARAM)0L);
  }
}

// Copy selection to clipboard
void wxTextWindow::Copy(void)
{
  Synch();
#if !EDITABLE_TEXT_WINDOW
  if (windowStyle & wxNATIVE_IMPL)
#else
  if (TRUE)
#endif
  {
    HWND hWnd = GetHWND();
    SendMessage(hWnd, WM_COPY, 0, 0L);
  }
}

// Paste clipboard into text window
void wxTextWindow::Paste(void)
{
  Synch();
#if !EDITABLE_TEXT_WINDOW
  if (windowStyle & wxNATIVE_IMPL)
#else
  if (TRUE)
#endif
  {
    HWND hWnd = GetHWND();
    SendMessage(hWnd, WM_PASTE, 0, 0L);
  }
}

// Copy selection to clipboard, then remove selection.
void wxTextWindow::Cut(void)
{
  Synch();
#if !EDITABLE_TEXT_WINDOW
  if (windowStyle & wxNATIVE_IMPL)
#else
  if (TRUE)
#endif
  {
    HWND hWnd = GetHWND();
    SendMessage(hWnd, WM_CUT, 0, 0L);
  }
}

#if (!EDITABLE_TEXT_WINDOW)
wxTextWnd::wxTextWnd(wxWnd *parent, wxWindow *wx_win,
                       int x, int y, int width, int height, DWORD style):
  wxSubWnd(parent, wxCanvasClassName, wx_win, x, y, width, height, style)
{
  no_lines = 1;
  current_line = 0;
  lines.Add("");

  OnCreate2();
  ShowScrollBar(handle, SB_BOTH, TRUE);
}

wxTextWnd::~wxTextWnd()
{
}

BOOL wxTextWnd::OnPaint()
{
  RECT rect;
  if (GetUpdateRect(handle, &rect, FALSE))
  {
    PAINTSTRUCT ps;
    // Hold a pointer to the dc so long as the OnPaint() message
    // is being processed
    HDC dc = BeginPaint(handle, &ps);

    COLORREF bkgnd_color = GetSysColor(COLOR_WINDOW);
    ::SetTextColor(dc, GetSysColor(COLOR_WINDOWTEXT));
    ::SetBkColor(dc, bkgnd_color);

    HFONT oldFont = 0;
    if (wx_window->GetFont())
    {
      wx_window->GetFont()->RealizeResource();
      if (wx_window->GetFont()->GetResourceHandle())
        oldFont = ::SelectObject(dc, wx_window->GetFont()->GetResourceHandle());
    }

    HBRUSH brush = CreateSolidBrush(bkgnd_color);
    ::FillRect(dc, &rect, brush);
    DeleteObject(brush);

    int nStart = yscroll_position;
    int nEnd = wxMin(no_lines, yscroll_position + (rect.bottom/cyChar) + 1);

    int i;
    int x,y;
    wxNode *node = lines.Nth(nStart);
    i = nStart;
    while (node && (i < nEnd))
    {
      char *s = (char *)node->Data();
      x = cxChar * (1 - xscroll_position);
      y = cyChar * (i - yscroll_position);
      TextOut(dc, x, y, s, strlen(s));
      i ++;
      node = node->Next();
    }
    if (oldFont != 0)
      ::SelectObject(dc, oldFont);
    EndPaint(handle, &ps);
    return 0;
  }
  return 1;
}

void wxTextWnd::OnCreate2(void)
{
  TEXTMETRIC tm;
  HDC dc = GetDC(handle);
  GetTextMetrics(dc, &tm);
  ReleaseDC(handle, dc);
  cxChar = tm.tmAveCharWidth;
  cyChar = tm.tmHeight + tm.tmExternalLeading;
  yscroll_pixels_per_line = cyChar;
  xscroll_pixels_per_line = cxChar;
  xscroll_lines = 300;
  yscroll_lines = 0;
  ReleaseDC(handle, dc);
}

void wxTextWnd::OnSize(int x, int y, UINT)
{
  CalcNewSize(x, y);
  InvalidateRgn(handle, NULL, TRUE);
}

void wxTextWnd::CalcNewSize(int x, int y)
{
  cxClient = x;
  cyClient = y;

  int nMaxWidth = xscroll_lines*xscroll_pixels_per_line;

  int nVscrollMax = wxMax(0, (int)(no_lines + 2 - cyClient/cyChar));
  yscroll_position = wxMin(yscroll_position, nVscrollMax);

  SetScrollRange(handle, SB_VERT, 0, nVscrollMax, FALSE);
  SetScrollPos(handle, SB_VERT, yscroll_position, TRUE);

  int nHscrollMax = wxMax(0, (int)(2 + nMaxWidth - cxClient/cxChar));
  xscroll_position = wxMin(xscroll_position, nHscrollMax);

  SetScrollRange(handle, SB_HORZ, 0, nHscrollMax, FALSE);
  SetScrollPos(handle, SB_HORZ, xscroll_position, TRUE);

  yscroll_lines = no_lines;

  yscroll_lines_per_page = wxMax(1, cyClient/cyChar);
  xscroll_lines_per_page = 10;
}

#endif // EDITABLE_TEXT_WINDOW
