/*
 * File:	main.cc
 * Purpose:	Stub for calling main wxWindows entry point
 *              NOTE: to use a C main instead, compile main.c.
 * Author:	Julian Smart
 * Created:	1995
 * Updated:	1995
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

extern int wxEntry(int argc, char *argv[]);

#ifdef _LINUX_DLL
#define main __linux_dll_main
#endif

/* Can include some code to setup the
   environment and do a few things "before"
   firing up wxWindows
*/

// Some compilers need main() to be defined in the main application
// file (e.g. hello.cc). In which case, use IMPLEMENT_WXWIN_MAIN
// in your main application files (see wb_main.h).

#if !(defined(AIX) || defined(AIX4)) /* || defined(__hpux)) */

/* Main Stub */
int
main(int argc, char *argv[])
{
  /* Main entry stub */
  return wxEntry(argc, argv);
}

#endif
 /* End AIX */
