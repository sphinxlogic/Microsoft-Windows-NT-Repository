/*
 * File:	wx_utils.cc
 * Purpose:	Various utilities
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	August 1994
 * RCS_ID:      $Id: wx_utils.cc,v 1.2 1994/08/14 23:00:24 edz Exp edz $
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* static const char sccsid[] = "%W% %G%"; */

#ifdef __GNUG__
#pragma implementation
#pragma implementation "wx_utils.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx_setup.h"
#include "wx_utils.h"
#include "wx_main.h"
#include "wx_gdi.h"
#endif

#include "wx_timer.h"
#include <ctype.h>

#if !defined(__MWERKS__) && !defined(GNUWIN32)
#include <direct.h>
#endif

#ifdef GNUWIN32
#include <sys/unistd.h>
#include <sys/stat.h>
#include <std.h>
#define stricmp strcasecmp
#endif

#ifndef GNUWIN32
#include <dos.h>
#endif

#ifdef __BORLANDC__ // Please someone tell me which version of Borland needs
                    // this (3.1 I believe) and how to test for it.
                    // If this works for Borland 4.0 as well, then no worries.
#include <dir.h>
#endif

#ifdef WIN32
#include <io.h>
#ifndef GNUWIN32
#include <shellapi.h>
#endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef __WATCOMC__
#if !(defined(_MSC_VER) && (_MSC_VER > 800))
#include <errno.h>
#endif
#endif
#include <stdarg.h>

// In the WIN.INI file
static const char WX_SECTION[] = "wxWindows";
static const char eHOSTNAME[]  = "HostName";
static const char eUSERID[]    = "UserId";
static const char eUSERNAME[]  = "UserName";


// For the following functions we SHOULD fill in support
// for Windows-NT (which I don't know) as I assume it begin
// a POSIX Unix (so claims MS) that it has some special
// functions beyond those provided by WinSock

// Get full hostname (eg. DoDo.BSn-Germany.crg.de)
Bool wxGetHostName(char *buf, int maxSize)
{
#ifdef WIN32
  DWORD nSize = maxSize;
  return ::GetComputerName(buf, &nSize);
#else
  char *sysname;
  const char *default_host = "noname";

  if ((sysname = getenv("SYSTEM_NAME")) == NULL) {
     GetProfileString(WX_SECTION, eHOSTNAME, default_host, buf, maxSize - 1);
  } else
    strncpy(buf, sysname, maxSize - 1);
  buf[maxSize] = '\0';
  return *buf ? TRUE : FALSE;
#endif
}

// Get user ID e.g. jacs
Bool wxGetUserId(char *buf, int maxSize)
{
#if defined(WIN32) && !defined(__win32s__) && 0
  // Gets the current user's full name according to the MS article PSS ID
  // Number: Q119670
  // Seems to be the same as the login name for me?
  char *UserName = new char[256];
  char *Domain = new char[256];
  DWORD maxCharacters = 255;
  GetUserName( UserName, &maxCharacters );
  GetComputerName( Domain, &maxCharacters );

  WCHAR  wszUserName[256];           // Unicode user name
  WCHAR  wszDomain[256];
  LPBYTE ComputerName;
 
  struct _SERVER_INFO_100 *si100;   // Server structure
  struct _USER_INFO_2 *ui;          // User structure
 
  // Convert ASCII user name and domain to Unicode.
 
  MultiByteToWideChar( CP_ACP, 0, UserName,
     strlen(UserName)+1, wszUserName, sizeof(wszUserName) );
  MultiByteToWideChar( CP_ACP, 0, Domain,
     strlen(Domain)+1, wszDomain, sizeof(wszDomain) );

  // Get the computer name of a DC for the specified domain.
  // >If you get a link error on this, include netapi32.lib<
 
  NetGetDCName( NULL, wszDomain, &ComputerName );
 
  // Look up the user on the DC.
 
  if(NetUserGetInfo( (LPWSTR) ComputerName,
     (LPWSTR) &wszUserName, 2, (LPBYTE *) &ui))
  {
     printf( "Error getting user information.\n" );
     return( FALSE );
  }
 
  // Convert the Unicode full name to ASCII.
 
  WideCharToMultiByte( CP_ACP, 0, ui->usri2_full_name,
     -1, buf, 256, NULL, NULL );
  }
  return( TRUE );
