#include <stdio.h>
#include "global.h"
#include "line.h"

#ifdef __STDC__
void
set_line (struct line *line, char *string)
#else
void
set_line (line, string)
     struct line *line;
     char *string;
#endif
{
  int len;

  len = strlen (string);
  if (line->alloc <= len)
    {
      if (len < LINE_MIN)
	len = LINE_MIN;
      else
	len++;
      line->alloc = len + 1;
      if (line->buf)
	line->buf = ck_realloc (line->buf, line->alloc);
      else
	line->buf = ck_malloc (line->alloc);
    }
  strcpy (line->buf, string);
}

#ifdef __STDC__
void
setn_line (struct line *line, char *string, int n)
#else
void
setn_line (line, string, n)
     struct line *line;
     char *string;
     int n;
#endif
{
  int len = n;
  if (line->alloc <= len)
    {
      if (len < LINE_MIN)
	len = LINE_MIN;
      else
	len++;
      line->alloc = len;
      line->buf = ck_remalloc (line->buf, line->alloc);
    }
  strcpy (line->buf, string);
}

#define Max(A,B)  ((A) > (B) ? (A) : (B))

#ifdef __STDC__
void
catn_line (struct line *line, char *string, int n)
#else
void
catn_line (line, string, n)
     struct line *line;
     char *string;
     int n;
#endif
{
  int len = (line->buf ? strlen (line->buf) : 0);
  if (line->alloc <= len + n + 1)
    {
      line->alloc = Max (len + n + 1, LINE_MIN);
      line->buf = ck_remalloc (line->buf, line->alloc);
    }
  bcopy (string, line->buf + len, n);
  line->buf[len + n] = '\0';
}


#ifdef __STDC__
void
sprint_line (struct line *line, char * fmt, ...)
#else
void
sprint_line (line, fmt, va_alist)
     struct line *line;
     char *fmt;
     va_dcl
#endif
{
  va_list iggy;
  int len;

  len = strlen (fmt) + 200;
  if (!line->alloc)
    {
      line->buf = ck_malloc (len);
      line->alloc = len;
    }
  else if (line->alloc < len)
    {
      line->buf = ck_realloc (line->buf, len);
      line->alloc = len;
    }
  var_start (iggy, fmt);
  vsprintf (line->buf, fmt, iggy);
  va_end (iggy);
}

#ifdef __STDC__
void
splicen_line (struct line * line, char * str, int n, int pos)
#else
void
splicen_line (line, str, n, pos)
     struct line * line;
     char * str;
     int n;
     int pos;
#endif
{
  int old_len = strlen (line->buf);
  int len = old_len + n;
  if (line->alloc <= len)
    {
      line->alloc = len;
      line->buf = ck_remalloc (line->buf, len + 1);
    }
  line->buf[len--] = '\0';
  --old_len;
  while (old_len >= pos)
    {
      line->buf[len] = line->buf[old_len];
      --len;
      --old_len;
    }
  while (n--)
    line->buf[pos + n] = str[n];
}

#ifdef __STDC__
void
edit_line (struct line * line, int begin, int len)
#else
void
edit_line (line, begin, len)
     struct line * line;
     int begin;
     int len;
#endif
{
  int old_len = strlen (line->buf);
  int new_len = old_len - len;
  while (begin < new_len)
    {
      line->buf[begin] = line->buf[begin + len];
      ++begin;
    }
  line->buf[begin] = '\0';
}


#ifdef __STDC__
void
free_line (struct line * line)
#else
void
free_line (line)
     struct line * line;
#endif
{
  if (line->buf && line->alloc)
    free (line->buf);
  line->buf = 0;
  line->alloc = 0;
}




#ifdef __STDC__
int
read_line (struct line * line, FILE * fp, int * linec)
#else
int
read_line (line, fp, linec)
     struct line * line;
     FILE * fp;
     int * linec;
#endif
{
  int pos = 0;
  int c = getc (fp);

  while ((c != EOF) && (c != '\n'))
    {
      if (pos + 2 >= line->alloc)
	{
	  line->alloc = (line->alloc ? line->alloc * 2 : 1);
	  line->buf = ck_remalloc (line->buf, line->alloc);
	}
      if (c != '\\')
	line->buf[pos++] = c;
      else
	{
	  int next_c = getc (fp);
	  if (next_c != '\n')
	    {
	      line->buf[pos++] = '\\';
	      line->buf[pos++] = next_c;
	    }
	  /* Else the backslash and newline are discarded from the input. */
	  else
	    ++*linec;
	}
      c = getc (fp);
    }
  if (pos + 1 > line->alloc)
    {
      ++line->alloc;
      line->buf = ck_remalloc (line->buf, line->alloc);
    }
  line->buf[pos] = 0;
  if (line->buf[0] || (c != EOF))
    {
      ++*linec;
      return 1;
    }
  else
    return 0;
}
