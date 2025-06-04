/*
 * File:      wx_utils.cc
 * Purpose:     Various utilities (X version)
 * Author:      Julian Smart
 * Created:     1993
 * Updated:	August 1994
 * RCS_ID:      $Id: wx_utils.cc,v 1.64 1994/11/03 00:23:48 edz Exp edz $
 * Copyright:   (c) 1993, AIAI, University of Edinburgh
 */

// $Log: wx_utils.cc,v $
// Revision 1.64  1994/11/03  00:23:48  edz
// Fixed OS Version function for XView, Motif still needs to
// be done. The function returned the X protocol version and not
// the Window OS version.
//
// Revision 1.63  1994/11/02  22:38:00  edz
// Slight modification to file find first/next code.
//
// Revision 1.62  1994/11/02  22:18:13  edz
// Modification to wxGetWorkingDirectory().
//
// Revision 1.61  1994/11/02  12:31:07  edz
// Improved (made consistent) the algorithm for the determination of
// the Resource Database directory.
//
// Revision 1.60  1994/11/02  12:12:30  edz
// Sync with reference.
//
//

static const char sccsid[] = "%W% %G%";

#ifdef __GNUG__
#pragma implementation
#pragma implementation "wx_utils.h"
#endif

#include <fstream.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "wx_defs.h"

#include "wx_setup.h"
#include "wx_utils.h"
#include "wx_main.h"
#include "wx_dialg.h"
#include "wx_text.h"
#include "wx_cmdlg.h"

#if USE_IOSTREAMH
#include <iostream.h>
#else
#include <iostream>
#endif

// Sun CC compatibility (interference with xview/pkg.h, apparently...)
#if defined(SUN_CC) && defined(wx_xview)
#undef va_start
#undef va_end
#undef va_arg
#undef va_list
#endif

#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>

#if defined(__alpha) && !defined(VMS)
#ifdef __cplusplus
extern "C" {
#endif
#include <sys/systeminfo.h>
#ifdef __cplusplus
}
#endif
#endif

#ifdef wx_motif
#if !(defined(VMS) || defined(linux) || defined(__sgi) || defined(__hpux) || defined(__ultrix)) || defined(__bsdi__)
#include <tiuser.h>
#endif
#endif

#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include <X11/Xutil.h>
/* Experimental removal: gives string.h conflict on Sun/gcc 2.6.3 */
// #include <X11/Xos.h>
#include <X11/Xatom.h>

#include <sys/types.h>
#include <sys/time.h>

#ifdef VMS
/*steve*/
#ifdef __HIDE_FORBIDDEN_NAMES
#undefine __HIDE_FORBIDDEN_NAMES
#endif
#include <socket.h>
#ifdef VAX
/*because 'noshare' is not valid in vax C++*/
#define CC$VAXCSHR 1
#endif
#include <unixlib.h>
#define unlink DELETE

#else

#if defined(_AIX) || defined(__xlC__)
#include <sys/socket.h>
#include <sys/select.h>
#else
#ifndef DG
#include <sys/syscall.h>
#endif
#endif

#include <sys/wait.h>
#include <unistd.h>
#include <dirent.h>
#include <pwd.h>

#endif

#include <sys/file.h>

// Folks: I put this in for the benefit of Solaris + SunPro compiler,
// whose sys/signal.h didn't do the trick.
// If this screws things up, we probably need to test for
// #if (defined(SVR4) && defined(sun)
// instead.

#ifdef SVR4
#include <signal.h>
#else
#include <sys/signal.h>
#endif

#ifdef __sgi
#include <signal.h>
#include <sys/signal.h>
#endif

#if defined(__osf__)
extern "C" {
#include <netdb.h>
}
#else
#include <netdb.h>
#endif

#ifndef _MAXPATHLEN
#define _MAXPATHLEN 1024
#endif

#ifdef wx_xview
#include <xview/canvas.h>
#endif

#if USE_RESOURCES
#ifdef wx_xview
#define use_xview_code
#include <xview/defaults.h>
#endif
#endif

#ifdef sun
# ifndef __GNUG__
#  ifndef SUN_CC
#   define SUN_CC 1
#  endif
# endif
#endif

// Yuck this is really BOTH site and platform dependent
// so we should use some other strategy!
#ifdef sun
# define DEFAULT_XRESOURCE_DIR "/usr/openwin/lib/app-defaults"
#else
# define DEFAULT_XRESOURCE_DIR "/usr/lib/X11/app-defaults"
#endif

#if !(defined(DG) || defined(_AIX) || defined(__xlC__) || defined(VMS) || defined(SUN_CC) || defined(__CLCC__) || defined(__hpux) || defined(__osf__) || defined(SVR4))
extern "C"
{
  int select (int, fd_set *, fd_set *, fd_set *, struct timeval *);
  int sigsetmask (int);
#if !defined(sun)
  int sigblock (int);
#endif
#ifndef __FreeBSD__
  int vfork();
#endif
//  long int vfork(); // YOU MAY NEED *THIS* INSTEAD OF THE ABOVE
#if !defined(__GNUG__) && !defined(__sgi)
  int gethostname (char *host, unsigned const namelen);
#endif
}
#endif

#ifdef __osf__  
extern "C" int gethostname(char *host , int len);
extern "C" struct passwd *getpwuid(uid_t uid);
extern "C" uid_t getuid(void);
#endif

#if defined(SVR4) && !defined(__hpux)
#include <sys/systeminfo.h>
#endif

#if (defined(SUN_CC) || defined(__CLCC__))
#include <sysent.h>
#endif

