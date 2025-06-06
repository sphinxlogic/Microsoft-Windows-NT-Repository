/* charspace -- find intercharacter spacing based on user information.

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

#include "cmdline.h"
#include "encoding.h"
#include "font.h"
#include "getopt.h"
#include "report.h"

#include "char.h"
#include "input-cmi.h"
#include "main.h"
#include "output.h"
#include "symtab.h"

/* Must come after `symtab.h', since it uses `symval_type'.  (It's
   generated by Bison, so we can't fix it.)  */
#include "cmi.h"


/* The resolution of the font we will read, in pixels per inch.  We
   don't deal with nonsquare pixels.  (-dpi)  */
string dpi = "300";
real dpi_real;

/* The name of the encoding file specified by the user, and the
   structure we parse it into.  (-encoding)  */
static string encoding_name = NULL;
encoding_info_type encoding_info;

/* Whether or not there was a TFM file corresponding to `input_name'.
  If true, `tfm_get_char' can be called -- otherwise not.  */
boolean have_tfm = false;

/* The name of the bitmap font we input.  */
string input_name;

/* Whether or not to output a GF file.  (-no-gf) */
boolean no_gf = false;

/* Says which characters we should process.  This is independent of the
   ordering in the font file.  (-range)  */
charcode_type starting_char = 0;
charcode_type ending_char = MAX_CHARCODE;


static string read_command_line (int, string[]);
static encoding_info_type read_encoding_info (string);
static string *scan_string_list (string);

/* Charspace computes side bearing values to put in a font.  It doesn't
   do so automatically, unfortunately: auxiliary files have to specify
   everything.  The basic idea is due to Harry Carter as described in
   Walter Tracy's book (see README for the citation): space a few of the
   letters (O, H, o, n) by hand amongst themselves.  Then a reasonable
   value for most of the other side bearings can be determined
   (independent of the typeface) as a percentage of those ``control
   letters'' sidebearings.
   
   The basic strategy is to first read `common.cmi', which specifies the
   font-independent side bearing apportionments.  Then we read
   font-specific CMI files to actually define the side bearing values,
   specify kerns, and so on.
   
   This defines a whole bunch of things in the symbol table (see
   `symtab.c').  We then resolve the side bearing information for all
   the characters we will output, after all the definitions have been
   read.
   
   Then we output the revised GF and TFM files.  We read an existing TFM
   file and an encoding file, as well as accepting various options, to
   get lig/kern and other TFM information.  */
   
int
main (int argc, string argv[])
{
  /* This is static only because we want it to be initialized entirely
     to NULL pointers, and it's too painful to write out 256 NULL's.  */
  static char_type *chars[MAX_CHARCODE + 1];
  unsigned code;
  bitmap_font_type font;
  string font_rootname;
  string tfm_name;

  /* Get the bitmap font we're reading.  */
  input_name = read_command_line (argc, argv);
  font = get_bitmap_font (input_name, atou (dpi));
  font_rootname = remove_suffix (basename (input_name));

  /* Make sure the output name is ok.  */
  if (output_name == NULL)
    output_name = input_name;

  if (!no_gf && find_suffix (output_name) != NULL)
    FATAL ("You can't specify a suffix and more than one output file");

  tfm_name = find_tfm_filename (input_name);
  if (tfm_name != NULL)
    {
      if (!tfm_open_input_file (tfm_name))
        FATAL1 ("%s: Could not open TFM file", tfm_name);
      else
        have_tfm = true;
    }

  /* Set the numeric counterpart of `dpi', for use in lots of places. */
  dpi_real = atof (dpi);
  
  /* If the user didn't specify CMI files to read, then use
     `<input_name>.<dpi>cmi'.  */
  if (cmi_names == NULL)
    cmi_names = scan_string_list (input_name);

  /* Define the designsize, so the CMI files can refer to it.  */
  {
    real ds_points = BITMAP_FONT_DESIGN_SIZE (font);
    real ds_pixels = POINTS_TO_REAL_PIXELS (ds_points, dpi_real);
    symtab_define ("designsize", symtab_real_node (ds_pixels));
  }

  /* Read all the CMI information.  */
  read_cmi_file ("common.cmi", "");
  read_cmi_file_list (cmi_names, dpi);
  
  /* Figure out the font encoding scheme.  Must be done after reading
     the CMI files, since they can specify the codingscheme.  */
  encoding_info = read_encoding_info (encoding_name);
  
  /* The main loop: compute each character's information.  */
  for (code = starting_char; code <= ending_char; code++)
    {
      do_char (code, &chars[code]);
    }

  /* Output what we've so laboriously collected.  */
  output_font (font, chars);

  close_font (input_name);
  if (have_tfm)
    tfm_close_input_file ();
    
  return 0;
}

/* Return the font encoding.  Use
   1) USER_NAME (if it's non-null);
   2) the value of `codingscheme' in the symbol table (if it's a string);
   3) the codingscheme from an existing TFM file (if we have one).
   
   Otherwise, give a fatal error.  */

