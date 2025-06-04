/*
 * File:	wx_utils.h
 * Purpose:	Miscellaneous utilities
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
	Last change:  JS   25 May 97   11:20 am
 */

/* sccsid[] = "%W% %G%" */

#ifndef wxb_utilsh
#define wxb_utilsh

#ifdef __GNUG__
#pragma interface
#endif

#include "wx_setup.h"
#include "wx_obj.h"
#include "wx_list.h"
#include "wx_win.h"

#if USE_IOSTREAMH
#include <iostream.h>
#else
#include <iostream>
#endif

// ITA change - these are only defined in MSL for Be OS and Power TV
#if defined( __MWERKS__)
int			strcasecmp (const char *str1, const char *str2);
int			strncasecmp(const char *str1, const char *str2, unsigned nchars);
#endif

#ifdef wx_x
#ifndef VMS
/*steve: these two are not known under VMS */
#include <dirent.h>
#include <unistd.h>
#endif
#endif

// sprintf is often needed, but we don't always want to include the whole
// of stdio.h!
#if 0
extern "C" int sprintf(char *, const char *, ...);
#else
#include <stdio.h>
#endif

#if defined( GNUWIN32 ) && defined( __MWERKS__)
#define stricmp strcasecmp
#define strnicmp strncasecmp
#endif

// Forward declaration
class wxFrame;

// Stupid ASCII macros
#define   wxToUpper(C)      (((C) >= 'a' && (C) <= 'z')? (C) - 'a' + 'A': (C))
#define   wxToLower(C)      (((C) >= 'A' && (C) <= 'Z')? (C) - 'A' + 'a': (C))

// Return a string with the current date/time
char * WXDLLEXPORT wxNow(void);

// Make a copy of this string using 'new'
WXDLLEXPORT char *copystring(const char *s);

// Generate a unique ID
WXDLLEXPORT long wxNewId(void);
#define NewId wxNewId

// Ensure subsequent IDs don't clash with this one
WXDLLEXPORT void wxRegisterId(long id);
#define RegisterId wxRegisterId

// Return the current ID
WXDLLEXPORT long wxGetCurrentId(void);

// Useful buffer
extern char *wxBuffer;

extern const char *wxFloatToStringStr;
extern const char *wxDoubleToStringStr;

// Various conversions
WXDLLEXPORT void StringToFloat(char *s, float *number);
WXDLLEXPORT char *FloatToString(float number, const char *fmt = wxFloatToStringStr);
WXDLLEXPORT void StringToDouble(char *s, double *number);
WXDLLEXPORT char *DoubleToString(double number, const char *fmt = wxDoubleToStringStr);
WXDLLEXPORT void StringToInt(char *s, int *number);
WXDLLEXPORT void StringToLong(char *s, long *number);
WXDLLEXPORT char *IntToString(int number);
WXDLLEXPORT char *LongToString(long number);

// Matches string one within string two regardless of case
#ifndef IN_CPROTO
WXDLLEXPORT Bool StringMatch(char *one, char *two, Bool subString = TRUE, Bool exact = FALSE);
#endif

// A shorter way of using strcmp
#define wxStringEq(s1, s2) (s1 && s2 && (strcmp(s1, s2) == 0))

// Convert 2-digit hex number to decimal
WXDLLEXPORT int wxHexToDec(char *buf);

// Convert decimal integer to 2-character hex string
WXDLLEXPORT void wxDecToHex(int dec, char *buf);

// Some file utilities

#ifdef IN_CPROTO
typedef       void    *wxPathList ;
typedef       void    *wxLogClass;
#else
// Path searching
class WXDLLEXPORT wxPathList: public wxList
{
  DECLARE_DYNAMIC_CLASS(wxPathList)

  public:

  void AddEnvList(char *envVariable);    // Adds all paths in environment variable
  void Add(char *path);
  char *FindValidPath(char *filename);   // Find the first full path
                                         // for which the file exists
  char *FindAbsoluteValidPath(char *filename);   // Find the first full path
                                         // for which the file exists; ensure it's an absolute
                                         // path that gets returned.
  void EnsureFileAccessible(char *path); // Given full path and filename,
                                         // add path to list
  Bool Member(char *path);
};

