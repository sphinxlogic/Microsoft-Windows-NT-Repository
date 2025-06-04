/*
 * File:        wx_clipb.cc
 * Purpose:     Clipboard implementation
 * Author:      Julian Smart
 * Created:     1993
 * Updated:     March 1996
 * Copyright:   (c) 1995, AIAI, University of Edinburgh
 */

/* static const char sccsid[] = "@(#)wx_clipb.cc        1.2 5/9/94"; */

#ifdef __GNUG__
#pragma implementation
#pragma implementation "wx_clipb.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx_setup.h"
#endif

#if USE_CLIPBOARD

#ifndef WX_PRECOMP
#include "wx_main.h"
#include "wx_frame.h"
#include "wx_gdi.h"
#include "wx_utils.h"
#endif

#include "wx_mf.h"
#include "wx_clipb.h"
#include "wx_privt.h"

Bool wxClipboardIsOpen = FALSE;

Bool wxOpenClipboard(void)
{
  if (wxTheApp->wx_frame && !wxClipboardIsOpen)
  {
    wxClipboardIsOpen = (Bool)::OpenClipboard(((wxWnd *)wxTheApp->wx_frame->handle)->handle);
    return wxClipboardIsOpen;
  }
  else return FALSE;
}

Bool wxCloseClipboard(void)
{
  if (wxClipboardIsOpen)
  {
    wxClipboardIsOpen = FALSE;
  }
  return (Bool)::CloseClipboard();
}

Bool wxEmptyClipboard(void)
{
  return (Bool)::EmptyClipboard();
}

Bool wxClipboardOpen(void)
{
  return wxClipboardIsOpen;
}

Bool wxIsClipboardFormatAvailable(int dataFormat)
{
  return ::IsClipboardFormatAvailable(dataFormat);
}

#if USE_IMAGE_LOADING_IN_MSW
#include "..\..\utils\dib\dib.h"
#endif

Bool wxSetClipboardData(int dataFormat, wxObject *obj, int width, int height)
{
  switch (dataFormat)
  {
    case wxCF_BITMAP:
    {
      wxBitmap *wxBM = (wxBitmap *)obj;

      HDC hdcMem = CreateCompatibleDC(NULL);
      HDC hdcSrc = CreateCompatibleDC(NULL);
      HBITMAP old = ::SelectObject(hdcSrc, wxBM->ms_bitmap);
      HBITMAP hBitmap = CreateCompatibleBitmap(hdcSrc,
                                              wxBM->GetWidth(), wxBM->GetHeight());
      if (!hBitmap)
      {
        SelectObject(hdcSrc, old);
        DeleteDC(hdcMem);
        DeleteDC(hdcSrc);
        return FALSE;
      }
      HBITMAP old1 = SelectObject(hdcMem, hBitmap);
      BitBlt(hdcMem, 0, 0, wxBM->GetWidth(), wxBM->GetHeight(),
             hdcSrc, 0, 0, SRCCOPY);

      // Select new bitmap out of memory DC
      SelectObject(hdcMem, old1);

      // Set the data
      Bool success = (Bool)::SetClipboardData(CF_BITMAP, hBitmap);

      // Clean up
      SelectObject(hdcSrc, old);
      DeleteDC(hdcSrc);
      DeleteDC(hdcMem);      
      return success;
      break;
    }
    case wxCF_DIB:
    {
#if USE_IMAGE_LOADING_IN_MSW
      HBITMAP hBitmap=((wxBitmap *)obj)->ms_bitmap;
      HANDLE hDIB=BitmapToDIB(hBitmap,NULL); // NULL==uses system palette
      Bool success = (Bool) ::SetClipboardData(CF_DIB,hDIB);
#else
      Bool success=FALSE;
#endif
      return success;
      break;
    }
#if USE_METAFILE
    case wxCF_METAFILE:
    {
      wxMetaFile *wxMF = (wxMetaFile *)obj;
      HANDLE data = GlobalAlloc(GHND, sizeof(METAFILEPICT) + 1);
#ifdef __WINDOWS_386__
      METAFILEPICT *mf = (METAFILEPICT *)MK_FP32(GlobalLock(data));
#else
      METAFILEPICT *mf = (METAFILEPICT *)GlobalLock(data);
#endif

      mf->mm = wxMF->windowsMappingMode;
      mf->xExt = width;
      mf->yExt = height;
      mf->hMF = wxMF->metafile;
      GlobalUnlock(data);
      wxMF->metafile = NULL;

      return (Bool)SetClipboardData(CF_METAFILEPICT, data);
      break;
    }
#endif
    case CF_SYLK:
    case CF_DIF:
    case CF_TIFF:
    case CF_PALETTE:
    {
      return FALSE;
      break;
    }
    case wxCF_OEMTEXT:
      dataFormat = wxCF_TEXT;
    case wxCF_TEXT:
       width = strlen((char *)obj) + 1;
       height = 1;
    default:
    {
      char *s = (char *)obj;
      DWORD l;

      l = (width * height);
      HANDLE hGlobalMemory = GlobalAlloc(GHND, l);
                if (!hGlobalMemory)
        return FALSE;

#ifdef __WINDOWS_386__
      LPSTR lpGlobalMemory = (LPSTR)MK_FP32(GlobalLock(hGlobalMemory));
#else
      LPSTR lpGlobalMemory = (LPSTR)GlobalLock(hGlobalMemory);
#endif

#ifdef WIN32
      memcpy(lpGlobalMemory, s, l);
#elif defined(__WATCOMC__) && defined(__WINDOWS_386__)
      memcpy(lpGlobalMemory, s, l);
#else
      hmemcpy(lpGlobalMemory, s, l);
#endif

      GlobalUnlock(hGlobalMemory);
      HANDLE success = SetClipboardData(dataFormat, hGlobalMemory);
      return (Bool)success;
      break;
    }
  }
  return FALSE;
}

