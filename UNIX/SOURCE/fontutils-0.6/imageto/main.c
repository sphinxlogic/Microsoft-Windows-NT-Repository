/* imageto -- convert a scanned image.

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

#include <ctype.h>

#define CMDLINE_NO_DPI /* It's not in the input filename.  */
#include "cmdline.h"
#include "encoding.h" /* For `DEFAULT_ENCODING'.  */
#include "getopt.h"
#include "gf.h"
#include "report.h"

#include "ifi.h" /* For `encoding_name'.  */
#include "image-header.h"
#include "input-img.h"
#include "input-pbm.h"
#include "main.h"
#include "out-chars.h"
#include "out-epsf.h"
#include "out-strips.h"


/* General information about the image.  Set by the input routines.  */
image_header_type image_header;

/* Pointers to functions based on input format.  (-input-format)  */
void (*image_open_input_file) (string) = NULL;
void (*image_close_input_file) (void) = NULL;
void (*image_get_header) (void) = NULL;
boolean (*image_get_scanline) (one_byte *) = NULL;

/* The filename for the input image.  */
string input_name;

/* Show every scanline on the terminal as we read it?  (-trace-scanlines) */
boolean trace_scanlines = false;

/* Private variables.  */

/* The design size of the font we're creating.  (-designsize)  */
static real design_size = 10.0;

/* The suffix for the image file.  */
static string input_extension;

/* If set, write an EPSF file.  (-epsf)  */
static boolean output_epsf = false;

/* The name of the file we're going to write.  (-output-file) */
static string output_name;

/* If set, make each ``character'' a (more or less) constant number of
   scanlines.  (-strips) */
static boolean do_strips = false;


static string read_command_line (int, string[]);
static void set_img_input_format (void);
static void set_input_format (string);
static void set_pbm_input_format (void);

/* We have three different strategies for processing the image:
     1)  (normal) analyze the image and write out the ``true'' characters,
     2) (-strips) takes a constant number of scanlines as each character,
     3)   (-epsf) write out the image as an Encapsulated PostScript file.
   
   The output name is, correspondingly, constructed differently:
     1) if `output_name' was supplied, and has a suffix, that's it.
     2) if `output_name' was supplied, but has no suffix, append `.eps'
        if we're doing EPSF, else append `<suffix>.<dpi>gf', where
        <suffix> is 
          a) `sp', if we are doing strips;
          b) the design size, if not.
     3) if `output_name' was not supplied, use the basename of the input
        filename extended as in #2.
   */

int
main (int argc, string argv[])
{
  boolean writing_gf;

  input_name = read_command_line (argc, argv);
  writing_gf = !output_epsf;
  
  set_input_format (input_name);  

  /* If the input format is PBM, then they must also tell us the dpi.  */
  if (image_open_input_file == pbm_open_input_file 
      && image_header.hres == 0)
    {
      fprintf (stderr, "If you use PBM format, you must supply the dpi.\n");
      fprintf (stderr, "For more information, use ``-help''.\n");
      exit (1);
    }

  /* Open the main input file.  */
  (*image_open_input_file) (extend_filename (input_name, input_extension));
  
  /* We need the horizontal resolution before we can make the GF name,
     so, at least for IMG input, have to read the header.  */
  (*image_get_header) ();

  /* If the user didn't give an output name, use the input name.  */
  if (output_name == NULL)
    output_name = remove_suffix (basename (input_name));
  
  /* If they didn't give an output name with a suffix, use ours.  */
  if (find_suffix (output_name) == NULL)
    {
      if (writing_gf)
        {
          char dpi[MAX_INT_LENGTH + 1];
          string suffix = do_strips ? "sp" : itoa ((unsigned) design_size);

          sprintf (dpi, "%u", image_header.hres);
          output_name = concat5 (output_name, suffix, ".", dpi, "gf");
	}
      else if (output_epsf)
        output_name = concat (output_name, ".eps");
      else
        abort (); /* Should never happen.  */
    }

  /* If necessary, open the GF file.  */
  if (writing_gf)
    {
      if (!gf_open_output_file (output_name))
        FATAL_PERROR (output_name);

      /* Identify ourselves in the GF comment.  */
      gf_put_preamble (concat ("imageto output ", now () + 4));
    }

  /* Do the real work, whichever the user wants.  */
  if (do_strips)
    { /* The design size is irrelevant when we're creating strips,
         but it should be pretty large, lest the relative dimensions in
         the TFM file get too big.  */
      design_size = 100.0;
      write_chars_as_strips (image_header, design_size);
    }
  else if (output_epsf)
    write_epsf (output_name, image_header);
  else
    write_image_chars (image_header, design_size);

  /* Clean up.  */
  if (verbose)
    REPORT ("\n");

  if (writing_gf)
    {
      /* We've read all the characters we're supposed to (or else the whole
         image).  Finish up the font.  */
      gf_put_postamble (real_to_fix (design_size),
                        (real) image_header.hres, (real) image_header.vres);
      gf_close_output_file ();
    }

  (*image_close_input_file) ();
  
  return 0;
}

