#include <lib$routines.h>
#include <ssdef.h>
/* File IO for GNU Emacs.
   Copyright (C) 1985, 1986, 1987, 1988, 1993, 1994 Free Software Foundation, Inc.

This file is part of GNU Emacs.

GNU Emacs is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU Emacs is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Emacs; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include <sys/types.h>
#include <sys/stat.h>

#include <config.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#if !defined (S_ISLNK) && defined (S_IFLNK)
#  define S_ISLNK(m) (((m) & S_IFMT) == S_IFLNK)
#endif

#if !defined (S_ISREG) && defined (S_IFREG)
#  define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#endif

#ifdef VMS
#include "vms-pwd.h"
#else
#include <pwd.h>
#endif

#ifdef MSDOS
#include "msdos.h"
#include <sys/param.h>
#endif

#include <ctype.h>

#ifdef VMS
#include "vmsdir.h"
#include <perror.h>
#include <stddef.h>
#include <string.h>
#endif

#include <errno.h>

#ifndef vax11c
extern int errno;
#endif

#ifdef VMS
#define err_str(a) ((a != EVMSERR) ? strerror(a) : strerror(a,vaxc$errno))
#else
extern char *strerror ();
#define err_str(a) (strerror(a))
#endif

#ifdef APOLLO
#include <sys/time.h>
#endif

#ifndef USG
#ifndef VMS
#ifndef BSD4_1
#define HAVE_FSYNC
#endif
#endif
#endif

#include "lisp.h"
#include "intervals.h"
#include "buffer.h"
#include "window.h"

#ifdef VMS
#include <file.h>
#include <rmsdef.h>
#include <fab.h>
#include <nam.h>
#endif

#include "systime.h"

#ifdef HPUX
#include <netio.h>
#ifndef HPUX8
#ifndef HPUX9
#include <errnet.h>
#endif
#endif
#endif

#ifndef O_WRONLY
#define O_WRONLY 1
#endif

#ifndef O_RDONLY
#define O_RDONLY 0
#endif

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

/* Nonzero during writing of auto-save files */
int auto_saving;

/* Set by auto_save_1 to mode of original file so Fwrite_region will create
   a new file with the same mode as the original */
int auto_save_mode_bits;

/* Alist of elements (REGEXP . HANDLER) for file names 
   whose I/O is done with a special handler.  */
Lisp_Object Vfile_name_handler_alist;

/* Functions to be called to process text properties in inserted file.  */
Lisp_Object Vafter_insert_file_functions;

/* Functions to be called to create text property annotations for file.  */
#ifdef VMS			/* On VMS, this names is too long */
#define Vwrite_region_annotate_functions	Vwrt_region_annotate_functions
#endif
Lisp_Object Vwrite_region_annotate_functions;

/* During build_annotations, each time an annotation function is called,
   this holds the annotations made by the previous functions.  */
#ifdef VMS			/* On VMS, this names is too long */
#define Vwrite_region_annotations_so_far	Vwrt_region_annotations_so_far
#endif
Lisp_Object Vwrite_region_annotations_so_far;

/* File name in which we write a list of all our auto save files.  */
Lisp_Object Vauto_save_list_file_name;

/* Nonzero means, when reading a filename in the minibuffer,
 start out by inserting the default directory into the minibuffer. */
int insert_default_directory;

/* On VMS, nonzero means write new files with record format stmlf.
   Zero means use var format.  */
int vms_stmlf_recfm;

/* These variables describe handlers that have "already" had a chance
   to handle the current operation.

   Vinhibit_file_name_handlers is a list of file name handlers.
   Vinhibit_file_name_operation is the operation being handled.
   If we try to handle that operation, we ignore those handlers.  */

static Lisp_Object Vinhibit_file_name_handlers;
static Lisp_Object Vinhibit_file_name_operation;

Lisp_Object Qfile_error, Qfile_already_exists;

Lisp_Object Qfile_name_history;

Lisp_Object Qcar_less_than_car;

report_file_error (string, data)
     char *string;
     Lisp_Object data;
{
  Lisp_Object errstring;

  errstring = build_string (err_str (errno));

  /* System error messages are capitalized.  Downcase the initial
     unless it is followed by a slash.  */
  if (XSTRING (errstring)->data[1] != '/')
    XSTRING (errstring)->data[0] = DOWNCASE (XSTRING (errstring)->data[0]);

  while (1)
    Fsignal (Qfile_error,
	     Fcons (build_string (string), Fcons (errstring, data)));
}

close_file_unwind (fd)
     Lisp_Object fd;
{
  close (XFASTINT (fd));
}

/* Restore point, having saved it as a marker.  */

restore_point_unwind (location)
     Lisp_Object location; 
{
  SET_PT (marker_position (location));
  Fset_marker (location, Qnil, Qnil);
}

Lisp_Object Qexpand_file_name;
Lisp_Object Qdirectory_file_name;
Lisp_Object Qfile_name_directory;
Lisp_Object Qfile_name_nondirectory;
Lisp_Object Qunhandled_file_name_directory;
Lisp_Object Qfile_name_as_directory;
Lisp_Object Qcopy_file;
Lisp_Object Qmake_directory_internal;
Lisp_Object Qdelete_directory;
Lisp_Object Qdelete_file;
Lisp_Object Qrename_file;
Lisp_Object Qadd_name_to_file;
Lisp_Object Qmake_symbolic_link;
Lisp_Object Qfile_exists_p;
Lisp_Object Qfile_executable_p;
Lisp_Object Qfile_readable_p;
Lisp_Object Qfile_symlink_p;
Lisp_Object Qfile_writable_p;
Lisp_Object Qfile_directory_p;
Lisp_Object Qfile_accessible_directory_p;
Lisp_Object Qfile_modes;
Lisp_Object Qset_file_modes;
Lisp_Object Qfile_newer_than_file_p;
Lisp_Object Qinsert_file_contents;
Lisp_Object Qwrite_region;
Lisp_Object Qverify_visited_file_modtime;
Lisp_Object Qset_visited_file_modtime;

DEFUN ("find-file-name-handler", Ffind_file_name_handler, Sfind_file_name_handler, 2, 2, 0,
  "Return FILENAME's handler function for OPERATION, if it has one.\n\
Otherwise, return nil.\n\
A file name is handled if one of the regular expressions in\n\
`file-name-handler-alist' matches it.\n\n\
If OPERATION equals `inhibit-file-name-operation', then we ignore\n\
any handlers that are members of `inhibit-file-name-handlers',\n\
but we still do run any other handlers.  This lets handlers\n\
use the standard functions without calling themselves recursively.")
  (filename, operation)
    Lisp_Object filename, operation;
{
  /* This function must not munge the match data.  */
  Lisp_Object chain, inhibited_handlers;

  CHECK_STRING (filename, 0);

  if (EQ (operation, Vinhibit_file_name_operation))
    inhibited_handlers = Vinhibit_file_name_handlers;
  else
    inhibited_handlers = Qnil;

  for (chain = Vfile_name_handler_alist; XTYPE (chain) == Lisp_Cons;
       chain = XCONS (chain)->cdr)
    {
      Lisp_Object elt;
      elt = XCONS (chain)->car;
      if (XTYPE (elt) == Lisp_Cons)
	{
	  Lisp_Object string;
	  string = XCONS (elt)->car;
	  if (XTYPE (string) == Lisp_String
	      && fast_string_match (string, filename) >= 0)
	    {
	      Lisp_Object handler, tem;

	      handler = XCONS (elt)->cdr;
	      tem = Fmemq (handler, inhibited_handlers);
	      if (NILP (tem))
		return handler;
	    }
	}

      QUIT;
    }
  return Qnil;
}

DEFUN ("file-name-directory", Ffile_name_directory, Sfile_name_directory,
  1, 1, 0,
  "Return the directory component in file name NAME.\n\
Return nil if NAME does not include a directory.\n\
Otherwise return a directory spec.\n\
Given a Unix syntax file name, returns a string ending in slash;\n\
on VMS, perhaps instead a string ending in `:', `]' or `>'.")
  (file)
     Lisp_Object file;
{
  register unsigned char *beg;
  register unsigned char *p;
  Lisp_Object handler;

  CHECK_STRING (file, 0);

  /* If the file name has special constructs in it,
     call the corresponding file handler.  */
  handler = Ffind_file_name_handler (file, Qfile_name_directory);
  if (!NILP (handler))
    return call2 (handler, Qfile_name_directory, file);

#ifdef FILE_SYSTEM_CASE
  file = FILE_SYSTEM_CASE (file);
#endif
  beg = XSTRING (file)->data;
  p = beg + XSTRING (file)->size;

  while (p != beg && p[-1] != '/'
#ifdef VMS
	 && p[-1] != ':' && p[-1] != ']' && p[-1] != '>'
#endif /* VMS */
#ifdef MSDOS
	 && p[-1] != ':' && p[-1] != '\\'
#endif
	 ) p--;

  if (p == beg)
    return Qnil;
#ifdef MSDOS
  /* Expansion of "c:" to drive and default directory.  */
  if (p == beg + 2 && beg[1] == ':')
    {
      int drive = (*beg) - 'a';
      /* MAXPATHLEN+1 is guaranteed to be enough space for getdefdir.  */
      unsigned char *res = alloca (MAXPATHLEN + 5);
      if (getdefdir (drive + 1, res + 2)) 
	{
	  res[0] = drive + 'a';
	  res[1] = ':';
	  if (res[strlen (res) - 1] != '/')
	    strcat (res, "/");
	  beg = res;
	  p = beg + strlen (beg);
	}
    }
#endif
  return make_string (beg, p - beg);
}

DEFUN ("file-name-nondirectory", Ffile_name_nondirectory, Sfile_name_nondirectory,
  1, 1, 0,
  "Return file name NAME sans its directory.\n\
For example, in a Unix-syntax file name,\n\
this is everything after the last slash,\n\
or the entire name if it contains no slash.")
  (file)
     Lisp_Object file;
{
  register unsigned char *beg, *p, *end;
  Lisp_Object handler;

  CHECK_STRING (file, 0);

  /* If the file name has special constructs in it,
     call the corresponding file handler.  */
  handler = Ffind_file_name_handler (file, Qfile_name_nondirectory);
  if (!NILP (handler))
    return call2 (handler, Qfile_name_nondirectory, file);

  beg = XSTRING (file)->data;
  end = p = beg + XSTRING (file)->size;

  while (p != beg && p[-1] != '/'
#ifdef VMS
	 && p[-1] != ':' && p[-1] != ']' && p[-1] != '>'
#endif /* VMS */
#ifdef MSDOS
	 && p[-1] != ':' && p[-1] != '\\'
#endif
	 ) p--;

  return make_string (p, end - p);
}

DEFUN ("unhandled-file-name-directory", Funhandled_file_name_directory, Sunhandled_file_name_directory, 1, 1, 0,
  "Return a directly usable directory name somehow associated with FILENAME.\n\
A `directly usable' directory name is one that may be used without the\n\
intervention of any file handler.\n\
If FILENAME is a directly usable file itself, return\n\
(file-name-directory FILENAME).\n\
The `call-process' and `start-process' functions use this function to\n\
get a current directory to run processes in.")
  (filename)
    Lisp_Object filename;
{
  Lisp_Object handler;

  /* If the file name has special constructs in it,
     call the corresponding file handler.  */
  handler = Ffind_file_name_handler (filename, Qunhandled_file_name_directory);
  if (!NILP (handler))
    return call2 (handler, Qunhandled_file_name_directory, filename);

  return Ffile_name_directory (filename);
}


char *
file_name_as_directory (out, in)
     char *out, *in;
{
  int size = strlen (in) - 1;
  int ext_point = 0;

  strcpy (out, in);

#ifdef VMS
  /* Is it already a directory string? */
  if (in[size] == ':' || in[size] == ']' || in[size] == '>')
    return out;
  /* Is it a VMS directory file name?  If so, hack VMS syntax.  */
  else
    if (! index (in, '/'))
      {
	ext_point = 1;
	if (size > 3 && (! strcmp (&in[size - 3], ".DIR")
			 || ! strcmp (&in[size - 3], ".dir")))
	  ext_point = -3;
	else
	  if (size > 5 && (! strncmp (&in[size - 5], ".DIR", 4)
			   || ! strncmp (&in[size - 5], ".dir", 4))
	      && (in[size - 1] == '.' || in[size - 1] == ';')
	      && in[size] == '1')
	    ext_point = -5;
      }
  if (ext_point != 0)
    {
      register char *p, *dot;
      char brack;

      /* dir:[000000]x.dir --> dir:x.dir --> dir:[x]
	 dir:[000000.x]y.dir --> dir:[x]y.dir --> dir:[x.y]
	 but dir:[000000.000000]x.dir --> dir:[000000.000000.x]
	     dir:[000000.000000.x]y.dir --> dir:[000000.000000.x.y] */
      static char tem[256];

      p = dot = strchr(in,':');
      if (p != 0 && (p[1] == '[' || p[1] == '<'))
	{
	  p += 2;
	  if (strncmp(p,"000000",6) == 0)
	    {
	      p += 6;
	      if (strncmp(p,".000000",7) != 0
		  && (*p == ']' || *p == '>' || *p == '.'))
		{
		  size = dot - in + 1;
		  strncpy(tem, in, size);
		  if (*p == '.')
		    tem[size++] = '[';
		  strcpy(tem + size, p + 1);
		  in = tem;
		  size = strlen(in) - 1;
		}
	    }
	}
      /* x.dir -> [.x]
	 dir:x.dir --> dir:[x]
	 dir:[x]y.dir --> dir:[x.y] */
      p = in + size;
      while (p != in && *p != ':' && *p != '>' && *p != ']') p--;
      {
	char *emergency_dir = 0;
	int emergency_point = 0; /* relative to the end of `out' */

	if (p != in)
	  {
	    strncpy (out, in, p - in);
	    out[p - in] = '\0';
	    if (*p == ':')
	      {
		brack = ']';
		strcat (out, ":[");
		emergency_dir = "000000";
		emergency_point = 0;
	      }
	    else
	      {
		brack = *p;
		strcat (out, ".");
		emergency_dir = "";
		emergency_point = -1;
	      }
	    p++;
	  }
	else
	  {
	    brack = ']';
	    strcpy (out, "[.");
	    emergency_dir = "";
	    emergency_point = -2;
	  }
	if (strncmp (p, "000000.", 7) == 0
	    && (strncmp (p+7, "000000", 6) != 0
		|| (p[13] != ']' && p[13] != '>' && p[13] != '.')))
	  p += 7;
	if (p < (in + size + ext_point))
	  {
	    register copy_len = ((in + size + ext_point) - p);
	    size = strlen (out) + copy_len;
	    strncat (out, p, copy_len);
	  }
	else
	  {
	    size = strlen (out) + emergency_point;
	    strcpy (out + size, emergency_dir);
	    size += strlen (emergency_dir);
	  }
      }
      out[size++] = brack;
      out[size] = '\0';
    }
#else /* not VMS */
  /* For Unix syntax, Append a slash if necessary */
#ifdef MSDOS
  if (out[size] != ':' && out[size] != '/' && out[size] != '\\')
#else
  if (out[size] != '/')
#endif
    strcat (out, "/");
#endif /* not VMS */
  return out;
}

DEFUN ("file-name-as-directory", Ffile_name_as_directory,
       Sfile_name_as_directory, 1, 1, 0,
  "Return a string representing file FILENAME interpreted as a directory.\n\
This operation exists because a directory is also a file, but its name as\n\
a directory is different from its name as a file.\n\
The result can be used as the value of `default-directory'\n\
or passed as second argument to `expand-file-name'.\n\
For a Unix-syntax file name, just appends a slash.\n\
On VMS, converts \"[X]FOO.DIR\" to \"[X.FOO]\", etc.")
  (file)
     Lisp_Object file;
{
  char *buf;
  Lisp_Object handler;

  CHECK_STRING (file, 0);
  if (NILP (file))
    return Qnil;

  /* If the file name has special constructs in it,
     call the corresponding file handler.  */
  handler = Ffind_file_name_handler (file, Qfile_name_as_directory);
  if (!NILP (handler))
    return call2 (handler, Qfile_name_as_directory, file);

  buf = (char *) alloca (XSTRING (file)->size + 10);
  return build_string (file_name_as_directory (buf, XSTRING (file)->data));
}

/*
 * Convert from directory name to filename.
 * On VMS:
 *       xyzzy:[mukesh.emacs] => xyzzy:[mukesh]emacs.dir.1
 *       xyzzy:[mukesh] => xyzzy:[000000]mukesh.dir.1
 * On UNIX, it's simple: just make sure there is a terminating /

 * Value is nonzero if the string output is different from the input.
 */

