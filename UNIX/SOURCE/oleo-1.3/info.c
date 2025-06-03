/*	Copyright (C) 1993 Free Software Foundation, Inc.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this software; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include "global.h"
#include "info.h"
#include "hash.h"
#include "cmd.h"

struct hash_control * info_buffers;


#ifdef __STDC__
void
init_info (void)
#else
void
init_info ()
#endif
{
  info_buffers = hash_new();
}

#ifdef __STDC__
struct info_buffer *
find_info (char * name)
#else
struct info_buffer *
find_info (name)
     char * name;
#endif
{
  return (struct info_buffer *)hash_find (info_buffers, name);
}


#ifdef __STDC__
struct info_buffer * 
find_or_make_info (char * name)
#else
struct info_buffer * 
find_or_make_info (name)
     char * name;
#endif
{
  struct info_buffer * buf =
    (struct info_buffer *)hash_find (info_buffers, name);
  if (buf)
    return buf;
  
  buf = ((struct info_buffer *)
	 ck_malloc (sizeof (struct info_buffer) + strlen(name) + 1));
  buf->name = (char *)buf + sizeof (*buf);
  strcpy (buf->name, name);
  buf->len = 0;
  buf->text = 0;
  hash_insert (info_buffers, buf->name, buf);
  return buf;
}

#ifdef __STDC__
void
clear_info (struct info_buffer * buf)
#else
void
clear_info (buf)
     struct info_buffer * buf;
#endif
{
  if (buf->text)
    {
      int x;
      int stop = buf->len;
      for (x = 0; x < stop; ++x)
	ck_free (buf->text);
      ck_free (buf->text);
    }
  buf->text = 0;
  buf->len = 0;
}

#ifdef __STDC__
void
print_info (struct info_buffer * buf, char * format, ...)
#else
void
print_info (buf, format, va_alist)
     struct info_buffer * buf;
     char * format;
     va_dcl
#endif
{
  va_list ap;
  char txt[1000];
  int len;			/* Length of the new line */
  
  var_start (ap, format);
  vsprintf (txt, format, ap);
  va_end (ap);
  len = strlen (txt);

  ++buf->len;			/* Number of lines in the buffer */
  buf->text = (char **)ck_remalloc (buf->text, buf->len * sizeof (char *));
  buf->text[buf->len - 1] = (char *)ck_malloc (len + 1);
  bcopy (txt, buf->text[buf->len - 1], len + 1);
}




/* A generic buffer for the use informational commands like show-options */
static struct info_buffer * the_text_buf;

#ifdef __STDC__
void
io_text_start (void)
#else
void
io_text_start ()
#endif
{
  the_text_buf = find_or_make_info ("_text");
  clear_info (the_text_buf);
}



#ifdef __STDC__
void
io_text_line (char * format, ...)
#else
void
io_text_line (format, va_alist)
     char * format;
     va_dcl
#endif
{
  va_list ap;
  char txt[1000];
  int len;			/* Length of the new line */
  
  var_start (ap, format);
  vsprintf (txt, format, ap);
  va_end (ap);
  len = strlen (txt);

  ++the_text_buf->len;		/* Number of lines in the the_text_buf */
  the_text_buf->text =
    (char **)ck_remalloc (the_text_buf->text,
			  the_text_buf->len * sizeof (char *));
  the_text_buf->text[the_text_buf->len - 1] = (char *)ck_malloc (len + 1);
  bcopy (txt, the_text_buf->text[the_text_buf->len - 1], len + 1);
}

#ifdef __STDC__
void
io_text_finish (void)
#else
void
io_text_finish ()
#endif
{
  run_string_as_macro ("{view-info _text}", 1);
}

