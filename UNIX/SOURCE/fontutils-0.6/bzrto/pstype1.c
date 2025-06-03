/* pstype1.c: translate the BZR font to a Type 1 PostScript font. 
   See the book Adobe Type 1 Font Format for details of the format.

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

#include "bounding-box.h"
#include "bzr.h"
#include "char.h"
#include "encoding.h"
#include "hexify.h"
#include "spline.h"
#include "varstring.h"
#include "vector.h"

#include "input-ccc.h"
#include "pstype1.h"
#include "psutil.h"

#include "ccc.h"


/* Where we'll output.  */
static FILE *ps_file;
static string ps_filename;

/* Factor by which to multiply a dimension in points to get a number in
   Adobe's 1000 units/em coordinate system.  */
static real em_factor;

/* The interword space, in points.  */
static real interword_space;

/* Whether or not a particular character has been written.  We depend on
   this being initialized to false by virtue of its storage class.  */
static boolean char_output_p[ENCODING_VECTOR_SIZE];

/* For each character code, tells the corresponding subroutine number;
   assigned by `output_subr_start'.  */
static unsigned subr_number[MAX_CHARCODE + 1];

/* As we read a character, we encode each command in the Type 1 scheme
   and stick the result into a buffer.  (Because we need to output the
   length of the entire charstring before we output the charstring
   itself, we can't output as we go.)  */
static variable_string charstring_buffer;

/* Before encrypting, fill this and prepend onto above.  */
static variable_string pre_charstring_buffer;


/* This should be used for outputting a string S on a line by itself.  */
#define OUT_LINE(s)							\
  do { fprintf (ps_file, "%s\n", s); } while (0)			\

/* These output their arguments, preceded by the indentation.  */
#define OUT1(s, e)							\
  do { fprintf (ps_file, s, e); } while (0)

#define OUT2(s, e1, e2)							\
  do { fprintf (ps_file, s, e1, e2); } while (0)

#define OUT4(s, e1, e2, e3, e4)						\
  do { fprintf (ps_file, s, e1, e2, e3, e4); } while (0)


/* Type 1 opcodes.  */
typedef unsigned char type1_opcode;

#define T1_CALLSUBR	10
#define T1_CLOSEPATH	 9
#define T1_ENDCHAR	14
#define T1_HLINETO	 6
#define T1_HMOVETO	22
#define T1_HSBW		13
#define T1_HVCURVETO	31
#define T1_RETURN	11
#define T1_RLINETO	 5
#define T1_RMOVETO	21
#define T1_RRCURVETO	 8
#define T1_VHCURVETO	30
#define T1_VLINETO	 7
#define T1_VMOVETO	 4

#define READONLY_PUT "|"
#define READONLY_DEF "|-"

static void absolute_cs_number (int, variable_string *);
static void begin_char (string);
static void end_char (real, real);
static void cs_byte (one_byte, variable_string *);
static variable_string cs_encrypt (variable_string);
static void cs_number (real, variable_string *);
static void init_charstring_buffers (void);
static void out_bb_char
  (real_bounding_box_type *, real_coordinate_type *, bzr_char_type);
static void out_curveto (real_coordinate_type *, spline_type);
static void out_lineto (real_coordinate_type *, real_coordinate_type);
static void out_move (real_coordinate_type *, real, real, boolean);
static void out_moveto (real_coordinate_type *, real_coordinate_type);
static void out_point (real_coordinate_type *, real_coordinate_type);
static void out_relative_cmd (real_coordinate_type *, real_coordinate_type,
                              type1_opcode, type1_opcode, type1_opcode);
static void output_bzr_body (bzr_char_type);
static real_bounding_box_type output_ccc_body (ccc_type, char_type **);
static void output_char_finish (real, real, charcode_type);
static boolean output_char_start (charcode_type);
static void output_charstring_buffers (string);
static void output_private_dict_1 (unsigned);
static void output_private_dict_2 (void);
static void output_subr_end (void);
static void output_subr_start (charcode_type);
static int points_to_adobes (real);

/* Convert N, a number in printer's points, to the 1000-unit/em
   coordinate system the Type 1 format requires.  (We call these units
   ``adobes''.)  We've already computed the scaling factor `em_factor'
   for this, since it depends only on the design size of the font.  */

static int
points_to_adobes (real n)
{
  int n_in_adobes = ROUND (n * em_factor);
  
  return n_in_adobes;
}

