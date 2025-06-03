/*
 * man.c
 *
 * Copyright (c) 1991, John W. Eaton.
 *
 * You may distribute under the terms of the GNU General Public
 * License as specified in the README file that comes with the man 1.0
 * distribution.  
 *
 * John W. Eaton
 * jwe@che.utexas.edu
 * Department of Chemical Engineering
 * The University of Texas at Austin
 * Austin, Texas  78712
 */

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <signal.h>
#include "config.h"
#include "gripes.h"
#include "version.h"

#ifdef STD_HEADERS
#if defined(__svr4__) || defined(__bsdi__)
#include <unistd.h>
#else
#include <stdlib.h>
#endif
#else
extern char *malloc ();
extern char *getenv ();
extern void free ();
extern int system ();
extern int strcmp ();
extern int strncmp ();
extern int exit ();
extern int fflush ();
extern int fprintf ();
extern FILE *fopen ();
extern int fclose ();
#ifdef CHARSPRINTF
extern char *sprintf ();
#else
extern int sprintf ();
#endif
#endif

extern char *strdup ();

extern char **glob_vector ();
extern int access ();
extern int unlink ();
extern int system ();
extern int stat ();

char *prognam;
static char *pager;
static char *manp;
static char *manpathlist[MAXDIRS];
static char *section;
static int apropos;
static int whatis;
static int findall;

#ifdef ALT_SYSTEMS
static int alt_system;
static char *alt_system_name;
#endif

static int troff;					    /* set if we're troffing */

int debug;
int display_page (char *path, char *man_file, char *cat_file);

int
main (argc, argv)
     int argc;
     char **argv;
{
  int status = 0;
  char *nextarg;
  char *tmp;
  extern int optind;
  extern char *mkprogname ();
  char *is_section ();
  void man_getopt ();
  void do_apropos ();
  void do_whatis ();
  int man ();

  prognam = mkprogname (argv[0]);

  man_getopt (argc, argv);

  if (optind == argc)
    gripe_no_name (NULL);

  if (optind == argc - 1)
    {
      tmp = is_section (argv[optind]);

      if (tmp != NULL)
	gripe_no_name (tmp);
    }

  while (optind < argc)
    {
      nextarg = argv[optind++];

      /*
       * See if this argument is a valid section name.  If not,
       * is_section returns NULL.
       */
      tmp = is_section (nextarg);

      if (tmp != NULL)
	{
	  section = tmp;

	  if (debug)
	    fprintf (stderr, "\nsection: %s\n", section);

	  continue;
	}

      if (apropos)
	do_apropos (nextarg);
      else if (whatis)
	do_whatis (nextarg);
      else
	{
	  status = man (nextarg);

	  if (status == 0)
	    gripe_not_found (nextarg, section);
	}
    }
  return status;
}

/*
 * Get options from the command line and user environment.
 */
