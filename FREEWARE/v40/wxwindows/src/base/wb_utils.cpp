/*
 * File:        wb_utils.cc
 * Purpose:     Miscellaneous utilities
 * Author:      Julian Smart
 * Created:     1993
 * Updated:     August 1994
 * RCS_ID:      $Id: wb_utils.cc,v 1.5 1994/08/15 21:53:50 edz Exp edz $
 * Copyright:   (c) 1993, AIAI, University of Edinburgh
 */

/* static const char sccsid[] = "%W% %G%"; */

#ifdef __GNUG__
#pragma implementation
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx_defs.h"
#include "wx_utils.h"
#include "wx_win.h"
#include "wx_menu.h"
#endif

#if USE_IOSTREAMH
#include <iostream.h>
#else
#include <iostream>
#endif

#include <fstream.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if !defined(__WATCOMC__)
#if !(defined(_MSC_VER) && (_MSC_VER > 800))
#include <errno.h>
#endif
#endif
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

// Pattern matching code.
// Yes, this path is deliberate (for Borland compilation)
#ifdef wx_mac /* MATTHEW: [5] Mac doesn't like paths with "/" */
#include "glob.inc"
#else
#include "../base/glob.inc"
#endif

#define _MAXPATHLEN 500

extern char *wxBuffer;

#if defined(VMS)
// we have no strI functions under VMS, therefore I have implemented
// an inefficient but portable version: convert copies of strings to lowercase
// and then use the normal comparison
static void myLowerString(char *s)
{
  while(*s){
    if(isalpha(*s)) *s = (char)tolower(*s);
    s++;
  }
}

int strcasecmp(const char *str_1, const char *str_2)
{
  char *temp1 = new char[strlen(str_1)+1];
  char *temp2 = new char[strlen(str_2)+1];
  strcpy(temp1,str_1);
  strcpy(temp2,str_2);
  myLowerString(temp1);
  myLowerString(temp2);

  int result = strcmp(temp1,temp2);
  delete[] temp1;
  delete[] temp2;

  return(result);
}

int strncasecmp(const char *str_1, const char *str_2, size_t maxchar)
{
  char *temp1 = new char[strlen(str_1)+1];
  char *temp2 = new char[strlen(str_2)+1];
  strcpy(temp1,str_1);
  strcpy(temp2,str_2);
  myLowerString(temp1);
  myLowerString(temp2);

  int result = strncmp(temp1,temp2,maxchar);
  delete[] temp1;
  delete[] temp2;

  return(result);
}
#endif

#ifdef wx_msw

#define   _wxToLower(_c)     (char)lcharmap[(unsigned char)(_c)]

#ifndef GNUWIN32
#define strcasecmp stricmp
#define strncasecmp strnicmp
#endif

#ifdef _MSC_VER
#pragma warning (disable : 4245)
#endif

// Lower case filename map
static unsigned char lcharmap[] =
{
  '\000', '\001', '\002', '\003', '\004', '\005', '\006', '\007',
  '\010', '\011', '\012', '\013', '\014', '\015', '\016', '\017',
  '\020', '\021', '\022', '\023', '\024', '\025', '\026', '\027',
  '\030', '\031', '\032', '\033', '\034', '\035', '\036', '\037',
  '\040', '\041', '\042', '\043', '\044', '\045', '\046', '\047',
  '\050', '\051', '\052', '\053', '\054', '\055', '\056', '\057',
  '\060', '\061', '\062', '\063', '\064', '\065', '\066', '\067',
  '\070', '\071', '\072', '\073', '\074', '\075', '\076', '\077',
  '\100', '\141', '\142', '\143', '\144', '\145', '\146', '\147',
  '\150', '\151', '\152', '\153', '\154', '\155', '\156', '\157',
  '\160', '\161', '\162', '\163', '\164', '\165', '\166', '\167',
  '\170', '\171', '\172', '\133', '\134', '\135', '\136', '\137',
  '\140', '\141', '\142', '\143', '\144', '\145', '\146', '\147',
  '\150', '\151', '\152', '\153', '\154', '\155', '\156', '\157',
  '\160', '\161', '\162', '\163', '\164', '\165', '\166', '\167',
  '\170', '\171', '\172', '\173', '\174', '\175', '\176', '\177',
  '\200', '\201', '\202', '\203', '\204', '\205', '\206', '\207',
  '\210', '\211', '\212', '\213', '\214', '\215', '\216', '\217',
  '\220', '\221', '\222', '\223', '\224', '\225', '\226', '\227',
  '\230', '\231', '\232', '\233', '\234', '\235', '\236', '\237',
  '\240', '\241', '\242', '\243', '\244', '\245', '\246', '\247',
  '\250', '\251', '\252', '\253', '\254', '\255', '\256', '\257',
  '\260', '\261', '\262', '\263', '\264', '\265', '\266', '\267',
  '\270', '\271', '\272', '\273', '\274', '\275', '\276', '\277',
  '\300', '\341', '\342', '\343', '\344', '\345', '\346', '\347',
  '\350', '\351', '\352', '\353', '\354', '\355', '\356', '\357',
  '\360', '\361', '\362', '\363', '\364', '\365', '\366', '\367',
  '\370', '\371', '\372', '\333', '\334', '\335', '\336', '\337',
  '\340', '\341', '\342', '\343', '\344', '\345', '\346', '\347',
  '\350', '\351', '\352', '\353', '\354', '\355', '\356', '\357',
  '\360', '\361', '\362', '\363', '\364', '\365', '\366', '\367',
  '\370', '\371', '\372', '\373', '\374', '\375', '\376', '\377',
};

