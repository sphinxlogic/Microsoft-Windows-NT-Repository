/* out-chars.c: try to extract the real characters from the image.

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

#include "bb-outline.h"
#include "gf.h"
#include "report.h"

#include "bitmap2.h"
#include "extract.h"
#include "ifi.h"
#include "image-header.h"
#include "main.h"
#include "out-strips.h"


/* A list of the image rows on which the baselines occur.  The end of
   the list is marked with an element -1.  (-baselines)  */
int *baseline_list = NULL;

/* Find about this many characters in the image, then stop.  This is
   useful only for testing, because converting the entire image takes a long
   time.  So we might in truth output a few more characters than
   specified here.  (-nchars)  */
unsigned nchars_wanted = MAX_CHARCODE + 1;

/* If set, prints diagnostics about which boxes are and aren't cleaned. 
   (-print-clean-info)  */ 
boolean print_clean_info = false;

/* Says whether to print the row numbers of each character as we go, 
   so that the user can decide how to adjust jumping baselines. 
   (-print-guidelines)  */
boolean print_guidelines = false;

/* Says which characters to output.  This is independent of the
   ordering in the font file.  (-range) */
int starting_char = 0;
int ending_char = MAX_CHARCODE;

/* The design size of the font we're creating.  */
static real design_size;

/* Where the baseline of the current row is in the image.  The first row
   is #1, and so on.  We start over at row 1 at each image row.*/
static unsigned row_baseline;
static unsigned row_height;

/* How many boxes total the characters take up.  */
static unsigned total_boxes_expected;


static boolean bb_equal_p (bounding_box_type, bounding_box_type);
static gf_char_type bitmap_to_gf_char
  (bitmap_type, real, bounding_box_type, image_char_type);
static void clean_bitmap (bitmap_type *, bb_list_type);
static boolean do_image_line
  (bitmap_type, unsigned *, unsigned *, real, image_char_list_type);
static void image_to_bitmap_bbs (bb_list_type *);
extern void offset_bb_list (bb_list_type *l, int offset);
static unsigned output_chars
  (bb_list_type, bitmap_type, real, image_char_list_type, unsigned);

/* Analyze the input image, outputting the characters we find.  */

void
write_image_chars (image_header_type image_header, real ds)
{
  bitmap_type *image_bitmap;
  unsigned nchars_done = 0;

  /* Read the image information.  This tells us to which character
       each bounding box belongs, among other things.  */
  image_char_list_type image_char_list = read_ifi_file (&total_boxes_expected);

  /* Remember this so we don't need to pass it through all the
     intervening routines to the low-level output.  */
  design_size = ds;
  
  /* The main loop also (and more commonly) exits when we've read the
     entire image.  */ 
  while (nchars_done < nchars_wanted)
    {
      unsigned *transitions;

      /* Read one line of characters in the image.  After this,
         `image_bitmap' is, for example, `a...z', with blank columns at
         the left and right.  Whatever is in the original image.  */
      image_bitmap
        = some_black_to_all_white_row (image_header.width, &transitions);
      if (image_bitmap == NULL)
        break; /* We've read the whole image file.  */

      if (baseline_list == NULL || *baseline_list == -1)
        {
         if (baseline_list != NULL)
            WARNING ("imageto: Not enough baselines specified");
          row_baseline = 0;
        }
      else
        row_baseline = *baseline_list++;

      row_height = BITMAP_HEIGHT (*image_bitmap);

      /* If `do_image_line' fails, we're supposed to read the next
         row in the image, and put it below the current line.  For
         example, if a line has only an `!' on it, we will only get
         the stem on the first call to `some_black_to_all_white_row'.  
         We want to get the dot in there, too.  */
      while (!do_image_line (*image_bitmap, transitions, &nchars_done,
                             (real) image_header.hres, image_char_list))
        {
          bitmap_type *revised
            = append_next_image_row (*image_bitmap, image_header.width,
                                     &transitions);
          if (revised == NULL)
            {
              WARNING ("imageto: Image ended in the midst of a character");
              break;
            }

          /* Tell the user the image row didn't end on a character.  */
          REPORT ("+");

          free_bitmap (image_bitmap);
          image_bitmap = revised;
          if (baseline_list == NULL)
            row_baseline = 0;

	 row_height = BITMAP_HEIGHT (*image_bitmap);
        }

      free_bitmap (image_bitmap);
    }
}