wxObject *wxGetClipboardData(int dataFormat, long *len)
{
  switch (dataFormat)
  {
    case wxCF_BITMAP:
    {
      BITMAP bm;
      HBITMAP hBitmap = GetClipboardData(CF_BITMAP);
      if (!hBitmap)
        return NULL;

      HDC hdcMem = CreateCompatibleDC(NULL);
      HDC hdcSrc = CreateCompatibleDC(NULL);

      HBITMAP old = ::SelectObject(hdcSrc, hBitmap);
      GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bm);

      HBITMAP hNewBitmap = CreateBitmapIndirect(&bm);

      if (!hNewBitmap)
      {
        SelectObject(hdcSrc, old);
        DeleteDC(hdcMem);
        DeleteDC(hdcSrc);
        return NULL;
      }

      HBITMAP old1 = SelectObject(hdcMem, hNewBitmap);
      BitBlt(hdcMem, 0, 0, bm.bmWidth, bm.bmHeight,
             hdcSrc, 0, 0, SRCCOPY);

      // Select new bitmap out of memory DC
      SelectObject(hdcMem, old1);

      // Clean up
      SelectObject(hdcSrc, old);
      DeleteDC(hdcSrc);
      DeleteDC(hdcMem);

      // Create and return a new wxBitmap
      wxBitmap *wxBM = new wxBitmap;
      wxBM->ms_bitmap = hNewBitmap;
      wxBM->SetWidth(bm.bmWidth);
      wxBM->SetHeight(bm.bmHeight);
      wxBM->SetDepth(bm.bmPlanes);
      wxBM->SetOk(TRUE);
      return (wxObject *)wxBM;
      break;
    }
    case wxCF_METAFILE:
    case CF_SYLK:
    case CF_DIF:
    case CF_TIFF:
    case CF_PALETTE:
    case wxCF_DIB:
    {
      return FALSE;
      break;
    }
    case wxCF_OEMTEXT:
      dataFormat = wxCF_TEXT;
         case wxCF_TEXT:
    default:
    {
      HANDLE hGlobalMemory = GetClipboardData(dataFormat);
      if (!hGlobalMemory)
        return NULL;

      int hsize = (int)GlobalSize(hGlobalMemory);
      if (len)
        *len = hsize;

      char *s = new char[hsize];
      if (!s)
        return NULL;

#ifdef __WINDOWS_386__
      LPSTR lpGlobalMemory = (LPSTR)MK_FP32(GlobalLock(hGlobalMemory));
#else
      LPSTR lpGlobalMemory = (LPSTR)GlobalLock(hGlobalMemory);
#endif

#ifdef WIN32
      memcpy(s, lpGlobalMemory, GlobalSize(hGlobalMemory));
#elif __WATCOMC__ && defined(__WINDOWS_386__)
      memcpy(s, lpGlobalMemory, GlobalSize(hGlobalMemory));
#else
      hmemcpy(s, lpGlobalMemory, GlobalSize(hGlobalMemory));
#endif

      GlobalUnlock(hGlobalMemory);

      return (wxObject *)s;
      break;
    }
  }
  return NULL;
}

