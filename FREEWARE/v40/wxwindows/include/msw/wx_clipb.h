/*
 * File:	wx_clipb.h
 * Purpose:	Clipboard functionality
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "@(#)wx_clipb.h	1.2 5/9/94" */

#ifndef wx_clipbh
#define wx_clipbh

#ifdef __GNUG__
#pragma interface
#endif

#include "wx_defs.h"
#include "wx_setup.h"

#if USE_CLIPBOARD

#include "wx_list.h"

#ifndef IN_CPROTO
WXDLLEXPORT Bool wxOpenClipboard(void);
WXDLLEXPORT Bool wxClipboardOpen(void);
WXDLLEXPORT Bool wxCloseClipboard(void);
WXDLLEXPORT Bool wxEmptyClipboard(void);
WXDLLEXPORT Bool wxIsClipboardFormatAvailable(int dataFormat);
WXDLLEXPORT Bool wxSetClipboardData(int dataFormat, wxObject *obj, int width = 0, int height = 0);
WXDLLEXPORT wxObject *wxGetClipboardData(int dataFormat, long *len = NULL);
WXDLLEXPORT int  wxEnumClipboardFormats(int dataFormat);
WXDLLEXPORT int  wxRegisterClipboardFormat(char *formatName);
WXDLLEXPORT Bool wxGetClipboardFormatName(int dataFormat, char *formatName, int maxCount);

/* The following is Matthew Flatt's implementation of the MSW
 * side of generic clipboard functionality.
 */

/* A clipboard client holds data belonging to the clipboard.
   For plain text, a client is not necessary. */
class WXDLLEXPORT wxClipboardClient : public wxObject
{
  DECLARE_ABSTRACT_CLASS(wxClipboardClient)

 public:
  /* This list should be filled in with strings indicating the formats
     this client can provide. Almost all clients will provide "TEXT".
     Format names should be 4 characters long, so things will work
     out on the Macintosh */
  wxStringList formats;

  /* This method is called when the client is losing the selection. */
  virtual void BeingReplaced(void) = 0;

  /* This method is called when someone wants the data this client is
     supplying to the clipboard. "format" is a string indicating the
     format of the data - one of the strings from the "formats"
     list. "*size" should be filled with the size of the resulting
     data. In the case of text, "*size" does not count the
     NULL terminator. */
  virtual char *GetData(char *format, long *size) = 0;
};

/* ONE instance of this class: */
class WXDLLEXPORT wxClipboard : public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxClipboard)

 public:
  wxClipboardClient *clipOwner;
  char *cbString, *sentString, *receivedString;
  void *receivedTargets;
  long receivedLength;
#ifdef wx_xview
  long sel_owner;
#endif

  wxClipboard();
  ~wxClipboard();

  /* Set the clipboard data owner. "time" comes from the event record. */
  void SetClipboardClient(wxClipboardClient *, long time);

  /* Set the clipboard string; does not require a client. */
  void SetClipboardString(char *, long time);

  /* Get data from the clipboard in the format "TEXT". */
  char *GetClipboardString(long time);

  /* Get data from the clipboard */
  char *GetClipboardData(char *format, long *length, long time);

  /* Get the clipboard client directly. Will be NULL if clipboard data
     is a string, or if some other application owns the clipboard. 
     This can be useful for shortcutting data translation, if the
     clipboard user can check for a specific client. (This is used
     by the wxMediaEdit class.) */
  wxClipboardClient *GetClipboardClient(void);
};

/* Initialize wxTheClipboard. Can be called repeatedly */
WXDLLEXPORT void wxInitClipboard(void);

/* The clipboard */
WXDLLEXPORT extern wxClipboard *wxTheClipboard;

#endif // IN_CPROTO
#endif // USE_CLIPBOARD
#endif // wx_clipbh
