/*
 * File:     wb_utils.cc
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


#include "common.h"
#include "wx_utils.h"
#include "wx_list.h"

#include <iostream.h>
#include <fstream.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// Useful buffer
char wxBuffer[500];

char *copystring(char *s)
{
  int l = strlen(s);
  char *news = new char[l+1];
  strcpy(news, s);
  return news;
}

// Id generation
static long wxCurrentId = 100;
long NewId(void)
{
  return wxCurrentId++;
}

void RegisterId(long id)
{
  if (id >= wxCurrentId)
    wxCurrentId = id + 1;
}


void StringToFloat(char *s, float *number)
{
  if (s && strlen(s) > 0)
  {
    double n = strtod(s, NULL);
    *number = (float)n;
  }
}

void StringToDouble(char *s, double *number)
{
  if (s && strlen(s) > 0)
  {
    *number = strtod(s, NULL);
  }
}

char *FloatToString(float number)
{
  char buf[80];
  sprintf(buf, "%.2f", number);
  return copystring(buf);
}

char *DoubleToString(double number)
{
  char buf[80];
  sprintf(buf, "%.2f", (float)number);
  return copystring(buf);
}

void StringToInt(char *s, int *number)
{
  if (s && strlen(s) > 0)
  {
    long n = strtol(s, NULL, 10);
    *number = (int)n;
  }
}

void StringToLong(char *s, long *number)
{
  if (s && strlen(s) > 0)
  {
    *number = strtol(s, NULL, 10);
  }
}

char *IntToString(int number)
{
  char buf[80];
  sprintf(buf, "%d", number);
  return copystring(buf);
}

char *LongToString(long number)
{
  char buf[80];
  sprintf(buf, "%ld", number);
  return copystring(buf);
}

// Match a string (one) within a string (two)
Bool StringMatch(char *one, char *two, Bool subString, Bool exact)
{
  int lenOne = strlen(one);
  int lenTwo = strlen(two);

  char *oneUpper = new char[lenOne + 1];
  char *twoUpper = new char[lenTwo + 1];

  int j = 0;
  for (j = 0; j < lenOne; j++)
    oneUpper[j] = toupper(one[j]);
  oneUpper[j] = 0;

  for (j = 0; j < lenTwo; j++)
    twoUpper[j] = toupper(two[j]);
  twoUpper[j] = 0;
  
  int found = FALSE;
  if (subString)
  {
    int i = 0;
    while (!found && i < lenTwo)
    {
      strncpy(wxBuffer, twoUpper + i, lenOne);
      wxBuffer[lenOne] = 0;
      if (strcmp(oneUpper, wxBuffer) == 0)
        found = TRUE;
      i ++;
    }
  }
  else
  {
    if (exact)
      found = (strcmp(oneUpper, twoUpper) == 0);
    else
      found = (strncmp(oneUpper, twoUpper, min(lenOne, lenTwo)) == 0);
  }

  delete[] oneUpper;
  delete[] twoUpper;

  return found;
}


/****** FILE UTILITIES ******/

void wxPathList::Add(char *path)
{
  Append((wxObject *)path);
}

// Add paths e.g. from the PATH environment variable
void wxPathList::AddEnvList(char *envVariable)
{
  char *val = getenv(envVariable);
  if (val)
  {
    char *s = copystring(val);
#if (defined(wx_msw) || defined(wx_dos))
    char *token = strtok(s, " ;"); // Don't seperate with colon in DOS as this may be
                                   // part of a drive specification.
#else
    char *token = strtok(s, " :;");
#endif

    if (token)
    {
      Add(copystring(token));
      while (token)
      {
#if (defined(wx_msw) || defined(wx_dos))
        token = strtok(NULL, " ;");
#else
        token = strtok(NULL, " :;");
#endif
        if (token)
          Add(copystring(token));
      }
    }
  }
}

// Given a full filename (with path), ensure that that file can
// be accessed again USING FILENAME ONLY by adding the path
// to the list if not already there.
void wxPathList::EnsureFileAccessible(char *path)
{
  char *path_only = PathOnly(path);
  if (path_only)
  {
    if (!Member(path_only))
      Add(path_only);
    else delete[] path_only;
  }
}

Bool wxPathList::Member(char *path)
{
  char *pathcopy = copystring(path);
  int i;
  int len = strlen(pathcopy);
  for (i = 0; i < len; i++)
    pathcopy[i] = toupper(pathcopy[i]);

  wxNode *node = First();
  Bool found = FALSE;
  while (node && !found)
  {
    char *path = (char *)node->Data();
    
    strcpy(wxBuffer, path);

    len = strlen(wxBuffer);
    for (i = 0; i < len; i++)
      wxBuffer[i] = toupper(wxBuffer[i]);

    if (strcmp(pathcopy, wxBuffer) == 0)
    {
      found = TRUE;
      node = NULL;
    }
    else node = node->Next();
  }
  delete[] pathcopy;
  return found;
}

