/* bzrto -- translate a font in the binary BZR outline format to various
   other formats.

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
#define CMDLINE_NO_DPI /* BZR fonts are resolution-independent.  */
#include "cmdline.h"
#include "filename.h"
#include "getopt.h"
#include "lib.h"
#include "list.h"
#include "report.h"
#include "tfm.h"

#include "input-ccc.h"
#include "main.h"
#include "metafont.h"
#include "pstype1.h"
#include "pstype3.h"
#include "psutil.h"
#include "text.h"

#include "ccc.h"


/* The encoding vector.  This is not static because it's too much
   trouble to pass it down to the output routines.  */
encoding_info_type encoding_info;

/* Says which characters we should process.  This is independent of the
   ordering in the font file.  (-range)  */
charcode_type starting_char = 0;
charcode_type ending_char = MAX_CHARCODE;

/* The font parameters from the main TFM file, or NULL.  */
tfm_global_info_type *tfm_global_info;

/* The name of the CCC input file specified by the user.  (-ccc-file)  */
static string ccc_name = NULL;

/* Possible targets for the translation.  */
typedef enum
{ metafont, pstype1, pstype3, text, dummy_for_end
} translation_targets;
#define NTARGETS dummy_for_end

/* This array tells us to which forms to translate the BZR file.  */
static boolean output[NTARGETS];

/* The names of BZR files to be concatenated onto the main one in the
   output.  (-concat)  */
static list_type bzr_name_list;

/* The name of the encoding file.  (-encoding)  */
static string encoding_name = NULL;

/* The name of the output file specified by the user.  (-output-file)  */
static string output_name = NULL;

/* The FontName of the PostScript font.  */
static string ps_fontname;

/* This is defined in version.c.  */
extern string version_string;

/* The number of characters we report per line if verbose.  */
#define NCHARS_PER_LINE 13

static void append_concat_list (list_type *, string);
static unsigned count_non_subr_chars
  (char_type *[], bzr_char_type *[], ccc_type *[]);
static void init_tfm_and_encoding_info
  (string, tfm_global_info_type **, encoding_info_type *);
static void output_bzr_char (bzr_char_type, char_type *[]);
static void output_bzr_subr (bzr_char_type);
static void output_ccc_char (ccc_type, charcode_type, char_type *[]);
static void output_ccc_subr (ccc_type, charcode_type, char_type *[]);
static void read_bzr_file (bzr_char_type *[], string);
static bzr_char_type **read_bzr_files (list_type);
static string read_command_line (int, string []);

/* Convert generic outline fonts in BZR format to a form that can
   actually be used for typesetting.  */