void
man_getopt (argc, argv)
     int argc;
     char **argv;
{
  int c;
  char *p;
  char *end;
  char **mp;
  extern char *optarg;
  extern int getopt ();
  extern void downcase ();
  extern char *manpath ();
  void usage ();

#ifdef HAS_TROFF
#ifdef ALT_SYSTEMS
  while ((c = getopt (argc, argv, "M:P:S:adfhkt?")) != EOF)
#else
  while ((c = getopt (argc, argv, "M:P:adfhkt?")) != EOF)
#endif
#else
#ifdef ALT_SYSTEMS
  while ((c = getopt (argc, argv, "M:P:S:adfhk?")) != EOF)
#else
  while ((c = getopt (argc, argv, "M:P:adfhk?")) != EOF)
#endif
#endif
    {
      switch (c)
	{
	case 'M':
	  manp = strdup (optarg);
	  break;
	case 'P':
	  pager = strdup (optarg);
	  break;
#ifdef ALT_SYSTEMS
	case 'S':
	  alt_system++;
	  alt_system_name = strdup (optarg);
	  break;
#endif
	case 'a':
	  findall++;
	  break;
	case 'd':
	  debug++;
	  break;
	case 'f':
#ifdef HAS_TROFF
	  if (troff)
	    gripe_incompatible ("-f and -t");
#endif
	  if (apropos)
	    gripe_incompatible ("-f and -k");
	  whatis++;
	  break;
	case 'k':
#ifdef HAS_TROFF
	  if (troff)
	    gripe_incompatible ("-t and -k");
#endif
	  if (whatis)
	    gripe_incompatible ("-f and -k");
	  apropos++;
	  break;
#ifdef HAS_TROFF
	case 't':
	  if (apropos)
	    gripe_incompatible ("-t and -k");
	  if (whatis)
	    gripe_incompatible ("-t and -f");
	  troff++;
	  break;
#endif
	case 'h':
	case '?':
	default:
	  usage();
	  break;
	}
    }

  if (pager == NULL || *pager == NULL)
    if ((pager = getenv ("PAGER")) == NULL)
      pager = strdup (PAGER);

  if (debug)
    fprintf (stderr, "\nusing %s as pager\n", pager);

  if (manp == NULL)
    {
      if ((manp = manpath (0)) == NULL)
	gripe_manpath ();

      if (debug)
	fprintf (stderr,
		 "\nsearch path for pages determined by manpath is\n%s\n\n",
		 manp);
    }

#ifdef ALT_SYSTEMS
  if (alt_system_name == NULL || *alt_system_name == NULL)
    if ((alt_system_name = getenv ("SYSTEM")) == NULL)
      alt_system_name = strdup (alt_system_name);

  downcase (alt_system_name);
#endif

  /*
   * Expand the manpath into a list for easier handling.
   */
  mp = manpathlist;
  for (p = manp ; ; p = end+1)
    {
      if ((end = strchr (p, ':')) != NULL)
	*end = '\0';

      if (debug)
	fprintf (stderr, "adding %s to manpathlist\n", p);

#ifdef ALT_SYSTEMS
      if (alt_system)
	{
	  char buf[BUFSIZ];

	  strcpy (buf, p);
	  strcat (buf, "/");
	  strcat (buf, alt_system_name);

	  *mp++ = strdup (buf);
	}
#else
      *mp++ = strdup (p);
#endif

      if (end == NULL)
	break;
    }
  *mp = NULL;

}

void
usage ()
{
  static char usage_string[1024] = "%s, version %s\n\n";

#ifdef HAS_TROFF
#ifdef ALT_SYSTEMS
  static char s1[] =  "usage: %s [-afhkt] [section] [-M path] [-P pager] [-S system] name ...\n\n";
#else
  static char s1[] =  "usage: %s [-afhkt] [section] [-M path] [-P pager] name ...\n\n";
#endif
#else
#ifdef ALT_SYSTEMS
  static char s1[] =  "usage: %s [-afhk] [section] [-M path] [-P pager] [-S system] name ...\n\n";
#else
  static char s1[] =  "usage: %s [-afhk] [section] [-M path] [-P pager] name ...\n\n";
#endif
#endif

static char s2[] = "  a : find all matching entries\n\
  d : print gobs of debugging information\n\
  f : same as whatis(1)\n\
  h : print this help message\n\
  k : same as apropos(1)\n";

#ifdef HAS_TROFF
  static char s3[] = "  t : use troff to format pages for printing\n";
#endif

  static char s4[] = "\n  M path   : set search path for manual pages to `path'\n\
  P pager  : use program `pager' to display pages\n";

#ifdef ALT_SYSTEMS
  static char s5[] = "  S system : search for alternate system's man pages\n";
#endif

  strcat (usage_string, s1);
  strcat (usage_string, s2);

#ifdef HAS_TROFF
  strcat (usage_string, s3);
#endif

  strcat (usage_string, s4);

#ifdef ALT_SYSTEMS
  strcat (usage_string, s5);
#endif

  fprintf (stderr, usage_string, prognam, version, prognam);
  exit(1);
}

/*
 * Check to see if the argument is a valid section number.  If the
 * first character of name is a numeral, or the name matches one of
 * the sections listed in config.h, we'll assume that it's a section.
 * The list of sections in config.h simply allows us to specify oddly
 * named directories like .../man3f.  Yuk. 
 */
