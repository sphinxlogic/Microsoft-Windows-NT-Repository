/* curve.c: operations on the lists of pixels and lists of curves.

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

#include "logreport.h"

#include "curve.h"
#include "display.h"


/* Return an entirely empty curve.  */

curve_type
new_curve ()
{
  curve_type curve = xmalloc (sizeof (struct curve));

  curve->point_list = NULL;
  CURVE_LENGTH (curve) = 0;
  CURVE_CYCLIC (curve) = false;
  CURVE_START_TANGENT (curve) = CURVE_END_TANGENT (curve) = NULL;
  PREVIOUS_CURVE (curve) = NEXT_CURVE (curve) = NULL;

  return curve;
}


/* Start the returned curve off with COORD as the first point.  */

curve_type
init_curve (coordinate_type coord)
{
  curve_type curve = new_curve ();

  curve->point_list = xmalloc (sizeof (point_type));
  CURVE_LENGTH (curve) = 1;

  CURVE_POINT (curve, 0) = int_to_real_coord (coord);

  return curve;
}


/* Don't copy the points or tangents, but copy everything else.  */

curve_type
copy_most_of_curve (curve_type old_curve)
{
  curve_type curve = new_curve ();

  CURVE_CYCLIC (curve) = CURVE_CYCLIC (old_curve);
  PREVIOUS_CURVE (curve) = PREVIOUS_CURVE (old_curve);
  NEXT_CURVE (curve) = NEXT_CURVE (old_curve);

  return curve;
}


/* The length of CURVE will be zero if we ended up not being able to fit
   it (which in turn implies a problem elsewhere in the program, but at
   any rate, we shouldn't try here to free the nonexistent curve).  */

void
free_curve (curve_type curve)
{
  if (CURVE_LENGTH (curve) > 0)
    safe_free ((address *) &(curve->point_list));
}


void
append_pixel (curve_type curve, coordinate_type coord)
{
  append_point (curve, int_to_real_coord (coord));
}


void
append_point (curve_type curve, real_coordinate_type coord)
{
  CURVE_LENGTH (curve)++;
  XRETALLOC (curve->point_list, CURVE_LENGTH (curve), point_type);
  LAST_CURVE_POINT (curve) = coord;
  /* The t value does not need to be set.  */
}

/* Show the pixels of the curve online.  */

void
display_curve (curve_type c)
{
  unsigned this_point;

  if (!wants_display) return;
  
  for (this_point = 0; this_point < CURVE_LENGTH (c); this_point++)
    display_pixel (CURVE_POINT (c, this_point));
}


/* Print a curve in human-readable form.  It turns out we never care
   about most of the points on the curve, and so it is pointless to
   print them all out umpteen times.  What matters is that we have some
   from the end and some from the beginning.  */

#define NUM_TO_PRINT 3

#define LOG_CURVE_POINT(c, p, print_t)					\
  do									\
    {									\
      LOG2 ("(%.3f,%.3f)", CURVE_POINT (c, p).x, CURVE_POINT (c, p).y);	\
      if (print_t)							\
        LOG1 ("/%.2f", CURVE_T (c, p));					\
    }									\
  while (0)

void
log_curve (curve_type curve, boolean print_t)
{
  unsigned this_point;
  
  if (!logging) return;

  LOG1 ("curve id = %x:\n", (unsigned) curve);
  LOG1 ("  length = %u.\n", CURVE_LENGTH (curve));
  if (CURVE_CYCLIC (curve))
    LOG ("  cyclic.\n");

  /* It should suffice to check just one of the tangents for being null
     -- either they both should be, or neither should be.  */
  if (CURVE_START_TANGENT (curve) != NULL)
    LOG4 ("  tangents = (%.3f,%.3f) & (%.3f,%.3f).\n",
          CURVE_START_TANGENT (curve)->dx, CURVE_START_TANGENT (curve)->dy,
          CURVE_END_TANGENT (curve)->dx, CURVE_END_TANGENT (curve)->dy);

  LOG ("  ");
  
  /* If the curve is short enough, don't use ellipses.  */
  if (CURVE_LENGTH (curve) <= NUM_TO_PRINT * 2)
    {
      for (this_point = 0; this_point < CURVE_LENGTH (curve); this_point++)
        {
          LOG_CURVE_POINT (curve, this_point, print_t);
          LOG (" ");

          if (this_point != CURVE_LENGTH (curve) - 1
              && (this_point + 1) % NUM_TO_PRINT == 0)
            LOG ("\n  ");
        }
    }
  else
    {
      for (this_point = 0;
           this_point < NUM_TO_PRINT && this_point < CURVE_LENGTH (curve);
           this_point++)
        {
          LOG_CURVE_POINT (curve, this_point, print_t);
          LOG (" ");
        }

      LOG ("...\n   ...");

      for (this_point = CURVE_LENGTH (curve) - NUM_TO_PRINT;
           this_point < CURVE_LENGTH (curve);
           this_point++)
        {
          LOG (" ");
          LOG_CURVE_POINT (curve, this_point, print_t);
        }
    }

  LOG (".\n");
}