static encoding_info_type
read_encoding_info (string user_name)
{
  encoding_info_type ei;
  
  if (user_name != NULL)
    ei = read_encoding_file (user_name);
  else
    {
      string enc_name;
      string codingscheme = NULL;
      symval_type *sv = symtab_lookup ("codingscheme");

      if (sv == NULL)
        codingscheme = have_tfm ? tfm_get_coding_scheme () : DEFAULT_ENCODING;

      else if (SYMVAL_TAG (*sv) == symval_string)
        codingscheme = SYMVAL_STRING (*sv);
      
      else
        FATAL ("codingscheme: defined (in CMI file) as a non-string");
      
      assert (codingscheme != NULL);
      
      enc_name = coding_scheme_to_filename (codingscheme);
      ei = read_encoding_file (enc_name);
    }
  
  return ei;
}

/* Reading the options.  */

/* This is defined in version.c.  */
extern string version_string;

#define USAGE "Options:
<font_name> should be a filename, possibly with a resolution, e.g.,
  `cmr10' or `cmr10.300'.\n"						\
  GETOPT_USAGE								\
"cmi-files <file1>,<file2>,...: read the CMI files
  `<file1>.<dpi>cmi', `<file2>.<dpi>cmi', etc., after reading
  `common.cmi'; default is `<font-name>.<dpi>cmi'.  The <dpi>cmi is not
  appended to any of the <file>s which already have a suffix.
dpi <unsigned>: use this resolution; default is 300.
encoding <filename>: read ligature and other encoding information
  from <filename>.enc; there is no default.  (A TFM file <font-name>.tfm
  is also read, if it exists.)
fontdimens <fontdimen>:<real>,<fontdimen>:<real>,...: assign <value>
  to each <fontdimen> given, when outputting a TFM file.  A <fontdimen>
  can be either one of the standard names (in either upper or
  lowercase), or a number between 1 and 30.  Each <real> is taken to be
  in points (except in the case of the <fontdimen> `SLANT' (parameter
  1), which is a dimensionless number).
no-gf: don't output a GF file.
output-file <filename>: write the TFM file to `<filename>.tfm' and the
  GF file to `<filename>.<dpi>gf'; <filename> shouldn't have a suffix;
  default is <font-name>.tfm and <font-name>.<dpi>gf, or, if those would
  overwrite the input, those preceded by `x'.
range <char1>-<char2>: only process characters between <char1> and
  <char2>, inclusive. 
verbose: print brief progress reports on stdout.
version: print the version number of this program.
xheight-char <charcode>: use the height of this character as the
  default x-height (for the TFM output); default is 120 (ASCII `x').
"

static string
read_command_line (int argc, string argv[])
{
  int g;   /* `getopt' return code.  */
  int option_index;
  boolean explicit_dpi = false;
  boolean printed_version = false;
  struct option long_options[]
    = { { "dpi",		1, 0, 0 },
        { "cmi-files",		1, 0, 0 },
        { "encoding",		1, 0, 0 },
        { "fontdimens",		1, 0, 0 },
        { "help",               0, 0, 0 },
        { "no-gf",		0, (int *) &no_gf, 1 },
        { "output-file",	1, 0, 0 },
        { "range",		1, 0, 0 },
        { "verbose",		0, (int *) &verbose, 1 },
        { "version",		0, (int *) &printed_version, 1 },
        { "xheight-char",	1, 0, 0 },
        { 0, 0, 0, 0 } };
  
  while (true)
    {
      g = getopt_long_only (argc, argv, "", long_options, &option_index);
      
      if (g == EOF)
        break;

      if (g == '?')
        continue;  /* Unknown option.  */
  
      assert (g == 0); /* We have no short option names.  */
  
      if (ARGUMENT_IS ("cmi-files"))
        cmi_names = scan_string_list (optarg);

      else if (ARGUMENT_IS ("dpi"))
        dpi = optarg;
      
      else if (ARGUMENT_IS ("encoding"))
        encoding_name = optarg;

      else if (ARGUMENT_IS ("fontdimens"))
        fontdimens = optarg;

      else if (ARGUMENT_IS ("help"))
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
      
      else if (ARGUMENT_IS ("xheight-char"))
        xheight_char = xparse_charcode (optarg);
        
      /* Else it was just a flag; getopt has already done the assignment.  */
    }
  
  FINISH_COMMAND_LINE ();
}


/* Take a string L consisting of unsigned strings separated by commas
   and return a vector of the strings, as pointers. 
   Append an element to the vector.  */

static string *
scan_string_list (string l)
{
  string map;
  unsigned length = 1;
  string *vector = xmalloc (sizeof (string));
  
  for (map = strtok (l, ","); map != NULL; map = strtok (NULL, ","))
    {
      length++;
      vector = xrealloc (vector, length * sizeof (string));
      vector[length - 2] = map;
    }
  
  vector[length - 1] = NULL;
  return vector;
}