/* We are semi-clever about printing this, for the sake of huge images.  */

void
print_scanline (one_byte line[], unsigned width)
{
  static unsigned scanline_count = 0;

  if (trace_scanlines)
    {
      printf ("%7d:", scanline_count++);
      
      /* If the line is entirely white, don't print anything.  */
      if (memchr (line, BLACK, width))
        {
          unsigned c;
          
          for (c = 0; c < width; c++)
            { /* Compress eight consecutive spaces to a tab, if we're at
                 the beginning of a tab mark.  This handles the usual
                 case, although we could do still better.  */
              if (c % 8 == 0 && c + 7 < width
	          && memchr (line + c, BLACK, 8) == NULL)
                {
                  putchar ('\t');
                  c += 7;
		}
              else
                putchar (line[c] ? '*' : ' ');
	    }
	}

      putchar ('\n');
    }
}

/* Reading the options.  */

/* This is defined in version.c.  */
extern string version_string;

#define USAGE "Options:
<font_name> should be a base filename, e.g., `ggmr'.  (More properly, it
is an <image_name>, not a <font_name>.)" 				\
  GETOPT_USAGE								\
"baselines <row1>,<row2>,...: define the baselines for each image row.
  The baseline of the first image row is taken to be scanline <row1>, etc.
designsize <real>: set the designsize of the font to <real>; default is 10.0.
dpi <unsigned>: resolution (required for pbm input).
encoding <filename>: read ligature and other encoding information
  from `<filename>.enc'; the default is to assign successive character codes.
epsf: write the image as an Encapsulated PostScript file, instead of a
  bitmap font.
help: print this message.
ifi-file <filename>: use <filename>.ifi (if <filename doesn't have a
  suffix; otherwise use <filename>) for the IFI filename; default is
  `<font_name>.ifi'.
input-format <format>: specify format of input image; <format> must be
  one of `pbm' or `img'.
nchars <unsigned>: only write the first <unsigned> (approximately)
  characters to the font; default is infinity.
output-file <filename>: write to <filename> if <filename> has a suffix.
  If <filename> doesn't have a suffix, then if writing strips, write to
  <filename>sp.<dpi>gf and to <filename>.<dpi>gf if not.  By default,
  use <font_name> for <filename>.
print-clean-info: print gray values for the bounding boxes that are
  considered for cleaning.  This implies `-verbose'.
print-guidelines: print the numbers of the top and bottom rows (in that
  order) of each character.  This implies `-verbose'.
range <char1>-<char2>: only process characters between <char1> and
  <char2>, inclusive. 
strips: take a constant number of scanlines as each character,
  instead of using an IFI file to analyze the image.
trace-scanlines: show every scanline as we read it.
verbose: output progress reports.
version: print the version number of this program.
"