#ifdef wx_xview
#include <xview/screen.h>
#include <xview/server.h>
#include <xview/notify.h>
extern Xv_Server xview_server;
#endif

#ifndef use_xview_code
static XrmDatabase wxResourceDatabase = 0;
#endif

#if USE_RESOURCES
void wxXMergeDatabases (wxApp * theApp, Display * display);
#endif

static char *GetIniFile (char *dest, const char *filename);

// Get fully qualified hostname e.g. foo.bar.edu
Bool 
wxGetHostName (char *buf, int maxSize)
{
#if defined(SVR4) && !defined(__hpux)
  return (sysinfo (SI_HOSTNAME, buf, maxSize) != -1);
#else /* BSD Sockets */
  char name[255];
  struct hostent *h;

  // Get hostname
  if (gethostname (name, sizeof (name) / sizeof (char) - 1) == -1)
      return FALSE;
  // Get official full name of host
  strncpy (buf
	   ,(h = gethostbyname (name)) != NULL ? h->h_name : name
	   ,maxSize - 1);
  return TRUE;
#endif
}

// Get user ID e.g. jacs
Bool 
wxGetUserId (char *buf, int maxSize)
{
#ifdef VMS
  *buf = '\0'; // return empty string
  return FALSE;
#else
  struct passwd *who;

  if ((who = getpwuid (getuid ())) != NULL)
    {
      strncpy (buf, who->pw_name, maxSize - 1);
      return TRUE;
    }
  return FALSE;
#endif
}

// Get user name e.g. Julian Smart
Bool 
wxGetUserName (char *buf, int maxSize)
{
#ifdef VMS
  *buf = '\0'; // return empty string
  return FALSE;
#else
  struct passwd *who;

  if ((who = getpwuid (getuid ())) != NULL)
    {
      strncpy (buf, who->pw_gecos, maxSize - 1);
      return TRUE;
    }
  return FALSE;
#endif
}

// Execute a command (e.g. another program) in a
// system-independent manner.

long wxExecute (char **argv, Bool sync)
{
#ifdef VMS
  return(0);
#else
  if (*argv == NULL)
    return 0;	// Nothing???
  // Run a program the recomended way under X (XView) 

  /* fork the process */
#if defined(sun) || defined(__ultrix) || defined(__bsdi__)
  pid_t pid = vfork ();
#else
  pid_t pid = fork ();
#endif
  if (pid == -1)
    {
      return 0;
    }
  else if (pid == 0)
    {
      /* child */
#ifdef _AIX
      execvp ((const char *)*argv, (const char **)argv);
#else
      execvp (*argv, argv);
#endif
      if (errno == ENOENT)
	printf ("%s: command not found\n", *argv);
      else
	perror (*argv);
      printf ("wxWindows: could not execute '%s'\n", *argv);
      _exit (-1);
    }

#ifdef wx_xview
  static Notify_client sys_client = 42;
  notify_set_wait3_func (sys_client, (Notify_func) notify_default_wait3, pid);
#endif
  // Code below is NOT really acceptable!
  // One should NEVER use wait under X
  // Ideas? A Sleep idle callback?

  // WARNING: WARNING: WARNING: WARNING:
  // The CODE BELOW IS BAD BAD BAD BAD!
  if (sync)
    {
      int status;

      waitpid( pid, &status, 0 );

      if (WIFEXITED(status) != 0)
        return WEXITSTATUS(status);
    }

  return pid;
#endif
  // end VMS
}


long wxExecute (const char *command, Bool sync)
{
#ifdef VMS
  return(0);
#else
  if (command == NULL || *command == '\0')
    return 0; // Nothing to do

  // Run a program the recomended way under X (XView) 
  int argc = 0;
  char *argv[127];
  char tmp[1024];
  const char *IFS = " \t\n";

  // Build argument vector 
  strncpy (tmp, command, sizeof (tmp) / sizeof (char) - 1);
  tmp[sizeof (tmp) / sizeof (char) - 1] = '\0';
  argv[argc++] = strtok (tmp, IFS);
  while ((argv[argc++] = strtok (NULL, IFS)) != NULL)
    /* loop */ ;

  return wxExecute(argv, sync);
#endif
  // VMS
}

int wxKill(long pid, int sig)
{
  int unixSignal = 0;
  switch (sig)
  {
    case wxSIGTERM:
    default:
      unixSignal = SIGTERM;
  }
  return kill( (int)pid, unixSignal);
}

//
// Execute a program in an Interactive Shell
//
Bool
wxShell(const char *command)
{
#ifdef VMS
  return(FALSE);
#else
#if defined(sun) || defined(__ultrix) || defined(__bsdi__)
  pid_t pid = vfork ();
#else
  pid_t pid = fork ();
#endif
  switch( pid ) {
    case -1:			/* error */
	return(FALSE);
    case 0:			/* child */
#ifdef wx_xview
	execlp("shelltool", "-c", (char *) command, NULL);
#else
	// Generic X windows terminal window
	if (command && *command)
	  execlp("xterm", "-e", (char *) command, NULL);
	else
	  execlp("xterm", NULL);
#endif
	_exit(127);
  }
#ifdef wx_xview
  static Notify_client sys_client = 42;
  notify_set_wait3_func (sys_client, (Notify_func) notify_default_wait3, pid);
#endif
  return TRUE;
#endif
 // End VMS
}