#ifdef _MSC_VER
#pragma warning (default : 4245)
#endif

#else
// This declaration is missing in SunOS!
// (Yes, I know it is NOT ANSI-C but its in BSD libc)
#if defined(__xlC) || defined(_AIX) || defined(__GNUG__)
extern "C"
{
  int strcasecmp (const char *, const char *);
  int strncasecmp (const char *, const char *, size_t);
}
#endif
#endif				/* wx_msw */


char *
copystring (const char *s)
{
  if (s == NULL) s = "";
  size_t len = strlen (s) + 1;

  char *news = new char[len];
  memcpy (news, s, len);	// Should be the fastest

  return news;
}

// Id generation
static long wxCurrentId = 100;

long 
wxNewId (void)
{
  return wxCurrentId++;
}

long
wxGetCurrentId(void) { return wxCurrentId; }

void 
wxRegisterId (long id)
{
  if (id >= wxCurrentId)
    wxCurrentId = id + 1;
}

void 
StringToFloat (char *s, float *number)
{
  if (s && *s && number)
    *number = (float) strtod (s, NULL);
}

void 
StringToDouble (char *s, double *number)
{
  if (s && *s && number)
    *number = strtod (s, NULL);
}

char *
FloatToString (float number, const char *fmt)
{
  static char buf[256];

//  sprintf (buf, "%.2f", number);
  sprintf (buf, fmt, number);
  return buf;
}

char *
DoubleToString (double number, const char *fmt)
{
  static char buf[256];

  sprintf (buf, fmt, number);
  return buf;
}

void 
StringToInt (char *s, int *number)
{
  if (s && *s && number)
    *number = (int) strtol (s, NULL, 10);
}

void 
StringToLong (char *s, long *number)
{
  if (s && *s && number)
    *number = strtol (s, NULL, 10);
}

char *
IntToString (int number)
{
  static char buf[20];

  sprintf (buf, "%d", number);
  return buf;
}

char *
LongToString (long number)
{
  static char buf[20];

  sprintf (buf, "%ld", number);
  return buf;
}

// Array used in DecToHex conversion routine.
static char hexArray[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B',
  'C', 'D', 'E', 'F' };

// Convert 2-digit hex number to decimal
int wxHexToDec(char *buf)
{
  int firstDigit, secondDigit;
  
  if (buf[0] >= 'A')
    firstDigit = buf[0] - 'A' + 10;
  else
    firstDigit = buf[0] - '0';

  if (buf[1] >= 'A')
    secondDigit = buf[1] - 'A' + 10;
  else
    secondDigit = buf[1] - '0';
    
  return firstDigit * 16 + secondDigit;
}

// Convert decimal integer to 2-character hex string
void wxDecToHex(int dec, char *buf)
{
  int firstDigit = (int)(dec/16.0);
  int secondDigit = (int)(dec - (firstDigit*16.0));
  buf[0] = hexArray[firstDigit];
  buf[1] = hexArray[secondDigit];
  buf[2] = 0;
}

// Match a string INDEPENDENT OF CASE
Bool 
StringMatch (char *str1, char *str2, Bool subString, Bool exact)
{
  if (str1 == NULL || str2 == NULL)
    return FALSE;
  if (str1 == str2)
    return TRUE;

  if (subString)
    {
      int len1 = strlen (str1);
      int len2 = strlen (str2);
      int i;

      // Search for str1 in str2
      // Slow .... but acceptable for short strings
      for (i = 0; i <= len2 - len1; i++)
	{
	  if (strncasecmp (str1, str2 + i, len1) == 0)
	    return TRUE;
	}
    }
  else if (exact)
    {
      if (strcasecmp (str1, str2) == 0)
	return TRUE;
    }
  else
    {
      int len1 = strlen (str1);
      int len2 = strlen (str2);

      if (strncasecmp (str1, str2, wxMin (len1, len2)) == 0)
	return TRUE;
    }

  return FALSE;
}


