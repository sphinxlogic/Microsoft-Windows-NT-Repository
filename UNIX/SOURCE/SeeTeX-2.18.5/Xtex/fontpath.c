#include <stdio.h>
#include <sys/param.h>
#include <ctype.h>
#include <X11/Xlib.h>

/* X toolkit header files */

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>

/* Widget header files. */

/*
 * Insure that a single directory is in the font path
 *
 * Made changes to take care of duplication of directories caused by :
 *    (a) Presence/absence of ending slahes in path components
 *    (b) Confusion due to links
 * Shankar Ramamoorthy 04 / 07 / 92
 */

#if defined (__hpux) || defined (__svr4__)
#define getwd(d) getcwd(d, MAXPATHLEN)
#endif

extern char *ProgName;

static int
fontPathInsureSingleDirectory(dpy, path)
     Display *dpy;
     char *path;
{
  int ncurrent;
  int i;
  char currentDir[MAXPATHLEN], absPath[MAXPATHLEN], 
	  fpCompAbsPath[MAXPATHLEN];
  char **currentList = XGetFontPath (dpy, &ncurrent);
  if (!currentList) {
    fprintf (stderr, "%s:  unable to get old font path.\n",
	     ProgName);
    return(0);
  }
  
  /* Get the current working directory and save it */
  (void) getwd (currentDir);

  /* Get the absolute pathname of directory to check */
  if (chdir (path) == 0)
    (void) getwd (absPath);
  else
    {
    /* Unable to change to directory */
    XFreeFontPath (currentList);
    return (0);
    }

  for ( i= 0; i < ncurrent; i++ ) {
    /* Change to next component on fontpath */
    if (chdir (currentList[i]) == 0) {
      (void) getwd (fpCompAbsPath);
      if (strcmp (fpCompAbsPath, absPath) == 0) {
	/* Its there.
	 * Restore the current working directory and return.
	 */
        chdir (currentDir);
        XFreeFontPath (currentList);
        return (1);
      }
    }
  }

  /* Restore current working directory */
  chdir (currentDir);
  
  /* Wasn't found, so install it at the beginning */
  
  {
    char **newList = (char **) malloc( sizeof( char *) * (ncurrent + 1) );

    for (i = 0 ; i < ncurrent; i++ ) {
      newList[i] = currentList[i];
    }
    newList[ncurrent] = path;

    XSetFontPath(dpy, newList, ncurrent + 1);
    free( newList );
    XFreeFontPath( currentList );
  }
}

fontPathInsurePaths(dpy, stringOfPaths)
     Display *dpy;
     char *stringOfPaths;
{

  /* make a copy of stringOfPaths, because we modify it */

  int len = strlen( stringOfPaths );

  if (len > 0) {
    char *newString = (char *) malloc( sizeof(char ) * (len + 1) );
    char *start, *end;
    int done = FALSE;

    strcpy(newString, stringOfPaths);

    start = end = newString;

    done = (start == 0 || *start == 0);

    while ( !done ) {

      int atEnd;

      while ( start && isspace(*start) ) start++;
      end = start;
      while ( end && *end != 0 && ! (*end == ',' || *end == ':') ) end++;
      done = (*end == 0);

      /* terminate string, check it's non-null and shove to X server */

      *end = 0;
      if ( strlen(start) >  0) {
	fontPathInsureSingleDirectory( dpy, start );
      }

      start = end + 1;

    }

    /* delete copy we allocated */

    free( newString );
  }
}