directory_file_name (src, dst)
     char *src, *dst;
{
  long slen;
#ifdef VMS
  long rlen;
  char * ptr, * rptr;
  char bracket;
  struct FAB fab = cc$rms_fab;
  struct NAM nam = cc$rms_nam;
  char esa[NAM$C_MAXRSS];
#endif /* VMS */

  slen = strlen (src);
#ifdef VMS
  if (! index (src, '/')
      && (src[slen - 1] == ']'
	  || src[slen - 1] == ':'
	  || src[slen - 1] == '>'))
    {
      /* VMS style - convert [x.y.z] to [x.y]z, [x] to [000000]x */
      fab.fab$l_fna = src;
      fab.fab$b_fns = slen;
      fab.fab$l_nam = &nam;
      fab.fab$l_fop = FAB$M_NAM;

      nam.nam$l_esa = esa;
      nam.nam$b_ess = sizeof esa;
      nam.nam$b_nop |= NAM$M_SYNCHK;

      /* We call SYS$PARSE to handle such things as [--] for us. */
      if (SYS$PARSE(&fab, 0, 0) == RMS$_NORMAL)
	{
	  slen = nam.nam$b_esl;
	  if (esa[slen - 1] == ';' && esa[slen - 2] == '.')
	    slen -= 2;
	  esa[slen] = '\0';
	  src = esa;
	}
      if (src[slen - 1] != ']' && src[slen - 1] != '>')
	{
	  /* what about when we have logical_name:???? */
	  if (src[slen - 1] == ':')
	    {			/* Xlate logical name and see what we get */
	      ptr = strcpy (dst, src); /* upper case for getenv */
	      while (*ptr)
		{
		  if ('a' <= *ptr && *ptr <= 'z')
		    *ptr -= 040;
		  ptr++;
		}
	      dst[slen - 1] = 0;	/* remove colon */
	      if (!(src = egetenv (dst)))
		return 0;
	      /* should we jump to the beginning of this procedure?
		 Good points: allows us to use logical names that xlate
		 to Unix names,
		 Bad points: can be a problem if we just translated to a device
		 name...
		 For now, I'll punt and always expect VMS names, and hope for
		 the best! */
	      slen = strlen (src);
	      if (src[slen - 1] != ']' && src[slen - 1] != '>')
		{ /* no recursion here! */
		  strcpy (dst, src);
		  return 0;
		}
	    }
	  else
	    {		/* not a directory spec */
	      strcpy (dst, src);
	      return 0;
	    }
	}
      bracket = src[slen - 1];

      /* If bracket is ']' or '>', bracket - 2 is the corresponding
	 opening bracket.  */
      ptr = index (src, bracket - 2);
      if (ptr == 0)
	{ /* no opening bracket */
	  strcpy (dst, src);
	  return 0;
	}
      if (!(rptr = rindex (src, '.')))
	rptr = ptr;
      slen = rptr - src;
      strncpy (dst, src, slen);
      dst[slen] = '\0';
      if (*rptr == '.')
	{
	  dst[slen++] = bracket;
	  dst[slen] = '\0';
	}
      else
	{
	  /* If we have the top-level of a rooted directory (i.e. xx:[000000]),
	     then translate the device and recurse. */
	  if (dst[slen - 1] == ':'
	      && dst[slen - 2] != ':'	/* skip decnet nodes */
	      && strcmp(src + slen, "[000000]") == 0)
	    {
	      dst[slen - 1] = '\0';
	      if ((ptr = egetenv (dst))
		  && (rlen = strlen (ptr) - 1) > 0
		  && (ptr[rlen] == ']' || ptr[rlen] == '>')
		  && ptr[rlen - 1] == '.')
		{
		  char * buf = (char *) alloca (strlen (ptr) + 1);
		  strcpy (buf, ptr);
		  buf[rlen - 1] = ']';
		  buf[rlen] = '\0';
		  return directory_file_name (buf, dst);
		}
	      else
		dst[slen - 1] = ':';
	    }
	  strcat (dst, "[000000]");
	  slen += 8;
	}
      rptr++;
      rlen = strlen (rptr) - 1;
      strncat (dst, rptr, rlen);
      dst[slen + rlen] = '\0';
      strcat (dst, ".DIR.1");
      return 1;
    }
#endif /* VMS */
  /* Process as Unix format: just remove any final slash.
     But leave "/" unchanged; do not change it to "".  */
  strcpy (dst, src);
  if (slen > 1 
#ifdef MSDOS
      && (dst[slen - 1] == '/' || dst[slen - 1] == '/')
      && dst[slen - 2] != ':'
#else
      && dst[slen - 1] == '/'
#endif
      )
    dst[slen - 1] = 0;
  return 1;
}

DEFUN ("directory-file-name", Fdirectory_file_name, Sdirectory_file_name,
  1, 1, 0,
  "Returns the file name of the directory named DIR.\n\
This is the name of the file that holds the data for the directory DIR.\n\
This operation exists because a directory is also a file, but its name as\n\
a directory is different from its name as a file.\n\
In Unix-syntax, this function just removes the final slash.\n\
On VMS, given a VMS-syntax directory name such as \"[X.Y]\",\n\
it returns a file name such as \"[X]Y.DIR.1\".")
  (directory)
     Lisp_Object directory;
{
  char *buf;
  Lisp_Object handler;

  CHECK_STRING (directory, 0);

  if (NILP (directory))
    return Qnil;

  /* If the file name has special constructs in it,
     call the corresponding file handler.  */
  handler = Ffind_file_name_handler (directory, Qdirectory_file_name);
  if (!NILP (handler))
    return call2 (handler, Qdirectory_file_name, directory);

#ifdef VMS
  /* 20 extra chars is insufficient for VMS, since we might perform a
     logical name translation. an equivalence string can be up to 255
     chars long, so grab that much extra space...  - sss */
  buf = (char *) alloca (XSTRING (directory)->size + 20 + 255);
#else
  buf = (char *) alloca (XSTRING (directory)->size + 20);
#endif
  directory_file_name (XSTRING (directory)->data, buf);
  return build_string (buf);
}

DEFUN ("make-temp-name", Fmake_temp_name, Smake_temp_name, 1, 1, 0,
  "Generate temporary file name (string) starting with PREFIX (a string).\n\
The Emacs process number forms part of the result,\n\
so there is no danger of generating a name being used by another process.")
  (prefix)
     Lisp_Object prefix;
{
  Lisp_Object val;
  val = concat2 (prefix, build_string ("XXXXXX"));
  mktemp (XSTRING (val)->data);
  return val;
}

DEFUN ("expand-file-name", Fexpand_file_name, Sexpand_file_name, 1, 2, 0,
  "Convert FILENAME to absolute, and canonicalize it.\n\
Second arg DEFAULT is directory to start with if FILENAME is relative\n\
 (does not start with slash); if DEFAULT is nil or missing,\n\
the current buffer's value of default-directory is used.\n\
Path components that are `.' are removed, and \n\
path components followed by `..' are removed, along with the `..' itself;\n\
note that these simplifications are done without checking the resulting\n\
paths in the file system.\n\
An initial `~/' expands to your home directory.\n\
An initial `~USER/' expands to USER's home directory.\n\
See also the function `substitute-in-file-name'.\n\n\
On VMS, expand-file-name does a little more work. The following\n\
conversions are done:\n\
    - [000000.foo]		=> [foo]\n\
    - [000000.000000.foo]	=> [000000.000000.foo]\n\
    - [000000.][foo]		=> [foo]\n\
    - [000000][.foo]		=> [foo]\n\
    - [000000][.000000]		=> [000000.000000]\n\
    - [foo.][000000]		=> [foo]\n\
    - [foo][.000000]		=> [foo.000000]\n\
    - [foo.][000000.000000]	=> [foo.000000]\n\
    - [foo.][000000.bar]	=> [foo.bar]\n\
    - [foo][.000000.bar]	=> [foo.000000.bar]\n\
    - [foo.][000000.000000.bar]	=> [foo.000000.bar]\n\
    - [foo.][bar]		=> [foo.bar]\n\
    - [foo][.bar]		=> [foo.bar]\n\
    - [foo][bar]		=> [bar]\n\n\
Also, if <> has been used instead of [] as directory limiters, they\n\
are converted back to [].")
     (name, defalt)
     Lisp_Object name, defalt;
{
  unsigned char *nm;
  
  register unsigned char *newdir, *p, *o;
  int tlen;
  unsigned char *target;
  struct passwd *pw;
#ifdef VMS
  unsigned char * colon = 0;
  unsigned char * close = 0;
  unsigned char * slash = 0;
  unsigned char * brack = 0;
  int lbrack = 0, rbrack = 0;
  int dots = 0;
  int zeroes = -1;		/* counts zeroes in [000000. construct */
  unsigned char * firstbrack = 0, * lastbrack = 0;
#endif /* VMS */
#ifdef MSDOS	/* Demacs 1.1.2 91/10/20 Manabu Higashida */
  int drive = -1;
  int relpath = 0;
  unsigned char *tmp, *defdir;
#endif
  Lisp_Object handler;
  
  CHECK_STRING (name, 0);

  /* If the file name has special constructs in it,
     call the corresponding file handler.  */
  handler = Ffind_file_name_handler (name, Qexpand_file_name);
  if (!NILP (handler))
    return call3 (handler, Qexpand_file_name, name, defalt);

  /* Use the buffer's default-directory if DEFALT is omitted.  */
  if (NILP (defalt))
    defalt = current_buffer->directory;
  CHECK_STRING (defalt, 1);

  /* Make sure DEFALT is properly expanded.
     It would be better to do this down below where we actually use
     defalt.  Unfortunately, calling Fexpand_file_name recursively
     could invoke GC, and the strings might be relocated.  This would
     be annoying because we have pointers into strings lying around
     that would need adjusting, and people would add new pointers to
     the code and forget to adjust them, resulting in intermittent bugs.
     Putting this call here avoids all that crud.

     The EQ test avoids infinite recursion.  */
  if (! NILP (defalt) && !EQ (defalt, name)
      /* This saves time in a common case.  */
      && XSTRING (defalt)->data[0] != '/')
    {
      struct gcpro gcpro1;

      GCPRO1 (name);
      defalt = Fexpand_file_name (defalt, Qnil);
      UNGCPRO;
    }

#ifdef VMS
  /* Filenames on VMS are always upper case.  */
  name = Fupcase (name);
#endif
#ifdef FILE_SYSTEM_CASE
  name = FILE_SYSTEM_CASE (name);
#endif

  nm = XSTRING (name)->data;
  
#ifdef MSDOS
  /* First map all backslashes to slashes.  */
  dostounix_filename (nm = strcpy (alloca (strlen (nm) + 1), nm));

  /* Now strip drive name. */
  {
    unsigned char *colon = rindex (nm, ':');
    if (colon)
      if (nm == colon)
	nm++;
      else
	{
	  drive = tolower (colon[-1]) - 'a';
	  nm = colon + 1;
	  if (*nm != '/')
	    {
	      defdir = alloca (MAXPATHLEN + 1);
	      relpath = getdefdir (drive + 1, defdir);
	    }
	}	
  }
#endif

  /* If nm is absolute, flush ...// and detect /./ and /../.
     If no /./ or /../ we can return right away. */
  if (
      nm[0] == '/'
#ifdef VMS
      || index (nm, ':')
#endif /* VMS */
      )
    {
      /* If it turns out that the filename we want to return is just a
	 suffix of FILENAME, we don't need to go through and edit
	 things; we just need to construct a new string using data
	 starting at the middle of FILENAME.  If we set lose to a
	 non-zero value, that means we've discovered that we can't do
	 that cool trick.  */
      int lose = 0;

      p = nm;
      while (*p)
	{
	  /* Since we know the path is absolute, we can assume that each
	     element starts with a "/".  */

	  /* "//" anywhere isn't necessarily hairy; we just start afresh
	     with the second slash.  */
	  if (p[0] == '/' && p[1] == '/'
#ifdef APOLLO
	      /* // at start of filename is meaningful on Apollo system */
	      && nm != p
#endif /* APOLLO */
	      )
	    nm = p + 1;

	  /* "~" is hairy as the start of any path element.  */
	  if (p[0] == '/' && p[1] == '~')
	    nm = p + 1, lose = 1;

	  /* "." and ".." are hairy.  */
	  if (p[0] == '/'
	      && p[1] == '.'
	      && (p[2] == '/'
		  || p[2] == 0
		  || (p[2] == '.' && (p[3] == '/'
				      || p[3] == 0))))
	    lose = 1;
#ifdef VMS
	  if (p[0] == '\\')
	    lose = 1;
	  if (p[0] == '/') {
	    /* if dev:[dir]/, move nm to / */
	    if (!slash && p > nm && (brack || colon)) {
	      nm = (brack ? brack + 1 : colon + 1);
	      lbrack = rbrack = 0;
	      brack = 0;
	      colon = 0;
	    }
	    slash = p;
	  }
	  if (p[0] == '-')
#ifndef VMS4_4
	    /* VMS pre V4.4,convert '-'s in filenames. */
	    if (lbrack == rbrack)
	      {
		if (dots < 2)	/* this is to allow negative version numbers */
		  p[0] = '_';
	      }
	    else
#endif /* VMS4_4 */
	      if (lbrack > rbrack &&
		  ((p[-1] == '.' || p[-1] == '[' || p[-1] == '<') &&
		   (p[1] == '.' || p[1] == ']' || p[1] == '>')))
		lose = 1;
#ifndef VMS4_4
	      else
		p[0] = '_';
#endif /* VMS4_4 */
	  /* count zeroes, and if already six, check if dot */
	  /* N.B. this only sets lose = 1 when found [000000.xxx
	     All other valid cases are detected by the ][ detection */
	  if (zeroes == 6 && p[0] == '.' && p[1] != ']' && p[1] != '>')
	    lose = 1; /* six zeroes found, followed by a dot */
	  if (zeroes >= 0 && zeroes <= 6 && p[0] == '0')
	    zeroes++;
	  else
	    zeroes = -1;
	  /* count open brackets, reset close bracket pointer */
	  if (p[0] == '[' || p[0] == '<')
	    lbrack++, brack = 0, lose = (lose || p[0] == '<'), zeroes = 0;
	  /* count close brackets, set close bracket pointer */
	  if (p[0] == ']' || p[0] == '>')
	    rbrack++, brack = p, lose = (lose || p[0] == '>');
	  /* detect ][ or >< */
	  if ((p[0] == ']' || p[0] == '>') && (p[1] == '[' || p[1] == '<'))
	    lose = 1;
	  if ((p[0] == ':' || p[0] == ']' || p[0] == '>') && p[1] == '~')
	    nm = p + 1, lose = 1;
	  if (p[0] == ':' && (colon || slash))
	    /* if dev1:[dir]dev2:, move nm to dev2:  */
	    if (brack)
	      {
		nm = brack + 1;
		brack = 0;
		colon = 0;	/* To prevent dev1:[dir]node:: from becoming
				   [dir]node::  */
	      }
	    /* if /pathname/dev:, move nm to dev: */
	    else if (slash)
	      nm = slash + 1;
	    /* if node::dev:, move colon following dev */
	    else if (colon && colon[-1] == ':')
	      colon = p;
	    /* if dev1:dev2:, move nm to dev2: */
	    else if (colon && colon[-1] != ':')
	      {
		nm = colon + 1;
		colon = 0;
	      }
	  if (p[0] == ':' && !colon)
	    {
	      if (p[1] == ':')
		p++;
	      colon = p;
	    }
	  if (lbrack == rbrack)
	    if (p[0] == ';')
	      dots = 2;
	    else if (p[0] == '.')
	      dots++;
	  if (p[0] == '.' && p[1] == '.')
	    lose = 1;
#endif /* VMS */
	  p++;
	}
      if (!lose)
	{
#ifdef VMS
	  if (index (nm, '/') || strcmp (nm, "..") == 0)
	    return build_string (sys_translate_unix (nm));
#endif /* VMS */
#ifndef MSDOS
	  if (nm == XSTRING (name)->data)
	    return name;
	  return build_string (nm);
#endif
	}
    }

  /* Now determine directory to start with and put it in newdir */

  newdir = 0;

  if (nm[0] == '~')		/* prefix ~ */
    {
      if (nm[1] == '/'
#ifdef VMS
	  || nm[1] == ':'
#endif				/* VMS */
	  || nm[1] == 0)	/* ~ by itself */
	{
	  if (!(newdir = (unsigned char *) egetenv ("HOME")))
	    newdir = (unsigned char *) "";
#ifdef MSDOS
	  dostounix_filename (newdir);
#endif
	  nm++;
#ifdef VMS
	  if (*nm != 0)
	    nm++;		/* Don't leave the slash in nm.  */
#endif /* VMS */
	}
      else			/* ~user/filename */
	{
	  for (p = nm; *p && (*p != '/'
#ifdef VMS
			      && *p != ':'
#endif /* VMS */
			      ); p++);
	  o = (unsigned char *) alloca (p - nm + 1);
	  bcopy ((char *) nm, o, p - nm);
	  o [p - nm] = 0;

	  pw = (struct passwd *) getpwnam (o + 1);
	  if (pw)
	    {
	      newdir = (unsigned char *) pw -> pw_dir;
#ifdef VMS
	      nm = p + 1;	/* skip the terminator */
#else
	      nm = p;
#endif				/* VMS */
	    }

	  /* If we don't find a user of that name, leave the name
	     unchanged; don't move nm forward to p.  */
	}
    }

  if (nm[0] != '/'
#ifdef VMS
      && !index (nm, ':')
#endif /* not VMS */
#ifdef MSDOS
      && drive == -1
#endif
      && !newdir)
    {
      newdir = XSTRING (defalt)->data;
    }

#ifdef MSDOS
  if (newdir == 0 && relpath)
    newdir = defdir; 
#endif
  if (newdir != 0)
    {
      /* Get rid of any slash at the end of newdir.  */
      int length = strlen (newdir);
      /* Adding `length > 1 &&' makes ~ expand into / when homedir
	 is the root dir.  People disagree about whether that is right.
	 Anyway, we can't take the risk of this change now.  */
#ifdef MSDOS
      if (newdir[1] != ':' && length > 1)
#endif
      if (newdir[length - 1] == '/')
	{
	  unsigned char *temp = (unsigned char *) alloca (length);
	  bcopy (newdir, temp, length - 1);
	  temp[length - 1] = 0;
	  newdir = temp;
	}
      tlen = length + 1;
    }
  else
    tlen = 0;

  /* Now concatenate the directory and name to new space in the stack frame */
  tlen += strlen (nm) + 1;
#ifdef VMS
  tlen += 10;			/* VMS might need that extra space in
				   one special case, '[x.-]' ==> '[000000]' */
#endif
#ifdef MSDOS
  /* Add reserved space for drive name.  */
  target = (unsigned char *) alloca (tlen + 2) + 2;
#else
  target = (unsigned char *) alloca (tlen);
#endif
  *target = 0;

  if (newdir)
    {
#ifndef VMS
      if (nm[0] == 0 || nm[0] == '/')
	strcpy (target, newdir);
      else
#endif
	file_name_as_directory (target, newdir);
    }

#ifdef VMS
  if (index (target, '/') || strcmp (target, "..") == 0)
    strcpy (target, sys_translate_unix (target));
  if (index (nm, '/') || strcmp (nm, "..") == 0)
    strcat (target, sys_translate_unix (nm));
  else
#endif /* VMS */
  strcat (target, nm);

  /* Now canonicalize by removing /. and /foo/.. if they appear.  */

  p = target;
  o = target;

#ifdef VMS
  zeroes = -1;
  rbrack = 0;			/* I reuse rbrack here, to detect the
				   presence of ][ */
#endif /* VMS */

  while (*p)
    {
#ifdef VMS
      if (*p != ']' && *p != '>' && *p != '-')
	{
	  /* count zeroes, and if already six, check if dot */
	  if (zeroes == 7)
	    {
	      /* dev:[000000.foo] -> dev:[foo] */
	      /* however, dev:[000000.000000], dev:[000000.000000.foo],
		 stay as they are */
	      if (rbrack
		  || strncmp(p, "000000", 6) != 0
		  || (p[6] != '.' && p[6] != ']' && p[6] != '>'))
		o -= 7;		/* Get rid of 000000. */
	      zeroes = -1;
	    }
	  else
	    if (zeroes == 6 && p[0] == '.'
		|| zeroes >= 0 && zeroes <= 5 && p[0] == '0')
	      zeroes++;
	    else
	      zeroes = -1;

	  if (zeroes == -1)
	    rbrack = 0;

	  if (*p == '[' || *p == '<')
	    lbrack = 1, zeroes = 0;
	  else
	    lbrack = 0;
	  if (firstbrack >= o)
	    firstbrack = 0;
	  if (firstbrack == 0 && lbrack)
	    firstbrack = o, lbrack = 0;
	  if (*p == '\\')
	    {
	      p++;
	      *o++ = *p++;
	    }
	  else if (*p == '.' && o[-1] == '.')
	    p++;
	  else
	    *o++ = *p++;
	}
      else if ((p[0] == ']' || p[0] == '>') && (p[1] == '[' || p[1] == '<'))
	{
	  register int tmp_rbrack = 1;
	  {
	    extern int noninteractive;
	    /*if(!noninteractive)lib$signal(SS$_DEBUG); /* TMPTMPTMP */
	  }
	  p += 2;
	  if (zeroes == -1) zeroes = 0;
	  if (*p != '.' && *p != '-' && o[-1] != '.')
	    {
	      /* convert [foo][bar] to [bar] */
	      while (o[-1] != '[' && o[-1] != '<')
		o--;
	      tmp_rbrack = 0;
	    }
	  else if (zeroes == 7)
	    /* convert [000000.][anything] to [anything],
	       convert [foo.][000000.][anything] to [foo.anything]
	       (and then check [anything] for starting [000000 ) */
	    {
	      if (rbrack)
		o -= 7, zeroes = 0;
	      else
		o -= 8, p--, zeroes = -1, tmp_rbrack = 0;
	    }
	  else if (zeroes == 6 && (strncmp(p, ".000000",7) != 0 || rbrack))
	    /* convert [000000][.foo] to [foo] as long as .foo doesn't
	       start with .000000 */
	    /* also convert [foo.][000000][.bar] to [foo.bar] */
	    o -= 6, p++, zeroes = -1;
	  else if (*p == '-' && o[-1] != '.')
	    *o++ = '.', tmp_rbrack = 1, zeroes = -1;
	  rbrack = tmp_rbrack;
	  if (zeroes != -1) zeroes = 0;
	}
      else if (p[0] == '-' && o[-1] == '.' &&
	       (p[1] == '.' || p[1] == ']' || p[1] == '>'))
	/* flush .foo.- ; leave - if stopped by '[' or '<' */
	{
	  register char *tmp_o = o;
	  zeroes = 0;
	  do
	    {
	      tmp_o--;
	      if (zeroes != -1 && *tmp_o == '0'
		  || zeroes == 0 && *tmp_o == '.') zeroes++;
	      else zeroes = -1;
	    }
	  while (tmp_o[-1] != '.' && tmp_o[-1] != '[' && tmp_o[-1] != '<');
	  if (zeroes != 7)
	    o = tmp_o, zeroes = -1;
	  else			/* [000000.000000.-. */
	    {
	      char *save_tmp_o = tmp_o;
	      if (tmp_o[-1] != '[' && tmp_o[-1] != '<')
		do
		  {
		    tmp_o--;
		    if (zeroes != -1 && *tmp_o == '0'
			|| zeroes == 7 && *tmp_o == '.') zeroes++;
		    else zeroes = -1;
		  }
	      while (tmp_o[-1] != '.' && tmp_o[-1] != '[' && tmp_o[-1] != '<'
		     && zeroes != -1);
	      if (zeroes != 7)
		o = save_tmp_o;
	      if (zeroes == 14 && (tmp_o[-1] == '[' || tmp_o[-1] == '<'))
		zeroes = 7;
	      else
		zeroes = -1;
	    }
	  if (o == p) /* '[000000.-' ==> '[000000.-' */
	    o = ++p, zeroes = -1;
	  else
	    {
	      if (zeroes == -1 && (o[-1] == '[' || o[-1] == '<'))
		zeroes = 0;
	      if (p[1] == '.')	/* foo.-.bar ==> bar*/
		p += 2;
	      else if (o[-1] == '.') /* '.foo.-]' ==> ']' */
		{
		  if (zeroes != -1)
		    zeroes--;
		  p++, o--;
		}
	      else /* [foo.-] ==> [000000] */
		{
		  p++;
		  if (p - o < 6)
		    memmove (p + 6 - (p - o), p, strlen (p) + 1);
		  else if (p - o > 6)
		    memmove (o + 6, p, strlen (p) + 1);
		  memcpy (o, "000000", 6);
		  o += 6;
		  p = o;
		  zeroes = 6;
		  rbrack = 0;
		}
	    }
	}
      else if (*p == '.' && o[-1] == '.')
	p++;
      else
	{
#ifndef VMS4_4
	  if (*p == '-' &&
	      o[-1] != '[' && o[-1] != '<' && o[-1] != '.' &&
	      p[1] != ']' && p[1] != '>' && p[1] != '.')
	    *p = '_';
#endif /* VMS4_4 */
	  if (*p == ']' || *p == '>')
	    {
	      lbrack = 1;
	      /* finally convert [foo.][000000] to [foo] */
	      if (rbrack && zeroes == 6)
		{
		  o -= 7;
		  zeroes = -1;
		}
	      lastbrack = o;
	    }
	  *o++ = *p++;
	}
#else /* not VMS */
      if (*p != '/')
 	{
	  *o++ = *p++;
	}
      else if (!strncmp (p, "//", 2)
#ifdef APOLLO
	       /* // at start of filename is meaningful in Apollo system */
	       && o != target
#endif /* APOLLO */
	       )
	{
	  o = target;
	  p++;
	}
      else if (p[0] == '/'
	       && p[1] == '.'
	       && (p[2] == '/'
		   || p[2] == 0))
	{
	  /* If "/." is the entire filename, keep the "/".  Otherwise,
	     just delete the whole "/.".  */
	  if (o == target && p[2] == '\0')
	    *o++ = *p;
	  p += 2;
	}
      else if (!strncmp (p, "/..", 3)
	       /* `/../' is the "superroot" on certain file systems.  */
	       && o != target
	       && (p[3] == '/' || p[3] == 0))
	{
	  while (o != target && *--o != '/')
	    ;
#ifdef APOLLO
	  if (o == target + 1 && o[-1] == '/' && o[0] == '/')
	    ++o;
	  else
#endif /* APOLLO */
	  if (o == target && *o == '/')
	    ++o;
	  p += 3;
	}
      else
 	{
	  *o++ = *p++;
	}
#endif /* not VMS */
    }

#ifdef MSDOS
  /* at last, set drive name. */
  if (target[1] != ':')
    {
      target -= 2;
      target[0] = (drive < 0 ? getdisk () : drive) + 'a';
      target[1] = ':';
    }
#endif

#ifdef VMS
  if (firstbrack != 0 && lastbrack != 0)
    {
      *firstbrack = '[';
      *lastbrack = ']';
    }
  return Fupcase (make_string (target, o - target));
#endif

  return make_string (target, o - target);
}
#if 0
/* Changed this DEFUN to a DEAFUN, so as not to confuse `make-docfile'.
DEAFUN ("expand-file-name", Fexpand_file_name, Sexpand_file_name, 1, 2, 0,
  "Convert FILENAME to absolute, and canonicalize it.\n\
Second arg DEFAULT is directory to start with if FILENAME is relative\n\
 (does not start with slash); if DEFAULT is nil or missing,\n\
the current buffer's value of default-directory is used.\n\
Filenames containing `.' or `..' as components are simplified;\n\
initial `~/' expands to your home directory.\n\
See also the function `substitute-in-file-name'.")
     (name, defalt)
     Lisp_Object name, defalt;
{
  unsigned char *nm;
  
  register unsigned char *newdir, *p, *o;
  int tlen;
  unsigned char *target;
  struct passwd *pw;
  int lose;
#ifdef VMS
  unsigned char * colon = 0;
  unsigned char * close = 0;
  unsigned char * slash = 0;
  unsigned char * brack = 0;
  int lbrack = 0, rbrack = 0;
  int dots = 0;
#endif /* VMS */
  
  CHECK_STRING (name, 0);