// Get a temporary filename, opening and closing the file.
char *wxGetTempFileName (const char *prefix, char *dest)
{
  static short last_temp = 0;	// cache last to speed things a bit
  // At most 1000 temp files to a process! We use a ring count.
  char buf[64];

  for (short suffix = last_temp + 1; suffix != last_temp; ++suffix %= 1000)
    {
      sprintf (buf, "/tmp/%s%d.%03x", prefix, (int) getpid (), (int) suffix);
      if (!wxFileExists (buf))
	{
	  // Touch the file to create it (reserve name)
	  FILE *fd = fopen (buf, "w");
	  if (fd)
	    fclose (fd);
	  last_temp = suffix;
          if (dest)
	    strcpy(dest, buf);
	  else
	    dest = copystring(buf);
	  return dest;
	}
    }
  cerr << "wxWindows: error finding temporary file name.\n";
  if (dest) dest[0] = 0;
  return NULL;
}

Bool 
wxRemoveFile (const char *file)
{
#ifdef VMS
  // unlink doesn't work on some ALPHAs
  // therefore a badbadbad solution
  // but it works!
  char cmd[256];
  sprintf(cmd,"delete %s;*",file);
  system(cmd);
  return(TRUE);
#else
  return ((unlink (file) == 0) ? TRUE : FALSE);
#endif
}

Bool 
wxMkdir (const char *dir)
{
  // give default perms of owner read and write, group read and
  // others read. The interface to this func should be changed
  // to pass the perms info in.
  // Since directory it must also be searchable @@@
  // Added S_IXUSR | S_IXGRP | S_IXOTH
#ifdef VMS
  return FALSE;
#else
  return (mkdir (dir, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) == 0);
#endif
}

Bool 
wxRmdir (const char *dir, int WXUNUSED(flags))
{
#ifdef VMS
  return FALSE;
#else
  return (rmdir (dir) == 0);
#endif
}

Bool 
wxDirExists (const char *dir)
{
#ifdef VMS
  return FALSE;
#else
  struct stat sbuf;
  return (stat(dir, &sbuf) != -1) && S_ISDIR(sbuf.st_mode) ? TRUE : FALSE;
#endif
}


// Get first file name matching given wild card.
// Flags are reserved for future use.

#ifndef VMS
static DIR *wxDirStream = NULL;
static char *wxFileSpec = NULL;
static int wxFindFileFlags = 0;
#endif

char *wxFindFirstFile(const char *spec, int flags)
{
#ifndef VMS
  if (wxDirStream)
    closedir(wxDirStream); // edz 941103: better housekeping

  wxFindFileFlags = flags;

  if (wxFileSpec)
    delete[] wxFileSpec;
  wxFileSpec = copystring(spec);

  // Find path only so we can concatenate
  // found file onto path
  char *p = wxPathOnly(wxFileSpec);

  /* MATTHEW: special case: path is really "/" */
  if (p && !*p && *wxFileSpec == '/')
    p = "/";
  /* MATTHEW: p is NULL => Local directory */
  if (!p)
    p = "";
    
  if ((wxDirStream=opendir(p))==NULL)
    return NULL;

 /* MATTHEW: [5] wxFindNextFile can do the rest of the work */
  return wxFindNextFile();
#endif
 // ifndef VMS
  return NULL;
}

char *wxFindNextFile(void)
{
#ifndef VMS
  static char buf[400];

  /* MATTHEW: [2] Don't crash if we read too many times */
  if (!wxDirStream)
    return NULL;

  // Find path only so we can concatenate
  // found file onto path
  char *p = wxPathOnly(wxFileSpec);
  char *n = wxFileNameFromPath(wxFileSpec);

  /* MATTHEW: special case: path is really "/" */
  if (p && !*p && *wxFileSpec == '/')
    p = "/";

  // Do the reading
  struct dirent *nextDir;
  for (nextDir = readdir(wxDirStream); nextDir != NULL; nextDir = readdir(wxDirStream))
  {

    /* MATTHEW: [5] Only return "." and ".." when they match, and only return
       directories when flags & wxDIR */
    if (wxMatchWild(n, nextDir->d_name)) {
      Bool isdir;

      if ((strcmp(nextDir->d_name, ".") == 0) ||
	  (strcmp(nextDir->d_name, "..") == 0)) {
	if (wxFindFileFlags && !(wxFindFileFlags & wxDIR))
	  continue;
	isdir = TRUE;
      } else
	isdir = wxDirExists(buf);
      
      buf[0] = 0;
      if (p && *p) {
        strcpy(buf, p);
        if (strcmp(p, "/") != 0)
          strcat(buf, "/");
      }
      strcat(buf, nextDir->d_name);

      if (!wxFindFileFlags
	  || ((wxFindFileFlags & wxDIR) && isdir)
	  || ((wxFindFileFlags & wxFILE) && !isdir))
	return buf;
    }
  }
  closedir(wxDirStream);
  wxDirStream = NULL;
#endif
 // ifndef VMS

  return NULL;
}

// Get current working directory.
// If buf is NULL, allocates space using new, else
// copies into buf.
char *wxGetWorkingDirectory(char *buf, int sz)
{
  if (!buf)
    buf = new char[sz+1];
  if (getcwd(buf, sz) == NULL) {
    buf[0] = '.';
    buf[1] = '\0';
  }
  return buf;
}

Bool wxSetWorkingDirectory(char *d)
{
  return (chdir(d) == 0);
}

// Get free memory in bytes, or -1 if cannot determine amount (e.g. on UNIX)
long 
wxGetFreeMemory (void)
{
  return -1;
}