/****** FILE UTILITIES ******/

void wxPathList::Add (char *path)
{
  Append ((wxObject *) path);
}

// Add paths e.g. from the PATH environment variable
void wxPathList::AddEnvList (char *envVariable)
{
  static const char PATH_TOKS[] =
#ifdef wx_msw
	" ;"; // Don't seperate with colon in DOS (used for drive)
#else
	" :;";
#endif

  char *val = getenv (envVariable);
  if (val && *val)
    {
      char *s = copystring (val);
      char *token = strtok (s, PATH_TOKS);

      if (token)
	{
	  Add (copystring (token));
	  while (token)
	    {
	      if ((token = strtok (NULL, PATH_TOKS)) != NULL)
		Add (copystring (token));
	    }
	}
      delete[]s;
    }
}

// Given a full filename (with path), ensure that that file can
// be accessed again USING FILENAME ONLY by adding the path
// to the list if not already there.
void wxPathList::EnsureFileAccessible (char *path)
{
  char *path_only = wxPathOnly (path);
  if (path_only)
    {
      if (!Member (path_only))
	Add (path_only);
    }
}

// BugFIX @@@@ Unix is case sensitive!
Bool wxPathList::Member (char *path)
{
  for (wxNode * node = First (); node != NULL; node = node->Next ())
    {
      char *path2 = (char *) node->Data ();
      if (path2 &&
#if defined(wx_msw) || defined(VMS)
      // Case INDEPENDENT
	  strcasecmp (path, path2) == 0
#else
      // Case sensitive File System 
	  strcmp (path, path2) == 0
#endif
	)
	return TRUE;
    }
  return FALSE;
}

char *wxPathList::FindValidPath (char *file)
{
/*
  if (wxFileExists (file))
    return file;
*/
  if (wxFileExists (wxExpandPath(wxBuffer, file)))
    return wxBuffer;

  char buf[_MAXPATHLEN];
  strcpy(buf, wxBuffer);

  char *filename = IsAbsolutePath (buf) ? wxFileNameFromPath (buf) : (char *)buf;

  for (wxNode * node = First (); node; node = node->Next ())
    {
      char *path = (char *) node->Data ();
      strcpy (wxBuffer, path);
      char ch = wxBuffer[strlen(wxBuffer)-1];
      if (ch != '\\' && ch != '/')
        strcat (wxBuffer, "/");
      strcat (wxBuffer, filename);
#ifdef wx_msw
      Unix2DosFilename (wxBuffer);
#endif
      if (wxFileExists (wxBuffer))
      {
	return wxBuffer;	// Found!
      }
    }				// for()

  return NULL;			// Not found
}

char *wxPathList::FindAbsoluteValidPath (char *file)
{
  char *f = FindValidPath(file);
  if (wxIsAbsolutePath(f))
    return f;
  else
  {
    char *s = copystring(f);
    char buf[500];
    wxGetWorkingDirectory(buf, 499);
    int len = (int)strlen(buf);
    char lastCh = 0;
    if (len > 0)
      lastCh = buf[len-1];
    if (lastCh != '/' && lastCh != '\\')
    {
#ifdef wx_msw
      strcat(buf, "\\");
#else
      strcat(buf, "/");
#endif
    }
    strcat(buf, s);
    delete[] s;
    strcpy(wxBuffer, buf);
    return wxBuffer;
  }
  return NULL;
}

Bool 
wxFileExists (const char *filename)
{
  struct stat stbuf;

  // (char *) cast necessary for VMS
  if (filename && stat ((char *)filename, &stbuf) == 0)
    return TRUE;
  return FALSE;
}