#ifdef VMS
  /* Filenames on VMS are always upper case.  */
  name = Fupcase (name);
#endif

  nm = XSTRING (name)->data;
  
  /* If nm is absolute, flush ...// and detect /./ and /../.
     If no /./ or /../ we can return right away. */
  if (
      nm[0] == '/'
#ifdef VMS
      || index (nm, ':')
#endif /* VMS */
      )
    {
      p = nm;
      lose = 0;
      while (*p)
	{
	  if (p[0] == '/' && p[1] == '/'
#ifdef APOLLO
	      /* // at start of filename is meaningful on Apollo system */
	      && nm != p
#endif /* APOLLO */
	      )
	    nm = p + 1;
	  if (p[0] == '/' && p[1] == '~')
	    nm = p + 1, lose = 1;
	  if (p[0] == '/' && p[1] == '.'
	      && (p[2] == '/' || p[2] == 0
		  || (p[2] == '.' && (p[3] == '/' || p[3] == 0))))
	    lose = 1;
#ifdef VMS
	  if (p[0] == '\\')
	    lose = 1;
	  if (p[0] == '/') {
	    /* if dev:[dir]/, move nm to / */
	    if (!slash && p > nm && (brack || colon)) {
	      nm = (brack ? brack + 1 : colon + 1);
	      lbrack = rbrack = 0;
	      brack = 0;
	      colon = 0;
	    }
	    slash = p;
	  }
	  if (p[0] == '-')
#ifndef VMS4_4
	    /* VMS pre V4.4,convert '-'s in filenames. */
	    if (lbrack == rbrack)
	      {
		if (dots < 2)	/* this is to allow negative version numbers */
		  p[0] = '_';
	      }
	    else
#endif /* VMS4_4 */
	      if (lbrack > rbrack &&
		  ((p[-1] == '.' || p[-1] == '[' || p[-1] == '<') &&
		   (p[1] == '.' || p[1] == ']' || p[1] == '>')))
		lose = 1;
#ifndef VMS4_4
	      else
		p[0] = '_';
#endif /* VMS4_4 */
	  /* count open brackets, reset close bracket pointer */
	  if (p[0] == '[' || p[0] == '<')
	    lbrack++, brack = 0;
	  /* count close brackets, set close bracket pointer */
	  if (p[0] == ']' || p[0] == '>')
	    rbrack++, brack = p;
	  /* detect ][ or >< */
	  if ((p[0] == ']' || p[0] == '>') && (p[1] == '[' || p[1] == '<'))
	    lose = 1;
	  if ((p[0] == ':' || p[0] == ']' || p[0] == '>') && p[1] == '~')
	    nm = p + 1, lose = 1;
	  if (p[0] == ':' && (colon || slash))
	    /* if dev1:[dir]dev2:, move nm to dev2: */
	    if (brack)
	      {
		nm = brack + 1;
		brack = 0;
	      }
	    /* if /pathname/dev:, move nm to dev: */
	    else if (slash)
	      nm = slash + 1;
	    /* if node::dev:, move colon following dev */
	    else if (colon && colon[-1] == ':')
	      colon = p;
	    /* if dev1:dev2:, move nm to dev2: */
	    else if (colon && colon[-1] != ':')
	      {
		nm = colon + 1;
		colon = 0;
	      }
	  if (p[0] == ':' && !colon)
	    {
	      if (p[1] == ':')
		p++;
	      colon = p;
	    }
	  if (lbrack == rbrack)
	    if (p[0] == ';')
	      dots = 2;
	    else if (p[0] == '.')
	      dots++;
#endif /* VMS */
	  p++;
	}
      if (!lose)
	{
#ifdef VMS
	  if (index (nm, '/') || strcmp (nm, "..") == 0)
	    return build_string (sys_translate_unix (nm));
#endif /* VMS */
	  if (nm == XSTRING (name)->data)
	    return name;
	  return build_string (nm);
	}
    }

  /* Now determine directory to start with and put it in NEWDIR */

  newdir = 0;

  if (nm[0] == '~')		/* prefix ~ */
    if (nm[1] == '/'
#ifdef VMS
	|| nm[1] == ':'
#endif /* VMS */
	|| nm[1] == 0)/* ~/filename */
      {
	if (!(newdir = (unsigned char *) egetenv ("HOME")))
	  newdir = (unsigned char *) "";
	nm++;
#ifdef VMS
	nm++;			/* Don't leave the slash in nm.  */
#endif /* VMS */
      }
    else  /* ~user/filename */
      {
	/* Get past ~ to user */
	unsigned char *user = nm + 1;
	/* Find end of name. */
	unsigned char *ptr = (unsigned char *) index (user, '/');
	int len = ptr ? ptr - user : strlen (user);
#ifdef VMS
	unsigned char *ptr1 = index (user, ':');
	if (ptr1 != 0 && ptr1 - user < len)
	  len = ptr1 - user;
#endif				/* VMS */
	/* Copy the user name into temp storage. */
	o = (unsigned char *) alloca (len + 1);
	bcopy ((char *) user, o, len);
	o[len] = 0;

	/* Look up the user name. */
	pw = (struct passwd *) getpwnam (o + 1);
	if (!pw)
	  error ("\"%s\" isn't a registered user", o + 1);

	newdir = (unsigned char *) pw->pw_dir;

	/* Discard the user name from NM.  */
	nm += len;
      }

  if (nm[0] != '/'
#ifdef VMS
      && !index (nm, ':')
#endif /* not VMS */
      && !newdir)
    {
      if (NILP (defalt))
	defalt = current_buffer->directory;
      CHECK_STRING (defalt, 1);
      newdir = XSTRING (defalt)->data;
    }

  /* Now concatenate the directory and name to new space in the stack frame */

  tlen = (newdir ? strlen (newdir) + 1 : 0) + strlen (nm) + 1;
  target = (unsigned char *) alloca (tlen);
  *target = 0;

  if (newdir)
    {
#ifndef VMS
      if (nm[0] == 0 || nm[0] == '/')
	strcpy (target, newdir);
      else
#endif
      file_name_as_directory (target, newdir);
    }

  strcat (target, nm);
#ifdef VMS
  if (index (target, '/') || strcmp (target, "..") == 0)
    strcpy (target, sys_translate_unix (target));
#endif /* VMS */

  /* Now canonicalize by removing /. and /foo/.. if they appear */

  p = target;
  o = target;

  while (*p)
    {
#ifdef VMS
      if (*p != ']' && *p != '>' && *p != '-')
	{
	  if (*p == '\\')
	    p++;
	  *o++ = *p++;
	}
      else if ((p[0] == ']' || p[0] == '>') && p[0] == p[1] + 2)
	/* brackets are offset from each other by 2 */
	{
	  p += 2;
	  if (*p != '.' && *p != '-' && o[-1] != '.')
	    /* convert [foo][bar] to [bar] */
	    while (o[-1] != '[' && o[-1] != '<')
	      o--;
	  else if (*p == '-' && *o != '.')
	    *--p = '.';
	}
      else if (p[0] == '-' && o[-1] == '.' &&
	       (p[1] == '.' || p[1] == ']' || p[1] == '>'))
	/* flush .foo.- ; leave - if stopped by '[' or '<' */
	{
	  do
	    o--;
	  while (o[-1] != '.' && o[-1] != '[' && o[-1] != '<');
	  if (p[1] == '.')	/* foo.-.bar ==> bar*/
	    p += 2;
	  else if (o[-1] == '.') /* '.foo.-]' ==> ']' */
	    p++, o--;
	  /* else [foo.-] ==> [-] */
	}
      else
	{
#ifndef VMS4_4
	  if (*p == '-' &&
	      o[-1] != '[' && o[-1] != '<' && o[-1] != '.' &&
	      p[1] != ']' && p[1] != '>' && p[1] != '.')
	    *p = '_';
#endif /* VMS4_4 */
	  *o++ = *p++;
	}
#else /* not VMS */
      if (*p != '/')
 	{
	  *o++ = *p++;
	}
      else if (!strncmp (p, "//", 2)
#ifdef APOLLO
	       /* // at start of filename is meaningful in Apollo system */
	       && o != target
#endif /* APOLLO */
	       )
	{
	  o = target;
	  p++;
	}
      else if (p[0] == '/' && p[1] == '.' &&
	       (p[2] == '/' || p[2] == 0))
	p += 2;
      else if (!strncmp (p, "/..", 3)
	       /* `/../' is the "superroot" on certain file systems.  */
	       && o != target
	       && (p[3] == '/' || p[3] == 0))
	{
	  while (o != target && *--o != '/')
	    ;
#ifdef APOLLO
	  if (o == target + 1 && o[-1] == '/' && o[0] == '/')
	    ++o;
	  else
#endif /* APOLLO */
	  if (o == target && *o == '/')
	    ++o;
	  p += 3;
	}
      else
 	{
	  *o++ = *p++;
	}
#endif /* not VMS */
    }
#ifdef VMS
  if (firstbrack != 0 && lastbrack != 0)
    {
      *firstbrack = '[';
      *lastbrack = ']';
    }
#endif

  return Fupcase (make_string (target, o - target));
}
#endif