// Sleep for nSecs seconds.
// XView implementation according to the Heller manual
void 
wxSleep (int nSecs)
{
#if defined(__sgi) || defined(VMS) || defined(sun) // Added || defined(sun) (R.Oddy's advice)
  sleep (nSecs);
#else
#if defined(SVR4)
//  struct sigset_t oldmask, mask;
  sigset_t oldmask, mask;
  struct timeval tv;

  tv.tv_sec = nSecs;
  tv.tv_usec = 0;

  sigemptyset (&mask);
  sigaddset (&mask, SIGIO);
  sigaddset (&mask, SIGALRM);
  sigprocmask (SIG_BLOCK, &mask, &oldmask);
  if ((select (0, 0, 0, 0, &tv)) == -1)
    {
      perror ("select in wxSleep");
    }
//  sigprocmask(SIG_BLOCK, &oldmask, (sigset_t *) NULL); // Bug according to Kari
  sigprocmask (SIG_SETMASK, &oldmask, (sigset_t *) NULL);
#else
  int oldmask, mask;
  struct timeval tv;

  tv.tv_sec = nSecs;
  tv.tv_usec = 0;

  mask = sigmask (SIGIO);
  mask |= sigmask (SIGALRM);
  oldmask = sigblock (mask);
  if ((select (0, 0, 0, 0, &tv)) == -1)
    {
      perror ("select in wxSleep");
    }
  sigsetmask (oldmask);
#endif
#endif // __sgi
}

// Consume all events until no more left
#ifdef wx_xview
extern "C" int xv_input_pending (Display *, int);
extern "C" int ndis_dispatch (void);
#endif

void 
wxFlushEvents (void)
{
  Display *display = wxGetDisplay(); /* MATTHEW: [4] Always use GetDisplay */

#ifdef wx_motif
  XSync (display, FALSE);
  XEvent event;
  // XtAppPending returns availability of events AND timers/inputs, which
  // are processed via callbacks, so XtAppNextEvent will not return if 
  // there are no events. So added '& XtIMXEvent' - Sergey.
  while (XtAppPending (wxTheApp->appContext) & XtIMXEvent)
    {
      XFlush (XtDisplay (wxTheApp->topLevel));
      XtAppNextEvent (wxTheApp->appContext, &event);
      XtDispatchEvent (&event);
    }
#endif
#ifdef wx_xview
  XSync (display, FALSE);
  XFlush (display);
/* Causes nasty internal problems. Pity, I thought I'd cracked it...
   while(XPending(display))
   {
   XEvent event;
   XPeekEvent(display, &event);
   xv_input_pending(display, 0);
   ndis_dispatch();
   }
 */
  // My try.... (edz)
  xv_set(xview_server, SERVER_SYNC_AND_PROCESS_EVENTS, NULL);
#endif
}

// Output a debug mess., in a system dependent fashion.

void 
wxDebugMsg (const char *fmt...)
{
// #ifndef __sgi
  va_list ap;
  char buffer[BUFSIZ];

  if (!wxTheApp->wantDebugOutput)
    return ;

  va_start (ap, fmt);

  vsprintf (buffer, fmt, ap);
  cerr << buffer;

  va_end (ap);
// #else
//  cerr << "Error: cannot use variable-argument functions on SGI!\n";
// #endif
}

// Non-fatal error: write error and continue
void 
wxError (const char *msg, const char *title)
{
  cerr << title << ": " << msg << "\n";
}

// Fatal error: pop up message box and abort
void 
wxFatalError (const char *msg, const char *title)
{
  cerr << title << ": " << msg << "\n";
  exit (1);
}

// Emit a beeeep...

void 
wxBell ()
{
  Display *display = wxGetDisplay();

  // Use current setting for the bell
  XBell (display, 0);
}

int 
wxGetOsVersion (int *majorVsn, int *minorVsn)
{
#ifdef wx_xview
  Display *display = wxGetDisplay();

  // Edward, xview_version not defined!
  if (majorVsn)
    *majorVsn = ProtocolVersion (display);
  if (minorVsn)
    *minorVsn = ProtocolRevision (display);

/*
  // Fetch Version of XView (not X11)
  if (majorVsn)
    *majorVsn = xview_version / 1000;
  if (minorVsn)
    *minorVsn = xview_version % 1000;
*/
  return wxXVIEW_X;
#elif defined(wx_motif)
  // This code is WRONG!! Does NOT return the
  // Motif version of the libs but the X protocol
  // version! @@@@@ Fix ME!!!!!!!!!
  Display *display = XtDisplay (wxTheApp->topLevel);
  if (majorVsn)
    *majorVsn = ProtocolVersion (display);
  if (minorVsn)
    *minorVsn = ProtocolRevision (display);
  return wxMOTIF_X;
#endif
}

// Reading and writing resources (eg WIN.INI, .Xdefaults)

#if USE_RESOURCES

static char *
GetResourcePath(char *buf, char *name, Bool create = FALSE)
{
  if (create && FileExists (name) ) {
    strcpy(buf, name);
    return buf; // Exists so ...
  }

  if (*name == '/')
    strcpy(buf, name);
  else {
    // Put in standard place for resource files if not absolute
#ifdef wx_xview
    // OpenWindows home dir (don't know about relative Motif)
    char *openwin = getenv("OPENWINHOME");
    if (openwin) {
      strcpy(buf, openwin);
      strcat(buf, "/lib/app-defaults");
    } else
#endif
      strcpy (buf, DEFAULT_XRESOURCE_DIR);
    strcat (buf, "/");
    strcat (buf, FileNameFromPath (name));
  }

  if (create) {
    // Touch the file to create it
    FILE *fd = fopen (buf, "w");
    if (fd) fclose (fd);
  }
  return buf;
}

/*
 * We have a cache for writing different resource files,
 * which will only get flushed when we call wxFlushResources().
 * Build up a list of resource databases waiting to be written.
 *
 */

wxList wxResourceCache (wxKEY_STRING);