char *
is_section (name)
     char *name;
{
  char **vs;

  for (vs = valid_sections; *vs != NULL; vs++)
    if ((strcmp (*vs, name) == NULL) || (isdigit (name[0])))
      return strdup (name);

  return NULL;
}

/*
 * Handle the apropos option.  Cheat by using another program.
 */
void
do_apropos (name)
     char *name;
{
  int status;
  int len;
  char *command;

  len = strlen (APROPOS) + strlen (name) + 2;

  if ((command = malloc(len)) == NULL)
    gripe_alloc (len, "command");

  sprintf (command, "%s %s", APROPOS, name);

  status = 0;
  if (debug)
    fprintf (stderr, "\ntrying command: %s\n", command);
  else
    status = system (command);

  if (status == 127)
    gripe_system_command (status);

  free (command);
}

/*
 * Handle the whatis option.  Cheat by using another program.
 */
void
do_whatis (name)
     char *name;
{
  int status;
  int len;
  char *command;

  len = strlen (WHATIS) + strlen (name) + 2;

  if ((command = malloc(len)) == NULL)
    gripe_alloc (len, "command");

  sprintf (command, "%s %s", WHATIS, name);

  status = 0;
  if (debug)
    fprintf (stderr, "\ntrying command: %s\n", command);
  else
    status = system (command);

  if (status == 127)
    gripe_system_command (status);

  free (command);
}

/*
 * Search for manual pages.
 *
 * If preformatted manual pages are supported, look for the formatted
 * file first, then the man page source file.  If they both exist and
 * the man page source file is newer, or only the source file exists,
 * try to reformat it and write the results in the cat directory.  If
 * it is not possible to write the cat file, simply format and display
 * the man file.
 *
 * If preformatted pages are not supported, or the troff option is
 * being used, only look for the man page source file.
 *
 */
int
man (name)
     char *name;
{
  int found;
  int glob;
  char **mp;
  char **sp;
  int try_section ();

  found = 0;

  fflush (stdout);
  if (section != NULL)
    {
      for (mp = manpathlist; *mp != NULL; mp++)
	{
	  if (debug)
	    fprintf (stderr, "\nsearching in %s\n", *mp);

	  glob = 0;

	  found += try_section (*mp, section, name, glob);

	  if (found && !findall)			    /* i.e. only do this section... */
	    return found;
	}
    }
  else
    {
      for (sp = valid_sections; *sp != NULL; sp++)
	{
	  for (mp = manpathlist; *mp != NULL; mp++)
	    {
	      if (debug)
		fprintf (stderr, "\nsearching in %s\n", *mp);

	      glob = 1;

	      found += try_section (*mp, *sp, name, glob);

	      if (found && !findall)			    /* i.e. only do this section... */
		return found;

	    }
	}
    }
  return found;
}

/* uncompress_cmd: put commands in command to uncompress file to stdout.
 * Return 1 if commands were inserted, otherwise 0. */
int uncompress_cmd (char *command, char *file)
{
#ifdef UNPACK
  if (! strcmp (&file [strlen (file) - strlen (ISPACKED)], ISPACKED))
    {
    sprintf (&command [strlen (command)], "%s %s | ", UNPACK, file);
    return 1;
    }
#endif
#ifdef ZCAT
  if (! strcmp (&file [strlen (file) - strlen (ISCOMPRESSED)], ISCOMPRESSED))
    {
    sprintf (&command [strlen (command)], "%s %s | ", ZCAT, file);
    return 1;
    }
#endif
#ifdef GUNZIP
  if (! strcmp (&file [strlen (file) - strlen (ISGZIPPED)], ISGZIPPED))
    {
    sprintf (&command [strlen (command)], "%s %s | ", GUNZIP, file);
    return 1;
    }
#endif
  return 0;						    /* not compressed */
  }

/* recompress_cmd: put commands in command to recompress file to stdout.
 * Return 1 if commands were inserted, otherwise 0.
 * If we don't support the kind of compression, remove the suffix of
 * the file name so that it will work next time, too. */
