/* bpltobzr -- convert from the textual ``Bezier property list'' format
   to the binary BZR format.

Copyright (C) 1992 Free Software Foundation, Inc.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include "config.h"

#include "bzr.h"
#define CMDLINE_NO_DPI /* Outline fonts are resolution-independent.  */
#include "cmdline.h"
#include "getopt.h"
#include "report.h"

#include "main.h"

/* The input stream.  */
FILE *bpl_file;

/* The name of the output file specified by the user.  (-output-file)  */
static string output_name = NULL;

/* Says which characters we should process.  This is independent of the
   ordering in the font file.  (-range)  */
static int starting_char = 0;
static int ending_char = MAX_CHARCODE;

/* This is defined in version.c.  */
extern string version_string;

static string read_command_line (int, string []);

/* It would be nice if we could actually use Lisp to do this job.  The
   problem is not parsing the input -- we get that essentially for free,
   since property list syntax is Lisp syntax -- but in writing the
   output.  I just couldn't see doing all the funky floating-point and
   file manipulations from Elisp, when I already had them written in C.  */

int
main (int argc, string argv[])
{
  extern int yyparse ();
  int err;
  string bpl_name = read_command_line (argc, argv);
  
  /* Add the default extensions and open the files.  */
  bpl_name = extend_filename (bpl_name, "bpl");
  bpl_file = xfopen (bpl_name, "r");
  
  if (output_name == NULL)
    output_name = remove_suffix (basename (bpl_name));
  output_name = extend_filename (output_name, "bzr");
  if (!bzr_open_output_file (output_name))
    FATAL_PERROR (output_name);
  
  /* Parse and convert the BPL file.  */
  err = yyparse ();
  
  bzr_close_output_file ();
  
  exit (err);
}

/* Reading the options.  */

/* This is defined in version.c.  */
extern string version_string;

#define USAGE "Options:
<font_name> should be a filename, e.g., `cmr10'.  Any extension is ignored." \
  GETOPT_USAGE								     \
"help: print this message.
output-file <filename>: output to <filename> (if it has a suffix) or to
  <filename>.bzr (if it doesn't).
range <char1>-<char2>: only work on characters between <char1> and
  <char2> inclusive.
verbose: print brief progress reports on stderr.
version: print the version number of this program.
"


/* We return the name of the font to process.  */

static string
read_command_line (int argc, string argv[])
{
  int g;   /* `getopt' return code.  */
  int option_index;
  boolean printed_version = false;
  struct option long_options []
    = { { "help",               0, 0, 0 },
        { "output-file",	1, 0, 0 },
        { "range",		1, 0, 0 },
        { "verbose",            0, (int *) &verbose, 1 },
        { "version",            0, (int *) &printed_version, 1 },
        { 0, 0, 0, 0 } };

  while (true)
    {
      g = getopt_long_only (argc, argv, "", long_options, &option_index);

      if (g == EOF) break;
      if (g == '?') exit (1);  /* Unknown option.  */
  
      assert (g == 0); /* We have no short option names.  */
      
      if (ARGUMENT_IS ("help"))
        {
          fprintf (stderr, "Usage: %s [options] <font_name>.\n", argv[0]);
          fprintf (stderr, USAGE);
          exit (0);
        }
        
      else if (ARGUMENT_IS ("output-file"))
        output_name = optarg;

      else if (ARGUMENT_IS ("range"))
        GET_RANGE (optarg, starting_char, ending_char);
        
      else if (ARGUMENT_IS ("version"))
        printf ("%s.\n", version_string);
      
      /* Else it was just a flag; getopt has already done the assignment.  */
    }
  
  FINISH_COMMAND_LINE ();
}
