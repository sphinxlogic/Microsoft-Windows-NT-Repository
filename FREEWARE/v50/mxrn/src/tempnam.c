#include "config.h"
#include "utils.h"

/* from Jonathan I. Kamens          <jik@pit-manager.mit.edu> */

#ifdef NEED_TEMPNAM

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <stdio.h>

#ifndef P_tmpdir
#define P_tmpdir	"/usr/tmp/"
#endif

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
char *utTempnam(dir, pfx)
char *dir, *pfx;
{
     extern char *getenv();
     char *tmpdir = NULL, *env, *filename;
     static char unique_letters[4] = "AAA";
     char addslash = 0;
     
     /*
      * If a directory is passed in, verify that it exists and is a
      * directory and is writeable by this process.  If no directory
      * is passed in, or if the directory that is passed in does not
      * exist, check the environment variable TMPDIR.  If it isn't
      * set, check the predefined constant P_tmpdir.  If that isn't
      * set, use "/tmp/".
      */

     if (dir && check_directory(dir))
	  tmpdir = dir;
     else if ((env = getenv("TMPDIR")) && check_directory(env))
	  tmpdir = env;
#ifdef P_tmpdir
     else if (check_directory(P_tmpdir))
	  tmpdir = P_tmpdir;
#endif
     else
	  tmpdir = "/tmp/";
     
     /*
      * OK, now that we've got a directory, figure out whether or not
      * there's a slash at the end of it.
      */
     if (tmpdir[strlen(tmpdir) - 1] != '/')
	  addslash = 1;

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
     filename = XtMalloc(strlen(tmpdir) + addslash + utStrlen(pfx) + 10);
     
     /*
      * And create the string.
      */
     (void) sprintf(filename, "%s%s%s%sa%05d", tmpdir, addslash ? "/" : "",
		    pfx ? pfx : "", unique_letters, getpid());

     return filename;
}

#endif