/* Analyze and output all of the bitmap IMAGE, which is one line of type
   in the original.  The resolution of the image is H_RESOLUTION, and
   we've read NCHARS characters so far.  We use IMAGE_CHAR_LIST and the
   transition vector TRANSITIONS, which has in it how IMAGE breaks into
   characters.
   
   We return false if we need to be given another image row.  */

#define NEXT_TRANSITION()  ({						\
  if (*transitions == BITMAP_WIDTH (image) + 1)				\
    {									\
      WARNING ("imageto: Expected more transitions");			\
      break;								\
    }									\
  *transitions++;							\
})

static boolean
do_image_line (bitmap_type image, unsigned *transitions, unsigned *nchars, 
               real h_resolution, image_char_list_type image_char_list)
{
  static unsigned box_count = 0;
  bounding_box_type bb;
  
  /* We always use the entire image line vertically.  */
  MIN_ROW (bb) = 0;
  MAX_ROW (bb) = BITMAP_HEIGHT (image) - 1;
  
  /* `nchars_wanted' is an option to the program, defined at the top.  */
  while (*nchars < nchars_wanted && *transitions != BITMAP_WIDTH (image) + 1)
    {
      int bb_offset = 0;
      unsigned original_box_count = box_count;
      bb_list_type boxes = bb_list_init ();
      bitmap_type *char_bitmap = XTALLOC1 (bitmap_type);
      bitmap_type *temp_bitmap = XTALLOC1 (bitmap_type);

      /* The first element of TRANSITIONS is white-to-black.
         Thereafter, they alternate.  */
      MIN_COL (bb) = NEXT_TRANSITION ();
      MAX_COL (bb) = NEXT_TRANSITION ();
      
      /* After this, `char_bitmap' might be just an `a' (with blank
         rows above and below, because `a' has neither a descender nor
         an ascender), then `b', and so on.  In many specimens, some
         characters overlap; for example, we might get `ij' as one
         bitmap, instead of `i' and then `j'.  */
      *char_bitmap = extract_subbitmap (image, bb);
      *temp_bitmap = copy_bitmap (*char_bitmap);

      while (true)
        {
          bb_list_type temp_boxes;
          bounding_box_type previous_bb;

          /* If we've read all the bounding boxes expected, we're done.  */
          if (box_count == total_boxes_expected)
            break;

          /* If we've read more than we expected to, we're in trouble.  */
          if (box_count > total_boxes_expected)
            {
              WARNING2 ("imageto: Read box #%u but expected only %u",
                        box_count, total_boxes_expected);
              /* No point in giving this message more than once.  */
              total_boxes_expected = INT_MAX;
            }

          /* Find bounding boxes around all the shapes in `char_bitmap'.
             Continuing the `ij' example, this would result in four
             bounding boxes (one for each dot, one for the dotless `i',
             and one for the dotless `j').  */
          temp_boxes = find_outline_bbs (*temp_bitmap, false, 0, 0);

          /* The subimages we've created all start at column zero.  But
             we want to put the images side-by-side, instead of
             overlaying them.  So we change the bounding box numbers.  */
          offset_bb_list (&temp_boxes, bb_offset);

          box_count += BB_LIST_LENGTH (temp_boxes);
          bb_list_splice (&boxes, temp_boxes);

          /* If the white column was at the end of a character, exit the
             loop to output what we've got..  */
          if (box_at_char_boundary_p (image_char_list, *nchars,
                                      box_count - original_box_count))
            break;

          /* If we're at the end of the image row, tell our caller that
             we had to quit in the middle of a character.  */
	  if (*transitions == BITMAP_WIDTH (image) + 1)
            { /* Forget that we've seen this before.  */
              box_count = original_box_count;
              return false;
            }

	  previous_bb = bb;
	  MIN_COL (bb) = NEXT_TRANSITION ();
          MAX_COL (bb) = NEXT_TRANSITION ();
          
	  /* Leave in the white space between the character parts.  */
	  MIN_COL (bb) = MAX_COL (previous_bb);
          
	  free_bitmap (temp_bitmap);
          *temp_bitmap = extract_subbitmap (image, bb);
          if (temp_bitmap == NULL)
            {
              WARNING1 ("imageto: Expected more bounding boxes for `%d'",
                       IMAGE_CHARCODE (IMAGE_CHAR (image_char_list, *nchars)));
              break;
            }

          /* The boxes `temp_bitmap' should be just to the right of the
             bitmap we've accumulated in `char_bitmap'.  This happens
             above, next time through the loop.  */
          bb_offset = BITMAP_WIDTH (*char_bitmap);

	  /* When this happens, it usually means that the IFI file
             didn't specify enough bounding boxes for some character,
             and so things are out of sync.  */
          if (BITMAP_HEIGHT (*char_bitmap) != BITMAP_HEIGHT (*temp_bitmap))
            {
              WARNING ("imageto: Line ended inside a character");
              break;
            }
          bitmap_concat (char_bitmap, *temp_bitmap);
        }  /* while (true) */
        
      free_bitmap (temp_bitmap);

      /* Convert the bits inside those bounding boxes into one (if not
         overlapping with another) or more (if overlapping) characters
	 in the GF font.  */
      *nchars += output_chars (boxes, *char_bitmap, h_resolution,
                               image_char_list, *nchars);

      free_bitmap (char_bitmap);
      bb_list_free (&boxes);
    }
  
  return true;
}

