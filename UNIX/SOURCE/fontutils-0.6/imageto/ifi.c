/* ifi.c: read an IFI file.  */

#include "config.h"

#include "encoding.h"
#include "libfile.h"

#include "ifi.h"
#include "main.h"


/* The name of the encoding file specified by the user, and the
   structure we parse it into.  (-encoding)  */
string encoding_name = NULL;

/* The name of the IFI file.  (-ifi-file)  */
string ifi_filename = NULL;


static void append_image_char (image_char_list_type *l, image_char_type c);
static image_char_list_type new_image_char_list (void);

/* Read the IFI file `ifi_filename' (or construct the IFI filename from
   `input_name'), which gives us information about the image we are
   going to read.
   
   See the documentation for full details and examples of IFI files.
   Here we just sketch the bare bones.
   
   Each (non-blank, non-comment) line in this file represents a
   ``character'' in the image, and has one to five entries, separated by
   spaces and/or tabs.  Comments start with `%' and continue to the end
   of the line.
   
   The first entry represents the character code.  See the documentation
   for the various forms of charcodes.  We also allow `.notdef', meaning
   the ``character'' should not be output in the font.  
   
   The second entry, if it exists, is an adjustment to the
   baseline.  We move the baseline down for negative numbers, and up
   for positive ones.  Default is zero.
   
   The third entry, if it exists, is the number of bounding boxes that
   comprise this character.  If it is negative, the bounding boxes
   are not consecutive in the image; instead, they alternate with
   bounding boxes for some other character.
   
   The fourth and fifth entries, if they exist, are the left and right
   side bearings, respectively.  */
   
image_char_list_type
read_ifi_file (unsigned *total_count)
{
  string line;
  int bb_count;
  encoding_info_type *encoding_info;
  int n = 0;
  image_char_list_type image_char_list = new_image_char_list ();
  
  if (encoding_name)
    {
      encoding_info = XTALLOC1 (encoding_info_type);
      *encoding_info = read_encoding_file (encoding_name);
    }
  else  
    encoding_info = NULL;
    
  if (!ifi_filename)
    ifi_filename = make_suffix (input_name, "ifi");

  libfile_start (ifi_filename, "ifi");

  *total_count = 0;

  while ((line = libfile_line ()) != NULL)
    {
      image_char_type c;
      string baseline_adjust_str, bb_count_str, charname;
      string lsb_str, rsb_str;
      string save_line = line; /* So we can free it.  */
      
      /* The character name.  */
      charname = strtok (line, " \t");
      
      /* `libfile_line' should never return an empty line.  */
      assert (charname != NULL);
      
      if (STREQ (charname, ".notdef"))
        IMAGE_CHAR_OMIT (c) = true;
      else
        {
          if (encoding_info)
            n = encoding_number (*encoding_info, charname);
          else
            n++;
          
          if (n == -1)
            {
              LIBFILE_WARN2 ("Character `%s' undefined in encoding %s",
                        charname, ENCODING_SCHEME_NAME (*encoding_info));
              IMAGE_CHAR_OMIT (c) = true;
	    }
          else
            {
    	      IMAGE_CHARCODE (c) = n;
              IMAGE_CHARNAME (c) = xstrdup (charname);
              IMAGE_CHAR_OMIT (c) = false;
	    }
        }

      /* The baseline adjustment.  */
      baseline_adjust_str = strtok (NULL, " \t");
      IMAGE_CHAR_BASELINE_ADJUST (c)
        = baseline_adjust_str == NULL ? 0 : atoi (baseline_adjust_str);
      
      /* The bounding box count.  */
      bb_count_str = strtok (NULL, " \t");
      bb_count = bb_count_str == NULL ? 1 : atoi (bb_count_str);
      
      if (bb_count < 0)
        {
          IMAGE_CHAR_BB_COUNT (c) = -bb_count;
          IMAGE_CHAR_BB_ALTERNATING (c) = true;
        }
      else
        {
          IMAGE_CHAR_BB_COUNT (c) = bb_count;
          IMAGE_CHAR_BB_ALTERNATING (c) = false;
        }

      /* The left side bearing.  */
      lsb_str = strtok (NULL, " \t");
      IMAGE_CHAR_LSB (c) = lsb_str == NULL ? 0 : atoi (lsb_str);
      
      /* The right side bearing.  */
      rsb_str = strtok (NULL, " \t");
      IMAGE_CHAR_RSB (c) = rsb_str == NULL ? 0 : atoi (rsb_str);
      
      *total_count += IMAGE_CHAR_BB_COUNT (c);
      append_image_char (&image_char_list, c);
      free (save_line);
    }
  
  libfile_close ();
  if (encoding_info)
    free (encoding_info);
  
  return image_char_list;
}

/* Return an initialized, empty list.  */

static image_char_list_type
new_image_char_list ()
{
  image_char_list_type l;
  
  IMAGE_CHAR_LIST_LENGTH (l) = 0;
  IMAGE_CHAR_LIST_DATA (l) = NULL;
  
  return l;
}


/* Append the character C to the list L.  */

void
append_image_char (image_char_list_type *l, image_char_type c)
{
  IMAGE_CHAR_LIST_LENGTH (*l)++;
  IMAGE_CHAR_LIST_DATA (*l)
    = xrealloc (IMAGE_CHAR_LIST_DATA (*l),
                  IMAGE_CHAR_LIST_LENGTH (*l) * sizeof (image_char_type));
  IMAGE_CHAR (*l, IMAGE_CHAR_LIST_LENGTH (*l) - 1) = c;
}


/* Return false if the box BOX_COUNT boxes beyond FIRST_CHAR in LIST
   is in the middle of a character, true otherwise.  To do this, we
   add up all the box counts for characters starting at FIRST_CHAR,
   until we see where we land.  */

boolean
box_at_char_boundary_p (image_char_list_type list, unsigned first_char,
                        unsigned box_count)
{
  unsigned count = 0;
  
  while (count < box_count && first_char < IMAGE_CHAR_LIST_LENGTH (list))
    {
      image_char_type c = IMAGE_CHAR (list, first_char);
      count += IMAGE_CHAR_BB_COUNT (c);
      first_char++;
    }
    
  return count == box_count;
}