int recompress_cmd (char *command, char *file)
{
  if (! strcmp (&file [strlen (file) - strlen (ISPACKED)], ISPACKED))
#ifdef PACK
    {
    sprintf (&command [strlen (command)], "| %s > %s ", PACK, file);
    return 1;
    }
#else							    /* can't pack, */
    file [strlen (file) - strlen (ISPACKED)] = '\0';	    /* truncate the name */
#endif
  if (! strcmp (&file [strlen (file) - strlen (ISCOMPRESSED)], ISCOMPRESSED))
#ifdef COMPRESS
    {
    sprintf (&command [strlen (command)], "| %s > %s ", COMPRESS, file);
    return 1;
    }
#else	  
    file [strlen (file) - strlen (ISCOMPRESSED)]= '0';	    /* truncate */
#endif
  if (! strcmp (&file [strlen (file) - strlen (ISGZIPPED)], ISGZIPPED))
#ifdef GZIP
    {
    sprintf (&command [strlen (command)], "| %s > %s ", GZIP, file);
    return 1;
    }
#else
    file [strlen (file) - strlen (ISGZIPPED)]= '0';	    /* truncate */
#endif
  sprintf (&command [strlen (command)], " > %s ", file);    /* just store to the file */
  return 0;						    /* not compressed */
  }

/* Insert a subcommand into command, in the form of
 * | less -Sc -Pman.1z */
void pager_cmd (char *command, char *man_file, int withpipe)
{
#ifndef PIPE_PAGER
  char *mypager = pager;
#else
  char mypager [MAXPATHLEN];

  sprintf (mypager, PIPE_PAGER, man_file);
#endif
  if (withpipe)
    sprintf (&command [strlen (command)], "| %s ", mypager);
else
    sprintf (&command [strlen (command)], "%s ", mypager);  /* leave out pipe symbol */
  }

/* insert a command in *command to page cat_file. */
void page_file_cmd (char *command, char *cat_file)
{
  sprintf (&command [strlen (command)], "%s %s ", pager, cat_file);
  }

/*
 * See if the preformatted man page or the source exists in the given
 * section.
 */
int
try_section (path, section, name, glob)
     char *path;
     char *section;
     char *name;
     int glob;
{
  int found = 0;
  int to_cat;
  int cat;
  char **names;
  char **np;
  char **glob_for_file ();
  char **make_name ();
  char *convert_name ();
  char *ultimate_source ();
  int formatted;					    /* set if the path we are looking at
							     * contains formatted pages */

#ifdef ANYEXT
  glob = 1;						    /* force globbing if we accept any extension */
#endif
  if (debug)
    {
      if (glob)
	fprintf (stderr, "trying section %s with globbing\n", section);
      else
	fprintf (stderr, "trying section %s without globbing\n", section);
    }

  /*
   * Look for man page source files.
   */
  cat = 0;
  if (glob)
    names = glob_for_file (path, section, name, cat);
  else
    names = make_name (path, section, name, cat);

  /* Some man pages stored in directories man* are formatted. This is the case,
   * for example, with Interactive UNIX/386, where the standard man page path
   * looks like /usr/catman/u_man/man1/page.1z. Catch this here. */
  
  if (strstr (path, ISFORMATTED))			    /* formatted */
    {
    if ((int) names != -1 && *names != NULL)
      {
      for (np = names; *np != NULL; np++)
	found += display_page (path, NULL, *np);
      }
    }
  else if ((int) names == -1
	   || *names == NULL )
    /*
     * No files match.  If we're not using troff and we're supporting
     * preformatted pages, see if there's one around that we can
     * display. 
     */
    {
    if (cat_support && !troff)
      {
      cat = 1;
      if (glob)
	names = glob_for_file (path, section, name, cat);
      else
	names = make_name (path, section, name, cat);
      
      if ((int) names != -1 && *names != NULL)
	{
	for (np = names; *np != NULL; np++)
	  found += display_page (path, NULL, *np);
	}
      }
    }
  else							    /* found some source files */
    {
    for (np = names; *np != NULL; np++)
      {
      char *cat_file = NULL;
      char *man_file;
      
      man_file = ultimate_source (*np, path);
      
      if (cat_support && !troff)
	{
	to_cat = 1;
	
	cat_file = convert_name (man_file, to_cat);
	if (debug)
	  fprintf (stderr, "will try to write %s if needed\n", cat_file);
	}
      
      found += display_page (path, man_file, cat_file);
      }
    }

  return found;
}