/* Move all the elements in BB_LIST to the right by OFFSET.  */

void
offset_bb_list (bb_list_type *bb_list, int offset)
{
  unsigned this_bb;
  
  for (this_bb = 0; this_bb < BB_LIST_LENGTH (*bb_list); this_bb++)
    {
      bounding_box_type *bb = &BB_LIST_ELT (*bb_list, this_bb);
      MIN_COL (*bb) += offset;
      MAX_COL (*bb) += offset;
    }
}


/* Return true if BB1 and BB2 are equal.  */

static boolean
bb_equal_p (bounding_box_type bb1, bounding_box_type bb2)
{
  return
    MIN_COL (bb1) == MIN_COL (bb2)
    && MIN_ROW (bb1) == MIN_ROW (bb2)
    && MAX_COL (bb1) == MAX_COL (bb2)
    && MAX_ROW (bb1) == MAX_ROW (bb2);
}

/* For each bounding box in the list BOXES, extract from IMAGE_BITMAP
   and turn the resulting bitmap into a single character in the font. 
   The information in IMAGE_CHAR_LIST maps bounding boxes to character codes;
   consecutive bounding boxes may belong to the same character.  For
   example, `i' will appear twice, once for the dot and once for the
   stem.  We assume that all the bounding boxes for a given character
   will appear in IMAGE_BITMAP.
   
   We return the number of characters (not bounding boxes) found,
   including characters that were omitted.  */

/* Predicate to tell us if we want to actually write the character.  */
#define OUTPUT_CHAR_P(code, image_char)					\
  ((code) >= starting_char && (code) <= ending_char			\
   && !IMAGE_CHAR_OMIT (image_char))