WXDLLEXPORT Bool wxFileExists(const char *filename);
#define FileExists wxFileExists

WXDLLEXPORT Bool wxDirExists(const char *dir);
#define DirExists wxDirExists

WXDLLEXPORT Bool wxIsAbsolutePath(const char *filename);
#define IsAbsolutePath wxIsAbsolutePath

// Get filename
WXDLLEXPORT char *wxFileNameFromPath(char *path);
#define FileNameFromPath wxFileNameFromPath

// Get directory
WXDLLEXPORT char *wxPathOnly(char *path);
#define PathOnly wxPathOnly

WXDLLEXPORT void wxDos2UnixFilename(char *s);
#define Dos2UnixFilename wxDos2UnixFilename

WXDLLEXPORT void wxUnix2DosFilename(char *s);
#define Unix2DosFilename wxUnix2DosFilename

// Strip the extension, in situ
WXDLLEXPORT void wxStripExtension(char *buffer);

// Get a temporary filename, opening and closing the file.
WXDLLEXPORT char *wxGetTempFileName(const char *prefix, char *buf = NULL);

// Expand file name (~/ and ${OPENWINHOME}/ stuff)
WXDLLEXPORT char *wxExpandPath(char *dest, const char *path);

// Contract w.r.t environment (</usr/openwin/lib, OPENWHOME> -> ${OPENWINHOME}/lib)
// and make (if under the home tree) relative to home
// [caller must copy-- volatile]
WXDLLEXPORT char *wxContractPath (const char *filename,
   const char *envname = NULL, const char *user = NULL);

// Destructive removal of /./ and /../ stuff
WXDLLEXPORT char *wxRealPath(char *path);

// Allocate a copy of the full absolute path
WXDLLEXPORT char *wxCopyAbsolutePath(const char *path);

// Get first file name matching given wild card.
// Flags are reserved for future use.
#define wxFILE  1
#define wxDIR   2
WXDLLEXPORT char *wxFindFirstFile(const char *spec, int flags = wxFILE);
WXDLLEXPORT char *wxFindNextFile(void);

// Does the pattern contain wildcards?
WXDLLEXPORT Bool wxIsWild(const char *pattern);

// Does the pattern match the text (usually a filename)?
// If dot_special is TRUE, doesn't match * against . (eliminating
// `hidden' dot files)
WXDLLEXPORT Bool wxMatchWild(const char *pattern,  const char *text, Bool dot_special = TRUE);

// Execute another program. Returns 0 if there was an error, a PID otherwise.
WXDLLEXPORT long wxExecute(char **argv, Bool Async = FALSE);
WXDLLEXPORT long wxExecute(const char *command, Bool Async = FALSE);

#define wxSIGTERM 1

WXDLLEXPORT int wxKill(long pid, int sig=wxSIGTERM);

// Execute a command in an interactive shell window
// If no command then just the shell
WXDLLEXPORT Bool wxShell(const char *command = NULL);

// Concatenate two files to form third
WXDLLEXPORT Bool wxConcatFiles(const char *file1, const char *file2, const char *file3);

// Copy file1 to file2
WXDLLEXPORT Bool wxCopyFile(const char *file1, const char *file2);

// Remove file
WXDLLEXPORT Bool wxRemoveFile(const char *file);

// Rename file
WXDLLEXPORT Bool wxRenameFile(const char *file1, const char *file2);

// Get current working directory.
// If buf is NULL, allocates space using new, else
// copies into buf.
// IMPORTANT NOTE getcwd is know not to work under some releases
// of Win32s 1.3, according to MS release notes!
WXDLLEXPORT char *wxGetWorkingDirectory(char *buf = NULL, int sz = 1000);

// Set working directory
WXDLLEXPORT Bool wxSetWorkingDirectory(char *d);