DEFUN ("substitute-in-file-name", Fsubstitute_in_file_name,
  Ssubstitute_in_file_name, 1, 1, 0,
  "Substitute environment variables referred to in FILENAME.\n\
`$FOO' where FOO is an environment variable name means to substitute\n\
the value of that variable.  The variable name should be terminated\n\
with a character not a letter, digit or underscore; otherwise, enclose\n\
the entire variable name in braces.\n\
If `/~' appears, all of FILENAME through that `/' is discarded.\n\n\
On VMS, `$' substitution is not done; this function does little and only\n\
duplicates what `expand-file-name' does.")
  (string)
     Lisp_Object string;
{
  unsigned char *nm;

  register unsigned char *s, *p, *o, *x, *endp;
  unsigned char *target;
  int total = 0;
  int substituted = 0;
  unsigned char *xnm;

  CHECK_STRING (string, 0);

  nm = XSTRING (string)->data;
#ifdef MSDOS
  dostounix_filename (nm = strcpy (alloca (strlen (nm) + 1), nm));
  substituted = !strcmp (nm, XSTRING (string)->data);
#endif
  endp = nm + XSTRING (string)->size;

  /* If /~ or // appears, discard everything through first slash. */

  for (p = nm; p != endp; p++)
    {
      if ((p[0] == '~' ||
#ifdef APOLLO
	   /* // at start of file name is meaningful in Apollo system */
	   (p[0] == '/' && p - 1 != nm)
#else /* not APOLLO */
	   p[0] == '/'
#endif /* not APOLLO */
	   )
	  && p != nm &&
#ifdef VMS
	  (p[-1] == ':' || p[-1] == ']' || p[-1] == '>' ||
#endif /* VMS */
	  p[-1] == '/')
#ifdef VMS
	  )
#endif /* VMS */
	{
	  nm = p;
	  substituted = 1;
	}
#ifdef MSDOS
      if (p[0] && p[1] == ':')
	{
	  nm = p;
	  substituted = 1;
	}
#endif /* MSDOS */
    }

#ifdef VMS
  return build_string (nm);
#else

  /* See if any variables are substituted into the string
     and find the total length of their values in `total' */

  for (p = nm; p != endp;)
    if (*p != '$')
      p++;
    else
      {
	p++;
	if (p == endp)
	  goto badsubst;
	else if (*p == '$')
	  {
	    /* "$$" means a single "$" */
	    p++;
	    total -= 1;
	    substituted = 1;
	    continue;
	  }
	else if (*p == '{')
	  {
	    o = ++p;
	    while (p != endp && *p != '}') p++;
	    if (*p != '}') goto missingclose;
	    s = p;
	  }
	else
	  {
	    o = p;
	    while (p != endp && (isalnum (*p) || *p == '_')) p++;
	    s = p;
	  }

	/* Copy out the variable name */
	target = (unsigned char *) alloca (s - o + 1);
	strncpy (target, o, s - o);
	target[s - o] = 0;
#ifdef MSDOS
	strupr (target); /* $home == $HOME etc.  */
#endif

	/* Get variable value */
	o = (unsigned char *) egetenv (target);
	if (!o) goto badvar;
	total += strlen (o);
	substituted = 1;
      }

  if (!substituted)
    return string;

  /* If substitution required, recopy the string and do it */
  /* Make space in stack frame for the new copy */
  xnm = (unsigned char *) alloca (XSTRING (string)->size + total + 1);
  x = xnm;

  /* Copy the rest of the name through, replacing $ constructs with values */
  for (p = nm; *p;)
    if (*p != '$')
      *x++ = *p++;
    else
      {
	p++;
	if (p == endp)
	  goto badsubst;
	else if (*p == '$')
	  {
	    *x++ = *p++;
	    continue;
	  }
	else if (*p == '{')
	  {
	    o = ++p;
	    while (p != endp && *p != '}') p++;
	    if (*p != '}') goto missingclose;
	    s = p++;
	  }
	else
	  {
	    o = p;
	    while (p != endp && (isalnum (*p) || *p == '_')) p++;
	    s = p;
	  }

	/* Copy out the variable name */
	target = (unsigned char *) alloca (s - o + 1);
	strncpy (target, o, s - o);
	target[s - o] = 0;
#ifdef MSDOS
	strupr (target); /* $home == $HOME etc.  */
#endif

	/* Get variable value */
	o = (unsigned char *) egetenv (target);
	if (!o)
	  goto badvar;

	strcpy (x, o);
	x += strlen (o);
      }

  *x = 0;

  /* If /~ or // appears, discard everything through first slash. */

  for (p = xnm; p != x; p++)
    if ((p[0] == '~' ||
#ifdef APOLLO
	 /* // at start of file name is meaningful in Apollo system */
	 (p[0] == '/' && p - 1 != xnm)
#else /* not APOLLO */
	 p[0] == '/'
#endif /* not APOLLO */
	 )
	&& p != nm && p[-1] == '/')
      xnm = p;
#ifdef MSDOS
    else if (p[0] && p[1] == ':')
	xnm = p;
#endif

  return make_string (xnm, x - xnm);

 badsubst:
  error ("Bad format environment-variable substitution");
 missingclose:
  error ("Missing \"}\" in environment-variable substitution");
 badvar:
  error ("Substituting nonexistent environment variable \"%s\"", target);

  /* NOTREACHED */
#endif /* not VMS */
}

/* A slightly faster and more convenient way to get
   (directory-file-name (expand-file-name FOO)).  */

Lisp_Object
expand_and_dir_to_file (filename, defdir)
     Lisp_Object filename, defdir;
{
  register Lisp_Object abspath;

  abspath = Fexpand_file_name (filename, defdir);
#ifdef VMS
  {
    register int c = XSTRING (abspath)->data[XSTRING (abspath)->size - 1];
    if (c == ':' || c == ']' || c == '>')
      abspath = Fdirectory_file_name (abspath);
  }
#else
  /* Remove final slash, if any (unless path is root).
     stat behaves differently depending!  */
  if (XSTRING (abspath)->size > 1
      && XSTRING (abspath)->data[XSTRING (abspath)->size - 1] == '/')
    /* We cannot take shortcuts; they might be wrong for magic file names.  */
    abspath = Fdirectory_file_name (abspath);
#endif
  return abspath;
}

barf_or_query_if_file_exists (absname, querystring, interactive)
     Lisp_Object absname;
     unsigned char *querystring;
     int interactive;
{
  register Lisp_Object tem;
  struct stat statbuf;
  struct gcpro gcpro1;

  /* stat is a good way to tell whether the file exists,
     regardless of what access permissions it has.  */
  if (stat (XSTRING (absname)->data, &statbuf) >= 0)
    {
      if (! interactive)
	Fsignal (Qfile_already_exists,
		 Fcons (build_string ("File already exists"),
			Fcons (absname, Qnil)));
      GCPRO1 (absname);
      tem = do_yes_or_no_p (format1 ("File %s already exists; %s anyway? ",
				     XSTRING (absname)->data, querystring));
      UNGCPRO;
      if (NILP (tem))
	Fsignal (Qfile_already_exists,
		 Fcons (build_string ("File already exists"),
			Fcons (absname, Qnil)));
    }
  return;
}

DEFUN ("copy-file", Fcopy_file, Scopy_file, 2, 4,
  "fCopy file: \nFCopy %s to file: \np\nP",
  "Copy FILE to NEWNAME.  Both args must be strings.\n\
Signals a `file-already-exists' error if file NEWNAME already exists,\n\
unless a third argument OK-IF-ALREADY-EXISTS is supplied and non-nil.\n\
A number as third arg means request confirmation if NEWNAME already exists.\n\
This is what happens in interactive use with M-x.\n\
Fourth arg KEEP-TIME non-nil means give the new file the same\n\
last-modified time as the old one.  (This works on only some systems.)\n\
A prefix arg makes KEEP-TIME non-nil.")
  (filename, newname, ok_if_already_exists, keep_date)
     Lisp_Object filename, newname, ok_if_already_exists, keep_date;
{
  int ifd, ofd, n;
  char buf[16 * 1024];
  struct stat st;
  Lisp_Object handler;
  struct gcpro gcpro1, gcpro2;
  int count = specpdl_ptr - specpdl;
  int input_file_statable_p;

  GCPRO2 (filename, newname);
  CHECK_STRING (filename, 0);
  CHECK_STRING (newname, 1);
  filename = Fexpand_file_name (filename, Qnil);
  newname = Fexpand_file_name (newname, Qnil);

  /* If the input file name has special constructs in it,
     call the corresponding file handler.  */
  handler = Ffind_file_name_handler (filename, Qcopy_file);
  /* Likewise for output file name.  */
  if (NILP (handler))
    handler = Ffind_file_name_handler (newname, Qcopy_file);
  if (!NILP (handler))
    RETURN_UNGCPRO (call5 (handler, Qcopy_file, filename, newname,
			   ok_if_already_exists, keep_date));

  if (NILP (ok_if_already_exists)
      || XTYPE (ok_if_already_exists) == Lisp_Int)
    barf_or_query_if_file_exists (newname, "copy to it",
				  XTYPE (ok_if_already_exists) == Lisp_Int);

  ifd = open (XSTRING (filename)->data, O_RDONLY);
  if (ifd < 0)
    report_file_error ("Opening input file", Fcons (filename, Qnil));

  record_unwind_protect (close_file_unwind, make_number (ifd));

  /* We can only copy regular files and symbolic links.  Other files are not
     copyable by us. */
  input_file_statable_p = (fstat (ifd, &st) >= 0);

#if defined (S_ISREG) && defined (S_ISLNK)
  if (input_file_statable_p)
    {
      if (!(S_ISREG (st.st_mode)) && !(S_ISLNK (st.st_mode)))
	{
#if defined (EISDIR)
	  /* Get a better looking error message. */
	  errno = EISDIR;
#endif /* EISDIR */
	report_file_error ("Non-regular file", Fcons (filename, Qnil));
	}
    }
#endif /* S_ISREG && S_ISLNK */

#ifdef VMS
  /* Create the copy file with the same record format as the input file */
  ofd = sys_creat (XSTRING (newname)->data, 0666, ifd);
#else
#ifdef MSDOS
  /* System's default file type was set to binary by _fmode in emacs.c.  */
  ofd = creat (XSTRING (newname)->data, S_IREAD | S_IWRITE);
#else /* not MSDOS */
  ofd = creat (XSTRING (newname)->data, 0666);
#endif /* not MSDOS */
#endif /* VMS */
  if (ofd < 0)
      report_file_error ("Opening output file", Fcons (newname, Qnil));

  record_unwind_protect (close_file_unwind, make_number (ofd));

  immediate_quit = 1;
  QUIT;
  while ((n = read (ifd, buf, sizeof buf)) > 0)
    if (write (ofd, buf, n) != n)
	report_file_error ("I/O error", Fcons (newname, Qnil));
  immediate_quit = 0;

  /* Closing the output clobbers the file times on some systems.  */
  if (close (ofd) < 0)
    report_file_error ("I/O error", Fcons (newname, Qnil));

  if (input_file_statable_p)
    {
#ifndef VMS /* On VMS, chmod changes the modification time, whic is quite
	       stupid, so we have to put the VMS code after the chmod...  */
      if (!NILP (keep_date))
	{
	  EMACS_TIME atime, mtime;
	  EMACS_SET_SECS_USECS (atime, st.st_atime, 0);
	  EMACS_SET_SECS_USECS (mtime, st.st_mtime, 0);
	  EMACS_SET_UTIMES (XSTRING (newname)->data, atime, mtime);
	}
#endif /* not VMS */
#ifdef APOLLO
      if (!egetenv ("USE_DOMAIN_ACLS"))
#endif
	chmod (XSTRING (newname)->data, st.st_mode & 07777);
#ifdef VMS /* ... so here it is.  */
      if (!NILP (keep_date))
	{
	  EMACS_TIME atime, mtime;
	  EMACS_SET_SECS_USECS (atime, st.st_atime, 0);
	  EMACS_SET_SECS_USECS (mtime, st.st_mtime, 0);
	  EMACS_SET_UTIMES (XSTRING (newname)->data, atime, mtime);
	}
#endif /* VMS */
    }

  close (ifd);

  /* Discard the unwind protects.  */
  specpdl_ptr = specpdl + count;

  UNGCPRO;
  return Qnil;
}

DEFUN ("make-directory-internal", Fmake_directory_internal,
       Smake_directory_internal, 1, 1, 0,
  "Create a directory.  One argument, a file name string.")
  (dirname)
     Lisp_Object dirname;
{
  unsigned char *dir;
  Lisp_Object handler;

  CHECK_STRING (dirname, 0);
  dirname = Fexpand_file_name (dirname, Qnil);

  handler = Ffind_file_name_handler (dirname, Qmake_directory_internal);
  if (!NILP (handler))
    return call2 (handler, Qmake_directory_internal, dirname);

  dir = XSTRING (dirname)->data;

  if (mkdir (dir, 0777) != 0)
    report_file_error ("Creating directory", Flist (1, &dirname));

  return Qnil;
}

DEFUN ("delete-directory", Fdelete_directory, Sdelete_directory, 1, 1, "FDelete directory: ",
  "Delete a directory.  One argument, a file name or directory name string.")
  (dirname)
     Lisp_Object dirname;
{
  unsigned char *dir;
  Lisp_Object handler;

  CHECK_STRING (dirname, 0);
  dirname = Fdirectory_file_name (Fexpand_file_name (dirname, Qnil));
  dir = XSTRING (dirname)->data;

  handler = Ffind_file_name_handler (dirname, Qdelete_directory);
  if (!NILP (handler))
    return call2 (handler, Qdelete_directory, dirname);

#ifdef VMS
  dirname = Fdirectory_file_name(dirname);
  dir = XSTRING (dirname)->data;
  chmod (dir, 0755);		/* Otherwise, VMS won't let me delete it! */
  if (remove (dir) != 0)
    report_file_error ("Removing directory", Flist (1, &dirname));
#else
  if (rmdir (dir) != 0)
    report_file_error ("Removing directory", Flist (1, &dirname));
#endif

  return Qnil;
}

DEFUN ("delete-file", Fdelete_file, Sdelete_file, 1, 1, "fDelete file: ",
  "Delete specified file.  One argument, a file name string.\n\
If file has multiple names, it continues to exist with the other names.")
  (filename)
     Lisp_Object filename;
{
  Lisp_Object handler;
  CHECK_STRING (filename, 0);
  filename = Fexpand_file_name (filename, Qnil);

  handler = Ffind_file_name_handler (filename, Qdelete_file);
  if (!NILP (handler))
    return call2 (handler, Qdelete_file, filename);

  if (0 > unlink (XSTRING (filename)->data))
    report_file_error ("Removing old name", Flist (1, &filename));
  return Qnil;
}

DEFUN ("rename-file", Frename_file, Srename_file, 2, 3,
  "fRename file: \nFRename %s to file: \np",
  "Rename FILE as NEWNAME.  Both args strings.\n\
If file has names other than FILE, it continues to have those names.\n\
Signals a `file-already-exists' error if a file NEWNAME already exists\n\
unless optional third argument OK-IF-ALREADY-EXISTS is non-nil.\n\
A number as third arg means request confirmation if NEWNAME already exists.\n\
This is what happens in interactive use with M-x.")
  (filename, newname, ok_if_already_exists)
     Lisp_Object filename, newname, ok_if_already_exists;
{
#ifdef NO_ARG_ARRAY
  Lisp_Object args[2];
#endif
  Lisp_Object handler;
  struct gcpro gcpro1, gcpro2;

  GCPRO2 (filename, newname);
  CHECK_STRING (filename, 0);
  CHECK_STRING (newname, 1);
  filename = Fexpand_file_name (filename, Qnil);
  newname = Fexpand_file_name (newname, Qnil);

  /* If the file name has special constructs in it,
     call the corresponding file handler.  */
  handler = Ffind_file_name_handler (filename, Qrename_file);
  if (NILP (handler))
    handler = Ffind_file_name_handler (newname, Qrename_file);
  if (!NILP (handler))
    RETURN_UNGCPRO (call4 (handler, Qrename_file,
			   filename, newname, ok_if_already_exists));

  if (NILP (ok_if_already_exists)
      || XTYPE (ok_if_already_exists) == Lisp_Int)
    barf_or_query_if_file_exists (newname, "rename to it",
				  XTYPE (ok_if_already_exists) == Lisp_Int);
#ifndef BSD4_1
  if (0 > rename (XSTRING (filename)->data, XSTRING (newname)->data))
#else
  if (0 > link (XSTRING (filename)->data, XSTRING (newname)->data)
      || 0 > unlink (XSTRING (filename)->data))
#endif
    {
      if (errno == EXDEV)
	{
	  Fcopy_file (filename, newname,
		      /* We have already prompted if it was an integer,
			 so don't have copy-file prompt again.  */
		      NILP (ok_if_already_exists) ? Qnil : Qt, Qt);
	  Fdelete_file (filename);
	}
      else
#ifdef NO_ARG_ARRAY
	{
	  args[0] = filename;
	  args[1] = newname;
	  report_file_error ("Renaming", Flist (2, args));
	}
#else
	report_file_error ("Renaming", Flist (2, &filename));
#endif
    }
  UNGCPRO;
  return Qnil;
}

DEFUN ("add-name-to-file", Fadd_name_to_file, Sadd_name_to_file, 2, 3,
  "fAdd name to file: \nFName to add to %s: \np",
  "Give FILE additional name NEWNAME.  Both args strings.\n\
Signals a `file-already-exists' error if a file NEWNAME already exists\n\
unless optional third argument OK-IF-ALREADY-EXISTS is non-nil.\n\
A number as third arg means request confirmation if NEWNAME already exists.\n\
This is what happens in interactive use with M-x.")
  (filename, newname, ok_if_already_exists)
     Lisp_Object filename, newname, ok_if_already_exists;
{
#ifdef NO_ARG_ARRAY
  Lisp_Object args[2];
#endif
  Lisp_Object handler;
  struct gcpro gcpro1, gcpro2;

  GCPRO2 (filename, newname);
  CHECK_STRING (filename, 0);
  CHECK_STRING (newname, 1);
  filename = Fexpand_file_name (filename, Qnil);
  newname = Fexpand_file_name (newname, Qnil);

  /* If the file name has special constructs in it,
     call the corresponding file handler.  */
  handler = Ffind_file_name_handler (filename, Qadd_name_to_file);
  if (!NILP (handler))
    RETURN_UNGCPRO (call4 (handler, Qadd_name_to_file, filename,
			   newname, ok_if_already_exists));

  if (NILP (ok_if_already_exists)
      || XTYPE (ok_if_already_exists) == Lisp_Int)
    barf_or_query_if_file_exists (newname, "make it a new name",
				  XTYPE (ok_if_already_exists) == Lisp_Int);
  unlink (XSTRING (newname)->data);
  if (0 > link (XSTRING (filename)->data, XSTRING (newname)->data))
    {
#ifdef NO_ARG_ARRAY
      args[0] = filename;
      args[1] = newname;
      report_file_error ("Adding new name", Flist (2, args));
#else
      report_file_error ("Adding new name", Flist (2, &filename));
#endif
    }

  UNGCPRO;
  return Qnil;
}

