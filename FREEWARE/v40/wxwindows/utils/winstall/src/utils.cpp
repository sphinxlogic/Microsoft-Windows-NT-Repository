/*
 * File:	utils.cc
 * Purpose:     wxWindows installation utility: utility functions.
 * Author:      Julian Smart
 * Date:        20th April 1995
 *              Written in native Windows for small size.
 */

#include "windows.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#ifndef GNUWIN32
#include <sys/stat.h>
#endif
#include <direct.h>
#include <stdarg.h>

#include <dos.h>

#ifdef GNUWIN32
#include <sys/unistd.h>
#endif

#ifdef __BORLANDC__ // Please someone tell me which version of Borland needs
                    // this (3.1 I believe) and how to test for it.
                    // If this works for Borland 4.0 as well, then no worries.
#include <dir.h>
#endif

#ifdef WIN32
#include <io.h>
#endif

#include "utils.h"

char wxBuffer[1000];

char *
copystring (char *s)
{
  if (s == NULL) s = "";
  size_t len = strlen (s) + 1;

  char *news = new char[len];
  memcpy (news, s, len);	// Should be the fastest

  return news;
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

Bool wxGetResource(const char *section, const char *entry, int *value, const char *file)
{
  char *s = NULL;
  Bool succ = wxGetResource(section, entry, &s, file);
  if (succ)
  {
    *value = (int)strtol(s, NULL, 10);
    delete[] s; 
    return TRUE;
  }
  else return FALSE;
}

/*
 * Strip off any extension (dot something) from end of file,
 * IF one exists. Inserts zero into buffer.
 *
 */
 
void wxStripExtension(char *buffer)
{
  int len = strlen(buffer);
  int i = len-1;
  while (i > 0)
  {
    if (buffer[i] == '.')
    {
      buffer[i] = 0;
      break;
    }
    i --;
  }
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

Bool wxMkdir(const char *dir)
{
  return (mkdir(dir) == 0);
}

// Yield to incoming messages
Bool wxYield(void)
{
  MSG msg;
  // We want to go back to the main message loop
  // if we see a WM_QUIT. (?)
  while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) && msg.message != WM_QUIT)
  {
    if (!::GetMessage(&msg, NULL, NULL, NULL))
    {
      return FALSE;
    }
    
    ::TranslateMessage(&msg);
    ::DispatchMessage(&msg);
    return TRUE;
  }

  return TRUE;
}

// Copy files
Bool 
wxCopyFile (const char *file1, const char *file2)
{
  FILE *fd1;
  FILE *fd2;
  int ch;

  if ((fd1 = fopen (file1, "rb")) == NULL)
    return FALSE;
  if ((fd2 = fopen (file2, "wb")) == NULL)
    {
      fclose (fd1);
      return FALSE;
    }

  while ((ch = getc (fd1)) != EOF)
    putc (ch, fd2);

  fclose (fd1);
  fclose (fd2);
  return TRUE;
}

Bool wxExecute(const char *command, Bool sync)
{
  if (command == NULL || *command == '\0')
    return FALSE;

  long Instance_ID = WinExec((LPCSTR)command, SW_SHOW);
  if (Instance_ID < 32) return(FALSE);
// WIN32 doesn't have GetModuleUsage!!
#ifndef WIN32
  if (sync) {
    int running;
    do {
      wxYield();
      running = GetModuleUsage((HANDLE)Instance_ID);
    } while (running);
  }
#endif
  return(TRUE);
}

Bool wxFileExists(const char *f)
{
  FILE *fd = fopen(f, "r");
  if (fd)
  {
    fclose(fd);
    return TRUE;
  }
  return FALSE;
}

// Output a debug mess., in a system dependent fashion.
void wxDebugMsg(const char *fmt ...)
{
  va_list ap;
  static char buffer[512];

  va_start(ap, fmt);

  wvsprintf(buffer,fmt,ap) ;
  OutputDebugString((LPCSTR)buffer) ;

  va_end(ap);
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