/*
 * Change a name of the form ...man/man1/name.1 to ...man/cat1/name.1
 * or a name of the form ...man/cat1/name.1 to ...man/man1/name.1
 */
char *
convert_name (name, to_cat)
     char *name;
     int to_cat;
{
  char *to_name;
  char *t1;
  char *t2 = NULL;

  to_name = strdup (name);

  t1 = strrchr (to_name, '/');
  if (t1 != NULL)
    {
      *t1 = NULL;
      t2 = strrchr (to_name, '/');
      *t1 = '/';
    }

  if (t2 == NULL)
    gripe_converting_name (name, to_cat);

  if (to_cat)
    {
      *(++t2) = 'c';
      *(t2+2) = 't';
    }
  else
    {
      *(++t2) = 'm';
      *(t2+2) = 'n';
    }

  return to_name;
}


/*
 * Try to find the man page corresponding to the given name.  The
 * reason we do this with globbing is because some systems have man
 * page directories named man3 which contain files with names like
 * XtPopup.3Xt.  Rather than requiring that this program know about
 * all those possible names, we simply try to match things like
 * .../man[sect]/name[sect]*.  This is *much* easier.
 *
 * Note that globbing is only done when the section is unspecified.
 */
char **
glob_for_file (path, section, name, cat)
     char *path;
     char *section;
     char *name;
     int cat;
{
  char pathname[BUFSIZ];
  char **glob_filename ();
  char **gf;
  char *dir = cat? "cat": "man";			    /* decide on directory */

#ifdef ANYEXT						    /* accept any filename extension */
  sprintf (pathname, "%s/%s%s/%s.*", path, dir, section, name);
#else
  sprintf (pathname, "%s/%s%s/%s.%s*", path, dir, section, name, section);
#endif
  if (debug)
    fprintf (stderr, "globbing %s\n", pathname);

  gf = glob_filename (pathname);

  /* Check if we found a file. If not, and section starts with a digit, have
   * another throw. This doesn't make much sense if our system doesn't append
   * the section name to the filename, of course. */
#ifndef ANYEXT
  if (((int) gf == -1 || *gf == NULL) && isdigit (*section))
    {
    sprintf (pathname, "%s/%s%s/%s.%c*", path, dir, section, name, *section);
    gf = glob_filename (pathname);
    }
#endif
  return gf;
}

/*
 * Return an un-globbed name in the same form as if we were doing
 * globbing. 
 */
char **
make_name (path, section, name, cat)
     char *path;
     char *section;
     char *name;
     int cat;
{
  int i = 0;
  static char *names[3];
  char buf[BUFSIZ];

  if (cat)
    sprintf (buf, "%s/cat%s/%s.%s", path, section, name, section);
  else
    sprintf (buf, "%s/man%s/%s.%s", path, section, name, section);

  if (access (buf, R_OK) == 0)
    names[i++] = strdup (buf);

  /*
   * If we're given a section that looks like `3f', we may want to try
   * file names like .../man3/foo.3f as well.  This seems a bit
   * kludgey to me, but what the hey...
   */
  if (section[1] != '\0')
    {
      if (cat)
	sprintf (buf, "%s/cat%c/%s.%s", path, section[0], name, section);
      else
	sprintf (buf, "%s/man%c/%s.%s", path, section[0], name, section);

      if (access (buf, R_OK) == 0)
	names[i++] = strdup (buf);
    }

  names[i] = NULL;

  return &names[0];
}