char *wxPathList::FindValidPath(char *file)
{
  if (wxFileExists(file))
    return file;

  char *filename;

  if (wxIsAbsolutePath(file))
    filename = FileNameFromPath(file);
  else
    filename = copystring(file);

  wxNode *node = First();
  char *found = NULL;
  while (node && !found)
  {
    char *path = (char *)node->Data();
    strcpy(wxBuffer, path);
#ifdef wx_x
    strcat(wxBuffer, "/");
#endif
#if (defined(wx_msw) || defined(wx_dos))
    strcat(wxBuffer, "\\");
#endif
    strcat(wxBuffer, filename);
#if (defined(wx_msw) || defined(wx_dos))
    Unix2DosFilename(wxBuffer);
#endif
    if (wxFileExists(wxBuffer))
    {
      found = wxBuffer;
    }
    else
      node = node->Next();
  }
  delete[] filename;
  return found;
}

Bool wxFileExists(char *filename)
{
  if (!filename)
    return FALSE;
  FILE *fd = fopen(filename, "r");
  Bool bad = (fd == NULL);
  if (fd)
    fclose(fd);

  return !bad;
}

Bool wxIsAbsolutePath(char *filename)
{
  int len = strlen(filename);
  if ((len > 0 && (filename[0] == '/' || filename[0] == '\\')) ||
      (len > 1 && (filename[1] == ':')))
    return TRUE;
  else
    return FALSE;
}

// Return just the filename, not the path
char *wxFileNameFromPath(char *path)
{
  if (!path)
    return NULL;

  char buf[200];
  int i = 0;
  int l = strlen(path);
  Bool done = FALSE;

  i = l - 1;

  // Search backward for a backward or forward slash
  while (!done && i > -1)
  {
    if (path[i] == '/' || path[i] == '\\')
    {
      done = TRUE;
    }
    else i --;
  }
  i ++;

  int j;
  for (j = i; j < l; j++)
  {
    buf[j - i] = path[j];
  }
  buf[j - i] = 0;
  return copystring(buf);
}

// Return just the directory, or NULL if no directort
char *wxPathOnly(char *path)
{
  if (!path)
    return NULL;

  char buf[200];
  strcpy(buf, path);

  int i = 0;
  int l = strlen(path);
  Bool done = FALSE;

  i = l - 1;

  // Search backward for a backward or forward slash
  while (!done && i > -1)
  {
    if (path[i] == '/' || path[i] == '\\')
    {
      done = TRUE;
    }
    else i --;
  }

  if (i >= 0)
    buf[i] = 0;
  else
    buf[0] = 0;

  if (i <= 0)
    return NULL;
  else
    return copystring(buf);
}

// Utility for converting delimiters in DOS filenames to UNIX style
// and back again - or we get nasty problems with delimiters

void Dos2UnixFilename(char *s)
{
  int l = strlen(s);
  int i;
  for (i = 0; i < l; i++)
    if (s[i] == '\\')
      s[i] = '/';
}

void Unix2DosFilename(char *s)
{
#if (defined(wx_msw) || defined(wx_dos))
  int l = strlen(s);
  int i;
  for (i = 0; i < l; i++)
    if (s[i] == '/')
      s[i] = '\\';
#endif
}

/*
 * Check whether the filename has wildcards
 *
 */

Bool wxIsWild(char *pattern)
{
  register char *p = pattern;
  register char c;

  while ((c = *p++) != '\0')
    switch (c)
      {
      case '?':
      case '[':
      case '*':
	return TRUE;

      case '\\':
	if (*p++ == '\0')
	  return FALSE;
      }

  return FALSE;
}

/* Match the pattern PATTERN against the string TEXT;
   return 1 if it matches, 0 otherwise.

   A match means the entire string TEXT is used up in matching.

   In the pattern string, `*' matches any sequence of characters,
   `?' matches any character, [SET] matches any character in the specified set,
   [!SET] matches any character not in the specified set.

   A set is composed of characters or ranges; a range looks like
   character hyphen character (as in 0-9 or A-Z).
   [0-9a-zA-Z_] is the set of characters allowed in C identifiers.
   Any other character in the pattern must be matched exactly.

   To suppress the special syntactic significance of any of `[]*?!-\',
   and match the character exactly, precede it with a `\'.

   If DOT_SPECIAL is nonzero,
   `*' and `?' do not match `.' at the beginning of TEXT.

   THESE ROUTINES TAKEN FROM GLOB.C IN THE GNU LIBRARY. SEE GNU LICENSE.
 */

static int
wx_glob_match_after_star (char *pattern, char *text);