/*
  DWORD nSize = maxSize;
  return ::GetUserName(buf, &nSize);
*/
#else
  char *user;
  const char *default_id = "anonymous";

  // Can't assume we have NIS (PC-NFS) or some other ID daemon
  // So we ...
  if (	(user = getenv("USER")) == NULL &&
	(user = getenv("LOGNAME")) == NULL ) {
     // Use wxWindows configuration data (comming soon)
     GetProfileString(WX_SECTION, eUSERID, default_id, buf, maxSize - 1);
  } else
    strncpy(buf, user, maxSize - 1);
  return *buf ? TRUE : FALSE;
#endif
}

// Get user name e.g. Julian Smart
Bool wxGetUserName(char *buf, int maxSize)
{
  const char *default_name = "Unknown User"; 
#if defined(WIN32)
  DWORD nSize = maxSize;
/* In VC++ 4.0, results in unresolved symbol __imp__GetUserNameA
  if (GetUserName(buf, &nSize))
    return TRUE;
  else
*/
    // Could use NIS, MS-Mail or other site specific programs
    // Use wxWindows configuration data 
    GetProfileString(WX_SECTION, eUSERNAME, default_name, buf, maxSize - 1);
    return *buf ? TRUE : FALSE;
//  }
#else
#if !defined(__WATCOMC__) && !defined(GNUWIN32)
  extern HANDLE hPenWin; // PenWindows Running?
  if (hPenWin)
  {
    // PenWindows Does have a user concept!
    // Get the current owner of the recognizer
    GetPrivateProfileString("Current", "User", default_name, wxBuffer, maxSize - 1, "PENWIN.INI");
    strncpy(buf, wxBuffer, maxSize - 1);
  }
  else
#endif
  {
    // Could use NIS, MS-Mail or other site specific programs
    // Use wxWindows configuration data 
    GetProfileString(WX_SECTION, eUSERNAME, default_name, buf, maxSize - 1);
  }
  return *buf ? TRUE : FALSE;
#endif
}

// Execute a command (e.g. another program) in a
// system-independent manner.

long wxExecute(char **argv, Bool sync)
{
  if (*argv == NULL)
    return 0;

  char command[1024];
  command[0] = '\0';

  int argc;
  for (argc = 0; argv[argc]; argc++)
   {
    if (argc)
      strcat(command, " ");
    strcat(command, argv[argc]);
   }

  return wxExecute((char *)command, sync);
}

long wxExecute(const char *command, Bool sync)
{
  if (command == NULL || *command == '\0')
    return 0;

#ifdef WIN32
  char * cl;
  char * argp;
  int clen;
  HINSTANCE result;
  DWORD dresult;

  // copy the command line
  clen = strlen( command);
  if (!clen) return -1;
  cl = (char *) calloc( 1, 256);
  if (!cl) return -1;
  strcpy( cl, command);

  // isolate command and arguments
  argp = strchr( cl, ' ');
  if (argp)
    *argp++ = '\0';

  // execute the command
#ifdef GNUWIN32
  result = ShellExecute( (wxTheApp->GetTopWindow() ? wxTheApp->GetTopWindow()->GetHWND() : NULL),
     (const wchar_t) "open", (const wchar_t) cl, (const wchar_t) argp, (const wchar_t) NULL, SW_SHOWNORMAL);
#else
  result = ShellExecute( (wxTheApp->GetTopWindow() ? wxTheApp->GetTopWindow()->GetHWND() : NULL),
     "open", cl, argp, NULL, SW_SHOWNORMAL);
#endif

  if (((long)result) <= 32) {
   free(cl);
   return 0;
  }
  
  if (!sync)
  {
    free(cl);
    return dresult;
  }

  // waiting until command executed
  do {
    wxYield();
    dresult = GetModuleFileName( result, cl, 256);
  } while( dresult);
  
  long lastError = GetLastError();

  free(cl);
  return 0;
#else
  long instanceID = WinExec((LPCSTR)command, SW_SHOW);
  if (instanceID < 32) return(0);

  if (sync) {
    int running;
    do {
      wxYield();
      running = GetModuleUsage((HANDLE)instanceID);
    } while (running);
  }
  return(instanceID);
#endif
}