static string
read_command_line (int argc, string argv[])
{
  int g;  /* `getopt' return code.  */
  int option_index;
  boolean printed_version = false;
  struct option long_options[]
    = { { "baselines",		1, 0, 0 },
        { "designsize",		1, 0, 0 },
        { "encoding",		1, 0, 0 },
        { "epsf",		0, (int *) &output_epsf, 1 },
        { "help",               0, 0, 0 },
	{ "dpi",		1, 0, 0 },
        { "ifi-file",		1, 0, 0 },
        { "nchars",		1, 0, 0 },
        { "input-format",	1, 0, 0 },
        { "output-file",	1, 0, 0 },
        { "print-clean-info",	0, (int *) &print_clean_info, 1 },
        { "print-guidelines",	0, (int *) &print_guidelines, 1 },
        { "range",              1, 0, 0 },
        { "strips",		0, (int *) &do_strips, 1 },
        { "trace-scanlines",	0, (int *) &trace_scanlines, 1 },
        { "verbose",		0, (int *) &verbose, 1 },
        { "version",            0, (int *) &printed_version, 1 },
        { 0, 0, 0, 0 } };

  while (true)
    {
      g = getopt_long_only (argc, argv, "", long_options, &option_index);
      
      if (g == EOF)
        break;

      if (g == '?')
        exit (1);  /* Unknown option.  */
  
      assert (g == 0); /* We have no short option names.  */
      
      if (ARGUMENT_IS ("baselines"))
        baseline_list = scan_unsigned_list (optarg);
      
      else if (ARGUMENT_IS ("designsize"))
        design_size = atof (optarg);
      
      else if (ARGUMENT_IS ("dpi"))
	{
          image_header.hres = (two_bytes) atou (optarg);
          image_header.vres = image_header.hres;
        }

      else if (ARGUMENT_IS ("encoding"))
        encoding_name = optarg;

      else if (ARGUMENT_IS ("help"))
        {
          fprintf (stderr, "Usage: %s [options] <font_name>.\n", argv[0]);
          fprintf (stderr, USAGE);
          exit (0);
        }

      else if (ARGUMENT_IS ("ifi-file"))
        ifi_filename = optarg;

      else if (ARGUMENT_IS ("input-format"))
	{
	  if (STREQ ("pbm", optarg))
            set_pbm_input_format ();
	  else if (STREQ ("img", optarg))
            set_img_input_format ();
          else
            FATAL1 ("imageto: Unknown input format `%s'; expected one of \
`img' or `pbm'", optarg);
        }

      else if (ARGUMENT_IS ("nchars"))
        nchars_wanted = atou (optarg);

      else if (ARGUMENT_IS ("output-file"))
        output_name = optarg;

      else if (ARGUMENT_IS ("print-clean-info")
               || ARGUMENT_IS ("print-guidelines"))
        verbose = true;

      else if (ARGUMENT_IS ("range"))
        GET_RANGE (optarg, starting_char, ending_char);
      
      else if (ARGUMENT_IS ("version"))
        printf ("%s.\n", version_string);

      /* Else it was a flag; getopt has already done the assignment.  */
    }
  
  if (do_strips && output_epsf)
    FATAL ("imageto: Sorry, -strips and -epsf are mutually exclusive");

  FINISH_COMMAND_LINE ();
}

/* If the input format wasn't explicitly set on the command line,
   attempt to intuit it from FILENAME, and set the necessary variables.
   If we can't tell what the format should be, quit.  */

static void
set_input_format (string filename)
{
  string input_extension;
  
  /* If it's already set, just return.  */
  if (image_open_input_file != NULL)
    return;

  /* Try to guess based on FILENAME.  */
  input_extension = find_suffix (filename) ? : "";

  if (STREQ (input_extension, "img"))
    set_img_input_format ();

  else if (STREQ (input_extension, "pbm"))
    set_pbm_input_format ();
  
  else /* Can't guess it; quit.  */
    {
      fprintf (stderr, "You must supply an input format.\n");
      fprintf (stderr, "(I can't guess from the filename `%s'.)\n", filename);
      fprintf (stderr, "For more information, use ``-help''.\n");
      exit (1);
    }
}


/* Set up for reading a PBM file.  */

static void
set_pbm_input_format ()
{
  image_open_input_file = pbm_open_input_file;
  image_close_input_file = pbm_close_input_file;
  image_get_header = pbm_get_header;
  image_get_scanline = pbm_get_scanline;
  input_extension = "pbm";
}


/* Set up for reading an IMG file.  */

static void
set_img_input_format ()
{
  image_open_input_file = img_open_input_file;
  image_close_input_file = img_close_input_file;
  image_get_header = img_get_header;
  image_get_scanline = img_get_scanline;
  input_extension = "img";
}
