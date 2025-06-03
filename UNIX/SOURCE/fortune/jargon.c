/* Quick and dirty jargon file browser.
 * Greg Lehey, LEMIS, 5 February 1994
 */
#include <unistd.h>
#ifdef __svr4__
#include <stdio.h>
#endif
#ifndef JARGFILE
#define JARGFILE "/usr/games/fortune/jargon.txt"	    /* do this by environment variable XXX */
#endif
#define LINESIZE 132					    /* 80 should probably do */
main (int argc, char *argv [])
{
  char line [LINESIZE];
  FILE *jarg;
  int subject_length;
  
  if (argc != 2)
    {
    printf ("Usage:\n\tjargon <keyword>");
    exit (0);
    }
  subject_length = strlen (argv [1]);
  if (! (jarg = fopen (JARGFILE, "r")))			    /* can't open? */
    {
    char message [LINESIZE];
    sprintf (message, "Can't open jargon file %s", JARGFILE);
    perror (message);
    exit (1);
    }
  do
    {
    if (! fgets (line, LINESIZE, jarg))
      {
      if (feof (jarg))
	{
	printf ("Couldn't find an entry for %s\n", argv [1]);
	exit (1);
	}
      else
	{
	char message [LINESIZE];
	sprintf (message, "Error reading jargon file %s", JARGFILE);
	perror (message);
	exit (1);
	}
      }
    }
  while ((*line != ':') || memcmp (&line [1], argv [1], subject_length));
  do
    {
    printf ("%s", line);
    if (! fgets (line, LINESIZE, jarg))
      {
      if (feof (jarg))
	exit (0);
      else
	{
	char message [LINESIZE];
	sprintf (message, "Error reading jargon file %s", JARGFILE);
	perror (message);
	exit (1);
	}
      }
    }
  while ((*line != ':') || (! memcmp (&line [1], argv [1], subject_length)));
  exit (0);
  }
