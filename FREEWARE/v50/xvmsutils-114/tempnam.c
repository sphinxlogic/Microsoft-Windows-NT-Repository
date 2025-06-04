/*---------------------------------------------------------------------------*/
/* The origianl source for this comes from the mxrn distribution and         */
/* was developed by Jonathan I. Kamens          <jik@pit-manager.mit.edu>    */
/*                                                                           */ 
/* I only added a few VMS specific things and did some clean-up.             */
/* 950627 Martin P.J. Zinser (M.Zinser@gsi.de or eurmpz@eur.sas.com )        */
/*---------------------------------------------------------------------------*/

#include <types.h>
#include <stat.h>
#include <file.h>
#include <stdio.h>
#include <unixlib.h>
#include <unixio.h>
#include <X11/Intrinsic.h>

#include "unix_types.h"

#define P_tmpdir	"sys$scratch:"

#define utStrlen(s)     ((s) ? strlen(s) : 0)

static char check_directory(dir)
char *dir;
{
     struct stat statbuf;

     if (! dir)
	  return 0;
     else if (stat(dir, &statbuf) < 0)
	  return 0;
     else if ((statbuf.st_mode & S_IFMT) != S_IFDIR)
	  return 0;
     else if (access(dir, W_OK | X_OK) < 0)
	  return 0;
     else
	  return 1;
}

/* function for creating temporary filenames */
char *tempnam(dir, pfx)
char *dir, *pfx;
{
     extern char *getenv();
     char *tmpdir = NULL, *env, *filename;
     static char unique_letters[4] = "AAA";
     char addcolon = 0;
     
     /*
      * If a directory is passed in, verify that it exists and is a
      * directory and is writeable by this process.  If no directory
      * is passed in, or if the directory that is passed in does not
      * exist, check the environment variable TMPDIR.  If it isn't
      * set, check the predefined constant P_tmpdir.  If that isn't
      * set, use "sys$scratch".
      */

     if (dir && check_directory(dir))
	  tmpdir = dir;
     else if ((env = getenv("TMPDIR")) && check_directory(env))
	  tmpdir = env;
     else if (check_directory(P_tmpdir))
	  tmpdir = P_tmpdir;
     else
	  tmpdir = "sys$scratch:";
     
     /*
      * OK, now that we've got a directory, figure out whether or not
      * there's a colon at the end of it.
      */
     if (tmpdir[strlen(tmpdir) - 1] != ':')
	  addcolon = 1;

     /*
      * Now figure out the set of unique letters.
      */
     unique_letters[0]++;
     if (unique_letters[0] > 'Z') {
	  unique_letters[0] = 'A';
	  unique_letters[1]++;
	  if (unique_letters[1] > 'Z') {
	       unique_letters[1] = 'A';
	       unique_letters[2]++;
	       if (unique_letters[2] > 'Z') {
		    unique_letters[2]++;
	       }
	  }
     }

     /*
      * Allocate a string of sufficient length.
      */
     filename = XtMalloc(strlen(tmpdir) + addcolon + utStrlen(pfx) + 10);
     
     /*
      * And create the string.
      */
     (void) sprintf(filename, "%s%s%s%sa%05d", tmpdir, addcolon ? ":" : "",
		    pfx ? pfx : "", unique_letters, getpid());

     return filename;
}