int
main (unsigned argc, string argv[])
{
  string bzr_name;
  bzr_preamble_type preamble;
  bzr_postamble_type postamble;
  bzr_char_type **bzr_chars;
  ccc_type **ccc_chars;
  char_type **chars, **subrs;
  string *first_name;
  string font_name, font_rootname;
  unsigned subr_count;
  unsigned this_char;
  boolean ccc_warning_p = true;
  unsigned char_count = 0;

  /* Initialize the list of BZR filenames to have one empty element at
     the beginning, which we fill in below with the main BZR name.  */
  bzr_name_list = list_init ();
  first_name = LIST_TAPPEND (&bzr_name_list, string);
  
  font_name = read_command_line (argc, argv);
  font_rootname = remove_suffix (basename (font_name));

  if (output_name == NULL)
    output_name = strtok (xstrdup (font_rootname), "0123456789");
  else 
    if (find_suffix (output_name) != NULL
	&& ((output[metafont] && output[pstype1])
	    || (output[metafont] && output[pstype3])
	    || (output[pstype1] && output[pstype3])))
     FATAL ("You can't specify a suffix and more than one output file");

  bzr_name = extend_filename (font_name, "bzr");
  if (!bzr_open_input_file (bzr_name))
    FATAL_PERROR (bzr_name);

  /* The first element of the BZR name list.  */
  *first_name = bzr_name;

  /* Read the global information from the BZR file and then close it, as
     we open it again below when reading the characters.  */
  preamble = bzr_get_preamble ();
  postamble = bzr_get_postamble ();
  bzr_close_input_file ();
  
  /* Read global TFM information, if we have it, and make sure we have
     an encoding vector.  */
  init_tfm_and_encoding_info (bzr_name, &tfm_global_info, &encoding_info);

  if (output[metafont])
    metafont_start_output (output_name, preamble, tfm_global_info);
  if (output[pstype1])
    pstype1_start_output (output_name, preamble, postamble);
  if (output[pstype3])
    pstype3_start_output (output_name, preamble, postamble);
  if (output[text])
    text_start_output (font_name, preamble);


  /* We don't know which BZR characters the CCC file might use, so just
     read them all.  */
  bzr_chars = read_bzr_files (bzr_name_list);

  /* Read the composite character definitions.  */
  if (ccc_name == NULL)
    {
      ccc_name = remove_suffix (font_name);
      ccc_warning_p = false;
    }
  ccc_chars = parse_ccc_file (ccc_name, bzr_chars, tfm_global_info,
                              BZR_DESIGN_SIZE (preamble), ccc_warning_p);


  /* We've read the input.  Figure out which characters we need as
     subroutines (because they're used in the CCC characters).  */
  subrs = subr_chars (bzr_chars, ccc_chars, starting_char, ending_char,
                      &subr_count);


  /* An ugly special case due to context dependence in PostScript.  */
  if (output[pstype1])
    pstype1_start_subrs (subr_count);

  /* Output all the characters needed as subroutines.  We cannot simply
     output every character as a subroutine, because for 1200dpi
     originals the result is too large to fit in even big Metafont's
     260K memory.  */
  REPORT ("Writing subroutines:\n");
  for (this_char = 0; this_char <= MAX_CHARCODE; this_char++)
    if (subrs[this_char])
      { 
        char_type subr = *subrs[this_char];
        
        char_count++;
        REPORT2 ("[%d]%c", this_char,
                 char_count % NCHARS_PER_LINE ? ' ' : '\n');

        /* If we have both BZR and CCC definitions for `this_char',
           prefer the former.  */
        if (CHAR_CLASS (subr) == bzr_char_class)
          output_bzr_subr (*CHAR_BZR (subr));
        else if (CHAR_CLASS (subr) == ccc_char_class)
          output_ccc_subr (*CHAR_CCC (subr), this_char, subrs);
        else
          abort ();
      }


  /* The ugly special case continues.  */
  if (output[pstype1])
    pstype1_start_chars (count_non_subr_chars (subrs, bzr_chars, ccc_chars));


  /* And finally, output the non-subroutine characters.  */
  REPORT1 ("%sWriting characters:\n",
           char_count % NCHARS_PER_LINE ? "\n" : "");
  for (this_char = starting_char; this_char <= ending_char; this_char++)
    {
      if (bzr_chars[this_char] || ccc_chars[this_char])
        {
          char_count++;
          REPORT2 ("[%d]%c", this_char,
                   char_count % NCHARS_PER_LINE ? ' ' : '\n');
          
          if (bzr_chars[this_char])
            output_bzr_char (*bzr_chars[this_char], subrs);
          else if (ccc_chars[this_char])
            output_ccc_char (*ccc_chars[this_char], this_char, subrs);
          else
            abort ();
        }
    }

  if (output[metafont]) metafont_finish_output (bzr_name_list, chars);
  if (output[pstype1]) pstype1_finish_output ();
  if (output[pstype3]) pstype3_finish_output ();
  if (output[text]) text_finish_output (postamble);

  if (char_count % NCHARS_PER_LINE != 0)
    REPORT ("\n");

  return 0;
}

/* Read the global information from the TFM file corresponding to
   BZR_NAME (if it exists) into TFM_INFO.  Also read an encoding file
   specified either by `encoding_name' or the codingscheme info from the
   TFM file into ENCODING_INFO.  */

static void
init_tfm_and_encoding_info (string bzr_name, tfm_global_info_type **tfm_info,
                            encoding_info_type *encoding_info)
{
  /* Open the main TFM file if it exists.  */
  string tfm_root = make_suffix (bzr_name, "tfm");
  string tfm_name = find_tfm_filename (tfm_root);

  if (tfm_name == NULL || !tfm_open_input_file (tfm_name))
    {
      *tfm_info = NULL;
      if (encoding_name == NULL)
        encoding_name = DEFAULT_ENCODING;
    }
  else
    {
      *tfm_info = XTALLOC1 (tfm_global_info_type);

      /* Just get the TFM global info and then close the file again, as
         we will open it again later for the characters.  */
      **tfm_info = tfm_get_global_info ();
      tfm_close_input_file ();

      /* If the user hasn't specified an encoding name, guess from the
         TFM info.  */
      if (encoding_name == NULL)
        encoding_name
          = coding_scheme_to_filename (TFM_CODING_SCHEME (**tfm_info));
    }

  /* In any case, read the encoding file.  I don't think we actually
     need this in all cases, but what the heck, it's no problem to read
     the default one.  */
  *encoding_info = read_encoding_file (encoding_name);
}

/* Read each BZR file in BZR_NAMES, merging all the characters.  We
   scale all characters to the size of the first font, and oblique the
   characters if `oblique_angle' is nonzero.  */

