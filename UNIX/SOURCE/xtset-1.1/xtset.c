/*
  A new version of the XTERM window title program written in C rather than
  shell script for more flexibility.

  Martin Smith, msmith@lssec.bt.co.uk

  Modified by Greg Lehey, LEMIS, 4 June 1993, to include partial file names
  and terminal name.

  Further modified by Greg Lehey, 30 June 1993, to replace home directory by ~
    
  Passed a string to set the window title and / or icon name to and this string
  is text except for % characters which introduce fields like printf.

  %h is translated to hostname
  %d is translated to current dir
  %D is translated to current dir, only last part (unless this is src, man, doc, etc., in which
     case the previous part is included as well) (Greg Lehey, 4 June 1993)
  %u is current user name
  %g is current group name
  %U is current real user name
  %G is current real group name
  %m is octal current umask
  %e is value of environment variable XTXTRA or null string
  %t is the complete name of the stdin terminal, if it is a terminal
  %T is the last part of the stdin file name, if it is a terminal
  %% is a real percent sign, God knows why you'd want one but there you are

  So for instance the string:

  "%d@%h (%u:%g)" might expand to "/home/sol/msmith@rheya (msmith:group2)"

  command line arguments are:

  -t to update window title
  -i to icon name

  (if neither of these are given, both are done)

  The program exits silently if the TERM variable is not set to xterm
*/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pwd.h>
#include <grp.h>
#include <sys/param.h>

#ifdef __svr4__
#define getwd(x) getcwd (x, MAXPATHLEN)
#endif

extern struct passwd *getpwuid ();
extern struct group *getgrgid ();

#define REQUIRED_TERM "xterm"
#define ESC (char) 27
#define BELL (char) 7

typedef char *String;
typedef FILE *Stream;

char winstr[1024];

/* Modify behaviour to set both titles (Grog, 4 June 1993) */
int want_icon = 1;
int want_title = 1;

char our_dir[MAXPATHLEN];
char *our_tty;						    /* pointer into ttyname's () static storage */
char our_host[40];

char our_umask[10];

String real_userid;
String eff_userid;
String real_groupid;
String eff_groupid;
String home_directory;					    /* home directory or real user */

/*
 * Modify a current directory string, there's not much point printing /tmp_mnt
 * on the front if its there.
 */

void 
process_dirstring (str)
     String str;

{
  int homelen = strlen (home_directory);		    /* length of home directory name */
  if (strncmp (str, "/tmp_mnt/", 9) == 0)
    {
    strcpy (str, str + 8);
    }
  else if (! strncmp (str, home_directory, homelen))	    /* home directory or subdirectory, */
    {
    int sublen = strlen (str) - homelen;		    /* >0 if subdirectory, */
    *str = '~';						    /* convert to ~ */
    if (sublen)						    /* it's a subdirectory, */
      strcpy (&str [1], &str [homelen]);		    /* put the rest in */
    else
      str [1] = '\0';					    /* don't chop off before */
    }
}


/* Get the info from the OS. Return true if it was all available */

int 
get_info ()

{
  struct passwd *pwent;
  struct group *grent;
  int res = 1;

  /* start with the real username */

  if ((pwent = getpwuid (getuid ())) != NULL)
    {
    real_userid = strdup (pwent->pw_name);
    home_directory = strdup (pwent->pw_dir);		    /* save home directory */
    }
  else
    res = 0;

  /* then the effective username */

  if (res && (pwent = getpwuid (geteuid ())) != NULL)
    eff_userid = strdup (pwent->pw_name);
  else
    res = 0;

  /* then the real groupname */

  if (res && (grent = getgrgid (getgid ())) != NULL)
    real_groupid = strdup (grent->gr_name);
  else
    res = 0;

  /* then the effective groupname */

  if (res && (grent = getgrgid (getegid ())) != NULL)
    eff_groupid = strdup (grent->gr_name);
  else
    res = 0;

  if (res && gethostname (our_host, 32) < 0)
    res = 0;

  if (res && getwd (our_dir) == 0)
    return 0;
  process_dirstring (our_dir);

  if (! (our_tty = ttyname (0)))
    return 0;

  if (res)
    {
    int um;

    um = umask (0);					    /* this is silly but */
    (void) umask (um);					    /* you have to do it */
    sprintf (our_umask, "%03o", um);
    }


}


/* Parse the command line and return whether it was acceptable */

int 
parse_args (argc, argv)
     int argc;
     String argv[];