void 
wxFlushResources (void)
{
  char nameBuffer[512];

  wxNode *node = wxResourceCache.First ();
  while (node)
    {
      char *file = node->key.string;
      // If file doesn't exist, create it first.
      (void)GetResourcePath(nameBuffer, file, TRUE);

      XrmDatabase database = (XrmDatabase) node->Data ();
      XrmPutFileDatabase (database, nameBuffer);
      XrmDestroyDatabase (database);
      wxNode *next = node->Next ();
      delete node;
      node = next;
    }
}

Bool 
wxWriteResource (const char *section, const char *entry, char *value, const char *file)
{
  char buffer[500];

  (void) GetIniFile (buffer, file);

  XrmDatabase database;
  wxNode *node = wxResourceCache.Find (buffer);
  if (node)
    database = (XrmDatabase) node->Data ();
  else
    {
      database = XrmGetFileDatabase (buffer);
      wxResourceCache.Append (buffer, (wxObject *) database);
    }

  char resName[300];
  strcpy (resName, section);
  strcat (resName, ".");
  strcat (resName, entry);

  XrmPutStringResource (&database, resName, value);
  return TRUE;
}

Bool 
wxWriteResource (const char *section, const char *entry, float value, const char *file)
{
  char buf[50];
  sprintf (buf, "%.4f", value);
  return wxWriteResource (section, entry, buf, file);
}

Bool 
wxWriteResource (const char *section, const char *entry, long value, const char *file)
{
  char buf[50];
  sprintf (buf, "%ld", value);
  return wxWriteResource (section, entry, buf, file);
}

Bool 
wxWriteResource (const char *section, const char *entry, int value, const char *file)
{
  char buf[50];
  sprintf (buf, "%d", value);
  return wxWriteResource (section, entry, buf, file);
}

Bool 
wxGetResource (const char *section, const char *entry, char **value, const char *file)
{
#ifdef use_xview_code		/* @@@ */
  // New code using Xview
  char buf[1024];
  char *result;
  static int main_loaded = FALSE;

  /* MATTHEW: [4] Much faster... */
  if (!main_loaded || file) {
    defaults_load_db (GetIniFile (buf, file));
    if (!file)
      main_loaded = TRUE;
  }

  strcpy (buf, section);
  strcat (buf, ".");
  strcat (buf, entry);
  result = (char *) defaults_get_string (buf, buf, "$$default");
  if (strcmp (result, "$$default") == 0)
    return FALSE;
  if (*value)
    delete[] *value;
  *value = copystring (result);
  return TRUE;
#else // Old code for XView and code for Motif
  if (!wxResourceDatabase)
    {
      Display *display = wxGetDisplay();
      wxXMergeDatabases (wxTheApp, display);
    }

  XrmDatabase database;

  if (file)
    {
      char buffer[500];
      
      // Is this right? Trying to get it to look in the user's
      // home directory instead of current directory -- JACS
      (void) GetIniFile (buffer, file);

      wxNode *node = wxResourceCache.Find (buffer);
      if (node)
	database = (XrmDatabase) node->Data ();
      else
	{
	  database = XrmGetFileDatabase (buffer);
	  wxResourceCache.Append (buffer, (wxObject *) database);
	}
/*
      wxNode *node = wxResourceCache.Find (file);
      if (node)
	database = (XrmDatabase) node->Data ();
      else
	{
	  database = XrmGetFileDatabase (file);
	  wxResourceCache.Append (file, (wxObject *) database);
	}
*/
    }
  else
    database = wxResourceDatabase;

  XrmValue xvalue;
  char *str_type[20];
  char buf[150];
  strcpy (buf, section);
  strcat (buf, ".");
  strcat (buf, entry);

  Bool success = XrmGetResource (database, buf, "*", str_type,
				 &xvalue);
  // Try different combinations of upper/lower case, just in case...
  if (!success)
    {
      buf[0] = (isupper (buf[0]) ? tolower (buf[0]) : toupper (buf[0]));
      success = XrmGetResource (database, buf, "*", str_type,
				&xvalue);
    }
  if (success)
    {
      if (*value)
        delete[] *value;

      *value = new char[xvalue.size + 1];
      strncpy (*value, xvalue.addr, (int) xvalue.size);
      return TRUE;
    }
  return FALSE;
#endif // use_xview_code
}


Bool 
wxGetResource (const char *section, const char *entry, float *value, const char *file)
{
  char *s = NULL;
  Bool succ = wxGetResource (section, entry, &s, file);
  if (succ)
    {
      *value = (float) strtod (s, NULL);
      delete[]s;
      return TRUE;
    }
  else
    return FALSE;
}

Bool 
wxGetResource (const char *section, const char *entry, long *value, const char *file)
{
  char *s = NULL;
  Bool succ = wxGetResource (section, entry, &s, file);
  if (succ)
    {
      *value = strtol (s, NULL, 10);
      delete[]s;
      return TRUE;
    }
  else
    return FALSE;
}

Bool 
wxGetResource (const char *section, const char *entry, int *value, const char *file)
{
  char *s = NULL;
  Bool succ = wxGetResource (section, entry, &s, file);
  if (succ)
    {
      // Handle True, False here 
      // True, Yes, Enables, Set or  Activated 
      if (*s == 'T' || *s == 'Y' || *s == 'E' || *s == 'S' || *s == 'A')
	*value = TRUE;
      // False, No, Disabled, Reset, Cleared, Deactivated
      else if (*s == 'F' || *s == 'N' || *s == 'D' || *s == 'R' || *s == 'C')
	*value = FALSE;
      // Handle as Integer
      else
	*value = (int) strtol (s, NULL, 10);

      delete[]s;
      return TRUE;
    }
  else
    return FALSE;
}

