/* bzr_input.c: read a BZR-format font file.

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
#include "file-input.h"
#include "scaled-num.h"
#include "spline.h"

#include "bzr_opcodes.h"
#include "bzr_types.h"

/* Where the input will come from.  */
static FILE *bzr_input_file = NULL;
static string bzr_input_filename;


/* We remember the design size, so we can scale by it.  */
static real design_size;


/* The character locators are part of the postamble.  */
static char_locator_type char_loc[MAX_CHARCODE + 1];


/* Low-level input.  These macros call the corresponding lib routines
   with the static variables for the input file and filename.  */

#define BZR_GET_BYTE() get_byte (bzr_input_file, bzr_input_filename)
#define BZR_GET_PREVIOUS_BYTE() \
  get_previous_byte (bzr_input_file, bzr_input_filename)
#define BZR_GET_TWO() get_two (bzr_input_file, bzr_input_filename)
#define BZR_GET_FOUR() get_four (bzr_input_file, bzr_input_filename)
#define BZR_GET_PREVIOUS_FOUR() \
  get_previous_four (bzr_input_file, bzr_input_filename);
#define BZR_FSEEK(offset, from_where) \
  xfseek (bzr_input_file, (long) offset, from_where, bzr_input_filename)

/* These routines read a somewhat higher-level value.  */
static real bzr_get_abbrev_scaled (void);
static real bzr_get_scaled (void);
static real bzr_get_abbrev_design_scaled (void);
static real bzr_get_design_scaled (void);
static real_coordinate_type bzr_get_abbrev_point (void);
static real_coordinate_type bzr_get_point (void);

/* Open the input file as name FILENAME.  We allow only one input file
   at a time.  */

