/* VMS replacement for output_1_line */
/*
 * Copyright © 1994, 2000 the Free Software Foundation, Inc.
 *
 * Author: Richard Levitte <levitte@e.kth.se>
 *
 * This file is a part of GNU VMSLIB, the GNU library for porting GNU
 * software to VMS.
 *
 * GNU VMSLIB is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * GNU VMSLIB is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU VMSLIB; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * Revision history:
 *
 * 24-jan-2000	Craig A. Berry <craig.berry@metamorgs.com>
 *		- moved this out of util.c to keep GNU sources cleaner
 *		- removed ifdefs since this is all VMS-specific here
 *		- added comments
 *
 * We have to replace this routine from util.c because pipes get opened 
 * as VFC-format files and putc() ends up creating one record per character.
 * Note that args 2 and 3 to fwrite() have to be backwards to write one chunk
 * of the correct size rather than many chunks, which would also create
 * many records in a record-oriented file.
 * 
 * This can't be in vmsutil.c because it references the external symbols
 * outfile and tab_expand_flag, which are defined in diff.c but not the 
 * other programs.
 * 
 */

#include <stdio.h>
#include <stdlib.h>

#include "diff.h"
#include "config.h"

/* Output a line from TEXT up to LIMIT.  Without -t, output verbatim.
   With -t, expand white space characters to spaces, and if FLAG_FORMAT
   is nonzero, output it with argument LINE_FLAG after every
   internal carriage return, so that tab stops continue to line up.  */

void
vms_output_1_line (text, limit, flag_format, line_flag)
     char const *text, *limit, *flag_format, *line_flag;
{

  if (!tab_expand_flag)
    fwrite (text, limit - text, sizeof (char), outfile);
  else
    {
      register FILE *out = outfile;
      register unsigned char c;
      register char const *t = text;
      register unsigned column = 0;
      char *buf, *b;
      int tot_len = 0, flag_len = 0;

      {
	char test[50];
	sprintf (test, flag_format, line_flag);
	flag_len = strlen (test);
      }
/*
 * First make one pass to see how big a buffer to allocate.
 */
      while (t < limit)
	switch (c = *t++)
	  {
	  case '\t':
	    {
	      unsigned spaces = TAB_WIDTH - column % TAB_WIDTH;
	      column += spaces;
	      tot_len += spaces;
	    }
	    break;

	  case '\r':
	    tot_len++;
	    if (flag_format && t < limit && *t != '\n')
	      tot_len += flag_len;
	    column = 0;
	    break;

	  case '\b':
	    if (column == 0)
	      continue;
	    column--;
	    tot_len++;
	    break;

	  default:
	    if (ISPRINT (c))
	      column++;
	    tot_len++;
	    break;
	  }
      buf = malloc (tot_len + 10);
      b = buf;
/*
 * And now fill the buffer and write it out.
 */
      while (t < limit)
	switch ((c = *t++))
	  {
	  case '\t':
	    {
	      unsigned spaces = TAB_WIDTH - column % TAB_WIDTH;
	      column += spaces;
	      do
		*b = ' ';
	      while (--spaces);
	    }
	    break;

	  case '\r':
	    *b = c;
	    if (flag_format && t < limit && *t != '\n')
	      fprintf (out, flag_format, line_flag);
	    column = 0;
	    break;

	  case '\b':
	    if (column == 0)
	      continue;
	    column--;
	    *b = c;
	    break;

	  default:
	    if (ISPRINT (c))
	      column++;
	    *b = c;
	    break;
	  }
      fwrite (buf, tot_len, sizeof (char), outfile);
      free (buf);
    }
}