#ifdef XXXX
#ifdef wx_motif
/*
 * Not yet used but may be useful.
 *
 */
void 
wxSetDefaultResources (const Widget w, const char **resourceSpec, const char *name)
{
  int i;
  Display *dpy = XtDisplay (w);	// Retrieve the display pointer

  XrmDatabase rdb = NULL;	// A resource data base

  // Create an empty resource database
  rdb = XrmGetStringDatabase ("");

  // Add the Component resources, prepending the name of the component

  i = 0;
  while (resourceSpec[i] != NULL)
    {
      char buf[1000];

      sprintf (buf, "*%s%s", name, resourceSpec[i++]);
      XrmPutLineResource (&rdb, buf);
    }

  // Merge them into the Xt database, with lowest precendence

  if (rdb)
    {
#if (XlibSpecificationRelease>=5)
      XrmDatabase db = XtDatabase (dpy);
      XrmCombineDatabase (rdb, &db, FALSE);
#else
      XrmMergeDatabases (dpy->db, &rdb);
      dpy->db = rdb;
#endif
    }
}
#endif
#endif // 0


#ifndef use_xview_code
/*
 * Merging defaults databases. We need to find resource information
 * from various sources and merge them before we query resources.
 *
 */

void 
wxXMergeDatabases (wxApp * theApp, Display * display)
{
  XrmDatabase homeDB, serverDB, applicationDB;
  char filenamebuf[1024];

  char *filename = &filenamebuf[0];
  char *environment;
  char *classname = theApp->wx_class;
  char name[256];
  (void) strcpy (name, "/usr/lib/X11/app-defaults/");
  (void) strcat (name, classname);

  /* Get application defaults file, if any */
  applicationDB = XrmGetFileDatabase (name);
  (void) XrmMergeDatabases (applicationDB, &wxResourceDatabase);

  /* Merge server defaults, created by xrdb, loaded as a property of the root
   * window when the server initializes and loaded into the display
   * structure on XOpenDisplay;
   * if not defined, use .Xdefaults
   */

  if (XResourceManagerString (display) != NULL)
    {
      serverDB = XrmGetStringDatabase (XResourceManagerString (display));
    }
  else
    {
      (void) GetIniFile (filename, NULL);
      serverDB = XrmGetFileDatabase (filename);
    }
  XrmMergeDatabases (serverDB, &wxResourceDatabase);

  /* Open XENVIRONMENT file, or if not defined, the .Xdefaults,
   * and merge into existing database
   */

  if ((environment = getenv ("XENVIRONMENT")) == NULL)
    {
      size_t len;
      environment = GetIniFile (filename, NULL);
      len = strlen (environment);
#if defined(SVR4) && !defined(__hpux)
      (void) sysinfo (SI_HOSTNAME, environment + len, 1024 - len);
#else
      (void) gethostname (environment + len, 1024 - len);
#endif
    }
  homeDB = XrmGetFileDatabase (environment);
  XrmMergeDatabases (homeDB, &wxResourceDatabase);
}

#endif // !use_xview_code

#endif /* USE_RESOURCES */

// Read $HOME for what it says is home, if not
// read $USER or $LOGNAME for user name else determine
// the Real User, then determine the Real home dir.
static char *
GetIniFile (char *dest, const char *filename)
{
  char *home = NULL;
  if (filename && wxIsAbsolutePath(filename))
  {
    strcpy(dest, filename);
  }
  else if ((home = wxGetUserHome(NULL)) != NULL)
  {
    strcpy(dest, home);
    if (dest[strlen(dest) - 1] != '/')
      strcat (dest, "/");
    if (filename == NULL)
      {
        if ((filename = getenv ("XENVIRONMENT")) == NULL)
          filename = ".Xdefaults";
      }
    else if (*filename != '.')
      strcat (dest, ".");
    strcat (dest, filename);
  } else
  {
    dest[0] = '\0';    
  }
  return dest;
}

// SOON History.... 
char *wxGetHomeDir(char *dest)
{
#ifdef VMS
    *dest = '\0'; // return an empty string
#else
  char *ptr = wxGetUserHome(NULL);
  strcpy(dest, ptr && *ptr ? ptr : "/");
#endif
 // ifdef VMS
  return dest;
}

// Read $HOME for what it says is home, if not
// read $USER or $LOGNAME for user name else determine
// the Real User, then determine the Real home dir.
char *wxGetUserHome (const char *user)
{
#ifdef VMS
  return(NULL);
#else
  struct passwd *who = NULL;

  if (user == NULL || *user == '\0') {
    register char *ptr;

    if ((ptr = getenv("HOME")) != NULL) 
      return ptr;
    if ((ptr = getenv("USER")) != NULL ||
	(ptr = getenv("LOGNAME")) != NULL)
      {
	who = getpwnam( ptr );
      }
    // We now make sure the the user exists!
    if (who == NULL)
      who = getpwuid( getuid() );
  } else
    who = getpwnam (user);

  return who ? who->pw_dir : NULL;
#endif
 // ifdef VMS
}

/* MATTHEW: configurable display in Motif */
#ifdef wx_motif
static Display *wx_current_display = NULL;
static char *wx_display_name = NULL;
#endif

Display *wxGetDisplay(void)
{
#ifdef wx_motif
  if (wx_current_display)
    return wx_current_display;

  return XtDisplay (wxTheApp->topLevel);
#endif
#ifdef wx_xview
  Xv_Screen screen = xv_get (xview_server, SERVER_NTH_SCREEN, 0);
  Xv_opaque root_window = xv_get (screen, XV_ROOT);
  return (Display *) xv_get (root_window, XV_DISPLAY);
#endif
}