/* This routine should be called before the others in this file.  It
   opens the output file `OUTPUT_NAME.gsf', writes some preliminary
   boilerplate, the font bounding box, and sets up to write the
   characters.  */

void
pstype1_start_output (string output_name,
		      bzr_preamble_type pre, bzr_postamble_type post)
{
  ps_font_info_type ps_info = ps_set_font_info ();

  /* Remember the natural interword space, so we can output it later.  */
  interword_space = ps_info.interword_space;

  /* If a Type 3 font is eexec-encrypted and in ASCII, its extension
     should be `pfa'.  If they are eexec-encrypted and binary, its
     extension should be `pfb'.  But if they're not encrypted, I don't
     know of any real convention.  We use Ghostscript's semi-conventional
     extension, `gsf'.  */
  ps_filename = extend_filename (output_name, "gsf");
  ps_file = xfopen (ps_filename, "w");
  
  /* Output some identification.  */
  OUT2 ("%%!FontType1-1.0: %s %s\n", ps_info.font_name, ps_info.version);

  ps_start_font (ps_file, ps_info, BZR_COMMENT (pre));
  OUT_LINE ("/FontType 1 def");

  /* Always use the 1000 units-to-the-em coordinate system which Adobe
     has foisted on the world, and remember the scaling factor given
     this font's size.  */
  OUT_LINE ("/FontMatrix [0.001 0 0 0.001 0 0] readonly def"); 
  em_factor = 1000.0 / BZR_DESIGN_SIZE (pre);
  
  /* Some interpreters refuse to handle a normal array,
     despite the specification.  */
  OUT4 ("/FontBBox {%d %d %d %d} def\n",
        points_to_adobes (MIN_COL (BZR_FONT_BB (post))),
        points_to_adobes (MIN_ROW (BZR_FONT_BB (post))),
        points_to_adobes (MAX_COL (BZR_FONT_BB (post))),
        points_to_adobes (MAX_ROW (BZR_FONT_BB (post))) );

  /* We output the encoding and some of the Private dictionary after
     we've read all the characters, so we can guess good values.  I hope
     this doesn't break any interpreters, but it probably will.  */
  output_private_dict_1 (ps_info.unique_id);
}

/* Start the character subroutine definitions.  We need to make the
   procedures in `Private' visible because the charstring definitions
   use them -- i.e., the `-|', `-|', and `|' routines.  */
  
void
pstype1_start_subrs (unsigned subr_count)
{
  OUT_LINE ("Private begin");
  OUT1 ("/Subrs %d array \n", subr_count);
}


/* Output the BZR character C as a subroutine.  */

void
pstype1_output_bzr_subr (bzr_char_type c)
{
  output_subr_start (CHARCODE (c));
  output_bzr_body (c);
  output_subr_end ();
}


/* And the same, for the CCC character C.  */

void
pstype1_output_ccc_subr (ccc_type c, charcode_type code, char_type *subrs[])
{
  output_subr_start (code);
  output_ccc_body (c, subrs);
  output_subr_end ();
}


/* Output a call to the subroutine corresponding to the character code
   CODE.  */

#define OUT_CHAR_SUBR(code) 						\
 do {									\
   absolute_cs_number (subr_number[code], &charstring_buffer);		\
   cs_byte (T1_CALLSUBR, &charstring_buffer);				\
 } while (0)


/* Output the common beginning of a subroutine.  We don't check if CODE
   is in the encoding, since a character might well be used as a
   subroutine but not be encoded.  */

static void
output_subr_start (charcode_type code)
{
  static unsigned current_subr_number = 0;
  
  subr_number[code] = current_subr_number;
  OUT1 ("dup %d ", current_subr_number++);
  init_charstring_buffers ();
}


/* And the common ending.  */

static void
output_subr_end ()
{
  cs_byte (T1_RETURN, &charstring_buffer);
  output_charstring_buffers (READONLY_PUT);
}

/* End the subroutine definitions and start the character definitions.
   Aside from the characters in the input font, we always output
   the two characters `.notdef' and `space' (this happens in
   `pstype1_finish_output).  */
     
void
pstype1_start_chars (unsigned char_count)
{
  OUT_LINE (READONLY_DEF " % Subrs");
  OUT1 ("/CharStrings %d dict begin\n", char_count + 2);
}


/* Output the BZR character C; if it is defined in SUBRS, just output a
   call to the already-defined subroutine.  */