int  wxEnumClipboardFormats(int dataFormat)
{
  return ::EnumClipboardFormats(dataFormat);
}

int  wxRegisterClipboardFormat(char *formatName)
{
  return ::RegisterClipboardFormat(formatName);
}

Bool wxGetClipboardFormatName(int dataFormat, char *formatName, int maxCount)
{
  return (::GetClipboardFormatName(dataFormat, formatName, maxCount) > 0);
}

/*
 * Generalized clipboard implementation by Matthew Flatt
 */

wxClipboard *wxTheClipboard = NULL;

void wxInitClipboard(void)
{
  if (!wxTheClipboard)
    wxTheClipboard = new wxClipboard;
}

wxClipboard::wxClipboard()
{
  clipOwner = NULL;
  cbString = NULL;
}

wxClipboard::~wxClipboard()
{
  if (clipOwner)
    clipOwner->BeingReplaced();
  if (cbString)
    delete[] cbString;
}

static int FormatStringToID(char *str)
{
  if (!strcmp(str, "TEXT"))
    return wxCF_TEXT;
  
  return wxRegisterClipboardFormat(str);
}

void wxClipboard::SetClipboardClient(wxClipboardClient *client, long time)
{
  Bool got_selection;

  if (clipOwner)
    clipOwner->BeingReplaced();
  clipOwner = client;
  if (cbString) {
    delete[] cbString;
    cbString = NULL;
  }

  if (wxOpenClipboard()) {
    char **formats, *data;
         int i;
    int ftype;
    long size;

    formats = clipOwner->formats.ListToArray(FALSE);
    for (i = clipOwner->formats.Number(); i--; ) {
      ftype = FormatStringToID(formats[i]);
      data = clipOwner->GetData(formats[i], &size);
      if (!wxSetClipboardData(ftype, (wxObject *)data, size, 1)) {
        got_selection = FALSE;
        break;
      }
    }

    if (i < 0)
      got_selection = wxCloseClipboard();
  } else
    got_selection = FALSE;
  
  got_selection = FALSE; // Assume another process takes over

  if (!got_selection) {
    clipOwner->BeingReplaced();
         clipOwner = NULL;
  }
}

wxClipboardClient *wxClipboard::GetClipboardClient()
{
  return clipOwner;
}

void wxClipboard::SetClipboardString(char *str, long time)
{
  Bool got_selection;

  if (clipOwner) {
    clipOwner->BeingReplaced();
    clipOwner = NULL;
  }
  if (cbString)
    delete[] cbString;

  cbString = str;

  if (wxOpenClipboard()) {
    if (!wxSetClipboardData(wxCF_TEXT, (wxObject *)str))
      got_selection = FALSE;
    else
                got_selection = wxCloseClipboard();
  } else
    got_selection = FALSE;

  got_selection = FALSE; // Assume another process takes over

  if (!got_selection) {
    delete[] cbString;
    cbString = NULL;
  }
}

char *wxClipboard::GetClipboardString(long time)
{
  char *str;
  long length;

  str = GetClipboardData("TEXT", &length, time);
  if (!str) {
    str = new char[1];
    *str = 0;
  }

  return str;
}

char *wxClipboard::GetClipboardData(char *format, long *length, long time)
{
  if (clipOwner)  {
         if (clipOwner->formats.Member(format))
      return clipOwner->GetData(format, length);
    else
      return NULL;
  } else if (cbString) {
    if (!strcmp(format, "TEXT"))
      return copystring(cbString);
    else
      return NULL;
  } else {
    if (wxOpenClipboard()) {
      receivedString = (char *)wxGetClipboardData(FormatStringToID(format), 
                                                  length);
      wxCloseClipboard();
    } else
      receivedString = NULL;

    return receivedString;
  }
}


#endif // USE_CLIPBOARD