// Sleep for nSecs seconds under UNIX, do nothing under Windows
WXDLLEXPORT void wxSleep(int nSecs);

// Get free memory in bytes, or -1 if cannot determine amount (e.g. on UNIX)
WXDLLEXPORT long wxGetFreeMemory(void);

// Consume all events until no more left
WXDLLEXPORT void wxFlushEvents(void);

// Make directory
WXDLLEXPORT Bool wxMkdir(const char *dir);

// Remove directory. Flags reserved for future use.
WXDLLEXPORT Bool wxRmdir(const char *dir, int flags = 0);

/*
 * Network and username functions.
 *
 */

// Get eMail address
WXDLLEXPORT Bool wxGetEmailAddress(char *buf, int maxSize);

// Get hostname.
WXDLLEXPORT Bool wxGetHostName(char *buf, int maxSize);

// Get user ID e.g. jacs
WXDLLEXPORT Bool wxGetUserId(char *buf, int maxSize);

// Get user name e.g. Julian Smart
WXDLLEXPORT Bool wxGetUserName(char *buf, int maxSize);

/*
 * Strip out any menu codes
 */
WXDLLEXPORT char *wxStripMenuCodes(char *in, char *out = NULL);

// Find the window/widget with the given title or label.
// Pass a parent to begin the search from, or NULL to look through
// all windows.
WXDLLEXPORT wxWindow *wxFindWindowByLabel(char *title, wxWindow *parent = NULL);

// Find window by name, and if that fails, by label.
WXDLLEXPORT wxWindow *wxFindWindowByName(char *name, wxWindow *parent = NULL);

// Returns menu item id or -1 if none.
WXDLLEXPORT int wxFindMenuItemId(wxFrame *frame, char *menuString, char *itemString);

// A debugging stream buffer.
// Under Windows, this writes to the Windows debug output.
// Under other platforms, it writes to cerr.

// ALl this horrible gubbins required for Borland, because the calling
// convention needs to be the same as for streambuf.
// Thanks to Gerhard.Vogt@embl-heidelberg.de for this solution.

#if defined(__BORLANDC__) && defined(__BCOPT__) && !defined(_RTL_ALLOW_po) && !defined(__FLAT__)
#pragma option -po-
#endif

// ITA change - this class doen't compile under MSL
#if !defined(__MWERKS__) && !defined(WXUSINGDLL)

class WXDLLEXPORT wxDebugStreamBuf: public streambuf
{
  public:
    wxDebugStreamBuf(void);
    ~wxDebugStreamBuf(void) {}

    int overflow(int i);
    inline int underflow(void) { return EOF; }
    int sync(void);
};

#endif

#if defined(__BORLANDC__) && defined(__BCOPT__) && !defined(_RTL_ALLOW_po) && !defined(__FLAT__)
#pragma option -po.
#endif

/*
#if (!defined(__MINMAX_DEFINED) && !defined(max))
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#define __MINMAX_DEFINED 1
#endif
*/
#define wxMax(a,b)            (((a) > (b)) ? (a) : (b))
#define wxMin(a,b)            (((a) < (b)) ? (a) : (b))

// Yield to other apps/messages
WXDLLEXPORT Bool wxYield(void);

// Format a message on the standard error (UNIX) or the debugging
// stream (Windows)
WXDLLEXPORT void wxDebugMsg(const char *fmt ...) ;
 
// Sound the bell
WXDLLEXPORT void wxBell(void) ;
  
// Get OS version
WXDLLEXPORT int wxGetOsVersion(int *majorVsn=NULL,int *minorVsn=NULL) ;

// Set the cursor to the busy cursor for all windows
class wxCursor;
WXDLLEXPORT extern wxCursor *wxHOURGLASS_CURSOR;
WXDLLEXPORT void wxBeginBusyCursor(wxCursor *cursor = wxHOURGLASS_CURSOR);
 
// Restore cursor to normal
WXDLLEXPORT void wxEndBusyCursor(void);
 
