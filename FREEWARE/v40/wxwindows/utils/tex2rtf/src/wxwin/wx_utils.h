/*
 * File:     wx_utils.h
 * Purpose:  Miscellaneous utilities
 *
 *                       wxWindows 1.50
 * Copyright (c) 1993 Artificial Intelligence Applications Institute,
 *                   The University of Edinburgh
 *
 *                     Author: Julian Smart
 *                       Date: 7-9-93
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notice, author statement and this permission
 * notice appear in all copies of this software and related documentation.
 *
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND, EXPRESS,
 * IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
 * MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * IN NO EVENT SHALL THE ARTIFICIAL INTELLIGENCE APPLICATIONS INSTITUTE OR THE
 * UNIVERSITY OF EDINBURGH BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT ADVISED OF THE POSSIBILITY OF
 * DAMAGE, AND ON ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH
 * THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef wxb_utilsh
#define wxb_utilsh
#include "wx_obj.h"
#include "wx_list.h"

#ifdef wx_x
#include <dirent.h>
#include <unistd.h>
#endif

// sprintf is often needed, but we don't always want to include the whole
// of stdio.h!
#if (defined(wx_msw) || defined(wx_dos))
// extern "C" int __cdecl sprintf(char *, const char *, ...);
#include <stdio.h>
#endif
#ifdef wx_x
#include <stdio.h>
#endif

// Stupid ASCII macros
#define   wxToUpper(C)      (((C) >= 'a' && (C) <= 'z')? (C) - 'a' + 'A': (C))
#define   wxToLower(C)      (((C) >= 'A' && (C) <= 'Z')? (C) - 'A' + 'a': (C))

// Make a copy of this string using 'new'
char *copystring(char *s);

// Generate a unique ID
long NewId(void);

// Ensure subsequent IDs don't clash with this one
void RegisterId(long id);

// Useful buffer
extern char wxBuffer[];

// Various conversions
void StringToFloat(char *s, float *number);
char *FloatToString(float number);
void StringToDouble(char *s, double *number);
char *DoubleToString(double number);
void StringToInt(char *s, int *number);
void StringToLong(char *s, long *number);
char *IntToString(int number);
char *LongToString(long number);

// Matches string one within string two regardless of case
#ifndef IN_CPROTO
Bool StringMatch(char *one, char *two, Bool subString = TRUE, Bool exact = FALSE);
#endif

// Some file utilities

#ifdef IN_CPROTO
typedef       void    *wxPathList ;
typedef       void    *wxLogClass;
#else
// Path searching
class wxPathList: public wxList
{
  public:

  void AddEnvList(char *envVariable);    // Adds all paths in environment variable
  void Add(char *path);
  char *FindValidPath(char *filename);   // Find the first full path
                                         // for which the file exists
  void EnsureFileAccessible(char *path); // Given full path and filename,
                                         // add path to list
  Bool Member(char *path);
};

Bool wxFileExists(char *filename);
#define FileExists wxFileExists

Bool wxIsAbsolutePath(char *filename);
#define IsAbsolutePath wxIsAbsolutePath

// Get filename
char *wxFileNameFromPath(char *path);
#define FileNameFromPath wxFileNameFromPath

// Get directory
char *wxPathOnly(char *path);
#define PathOnly wxPathOnly

void Dos2UnixFilename(char *s);
void Unix2DosFilename(char *s);

// Get a temporary filename, opening and closing the file.
// void wxGetTempFileName(char *prefix, char *buf);

// Does the pattern contain wildcards?
Bool wxIsWild(char *pattern);

// Does the pattern match the text (usually a filename)?
// If dot_special is TRUE, doesn't match * against . (eliminating
// `hidden' dot files)
Bool wxMatchWild(char *pattern, char *text, Bool dot_special = TRUE);

// Execute another program. Returns FALSE if there was an error.
// Bool wxExecute(char *command);

// Concatenate two files to form third
Bool wxConcatFiles(char *file1, char *file2, char *file3);

// Copy file1 to file2
Bool wxCopyFile(char *file1, char *file2);

// Remove file
Bool wxRemoveFile(char *file);

// Rename file
Bool wxRenameFile(char *file1, char *file2);

// Get free memory in bytes, or -1 if cannot determine amount (e.g. on UNIX)
// long wxGetFreeMemory(void);

#ifndef max
// inline int max(int a, int b) { return a > b ? a : b; }
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
// inline int max(int a, int b) { return a < b ? a : b; }
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

inline Bool wxYield(void) { return TRUE; }

// void wxDebugMsg(char *fmt ...) ;
// void wxBell(void) ;

/*
 * Error message functions used by wxWindows
 *
 */

// Non-fatal error (continues) 
void wxError(char *msg, char *title = "wxWindows Internal Error");

// Fatal error (exits)
void wxFatalError(char *msg, char *title = "wxWindows Fatal Error");

void wxStripExtension(char *buffer);

#endif // IN_CPROTO
#endif // wxb_utilsh