/* MATTHEW: [4] Added wxSetDisplay and wxGetDisplayName */
Bool wxSetDisplay(char *display_name)
{
#ifdef wx_motif
  if (!display_name) {
    wx_current_display = NULL;
    if (wx_display_name)
      delete[] wx_display_name;
    wx_display_name = NULL;
    return TRUE;
  } else {
    Cardinal argc = 0;

    Display *display = XtOpenDisplay(wxTheApp->appContext,
				     display_name,
				     wxTheApp->appName,
				     wxTheApp->wx_class,
				     NULL,
# if XtSpecificationRelease < 5
				     0, &argc, NULL);
# else
				     0, (int *)&argc, NULL);
# endif

    if (display) {
      wx_display_name = copystring(display_name);
      wx_current_display = display;
      return TRUE;
    } else
      return FALSE;
  }
#endif
    return FALSE;
}

char *wxGetDisplayName(void)
{
#ifdef wx_motif
  return wx_display_name;
#else
  return NULL;
#endif
}

// Helper function for XView
#ifdef wx_xview
static void SetXCursor(wxWindow *win, wxCursor *cursor)
{
  Display *dpy = win->GetXDisplay();
  Window xwin = win->GetXWindow();
  if (cursor)
  {
    if (cursor->x_cursor)
    {
      if (cursor->use_raw_x_cursor)
      {
        XDefineCursor(dpy, xwin, cursor->x_cursor);
      }
      else
      {
        Xv_opaque x_win = (Xv_opaque)win->handle;
        if (win->IsKindOf(CLASSINFO(wxCanvas)))
        {
          Xv_Window win2 = xv_get(x_win, CANVAS_NTH_PAINT_WINDOW, 0);

          xv_set(win2, WIN_CURSOR, cursor->x_cursor, NULL);
        }
        else
          xv_set(x_win, WIN_CURSOR, cursor->x_cursor, NULL);
      }
    }
  }
  else
  {
    XSetWindowAttributes attrs;
    attrs.cursor = None;
    XChangeWindowAttributes (dpy, xwin, CWCursor, &attrs);
  }
}
#endif

// Old cursor
static int wxBusyCursorCount = FALSE;

// Helper function
static void 
wxXSetBusyCursor (wxWindow * win, wxCursor * cursor)
{
  Display *display = win->GetXDisplay();

#ifdef wx_motif
  Window xwin = win->GetXWindow();
  XSetWindowAttributes attrs;

  if (cursor)
    {
      attrs.cursor = cursor->GetXCursor(display);
    }
  else
    {
      // Restore old cursor
      if (win->wx_cursor)
	attrs.cursor = win->wx_cursor->GetXCursor(display);
      else
	attrs.cursor = None;
    }
  if (xwin)
    XChangeWindowAttributes (display, xwin, CWCursor, &attrs);
#endif
#ifdef wx_xview
  if (cursor)
    SetXCursor(win, cursor);
  else
    SetXCursor(win, win->wx_cursor);
#endif

  XFlush (display);

  // Forget old cursor if we're resetting
  //  if (!cursor)
  //    win->currentWindowCursor = 0;

  for(wxNode *node = win->GetChildren()->First (); node; node = node->Next())
    {
      wxWindow *child = (wxWindow *) node->Data ();
      if (child->IsKindOf(CLASSINFO(wxFrame)) ||
	  child->IsKindOf(CLASSINFO(wxCanvas)) ||
	  child->IsKindOf(CLASSINFO(wxPanel)) ||
	  child->IsKindOf(CLASSINFO(wxTextWindow)))
	wxXSetBusyCursor (child, cursor);
    }
}

// Set the cursor to the busy cursor for all windows
void 
wxBeginBusyCursor (wxCursor * cursor)
{
  wxBusyCursorCount++;
  if (wxBusyCursorCount == 1)
    {
      for(wxNode *node = wxTopLevelWindows.First (); node; node = node->Next())
	{
	  wxWindow *win = (wxWindow *) node->Data ();
	  wxXSetBusyCursor (win, cursor);
	}
    }
}

// Restore cursor to normal
void 
wxEndBusyCursor (void)
{
  if (wxBusyCursorCount == 0)
    return;

  wxBusyCursorCount--;
  if (wxBusyCursorCount == 0)
    {
      for(wxNode *node = wxTopLevelWindows.First (); node; node = node->Next())
	{
	  wxWindow *win = (wxWindow *) node->Data ();
	  wxXSetBusyCursor (win, NULL);
	}
    }
}

// TRUE if we're between the above two calls
Bool 
wxIsBusy (void)
{
  return (wxBusyCursorCount > 0);
}

/*
 * Some colour manipulation routines
 */
 
void wxHSVToXColor(wxHSV *hsv,XColor *rgb)
   {
     int h = hsv->h;
     int s = hsv->s;
     int v = hsv->v;
     int r, g, b;
     int i, f;
     int p, q, t;
     s = (s * wxMAX_RGB) / wxMAX_SV;
     v = (v * wxMAX_RGB) / wxMAX_SV;
     if (h == 360) h = 0;
     if (s == 0) { h = 0; r = g = b = v; }
     i = h / 60;
     f = h % 60;
     p = v * (wxMAX_RGB - s) / wxMAX_RGB;
     q = v * (wxMAX_RGB - s * f / 60) / wxMAX_RGB;
     t = v * (wxMAX_RGB - s * (60 - f) / 60) / wxMAX_RGB;
     switch (i) 
        {
          case 0: r = v, g = t, b = p; break;
          case 1: r = q, g = v, b = p; break;
          case 2: r = p, g = v, b = t; break;
          case 3: r = p, g = q, b = v; break;
          case 4: r = t, g = p, b = v; break;
          case 5: r = v, g = p, b = q; break;
        }
     rgb->red = r << 8;
     rgb->green = g << 8;
     rgb->blue = b << 8;
   }

