/* wc - print the number of bytes, words, and lines in files
   Copyright (C) 1985, 1991 Free Software Foundation, Inc.

   This part of vgp.

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
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
   02111-1307, USA.  */

/* Written by Paul Rubin, phr@ocf.berkeley.edu
   and David MacKenzie, djm@gnu.ai.mit.edu. */

/** This is a routine to count the number of linefeeds (hence lines) in **/
/**  a file.  This code was taken from the GNU textutils wc.c    **/

/** modified into a routine by Rick Niles. ***/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

/* Size of atomic reads. */
#define BUFFER_SIZE (16 * 1024)

int
wc (char *file, unsigned long *lines,
    unsigned long *words, unsigned long *chars)
{
  char buf[BUFFER_SIZE];
  int bytes_read;
  int in_word = 0;

  int fd = open (file, O_RDONLY);
  if (fd == -1)
    {
      printf ("Error opening file %s\n", file);
      exit (1);
    }

  while ((bytes_read = read (fd, buf, BUFFER_SIZE)) > 0)
    {
      register char *p = buf;

      *chars += bytes_read;
      do
	{
	  switch (*p++)
	    {
	    case '\n':
	      (*lines)++;
	      /* Fall through. */
	    case '\r':
	    case '\f':
	    case '\t':
	    case '\v':
	    case ' ':
	      if (in_word)
		{
		  in_word = 0;
		  (*words)++;
		}
	      break;
	    default:
	      in_word = 1;
	      break;
	    }
	}
      while (--bytes_read);
    }

  if (bytes_read < 0)
    {
      printf ("Error reading file %s\n", file);
      exit (1);
    }
  if (in_word)
    (*words)++;


  if (close (fd))
    {
      printf ("Error closing file %s\n", file);
      exit (1);
    }

  return 0;
}