Bool wxMatchWild(char *pattern, char *text, Bool dot_special)
{
  register char *p = pattern, *t = text;
  register char c;

  while ((c = *p++) != '\0')
    switch (c)
      {
      case '?':
	if (*t == '\0' || (dot_special && t == text && *t == '.'))
	  return 0;
	else
	  ++t;
	break;

      case '\\':
	if (*p++ != *t++)
	  return 0;
	break;

      case '*':
	if (dot_special && t == text && *t == '.')
	  return 0;
	return wx_glob_match_after_star (p, t);

      case '[':
	{
	  register char c1 = *t++;
	  int invert;

	  if (c1 == '\0')
	    return 0;

	  invert = (*p == '!');

	  if (invert)
	    p++;

	  c = *p++;
	  while (1)
	    {
	      register char cstart = c, cend = c;

	      if (c == '\\')
		{
		  cstart = *p++;
		  cend = cstart;
		}

	      if (cstart == '\0')
		return 0;	/* Missing ']'. */

	      c = *p++;

	      if (c == '-')
		{
		  cend = *p++;
		  if (cend == '\\')
		    cend = *p++;
		  if (cend == '\0')
		    return 0;
		  c = *p++;
		}
	      if (c1 >= cstart && c1 <= cend)
		goto match;
	      if (c == ']')
		break;
	    }
	  if (!invert)
	    return 0;
	  break;

	match:
	  /* Skip the rest of the [...] construct that already matched.  */
	  while (c != ']')
	    {
	      if (c == '\0')
		return 0;
	      c = *p++;
	      if (c == '\0')
		return 0;
	      if (c == '\\')
		p++;
	    }
	  if (invert)
	    return 0;
	  break;
	}

      default:
	if (c != *t++)
	  return 0;
      }

  return *t == '\0';
}

/* Like glob_match, but match PATTERN against any final segment of TEXT.  */

static Bool
wx_glob_match_after_star (char *pattern, char *text)
{
  register char *p = pattern, *t = text;
  register char c, c1;

  while ((c = *p++) == '?' || c == '*')
    if (c == '?' && *t++ == '\0')
      return 0;

  if (c == '\0')
    return 1;

  if (c == '\\')
    c1 = *p;
  else
    c1 = c;

  --p;
  while (1)
    {
      if ((c == '[' || *t == c1) && wxMatchWild (p, t, 0))
	return 1;
      if (*t++ == '\0')
	return 0;
    }
}

// Concatenate two files to form third
Bool wxConcatFiles(char *file1, char *file2, char *file3)
{
  FILE *fd1 = fopen(file1, "rb");
  FILE *fd2 = fopen(file2, "rb");
  FILE *fd3 = fopen(file3, "wb");

  if (!(fd1 && fd2 && fd3))
    return FALSE;

  int ch = -2;
  while (ch != EOF)
  {
    ch = getc(fd1);
    if (ch != EOF)
      putc(ch, fd3);
  }
  ch = -2;
  while (ch != EOF)
  {
    ch = getc(fd2);
    if (ch != EOF)
      putc(ch, fd3);
  }
  fclose(fd1);
  fclose(fd2);
  fclose(fd3);

  return TRUE;
}

// Copy files
Bool wxCopyFile(char *file1, char *file2)
{
  FILE *fd1 = fopen(file1, "rb");
  FILE *fd2 = fopen(file2, "wb");

  if (!(fd1 && fd2))
    return FALSE;

  int ch = -2;
  while (ch != EOF)
  {
    ch = getc(fd1);
    if (ch != EOF)
      putc(ch, fd2);
  }
  fclose(fd1);
  fclose(fd2);
  return TRUE;
}

Bool wxRenameFile(char *file1, char *file2)
{
  int flag = rename(file1, file2);
  if (flag == 0) return TRUE;
  return FALSE;
}

Bool wxRemoveFile(char *file)
{
#ifdef wx_x
  int flag = unlink(file);
#endif
#if defined(wx_dos) || defined(wx_msw)
  int flag = remove(file);
#endif
  if (flag == 0) return TRUE;
  return FALSE;
}

// Get a temporary filename, opening and closing the file.
/*
void wxGetTempFileName(char *prefix, char *buf)
{
  ::GetTempFileName(0, prefix, 0, buf);
}

// Get free memory in bytes, or -1 if cannot determine amount (e.g. on UNIX)
long wxGetFreeMemory(void)
{
  return (long)GetFreeSpace(0);
}

// Sleep for nSecs seconds under UNIX, do nothing under Windows
// XView implementation according to the Heller manual
void wxSleep(int nSecs)
{
}

// Consume all events until no more left
void wxFlushEvents(void)
{
}

// Output a debug mess., in a system dependent fashion.
void wxDebugMsg(char *fmt ...)
{
  va_list ap;
  static char buffer[512];

  va_start(ap, fmt);

  wvsprintf(buffer,fmt,ap) ;
  OutputDebugString(buffer) ;

  va_end(ap);
}
*/

// Non-fatal error: pop up message box and (possibly) continue
void wxError(char *msg, char *title)
{
  cerr << title << msg << "\n";
}

// Fatal error: pop up message box and abort
void wxFatalError(char *msg, char *title)
{
  cerr << title << msg << "\n";
  exit(1);
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