Bool 
wxIsAbsolutePath (const char *filename)
{
  if (filename)
    {
      if (*filename == '/'
#ifdef VMS
      || (*filename == '[' && *(filename+1) != '.')
#endif
#ifdef wx_msw
      /* MSDOS */
      || *filename == '\\' || (isalpha (*filename) && *(filename + 1) == ':')
#endif
	)
	return TRUE;
    }
  return FALSE;
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

// CAUSES PROBLEMS FOR GNU COMPILER -- does it? Check.
// #ifndef __GNUG__
// Destructive removal of /./ and /../ stuff
char *wxRealPath (char *path)
{
#ifdef wx_msw
  static const char SEP = '\\';
  Unix2DosFilename(path);
#else
  static const char SEP = '/';
#endif
  if (path[0] && path[1]) {
    /* MATTHEW: special case "/./x" */
    char *p;
    if (path[2] == SEP && path[1] == '.')
      p = &path[0];
    else
      p = &path[2];
    for (; *p; p++)
      {
	if (*p == SEP)
	  {
	    if (p[1] == '.' && p[2] == '.' && (p[3] == SEP || p[3] == '\0'))
	      {
                char *q;
		for (q = p - 1; q >= path && *q != SEP; q--);
		if (q[0] == SEP && (q[1] != '.' || q[2] != '.' || q[3] != SEP)
		    && (q - 1 <= path || q[-1] != SEP))
		  {
		    strcpy (q, p + 3);
		    if (path[0] == '\0')
		      {
			path[0] = SEP;
			path[1] = '\0';
		      }
#ifdef wx_msw
		    /* MATTHEW: check that path[2] is NULL! */
		    else if (path[1] == ':' && !path[2])
		      {
			path[2] = SEP;
			path[3] = '\0';
		      }
#endif
		    p = q - 1;
		  }
	      }
	    else if (p[1] == '.' && (p[2] == SEP || p[2] == '\0'))
	      strcpy (p, p + 2);
	  }
      }
  }
  return path;
}

// Must be destroyed [@@@ new func]
char *wxCopyAbsolutePath(const char *filename)
{
  if (filename == NULL)
    return NULL;

  if (! IsAbsolutePath(wxExpandPath(wxBuffer, filename))) {
    char    buf[_MAXPATHLEN];
    buf[0] = '\0';
    wxGetWorkingDirectory(buf, sizeof(buf)/sizeof(char));
    char ch = buf[strlen(buf) - 1];
#ifdef wx_msw
    if (ch != '\\' && ch != '/')
	strcat(buf, "\\");
#else
    if (ch != '/')
	strcat(buf, "/");
#endif
    strcat(buf, wxBuffer);
    return copystring( wxRealPath(buf) );
  }
  return copystring( wxBuffer );
}

/*-
 Handles:
   ~/ => home dir
   ~user/ => user's home dir
   If the environment variable a = "foo" and b = "bar" then:
   Unix:
	$a	=>	foo
	$a$b	=>	foobar
	$a.c	=>	foo.c
	xxx$a	=>	xxxfoo
	${a}!	=>	foo!
	$(b)!	=>	bar!
	\$a	=>	\$a
   MSDOS:
	$a	==>	$a
	$(a)	==>	foo
	$(a)$b	==>	foo$b
	$(a)$(b)==>	foobar
	test.$$	==>	test.$$
 */

/* input name in name, pathname output to buf. */

char *wxExpandPath(char *buf, const char *name)
{
    register char  *d, *s, *nm;
    char            lnm[_MAXPATHLEN];
    int            q;

    // Some compilers don't like this line.
//    const char      trimchars[] = "\n \t";

    char      trimchars[4];
    trimchars[0] = '\n';
    trimchars[1] = ' ';
    trimchars[2] = '\t';
    trimchars[3] = 0;

#ifdef wx_msw
     const char     SEP = '\\';
#else
     const char     SEP = '/';
#endif
    buf[0] = '\0';
    if (name == NULL || *name == '\0')
	return buf;
    nm = copystring(name); // Make a scratch copy
    char *nm_tmp = nm;

    /* Skip leading whitespace and cr */
    while (strchr((char *)trimchars, *nm) != NULL)
	nm++;
    /* And strip off trailing whitespace and cr */
    s = nm + (q = strlen(nm)) - 1;
    while (q-- && strchr((char *)trimchars, *s) != NULL)
	*s = '\0';

    s = nm;
    d = lnm;
#ifdef wx_msw
    q = FALSE;
#else
    q = nm[0] == '\\' && nm[1] == '~';
#endif

    /* Expand inline environment variables */
    while (*d++ = *s) {
#ifndef wx_msw
	if (*s == '\\') {
	    if (*(d - 1) = *++s) {
		s++;
		continue;
	    } else
		break;
	} else
#endif
#ifdef wx_msw
	if (*s++ == '$' && (*s == '{' || *s == ')'))
#else
	if (*s++ == '$')
#endif
	{
	    register char  *start = d;
	    register        braces = (*s == '{' || *s == '(');
	    register char  *value;
	    while (*d++ = *s)
		if (braces ? (*s == '}' || *s == ')') : !(isalnum(*s) || *s == '_') )
		    break;
		else
		    s++;
	    *--d = 0;
	    value = getenv(braces ? start + 1 : start);
	    if (value) {
		for (d = start - 1; *d++ = *value++;);
		d--;
		if (braces && *s)
		    s++;
	    }
	}
    }

    /* Expand ~ and ~user */
    nm = lnm;
    s = "";
    if (nm[0] == '~' && !q)
    {
	/* prefix ~ */
	if (nm[1] == SEP || nm[1] == 0)
        {	/* ~/filename */
	    if ((s = wxGetUserHome(NULL)) != NULL) {
		if (*++nm)
		    nm++;
	    }
        } else
        {		/* ~user/filename */
	    register char  *nnm;
	    register char  *home;
	    for (s = nm; *s && *s != SEP; s++);
	    int was_sep; /* MATTHEW: Was there a separator, or NULL? */
            was_sep = (*s == SEP);
	    nnm = *s ? s + 1 : s;
	    *s = 0;
	    if ((home = wxGetUserHome(nm + 1)) == NULL) {
               if (was_sep) /* replace only if it was there: */
 		  *s = SEP;
		s = "";
	    } else {
		nm = nnm;
		s = home;
	    }
	}
    }

    d = buf;
    if (s && *s) { /* MATTHEW: s could be NULL if user '~' didn't exist */
	/* Copy home dir */
	while ('\0' != (*d++ = *s++))
	  /* loop */;
	// Handle root home
	if (d - 1 > buf && *(d - 2) != SEP)
	  *(d - 1) = SEP;
    }
    s = nm;
    while (*d++ = *s++);

    delete[] nm_tmp; // clean up alloc
    /* Now clean up the buffer */
    return wxRealPath(buf);
}


/* Contract Paths to be build upon an environment variable
   component:

   example: "/usr/openwin/lib", OPENWINHOME --> ${OPENWINHOME}/lib

   The call wxExpandPath can convert these back!
 */
char *
wxContractPath (const char *filename, const char *envname, const char *user)
{
  static char dest[_MAXPATHLEN];

  if (filename == NULL)
    return NULL;

  strcpy (dest, filename);
#ifdef wx_msw
  Unix2DosFilename(dest);
#endif

  // Handle environment
  char *val, *tcp;
  if (envname != NULL && (val = getenv (envname)) != NULL &&
     (tcp = strstr (dest, val)) != NULL)
    {
        strcpy (wxBuffer, tcp + strlen (val));
        *tcp++ = '$';
        *tcp++ = '{';
        strcpy (tcp, envname);
        strcat (tcp, "}");
        strcat (tcp, wxBuffer);
    }

  // Handle User's home (ignore root homes!)
  size_t len;
  if ((val = wxGetUserHome (user)) != NULL &&
      (len = strlen(val)) > 2 &&
      strncmp(dest, val, len) == 0)
    {
      strcpy(wxBuffer, "~");
      if (user && *user)
	strcat(wxBuffer, user);
#ifdef wx_msw
//      strcat(wxBuffer, "\\");
#else
//      strcat(wxBuffer, "/");
#endif
      strcat(wxBuffer, dest + len);
      strcpy (dest, wxBuffer);
    }

  return dest;
}

// Return just the filename, not the path
// (basename)
char *
wxFileNameFromPath (char *path)
{
  if (path)
    {
      register char *tcp;

      tcp = path + strlen (path);
      while (--tcp >= path)
	{
	  if (*tcp == '/' || *tcp == '\\'
#ifdef VMS
     || *tcp == ':' || *tcp == ']')
#else
     )
#endif
	    return tcp + 1;
	}			/* while */
#ifdef wx_msw
      if (isalpha (*path) && *(path + 1) == ':')
	return path + 2;
#endif
    }
  return path;
}