{
  int res = 1;
  int arg;

  winstr[0] = (char) 0;

  for (arg = 1; arg < argc; ++arg)
    {
    if (argv[arg][0] != '-')				    /* its a piece of text */
      {
      if (winstr[0] != (char) 0)
	strcat (winstr, " ");

      strcat (winstr, argv[arg]);
      }
    else
      {
      switch (argv[arg][1])
      {
      case 't':
      case 'T':

	want_icon = 0;

	break;

      case 'i':
      case 'I':

	want_title = 0;

	break;

      default:

	fprintf (stderr, "unknown option: '%s'\n", argv[arg]);
	res = 0;
	break;
      }
      }
    }

  if (res && winstr[0] == (char) 0)
    {
    res = 0;
    fprintf (stderr, "xtermset: must specify setting string\n");
    }

  return res;
}

/* Parse the string and build another string from it expanding the variables */

int 
expand_percents (str1, str2)
     String str1;
     String str2;

{
  int res = 1;
  String ptr = &str2[0];

  *ptr = (char) 0;

  while (*str1)
    {
    if (*str1 != '%')
      {
      *str2++ = *str1++;				    /* just copy it */
      *str2 = (char) 0;
      }
    else
      {
      String addon;

      ++str1;

      switch (*str1)
      {
      case 'h':

	addon = our_host;
	break;

      case 'd':

	addon = our_dir;
	break;

      case 'D':						    /* just last part of directory (Grog, 4 June 1993) */
	{
	char *last = strrchr (our_dir, '/');
	if (last)					    /* more than one part */
	  {
	  if (! strcmp (last, "/src")			    /* don't just take these standard names, */
	      || (! strcmp (last, "/doc"))
	      || (! strcmp (last, "/config"))
	      || (! strcmp (last, "/lib"))
	      || (! strcmp (last, "/bin"))
	      || (! strcmp (last, "/etc"))
	      || (! strcmp (last, "/source"))
	      || (! strcmp (last, "/sys"))
	      || (! strcmp (last, "/man")) )
	    {
	    while ((last > our_dir)
		   && (*--last != '/') );
	    }
	  if (last > our_dir)				    /* not back at the start, */
	    last++;					    /* skip the / */
	  addon = last;
	  }
	else						    /* what, no / in the pathname? */
	  addon = our_dir;
	break;
	}

      case 'u':
	addon = real_userid;
	break;

      case 'U':

	addon = eff_userid;
	break;

      case 'g':

	addon = real_groupid;
	break;

      case 'G':

	addon = eff_groupid;
	break;

      case 'e':

	addon = getenv ("XTXTRA");
	break;

      case 'm':

	addon = our_umask;
	break;

      case 't':						    /* terminal name */
	addon = our_tty;
	break;

      case 'T':						    /* just last part of terminal name */
	{
	char *last = strrchr (our_tty, '/');
	if (last)					    /* more than one part */
	  addon = ++last;		  
	else
	  addon = our_tty;
	break;
	}
	
      case '%':

	addon = "%";
	break;

      default:

	addon = NULL;
	res = 0;

	fprintf (stderr,
		 "xtermset: don't know the expansion of '%%%c'\n",
		 *str1);
      }

      ++str1;

      if (addon != NULL)
	strcat (str2, addon);

      while (*str2)
	++str2;
      }
    }

  return res;
}


/* Emit the setting strings */

int 
do_setting ()

{
  char expand[2048];
  int res = 1;

  if (expand_percents (winstr, expand))
    {
    if (want_icon)
      printf ("%c]1;%s%c", ESC, expand, BELL);
    if (want_title)
      printf ("%c]2;%s%c", ESC, expand, BELL);
    }    
  else
    res = 0;

  return res;
}


/* The boring old main function, yawn city */

int 
main (argc, argv)
     int argc;
     String argv[];

{
  /* if we don't have the right terminal we might as well not bother */

  int status = 0;
  String termvar = getenv ("TERM");

  if (termvar != NULL && strcmp (termvar, REQUIRED_TERM) == 0)
    {
    if (parse_args (argc, argv) != 0)
      {
      if (get_info () != 0)
	if (do_setting () == 0)
	  status = 1;
	else
	  status = 1;
      }
    else
      status = 1;
    }

  return status;
}

static char RCSid[] = "$Id: xtset.c,v 1.4 93/04/27 13:29:26 msmith Exp $";