/*
 * Try to find the ultimate source file.  If the first line of the
 * current file is not of the form
 *
 *      .so man3/printf.3s
 *
 * the input file name is returned.
 */
char *
ultimate_source (name, path)
     char *name;
     char *path;
{
  FILE *fp;
  char buf[BUFSIZ];
  char ult[BUFSIZ];
  char *beg;
  char *end;

  strcpy (ult, name);
  strcpy (buf, name);

 next:

  if ((fp = fopen (ult, "r")) == NULL)
    return buf;

  if (fgets (buf, BUFSIZ, fp) == NULL)
    return ult;

  if (strlen (buf) < 5)
    return ult;

  beg = buf;
  if (*beg++ == '.' && *beg++ == 's' && *beg++ == 'o')
    {
      while ((*beg == ' ' || *beg == '\t') && *beg != '\0')
	beg++;

      end = beg;
      while (*end != ' ' && *end != '\t' && *end != '\n' && *end != '\0')
	end++;

      *end = '\0';

      strcpy (ult, path);
      strcat (ult, "/");
      strcat (ult, beg);

      strcpy (buf, ult);

      goto next;
    }

  if (debug)
    fprintf (stderr, "found ultimate source file %s\n", ult);

  return ult;
}

/*
 * Try to format the man page source and save it, then display it.  If
 * that's not possible, try to format the man page source and display
 * it directly.
 *
 * man_file is the name of an unformatted file, cat_file is the name of
 * a formatted file. Both names are fully qualified; the pathname is just
 * supplied to help with cd. It is up to the function to decide which of
 * the two files really exists.
 *
 * Note that in the commands below, the cd is necessary because some
 * man pages are one liners like my version of sprintf.3s:
 *
 *      .so man3/printf.3s
 */
