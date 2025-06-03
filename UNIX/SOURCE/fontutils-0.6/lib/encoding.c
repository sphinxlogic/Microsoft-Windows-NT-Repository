/* encoding.c: read a font encoding (.enc) file.

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

#include "encoding.h"
#include "libfile.h"
#include "str-lcase.h"
#include "tfm.h"


static encoding_char_type parse_encoding_line (string);
static tfm_ligature_type parse_ligature (void);

/* Look through INFO for NAME.  */

int 
encoding_number (encoding_info_type info, string name)
{
  unsigned code;

  if (name == NULL)
    return -1;

  for (code = 0; code <= MAX_CHARCODE; code++)
    if (ENCODING_CHAR_NAME (info, code) != NULL
        && STREQ (ENCODING_CHAR_NAME (info, code), name))   
      return code;

  return -1;  
}

/* Return the basename for the encoding file in which the encoding 
   CODING_SCHEME can be found.  If we can't find CODING_SCHEME, return
   DEFAULT_ENCODING.  */

string
coding_scheme_to_filename (string coding_scheme)
{
  string mapping_line, enc_name;
  string lc_coding_scheme = str_to_lower (coding_scheme);
  
  libfile_start ("encoding", "map");

  while ((mapping_line = libfile_line ()) != NULL)
    {
      string test_coding_scheme, lc_test_coding_scheme;

      enc_name = strtok (mapping_line, " \t");
      test_coding_scheme = strtok (NULL, "\t");
      lc_test_coding_scheme = str_to_lower (test_coding_scheme);

      if (STREQ (lc_coding_scheme, lc_test_coding_scheme))
        break;

      free (lc_test_coding_scheme);
      free (mapping_line);
    }

  libfile_close ();

  if (mapping_line == NULL)
    {
      WARNING3 ("%s: unknown encoding `%s'; using default `%s'",
                "encoding.map", coding_scheme, DEFAULT_ENCODING);
      enc_name = DEFAULT_ENCODING;
    }

  free (lc_coding_scheme);
  
  return enc_name;
}

/* Parse the encoding file `FILENAME.enc' and return a structure
   describing what we read.  If the file doesn't exist, give a fatal
   error.  */

encoding_info_type
read_encoding_file (string filename)
{
  encoding_info_type info;
  string line;
  unsigned code;
  
  /* Prepare to read from FILENAME.  */
  libfile_start (filename, "enc");
  
  /* Start with an empty encoding.  */
  for (code = 0; code <= MAX_CHARCODE; code++)
    ENCODING_CHAR_NAME (info, code) = NULL;

  /* The entire first line is the TFM `CODINGSCHEME'.  */
  ENCODING_SCHEME_NAME (info) = libfile_line ();
  
  /* Each remaining line defines one character.  */
  code = 0;
  while ((line = libfile_line ()) != NULL)
    {
      ENCODING_CHAR_ELT (info, code) = parse_encoding_line (line);
      code++;
      free (line);
    }
  
  return info;
}

/* Parse one line of the encoding file; this specifies one character.
   The BNF is:
   
   <line>      ::= <word>  <ligatures>

   <ligatures> ::= <ligature> [<ligature>]?
                 | <empty>

   <ligature>  ::= lig <charcode> =: <charcode>
   
   Whitespace must be between all elements.
   
   If the line is malformed, we give a fatal error.  */

#define WORD_SEPARATOR " \t"
#define GET_WORD() strtok (NULL, WORD_SEPARATOR)

static encoding_char_type
parse_encoding_line (string line)
{
  encoding_char_type c;
  
  c.name = xstrdup (strtok (line, WORD_SEPARATOR));
  c.ligature = list_init ();
  
  do
    {
      tfm_ligature_type *lig;
      string t = GET_WORD ();
      
      if (t == NULL) /* Do we have more ligature specs?  */
        break;
        
      if (!STREQ (t, "lig"))
        LIBFILE_ERROR1 ("Expected `lig', found `%s'", t);

      lig = LIST_TAPPEND (&c.ligature, tfm_ligature_type);
      *lig = parse_ligature ();
    }
  while (true);

  return c;
}


/* Parse a single ligature: <charcode> =: <charcode>.  */

static tfm_ligature_type
parse_ligature ()
{
  tfm_ligature_type lig;
  boolean valid;
  string t = GET_WORD ();
  
  lig.character = parse_charcode (t, &valid);
  
  if (!valid)
    LIBFILE_ERROR1 ("Invalid right character code `%s'", t);

  t = GET_WORD ();
  if (!STREQ (t, "=:"))
    LIBFILE_ERROR1 ("Expected `=:', found `%s'", t);
  
  t = GET_WORD ();
  lig.ligature = parse_charcode (t, &valid);
  if (!valid)
    LIBFILE_ERROR1 ("Invalid ligature character code `%s'", t);
  
  return lig;
}