void
pstype1_output_bzr_char (bzr_char_type c, char_type *subrs[])
{
  unsigned code = CHARCODE (c);
  
  if (output_char_start (code))
    {
      if (subrs[code])
        OUT_CHAR_SUBR (code);
      else
        output_bzr_body (c);
      
      output_char_finish (CHAR_LSB (c), CHAR_SET_WIDTH (c), code);
    }
}


/* Output the CCC character C with character code CODE, and using the
   character definitions in SUBRS.  */

void
pstype1_output_ccc_char (ccc_type c, charcode_type code, char_type *subrs[])
{
  if (output_char_start (code))
    {
      if (subrs[code])
        WARNING ("Sorry, CCC characters can't be subroutines yet");
      else
        {
          real_bounding_box_type bb = output_ccc_body (c, subrs);
          output_char_finish (MIN_COL (bb), MAX_COL (bb), code);
        }
    }
}


/* Common beginning for characters.  */

static boolean
output_char_start (charcode_type code)
{
  string charname = ps_encoding_name (code);
  boolean ok = !STREQ (charname, ".notdef");
  
  if (ok)
    begin_char (charname);
  
  return ok;
}


/* Begin a character definition, i.e., a definition in the CharStrings
   dictionary.  We are given the character name, the left sidebearing,
   and the set width.  The latter two are in printer's points.  We
   always output an `hsbw' command -- our characters do not have y
   sidebearings.  */

static void
begin_char (string name)
{
  OUT1 ("/%s\t", name);

  /* We cannot just output the bytes of the charstring as we go, because
     we need to output the length of the charstring before the
     charstring itself, so we can use readhexstring in the PostScript.  */
  init_charstring_buffers ();
}


/* Common ending.  */

static void
output_char_finish (real lsb, real set_width, charcode_type code)
{
  end_char (lsb, set_width);
  char_output_p[code] = true;
}


/* End a character definition with the `endchar' command.  */

static void
end_char (real lsb, real set_width)
{
  cs_number (lsb, &pre_charstring_buffer);
  cs_number (set_width, &pre_charstring_buffer);
  cs_byte (T1_HSBW, &pre_charstring_buffer);

  cs_byte (T1_ENDCHAR, &charstring_buffer);
  output_charstring_buffers (READONLY_DEF);
}

/* Output the splines in C.  */

static void
output_bzr_body (bzr_char_type c)
{
  unsigned this_list;
  spline_list_array_type shape = BZR_SHAPE (c);
  real_coordinate_type start = { 0.0, 0.0 };
  real_coordinate_type current = start;

  /* Go through the list of splines, outputting the
     path-construction commands.  Since all Type 1 commands are
     relative, we also keep track of the current point.  */
  for (this_list = 0; this_list < SPLINE_LIST_ARRAY_LENGTH (shape);
       this_list++)
    {
      unsigned this_spline;
      spline_list_type list = SPLINE_LIST_ARRAY_ELT (shape, this_list);
      spline_type first_spline = SPLINE_LIST_ELT (list, 0);

      /* Start this path.  */
      out_moveto (&current, START_POINT (first_spline));
      
      /* For each spline in the list, output the Type 1 commands.  */
      for (this_spline = 0; this_spline < SPLINE_LIST_LENGTH (list);
           this_spline++)
        {
          spline_type s = SPLINE_LIST_ELT (list, this_spline);

          if (SPLINE_DEGREE (s) == LINEAR)
            out_lineto (&current, END_POINT (s));
          else
            out_curveto (&current, s);
        }
      cs_byte (T1_CLOSEPATH, &charstring_buffer);
    }

  /* If we're outputting a subroutine, we want to wind up back at the
     origin, I guess.  */
  out_moveto (&current, start);
}


/* Output the composite character C, with all subroutines defined in SUBRS.  */

#define OUT_HMOVE(h, a) out_move (&current, h, 0.0, a)
#define OUT_VMOVE(v, a) out_move (&current, 0.0, v, a)