#ifdef S_IFLNK
DEFUN ("make-symbolic-link", Fmake_symbolic_link, Smake_symbolic_link, 2, 3,
  "FMake symbolic link to file: \nFMake symbolic link to file %s: \np",
  "Make a symbolic link to FILENAME, named LINKNAME.  Both args strings.\n\
Signals a `file-already-exists' error if a file NEWNAME already exists\n\
unless optional third argument OK-IF-ALREADY-EXISTS is non-nil.\n\
A number as third arg means request confirmation if NEWNAME already exists.\n\
This happens for interactive use with M-x.")
  (filename, linkname, ok_if_already_exists)
     Lisp_Object filename, linkname, ok_if_already_exists;
{
#ifdef NO_ARG_ARRAY
  Lisp_Object args[2];
#endif
  Lisp_Object handler;
  struct gcpro gcpro1, gcpro2;

  GCPRO2 (filename, linkname);
  CHECK_STRING (filename, 0);
  CHECK_STRING (linkname, 1);
  /* If the link target has a ~, we must expand it to get
     a truly valid file name.  Otherwise, do not expand;
     we want to permit links to relative file names.  */
  if (XSTRING (filename)->data[0] == '~')
    filename = Fexpand_file_name (filename, Qnil);
  linkname = Fexpand_file_name (linkname, Qnil);

  /* If the file name has special constructs in it,
     call the corresponding file handler.  */
  handler = Ffind_file_name_handler (filename, Qmake_symbolic_link);
  if (!NILP (handler))
    RETURN_UNGCPRO (call4 (handler, Qmake_symbolic_link, filename,
			   linkname, ok_if_already_exists));

  if (NILP (ok_if_already_exists)
      || XTYPE (ok_if_already_exists) == Lisp_Int)
    barf_or_query_if_file_exists (linkname, "make it a link",
				  XTYPE (ok_if_already_exists) == Lisp_Int);
  if (0 > symlink (XSTRING (filename)->data, XSTRING (linkname)->data))
    {
      /* If we didn't complain already, silently delete existing file.  */
      if (errno == EEXIST)
	{
	  unlink (XSTRING (linkname)->data);
	  if (0 <= symlink (XSTRING (filename)->data, XSTRING (linkname)->data))
	    {
	      UNGCPRO;
	      return Qnil;
	    }
	}

#ifdef NO_ARG_ARRAY
      args[0] = filename;
      args[1] = linkname;
      report_file_error ("Making symbolic link", Flist (2, args));
#else
      report_file_error ("Making symbolic link", Flist (2, &filename));
#endif
    }
  UNGCPRO;
  return Qnil;
}
#endif /* S_IFLNK */

#ifdef VMS

DEFUN ("define-logical-name", Fdefine_logical_name, Sdefine_logical_name,
       2, 2, "sDefine logical name: \nsDefine logical name %s as: ",
  "Define the job-wide logical name NAME to have the value STRING.\n\
If STRING is nil or a null string, the logical name NAME is deleted.")
  (varname, string)
     Lisp_Object varname;
     Lisp_Object string;
{
  CHECK_STRING (varname, 0);
  if (NILP (string))
    delete_logical_name (XSTRING (varname)->data);
  else
    {
      CHECK_STRING (string, 1);

      if (XSTRING (string)->size == 0)
        delete_logical_name (XSTRING (varname)->data);
      else
        define_logical_name (XSTRING (varname)->data, XSTRING (string)->data);
    }

  return string;
}
#endif /* VMS */

#ifdef HPUX_NET

DEFUN ("sysnetunam", Fsysnetunam, Ssysnetunam, 2, 2, 0,
       "Open a network connection to PATH using LOGIN as the login string.")
     (path, login)
     Lisp_Object path, login;
{
  int netresult;
  
  CHECK_STRING (path, 0);
  CHECK_STRING (login, 0);  
  
  netresult = netunam (XSTRING (path)->data, XSTRING (login)->data);

  if (netresult == -1)
    return Qnil;
  else
    return Qt;
}
#endif /* HPUX_NET */

DEFUN ("file-name-absolute-p", Ffile_name_absolute_p, Sfile_name_absolute_p,
       1, 1, 0,
       "Return t if file FILENAME specifies an absolute path name.\n\
On Unix, this is a name starting with a `/' or a `~'.")
     (filename)
     Lisp_Object filename;
{
  unsigned char *ptr;

  CHECK_STRING (filename, 0);
  ptr = XSTRING (filename)->data;
  if (*ptr == '/' || *ptr == '~'
#ifdef VMS
/* ??? This criterion is probably wrong for '<'.  */
      || index (ptr, ':') || index (ptr, '<')
      || (*ptr == '[' && (ptr[1] != '-' || (ptr[2] != '.' && ptr[2] != ']'))
	  && ptr[1] != '.')
#endif /* VMS */
#ifdef MSDOS
      || (*ptr != 0 && ptr[1] == ':' && (ptr[2] == '/' || ptr[2] == '\\'))
#endif
      )
    return Qt;
  else
    return Qnil;
}

DEFUN ("file-exists-p", Ffile_exists_p, Sfile_exists_p, 1, 1, 0,
  "Return t if file FILENAME exists.  (This does not mean you can read it.)\n\
See also `file-readable-p' and `file-attributes'.")
  (filename)
     Lisp_Object filename;
{
  Lisp_Object abspath;
  Lisp_Object handler;
  struct stat statbuf;

  CHECK_STRING (filename, 0);
  abspath = Fexpand_file_name (filename, Qnil);

  /* If the file name has special constructs in it,
     call the corresponding file handler.  */
  handler = Ffind_file_name_handler (abspath, Qfile_exists_p);
  if (!NILP (handler))
    return call2 (handler, Qfile_exists_p, abspath);

  return (stat (XSTRING (abspath)->data, &statbuf) >= 0) ? Qt : Qnil;
}

DEFUN ("file-executable-p", Ffile_executable_p, Sfile_executable_p, 1, 1, 0,
  "Return t if FILENAME can be executed by you.\n\
For a directory, this means you can access files in that directory.")
  (filename)
    Lisp_Object filename;

{
  Lisp_Object abspath;
  Lisp_Object handler;

  CHECK_STRING (filename, 0);
  abspath = Fexpand_file_name (filename, Qnil);

  /* If the file name has special constructs in it,
     call the corresponding file handler.  */
  handler = Ffind_file_name_handler (abspath, Qfile_executable_p);
  if (!NILP (handler))
    return call2 (handler, Qfile_executable_p, abspath);

  return (access (XSTRING (abspath)->data, 1) >= 0) ? Qt : Qnil;
}

DEFUN ("file-readable-p", Ffile_readable_p, Sfile_readable_p, 1, 1, 0,
  "Return t if file FILENAME exists and you can read it.\n\
See also `file-exists-p' and `file-attributes'.")
  (filename)
     Lisp_Object filename;
{
  Lisp_Object abspath;
  Lisp_Object handler;
  int desc;

  CHECK_STRING (filename, 0);
  abspath = Fexpand_file_name (filename, Qnil);

  /* If the file name has special constructs in it,
     call the corresponding file handler.  */
  handler = Ffind_file_name_handler (abspath, Qfile_readable_p);
  if (!NILP (handler))
    return call2 (handler, Qfile_readable_p, abspath);

  desc = open (XSTRING (abspath)->data, O_RDONLY);
  if (desc < 0)
    return Qnil;
  close (desc);
  return Qt;
}

DEFUN ("file-symlink-p", Ffile_symlink_p, Sfile_symlink_p, 1, 1, 0,
  "Return non-nil if file FILENAME is the name of a symbolic link.\n\
The value is the name of the file to which it is linked.\n\
Otherwise returns nil.")
  (filename)
     Lisp_Object filename;
{
#ifdef S_IFLNK
  char *buf;
  int bufsize;
  int valsize;
  Lisp_Object val;
  Lisp_Object handler;

  CHECK_STRING (filename, 0);
  filename = Fexpand_file_name (filename, Qnil);

  /* If the file name has special constructs in it,
     call the corresponding file handler.  */
  handler = Ffind_file_name_handler (filename, Qfile_symlink_p);
  if (!NILP (handler))
    return call2 (handler, Qfile_symlink_p, filename);

  bufsize = 100;
  while (1)
    {
      buf = (char *) xmalloc (bufsize);
      bzero (buf, bufsize);
      valsize = readlink (XSTRING (filename)->data, buf, bufsize);
      if (valsize < bufsize) break;
      /* Buffer was not long enough */
      xfree (buf);
      bufsize *= 2;
    }
  if (valsize == -1)
    {
      xfree (buf);
      return Qnil;
    }
  val = make_string (buf, valsize);
  xfree (buf);
  return val;
#else /* not S_IFLNK */
  return Qnil;
#endif /* not S_IFLNK */
}

#ifdef SOLARIS_BROKEN_ACCESS
/* In Solaris 2.1, the readonly-ness of the filesystem is not
   considered by the access system call.  This is Sun's bug, but we
   still have to make Emacs work.  */

#include <sys/statvfs.h>

static int
ro_fsys (path)
    char *path;
{
    struct statvfs statvfsb;

    if (statvfs(path, &statvfsb))
      return 1;  /* error from statvfs, be conservative and say not wrtable */
    else
      /* Otherwise, fsys is ro if bit is set.  */
      return statvfsb.f_flag & ST_RDONLY;
}
#else
/* But on every other os, access has already done the right thing.  */
#define ro_fsys(path) 0
#endif

/* Having this before file-symlink-p mysteriously caused it to be forgotten
   on the RT/PC.  */
DEFUN ("file-writable-p", Ffile_writable_p, Sfile_writable_p, 1, 1, 0,
  "Return t if file FILENAME can be written or created by you.")
  (filename)
     Lisp_Object filename;
{
  Lisp_Object abspath, dir;
  Lisp_Object handler;

  CHECK_STRING (filename, 0);
  abspath = Fexpand_file_name (filename, Qnil);

  /* If the file name has special constructs in it,
     call the corresponding file handler.  */
  handler = Ffind_file_name_handler (abspath, Qfile_writable_p);
  if (!NILP (handler))
    return call2 (handler, Qfile_writable_p, abspath);

  if (access (XSTRING (abspath)->data, 0) >= 0)
    return ((access (XSTRING (abspath)->data, 2) >= 0
	     && ! ro_fsys ((char *) XSTRING (abspath)->data))
	    ? Qt : Qnil);
  dir = Ffile_name_directory (abspath);
#ifdef VMS
  if (!NILP (dir))
    dir = Fdirectory_file_name (dir);
#endif /* VMS */
#ifdef MSDOS
  if (!NILP (dir))
    dir = Fdirectory_file_name (dir);
#endif /* MSDOS */
  return ((access (!NILP (dir) ? (char *) XSTRING (dir)->data : "", 2) >= 0
	   && ! ro_fsys ((char *) XSTRING (dir)->data))
	  ? Qt : Qnil);
}

DEFUN ("file-directory-p", Ffile_directory_p, Sfile_directory_p, 1, 1, 0,
  "Return t if file FILENAME is the name of a directory as a file.\n\
A directory name spec may be given instead; then the value is t\n\
if the directory so specified exists and really is a directory.")
  (filename)
     Lisp_Object filename;
{
  register Lisp_Object abspath;
  struct stat st;
  Lisp_Object handler;

  abspath = expand_and_dir_to_file (filename, current_buffer->directory);

  /* If the file name has special constructs in it,
     call the corresponding file handler.  */
  handler = Ffind_file_name_handler (abspath, Qfile_directory_p);
  if (!NILP (handler))
    return call2 (handler, Qfile_directory_p, abspath);

  if (stat (XSTRING (abspath)->data, &st) < 0)
    return Qnil;
  return (st.st_mode & S_IFMT) == S_IFDIR ? Qt : Qnil;
}

DEFUN ("file-accessible-directory-p", Ffile_accessible_directory_p, Sfile_accessible_directory_p, 1, 1, 0,
  "Return t if file FILENAME is the name of a directory as a file,\n\
and files in that directory can be opened by you.  In order to use a\n\
directory as a buffer's current directory, this predicate must return true.\n\
A directory name spec may be given instead; then the value is t\n\
if the directory so specified exists and really is a readable and\n\
searchable directory.")
  (filename)
     Lisp_Object filename;
{
  Lisp_Object handler;
  int tem;
  struct gcpro gcpro1;

  /* If the file name has special constructs in it,
     call the corresponding file handler.  */
  handler = Ffind_file_name_handler (filename, Qfile_accessible_directory_p);
  if (!NILP (handler))
    return call2 (handler, Qfile_accessible_directory_p, filename);

  /* It's an unlikely combination, but yes we really do need to gcpro:
     Suppose that file-accessible-directory-p has no handler, but
     file-directory-p does have a handler; this handler causes a GC which
     relocates the string in `filename'; and finally file-directory-p
     returns non-nil.  Then we would end up passing a garbaged string
     to file-executable-p.  */
  GCPRO1 (filename);
  tem = (NILP (Ffile_directory_p (filename))
	 || NILP (Ffile_executable_p (filename)));
  UNGCPRO;
  return tem ? Qnil : Qt;
}

DEFUN ("file-modes", Ffile_modes, Sfile_modes, 1, 1, 0,
  "Return mode bits of FILE, as an integer.")
  (filename)
     Lisp_Object filename;
{
  Lisp_Object abspath;
  struct stat st;
  Lisp_Object handler;

  abspath = expand_and_dir_to_file (filename, current_buffer->directory);

  /* If the file name has special constructs in it,
     call the corresponding file handler.  */
  handler = Ffind_file_name_handler (abspath, Qfile_modes);
  if (!NILP (handler))
    return call2 (handler, Qfile_modes, abspath);

  if (stat (XSTRING (abspath)->data, &st) < 0)
    return Qnil;
#ifdef MSDOS
  {
    int len;
    char *suffix;
    if (S_ISREG (st.st_mode)
	&& (len = XSTRING (abspath)->size) >= 5
	&& (stricmp ((suffix = XSTRING (abspath)->data + len-4), ".com") == 0
	    || stricmp (suffix, ".exe") == 0
	    || stricmp (suffix, ".bat") == 0))
      st.st_mode |= S_IEXEC;
  }
#endif /* MSDOS */

  return make_number (st.st_mode & 07777);
}

DEFUN ("set-file-modes", Fset_file_modes, Sset_file_modes, 2, 2, 0,
  "Set mode bits of FILE to MODE (an integer).\n\
Only the 12 low bits of MODE are used.")
  (filename, mode)
     Lisp_Object filename, mode;
{
  Lisp_Object abspath;
  Lisp_Object handler;

  abspath = Fexpand_file_name (filename, current_buffer->directory);
  CHECK_NUMBER (mode, 1);

  /* If the file name has special constructs in it,
     call the corresponding file handler.  */
  handler = Ffind_file_name_handler (abspath, Qset_file_modes);
  if (!NILP (handler))
    return call3 (handler, Qset_file_modes, abspath, mode);

#ifndef APOLLO
  if (chmod (XSTRING (abspath)->data, XINT (mode)) < 0)
    report_file_error ("Doing chmod", Fcons (abspath, Qnil));
#else /* APOLLO */
  if (!egetenv ("USE_DOMAIN_ACLS"))
    {
      struct stat st;
      struct timeval tvp[2];

      /* chmod on apollo also change the file's modtime; need to save the
	 modtime and then restore it. */
      if (stat (XSTRING (abspath)->data, &st) < 0)
	{
	  report_file_error ("Doing chmod", Fcons (abspath, Qnil));
	  return (Qnil);
	}
 
      if (chmod (XSTRING (abspath)->data, XINT (mode)) < 0)
 	report_file_error ("Doing chmod", Fcons (abspath, Qnil));
 
      /* reset the old accessed and modified times.  */
      tvp[0].tv_sec = st.st_atime + 1; /* +1 due to an Apollo roundoff bug */
      tvp[0].tv_usec = 0;
      tvp[1].tv_sec = st.st_mtime + 1; /* +1 due to an Apollo roundoff bug */
      tvp[1].tv_usec = 0;
 
      if (utimes (XSTRING (abspath)->data, tvp) < 0)
 	report_file_error ("Doing utimes", Fcons (abspath, Qnil));
    }
#endif /* APOLLO */

  return Qnil;
}

DEFUN ("set-default-file-modes", Fset_default_file_modes, Sset_default_file_modes, 1, 1, 0,
    "Set the file permission bits for newly created files.\n\
The argument MODE should be an integer; only the low 9 bits are used.\n\
This setting is inherited by subprocesses.")
  (mode)
     Lisp_Object mode;
{
  CHECK_NUMBER (mode, 0);
  
  umask ((~ XINT (mode)) & 0777);

  return Qnil;
}

DEFUN ("default-file-modes", Fdefault_file_modes, Sdefault_file_modes, 0, 0, 0,
    "Return the default file protection for created files.\n\
The value is an integer.")
  ()
{
  int realmask;
  Lisp_Object value;

  realmask = umask (0);
  umask (realmask);

  XSET (value, Lisp_Int, (~ realmask) & 0777);
  return value;
}

#ifdef unix

DEFUN ("unix-sync", Funix_sync, Sunix_sync, 0, 0, "",
  "Tell Unix to finish all pending disk updates.")
  ()
{
  sync ();
  return Qnil;
}

#endif /* unix */

DEFUN ("file-newer-than-file-p", Ffile_newer_than_file_p, Sfile_newer_than_file_p, 2, 2, 0,
  "Return t if file FILE1 is newer than file FILE2.\n\
If FILE1 does not exist, the answer is nil;\n\
otherwise, if FILE2 does not exist, the answer is t.")
  (file1, file2)
     Lisp_Object file1, file2;
{
  Lisp_Object abspath1, abspath2;
  struct stat st;
  int mtime1;
  Lisp_Object handler;
  struct gcpro gcpro1, gcpro2;

  CHECK_STRING (file1, 0);
  CHECK_STRING (file2, 0);

  abspath1 = Qnil;
  GCPRO2 (abspath1, file2);
  abspath1 = expand_and_dir_to_file (file1, current_buffer->directory);
  abspath2 = expand_and_dir_to_file (file2, current_buffer->directory);
  UNGCPRO;

  /* If the file name has special constructs in it,
     call the corresponding file handler.  */
  handler = Ffind_file_name_handler (abspath1, Qfile_newer_than_file_p);
  if (NILP (handler))
    handler = Ffind_file_name_handler (abspath2, Qfile_newer_than_file_p);
  if (!NILP (handler))
    return call3 (handler, Qfile_newer_than_file_p, abspath1, abspath2);

  if (stat (XSTRING (abspath1)->data, &st) < 0)
    return Qnil;

  mtime1 = st.st_mtime;

  if (stat (XSTRING (abspath2)->data, &st) < 0)
    return Qt;

  return (mtime1 > st.st_mtime) ? Qt : Qnil;
}

#ifdef MSDOS
Lisp_Object Qfind_buffer_file_type;
#endif