static unsigned
output_chars (bb_list_type boxes, bitmap_type image_bitmap, 
	      real h_resolution, image_char_list_type image_char_list,
              unsigned current_char)
{  
  static unsigned char_count = 0;
  int this_box; /* Because we might have to subtract when it's zero.  */
  boolean done[BB_LIST_LENGTH (boxes)];

  /* Since we report (a lot) more information when `print_guidelines' is
     true, we can fit fewer characters per line.  */
  unsigned nchars_per_line = print_guidelines ? 1 : 11;

  unsigned nchars_written = 0;
  
  for (this_box = 0; this_box < BB_LIST_LENGTH (boxes); this_box++)
    done[this_box] = false;

  for (this_box = 0; this_box < BB_LIST_LENGTH (boxes); this_box++)
    {
      bounding_box_type bb;
      bitmap_type bitmap;
      image_char_type c;
      charcode_type charcode;
      bb_list_type bb_list = bb_list_init ();
      
      /* `done[this_box]' will be set if we get to a bounding box that
         has already been combined with a previous one, because of
         an alternating combination.  Since we never go backwards, we
         don't bother to set `done' for every box we look at.  */
      if (done[this_box])
        continue;
        
      c = IMAGE_CHAR (image_char_list, current_char++);
      charcode = IMAGE_CHARCODE (c);
      
      REPORT ("[");

      /* Only bother to collect the character image if we're going to
         output it; otherwise, it just wastes a lot of time and space.  */
      if (OUTPUT_CHAR_P (charcode, c))
        { /* A character consisting of zero bounding boxes is invisible;
             e.g., a space.  We don't want to read any of the bitmap for
             such a thing.  */
          if (IMAGE_CHAR_BB_COUNT (c) == 0)
            {
              BITMAP_HEIGHT (bitmap) = 0;
              BITMAP_WIDTH (bitmap) = 0;
              BITMAP_BITS (bitmap) = NULL;
              bb = (bounding_box_type) { 0, 0, 0, 0 };
              
              /* Since we're not eating up any bounding boxes,
                 reconsider the current one.  */
              this_box--;
            }
          else
            {
              bb = BB_LIST_ELT (boxes, this_box);
              bitmap = extract_subbitmap (image_bitmap, bb);
              bb_list_append (&bb_list, bb);
            }
        }

      while (IMAGE_CHAR_BB_COUNT (c)-- > 1)
        {
          unsigned combine_box;
          
          if (IMAGE_CHAR_BB_ALTERNATING (c))
            {
              /* Don't increment `this_box', since it is incremented at
                 the end of the loop, and the next box is part of
                 another character.  */
              combine_box = this_box + 2;
              /* Don't look at the second box again in the outer loop.  */
              done[combine_box] = true;
            }
          else
            combine_box = ++this_box;

          if (combine_box >= BB_LIST_LENGTH (boxes))
            {
              WARNING1 ("imageto: Not enough outlines for char %u",
                        (unsigned) charcode);
              break;
            }

          if (OUTPUT_CHAR_P (charcode, c))
            {
              /* Get the shape to combine with `bitmap'.  */
              bounding_box_type next_bb = BB_LIST_ELT (boxes, combine_box);
              bitmap_type next_bitmap
                = extract_subbitmap (image_bitmap, next_bb);

              bb_list_append (&bb_list, next_bb);

              combine_images (&bitmap, next_bitmap, &bb, next_bb);
              free_bitmap (&next_bitmap);
            }
        }
      
      if (OUTPUT_CHAR_P (charcode, c))
        {
          gf_char_type gf_char;
          
          if (BITMAP_BITS (bitmap) != NULL)
            clean_bitmap (&bitmap, bb_list);

          gf_char = bitmap_to_gf_char (bitmap, h_resolution, bb, c);
          gf_put_char (gf_char);

          /* This and the GF character's bitmap are the same, so we only
             need to free one of the two.  */
          if (BITMAP_BITS (bitmap) != NULL)
            free_bitmap (&bitmap);
        }
      else
        REPORT ("x"); /* We're ignoring this character.  */

      REPORT1 ("]%c", ++char_count % nchars_per_line ? ' ' : '\n');
      nchars_written++;
      
      bb_list_free (&bb_list);
    }
  
  return nchars_written;
}

/* Remove bits of adjacent characters that may have crept into B because
   of overlapping characters in the original image.  KNOWN_BOXES lists
   all the known parts of B; if we find other bounding boxes in B, we
   remove them.  */

static void
clean_bitmap (bitmap_type *b, bb_list_type known_boxes)
{
  unsigned test;
  bb_list_type test_boxes = find_outline_bbs (*b, false, BITMAP_WIDTH (*b), 0);

  if (print_clean_info)
    REPORT2 ("Cleaning %ux%u bitmap:\n",
             BITMAP_WIDTH (*b), BITMAP_HEIGHT (*b));

  /* Convert KNOWN_BOXES to the same coordinates as `test_boxes'.  */
  image_to_bitmap_bbs (&known_boxes);
  
  for (test = 0; test < BB_LIST_LENGTH (test_boxes); test++)
    {
      unsigned known;
      unsigned known_length = BB_LIST_LENGTH (known_boxes);
      bounding_box_type test_bb = BB_LIST_ELT (test_boxes, test);

      if (print_clean_info)
        REPORT4 ("  checking (%d,%d)-(%d,%d) ... ",
                 MIN_COL (test_bb), MIN_ROW (test_bb),
                 MAX_COL (test_bb), MAX_ROW (test_bb));
      
      /* If we want to keep `test_bb', it will be one of the elements of
         BB_LIST.  Otherwise, it is a piece of an adjacent character,
         and we should erase it.  */
      for (known = 0; known < known_length && !bb_equal_p (test_bb,
                                             BB_LIST_ELT (known_boxes, known));
           known++)
        ;
      
      if (known == known_length)
        {
          unsigned r;
          int test_bb_width = BB_WIDTH (test_bb);
          
          assert (test_bb_width > 0);
          
          if (print_clean_info)
            REPORT ("clearing.\n");

          for (r = MIN_ROW (test_bb); r <= MAX_ROW (test_bb); r++)
            {
              one_byte *row = BITMAP_ROW (*b, r);
              memset (row + MIN_COL (test_bb), 0, test_bb_width);
            }
        }
      else if (print_clean_info)
        REPORT ("keeping.\n");
    }
}