// Return just the directory, or NULL if no directory
char *
wxPathOnly (char *path)
{
  if (path && *path)
    {
      static char buf[_MAXPATHLEN];

      // Local copy
      strcpy (buf, path);

      int l = strlen(path);
      Bool done = FALSE;

      int i = l - 1;

      // Search backward for a backward or forward slash
      while (!done && i > -1)
      {
        // ] is for VMS
        if (path[i] == '/' || path[i] == '\\' || path[i] == ']')
        {
          done = TRUE;
#ifdef VMS
          buf[i+1] = 0;
#else
          buf[i] = 0;
#endif

          return buf;
        }
        else i --;
      }

/* there's a bug here somewhere, so replaced with my original code.
      char *tcp;
      // scan back
      for (tcp = &buf[strlen (buf) - 1]; tcp >= buf; tcp--)
	{
	  // Search for Unix or Dos path sep {'\\', '/'}
	  if (*tcp == '\\' || *tcp == '/')
	    {
	      *tcp = '\0';
	      return buf;
	    }
	}			// for()
*/
#ifdef wx_msw
      // Try Drive specifier
      if (isalpha (buf[0]) && buf[1] == ':')
	{
	  // A:junk --> A:. (since A:.\junk Not A:\junk)
	  buf[2] = '.';
	  buf[3] = '\0';
	  return buf;
	}
#endif
    }

  return NULL;
}

