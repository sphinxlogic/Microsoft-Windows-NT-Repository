/* char.c: manipulate a TFM and BZR character simultaneously.

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
#include "spline.h"
#include "tfm.h"

#include "char.h"

#include "ccc.h"


/* Angle from the vertical by which to slant the shapes, in radians.
   (-oblique-angle)  */
real oblique_angle = 0.0;


static void scale_splines (spline_list_array_type *shape, real scale_factor);
static real_coordinate_type scale_point (real_coordinate_type, real);
static real_coordinate_type slant_point (real_coordinate_type);
static unsigned update_subrs
  (char_type *[], charcode_type, ccc_type *[], bzr_char_type *[]);

/* Scale the information in C by SCALE_FACTOR.  */

void
scale_char (bzr_char_type *c, real scale_factor)
{
  if (scale_factor == 1)
    return;

  CHAR_SET_WIDTH (*c) *= scale_factor;
  CHAR_MIN_COL(*c) *= scale_factor;
  CHAR_MAX_COL(*c) *= scale_factor;
  CHAR_MIN_ROW(*c) *= scale_factor;
  CHAR_MAX_ROW(*c) *= scale_factor;

  scale_splines (&BZR_SHAPE (*c), scale_factor);
}


/* Scale all the coordinates in each spline in the list.  */

static void
scale_splines (spline_list_array_type *shape, real scale_factor)
{
  unsigned this_list;

  for (this_list = 0; this_list < SPLINE_LIST_ARRAY_LENGTH (*shape);
       this_list++)
    {
      unsigned this_spline;
      spline_list_type list = SPLINE_LIST_ARRAY_ELT (*shape, this_list);

      for (this_spline = 0; this_spline < SPLINE_LIST_LENGTH (list);
           this_spline++)
        {
          spline_type s = SPLINE_LIST_ELT (list, this_spline);

          START_POINT (s) = scale_point (START_POINT (s), scale_factor);
          END_POINT (s) = scale_point (END_POINT (s), scale_factor);
          
          if (SPLINE_DEGREE (s) == CUBIC)
            {
              CONTROL1 (s) = scale_point (CONTROL1 (s), scale_factor);
              CONTROL2 (s) = scale_point (CONTROL2 (s), scale_factor);
            }
	
	  SPLINE_LIST_ELT (list, this_spline) = s;
        }
    }
}


/* Scale the single point P.  */

static real_coordinate_type
scale_point (real_coordinate_type p, real scale_factor)
{
  real_coordinate_type scaled_p;
  
  scaled_p.y = p.y * scale_factor;
  scaled_p.x = p.x * scale_factor;
  
  return scaled_p;
}

/* Merge CCC_CHARS and BZR_CHARS between STARTING_CHAR and ENDING_CHAR,
   with the latter taking precedence.  For each element in CCC_CHARS we use
   also incorporate any other characters it needs.  Return the resulting
   array of structures, and the number of subroutines found in SUBR_COUNT.  */
   
char_type **
subr_chars (bzr_char_type *bzr_chars[], ccc_type *ccc_chars[],
            charcode_type starting_char, charcode_type ending_char,
            unsigned *subr_count)
{
  unsigned code;
  char_type **subrs = XTALLOC (MAX_CHARCODE + 1, char_type *);
  
  *subr_count = 0;
  for (code = 0; code <= MAX_CHARCODE; code++)
    subrs[code] = NULL;

  for (code = starting_char; code <= ending_char; code++)
    {
      if (ccc_chars[code])
        *subr_count += update_subrs (subrs, code, ccc_chars, bzr_chars);
    }

  return subrs;
}


/* Update in SUBRS all the characters in CCC_CHARS and BZR_CHARS that
   are used as subroutines in CCC_CHARS[CODE].  Prefer entries in
   BZR_CHARS to ones in CCC_CHARS, if both exist.  We assume
   CCC_CHARS[CODE] is not null.  */

static unsigned
update_subrs (char_type *subrs[], charcode_type code,
              ccc_type *ccc_chars[], bzr_char_type *bzr_chars[])
{
  unsigned this_cmd;
  ccc_type c = *ccc_chars[code];
  unsigned subr_count = 0;

  for (this_cmd = 0; this_cmd < LIST_SIZE (c); this_cmd++)
    { 
      ccc_cmd_type cmd = *(ccc_cmd_type *) LIST_ELT (c, this_cmd);
      
      switch (CCC_OPCODE (cmd))
        {
        case TR_SETCHAR:
        case TR_SETCHARBB:
          {
            char_type *new_char = XTALLOC1 (char_type);
            charcode_type subr_code = CCC_CHARCODE (cmd);
            
            /* If we already needed this character, keep going.  */
            if (subrs[subr_code])
              continue;
            
            /* Prefer the BZR definition to the CCC if we have both.  */
            if (bzr_chars[subr_code])
              {
                CHAR_CLASS (*new_char) = bzr_char_class;
                CHAR_BZR (*new_char) = bzr_chars[subr_code];
                subrs[subr_code] = new_char;
                subr_count++;
              }
            else if (ccc_chars[subr_code])
              {
                CHAR_CLASS (*new_char) = ccc_char_class;
                CHAR_CCC (*new_char) = ccc_chars[subr_code];
                subrs[subr_code] = new_char;
                subr_count++;
                
                /* This CCC char might need more subroutines, so recurse.  */
                subr_count
                  += update_subrs (subrs, subr_code, ccc_chars, bzr_chars);
              }
            else
              WARNING2 ("Char %d used by CCC char %d, but undefined",
                        subr_code, code);
          }
          break;

        case TR_HMOVE:
        case TR_VMOVE:
          break;

        default:
          FATAL2 ("Bad CCC command %d in char %d", CCC_OPCODE (cmd), code);
	}
    }
  
  return subr_count;
}

/* Transform all the coordinates in each spline in the list.  */

spline_list_array_type
oblique_splines (spline_list_array_type shape)
{
  unsigned this_list;
  spline_list_array_type answer = new_spline_list_array ();

  if (oblique_angle == 0.0) return shape;
  
  for (this_list = 0; this_list < SPLINE_LIST_ARRAY_LENGTH (shape);
       this_list++)
    {
      unsigned this_spline;
      spline_list_type list = SPLINE_LIST_ARRAY_ELT (shape, this_list);
      spline_list_type *new = new_spline_list ();

      for (this_spline = 0; this_spline < SPLINE_LIST_LENGTH (list);
           this_spline++)
        {
          spline_type s = SPLINE_LIST_ELT (list, this_spline);

          START_POINT (s) = slant_point (START_POINT (s));
          END_POINT (s) = slant_point (END_POINT (s));
          
          if (SPLINE_DEGREE (s) == CUBIC)
            {
              CONTROL1 (s) = slant_point (CONTROL1 (s));
              CONTROL2 (s) = slant_point (CONTROL2 (s));
            }

          append_spline (new, s);
        }
      
      append_spline_list (&answer, *new);
    }
  free_spline_list_array (&shape);

  return answer;
}


/* Slant the single point P.  This is just trigonometry.  */

static real_coordinate_type
slant_point (real_coordinate_type p)
{
  real_coordinate_type slanted_p;
  
  slanted_p.y = p.y;
  slanted_p.x = p.x + tan (oblique_angle) * p.y;
  
  return slanted_p;
}