static real_bounding_box_type
output_ccc_body (ccc_type ccc_char, char_type **subrs)
{
  unsigned this_cmd;
  real_coordinate_type current = (real_coordinate_type) { 0.0, 0.0 };
  real_bounding_box_type bb
    = (real_bounding_box_type) { FLT_MAX, FLT_MIN, FLT_MAX, FLT_MIN };

  for (this_cmd = 0; this_cmd < LIST_SIZE (ccc_char); this_cmd++)
    { 
      ccc_cmd_type cmd = *(ccc_cmd_type *) LIST_ELT (ccc_char, this_cmd);
      
      switch (CCC_OPCODE (cmd))
        {
        case TR_SETCHAR:
          {
            char_type subr = *subrs[CCC_CHARCODE (cmd)];

            if (CHAR_CLASS (subr) == bzr_char_class)
              {
                bzr_char_type bzr_char = *CHAR_BZR (subr);
                
                update_real_bounding_box (&bb, current);
                OUT_HMOVE (CHAR_LSB (bzr_char), false);

                out_bb_char (&bb, &current, bzr_char);

                OUT_HMOVE (CHAR_RSB (bzr_char), false);
                update_real_bounding_box (&bb, current);
              }
            else
              /* We haven't implemented this yet since it doesn't seem
                 all that necessary at the moment, and it would require
                 figuring out a bounding box and possibly side bearings
                 for CCC character -- certainly not impossible, but not
                 trivial, either.  */
              WARNING ("Sorry, I can't `setchar' a CCC char yet");
          }
          break;

        case TR_SETCHARBB:
          {
            char_type subr = *subrs[CCC_CHARCODE (cmd)];

            if (CHAR_CLASS (subr) == bzr_char_class)
              out_bb_char (&bb, &current, *CHAR_BZR (subr));
            else
              WARNING ("Sorry, I can't `setcharbb' a CCC char yet");
          }
          break;

        case TR_HMOVE:
          OUT_HMOVE (CCC_DIMEN_AMOUNT (cmd), CCC_DIMEN_ABSOLUTE (cmd));
          break;

        case TR_VMOVE:
          OUT_VMOVE (CCC_DIMEN_AMOUNT (cmd), CCC_DIMEN_ABSOLUTE (cmd));
          break;

        default:
          FATAL1 ("Bad command %d in CCC character", CCC_OPCODE (cmd));
	}
    }
  
  return bb;
}
/* Move by H horizontally and V vertically.  If ABSOLUTE is true, the
   amount moved is supposed to be independent of the character size, but
   we don't know how to implement that yet.  Update CURRENT.  */

static void
out_move (real_coordinate_type *current, real h, real v, boolean absolute)
{
  real_coordinate_type new
    = (real_coordinate_type) { current->x + h, current->y + v};

  if (absolute)
    WARNING2 ("Move [by (%.2f,%.2f)] in absolute units unimplemented", h, v);

  out_moveto (current, new);
  *current = new;
}


/* Output the character shape in BZR_CHAR, sans side bearings.  Update
   CURRENT by the width of the shape, and update BB by the points before
   and after the character.  We take into account the height of the
   character when updating BB, even though we don't translate the
   current point up there.  */

#define VOFFSET_POINT(voffset) \
  ((real_coordinate_type) { current->x, current->y + (voffset) })

static void
out_bb_char (real_bounding_box_type *bb, real_coordinate_type *current,
             bzr_char_type bzr_char)
{
  /* Update BB with the current point.  */
  update_real_bounding_box (bb, VOFFSET_POINT (CHAR_MIN_ROW (bzr_char)));

  /* Output the call to the character.  */
  OUT_CHAR_SUBR (CHARCODE (bzr_char));
  
  /* Move by the width of the shape.  */
  out_move (current, BB_WIDTH (CHAR_BB (bzr_char)), 0, false);

  /* Update the BB again with the new current point.  */
  update_real_bounding_box (bb, VOFFSET_POINT (CHAR_MAX_ROW (bzr_char)));
}

/* Output the delta between *CURRENT and P, then update *CURRENT to be P.  */

static void
out_point (real_coordinate_type *current, real_coordinate_type p)
{
  cs_number (p.x - current->x, &charstring_buffer);
  cs_number (p.y - current->y, &charstring_buffer);
  *current = p;
}


/* Output some kind of lineto command between *CURRENT and P, and update
   *CURRENT.  If the line is horizontal or vertical, we can use hlineto
   or vlineto; otherwise, we have to use rlineto.  */

static void
out_lineto (real_coordinate_type *current, real_coordinate_type p)
{
  out_relative_cmd (current, p, T1_RLINETO, T1_HLINETO, T1_VLINETO);
}


/* Like out_lineto, except for moves.  */

static void
out_moveto (real_coordinate_type *current, real_coordinate_type p)
{
  out_relative_cmd (current, p, T1_RMOVETO, T1_HMOVETO, T1_VMOVETO);
}