DEFUN ("insert-file-contents", Finsert_file_contents, Sinsert_file_contents,
  1, 5, 0,
  "Insert contents of file FILENAME after point.\n\
Returns list of absolute file name and length of data inserted.\n\
If second argument VISIT is non-nil, the buffer's visited filename\n\
and last save file modtime are set, and it is marked unmodified.\n\
If visiting and the file does not exist, visiting is completed\n\
before the error is signaled.\n\n\
The optional third and fourth arguments BEG and END\n\
specify what portion of the file to insert.\n\
If VISIT is non-nil, BEG and END must be nil.\n\
If optional fifth argument REPLACE is non-nil,\n\
it means replace the current buffer contents (in the accessible portion)\n\
with the file contents.  This is better than simply deleting and inserting\n\
the whole thing because (1) it preserves some marker positions\n\
and (2) it puts less data in the undo list.")
  (filename, visit, beg, end, replace)
     Lisp_Object filename, visit, beg, end, replace;
{
  struct stat st;
  register int fd;
  register int inserted = 0;
  register int how_much;
  int count = specpdl_ptr - specpdl;
  struct gcpro gcpro1, gcpro2, gcpro3;
  Lisp_Object handler, val, insval;
  Lisp_Object p;
  int total;

  val = Qnil;
  p = Qnil;

  GCPRO3 (filename, val, p);
  if (!NILP (current_buffer->read_only))
    Fbarf_if_buffer_read_only();

  CHECK_STRING (filename, 0);
  filename = Fexpand_file_name (filename, Qnil);

  /* If the file name has special constructs in it,
     call the corresponding file handler.  */
  handler = Ffind_file_name_handler (filename, Qinsert_file_contents);
  if (!NILP (handler))
    {
      val = call6 (handler, Qinsert_file_contents, filename,
		   visit, beg, end, replace);
      goto handled;
    }

  fd = -1;

#ifndef APOLLO
  if (stat (XSTRING (filename)->data, &st) < 0)
#else
  if ((fd = open (XSTRING (filename)->data, O_RDONLY)) < 0
      || fstat (fd, &st) < 0)
#endif /* not APOLLO */
    {
      if (fd >= 0) close (fd);
    badopen:
      if (NILP (visit))
	report_file_error ("Opening input file", Fcons (filename, Qnil));
      st.st_mtime = -1;
      how_much = 0;
      goto notfound;
    }

#ifdef S_IFREG
  /* This code will need to be changed in order to work on named
     pipes, and it's probably just not worth it.  So we should at
     least signal an error.  */
  if (!S_ISREG (st.st_mode))
    Fsignal (Qfile_error,
	     Fcons (build_string ("not a regular file"),
		    Fcons (filename, Qnil)));
#endif

  if (fd < 0)
    if ((fd = open (XSTRING (filename)->data, O_RDONLY)) < 0)
      goto badopen;

  /* Replacement should preserve point as it preserves markers.  */
  if (!NILP (replace))
    record_unwind_protect (restore_point_unwind, Fpoint_marker ());

  record_unwind_protect (close_file_unwind, make_number (fd));

  /* Supposedly happens on VMS.  */
  if (st.st_size < 0)
    error ("File size is negative");

  if (!NILP (beg) || !NILP (end))
    if (!NILP (visit))
      error ("Attempt to visit less than an entire file");

  if (!NILP (beg))
    CHECK_NUMBER (beg, 0);
  else
    XFASTINT (beg) = 0;

  if (!NILP (end))
    CHECK_NUMBER (end, 0);
  else
    {
      XSETINT (end, st.st_size);
      if (XINT (end) != st.st_size)
	error ("maximum buffer size exceeded");
    }

  /* If requested, replace the accessible part of the buffer
     with the file contents.  Avoid replacing text at the
     beginning or end of the buffer that matches the file contents;
     that preserves markers pointing to the unchanged parts.  */
#if defined(MSDOS) || defined(VMS)
  /* On MSDOS, replace mode doesn't really work, except for binary files,
     and it's not worth supporting just for them.  */
  if (!NILP (replace))
    {
      replace = Qnil;
      XFASTINT (beg) = 0;
      XFASTINT (end) = st.st_size;
      del_range_1 (BEGV, ZV, 0);
    }
#else /* MSDOS */
  if (!NILP (replace))
    {
      unsigned char buffer[1 << 14];
      int same_at_start = BEGV;
      int same_at_end = ZV;
      int overlap;

      immediate_quit = 1;
      QUIT;
      /* Count how many chars at the start of the file
	 match the text at the beginning of the buffer.  */
      while (1)
	{
	  int nread, bufpos;

	  nread = read (fd, buffer, sizeof buffer);
	  if (nread < 0)
	    error ("IO error reading %s: %s",
		   XSTRING (filename)->data, err_str (errno));
	  else if (nread == 0)
	    break;
	  bufpos = 0;
	  while (bufpos < nread && same_at_start < ZV
		 && FETCH_CHAR (same_at_start) == buffer[bufpos])
	    same_at_start++, bufpos++;
	  /* If we found a discrepancy, stop the scan.
	     Otherwise loop around and scan the next bufferfull.  */
	  if (bufpos != nread)
	    break;
	}
      immediate_quit = 0;
      /* If the file matches the buffer completely,
	 there's no need to replace anything.  */
      if (same_at_start - BEGV == st.st_size)
	{
	  close (fd);
	  specpdl_ptr--;
	  /* Truncate the buffer to the size of the file.  */
	  del_range_1 (same_at_start, same_at_end, 0);
	  goto handled;
	}
      immediate_quit = 1;
      QUIT;
      /* Count how many chars at the end of the file
	 match the text at the end of the buffer.  */
      while (1)
	{
	  int total_read, nread, bufpos, curpos, trial;

	  /* At what file position are we now scanning?  */
	  curpos = st.st_size - (ZV - same_at_end);
	  /* If the entire file matches the buffer tail, stop the scan.  */
	  if (curpos == 0)
	    break;
	  /* How much can we scan in the next step?  */
	  trial = min (curpos, sizeof buffer);
	  if (lseek (fd, curpos - trial, 0) < 0)
	    report_file_error ("Setting file position",
			       Fcons (filename, Qnil));

	  total_read = 0;
	  while (total_read < trial)
	    {
	      nread = read (fd, buffer + total_read, trial - total_read);
	      if (nread <= 0)
		error ("IO error reading %s: %s",
		       XSTRING (filename)->data, err_str (errno));
	      total_read += nread;
	    }
	  /* Scan this bufferfull from the end, comparing with
	     the Emacs buffer.  */
	  bufpos = total_read;
	  /* Compare with same_at_start to avoid counting some buffer text
	     as matching both at the file's beginning and at the end.  */
	  while (bufpos > 0 && same_at_end > same_at_start
		 && FETCH_CHAR (same_at_end - 1) == buffer[bufpos - 1])
	    same_at_end--, bufpos--;
	  /* If we found a discrepancy, stop the scan.
	     Otherwise loop around and scan the preceding bufferfull.  */
	  if (bufpos != 0)
	    break;
	}
      immediate_quit = 0;

      /* Don't try to reuse the same piece of text twice.  */
      overlap = same_at_start - BEGV - (same_at_end + st.st_size - ZV);
      if (overlap > 0)
	same_at_end += overlap;

      /* Arrange to read only the nonmatching middle part of the file.  */
      XFASTINT (beg) = same_at_start - BEGV;
      XFASTINT (end) = st.st_size - (ZV - same_at_end);

      del_range_1 (same_at_start, same_at_end, 0);
      /* Insert from the file at the proper position.  */
      SET_PT (same_at_start);
    }
#endif /* MSDOS */

  total = XINT (end) - XINT (beg);

  {
    register Lisp_Object temp;

    /* Make sure point-max won't overflow after this insertion.  */
    XSET (temp, Lisp_Int, total);
    if (total != XINT (temp))
      error ("maximum buffer size exceeded");
  }

  if (NILP (visit) && total > 0)
    prepare_to_modify_buffer (point, point);

  move_gap (point);
  if (GAP_SIZE < total)
    make_gap (total - GAP_SIZE);

  if (XINT (beg) != 0 || !NILP (replace))
    {
      if (lseek (fd, XINT (beg), 0) < 0)
	report_file_error ("Setting file position", Fcons (filename, Qnil));
    }

  how_much = 0;
  while (inserted < total)
    {
      int try = min (total - inserted, 64 << 10);
      int this;

      /* Allow quitting out of the actual I/O.  */
      immediate_quit = 1;
      QUIT;
      this = read (fd, &FETCH_CHAR (point + inserted - 1) + 1, try);
      immediate_quit = 0;

      if (this <= 0)
	{
	  how_much = this;
	  break;
	}

      GPT += this;
      GAP_SIZE -= this;
      ZV += this;
      Z += this;
      inserted += this;
    }

#ifdef MSDOS
  /* Demacs 1.1.1 91/10/16 HIRANO Satoshi, MW July 1993 */
  /* Determine file type from name and remove LFs from CR-LFs if the file
     is deemed to be a text file.  */
  {
    struct gcpro gcpro1;
    Lisp_Object code;
    code = Qnil;
    GCPRO1 (filename);
    current_buffer->buffer_file_type
      = call1 (Qfind_buffer_file_type, filename);
    UNGCPRO;
    if (NILP (current_buffer->buffer_file_type))
      {
	int reduced_size
	  = inserted - crlf_to_lf (inserted, &FETCH_CHAR (point - 1) + 1);
	ZV -= reduced_size;
	Z -= reduced_size;
	GPT -= reduced_size;
	GAP_SIZE += reduced_size;
	inserted -= reduced_size;
      }
  }
#endif

  if (inserted > 0)
    {
      record_insert (point, inserted);

      /* Only defined if Emacs is compiled with USE_TEXT_PROPERTIES */
      offset_intervals (current_buffer, point, inserted);
      MODIFF++;
    }

  close (fd);

  /* Discard the unwind protect for closing the file.  */
  specpdl_ptr--;

  if (how_much < 0)
    error ("IO error reading %s: %s",
	   XSTRING (filename)->data, err_str (errno));

 notfound:
 handled:

  if (!NILP (visit))
    {
      if (!EQ (current_buffer->undo_list, Qt))
	current_buffer->undo_list = Qnil;
#ifdef APOLLO
      stat (XSTRING (filename)->data, &st);
#endif

      if (NILP (handler))
	{
	  current_buffer->modtime = st.st_mtime;
	  current_buffer->filename = filename;
	}

      current_buffer->save_modified = MODIFF;
      current_buffer->auto_save_modified = MODIFF;
      XFASTINT (current_buffer->save_length) = Z - BEG;
#ifdef CLASH_DETECTION
      if (NILP (handler))
	{
	  if (!NILP (current_buffer->filename))
	    unlock_file (current_buffer->filename);
	  unlock_file (filename);
	}
#endif /* CLASH_DETECTION */
      /* If visiting nonexistent file, return nil.  */
      if (current_buffer->modtime == -1)
	report_file_error ("Opening input file", Fcons (filename, Qnil));
    }

  if (inserted > 0 && NILP (visit) && total > 0)
    signal_after_change (point, 0, inserted);
  
  if (inserted > 0)
    {
      p = Vafter_insert_file_functions;
      while (!NILP (p))
	{
	  insval = call1 (Fcar (p), make_number (inserted));
	  if (!NILP (insval))
	    {
	      CHECK_NUMBER (insval, 0);
	      inserted = XFASTINT (insval);
	    }
	  QUIT;
	  p = Fcdr (p);
	}
    }

  if (NILP (val))
    val = Fcons (filename,
		 Fcons (make_number (inserted),
			Qnil));

  RETURN_UNGCPRO (unbind_to (count, val));
}

static Lisp_Object build_annotations ();

/* If build_annotations switched buffers, switch back to BUF.
   Kill the temporary buffer that was selected in the meantime.  */

static Lisp_Object 
build_annotations_unwind (buf)
     Lisp_Object buf;
{
  Lisp_Object tembuf;

  if (XBUFFER (buf) == current_buffer)
    return Qnil;
  tembuf = Fcurrent_buffer ();
  Fset_buffer (buf);
  Fkill_buffer (tembuf);
  return Qnil;
}

DEFUN ("write-region", Fwrite_region, Swrite_region, 3, 5,
  "r\nFWrite region to file: ",
  "Write current region into specified file.\n\
When called from a program, takes three arguments:\n\
START, END and FILENAME.  START and END are buffer positions.\n\
Optional fourth argument APPEND if non-nil means\n\
  append to existing file contents (if any).\n\
Optional fifth argument VISIT if t means\n\
  set the last-save-file-modtime of buffer to this file's modtime\n\
  and mark buffer not modified.\n\
If VISIT is a string, it is a second file name;\n\
  the output goes to FILENAME, but the buffer is marked as visiting VISIT.\n\
  VISIT is also the file name to lock and unlock for clash detection.\n\
If VISIT is neither t nor nil nor a string,\n\
  that means do not print the \"Wrote file\" message.\n\
Kludgy feature: if START is a string, then that string is written\n\
to the file, instead of any buffer contents, and END is ignored.")
  (start, end, filename, append, visit)
     Lisp_Object start, end, filename, append, visit;
{
  register int desc;
  int failure;
  int save_errno;
#ifdef VMS
  int save_vaxc$errno;
#endif
  unsigned char *fn;
  struct stat st;
  int tem;
  int count = specpdl_ptr - specpdl;
  int count1;
#ifdef VMS
  unsigned char *fname = 0;	/* If non-0, original filename (must rename) */
#endif /* VMS */
  Lisp_Object handler;
  Lisp_Object visit_file;
  Lisp_Object annotations;
  int visiting, quietly;
  struct gcpro gcpro1, gcpro2, gcpro3, gcpro4;
  struct buffer *given_buffer;
#ifdef MSDOS
  int buffer_file_type
    = NILP (current_buffer->buffer_file_type) ? O_TEXT : O_BINARY;
#endif

  if (!NILP (start) && !STRINGP (start))
    validate_region (&start, &end);

  GCPRO2 (filename, visit);
  filename = Fexpand_file_name (filename, Qnil);
  if (STRINGP (visit))
    visit_file = Fexpand_file_name (visit, Qnil);
  else
    visit_file = filename;
  UNGCPRO;

  visiting = (EQ (visit, Qt) || STRINGP (visit));
  quietly = !NILP (visit);

  annotations = Qnil;

  GCPRO4 (start, filename, annotations, visit_file);

  /* If the file name has special constructs in it,
     call the corresponding file handler.  */
  handler = Ffind_file_name_handler (filename, Qwrite_region);
  /* If FILENAME has no handler, see if VISIT has one.  */
  if (NILP (handler) && XTYPE (visit) == Lisp_String)
    handler = Ffind_file_name_handler (visit, Qwrite_region);    

  if (!NILP (handler))
    {
      Lisp_Object val;
      val = call6 (handler, Qwrite_region, start, end,
		   filename, append, visit);

      if (visiting)
	{
	  current_buffer->save_modified = MODIFF;
	  XFASTINT (current_buffer->save_length) = Z - BEG;
	  current_buffer->filename = visit_file;
	}
      UNGCPRO;
      return val;
    }

  /* Special kludge to simplify auto-saving.  */
  if (NILP (start))
    {
      XFASTINT (start) = BEG;
      XFASTINT (end) = Z;
    }

  record_unwind_protect (build_annotations_unwind, Fcurrent_buffer ());
  count1 = specpdl_ptr - specpdl;

  given_buffer = current_buffer;
  annotations = build_annotations (start, end);
  if (current_buffer != given_buffer)
    {
      start = BEGV;
      end = ZV;
    }

#ifdef CLASH_DETECTION
  if (!auto_saving)
    lock_file (visit_file);
#endif /* CLASH_DETECTION */

  fn = XSTRING (filename)->data;
  desc = -1;
  if (!NILP (append))
#ifdef MSDOS
    desc = open (fn, O_WRONLY | buffer_file_type);
#else
    desc = open (fn, O_WRONLY);
#endif

  if (desc < 0)
#ifdef VMS
    if (auto_saving)	/* Overwrite any previous version of autosave file */
      {
	vms_truncate (fn);	/* if fn exists, truncate to zero length */
	desc = open (fn, O_RDWR);
	if (desc < 0)
	  desc = creat_copy_attrs (STRINGP (current_buffer->filename)
				   ? XSTRING (current_buffer->filename)->data : 0,
				   fn);
      }
    else		/* Write to temporary name and rename if no errors */
      {
	Lisp_Object temp_name;
	temp_name = Ffile_name_directory (filename);

	if (!NILP (temp_name))
	  {
	    temp_name = Fmake_temp_name (concat2 (temp_name,
						  build_string ("$$SAVE$$")));
	    fname = XSTRING (filename)->data;
	    fn = XSTRING (temp_name)->data;
	    desc = creat_copy_attrs (fname, fn);
	    if (desc < 0)
	      {
		/* If we can't open the temporary file, try creating a new
		   version of the original file.  VMS "creat" creates a
		   new version rather than truncating an existing file. */
		fn = fname;
		fname = 0;
		desc = creat (fn, 0666);
#if 0 /* This can clobber an existing file and fail to replace it,
	 if the user runs out of space.  */
		if (desc < 0)
		  {
		    /* We can't make a new version;
		       try to truncate and rewrite existing version if any.  */
		    vms_truncate (fn);
		    desc = open (fn, O_RDWR);
		  }
#endif
	      }
	  }
	else
	  desc = creat (fn, 0666);
      }
#else /* not VMS */
#ifdef MSDOS
  desc = open (fn, 
	       O_WRONLY | O_TRUNC | O_CREAT | buffer_file_type, 
	       S_IREAD | S_IWRITE);
#else /* not MSDOS */
  desc = creat (fn, auto_saving ? auto_save_mode_bits : 0666);
#endif /* not MSDOS */
#endif /* not VMS */

  UNGCPRO;

  if (desc < 0)
    {
#ifdef CLASH_DETECTION
      save_errno = errno;
#ifdef VMS
      save_vaxc$errno = vaxc$errno;
#endif
      if (!auto_saving) unlock_file (visit_file);
      errno = save_errno;
#ifdef VMS
      vaxc$errno = save_vaxc$errno;
#endif
#endif /* CLASH_DETECTION */
      report_file_error ("Opening output file", Fcons (filename, Qnil));
    }

  record_unwind_protect (close_file_unwind, make_number (desc));

  if (!NILP (append))
    if (lseek (desc, 0, 2) < 0)
      {
#ifdef CLASH_DETECTION
	if (!auto_saving) unlock_file (visit_file);
#endif /* CLASH_DETECTION */
	report_file_error ("Lseek error", Fcons (filename, Qnil));
      }

#ifdef VMS
/*
 * Kludge Warning: The VMS C RTL likes to insert carriage returns
 * if we do writes that don't end with a carriage return. Furthermore
 * it cannot handle writes of more then 16K. The modified
 * version of "sys_write" in SYSDEP.C (see comment there) copes with
 * this EXCEPT for the last record (iff it doesn't end with a carriage
 * return). This implies that if your buffer doesn't end with a carriage
 * return, you get one free... tough. However it also means that if
 * we make two calls to sys_write (a la the following code) you can
 * get one at the gap as well. The easiest way to fix this (honest)
 * is to move the gap to the next newline (or the end of the buffer).
 * If the file has fixed-length records, it is essential to move the gap
 * to a record boundary.  The easiest way to do this is to just move the
 * gap all the way to the end of the buffer.
 * Thus this change.
 *
 * Yech!
 */
#if 0
  if (GPT > BEG && GPT_ADDR[-1] != '\n')
    move_gap (find_next_newline (GPT, 1));
#endif
  if (GPT > BEG && GPT < Z)
    move_gap (Z);
#endif /* VMS */

  failure = 0;
  immediate_quit = 1;

  if (STRINGP (start))
    {
      failure = 0 > a_write (desc, XSTRING (start)->data,
			     XSTRING (start)->size, 0, &annotations);
      save_errno = errno;
#ifdef VMS
      save_vaxc$errno = vaxc$errno;
#endif
    }
  else if (XINT (start) != XINT (end))
    {
      int nwritten = 0;
      if (XINT (start) < GPT)
	{
	  register int end1 = XINT (end);
	  tem = XINT (start);
	  failure = 0 > a_write (desc, &FETCH_CHAR (tem),
				 min (GPT, end1) - tem, tem, &annotations);
	  nwritten += min (GPT, end1) - tem;
	  save_errno = errno;
#ifdef VMS
	  save_vaxc$errno = vaxc$errno;
#endif
	}

      if (XINT (end) > GPT && !failure)
	{
	  tem = XINT (start);
	  tem = max (tem, GPT);
	  failure = 0 > a_write (desc, &FETCH_CHAR (tem), XINT (end) - tem,
				 tem, &annotations);
	  nwritten += XINT (end) - tem;
	  save_errno = errno;
#ifdef VMS
	  save_vaxc$errno = vaxc$errno;
#endif
	}

      if (nwritten == 0)
	{
	  /* If file was empty, still need to write the annotations */
	  failure = 0 > a_write (desc, "", 0, XINT (start), &annotations);
	  save_errno = errno;
#ifdef VMS
	  save_vaxc$errno = vaxc$errno;
#endif
	}
    }

  immediate_quit = 0;

#ifdef HAVE_FSYNC
  /* Note fsync appears to change the modtime on BSD4.2 (both vax and sun).
     Disk full in NFS may be reported here.  */
  /* mib says that closing the file will try to write as fast as NFS can do
     it, and that means the fsync here is not crucial for autosave files.  */
#ifdef VMS
  if (!auto_saving && fsync (desc) < 0)
    failure = 1, save_errno = errno, save_vaxc$errno = vaxc$errno;
#else
  if (!auto_saving && fsync (desc) < 0)
    failure = 1, save_errno = errno;
#endif
#endif

  /* Spurious "file has changed on disk" warnings have been 
     observed on Suns as well.
     It seems that `close' can change the modtime, under nfs.

     (This has supposedly been fixed in Sunos 4,
     but who knows about all the other machines with NFS?)  */
#if 0

  /* On VMS and APOLLO, must do the stat after the close
     since closing changes the modtime.  */
#ifndef VMS
#ifndef APOLLO
  /* Recall that #if defined does not work on VMS.  */
#define FOO
  fstat (desc, &st);
#endif
#endif
#endif

  /* NFS can report a write failure now.  */
#ifdef VMS
  if (close (desc) < 0)
    failure = 1, save_errno = errno, save_vaxc$errno = vaxc$errno;
#else
  if (close (desc) < 0)
    failure = 1, save_errno = errno;
#endif

#ifdef VMS
  /* If we wrote to a temporary name and had no errors, rename to real name. */
  if (fname)
    {
#ifdef VMS
      if (!failure)
	failure = (rename (fn, fname) != 0),
	  save_errno = errno, save_vaxc$errno = vaxc$errno;
#else
      if (!failure)
	failure = (rename (fn, fname) != 0), save_errno = errno;
#endif
      fn = fname;
    }
#endif /* VMS */

#ifndef FOO
  stat (fn, &st);
#endif
  /* Discard the unwind protect for close_file_unwind.  */
  specpdl_ptr = specpdl + count1;
  /* Restore the original current buffer.  */
  visit_file = unbind_to (count, visit_file);

#ifdef CLASH_DETECTION
  if (!auto_saving)
    unlock_file (visit_file);
#endif /* CLASH_DETECTION */

  /* Do this before reporting IO error
     to avoid a "file has changed on disk" warning on
     next attempt to save.  */
  if (visiting)
    current_buffer->modtime = st.st_mtime;

  if (failure)
#ifndef VMS
    error ("IO error writing %s: %s", fn, strerror (save_errno));
#else
    error ("IO error writing %s: %s", fn, strerror (save_errno,
						    save_vaxc$errno));
#endif

  if (visiting)
    {
      current_buffer->save_modified = MODIFF;
      XFASTINT (current_buffer->save_length) = Z - BEG;
      current_buffer->filename = visit_file;
      update_mode_lines++;
    }
  else if (quietly)
    return Qnil;

  if (!auto_saving)
    message ("Wrote %s", XSTRING (visit_file)->data);

  return Qnil;
}