void wxXColorToHSV(wxHSV *hsv,XColor *rgb)
   {
     int r = rgb->red >> 8;
     int g = rgb->green >> 8;
     int b = rgb->blue >> 8;
     int maxv = wxMax3(r, g, b);
     int minv = wxMin3(r, g, b);
     int h, s, v;
     v = maxv;
     if (maxv) s = (maxv - minv) * wxMAX_RGB / maxv;
     else s = 0;
     if (s == 0) h = 0;
     else 
        {
	      int rc, gc, bc, hex;
	      rc = (maxv - r) * wxMAX_RGB / (maxv - minv);
	      gc = (maxv - g) * wxMAX_RGB / (maxv - minv);
	      bc = (maxv - b) * wxMAX_RGB / (maxv - minv);
	      if (r == maxv) { h = bc - gc, hex = 0; } 
	      else if (g == maxv) { h = rc - bc, hex = 2; } 
	           else if (b == maxv) { h = gc - rc, hex = 4; }
	      h = hex * 60 + (h * 60 / wxMAX_RGB);
	      if (h < 0) h += 360;
        }
     hsv->h = h;
     hsv->s = (s * wxMAX_SV) / wxMAX_RGB;
     hsv->v = (v * wxMAX_SV) / wxMAX_RGB;
   }

void wxAllocNearestColor(Display *d,Colormap cmp,XColor *xc)
   {
     int llp;

     int screen = DefaultScreen(d);
     int num_colors = DisplayCells(d,screen);

     XColor *color_defs = new XColor[num_colors];
     for(llp = 0;llp < num_colors;llp++) color_defs[llp].pixel = llp;
     XQueryColors(d,cmp,color_defs,num_colors);

     wxHSV hsv_defs, hsv;
     wxXColorToHSV(&hsv,xc);

     int diff, min_diff, pixel = 0;

     for(llp = 0;llp < num_colors;llp++)
        {
          wxXColorToHSV(&hsv_defs,&color_defs[llp]);
          diff = wxSIGN(wxH_WEIGHT * (hsv.h - hsv_defs.h)) +
                 wxSIGN(wxS_WEIGHT * (hsv.s - hsv_defs.s)) +
                 wxSIGN(wxV_WEIGHT * (hsv.v - hsv_defs.v));
          if (llp == 0) min_diff = diff;
          if (min_diff > diff) { min_diff = diff; pixel = llp; }
          if (min_diff == 0) break;
        }

     xc -> red = color_defs[pixel].red;
     xc -> green = color_defs[pixel].green;
     xc -> blue = color_defs[pixel].blue;
     xc -> flags = DoRed | DoGreen | DoBlue;
     if (!XAllocColor(d,cmp,xc))
        cout << "wxAllocNearestColor : Warning : Cannot find nearest color !\n";

     delete color_defs;
   }

void wxAllocColor(Display *d,Colormap cmp,XColor *xc)
   {
     if (!XAllocColor(d,cmp,xc))
        {
//          cout << "wxAllocColor : Warning : Can not allocate color, attempt find nearest !\n";
          wxAllocNearestColor(d,cmp,xc);
        }
   }


// Check whether this window wants to process messages, e.g. Stop button
// in long calculations.
Bool wxCheckForInterrupt(wxWindow *wnd)
{
#ifdef wx_motif
	if(wnd){
		Display *dpy=wnd->GetXDisplay();
		Window win=wnd->GetXWindow();
		XEvent event;
		XFlush(dpy);
		if(wnd->handle){
			XmUpdateDisplay((Widget)(wnd->handle));
		}
		while(XCheckMaskEvent(dpy,
							ButtonPressMask|ButtonReleaseMask|ButtonMotionMask|
							PointerMotionMask|KeyPressMask|KeyReleaseMask,
							&event)){
			if(event.xany.window==win)
				XtDispatchEvent(&event);
	//		else
	//			XBell(dpy,50);
		}
		return TRUE;//*** temporary?
	}
	else{
		wxMessageBox("wnd==NULL !!!");
		return FALSE;//*** temporary?
	}
#else
  return FALSE;
#endif
}


/* Jorel Jean-Charles jjorel@silr.ireste.fr
   Motif2.0 patch for Linux ELF 1.2.13   */

// Change this to 1 if you have problems with fstat/stat.
#if 0

#ifdef wx_motif
#ifdef __linux__
#ifdef __ELF__
#include <Xm/Xm.h>
#if XmVersion == 2000 // Version 2.0.0

struct stat;

int fstat(int __fd, struct stat * __statbuf)
{
  return _fxstat(1, __fd, __statbuf);
}

int stat(const char * __path, struct stat * __statbuf)
{
  return _xstat(1, __path, __statbuf);
}

#endif // Motif Version
#endif // __ELF__
#endif // __linux__
#endif // wx_motif

#endif

void wxGetMousePosition(int* x, int* y)
{
    XMotionEvent xev;
    Window root, child;
    XQueryPointer(wxGetDisplay(),
                  DefaultRootWindow(wxGetDisplay()), &root, &child,
                  &(xev.x_root), &(xev.y_root),
                  &(xev.x),      &(xev.y),
                  &(xev.state));
    *x = xev.x_root;
    *y = xev.y_root;
}