/* Output a command relative to the points *CURRENT and P, then update
   CURRENT.  Use V_OPCODE if the two points are aligned vertically,
   H_OPCODE if horizontally, else R_OPCODE.  */

static void
out_relative_cmd (real_coordinate_type *current, real_coordinate_type p,
                  type1_opcode r_opcode, type1_opcode h_opcode,
                  type1_opcode v_opcode)
{
  boolean x_equal = epsilon_equal (p.x, current->x);
  boolean y_equal = epsilon_equal (p.y, current->y);
  
  if (x_equal)
    { /* If the two points are the same, we can omit this command
         altogether.  (And we should improve our curve-generating
         algorithm, perhaps.)  */
      if (!y_equal)
        {
          cs_number (p.y - current->y, &charstring_buffer);
          cs_byte (v_opcode, &charstring_buffer);
          current->y = p.y;
        }
    }
  else if (y_equal)
    {
      cs_number (p.x - current->x, &charstring_buffer);
      cs_byte (h_opcode, &charstring_buffer);
      current->x = p.x;
    }
  else
    {
      out_point (current, p);
      cs_byte (r_opcode, &charstring_buffer);
    }
}


/* Output a curveto command.  The relative versions here occur if the
   first tangent is horizontal and the second vertical, or vice versa.  */

static void
out_curveto (real_coordinate_type *current, spline_type s)
{
  /* First tangent vertical, second horizontal?  */
  if (epsilon_equal (current->x, CONTROL1 (s).x)
      && epsilon_equal (CONTROL2 (s).y, END_POINT (s).y))
    {
      cs_number (CONTROL1 (s).y - current->y, &charstring_buffer);
      current->y = CONTROL1 (s).y;
      out_point (current, CONTROL2 (s));
      cs_number (END_POINT (s).x - current->x, &charstring_buffer);
      current->x = END_POINT (s).x;
      cs_byte (T1_VHCURVETO, &charstring_buffer);
    }
  
  /* First tangent horizontal, second vertical?  */
  else if (epsilon_equal (current->y, CONTROL1 (s).y)
      && epsilon_equal (CONTROL2 (s).x, END_POINT (s).x))
    {
      cs_number (CONTROL1 (s).x - current->x, &charstring_buffer);
      current->x = CONTROL1 (s).x;
      out_point (current, CONTROL2 (s));
      cs_number (END_POINT (s).y - current->y, &charstring_buffer);
      current->y = END_POINT (s).y;
      cs_byte (T1_HVCURVETO, &charstring_buffer);
    }
  
  /* Fall back to general case.  */
  else
    {
      out_point (current, CONTROL1 (s));
      out_point (current, CONTROL2 (s));
      out_point (current, END_POINT (s));
      cs_byte (T1_RRCURVETO, &charstring_buffer);
    }
}

/* Charstring operations.  */

static void
init_charstring_buffers ()
{
  pre_charstring_buffer = vs_init ();
  charstring_buffer = vs_init ();
}


/* This appends the single byte B to BUFFER, unchanged.  */

static void
cs_byte (one_byte b, variable_string *buffer)
{
  vs_append_char (buffer, b);
}


/* This appends to BUFFER the charstring encoding of the number N,
   assumed to be in printer's points.  */

static void
cs_number (real n, variable_string *buffer)
{
  int n_in_adobes = points_to_adobes (n);

  absolute_cs_number (n_in_adobes, buffer);
}


/* See section 6.2 of the book Adobe Type 1 Font Format.  */

static void
absolute_cs_number (int n, variable_string *buffer)
{
  if (-107 <= n && n <= 107)
    cs_byte (n + 139, buffer);
  else if (108 <= n && n <= 1131)
    {
      n -= 108;
      cs_byte (n / 256 + 247, buffer);
      cs_byte (n % 256, buffer);
    }
  else if (-1131 <= n && n <= -108)
    {
      n = -n - 108;
      cs_byte (n / 256 + 251, buffer);
      cs_byte (n % 256, buffer);
    }
  else
    {
      cs_byte (255, buffer);
      cs_byte ((n & 0xff000000) >> 24, buffer);
      cs_byte ((n & 0x00ff0000) >> 16, buffer);
      cs_byte ((n & 0x0000ff00) >> 8, buffer);
      cs_byte  (n & 0x000000ff, buffer);
    }
}


/* Perform charstring encryption on S, returning the (dynamically
   allocated) result as a string of binary characters.  See page 62 of
   Adobe Type 1 Font Format.  */