/* Like `log_curve', but write the whole thing.  */

void
log_entire_curve (curve_type curve)
{
  unsigned this_point;
  
  if (!logging) return;

  LOG1 ("curve id = %x:\n", (unsigned) curve);
  LOG1 ("  length = %u.\n", CURVE_LENGTH (curve));
  if (CURVE_CYCLIC (curve))
    LOG ("  cyclic.\n");

  /* It should suffice to check just one of the tangents for being null
     -- either they both should be, or neither should be.  */
  if (CURVE_START_TANGENT (curve) != NULL)
    LOG4 ("  tangents = (%.3f,%.3f) & (%.3f,%.3f).\n",
          CURVE_START_TANGENT (curve)->dx, CURVE_START_TANGENT (curve)->dy,
          CURVE_END_TANGENT (curve)->dx, CURVE_END_TANGENT (curve)->dy);

  LOG (" ");

  for (this_point = 0; this_point < CURVE_LENGTH (curve); this_point++)
    {
      LOG (" ");
      LOG_CURVE_POINT (curve, this_point, true);
    }

  LOG (".\n");
}

/* Return an initialized but empty curve list.  */

curve_list_type
new_curve_list ()
{
  curve_list_type curve_list;

  curve_list.length = 0;
  curve_list.data = NULL;

  return curve_list;
}


/* Free a curve list and all the curves it contains.  */

void
free_curve_list (curve_list_type *curve_list)
{
  unsigned this_curve;

  for (this_curve = 0; this_curve < curve_list->length; this_curve++)
    free_curve (curve_list->data[this_curve]);

  /* If the character was empty, it won't have any curves.  */
  if (curve_list->data != NULL)
    safe_free ((address *) &(curve_list->data));
}


/* Add an element to a curve list.  */

void
append_curve (curve_list_type *curve_list, curve_type curve)
{
  curve_list->length++;
  XRETALLOC (curve_list->data, curve_list->length, curve_type);
  curve_list->data[curve_list->length - 1] = curve;
}

/* Return an initialized but empty curve list array.  */

curve_list_array_type
new_curve_list_array ()
{
  curve_list_array_type curve_list_array;

  CURVE_LIST_ARRAY_LENGTH (curve_list_array) = 0;
  curve_list_array.data = NULL;

  return curve_list_array;
}


/* Free a curve list array and all the curve lists it contains.  */

void
free_curve_list_array (curve_list_array_type *curve_list_array)
{
  unsigned this_list;

  for (this_list = 0; this_list < CURVE_LIST_ARRAY_LENGTH (*curve_list_array);
       this_list++) 
    free_curve_list (&CURVE_LIST_ARRAY_ELT (*curve_list_array, this_list));

  /* If the character was empty, it won't have any curves.  */
  if (curve_list_array->data != NULL)
    safe_free ((address *) &(curve_list_array->data));
}


/* Add an element to a curve list array.  */

void
append_curve_list (curve_list_array_type *l, curve_list_type curve_list)
{
  CURVE_LIST_ARRAY_LENGTH (*l)++;
  XRETALLOC (l->data, CURVE_LIST_ARRAY_LENGTH (*l), curve_list_type);
  LAST_CURVE_LIST_ARRAY_ELT (*l) = curve_list;
}