extern boolean
bzr_open_input_file (string filename)
{
  if (bzr_input_file != NULL)
    FATAL2 ("bzr_open_input_file: Attempt to open `%s', but `%s' is
already open", filename, bzr_input_filename);

  bzr_input_filename = filename;
  bzr_input_file = fopen (filename, "r");
  
  if (bzr_input_file != NULL)
    {
      unsigned this_char;

      for (this_char = 0; this_char <= MAX_CHARCODE; this_char++)
        CHAR_POINTER (char_loc[this_char]) = NULL_BYTE_PTR;
      return true;
    }
  else
    return false;
}


/* Close the input file and clear all the state variables.  It's an
   error to close the file before it's been opened.  */

void
bzr_close_input_file ()
{
  assert (bzr_input_file != NULL);
  
  xfclose (bzr_input_file, bzr_input_filename);
  bzr_input_file = NULL;
  bzr_input_filename = NULL;
}

/* The preamble.  Since many numbers in the BZR file are scaled by the
   design size, we squirrel that value away.  */

bzr_preamble_type
bzr_get_preamble ()
{
  one_byte comment_length;
  bzr_preamble_type p;
  
  assert (bzr_input_file != NULL);

  BZR_FSEEK (0, SEEK_SET);
  design_size = BZR_DESIGN_SIZE (p) = bzr_get_scaled ();
  if (design_size <= 0.0)
    {
      WARNING1 ("bzr_get_preamble: Strange design size %f changed to 128pt",
                design_size);
      design_size = 128.0;
    }

  comment_length = BZR_GET_BYTE ();
  if (comment_length == 0)
    BZR_COMMENT (p) = "";
  else
    {
      BZR_COMMENT (p) = get_n_bytes (comment_length, bzr_input_file,
                                     bzr_input_filename);
      BZR_COMMENT (p) = xrealloc (BZR_COMMENT (p), comment_length + 1);
      BZR_COMMENT (p)[comment_length] = 0;
    }

  return p;
}

/* Read the character with code CODE and return it, or NULL if the
   character doesn't exist.  Also skip leading no-ops.  */

bzr_char_type *
bzr_get_char (charcode_type code)
{
  byte_count_type char_ptr;
  
  assert (bzr_input_file != NULL);
  
  /* First find the position of the character by reading the character
     locators (if we haven't already done so).  */
  if (char_loc == NULL)
    (void) bzr_get_postamble ();
  
  /* If the character isn't in the file, return NULL.  */
  char_ptr = CHAR_POINTER (char_loc[code]);
  if (char_ptr == NULL_BYTE_PTR)
    return NULL;
    
  /* Move to the beginning of the character definition.  */
  BZR_FSEEK (char_ptr, SEEK_SET);
  
  return bzr_get_next_char ();
}


/* This returns the character starting at the current position in
   `bzr_input_file', or NULL if we are at the postamble, or gives a
   fatal error if we're not at either.  */

bzr_char_type *
bzr_get_next_char ()
{
  bzr_char_type *c;
  one_byte command;
  real_coordinate_type current_point;
  real (*get_num) (void);
  real_coordinate_type (*get_point) (void);
  spline_list_type list;
  boolean no_current_point = true;

  assert (bzr_input_file);

  command = BZR_GET_BYTE ();
  if (command == BOC)
    get_num = bzr_get_design_scaled;
  else if (command == BOC_ABBREV)
    get_num = bzr_get_abbrev_design_scaled;
  else if (command == POST)
    return NULL;
  else
    FATAL1 ("bzr_get_next_char: Expected BOC or POST, found %u", command);
  
  c = XTALLOC1 (bzr_char_type);
  
  /* Read the beginning-of-character information that is always present.  */
  CHARCODE (*c) = BZR_GET_BYTE ();
  CHAR_SET_WIDTH (*c) = get_num ();
  CHAR_MIN_COL (*c) = get_num ();
  CHAR_MIN_ROW (*c) = get_num ();
  CHAR_MAX_COL (*c) = get_num ();
  CHAR_MAX_ROW (*c) = get_num ();
  
  if (CHAR_MIN_COL (*c) > CHAR_MAX_COL (*c))
    {
      WARNING2 ("bzr_get_next_char: Min col %f > max col %f",
               CHAR_MIN_COL (*c), CHAR_MAX_COL (*c));
      CHAR_MIN_COL (*c) = CHAR_MAX_COL (*c);
    }
  if (CHAR_MIN_ROW (*c) > CHAR_MAX_ROW (*c))
    {
      WARNING2 ("bzr_get_next_char: Min row %f > max row %f",
               CHAR_MIN_ROW (*c), CHAR_MAX_ROW (*c));
      CHAR_MIN_ROW (*c) = CHAR_MAX_ROW (*c);
    }
  
  /* Read the shape.  */
  BZR_SHAPE (*c) = new_spline_list_array ();
  list = *new_spline_list ();
  
  while ((command = BZR_GET_BYTE ()) != EOC)
    {
      switch (command)
        {
        case START_PATH:
          no_current_point = false;
          current_point = bzr_get_point ();
          
          /* At the first START_PATH we come to, `list' will be empty.
             If by chance the shape does have two consecutive
             START_PATH's, there's no point in remembering the first,
             anyway.  */
          if (SPLINE_LIST_LENGTH (list) > 0)
            {
              append_spline_list (&BZR_SHAPE (*c), list);
              list = *new_spline_list ();
            }
          break;
      
        case SPLINE:
        case SPLINE_ABBREV:
          {
            spline_type s = new_spline ();
            SPLINE_DEGREE (s) = CUBIC;

            if (no_current_point)
              {
                WARNING ("bzr_get_next_char: Spline before path started");
                current_point = (real_coordinate_type) { 0.0, 0.0 };
              }

            get_point
              = command == SPLINE ? bzr_get_point : bzr_get_abbrev_point;

            START_POINT (s) = current_point;
            CONTROL1 (s) = get_point ();
            CONTROL2 (s) = get_point ();
            current_point = END_POINT (s) = get_point ();

            append_spline (&list, s);
            break;
          }
      
        case LINE:
        case LINE_ABBREV:
          {
            spline_type s = new_spline ();
            SPLINE_DEGREE (s) = LINEAR;

            if (no_current_point)
              {
                WARNING ("bzr_get_next_char: Line before path started");
                current_point = (real_coordinate_type) { 0.0, 0.0 };
              }

            get_point = command == LINE ? bzr_get_point : bzr_get_abbrev_point;

            START_POINT (s) = current_point;
            current_point = END_POINT (s) = get_point ();

            append_spline (&list, s);
            break;
          }
      
        case NO_OP:
          break;
      
        default:
          FATAL1 ("bzr_get_char: Expected path command, found %u", command);
        }
    }

  append_spline_list (&BZR_SHAPE (*c), list);
  
  return c;
}

/* We find the POST byte by starting at the end of the file and reading
   backwards until we get to the pointer.  Then we follow the pointer
   and read the postamble forwards.  */

bzr_postamble_type
bzr_get_postamble ()
{
  one_byte command;
  byte_count_type post_ptr;
  bzr_postamble_type p;

  assert (bzr_input_file != NULL);
  
  BZR_FSEEK (0, SEEK_END);
  
  /* We should find at least one NO_OP at the end.  */
  do
    command = BZR_GET_PREVIOUS_BYTE ();
  while (command == NO_OP);
  
  if (command != BZR_ID)
    WARNING1 ("bzr_get_postamble: Expected BZR id, found %u", command);
  
  post_ptr = BZR_GET_PREVIOUS_FOUR ();
  BZR_FSEEK (post_ptr, SEEK_SET);
  
  command = BZR_GET_BYTE ();
  if (command != POST)
    FATAL1 ("bzr_get_postamble: Expected POST, found %u", command);

  MIN_COL (BZR_FONT_BB (p)) = bzr_get_design_scaled ();
  MIN_ROW (BZR_FONT_BB (p)) = bzr_get_design_scaled ();
  MAX_COL (BZR_FONT_BB (p)) = bzr_get_design_scaled ();
  MAX_ROW (BZR_FONT_BB (p)) = bzr_get_design_scaled ();
  
  /* The character locators.  */
  while ((command = BZR_GET_BYTE ()) == CHAR_LOC || command == CHAR_LOC_ABBREV)
    {
      one_byte code = BZR_GET_BYTE ();
      if (CHAR_POINTER (char_loc[code]) != NULL_BYTE_PTR)
        WARNING1 ("bzr_get_postamble: Second locator for character %u",
                  code);
      CHAR_POINTER (char_loc[code])
        = command == CHAR_LOC ? BZR_GET_FOUR () : BZR_GET_TWO ();
    }

  BZR_NCHARS (p) = command;

  return p;
}

/* Low-level input routines.  */

/* Because the scaled numbers in the file are only three bytes long, we
   have to read the most significant byte into an integer, so that the
   sign extension will be done by the compiler.  */

static real
bzr_get_scaled ()
{
  signed_byte x;
  scaled s;

  x = BZR_GET_BYTE ();
  s = x << 16;;
  s |= BZR_GET_BYTE () << 8;
  s |= BZR_GET_BYTE ();
  
  return scaled_to_real (s);
}


static real
bzr_get_design_scaled ()
{
  return bzr_get_scaled () * design_size;
}


static real_coordinate_type
bzr_get_point ()
{
  real_coordinate_type c;
  
  c.x = bzr_get_design_scaled ();
  c.y = bzr_get_design_scaled ();
  
  return c;
}


/* Most numbers in a BZR file are less than the design size.  */

static real
bzr_get_abbrev_scaled ()
{
  signed_byte x;
  scaled s;

  x = BZR_GET_BYTE ();
  s = x << 8;
  s |= BZR_GET_BYTE ();
  
  return scaled_to_real (s);
}


static real
bzr_get_abbrev_design_scaled ()
{
  return bzr_get_abbrev_scaled () * design_size;
}


static real_coordinate_type
bzr_get_abbrev_point ()
{
  real_coordinate_type c;
  
  c.x = bzr_get_abbrev_design_scaled ();
  c.y = bzr_get_abbrev_design_scaled ();
  
  return c;
}