#define CS1 52845
#define CS2 22719

static variable_string
cs_encrypt (variable_string s)
{
  unsigned i;
  unsigned short cs_key = 4330;
  variable_string e = vs_init ();

  for (i = 0; i < VS_USED (s); i++)
    {
      unsigned char cipher = ((unsigned char) VS_CHARS (s)[i]) ^ (cs_key >> 8);
      cs_key = (cipher + cs_key) * CS1 + CS2;
      vs_append_char (&e, cipher);
    }
  
  return e;
}


/* Output what we've collected in `pre_charstring_buffer' and
   `charstring_buffer'.  Because Type 1 BuildChar is only required to
   handle encrypted charstrings, we must encrypt our encoded charstring.  */

static void
output_charstring_buffers (string end)
{
  variable_string encrypted_string;
  string hex_string;

  encrypted_string 
    = cs_encrypt (vs_concat (pre_charstring_buffer, charstring_buffer));

  hex_string = hexify (VS_CHARS (encrypted_string),
                       VS_USED (encrypted_string)); 

  /* The `-|' and `|-' procedures are defined in the Private dictionary.
     They read the hex string and define the character.  */
  OUT2 ("%d -| %s ", VS_USED (encrypted_string), hex_string);
  OUT_LINE (end);

  free (hex_string);
  vs_free (&encrypted_string);
  vs_free (&pre_charstring_buffer);
  vs_free (&charstring_buffer);
}

/* This is called last, after all the characters are output. We close
   the output file after outputting what's left.  */

void
pstype1_finish_output ()
{
  int space_encoding = ps_encoding_number ("space");
  
  /* If the space character hasn't been output, do so (even if it won't
     be encoded).  */
  if (space_encoding == -1 || !char_output_p[space_encoding])
    {
      begin_char ("space");
      end_char (0, interword_space);
      if (space_encoding != -1)
        char_output_p[space_encoding] = true;
    }

  /* Always output `.notdef'.  */
  begin_char (".notdef");
  end_char (0, 0);

  /* Pop the Private and CharStrings dictionaries from the dictionary
     stack, then define the latter in the main font dict.  */
  OUT_LINE ("currentdict end readonly % CharStrings");
  OUT_LINE ("end % Private");
  OUT_LINE ("def % CharStrings");

  /* Two more elements go in the font dictionary.  */
  ps_output_encoding (ps_file, char_output_p);
  output_private_dict_2 ();

  /* Define the font (and discard the result from the operand stack).  */
  OUT_LINE ("currentdict end definefont pop");
  
  xfclose (ps_file, ps_filename);
}

/* The first part of the Private dictionary defines things that are
   independent of the character definitions.  */

static void
output_private_dict_1 (unsigned unique_id)
{
  OUT_LINE ("/Private 9 dict begin");
  OUT_LINE ("  /MinFeature {16 16} def");
  OUT_LINE ("  /password 5839 def");

  /* According to the spec, the only reason to keep `lenIV' at 4 is to
     be compatible with the interpreter in the original LaserWriter.  We
     don't care about that, so we may as well reduce the number of bytes
     wasted on encryption.  */
  OUT_LINE ("  /lenIV 0 def");

  if (unique_id)
    OUT1 ("  /UniqueID %d def\n", unique_id);

  /* Here is how a character or a subroutine will get defined.  We
     expect two arguments on the stack -- a character/subroutine name C,
     and an integer N.  We read an N-byte hex string) from the file, and
     define C to be that string.  This is a simplified version of what
     Ghostscript's bdftops.ps does (the main simplification being no
     provision for lazy evaluation of characters).  */
  OUT_LINE ("  /-| {string currentfile exch readhexstring pop} readonly def");

  /* Once we have the string, this procedure defines the character.  */
  OUT_LINE ("  /|- {readonly def} readonly def");

  /* Likewise, for subroutines.  */
  OUT_LINE ("  /|  {readonly put} readonly def");
  
  /* End defining the private dictionary.  */
  OUT_LINE ("currentdict end def");
}


/* The second part of the Private dictionary defines the hinting
   information, which we know only after we've seen the characters.  Or
   at least we might know it after we understand how to output hints.  */

static void
output_private_dict_2 ()
{
  OUT_LINE ("Private begin");
  OUT_LINE ("  /BlueValues [] def");
  OUT_LINE ("currentdict end /Private exch readonly def");
}