int
display_page (char *path, char *man_file, char *cat_file)
{
  int status;
  int mode;
  int found;
  int made_cat = 0;					    /* set when we create a new cat */
  FILE *fp;
  char command[BUFSIZ];
  int is_newer ();

  char *roff_command;
  
  found = 0;
  if (troff)
    roff_command = troff_command;			    /* process with troff */
  else
    roff_command = nroff_command;			    /* just roff */
  
  if (man_file						    /* got a man file */
      && access (man_file, R_OK) != 0)			    /* but we can't access it */
    return found;					    /* what good is this? */
  
/* (Greg Lehey, 1 December 1993)
 * Now decide how to display the man page. We have a number of considerations here:
 *
 * 1. We may only have a man file, only a cat file, or both. If we only have a man
 *    file, cat files may not be supported. If we have both, we may still need to
 *    access the man page if it is newer.
 * 1. The page may be packed, compressed, or gzipped. In this case we need to
 *    un(pack|compress|zip) it.
 * 2. The page may be unformatted. In this case,
 *   a. If we are going to display with troff, we format it and display directly
 *   b. If there is a formatted page which is newer, we just display the newer page.
 *   c. Otherwise we try to format and save, then display the saved version.
 * 3. The -t (troff) command might be specified. This doesn't seem to work: the way
 *    it's set up here, it will just output PostScript directly to the terminal, without
 *    even a pager. It's not clear what good this is, so I'm not changing it yet.
 */

  command [0] = '\0';					    /* start off with no command */
  if (! cat_support)					    /* we don't support cat files */
    {
    sprintf (command, "(cd %s ; ", path);		    /* first get into the directory */
    if (uncompress_cmd (command, man_file))		    /* command now contains commands to uncompress
							     * the page to stdout */
      sprintf (&command [strlen (command)], " %s ", man_file); /* (cd man1; zcat /usr/man/man.1z */
    else						    /* nothing compressed */
      sprintf (&command [strlen (command)], " %s %s ",
	       roff_command,
	       man_file);				    /* (cd man1; nroff -man /usr/man/man.1z */
    pager_cmd (command, man_file, 1);			    /* | less -Sc -Pman.1z */
    strcat (command, ")");				    /* and final RPAR */
    }
  else if (man_file					    /* we have a man file */
	   && (((status = is_newer (man_file, cat_file)) == 1) /* man file is newer */
	       || status == -2 ) )			    /* or cat file doesn't exist */
    {
    if ((cat_support)					    /* we support cat */
	&& (fp = fopen (cat_file, "w")) )		    /* and we can create the cat file */
      {
      fclose (fp);					    /* we don't need it really */
      unlink (cat_file);				    /* and discard, the name may get changed */
      fprintf (stderr, "Formatting page, please wait...\n");
      
      sprintf (command, "(cd %s ; ", path);		    /* first get into the directory */
      if (uncompress_cmd (command, man_file))		    /* command now contains commands to uncompress
							     * the page to stdout */
	sprintf (&command [strlen (command)], " %s ", roff_command); /* roff -man */
      else						    /* nothing compressed */
	sprintf (&command [strlen (command)], " %s %s ",
		 roff_command,
		 man_file);				    /* (cd man1; zcat /usr/man/man.1z */
      recompress_cmd (command, cat_file);		    /* and put in something to store again */
      strcat (command, " ; ");
      page_file_cmd (command, cat_file);		    /* and a command to page the file */
      strcat (command, ")");				    /* and final RPAR */
      made_cat = 1;					    /* command makes a new cat */
      }
    else						    /* can't create a cat file, format directly */
      {
      sprintf (command, "(cd %s ; ", path);		    /* first get into the directory */
      if (uncompress_cmd (command, man_file))		    /* command now contains commands to uncompress
							     * the page to stdout */
	sprintf (&command [strlen (command)], " %s ", roff_command); /* | roff -man */
      else						    /* nothing compressed */
	sprintf (&command [strlen (command)], " %s %s ",
		 roff_command,
		 man_file);				    /* (cd man1; roff -man /usr/man/man.1 */
      pager_cmd (command, man_file, 1);			    /* | less -Sc -Pman.1z */
      strcat (command, ")");				    /* and final RPAR */
      }
    }
  else							    /* we have a cat file to display */
    {
    sprintf (command, "(cd %s ; ", path);		    /* first get into the directory */
    if (uncompress_cmd (command, cat_file))		    /* command now contains commands to uncompress
							     * the page to stdout */
      pager_cmd (command, cat_file, 0);			    /* | less -Sc -Pman.1z */
    else						    /* nothing compressed */
      page_file_cmd (command, cat_file);		    /* just a command to page the file */
    strcat (command, ")");				    /* and final RPAR */
    }
  
  /* Now command contains a command we can pass to system(3) and do the display */
  signal (SIGINT, SIG_IGN);
  
  status = 0;
  status = system (command);
  
  if (status == 127)
    gripe_system_command (status);
  else
    {
    found++;
    
    if (made_cat)					    /* our command created a new cat file, */
      {							    /* set security correctly */
      mode = CATMODE;
      chmod (cat_file, mode);
      
      if (debug)
	fprintf (stderr, "mode of %s is now %o\n", cat_file, mode);
      }
    }
  signal (SIGINT, SIG_DFL);				    /* reset signals */
  return found;
  }

/*
 * Is file a newer than file b?
 *
 * case:
 *
 *   a newer than b         returns    1
 *   a older than b         returns    0
 *   stat on a fails        returns   -1
 *   stat on b fails        returns   -2
 *   stat on a and b fails  returns   -3
 */
int
  is_newer (char *fa, char *fb)
{
  struct stat fa_sb;
  struct stat fb_sb;
  int fa_stat;
  int fb_stat;
  int status = 0;
  
  fa_stat = stat (fa, &fa_sb);
  if (fa_stat != 0)
    status = 1;
  
  fb_stat = stat (fb, &fb_sb);
  if (fb_stat != 0)
    status |= 2;
  
  if (status != 0)
    return -status;
  
  if (fa_sb.st_mtime > fb_sb.st_mtime)
    {
    status = 1;
    
    if (debug)
      fprintf (stderr, "%s is newer than %s\n", fa, fb);
    }
  else
    {
    status = 0;
    
    if (debug)
      fprintf (stderr, "%s is older than %s\n", fa, fb);
    }
  return status;
  }