static bzr_char_type **
read_bzr_files (list_type bzr_names)
{
  unsigned c, name;
  bzr_char_type **bzr_chars = XTALLOC (MAX_CHARCODE + 1, bzr_char_type *);
  
  /* Assume we'll find nothing.  */
  for (c = 0; c <= MAX_CHARCODE; c++)
    bzr_chars[c] = NULL;

  /* Read all the files, updating the array as we go.  */
  for (name = 0; name < LIST_SIZE (bzr_names); name++)
    {
      string *bzr_name = LIST_ELT (bzr_names, name);
      
      REPORT1 ("(%s", *bzr_name);
      read_bzr_file (bzr_chars, *bzr_name);
      REPORT (")\n");
    }

  return bzr_chars;
}


/* If the BZR file BZR_NAME exists, read it and update BZR_CHARS.  We do
   not overwrite existing BZR characters, since earlier files should
   override later files.  (The first file is the main input file.)
   Complain about missing input files, since the user specified all
   these names explicitly.  We scale and oblique the characters we read
   if desired.  */

static void
read_bzr_file (bzr_char_type *bzr_chars[], string passed_bzr_name)
{
  unsigned char_count = 0;
  static real output_design_size = 0.0;
  string bzr_name = make_suffix (passed_bzr_name, "bzr");

  if (bzr_open_input_file (bzr_name))
    {
      bzr_char_type *file_char;
      bzr_preamble_type pre = bzr_get_preamble ();
      
      /* Scale everything to the design size of the first font.  */
      if (output_design_size == 0.0)
        output_design_size = BZR_DESIGN_SIZE (pre);

      while (file_char = bzr_get_next_char ())
        {
          unsigned code = CHARCODE (*file_char);
          
          char_count++;
          REPORT2 ("%c[%d]", char_count % NCHARS_PER_LINE ? ' ' : '\n', code);
          
          if (bzr_chars[code] == NULL)
            {
              real design_size_ratio
                = BZR_DESIGN_SIZE (pre) / output_design_size;

              BZR_SHAPE (*file_char)
                = oblique_splines (BZR_SHAPE (*file_char));
              scale_char (file_char, design_size_ratio);
              
              bzr_chars[code] = file_char;
            }
          else
            free (file_char);
        }

      bzr_close_input_file ();
    }
  else
    perror (bzr_name);
}

/* Output BZR_CHAR as a subroutine in whatever output formats the user
   has requested.  Except that PostScript Type 3 and text output don't
   do subroutines, so we just output the character straight.  */

static void
output_bzr_subr (bzr_char_type bzr_char)
{
  if (output[metafont])
    metafont_output_bzr_subr (bzr_char);
  
  if (output[pstype1])
    pstype1_output_bzr_subr (bzr_char);
  
  /* Don't need to do Type 3 or text, since all the subroutines will
     also get output as characters.  */
}


/* Output CCC_CHAR as a subroutine.  We do nothing for the Type 3
   output format here, since it doesn't deal with subroutines.  */

static void
output_ccc_subr (ccc_type ccc_char, charcode_type code, char_type *subrs[])
{
  if (output[metafont])
    metafont_output_ccc_subr (ccc_char, code, subrs);
  
  if (output[pstype1])
    pstype1_output_ccc_subr (ccc_char, code, subrs);
  
  /* Don't need to do Type 3 or text, since all the subroutines will
     also get output as characters.  */
}


/* Output BZR_CHAR as a character (as opposed to a subroutine).  */

static void
output_bzr_char (bzr_char_type bzr_char, char_type *subrs[])
{
  if (output[metafont])
    metafont_output_bzr_char (bzr_char, subrs);
  
  if (output[pstype1])
    pstype1_output_bzr_char (bzr_char, subrs);
  
  if (output[pstype3])
    pstype3_output_char (bzr_char);
  
  if (output[text])
    text_output_bzr_char (bzr_char);
}


/* Output CCC_CHAR as a character (as opposed to a subroutine).  Again,
   we do nothing for Type 3 here.  */

static void
output_ccc_char (ccc_type ccc_char, charcode_type code, char_type *subrs[])
{
  if (output[metafont])
    metafont_output_ccc_char (ccc_char, code, subrs);
  
  if (output[pstype1])
    pstype1_output_ccc_char (ccc_char, code, subrs);
  
  /* if (output[pstype3])
       do nothing; */
  
  if (output[text])
    text_output_ccc_char (ccc_char);
}

/* Count the number of characters (between `starting_char' and
   `ending_char') that we will output, not including subroutines.  */

static unsigned
count_non_subr_chars (char_type *subrs[], bzr_char_type *bzr_chars[],
                      ccc_type *ccc_chars[])
{
  unsigned this_char;
  unsigned char_count = 0;
  
  for (this_char = starting_char; this_char <= ending_char; this_char++)
    char_count += bzr_chars[this_char] || ccc_chars[this_char];

  return char_count;
}

/* Reading the options.  */

/* This is defined in version.c.  */
extern string version_string;