Lisp_Object merge ();

DEFUN ("car-less-than-car", Fcar_less_than_car, Scar_less_than_car, 2, 2, 0,
  "Return t if (car A) is numerically less than (car B).")
  (a, b)
     Lisp_Object a, b;
{
  return Flss (Fcar (a), Fcar (b));
}

/* Build the complete list of annotations appropriate for writing out
   the text between START and END, by calling all the functions in
   write-region-annotate-functions and merging the lists they return.
   If one of these functions switches to a different buffer, we assume
   that buffer contains altered text.  Therefore, the caller must
   make sure to restore the current buffer in all cases,
   as save-excursion would do.  */

static Lisp_Object
build_annotations (start, end)
     Lisp_Object start, end;
{
  Lisp_Object annotations;
  Lisp_Object p, res;
  struct gcpro gcpro1, gcpro2;

  annotations = Qnil;
  p = Vwrite_region_annotate_functions;
  GCPRO2 (annotations, p);
  while (!NILP (p))
    {
      struct buffer *given_buffer = current_buffer;
      Vwrite_region_annotations_so_far = annotations;
      res = call2 (Fcar (p), start, end);
      /* If the function makes a different buffer current,
	 assume that means this buffer contains altered text to be output.
	 Reset START and END from the buffer bounds
	 and discard all previous annotations because they should have
	 been dealt with by this function.  */
      if (current_buffer != given_buffer)
	{
	  start = BEGV;
	  end = ZV;
	  annotations = Qnil;
	}
      Flength (res);   /* Check basic validity of return value */
      annotations = merge (annotations, res, Qcar_less_than_car);
      p = Fcdr (p);
    }
  UNGCPRO;
  return annotations;
}

/* Write to descriptor DESC the LEN characters starting at ADDR,
   assuming they start at position POS in the buffer.
   Intersperse with them the annotations from *ANNOT
   (those which fall within the range of positions POS to POS + LEN),
   each at its appropriate position.

   Modify *ANNOT by discarding elements as we output them.
   The return value is negative in case of system call failure.  */

int
a_write (desc, addr, len, pos, annot)
     int desc;
     register char *addr;
     register int len;
     int pos;
     Lisp_Object *annot;
{
  Lisp_Object tem;
  int nextpos;
  int lastpos = pos + len;

  while (NILP (*annot) || CONSP (*annot))
    {
      tem = Fcar_safe (Fcar (*annot));
      if (INTEGERP (tem) && XINT (tem) >= pos && XFASTINT (tem) <= lastpos)
	nextpos = XFASTINT (tem);
      else
	return e_write (desc, addr, lastpos - pos);
      if (nextpos > pos)
	{
	  if (0 > e_write (desc, addr, nextpos - pos))
	    return -1;
	  addr += nextpos - pos;
	  pos = nextpos;
	}
      tem = Fcdr (Fcar (*annot));
      if (STRINGP (tem))
	{
	  if (0 > e_write (desc, XSTRING (tem)->data, XSTRING (tem)->size))
	    return -1;
	}
      *annot = Fcdr (*annot);
    }
}

int
e_write (desc, addr, len)
     int desc;
     register char *addr;
     register int len;
{
  char buf[16 * 1024];
  register char *p, *end;

  if (!EQ (current_buffer->selective_display, Qt))
    return write (desc, addr, len) - len;
  else
    {
      p = buf;
      end = p + sizeof buf;
      while (len--)
	{
	  if (p == end)
	    {
	      if (write (desc, buf, sizeof buf) != sizeof buf)
		return -1;
	      p = buf;
	    }
	  *p = *addr++;
	  if (*p++ == '\015')
	    p[-1] = '\n';
	}
      if (p != buf)
	if (write (desc, buf, p - buf) != p - buf)
	  return -1;
    }
  return 0;
}

DEFUN ("verify-visited-file-modtime", Fverify_visited_file_modtime,
  Sverify_visited_file_modtime, 1, 1, 0,
  "Return t if last mod time of BUF's visited file matches what BUF records.\n\
This means that the file has not been changed since it was visited or saved.")
  (buf)
     Lisp_Object buf;
{
  struct buffer *b;
  struct stat st;
  Lisp_Object handler;

  CHECK_BUFFER (buf, 0);
  b = XBUFFER (buf);

  if (XTYPE (b->filename) != Lisp_String) return Qt;
  if (b->modtime == 0) return Qt;

  /* If the file name has special constructs in it,
     call the corresponding file handler.  */
  handler = Ffind_file_name_handler (b->filename,
				     Qverify_visited_file_modtime);
  if (!NILP (handler))
    return call2 (handler, Qverify_visited_file_modtime, buf);

  if (stat (XSTRING (b->filename)->data, &st) < 0)
    {
      /* If the file doesn't exist now and didn't exist before,
	 we say that it isn't modified, provided the error is a tame one.  */
      if (errno == ENOENT || errno == EACCES || errno == ENOTDIR)
	st.st_mtime = -1;
      else
	st.st_mtime = 0;
    }
  if (st.st_mtime == b->modtime
      /* If both are positive, accept them if they are off by one second.  */
      || (st.st_mtime > 0 && b->modtime > 0
	  && (st.st_mtime == b->modtime + 1
	      || st.st_mtime == b->modtime - 1)))
    return Qt;
  return Qnil;
}

DEFUN ("clear-visited-file-modtime", Fclear_visited_file_modtime,
  Sclear_visited_file_modtime, 0, 0, 0,
  "Clear out records of last mod time of visited file.\n\
Next attempt to save will certainly not complain of a discrepancy.")
  ()
{
  current_buffer->modtime = 0;
  return Qnil;
}

DEFUN ("visited-file-modtime", Fvisited_file_modtime,
  Svisited_file_modtime, 0, 0, 0,
  "Return the current buffer's recorded visited file modification time.\n\
The value is a list of the form (HIGH . LOW), like the time values\n\
that `file-attributes' returns.")
  ()
{
  return long_to_cons (current_buffer->modtime);
}

DEFUN ("set-visited-file-modtime", Fset_visited_file_modtime,
  Sset_visited_file_modtime, 0, 1, 0,
  "Update buffer's recorded modification time from the visited file's time.\n\
Useful if the buffer was not read from the file normally\n\
or if the file itself has been changed for some known benign reason.\n\
An argument specifies the modification time value to use\n\
\(instead of that of the visited file), in the form of a list\n\
\(HIGH . LOW) or (HIGH LOW).")
  (time_list)
     Lisp_Object time_list;
{
  if (!NILP (time_list))
    current_buffer->modtime = cons_to_long (time_list);
  else
    {
      register Lisp_Object filename;
      struct stat st;
      Lisp_Object handler;

      filename = Fexpand_file_name (current_buffer->filename, Qnil);

      /* If the file name has special constructs in it,
	 call the corresponding file handler.  */
      handler = Ffind_file_name_handler (filename, Qset_visited_file_modtime);
      if (!NILP (handler))
	/* The handler can find the file name the same way we did.  */
	return call2 (handler, Qset_visited_file_modtime, Qnil);
      else if (stat (XSTRING (filename)->data, &st) >= 0)
	current_buffer->modtime = st.st_mtime;
    }

  return Qnil;
}

Lisp_Object
auto_save_error ()
{
  ring_bell ();
  message ("Autosaving...error for %s", XSTRING (current_buffer->name)->data);
  Fsleep_for (make_number (1), Qnil);
  message ("Autosaving...error!for %s", XSTRING (current_buffer->name)->data);
  Fsleep_for (make_number (1), Qnil);
  message ("Autosaving...error for %s", XSTRING (current_buffer->name)->data);
  Fsleep_for (make_number (1), Qnil);
  return Qnil;
}

Lisp_Object
auto_save_1 ()
{
  unsigned char *fn;
  struct stat st;

  /* Get visited file's mode to become the auto save file's mode.  */
  if (stat (XSTRING (current_buffer->filename)->data, &st) >= 0)
    /* But make sure we can overwrite it later!  */
    auto_save_mode_bits = st.st_mode | 0600;
  else
    auto_save_mode_bits = 0666;

  return
    Fwrite_region (Qnil, Qnil,
		   current_buffer->auto_save_file_name,
		   Qnil, Qlambda);
}

static Lisp_Object
do_auto_save_unwind (desc)  /* used as unwind-protect function */
     Lisp_Object desc;
{
  close (XINT (desc));
  return Qnil;
}

DEFUN ("do-auto-save", Fdo_auto_save, Sdo_auto_save, 0, 2, "",
  "Auto-save all buffers that need it.\n\
This is all buffers that have auto-saving enabled\n\
and are changed since last auto-saved.\n\
Auto-saving writes the buffer into a file\n\
so that your editing is not lost if the system crashes.\n\
This file is not the file you visited; that changes only when you save.\n\
Normally we run the normal hook `auto-save-hook' before saving.\n\n\
Non-nil first argument means do not print any message if successful.\n\
Non-nil second argument means save only current buffer.")
  (no_message, current_only)
     Lisp_Object no_message, current_only;
{
  struct buffer *old = current_buffer, *b;
  Lisp_Object tail, buf;
  int auto_saved = 0;
  char *omessage = echo_area_glyphs;
  int omessage_length = echo_area_glyphs_length;
  extern int minibuf_level;
  int do_handled_files;
  Lisp_Object oquit;
  int listdesc;
  int count = specpdl_ptr - specpdl;
  int *ptr;

  /* Ordinarily don't quit within this function,
     but don't make it impossible to quit (in case we get hung in I/O).  */
  oquit = Vquit_flag;
  Vquit_flag = Qnil;

  /* No GCPRO needed, because (when it matters) all Lisp_Object variables
     point to non-strings reached from Vbuffer_alist.  */

  auto_saving = 1;
  if (minibuf_level)
    no_message = Qt;

  if (!NILP (Vrun_hooks))
    call1 (Vrun_hooks, intern ("auto-save-hook"));

  if (STRINGP (Vauto_save_list_file_name))
    {
#ifdef MSDOS
      listdesc = open (XSTRING (Vauto_save_list_file_name)->data, 
		       O_WRONLY | O_TRUNC | O_CREAT | O_TEXT,
		       S_IREAD | S_IWRITE);
#else /* not MSDOS */
      listdesc = creat (XSTRING (Vauto_save_list_file_name)->data, 0666);
#endif /* not MSDOS */
    }
  else
    listdesc = -1;
  
  /* Arrange to close that file whether or not we get an error.  */
  if (listdesc >= 0)
    record_unwind_protect (do_auto_save_unwind, make_number (listdesc));

  /* First, save all files which don't have handlers.  If Emacs is
     crashing, the handlers may tweak what is causing Emacs to crash
     in the first place, and it would be a shame if Emacs failed to
     autosave perfectly ordinary files because it couldn't handle some
     ange-ftp'd file.  */
  for (do_handled_files = 0; do_handled_files < 2; do_handled_files++)
    for (tail = Vbuffer_alist; XGCTYPE (tail) == Lisp_Cons;
	 tail = XCONS (tail)->cdr)
      {
	buf = XCONS (XCONS (tail)->car)->cdr;
	b = XBUFFER (buf);
      
	/* Record all the buffers that have auto save mode
	   in the special file that lists them.  */
	if (XTYPE (b->auto_save_file_name) == Lisp_String
	    && listdesc >= 0 && do_handled_files == 0)
	  {
	    write (listdesc, XSTRING (b->auto_save_file_name)->data,
		   XSTRING (b->auto_save_file_name)->size);
	    write (listdesc, "\n", 1);
	  }

	if (!NILP (current_only)
	    && b != current_buffer)
	  continue;

	/* Check for auto save enabled
	   and file changed since last auto save
	   and file changed since last real save.  */
	if (XTYPE (b->auto_save_file_name) == Lisp_String
	    && b->save_modified < BUF_MODIFF (b)
	    && b->auto_save_modified < BUF_MODIFF (b)
	    /* -1 means we've turned off autosaving for a while--see below.  */
	    && XINT (b->save_length) >= 0
	    && (do_handled_files
		|| NILP (Ffind_file_name_handler (b->auto_save_file_name,
						  Qwrite_region))))
	  {
	    EMACS_TIME before_time, after_time;

	    EMACS_GET_TIME (before_time);

	    /* If we had a failure, don't try again for 20 minutes.  */
	    if (b->auto_save_failure_time >= 0
		&& EMACS_SECS (before_time) - b->auto_save_failure_time < 1200)
	      continue;

	    if ((XFASTINT (b->save_length) * 10
		 > (BUF_Z (b) - BUF_BEG (b)) * 13)
		/* A short file is likely to change a large fraction;
		   spare the user annoying messages.  */
		&& XFASTINT (b->save_length) > 5000
		/* These messages are frequent and annoying for `*mail*'.  */
		&& !EQ (b->filename, Qnil)
		&& NILP (no_message))
	      {
		/* It has shrunk too much; turn off auto-saving here.  */
		message ("Buffer %s has shrunk a lot; auto save turned off there",
			 XSTRING (b->name)->data);
		/* Turn off auto-saving until there's a real save,
		   and prevent any more warnings.  */
		XSET (b->save_length, Lisp_Int, -1);
		Fsleep_for (make_number (1), Qnil);
		continue;
	      }
	    set_buffer_internal (b);
	    if (!auto_saved && NILP (no_message))
	      message1 ("Auto-saving...");
	    internal_condition_case (auto_save_1, Qt, auto_save_error);
	    auto_saved++;
	    b->auto_save_modified = BUF_MODIFF (b);
	    XFASTINT (current_buffer->save_length) = Z - BEG;
	    set_buffer_internal (old);

	    EMACS_GET_TIME (after_time);

	    /* If auto-save took more than 60 seconds,
	       assume it was an NFS failure that got a timeout.  */
	    if (EMACS_SECS (after_time) - EMACS_SECS (before_time) > 60)
	      b->auto_save_failure_time = EMACS_SECS (after_time);
	  }
      }

  /* Prevent another auto save till enough input events come in.  */
  record_auto_save ();

  if (auto_saved && NILP (no_message))
    {
      if (omessage)
	message2 (omessage, omessage_length);
      else
	message1 ("Auto-saving...done");
    }

  Vquit_flag = oquit;

  auto_saving = 0;
  unbind_to (count, Qnil);
  return Qnil;
}