// Utility for converting delimiters in DOS filenames to UNIX style
// and back again - or we get nasty problems with delimiters.
// Also, convert to lower case, since case is significant in UNIX.

void 
wxDos2UnixFilename (char *s)
{
  if (s)
    while (*s)
      {
	if (*s == '\\')
	  *s = '/';
#ifdef wx_msw
	else
	  *s = _wxToLower (*s);	// Case INDEPENDENT
#endif
	s++;
      }
}

void 
wxUnix2DosFilename (char *s)
{
// Yes, I really mean this to happen under DOS only! JACS
#ifdef wx_msw
  if (s)
    while (*s)
      {
	if (*s == '/')
	  *s = '\\';
	s++;
      }
#endif
}

// Return the current date/time
// [volatile]
char *wxNow( void )
{
  time_t now = time(NULL);
  char *date = ctime(&now); 
  date[24] = '\0';
  return date;
}

/* Get Full RFC822 style email address */
Bool
wxGetEmailAddress (char *address, int maxSize)
{
  char host[65];
  char user[65];

  if (wxGetHostName(host, 64) == FALSE)
    return FALSE;
  if (wxGetUserId(user, 64) == FALSE)
    return FALSE;

  char tmp[130];
  strcpy(tmp, user);
  strcat(tmp, "@");
  strcat(tmp, host);

  strncpy(address, tmp, maxSize - 1);
  address[maxSize-1] = '\0';
  return TRUE;
}

// Concatenate two files to form third
Bool 
wxConcatFiles (const char *file1, const char *file2, const char *file3)
{
  char *outfile = wxGetTempFileName("cat");

  FILE *fp1 = NULL;
  FILE *fp2 = NULL;
  FILE *fp3 = NULL;
  // Open the inputs and outputs
  if ((fp1 = fopen (file1, "rb")) == NULL ||
      (fp2 = fopen (file2, "rb")) == NULL ||
      (fp3 = fopen (outfile, "wb")) == NULL)
    {
      if (fp1)
	fclose (fp1);
      if (fp2)
	fclose (fp2);
      if (fp3)
	fclose (fp3);
      return FALSE;
    }

  int ch;
  while ((ch = getc (fp1)) != EOF)
    (void) putc (ch, fp3);
  fclose (fp1);

  while ((ch = getc (fp2)) != EOF)
    (void) putc (ch, fp3);
  fclose (fp2);

  fclose (fp3);
  Bool result = wxRenameFile(outfile, file3);
  delete[] outfile;
  return result;
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
    (void) putc (ch, fd2);

  fclose (fd1);
  fclose (fd2);
  return TRUE;
}

Bool 
wxRenameFile (const char *file1, const char *file2)
{
  // Normal system call
  if (0 == rename (file1, file2))
    return TRUE;
  // Try to copy
  if (wxCopyFile(file1, file2)) {
    wxRemoveFile(file1);
    return TRUE;
  }
  // Give up
  return FALSE;
}

/*
 * Strip out any menu codes
 */

char *wxStripMenuCodes (char *in, char *out)
{
  if (!in)
    return NULL;
    
  if (!out)
    out = copystring(in);

  char *tmpOut = out;
  
  while (*in)
    {
      if (*in == '&')
	{
	  // Check && -> &, &x -> x
	  if (*++in == '&')
	    *out++ = *in++;
	}
      else if (*in == '\t')
	{
          // Remove all stuff after \t in X mode, and let the stuff as is
          // in Windows mode.
          // Accelerators are handled in wx_item.cc for Motif, and are not
          // YET supported in XView
	  break;
	}
      else
	*out++ = *in++;
    }				// while

  *out = '\0';

  return tmpOut;
}


/*
 * Window search functions
 *
 */

/*
 * If parent is non-NULL, look through children for a label or title
 * matching the specified string. If NULL, look through all top-level windows.
 *
 */

static wxWindow *wxFindWindowByLabel1 (char *title, wxWindow * parent);

wxWindow *
wxFindWindowByLabel (char *title, wxWindow * parent)
{
  if (parent)
    {
      return wxFindWindowByLabel1 (title, parent);
    }
  else
    {
      for (wxNode * node = wxTopLevelWindows.First (); node; node = node->Next ())
	{
	  wxWindow *win = (wxWindow *) node->Data ();
	  wxWindow *retwin = wxFindWindowByLabel1 (title, win);
	  if (retwin)
	    return retwin;
	}			// for()

    }
  return NULL;
}