#define USAGE "Options:
<font_name> should be a filename, e.g., `cmr10'.  Any extension is ignored." \
  GETOPT_USAGE								     \
"concat <bzr_name_1>,<bzr_name_2>,...: concatenate the main input bzr file with
  the given <bzr_name>s; if a character code exists in more than one
  bzr file, it's the first occurrence that counts.
ccc-file <filename>: read the CCC file <filename> (if <filename> has a
  suffix) or <filename>.ccc (if it doesn't).  Default is <font_name>.
encoding <filename>: specify the encoding file; if <filename> has no
  suffix, use <filename>.enc, otherwise just <filename>.  Default is to
  try to guess the encoding from the coding_scheme string in the TFM
  file if exists, else to use the default " DEFAULT_ENCODING ".
help: print this message.
metafont: translate the font to a Metafont program.
mf: same as `metafont'.
oblique-angle <angle-in-degrees>: angle from the vertical by which to
  slant the shapes; default is 0.
output-file <filename>: output to <filename> (if it has a suffix) or to
  <filename>.<font format> (if it doesn't), where the <font format> is
  `mf', `gsf', etc.  <filename> cannot have a suffix if more than one of
  `metafont', `pstype1' and `pstype3' options are give.  Default is
  <font_name> with a trailing number removed.
ps-font-info <name>:<value>,...: assign each <value> to the
  corresponding <name> when outputting a PostScript font.  Possible
  <name>s: FontName, FamilyName, Weight, ItalicAngle, isFixedPitch,
  UnderlinePosition, UnderlineThickness, UniqueID, version.  Case is
  significant.  See the manual for more details.
pstype1: translate the font to (unencrypted) PostScript Type 1 font format.
pstype3: translate the font to PostScript Type 3 font format.
range <char1>-<char2>: only work on characters between <char1> and
  <char2> inclusive.
text: translate the font to human-readable text; write to stdout.
verbose: print brief progress reports.
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
    = { { "ccc-file",		1, 0, 0 },
        { "concat",		1, 0, 0 },
        { "encoding",		1, 0, 0 },
        { "fontname",		1, 0, 0 },
        { "help",               0, 0, 0 },
        { "metafont",		0, (int *) &output[metafont], 1 },
        { "mf",			0, (int *) &output[metafont], 1 },
        { "oblique-angle",	1, 0, 0 },
        { "output-file",	1, 0, 0 },
        { "ps-font-info",	1, 0, 0 },
        { "pstype1",		0, (int *) &output[pstype1], 1 },
        { "pstype3",		0, (int *) &output[pstype3], 1 },
        { "range",		1, 0, 0 },
        { "text",    		0, (int *) &output[text], 1 },
        { "verbose",            0, (int *) &verbose, 1 },
        { "version",            0, (int *) &printed_version, 1 },
        { 0, 0, 0, 0 } };

  while (true)
    {
      g = getopt_long_only (argc, argv, "", long_options, &option_index);

      if (g == EOF) break;
      if (g == '?') exit (1);  /* Unknown option.  */
  
      assert (g == 0); /* We have no short option names.  */
      
      if (ARGUMENT_IS ("ccc-file"))
        ccc_name = optarg;

      else if (ARGUMENT_IS ("concat"))
        append_concat_list (&bzr_name_list, optarg);
        
      else if (ARGUMENT_IS ("encoding"))
        encoding_name = optarg;
        
      else if (ARGUMENT_IS ("fontname"))
        ps_fontname = optarg;
        
      else if (ARGUMENT_IS ("help"))
        {
          fprintf (stderr, "Usage: %s [options] <font_name>.\n", argv[0]);
          fprintf (stderr, USAGE);
          exit (0);
        }

      else if (ARGUMENT_IS ("oblique-angle"))
        oblique_angle = M_PI * atof (optarg) / 180.0;
        
      else if (ARGUMENT_IS ("output-file"))
        output_name = optarg;

      else if (ARGUMENT_IS ("ps-font-info"))
        ps_global_info = optarg;
        
      else if (ARGUMENT_IS ("range"))
        GET_RANGE (optarg, starting_char, ending_char);
        
      else if (ARGUMENT_IS ("text"))
        report_file = stderr;

      else if (ARGUMENT_IS ("version"))
        printf ("%s.\n", version_string);
      
      /* Else it was just a flag; getopt has already done the assignment.  */
    }
  
  FINISH_COMMAND_LINE ();
}

/* The string S is a list of font names, separated by commas.  We append
   each onto CONCAT_LIST.  */

static void
append_concat_list (list_type *concat_list, string s)
{
  string name;
  
  /* The main routine must initialize CONCAT_LIST before we are called.  */
  assert (concat_list != NULL && LIST_SIZE (*concat_list) > 0);
  
  for (name = strtok (s, ","); name != NULL; name = strtok (NULL, ","))
    {
      string *new = LIST_TAPPEND (concat_list, string);
      *new = name;
    }
}