DEFUN ("set-buffer-auto-saved", Fset_buffer_auto_saved,
  Sset_buffer_auto_saved, 0, 0, 0,
  "Mark current buffer as auto-saved with its current text.\n\
No auto-save file will be written until the buffer changes again.")
  ()
{
  current_buffer->auto_save_modified = MODIFF;
  XFASTINT (current_buffer->save_length) = Z - BEG;
  current_buffer->auto_save_failure_time = -1;
  return Qnil;
}

DEFUN ("clear-buffer-auto-save-failure", Fclear_buffer_auto_save_failure,
  Sclear_buffer_auto_save_failure, 0, 0, 0,
  "Clear any record of a recent auto-save failure in the current buffer.")
  ()
{
  current_buffer->auto_save_failure_time = -1;
  return Qnil;
}

DEFUN ("recent-auto-save-p", Frecent_auto_save_p, Srecent_auto_save_p,
  0, 0, 0,
  "Return t if buffer has been auto-saved since last read in or saved.")
  ()
{
  return (current_buffer->save_modified < current_buffer->auto_save_modified) ? Qt : Qnil;
}

/* Reading and completing file names */
extern Lisp_Object Ffile_name_completion (), Ffile_name_all_completions ();

/* In the string VAL, change each $ to $$ and return the result.  */

static Lisp_Object
double_dollars (val)
     Lisp_Object val;
{
#ifndef VMS
  register unsigned char *old, *new;
  register int n;
  int osize, count;

  osize = XSTRING (val)->size;
  /* Quote "$" as "$$" to get it past substitute-in-file-name */
  for (n = osize, count = 0, old = XSTRING (val)->data; n > 0; n--)
    if (*old++ == '$') count++;
  if (count > 0)
    {
      old = XSTRING (val)->data;
      val = Fmake_string (make_number (osize + count), make_number (0));
      new = XSTRING (val)->data;
      for (n = osize; n > 0; n--)
	if (*old != '$')
	  *new++ = *old++;
	else
	  {
	    *new++ = '$';
	    *new++ = '$';
	    old++;
	  }
    }
#endif
  return val;
}

DEFUN ("read-file-name-internal", Fread_file_name_internal, Sread_file_name_internal,
  3, 3, 0,
  "Internal subroutine for read-file-name.  Do not call this.")
  (string, dir, action)
     Lisp_Object string, dir, action;
  /* action is nil for complete, t for return list of completions,
     lambda for verify final value */
{
  Lisp_Object name, specdir, realdir, val, orig_string;
  int changed;
  struct gcpro gcpro1, gcpro2, gcpro3, gcpro4;

  realdir = dir;
  name = string;
  orig_string = Qnil;
  specdir = Qnil;
  changed = 0;
  /* No need to protect ACTION--we only compare it with t and nil.  */
  GCPRO4 (string, realdir, name, specdir);

  if (XSTRING (string)->size == 0)
    {
      if (EQ (action, Qlambda))
	{
	  UNGCPRO;
	  return Qnil;
	}
    }
  else
    {
      orig_string = string;
      string = Fsubstitute_in_file_name (string);
      changed = NILP (Fstring_equal (string, orig_string));
      name = Ffile_name_nondirectory (string);
      val = Ffile_name_directory (string);
      if (! NILP (val))
	realdir = Fexpand_file_name (val, realdir);
    }

  if (NILP (action))
    {
      specdir = Ffile_name_directory (string);
      val = Ffile_name_completion (name, realdir);
      UNGCPRO;
      if (XTYPE (val) != Lisp_String)
	{
	  if (changed)
	    return double_dollars (string);
	  return val;
	}

      if (!NILP (specdir))
	val = concat2 (specdir, val);
#ifndef VMS
      return double_dollars (val);
#else /* not VMS */
      return val;
#endif /* not VMS */
    }
  UNGCPRO;

  if (EQ (action, Qt))
    return Ffile_name_all_completions (name, realdir);
  /* Only other case actually used is ACTION = lambda */
#ifdef VMS
  /* Supposedly this helps commands such as `cd' that read directory names,
     but can someone explain how it helps them? -- RMS */
  if (XSTRING (name)->size == 0)
    return Qt;
#endif /* VMS */
  return Ffile_exists_p (string);
}

DEFUN ("read-file-name", Fread_file_name, Sread_file_name, 1, 5, 0,
  "Read file name, prompting with PROMPT and completing in directory DIR.\n\
Value is not expanded---you must call `expand-file-name' yourself.\n\
Default name to DEFAULT if user enters a null string.\n\
 (If DEFAULT is omitted, the visited file name is used.)\n\
Fourth arg MUSTMATCH non-nil means require existing file's name.\n\
 Non-nil and non-t means also require confirmation after completion.\n\
Fifth arg INITIAL specifies text to start with.\n\
DIR defaults to current buffer's directory default.")
  (prompt, dir, defalt, mustmatch, initial)
     Lisp_Object prompt, dir, defalt, mustmatch, initial;
{
  Lisp_Object val, insdef, insdef1, tem;
  struct gcpro gcpro1, gcpro2;
  register char *homedir;
  int count;

  if (NILP (dir))
    dir = current_buffer->directory;
  if (NILP (defalt))
    defalt = current_buffer->filename;

  /* If dir starts with user's homedir, change that to ~. */
  homedir = (char *) egetenv ("HOME");
  if (homedir != 0
      && XTYPE (dir) == Lisp_String
      && !strncmp (homedir, XSTRING (dir)->data, strlen (homedir))
      && XSTRING (dir)->data[strlen (homedir)] == '/')
    {
      dir = make_string (XSTRING (dir)->data + strlen (homedir) - 1,
			 XSTRING (dir)->size - strlen (homedir) + 1);
      XSTRING (dir)->data[0] = '~';
    }

  if (insert_default_directory)
    {
      insdef = dir;
      if (!NILP (initial))
	{
	  Lisp_Object args[2], pos;

	  args[0] = insdef;
	  args[1] = initial;
	  insdef = Fconcat (2, args);
	  pos = make_number (XSTRING (double_dollars (dir))->size);
	  insdef1 = Fcons (double_dollars (insdef), pos);
	}
      else
	insdef1 = double_dollars (insdef);
    }
  else if (!NILP (initial))
    {
      insdef = initial;
      insdef1 = Fcons (double_dollars (insdef), 0);
    }
  else
    insdef = Qnil, insdef1 = Qnil;

#ifdef VMS
  count = specpdl_ptr - specpdl;
  specbind (intern ("completion-ignore-case"), Qt);
#endif

  GCPRO2 (insdef, defalt);
  val = Fcompleting_read (prompt, intern ("read-file-name-internal"),
			  dir, mustmatch, insdef1,
			  Qfile_name_history);

#ifdef VMS
  unbind_to (count, Qnil);
#endif

  UNGCPRO;
  if (NILP (val))
    error ("No file name specified");
  tem = Fstring_equal (val, insdef);
  if (!NILP (tem) && !NILP (defalt))
    return defalt;
  if (XSTRING (val)->size == 0 && NILP (insdef))
    {
      if (!NILP (defalt))
	return defalt;
      else
	error ("No default file name");
    }
  return Fsubstitute_in_file_name (val);
}

#if 0				/* Old version */
DEFUN ("read-file-name", Fread_file_name, Sread_file_name, 1, 5, 0,
  /* Don't confuse make-docfile by having two doc strings for this function.
     make-docfile does not pay attention to #if, for good reason!  */
  0)
  (prompt, dir, defalt, mustmatch, initial)
     Lisp_Object prompt, dir, defalt, mustmatch, initial;
{
  Lisp_Object val, insdef, tem;
  struct gcpro gcpro1, gcpro2;
  register char *homedir;
  int count;

  if (NILP (dir))
    dir = current_buffer->directory;
  if (NILP (defalt))
    defalt = current_buffer->filename;

  /* If dir starts with user's homedir, change that to ~. */
  homedir = (char *) egetenv ("HOME");
  if (homedir != 0
      && XTYPE (dir) == Lisp_String
      && !strncmp (homedir, XSTRING (dir)->data, strlen (homedir))
      && XSTRING (dir)->data[strlen (homedir)] == '/')
    {
      dir = make_string (XSTRING (dir)->data + strlen (homedir) - 1,
			 XSTRING (dir)->size - strlen (homedir) + 1);
      XSTRING (dir)->data[0] = '~';
    }

  if (!NILP (initial))
    insdef = initial;
  else if (insert_default_directory)
    insdef = dir;
  else
    insdef = build_string ("");

#ifdef VMS
  count = specpdl_ptr - specpdl;
  specbind (intern ("completion-ignore-case"), Qt);
#endif

  GCPRO2 (insdef, defalt);
  val = Fcompleting_read (prompt, intern ("read-file-name-internal"),
			  dir, mustmatch,
			  insert_default_directory ? insdef : Qnil,
			  Qfile_name_history);

#ifdef VMS
  unbind_to (count, Qnil);
#endif

  UNGCPRO;
  if (NILP (val))
    error ("No file name specified");
  tem = Fstring_equal (val, insdef);
  if (!NILP (tem) && !NILP (defalt))
    return defalt;
  return Fsubstitute_in_file_name (val);
}
#endif /* Old version */

syms_of_fileio ()
{
  Qexpand_file_name = intern ("expand-file-name");
  Qdirectory_file_name = intern ("directory-file-name");
  Qfile_name_directory = intern ("file-name-directory");
  Qfile_name_nondirectory = intern ("file-name-nondirectory");
  Qunhandled_file_name_directory = intern ("unhandled-file-name-directory");
  Qfile_name_as_directory = intern ("file-name-as-directory");
  Qcopy_file = intern ("copy-file");
  Qmake_directory_internal = intern ("make-directory-internal");
  Qdelete_directory = intern ("delete-directory");
  Qdelete_file = intern ("delete-file");
  Qrename_file = intern ("rename-file");
  Qadd_name_to_file = intern ("add-name-to-file");
  Qmake_symbolic_link = intern ("make-symbolic-link");
  Qfile_exists_p = intern ("file-exists-p");
  Qfile_executable_p = intern ("file-executable-p");
  Qfile_readable_p = intern ("file-readable-p");
  Qfile_symlink_p = intern ("file-symlink-p");
  Qfile_writable_p = intern ("file-writable-p");
  Qfile_directory_p = intern ("file-directory-p");
  Qfile_accessible_directory_p = intern ("file-accessible-directory-p");
  Qfile_modes = intern ("file-modes");
  Qset_file_modes = intern ("set-file-modes");
  Qfile_newer_than_file_p = intern ("file-newer-than-file-p");
  Qinsert_file_contents = intern ("insert-file-contents");
  Qwrite_region = intern ("write-region");
  Qverify_visited_file_modtime = intern ("verify-visited-file-modtime");
  Qset_visited_file_modtime = intern ("set-visited-file-modtime");

  staticpro (&Qexpand_file_name);
  staticpro (&Qdirectory_file_name);
  staticpro (&Qfile_name_directory);
  staticpro (&Qfile_name_nondirectory);
  staticpro (&Qunhandled_file_name_directory);
  staticpro (&Qfile_name_as_directory);
  staticpro (&Qcopy_file);
  staticpro (&Qmake_directory_internal);
  staticpro (&Qdelete_directory);
  staticpro (&Qdelete_file);
  staticpro (&Qrename_file);
  staticpro (&Qadd_name_to_file);
  staticpro (&Qmake_symbolic_link);
  staticpro (&Qfile_exists_p);
  staticpro (&Qfile_executable_p);
  staticpro (&Qfile_readable_p);
  staticpro (&Qfile_symlink_p);
  staticpro (&Qfile_writable_p);
  staticpro (&Qfile_directory_p);
  staticpro (&Qfile_accessible_directory_p);
  staticpro (&Qfile_modes);
  staticpro (&Qset_file_modes);
  staticpro (&Qfile_newer_than_file_p);
  staticpro (&Qinsert_file_contents);
  staticpro (&Qwrite_region);
  staticpro (&Qverify_visited_file_modtime);

  Qfile_name_history = intern ("file-name-history");
  Fset (Qfile_name_history, Qnil);
  staticpro (&Qfile_name_history);

  Qfile_error = intern ("file-error");
  staticpro (&Qfile_error);
  Qfile_already_exists = intern("file-already-exists");
  staticpro (&Qfile_already_exists);

#ifdef MSDOS
  Qfind_buffer_file_type = intern ("find-buffer-file-type");
  staticpro (&Qfind_buffer_file_type);
#endif

  Qcar_less_than_car = intern ("car-less-than-car");
  staticpro (&Qcar_less_than_car);

  Fput (Qfile_error, Qerror_conditions,
	Fcons (Qfile_error, Fcons (Qerror, Qnil)));
  Fput (Qfile_error, Qerror_message,
	build_string ("File error"));

  Fput (Qfile_already_exists, Qerror_conditions,
	Fcons (Qfile_already_exists,
	       Fcons (Qfile_error, Fcons (Qerror, Qnil))));
  Fput (Qfile_already_exists, Qerror_message,
	build_string ("File already exists"));

  DEFVAR_BOOL ("insert-default-directory", &insert_default_directory,
    "*Non-nil means when reading a filename start with default dir in minibuffer.");
  insert_default_directory = 1;

  DEFVAR_BOOL ("vms-stmlf-recfm", &vms_stmlf_recfm,
    "*Non-nil means write new files with record format `stmlf'.\n\
nil means use format `var'.  This variable is meaningful only on VMS.");
  vms_stmlf_recfm = 0;

  DEFVAR_LISP ("file-name-handler-alist", &Vfile_name_handler_alist,
    "*Alist of elements (REGEXP . HANDLER) for file names handled specially.\n\
If a file name matches REGEXP, then all I/O on that file is done by calling\n\
HANDLER.\n\
\n\
The first argument given to HANDLER is the name of the I/O primitive\n\
to be handled; the remaining arguments are the arguments that were\n\
passed to that primitive.  For example, if you do\n\
    (file-exists-p FILENAME)\n\
and FILENAME is handled by HANDLER, then HANDLER is called like this:\n\
    (funcall HANDLER 'file-exists-p FILENAME)\n\
The function `find-file-name-handler' checks this list for a handler\n\
for its argument.");
  Vfile_name_handler_alist = Qnil;

  DEFVAR_LISP ("after-insert-file-functions", &Vafter_insert_file_functions,
    "A list of functions to be called at the end of `insert-file-contents'.\n\
Each is passed one argument, the number of bytes inserted.  It should return\n\
the new byte count, and leave point the same.  If `insert-file-contents' is\n\
intercepted by a handler from `file-name-handler-alist', that handler is\n\
responsible for calling the after-insert-file-functions if appropriate.");
  Vafter_insert_file_functions = Qnil;

  DEFVAR_LISP ("write-region-annotate-functions", &Vwrite_region_annotate_functions,
    "A list of functions to be called at the start of `write-region'.\n\
Each is passed two arguments, START and END as for `write-region'.  It should\n\
return a list of pairs (POSITION . STRING) of strings to be effectively\n\
inserted at the specified positions of the file being written (1 means to\n\
insert before the first byte written).  The POSITIONs must be sorted into\n\
increasing order.  If there are several functions in the list, the several\n\
lists are merged destructively.");
  Vwrite_region_annotate_functions = Qnil;

  DEFVAR_LISP ("write-region-annotations-so-far",
	       &Vwrite_region_annotations_so_far,
    "When an annotation function is called, this holds the previous annotations.\n\
These are the annotations made by other annotation functions\n\
that were already called.  See also `write-region-annotate-functions'.");
  Vwrite_region_annotations_so_far = Qnil;

  DEFVAR_LISP ("inhibit-file-name-handlers", &Vinhibit_file_name_handlers,
    "A list of file name handlers that temporarily should not be used.\n\
This applies only to the operation `inhibit-file-name-operation'.");
  Vinhibit_file_name_handlers = Qnil;

  DEFVAR_LISP ("inhibit-file-name-operation", &Vinhibit_file_name_operation,
    "The operation for which `inhibit-file-name-handlers' is applicable.");
  Vinhibit_file_name_operation = Qnil;

  DEFVAR_LISP ("auto-save-list-file-name", &Vauto_save_list_file_name,
    "File name in which we write a list of all auto save file names.");
  Vauto_save_list_file_name = Qnil;

  defsubr (&Sfind_file_name_handler);
  defsubr (&Sfile_name_directory);
  defsubr (&Sfile_name_nondirectory);
  defsubr (&Sunhandled_file_name_directory);
  defsubr (&Sfile_name_as_directory);
  defsubr (&Sdirectory_file_name);
  defsubr (&Smake_temp_name);
  defsubr (&Sexpand_file_name);
  defsubr (&Ssubstitute_in_file_name);
  defsubr (&Scopy_file);
  defsubr (&Smake_directory_internal);
  defsubr (&Sdelete_directory);
  defsubr (&Sdelete_file);
  defsubr (&Srename_file);
  defsubr (&Sadd_name_to_file);
#ifdef S_IFLNK
  defsubr (&Smake_symbolic_link);
#endif /* S_IFLNK */
#ifdef VMS
  defsubr (&Sdefine_logical_name);
#endif /* VMS */
#ifdef HPUX_NET
  defsubr (&Ssysnetunam);
#endif /* HPUX_NET */
  defsubr (&Sfile_name_absolute_p);
  defsubr (&Sfile_exists_p);
  defsubr (&Sfile_executable_p);
  defsubr (&Sfile_readable_p);
  defsubr (&Sfile_writable_p);
  defsubr (&Sfile_symlink_p);
  defsubr (&Sfile_directory_p);
  defsubr (&Sfile_accessible_directory_p);
  defsubr (&Sfile_modes);
  defsubr (&Sset_file_modes);
  defsubr (&Sset_default_file_modes);
  defsubr (&Sdefault_file_modes);
  defsubr (&Sfile_newer_than_file_p);
  defsubr (&Sinsert_file_contents);
  defsubr (&Swrite_region);
  defsubr (&Scar_less_than_car);
  defsubr (&Sverify_visited_file_modtime);
  defsubr (&Sclear_visited_file_modtime);
  defsubr (&Svisited_file_modtime);
  defsubr (&Sset_visited_file_modtime);
  defsubr (&Sdo_auto_save);
  defsubr (&Sset_buffer_auto_saved);
  defsubr (&Sclear_buffer_auto_save_failure);
  defsubr (&Srecent_auto_save_p);

  defsubr (&Sread_file_name_internal);
  defsubr (&Sread_file_name);

#ifdef unix
  defsubr (&Sunix_sync);
#endif
}