// Recursive
static wxWindow *
wxFindWindowByLabel1 (char *title, wxWindow * parent)
{
  if (parent)
    {
      char *lab = parent->GetLabel ();
      if (lab && StringMatch (title, lab))
	return parent;
    }

  if (parent)
    {
      for (wxNode * node = parent->GetChildren()->First (); node; node = node->Next ())
	{
	  wxWindow *win = (wxWindow *) node->Data ();
	  wxWindow *retwin = wxFindWindowByLabel1 (title, win);
	  if (retwin)
	    return retwin;
	}			// for()

    }

  return NULL;			// Not found

}

/*
 * If parent is non-NULL, look through children for a name
 * matching the specified string. If NULL, look through all top-level windows.
 *
 */

static wxWindow *wxFindWindowByName1 (char *title, wxWindow * parent);

wxWindow *
wxFindWindowByName (char *title, wxWindow * parent)
{
  if (parent)
    {
      return wxFindWindowByName1 (title, parent);
    }
  else
    {
      for (wxNode * node = wxTopLevelWindows.First (); node; node = node->Next ())
	{
	  wxWindow *win = (wxWindow *) node->Data ();
	  wxWindow *retwin = wxFindWindowByName1 (title, win);
	  if (retwin)
	    return retwin;
	}			// for()

    }
  // Failed? Try by label instead.
  return wxFindWindowByLabel(title, parent);
}

// Recursive
static wxWindow *
wxFindWindowByName1 (char *title, wxWindow * parent)
{
  if (parent)
    {
      char *lab = parent->GetName ();
      if (lab && (strcmp(title, lab) == 0))
	return parent;
    }

  if (parent)
    {
      for (wxNode * node = parent->GetChildren()->First (); node; node = node->Next ())
	{
	  wxWindow *win = (wxWindow *) node->Data ();
	  wxWindow *retwin = wxFindWindowByName1 (title, win);
	  if (retwin)
	    return retwin;
	}			// for()

    }

  return NULL;			// Not found

}

// Returns menu item id or -1 if none.
int 
wxFindMenuItemId (wxFrame * frame, char *menuString, char *itemString)
{
  wxMenuBar *menuBar = frame->GetMenuBar ();
  if (!menuBar)
    return -1;
  return menuBar->FindMenuItem (menuString, itemString);
}

/*
 * wxDebugStreamBuf
 */

// Doesn't work for the Metrowerks compiler (why not?)
#if !defined(__MWERKS__) && !defined(WXUSINGDLL)

wxDebugStreamBuf::wxDebugStreamBuf(void)
{
  if (allocate()) setp(base(),ebuf());
}

int wxDebugStreamBuf::overflow(int WXUNUSED(i))
{
  int len = pptr() - pbase();
  char *txt = new char[len+1];
  strncpy(txt, pbase(), len);
  txt[len] = '\0';
#ifdef wx_msw
  OutputDebugString((LPCSTR)txt);
#else
  fprintf(stderr, txt);
#endif
  setp(pbase(), epptr());
  delete[] txt;
  return EOF;
}

int wxDebugStreamBuf::sync(void)
{
  int len = pptr() - pbase();
  char *txt = new char[len+1];
  strncpy(txt, pbase(), len);
  txt[len] = '\0';
#ifdef wx_msw
  OutputDebugString((LPCSTR)txt);
#else
  fprintf(stderr, txt);
#endif
  setp(pbase(), epptr());
  delete[] txt;
  return 0;
}

#endif

/*
On Fri, 21 Jul 1995, Paul Craven wrote:

> Is there a way to find the path of running program's executable? I can get
> my home directory, and the current directory, but I don't know how to get the
> executable directory.
> 

The code below (warty as it is), does what you want on most Unix,
DOS, and Mac platforms (it's from the ALS Prolog main).

|| Ken Bowen      Applied Logic Systems, Inc.         PO Box 180,     
||====            Voice:  +1 (617)965-9191            Newton Centre,
||                FAX:    +1 (617)965-1636            MA  02159  USA
                  Email:  ken@als.com        WWW: http://www.als.com
------------------------------------------------------------------------
*/

// This code is commented out but it may be integrated with wxWin at
// a later date, after testing. Thanks Ken!
#if 0

/*--------------------------------------------------------------------*
 | whereami is given a filename f in the form:  whereami(argv[0])
 | It returns the directory in which the executable file (containing 
 | this code [main.c] ) may be found.  A dot will be returned to indicate 
 | the current directory.
 *--------------------------------------------------------------------*/