/* Translate the elements of BOXES to the origin, i.e., shift each down
   by the minimum row and column.  We use this in `clean_bitmap' to
   change bounding boxes in the coordinates of the entire image to the
   coordinates of the single character we are cleaning.  */

static void
image_to_bitmap_bbs (bb_list_type *boxes)
{
  unsigned b;
  unsigned min_col = UINT_MAX;
  unsigned min_row = UINT_MAX;
  
  /* First find the minimum row and column of all the bb's in BOXES.  */
  for (b = 0; b < BB_LIST_LENGTH (*boxes); b++)
    {
      bounding_box_type bb = BB_LIST_ELT (*boxes, b);
      
      assert (MIN_COL (bb) >= 0 && MIN_ROW (bb) >= 0);
      
      MIN_EQUALS (min_col, MIN_COL (bb));
      MIN_EQUALS (min_row, MIN_ROW (bb));
    }

  /* Now translate all the bb's by those minimums.  */
  for (b = 0; b < BB_LIST_LENGTH (*boxes); b++)
    {
      bounding_box_type *bb = &BB_LIST_ELT (*boxes, b);
      
      MIN_COL (*bb) -= min_col;
      MAX_COL (*bb) -= min_col;
      MIN_ROW (*bb) -= min_row;
      MAX_ROW (*bb) -= min_row;
    }
}

/* Derive the information necessary to output the font character from
   the bitmap B, and return it.  The resolution of the bitmap is given
   in pixels per inch as H_RESOLUTION.  The bounding box BB encloses the
   character in the image coordinates.  We use BB and the static
   variables `row_baseline' and `row_height' to determine the
   positioning of the GF character.  */

#define BB_TO_CARTESIAN(x) 						\
  (row_height - 1 - (x) - row_baseline 					\
   - IMAGE_CHAR_BASELINE_ADJUST (image_char))

static gf_char_type
bitmap_to_gf_char (bitmap_type b, real h_resolution,
                   bounding_box_type bb, image_char_type image_char)
{
  gf_char_type gf_char;
  real width_in_points;
  bounding_box_type cartesian_bb;  
  charcode_type charcode = IMAGE_CHARCODE (image_char);
  
  MIN_ROW (cartesian_bb) = BB_TO_CARTESIAN (MAX_ROW (bb));
  MAX_ROW (cartesian_bb) = BB_TO_CARTESIAN (MIN_ROW (bb));
  
  REPORT1 ("%u", charcode);
  
  GF_CHARCODE (gf_char) = charcode;
  GF_BITMAP (gf_char) = b;
  GF_CHAR_MIN_COL (gf_char) = IMAGE_CHAR_LSB (image_char);
  GF_CHAR_MAX_COL (gf_char) = GF_CHAR_MIN_COL (gf_char) + BITMAP_WIDTH (b);
  GF_CHAR_MIN_ROW (gf_char) = MIN_ROW (cartesian_bb);
  GF_CHAR_MAX_ROW (gf_char) = MAX_ROW (cartesian_bb);

  GF_H_ESCAPEMENT (gf_char) = (GF_CHAR_MAX_COL (gf_char)
                               + IMAGE_CHAR_RSB (image_char));

  width_in_points = GF_H_ESCAPEMENT (gf_char) * POINTS_PER_INCH / h_resolution;
  GF_TFM_WIDTH (gf_char) = real_to_fix (width_in_points / design_size);

  if (print_guidelines)
    REPORT3 (" (%s) %d/%d",  IMAGE_CHARNAME (image_char),
	     MIN_ROW (cartesian_bb), MAX_ROW (cartesian_bb));

  return gf_char;
}