int wxKill(long pid, int sig)
{
  return 0;
}

//
// Execute a program in an Interactive Shell
//
Bool
wxShell(const char *command)
{
  char *shell;
  if ((shell = getenv("COMSPEC")) == NULL)
    shell = "\\COMMAND.COM";

  char tmp[255];
  if (command && *command)
    sprintf(tmp, "%s /c %s", shell, command);
  else
    strcpy(tmp, shell);

  return wxExecute((char *)tmp, FALSE);
}

Bool wxRemoveFile(const char *file)
{
// Zortech -- what's the proper define?
#ifdef ZTC
  int flag = unlink(file);
#else
  int flag = remove(file);
#endif
  if (flag == 0) return TRUE;
  return FALSE;
}

Bool wxMkdir(const char *dir)
{
#ifdef GNUWIN32
  return (mkdir (dir, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) == 0);
#else
  return (mkdir(dir) == 0);
#endif
}

Bool wxRmdir(const char *dir, int WXUNUSED(flags))
{
  return (rmdir(dir) == 0);
}

Bool wxDirExists(const char *dir)
{
  /* MATTHEW: [6] Always use same code for Win32, call FindClose */
#if defined(WIN32)
  WIN32_FIND_DATA fileInfo;
#else
#ifdef __BORLANDC__
  struct ffblk fileInfo;
#else
  struct find_t fileInfo;
#endif
#endif

#if defined(WIN32)
	HANDLE h = FindFirstFile((LPTSTR)dir,(LPWIN32_FIND_DATA)&fileInfo);

	if (h==INVALID_HANDLE_VALUE)
	 return FALSE;
	else {
	 FindClose(h);
	 return ((fileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY);
	}
#else
  // In Borland findfirst has a different argument
  // ordering from _dos_findfirst. But _dos_findfirst
  // _should_ be ok in both MS and Borland... why not?
#ifdef __BORLANDC__
  return ((findfirst(dir, &fileInfo, _A_SUBDIR) == 0  && (fileInfo.ff_attrib & _A_SUBDIR) != 0));
#else
  return (((_dos_findfirst(dir, _A_SUBDIR, &fileInfo) == 0) && (fileInfo.attrib & _A_SUBDIR)) != 0);
#endif
#endif
}


// OLD CODE
#if 0
Bool wxDirExists(const char *dir)
{
#if defined(WIN32)
  DWORD fileAttr = GetFileAttributes(dir);
  if (fileAttr & FILE_ATTRIBUTE_DIRECTORY)
    return TRUE;
  else
    return FALSE;
#else
  // In Borland findfirst has a different argument
  // ordering from _dos_findfirst. But _dos_findfirst
  // _should_ be ok in both MS and Borland... why not?
#ifdef __BORLANDC__
  struct ffblk fileInfo;
  return (findfirst(dir, &fileInfo, _A_SUBDIR) == 0  && (fileInfo.ff_attrib & _A_SUBDIR));
#else
  struct find_t fileInfo;
  return ((_dos_findfirst(dir, _A_SUBDIR, &fileInfo) == 0) && (fileInfo.attrib & _A_SUBDIR));
#endif
#endif
}
#endif

// Get a temporary filename, opening and closing the file.
char *wxGetTempFileName(const char *prefix, char *buf)
{
#ifndef	WIN32
  char tmp[144];
  ::GetTempFileName(0, prefix, 0, tmp);
#else
  char tmp[MAX_PATH];
  char tmpPath[MAX_PATH];
  ::GetTempPath(MAX_PATH, tmpPath);
  ::GetTempFileName(tmpPath, prefix, 0, tmp);
#endif
  if (buf) strcpy(buf, tmp);
  else buf = copystring(tmp);
  return buf;
/**** old
  char tmp[64];
  ::GetTempFileName(0, prefix, 0, tmp);
  if (buf) strcpy(buf, tmp);
  else buf = copystring(tmp);
  return buf;
*/
}

// Get first file name matching given wild card.


#ifdef WIN32
HANDLE wxFileStrucHandle = INVALID_HANDLE_VALUE;
WIN32_FIND_DATA wxFileStruc;
#else
#ifdef __BORLANDC__
static struct ffblk wxFileStruc;
#else
static struct _find_t wxFileStruc;
#endif
#endif
static char *wxFileSpec = NULL;
static int wxFindFileFlags;

char *wxFindFirstFile(const char *spec, int flags)
{
  if (wxFileSpec)
	 delete[] wxFileSpec;
  wxFileSpec = copystring(spec);
  wxFindFileFlags = flags; /* MATTHEW: [5] Remember flags */

  // Find path only so we can concatenate
  // found file onto path
  char *p = wxPathOnly(wxFileSpec);
  if (p && (strlen(p) > 0))
	 strcpy(wxBuffer, p);
  else
	 wxBuffer[0] = 0;

#ifdef WIN32
  if (wxFileStrucHandle != INVALID_HANDLE_VALUE)
	 FindClose(wxFileStrucHandle);

  wxFileStrucHandle = FindFirstFile(spec, &wxFileStruc);

  if (wxFileStrucHandle == INVALID_HANDLE_VALUE)
	 return NULL;

  Bool isdir = !!(wxFileStruc.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);

  if (isdir && !(flags & wxDIR))
	 return wxFindNextFile();
  else if (!isdir && flags && !(flags & wxFILE))
	 return wxFindNextFile();

  if (wxBuffer[0] != 0)
	 strcat(wxBuffer, "\\");
  strcat(wxBuffer, wxFileStruc.cFileName);
  return wxBuffer;
#else

  int flag = _A_NORMAL;
  if (flags & wxDIR) /* MATTHEW: [5] Use & */
    flag = _A_SUBDIR;

#ifdef __BORLANDC__
  if (findfirst(spec, &wxFileStruc, flag) == 0)
#else
  if (_dos_findfirst(spec, flag, &wxFileStruc) == 0)
#endif
  {
    /* MATTHEW: [5] Check directory flag */
    char attrib;

#ifdef __BORLANDC__
    attrib = wxFileStruc.ff_attrib;
#else
    attrib = wxFileStruc.attrib;
#endif

    if (attrib & _A_SUBDIR) {
      if (!(wxFindFileFlags & wxDIR))
	return wxFindNextFile();
    } else if (wxFindFileFlags && !(wxFindFileFlags & wxFILE))
		return wxFindNextFile();

	 if (wxBuffer[0] != 0)
		strcat(wxBuffer, "\\");

#ifdef __BORLANDC__
	 strcat(wxBuffer, wxFileStruc.ff_name);
#else
	 strcat(wxBuffer, wxFileStruc.name);
#endif
	 return wxBuffer;
  }
  else
    return NULL;
#endif // WIN32
}

char *wxFindNextFile(void)
{
  // Find path only so we can concatenate
  // found file onto path
  char *p = wxPathOnly(wxFileSpec);
  if (p && (strlen(p) > 0))
	 strcpy(wxBuffer, p);
  else
	 wxBuffer[0] = 0;
  
 try_again:

#ifdef WIN32
  if (wxFileStrucHandle == INVALID_HANDLE_VALUE)
	 return NULL;

  Bool success = FindNextFile(wxFileStrucHandle, &wxFileStruc);
  if (!success) {
		FindClose(wxFileStrucHandle);
      wxFileStrucHandle = INVALID_HANDLE_VALUE;
		return NULL;
  }

  Bool isdir = !!(wxFileStruc.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
  
  if (isdir && !(wxFindFileFlags & wxDIR))
    goto try_again;
  else if (!isdir && wxFindFileFlags && !(wxFindFileFlags & wxFILE))
	 goto try_again;

  if (wxBuffer[0] != 0)
    strcat(wxBuffer, "\\");
  strcat(wxBuffer, wxFileStruc.cFileName);
  return wxBuffer;  
#else

#ifdef __BORLANDC__
  if (findnext(&wxFileStruc) == 0)
#else
  if (_dos_findnext(&wxFileStruc) == 0)
#endif
  {
    /* MATTHEW: [5] Check directory flag */
    char attrib;

#ifdef __BORLANDC__
    attrib = wxFileStruc.ff_attrib;
#else
    attrib = wxFileStruc.attrib;
#endif

    if (attrib & _A_SUBDIR) {
      if (!(wxFindFileFlags & wxDIR))
	goto try_again;
    } else if (wxFindFileFlags && !(wxFindFileFlags & wxFILE))
      goto try_again;


	 if (wxBuffer[0] != 0)
      strcat(wxBuffer, "\\");
#ifdef __BORLANDC__
	 strcat(wxBuffer, wxFileStruc.ff_name);
#else
	 strcat(wxBuffer, wxFileStruc.name);
#endif
	 return wxBuffer;
  }
  else
    return NULL;
#endif
}


// OLD CODE
#if 0
// Flags are reserved for future use.

// Does this have to be global?
// Or can we use a new _find_t for wxFindNextFile?

#ifdef WIN32
WIN32_FIND_DATA wxFileStruc;
HANDLE wxFileStrucHandle = 0;
int wxFileStrucFlags = 0;
#else

#ifdef __BORLANDC__
static struct ffblk wxFileStruc;
#else
static struct _find_t wxFileStruc;
#endif

#endif

static char *wxFileSpec = NULL;
static int wxFileFlags = 0;

char *wxFindFirstFile(const char *spec, int flags)
{
  if (wxFileSpec)
    delete[] wxFileSpec;
  wxFileSpec = copystring(spec);
  wxFileFlags = flags; /* MATTHEW: [5] Remember flags */

  // Find path only so we can concatenate
  // found file onto path
  char *p = wxPathOnly(wxFileSpec);
  if (p && (strlen(p) > 0))
    strcpy(wxBuffer, p);
  else
    wxBuffer[0] = 0;

#ifdef WIN32
  wxFileStrucHandle = FindFirstFile(spec, &wxFileStruc);
  wxFileStrucFlags = flags;
  
  if (wxFileStrucHandle == INVALID_HANDLE_VALUE)
    return NULL;
    
  switch (flags)
  {
    case wxFILE:
    {
      if ((wxFileStruc.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)
      {
        if (wxBuffer[0] != 0)
          strcat(wxBuffer, "\\");
        strcat(wxBuffer, wxFileStruc.cFileName);
        return wxBuffer;
      }
      break;
    }
    case wxDIR:
    {
      if ((wxFileStruc.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
      {
        if (wxBuffer[0] != 0)
          strcat(wxBuffer, "\\");
        strcat(wxBuffer, wxFileStruc.cFileName);
        return wxBuffer;
      }
      break;
    }
  }

  // If this one not of the right type, find the next file.
  return wxFindNextFile();
#else
  int flag = _A_NORMAL;
  if (flags == wxDIR)
    flag = _A_SUBDIR;
  
#ifdef __BORLANDC__
  if (findfirst(spec, &wxFileStruc, flag) == 0)
#else
  if (_dos_findfirst(spec, flag, &wxFileStruc) == 0)
#endif
  {
    if (wxBuffer[0] != 0)
      strcat(wxBuffer, "\\");

#ifdef __BORLANDC__
    strcat(wxBuffer, wxFileStruc.ff_name);
#else
    strcat(wxBuffer, wxFileStruc.name);
#endif
    return wxBuffer;
  }
  else
    return NULL;
#endif
 // WIN32
}

char *wxFindNextFile(void)
{
  // Find path only so we can concatenate
  // found file onto path
  char *p = wxPathOnly(wxFileSpec);
  if (p && (strlen(p) > 0))
    strcpy(wxBuffer, p);
  else
    wxBuffer[0] = 0;

#ifdef WIN32
  while (TRUE)
  {
    Bool success = FindNextFile(wxFileStrucHandle, &wxFileStruc);
    if (!success)
      return NULL;

    switch (wxFileStrucFlags)
    {
      case wxFILE:
      {
        if ((wxFileStruc.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)
        {
          if (wxBuffer[0] != 0)
            strcat(wxBuffer, "\\");
          strcat(wxBuffer, wxFileStruc.cFileName);
          return wxBuffer;
        }
        break;
      }
      case wxDIR:
      {
        if ((wxFileStruc.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
        {
          if (wxBuffer[0] != 0)
            strcat(wxBuffer, "\\");
          strcat(wxBuffer, wxFileStruc.cFileName);
          return wxBuffer;
        }
        break;
      }
    }
  }
  return NULL;
#else

#ifdef __BORLANDC__
  if (findnext(&wxFileStruc) == 0)
#else
  if (_dos_findnext(&wxFileStruc) == 0)
#endif
  {
    if (wxBuffer[0] != 0)
      strcat(wxBuffer, "\\");
#ifdef __BORLANDC__
    strcat(wxBuffer, wxFileStruc.ff_name);
#else
    strcat(wxBuffer, wxFileStruc.name);
#endif
    return wxBuffer;
  }
  else
    return NULL;
#endif
}
#endif
 // OLD CODE
 
 
// Get current working directory.
// If buf is NULL, allocates space using new, else
// copies into buf.
char *wxGetWorkingDirectory(char *buf, int sz)
{
  if (!buf)
  {
    buf = new char[1000];
    sz = 1000;
  }
#ifdef __BORLANDC__
  (void)getcwd(buf, sz);
#elif defined(__WATCOMC__) || defined(GNUWIN32)
  (void)getcwd(buf, sz);
#else
  (void)_getcwd(buf, sz);
#endif
  return buf;
}

Bool wxSetWorkingDirectory(char *d)
{
#ifdef WIN32
  return (Bool)SetCurrentDirectory(d);
#else
  // Must change drive, too.
  Bool isDriveSpec = ((strlen(d) > 1) && (d[1] == ':'));
  if (isDriveSpec)
  {
    char firstChar = d[0];

    // To upper case
    if (firstChar > 90)
      firstChar = firstChar - 32;

    // To a drive number
    unsigned int driveNo = firstChar - 64;
    if (driveNo > 0)
    {
       unsigned int noDrives;
       _dos_setdrive(driveNo, &noDrives);
    }
  }
  Bool success = (chdir(d) == 0);

  return success;
#endif
}

// Get free memory in bytes, or -1 if cannot determine amount (e.g. on UNIX)
long wxGetFreeMemory(void)
{
#if defined(WIN32) && !defined(__BORLANDC__)
  MEMORYSTATUS memStatus;
  memStatus.dwLength = sizeof(MEMORYSTATUS);
  GlobalMemoryStatus(&memStatus);
  return memStatus.dwAvailPhys;
#else
  return (long)GetFreeSpace(0);
#endif
}

// Sleep for nSecs seconds. Attempt a Windows implementation using timers.
static Bool inTimer = FALSE;
class wxSleepTimer: public wxTimer
{
 public:
  inline void Notify(void)
  {
    inTimer = FALSE;
    Stop();
  }
};

static wxTimer *wxTheSleepTimer = NULL;

void wxSleep(int nSecs)
{
#if 0 // WIN32 - this causes whole process to hang
  Sleep( 1000*nSecs );
#else
  if (inTimer)
    return;

  wxTheSleepTimer = new wxSleepTimer;
  inTimer = TRUE;
  wxTheSleepTimer->Start(nSecs*1000);
  while (inTimer)
  {
    if (wxTheApp->Pending())
      wxTheApp->Dispatch();
  }
  delete wxTheSleepTimer;
  wxTheSleepTimer = NULL;
#endif
}

// Consume all events until no more left
void wxFlushEvents(void)
{
//  wxYield();
}

// Output a debug mess., in a system dependent fashion.
void wxDebugMsg(const char *fmt ...)
{
  va_list ap;
  static char buffer[512];

  if (!wxTheApp->wantDebugOutput)
    return ;

  va_start(ap, fmt);

  wvsprintf(buffer,fmt,ap) ;
  OutputDebugString((LPCSTR)buffer) ;

  va_end(ap);
}

// Non-fatal error: pop up message box and (possibly) continue
void wxError(const char *msg, const char *title)
{
  sprintf(wxBuffer, "%s\nContinue?", msg);
  if (MessageBox(NULL, (LPCSTR)wxBuffer, (LPCSTR)title,
             MB_ICONSTOP | MB_YESNO) == IDNO)
    wxExit();
}

// Fatal error: pop up message box and abort
void wxFatalError(const char *msg, const char *title)
{
  sprintf(wxBuffer, "%s: %s", title, msg);
  FatalAppExit(0, (LPCSTR)wxBuffer);
}

// Emit a beeeeeep
void wxBell(void)
{
#ifdef WIN32
  Beep(1000,1000) ;	// 1kHz during 1 sec.
#else
  MessageBeep(-1) ;
#endif
}

int wxGetOsVersion(int *majorVsn, int *minorVsn)
{
  extern char *wxOsVersion;
  if (majorVsn)
    *majorVsn = 0;
  if (minorVsn)
    *minorVsn = 0;
    
  int retValue ;
#ifndef WIN32
#ifdef __WINDOWS_386__
  retValue = wxWIN386;
#else

#if !defined(__WATCOMC__) && !defined(GNUWIN32)
  extern HANDLE hPenWin;
  retValue = hPenWin ? wxPENWINDOWS : wxWINDOWS ;
#endif

#endif
#else
  DWORD Version = GetVersion() ;
  WORD  lowWord  = LOWORD(Version) ;
  
  if (wxOsVersion)
  {
    if (strcmp(wxOsVersion, "Win95") == 0)
      return wxWIN95;
    else if (strcmp(wxOsVersion, "Win32s") == 0)
      return wxWIN32S;
    else if (strcmp(wxOsVersion, "Windows") == 0)
      return wxWINDOWS;
    else if (strcmp(wxOsVersion, "WinNT") == 0)
      return wxWINDOWS_NT;
  }
  BOOL  Win32s  = ( Version & 0x80000000 );
  BOOL  Win95   = (( Version & 0xFF ) >= 4);
  BOOL  WinNT   = Version < 0x80000000;
  
  // Get the version number
  if (majorVsn)
	  *majorVsn = LOBYTE( lowWord );
  if (minorVsn)
	  *minorVsn = HIBYTE( lowWord );

  if (Win95)
    return wxWIN95;
  else if (Win32s)
    return wxWIN32S;
  else if (WinNT)
    return wxWINDOWS_NT;
  else
    return wxWINDOWS;
    
//  retValue = ((high & 0x8000)==0) ? wxWINDOWS_NT : wxWIN32S ;
#endif
  // @@@@ To be completed. I don't have the manual here...
  if (majorVsn) *majorVsn = 3 ;
  if (minorVsn) *minorVsn = 1 ;
  return retValue ;
}

// Reading and writing resources (eg WIN.INI, .Xdefaults)
#if USE_RESOURCES
Bool wxWriteResource(const char *section, const char *entry, char *value, const char *file)
{
  if (file)
    return WritePrivateProfileString((LPCSTR)section, (LPCSTR)entry, (LPCSTR)value, (LPCSTR)file);
  else
    return WriteProfileString((LPCSTR)section, (LPCSTR)entry, (LPCSTR)value);
}

Bool wxWriteResource(const char *section, const char *entry, float value, const char *file)
{
  char buf[50];
  sprintf(buf, "%.4f", value);
  return wxWriteResource(section, entry, (char *)buf, file);
}

Bool wxWriteResource(const char *section, const char *entry, long value, const char *file)
{
  char buf[50];
  sprintf(buf, "%ld", value);
  return wxWriteResource(section, entry, (char *)buf, file);
}

Bool wxWriteResource(const char *section, const char *entry, int value, const char *file)
{
  char buf[50];
  sprintf(buf, "%d", value);
  return wxWriteResource(section, entry, (char *)buf, file);
}

Bool wxGetResource(const char *section, const char *entry, char **value, const char *file)
{
  static const char defunkt[] = "$$default";
  if (file)
  {
    int n = GetPrivateProfileString((LPCSTR)section, (LPCSTR)entry, (LPCSTR)defunkt,
                                    (LPSTR)wxBuffer, 1000, (LPCSTR)file);
    if (n == 0 || strcmp(wxBuffer, defunkt) == 0)
     return FALSE;
  }
  else
  {
    int n = GetProfileString((LPCSTR)section, (LPCSTR)entry, (LPCSTR)defunkt,
                                    (LPSTR)wxBuffer, 1000);
    if (n == 0 || strcmp(wxBuffer, defunkt) == 0)
      return FALSE;
  }
  if (*value) delete[] (*value);
      *value = copystring(wxBuffer);
      return TRUE;
    }

Bool wxGetResource(const char *section, const char *entry, float *value, const char *file)
{
  char *s = NULL;
  Bool succ = wxGetResource(section, entry, (char **)&s, file);
  if (succ)
  {
    *value = (float)strtod(s, NULL);
    delete[] s;
    return TRUE;
  }
  else return FALSE;
}

Bool wxGetResource(const char *section, const char *entry, long *value, const char *file)
{
  char *s = NULL;
  Bool succ = wxGetResource(section, entry, (char **)&s, file);
  if (succ)
  {
    *value = strtol(s, NULL, 10);
    delete[] s;
    return TRUE;
  }
  else return FALSE;
}

Bool wxGetResource(const char *section, const char *entry, int *value, const char *file)
{
  char *s = NULL;
  Bool succ = wxGetResource(section, entry, (char **)&s, file);
  if (succ)
  {
    *value = (int)strtol(s, NULL, 10);
    delete[] s; 
    return TRUE;
  }
  else return FALSE;
}
#endif // USE_RESOURCES

// Old cursor
static HCURSOR wxBusyCursorOld = 0;
static int wxBusyCursorCount = 0;

// Set the cursor to the busy cursor for all windows
void wxBeginBusyCursor(wxCursor *cursor)
{
  wxBusyCursorCount ++;
  if (wxBusyCursorCount == 1)
  {
    wxBusyCursorOld = ::SetCursor(cursor->ms_cursor);
  }
  else
  {
    (void)::SetCursor(cursor->ms_cursor);
  }
}

// Restore cursor to normal
void wxEndBusyCursor(void)
{
  if (wxBusyCursorCount == 0)
    return;
    
  wxBusyCursorCount --;
  if (wxBusyCursorCount == 0)
  {
    ::SetCursor(wxBusyCursorOld);
    wxBusyCursorOld = 0;
  }
}

// TRUE if we're between the above two calls
Bool wxIsBusy(void)
{
  return (wxBusyCursorCount > 0);
}    

// Hack for MS-DOS
char *wxGetUserHome (const char *user)
{
  char *home;
  if (user && *user) {
    char tmp[64];
    if (wxGetUserId(tmp, sizeof(tmp)/sizeof(char))) {
      // Guests belong in the temp dir
      if (stricmp(tmp, "annonymous") == 0) {
	if ((home = getenv("TMP")) != NULL ||
	    (home = getenv("TMPDIR")) != NULL ||
	    (home = getenv("TEMP")) != NULL)
	  return *home ? home : "\\";
      }
      if (stricmp(tmp, user) == 0)
	user = NULL;
    }
  }
  if (user == NULL || *user == '\0')
    if ((home = getenv("HOME")) != NULL)
    {
      strcpy(wxBuffer, home);
      Unix2DosFilename(wxBuffer);
      return wxBuffer;
    }
  return NULL; // No home known!
}

// Check whether this window wants to process messages, e.g. Stop button
// in long calculations.
Bool wxCheckForInterrupt(wxWindow *wnd)
{
	if(wnd){
		MSG msg;
		HWND win=wnd->GetHWND();
		while(PeekMessage(&msg,win,0,0,PM_REMOVE)){
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		return TRUE;//*** temporary?
	}
	else{
		wxError("wnd==NULL !!!");
		return FALSE;//*** temporary?
	}
}

// MSW only: get user-defined resource from the .res file.
// Returns NULL or newly-allocated memory, so use delete[] to clean up.

#ifdef wx_msw
char *wxLoadUserResource(const char *resourceName, const char *resourceType)
{
  char *s = NULL;
  HRSRC hResource = ::FindResource(wxhInstance, resourceName, resourceType);
  if (hResource == 0)
    return NULL;
  HGLOBAL hData = ::LoadResource(wxhInstance, hResource);
  if (hData == 0)
    return NULL;
  char *theText = (char *)LockResource(hData);
  if (!theText)
    return NULL;
    
  s = copystring(theText);

  // Obsolete in WIN32
#ifndef WIN32
  UnlockResource(hData);
#endif

  GlobalFree(hData);

  return s;
}
#endif

void wxGetMousePosition( int* x, int* y )
{
  POINT pt;
  GetCursorPos( &pt );
  *x = pt.x;
  *y = pt.y;
};