// TRUE if we're between the above two calls
WXDLLEXPORT Bool wxIsBusy(void);
  
/* Error message functions used by wxWindows */

// Non-fatal error (continues)
extern const char *wxInternalErrorStr;
WXDLLEXPORT void wxError(const char *msg, const char *title = wxInternalErrorStr);

// Fatal error (exits)
extern const char *wxFatalErrorStr;
WXDLLEXPORT void wxFatalError(const char *msg, const char *title = wxFatalErrorStr);

// Reading and writing resources (eg WIN.INI, .Xdefaults)
#if USE_RESOURCES
WXDLLEXPORT Bool wxWriteResource(const char *section, const char *entry, char *value, const char *file = NULL);
WXDLLEXPORT Bool wxWriteResource(const char *section, const char *entry, float value, const char *file = NULL);
WXDLLEXPORT Bool wxWriteResource(const char *section, const char *entry, long value, const char *file = NULL);
WXDLLEXPORT Bool wxWriteResource(const char *section, const char *entry, int value, const char *file = NULL);

WXDLLEXPORT Bool wxGetResource(const char *section, const char *entry, char **value, const char *file = NULL);
WXDLLEXPORT Bool wxGetResource(const char *section, const char *entry, float *value, const char *file = NULL);
WXDLLEXPORT Bool wxGetResource(const char *section, const char *entry, long *value, const char *file = NULL);
WXDLLEXPORT Bool wxGetResource(const char *section, const char *entry, int *value, const char *file = NULL);
#endif // USE_RESOURCES

#ifdef wx_x
// 'X' Only, will soon vanish....
// Get current Home dir and copy to dest
WXDLLEXPORT char *wxGetHomeDir(char *dest);
#endif
// Get the user's home dir (caller must copy--- volatile)
// returns NULL is no HOME dir is known
WXDLLEXPORT char *wxGetUserHome(const char *user = NULL);

// Check whether this window wants to process messages, e.g. Stop button
// in long calculations.
WXDLLEXPORT Bool wxCheckForInterrupt(wxWindow *wnd);

// MSW only: get user-defined resource from the .res file.
// Returns NULL or newly-allocated memory, so use delete[] to clean up.
#ifdef wx_msw
extern const char *wxUserResourceStr;
WXDLLEXPORT char *wxLoadUserResource(const char *resourceName, const char *resourceType = wxUserResourceStr);
#endif

// Gets the mouse cursor position
void wxGetMousePosition(int* x, int* y);

// X only
#ifdef wx_x
// Get X display: often needed in the wxWindows implementation.
Display *wxGetDisplay(void);
/* Matthew Flatt: Added wxSetDisplay and wxGetDisplayName */
Bool wxSetDisplay(char *display_name);
char *wxGetDisplayName(void);
#endif

#ifdef wx_x

#include <X11/Xlib.h>

#define wxMAX_RGB           0xff
#define wxMAX_SV            1000
#define wxSIGN(x)           ((x < 0) ? -x : x)
#define wxH_WEIGHT          4
#define wxS_WEIGHT          1
#define wxV_WEIGHT          2

typedef struct wx_hsv {
                        int h,s,v;
                      } wxHSV;
 
#define wxMax3(x,y,z) ((x > y) ? ((x > z) ? x : z) : ((y > z) ? y : z))
#define wxMin3(x,y,z) ((x < y) ? ((x < z) ? x : z) : ((y < z) ? y : z))

#define wxMax2(x,y)   ((x > y) ? x : y)
#define wxMin2(x,y)   ((x < y) ? x : y)

void wxHSVToXColor(wxHSV *hsv,XColor *xcolor);
void wxXColorToHSV(wxHSV *hsv,XColor *xcolor);
void wxAllocNearestColor(Display *display,Colormap colormap,XColor *xcolor);
void wxAllocColor(Display *display,Colormap colormap,XColor *xcolor);

#endif //wx_x

#endif // IN_CPROTO
#endif // wxb_utilsh