static void
whereami(name)
    char *name;
{
    register char *cutoff = NULL;	/* stifle -Wall */
    register char *s;
    register char *t;
    int   cc;
    char  ebuf[4096];

    /*
     * See if the file is accessible either through the current directory
     * or through an absolute path.
     */

    if (access(name, R_OK) == 0) {

	/*-------------------------------------------------------------*
	 * The file was accessible without any other work.  But the current
	 * working directory might change on us, so if it was accessible
	 * through the cwd, then we should get it for later accesses.
	 *-------------------------------------------------------------*/

	t = imagedir;
	if (!absolute_pathname(name)) {
#if defined(DOS) || defined(WIN32)
	    int   drive;
	    char *newrbuf;

	    newrbuf = imagedir;
#ifndef __DJGPP__
	    if (*(name + 1) == ':') {
		if (*name >= 'a' && *name <= 'z')
		    drive = (int) (*name - 'a' + 1);
		else
		    drive = (int) (*name - 'A' + 1);
		*newrbuf++ = *name;
		*newrbuf++ = *(name + 1);
		*newrbuf++ = DIR_SEPARATOR;
	    }
	    else {
		drive = 0;
		*newrbuf++ = DIR_SEPARATOR;
	    }
	    if (getcwd(newrbuf, drive) == 0) {	/* } */
#else
	    if (getcwd(newrbuf, 1024) == 0) {	/* } */
#endif
#else  /* DOS */
#ifdef HAVE_GETWD
	    if (getwd(imagedir) == 0) {		/* } */
#else  /* !HAVE_GETWD */
	    if (getcwd(imagedir, 1024) == 0) {
#endif /* !HAVE_GETWD */
#endif /* DOS */
		fatal_error(FE_GETCWD, 0);
	    }
	    for (; *t; t++)	/* Set t to end of buffer */
		;
	    if (*(t - 1) == DIR_SEPARATOR)	/* leave slash if already
						 * last char
						 */
		cutoff = t - 1;
	    else {
		cutoff = t;	/* otherwise put one in */
		*t++ = DIR_SEPARATOR;
	    }
	}
#if (!defined(MacOS) && !defined(_DJGPP__) && !defined(__GO32__) && !defined(WIN32))
	else
		(*t++ = DIR_SEPARATOR);
#endif

	/*-------------------------------------------------------------*
	 * Copy the rest of the string and set the cutoff if it was not
	 * already set.  If the first character of name is a slash, cutoff
	 * is not presently set but will be on the first iteration of the
	 * loop below.
	 *-------------------------------------------------------------*/

	for ((*name == DIR_SEPARATOR ? (s = name+1) : (s = name));;) {
	    if (*s == DIR_SEPARATOR)
			cutoff = t;
	    if (!(*t++ = *s++))
			break;
	}

    }
    else {

	/*-------------------------------------------------------------*
	 * Get the path list from the environment.  If the path list is
	 * inaccessible for any reason, leave with fatal error.
	 *-------------------------------------------------------------*/

#ifdef MacOS
	if ((s = getenv("Commands")) == (char *) 0)
#else
	if ((s = getenv("PATH")) == (char *) 0)
#endif
	    fatal_error(FE_PATH, 0);

	/*
	 * Copy path list into ebuf and set the source pointer to the
	 * beginning of this buffer.
	 */

	strcpy(ebuf, s);
	s = ebuf;

	for (;;) {
	    t = imagedir;
	    while (*s && *s != PATH_SEPARATOR)
		*t++ = *s++;
	    if (t > imagedir && *(t - 1) == DIR_SEPARATOR) 
		;		/* do nothing -- slash already is in place */
	    else
		*t++ = DIR_SEPARATOR;	/* put in the slash */
	    cutoff = t - 1;	/* set cutoff */
	    strcpy(t, name);
	    if (access(imagedir, R_OK) == 0)
		break;

	    if (*s)
		s++;		/* advance source pointer */
	    else
		fatal_error(FE_INFND, 0);
	}

    }

    /*-------------------------------------------------------------*
     | At this point the full pathname should exist in imagedir and
     | cutoff should be set to the final slash.  We must now determine
     | whether the file name is a symbolic link or not and chase it down
     | if it is.  Note that we reuse ebuf for getting the link.
     *-------------------------------------------------------------*/

#ifdef HAVE_SYMLINK
    while ((cc = readlink(imagedir, ebuf, 512)) != -1) {
	ebuf[cc] = 0;
	s = ebuf;
	if (*s == DIR_SEPARATOR) {
	    t = imagedir;
	}
	else {
	    t = cutoff + 1;
	}
	for (;;) {
	    if (*s == DIR_SEPARATOR)
		cutoff = t;	/* mark the last slash seen */
	    if (!(*t++ = *s++))	/* copy the character */
		break;
	}
    }

#endif /* HAVE_SYMLINK */

    strcpy(imagename, cutoff + 1);	/* keep the image name */
    *(cutoff + 1) = 0;		/* chop off the filename part */
}

#endif
