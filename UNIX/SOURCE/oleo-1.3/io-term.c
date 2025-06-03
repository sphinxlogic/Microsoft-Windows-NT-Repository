/*	Copyright (C) 1990, 1992, 1993 Free Software Foundation, Inc.

This file is part of Oleo, the GNU Spreadsheet.

Oleo is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

Oleo is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Oleo; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include <ctype.h>
#include <stdio.h>

#include "global.h"

#include "basic.h"
#include "cell.h"
#include "cmd.h"
#include "format.h"
#include "font.h"
#include "getopt.h"
#include "init.h"
#define DEFINE_IO_VARS 1
#include "io-abstract.h"
#include "io-curses.h"
#include "io-edit.h"
#include "io-generic.h"
#include "io-term.h"
#include "io-utils.h"
#include "io-x11.h"
#include "key.h"
#include "line.h"
#include "lists.h"
#define obstack_chunk_alloc ck_malloc
#define obstack_chunk_free free
#include "obstack.h"
#include "oleofile.h"
#include "print.h"
#include "ref.h"
#include "regions.h"
#include "window.h"
#include "funcs.h"
#include "graph.h"

#ifdef USE_DLD
/* If we're using dynamic linking, we get the names of the
	   functions to call by prepending the basename of save_name onto
		   _read_file
		   _write_file
		   _set_options
		   _show_options
	   so, if the file is sylk.o , the functions are named
		   sylk_read_file
		   sylk_write_file
		   sylk_set_options
		   sylk_show_options
		   */
char *io_name;
#else
#include "list.h"
#include "sc.h"
#include "sylk.h"
#endif




/* This should be updated for every release.
 * The file ANNOUNCE must be udpated as well.
 */
const char oleo_version_string[] = "Oleo version 1.3";

/* This variable is non-zero if the spreadsheet has been changed in any way */ 
int modified = 0;

/* User settable options */
int bkgrnd_recalc = 1;
int auto_recalc = 1;
int a0 = 0;
int topclear = 0;

/* This is how frequently the alarm should go off. */
unsigned int alarm_seconds = 1;

/* This is whether the alarm should go off at all. */
unsigned int alarm_active = 1;

/* Jump here on error.  This simply restarts the top 
 * level command loop.  User state should have been 
 * reset appropriately before the longjmp.
 */
jmp_buf error_exception;

char * current_filename = 0;

/* These are the hooks used to do file-io. */
#ifdef __STDC__
void (*read_file) (FILE *, int) = oleo_read_file;
void (*write_file) (FILE *, struct rng *) = oleo_write_file;
int (*set_file_opts) (int, char *) = oleo_set_options;
void (*show_file_opts) () = oleo_show_options;
#else
void (*read_file) () = oleo_read_file;
void (*write_file) () = oleo_write_file;
int (*set_file_opts) () = oleo_set_options;
void (*show_file_opts) () = oleo_show_options;
#endif

static char * disclaimer[] = 
{
  " Copyright (C) 1990, 1991, 1992, 1993 Free Software Foundation,Inc.\n",
  "There is ABSOLUTELY NO WARRANTY for Oleo; see the file COPYING\n",
  "for details.  Oleo is free software and you are welcome to distribute\n",
  "copies of it under certain conditions; see the file COPYING to see the\n",
  "conditions.\n\n",
  0
};

static char short_options[] = "Vqfh";
static struct option long_options[] =
{
  {"version", 0, NULL, 'V'},
  {"quiet", 0, NULL, 'q'},
  {"ignore-init-file", 0, NULL, 'f'},
  {"nw", 0, NULL, 'x'},
  {"help", 0, NULL, 'h'},
  {NULL, 0, NULL, 0}
};


static char * usage[] = 
{
  " [--version] [--quiet] [--ignore-init-file] [--nw] [--help] \n",
  " [-Vqfh] [file]\n",
  0
};

/* Avoid needless messages to stdout. */
int spread_quietly = 0;

/* Avoid using X no matter what else. (-x --no-x) */
int no_x = 0;

/* What kind of display? */
int using_x = 0;
int using_curses = 0;


/* Cell size paramaters. */
unsigned int default_width = 8;
unsigned int default_height = 1;

/* These values are used by clear_spreadsheet ()
 * to restore the defaults.
 */
unsigned int saved_default_width = 8;
unsigned int saved_default_height = 1;

/* Other cell defaults: */
int default_jst = JST_LFT;
int default_fmt = FMT_GEN;
int default_lock = LCK_UNL;

/* Pointers to interesting cmd_func structures. */
struct cmd_func *end_macro_cmd;
struct cmd_func *digit_0_cmd;
struct cmd_func *digit_9_cmd;
struct cmd_func * break_cmd;
struct cmd_func * universal_arg_cmd;

/* A bland signal handler. */
#ifdef __STDC__
static RETSIGTYPE
got_sig (int sig)
#else
static RETSIGTYPE
got_sig (sig)
     int sig;
#endif
{
}



/* An parser for the language grokked by option setting commands. */

#ifdef __STDC__
static int 
do_set_option (char *ptr)
#else
static int 
do_set_option (ptr)
     char *ptr;
#endif
{
  int set_opt = 1;

  while (*ptr == ' ')
    ptr++;
  if (!strincmp ("no", ptr, 2))
    {
      ptr += 2;
      set_opt = 0;
      while (*ptr == ' ')
	ptr++;
    }
  if (!stricmp ("auto", ptr))
    {
      auto_recalc = set_opt;
      return 0;
    }
  if (!stricmp ("bkgrnd", ptr) || !stricmp ("background", ptr))
    {
      bkgrnd_recalc = set_opt;
      return 0;
    }
  if (!stricmp ("a0", ptr))
    {
      a0 = set_opt;
      io_repaint ();
      return 0;
    }
  if (!stricmp ("backup", ptr))
    {
      __make_backups = set_opt;
      return 0;
    }
  if (!stricmp ("bkup_copy", ptr))
    {
      __backup_by_copying = set_opt;
      return 0;
    }
  if (set_opt && !strincmp ("ticks ", ptr, 6))
    {
      ptr += 6;
      cell_timer_seconds = astol (&ptr);
      return 0;
    }
  if (set_opt && !strincmp ("print ", ptr, 6))
    {
      ptr += 6;
      print_width = astol (&ptr);
      return 0;
    }
  if (set_opt && !strincmp ("file ", ptr, 5))
    {
#ifdef USE_DLD
      char *tmpstr;

      ptr += 5;
      tmpstr = ck_malloc (strlen (ptr) + 20);
      if (io_name)
	{
	  sprintf (tmpstr, "%s.o", ptr);
	  if (dld_unlink_by_file (tmpstr, 0))
	    {
	      io_error_msg ("Couldn't unlink old file format %s: %s", io_name, (dld_errno < 0 || dld_errno > dld_nerr) ? "Unknown error" : dld_errlst[dld_errno]);
	      goto bad_file;
	    }
	  free (io_name);
	}
      if (!stricmp (ptr, "panic"))
	{
	  io_name = 0;
	  read_file = panic_read_file;
	  write_file = panic_write_file;
	  set_file_opts = panic_set_options;
	  show_file_opts = panic_show_options;
	  free (tmpstr);
	  return 0;
	}
      io_name = strdup (ptr);
      sprintf (tmpstr, "%s.o", ptr);
      if (dld_link (tmpstr))
	{
	  io_error_msg ("Couldn't link new file format %s: %s", io_name, (dld_errno < 0 || dld_errno > dld_nerr) ? "Unknown error" : dld_errlst[dld_errno]);
	  goto bad_file;
	}
      if (dld_link ("libc.a"))
	io_error_msg ("Couldn't link libc.a");
      if (dld_link ("libm.a"))
	io_error_msg ("Couldn't link libm.a");

      sprintf (tmpstr, "%s_read_file", ptr);
      read_file = dld_function_executable_p (tmpstr) ? dld_get_func (tmpstr) : 0;
      sprintf (tmpstr, "%s_write_file", ptr);
      write_file = dld_function_executable_p (tmpstr) ? dld_get_func (tmpstr) : 0;

      sprintf (tmpstr, "%s_set_options", ptr);
      set_file_opts = (int (*)()) (dld_function_executable_p (tmpstr) ? dld_get_func (tmpstr) : 0);
      sprintf (tmpstr, "%s_show_options", ptr);
      show_file_opts = dld_function_executable_p (tmpstr) ? dld_get_func (tmpstr) : 0;

      if (!read_file
	  || !write_file
	  || !set_file_opts
	  || !show_file_opts)
	{
	  char **missing;
	  int n;

	  missing = dld_list_undefined_sym ();
	  io_text_start ();
	  io_text_line ("Undefined symbols in file format %s:", ptr);
	  io_text_line ("");
	  for (n = 0; n < dld_undefined_sym_count; n++)
	    io_text_line ("%s", missing[n]);
	  io_text_line ("");
	  io_text_finish ();
	  free (missing);
	  io_error_msg ("File format %s has undefined symbols: not loaded", ptr);
	bad_file:
	  sprintf (tmpstr, "%s.o", io_name);
	  dld_unlink_by_file (io_name, 0);
	  if (io_name)
	    free (io_name);
	  io_name = 0;
	  read_file = panic_read_file;
	  write_file = panic_write_file;
	  set_file_opts = panic_set_options;
	  show_file_opts = panic_show_options;
	}
      free (tmpstr);
#else
      ptr += 5;
      if (!stricmp ("oleo", ptr))
	{
	  read_file = oleo_read_file;
	  write_file = oleo_write_file;
	  set_file_opts = oleo_set_options;
	  show_file_opts = oleo_show_options;
	}
      else if (!stricmp ("sylk", ptr))
	{
	  sylk_a0 = 1;
	  read_file = sylk_read_file;
	  write_file = sylk_write_file;
	  set_file_opts = sylk_set_options;
	  show_file_opts = sylk_show_options;
	}
      else if (!stricmp ("sylk-noa0", ptr))
	{
	  sylk_a0 = 0;
	  read_file = sylk_read_file;
	  write_file = sylk_write_file;
	  set_file_opts = sylk_set_options;
	  show_file_opts = sylk_show_options;
	}
      else if (!stricmp ("sc", ptr))
	{
	  read_file = sc_read_file;
	  write_file = sc_write_file;
	  set_file_opts = sc_set_options;
	  show_file_opts = sc_show_options;
	}
      else if (!stricmp ("panic", ptr))
	{
	  read_file = panic_read_file;
	  write_file = panic_write_file;
	  set_file_opts = panic_set_options;
	  show_file_opts = panic_show_options;
	}
      else if (!stricmp ("list", ptr))
	{
	  read_file = list_read_file;
	  write_file = list_write_file;
	  set_file_opts = list_set_options;
	  show_file_opts = list_show_options;
	  /*if (ptr[4])
	    {
	    ptr+=4;
	    sl_sep=string_to_char(&ptr);
	    } */
	}
      else
	io_error_msg ("Unknown file format %s", ptr);
#endif
      return 0;
    }
#ifdef USE_DLD
  else if (!strincmp (ptr, "load ", 5))
    {
      char *tmpstr;
      struct function *new_funs;
      struct cmd_func *new_cmds;
      struct keymap **new_maps;
      void (*init_cmd) ();

      ptr += 5;
      tmpstr = ck_malloc (strlen (ptr) + 20);
      sprintf (tmpstr, "%s.o", ptr);
      if (dld_link (tmpstr))
	{
	  io_error_msg ("Couldn't link %s: %s", tmpstr, (dld_errno < 0 || dld_errno > dld_nerr) ? "Unknown error" : dld_errlst[dld_errno]);
	  free (tmpstr);
	  return 0;
	}
      if (dld_link ("libc.a"))
	io_error_msg ("Couldn't link libc.a");
      if (dld_link ("libm.a"))
	io_error_msg ("Couldn't link libm.a");

      if (dld_undefined_sym_count)
	{
	  char **missing;
	  int n;

	  missing = dld_list_undefined_sym ();
	  io_text_start ();
	  io_text_line ("Undefined symbols in file format %s:", ptr);
	  io_text_line ("");
	  for (n = 0; n < dld_undefined_sym_count; n++)
	    io_text_line ("%s", missing[n]);
	  io_text_line ("");
	  io_text_finish ();
	  free (missing);
	  io_error_msg ("%d undefined symbols in %s", dld_undefined_sym_count, ptr);
	  dld_unlink_by_file (tmpstr, 0);
	  free (tmpstr);
	  return 0;
	}
      sprintf (tmpstr, "%s_funs", ptr);
      new_funs = (struct function *) dld_get_symbol (tmpstr);
      if (new_funs)
	add_usr_funs (new_funs);
      sprintf (tmpstr, "%s_cmds", ptr);
      new_cmds = (struct cmd_func *) dld_get_symbol (tmpstr);
      if (new_cmds)
	add_usr_cmds (new_cmds);
      sprintf (tmpstr, "%s_maps", ptr);
      new_maps = (struct keymap **) dld_get_symbol (tmpstr);
      if (new_maps)
	add_usr_maps (new_maps);
      if (!new_funs && !new_cmds && !new_maps)
	{
	  io_error_msg ("Couldn't find anything to load in %s", ptr);
	  sprintf (tmpstr, "%s.o", ptr);
	  dld_unlink_by_file (tmpstr, 0);
	}
      sprintf (tmpstr, "%s_init", ptr);
      init_cmd = dld_function_executable_p (tmpstr) ? dld_get_func (tmpstr) : 0;
      if (init_cmd)
	(*init_cmd) ();
      free (tmpstr);
      return 0;
    }
#endif
  if (set_window_option (set_opt, ptr) == 0)
    {
      if ((*set_file_opts) (set_opt, ptr))
	io_error_msg ("Unknown option '%s'", ptr);
      return 0;
    }
  return 1;
}

#ifdef __STDC__
void
set_options (char * ptr)
#else
void
set_options (ptr)
     char *ptr;
#endif
{
  if (do_set_option (ptr))
    io_recenter_cur_win ();
}

#ifdef __STDC__
void 
show_options (void)
#else
void 
show_options ()
#endif
{
  int n;
  int fmts;
  char *data_buf[9];

  n = auto_recalc;
  io_text_start ();

  io_text_line ("auto-recalculation: %s        Recalculate in background: %s",
		n ? " on" : "off", bkgrnd_recalc ? "on" : "off");
  io_text_line ("make backup files:  %s        Copy files into backups:   %s",
	__make_backups ? " on" : "off", __backup_by_copying ? "on" : "off");

  io_text_line ("Asynchronous updates every %u ???",
		cell_timer_seconds);

  io_text_line ("Print width:      %5u", print_width);

  io_text_line ("");

  (*show_file_opts) ();

  io_text_line ("");
  show_window_options ();
  io_text_line ("");

  fmts = usr_set_fmts ();
  if (fmts)
    {
      io_text_line ("User-defined formats:");
      io_text_line ("Fmt    +Hdr    -Hdr   +Trlr   -Trlr    Zero   Comma Decimal  Prec         Scale");
      for (n = 0; n < 16; n++)
	{
	  if (fmts & (1 << n))
	    {
	      get_usr_stats (n, data_buf);
	      io_text_line ("%3d %7s %7s %7s %7s %7s %7s %7s %5s %13s",
			    n + 1,
			    data_buf[0],
			    data_buf[1],
			    data_buf[2],
			    data_buf[3],
			    data_buf[4],
			    data_buf[5],
			    data_buf[6],
			    data_buf[7],
			    data_buf[8]);
	    }
	}
    }
  else
    io_text_line ("No user-defined formats have been defined");

  io_text_finish ();
}


#ifdef __STDC__
void
read_mp_usr_fmt (char *ptr)
#else
void
read_mp_usr_fmt (ptr)
     char *ptr;
#endif
{
  int usr_n = -1;
  int n_chrs = 0;
  char *p;
  char *buf[9];
  int i;

  for (i = 0; i < 9; i++)
    buf[i] = "";
  p = ptr;
  while (*p == ';')
    {
      *p++ = '\0';
      switch (*p++)
	{
	case 'N':
	  usr_n = astol (&p) - 1;
	  break;
	case 'H':
	  switch (*p++)
	    {
	    case 'P':
	      i = 0;
	      break;
	    case 'N':
	      i = 1;
	      break;
	    default:
	      goto badline;
	    }
	  goto count_chars;
	case 'T':
	  switch (*p++)
	    {
	    case 'P':
	      i = 2;
	      break;
	    case 'N':
	      i = 3;
	      break;
	    default:
	      goto badline;
	    }
	  goto count_chars;

	case 'Z':
	  i = 4;
	  goto count_chars;

	case 'C':
	  i = 5;
	  goto count_chars;

	case 'D':
	  i = 6;
	  goto count_chars;

	case 'P':
	  i = 7;
	  goto count_chars;

	case 'S':
	  i = 8;
	  goto count_chars;

	count_chars:
	  buf[i] = p;
	  n_chrs++;
	  while (*p && *p != ';')
	    {
	      p++;
	      n_chrs++;
	    }
	  break;

	default:
	badline:
	  io_error_msg ("Unknown OLEO line %s", ptr);
	  return;
	}
    }
  if (*p || usr_n < 0 || usr_n > 15)
    goto badline;

  set_usr_stats (usr_n, buf);
}

/* Modify this to write out *all* the options */
#ifdef __STDC__
void
write_mp_options (FILE *fp)
#else
void
write_mp_options (fp)
     FILE *fp;
#endif
{
  fprintf (fp, "O;%sauto;%sbackground;%sa0;ticks %d\n",
	   auto_recalc ? "" : "no",
	   bkgrnd_recalc ? "" : "no",
	   a0 ? "" : "no",
	   cell_timer_seconds);
}

#ifdef __STDC__
void 
read_mp_options (char *str)
#else
void 
read_mp_options (str)
     char *str;
#endif
{
  char *np;

  while (np = (char *)index (str, ';'))
    {
      *np = '\0';
      do_set_option (str);
      *np++ = ';';
      str = np;
    }
  if (np = (char *)rindex (str, '\n'))
    *np = '\0';
  (void) do_set_option (str);
}




/* Commands related to variables. */

#ifdef __STDC__
void
set_var (char * var, char * val)
#else
void
set_var (var, val)
     char * var;
     char * val;
#endif
{
  char *ret;
  if (val)
    {
      while (isspace (*val))
	++val;
      if (!*val)
	val = 0;
    }
  modified = 1;
  ret = new_var_value (var, strlen(var), val);
  if (ret)
    io_error_msg ("Can't set-variable %s: %s\n", var, ret);
}

#ifdef __STDC__
void
show_var (char *ptr)
#else
void
show_var (ptr)
     char *ptr;
#endif
{
  struct var *v;
  int num;

  while (*ptr == ' ')
    ptr++;
  for (num = 0; ptr[num] && ptr[num] != ' '; num++)
    ;

  v = find_var (ptr, num);
  if (!v || v->var_flags == VAR_UNDEF)
    {
      io_error_msg ("There is no '%s'", ptr);
      return;
    }
  if (a0)
    {
      if (v->v_rng.lr != v->v_rng.hr || v->v_rng.lc != v->v_rng.hc)
	/* FOO */ sprintf (print_buf, "%s $%s$%u:$%s$%u", v->var_name, col_to_str (v->v_rng.lc), v->v_rng.lr, col_to_str (v->v_rng.hc), v->v_rng.hr);
      else
	/* FOO */ sprintf (print_buf, "%s $%s$%u", v->var_name, col_to_str (v->v_rng.lc), v->v_rng.lr);
    }
  else
    sprintf (print_buf, "%s %s", v->var_name, range_name (&(v->v_rng)));
  io_info_msg (print_buf);
}

#ifdef __STDC__
static void
show_a_var (char *name, struct var *v)
#else
static void
show_a_var (name, v)
     char *name;
     struct var *v;
#endif
{
  if (v->var_flags == VAR_UNDEF)
    return;
  if (a0)
    {
      if (v->v_rng.lr != v->v_rng.hr || v->v_rng.lc != v->v_rng.hc)
	/* FOO */ io_text_line ("%-20s  $%s$%u:$%s$%u", v->var_name, col_to_str (v->v_rng.lc), v->v_rng.lr, col_to_str (v->v_rng.hc), v->v_rng.hr);
      else
	/* FOO */ io_text_line ("%-20s  $%s$%u", v->var_name, col_to_str (v->v_rng.lc), v->v_rng.lr);
    }
  else
    io_text_line ("%-20s  %s", v->var_name, range_name (&(v->v_rng)));
}

#ifdef __STDC__
void
show_all_var (void)
#else
void
show_all_var ()
#endif
{
  io_text_start ();
  io_text_line ("%-20s  Current Value", "Variable Name");
  for_all_vars (show_a_var);
  io_text_finish ();
}

static FILE * write_variable_fp = 0;

#ifdef __STDC__
static void
write_a_var (char *name, struct var *v)
#else
static void
write_a_var (name, v)
     char *name;
     struct var *v;
#endif
{
  CELLREF r, c;
  if (v->var_flags == VAR_UNDEF)
    return;
  r = v->v_rng.lr;
  c = v->v_rng.lc;
  if (v->var_flags == VAR_CELL)
    fprintf (write_variable_fp, "%s=%s\n",
	     v->var_name, cell_value_string (r, c));
}

#ifdef __STDC__
void
write_variables (FILE * fp)
#else
void
write_variables (fp)
     FILE * fp;
#endif
{
  if (write_variable_fp)
    io_error_msg ("Can't re-enter write_variables.");
  else
    {
      write_variable_fp = fp;
      for_all_vars (write_a_var);
      write_variable_fp = 0;
    }
}

#ifdef __STDC__
void
read_variables (FILE * fp)
#else
void
read_variables (fp)
     FILE * fp;
#endif
{
  char buf[1024];
  int lineno = 0;
  while (fgets (buf, 1024, fp))
    {
      char * ptr;
      for (ptr = buf; *ptr && *ptr != '\n'; ++ptr)
	;
      *ptr = '\0';
      for (ptr = buf; isspace (*ptr); ptr++)
	;
      if (!*ptr || (*ptr == '#'))
	continue;
      {
	char * var_name = ptr;
	int var_name_len;
	char * value_string;
	while (*ptr && *ptr != '=')
	  ++ptr;
	if (!*ptr)
	  {
	    io_error_msg ("read-variables: format error near line %d.", lineno);
	    return;
	  }
	var_name_len = ptr - var_name;
	++ptr;
	value_string = ptr;
	{
	  struct var * var = find_var (var_name, var_name_len);
	  if (var)
	    {
	      switch (var->var_flags)
		{
		case VAR_UNDEF:
		  break;
		case VAR_CELL:
		  {
		    char * error = new_value (var->v_rng.lr, var->v_rng.lc,
					      value_string); 
		    if (error)
		      {
			io_error_msg (error);
			return;	/* actually, io_error_msg never returns. */
		      }
		    else
		      modified = 1;
		    break;
		  }
		case VAR_RANGE:
		  io_error_msg ("read-variables (line %d): ranges not supported.",
				lineno);
		  return;
		}
	    }
	}
      }
      ++lineno;
    }
  if (!feof (fp))
    {
      io_error_msg ("read-variables: read error near line %d.", lineno);
      return;
    }
}



#ifdef __STDC__
void
init_maps (void)
#else
void
init_maps ()
#endif
{
  num_maps = 0;
  the_maps = 0;
  map_names = 0;
  map_prompts = 0;

  the_funcs = ck_malloc (sizeof (struct cmd_func *) * 2);
  num_funcs = 1;
  the_funcs[0] = &cmd_funcs[0];

  find_func (0, &end_macro_cmd, "end-macro");
  find_func (0, &digit_0_cmd, "digit-0");
  find_func (0, &digit_9_cmd, "digit-9");
  find_func (0, &break_cmd, "break");
  find_func (0, &universal_arg_cmd, "universal-argument");

  create_keymap ("universal", 0);
  push_command_frame (0, 0, 0);
}

#ifdef __STDC__
int 
add_usr_cmds (struct cmd_func *new_cmds)
#else
int 
add_usr_cmds (new_cmds)
     struct cmd_func *new_cmds;
#endif
{
  num_funcs++;
  the_funcs = ck_realloc (the_funcs, num_funcs * sizeof (struct cmd_func *));
  the_funcs[num_funcs - 1] = new_cmds;
  return num_funcs - 1;
}

#ifdef USE_DLD
#ifdef __STDC__
static int 
add_usr_maps (struct keymap **new_maps)
#else
static int 
add_usr_maps (new_maps)
     struct keymap **new_maps;
#endif
{
  int n;

  for (n = 1; new_maps[n]; n++)
    ;
  the_maps = ck_realloc (the_maps, (n + num_maps) * sizeof (struct keymap *));
  bcopy (new_maps, &the_maps[num_maps], n * sizeof (struct keymap *));
  num_maps += n;
  return num_maps - n;
}
#endif /* USE_DLD */

#ifdef __STDC__
static void
show_usage (void)
#else
static void
show_usage ()
#endif
{
  char ** use = usage;
  fprintf (stderr, "Usage: %s ", argv_name);
  while (*use)
    {
      fprintf (stderr, "%s\n", *use);
      ++use;
    }
}

#ifdef __STDC__
static RETSIGTYPE
continue_oleo (int sig)
#else
static RETSIGTYPE
continue_oleo (sig)
     int sig;
#endif
{
  io_repaint ();
  if (using_curses)
    cont_curses ();
}

int display_opened = 0;

extern int sneaky_linec;

#ifdef __STDC__
int 
main (int argc, char **argv)
#else
int 
main (argc, argv)
     int argc;
     char **argv;
#endif
{
  volatile int ignore_init_file = 0;
  FILE * init_fp[2];
  char * init_file_names[2];
  volatile int init_fpc = 0;

  argv_name = argv[0];
  __make_backups = 1;

  /* Set up the minimal io handler. */
#if 0
  cmd_graphics ();
#endif

  {
    int opt;
    for (opt = getopt_long (argc, argv, short_options, long_options, (int *)0);
	 opt != EOF;
	 opt = getopt_long (argc, argv, short_options, long_options, (int *)0))
      {
	switch (opt)
	  {
	  case 'V':
	    fprintf  (stdout, "%s\n", oleo_version_string);
	    break;
	  case 'q':
	    spread_quietly = 1;
	    break;
	  case 'f':
	    ignore_init_file = 1;
	    break;
	  case 'x':
	    no_x = 1;
	    break;
	  case 'h':
	    show_usage ();
	    break;
	  }
      }
  }
  init_infinity ();
  init_mem ();
  init_eval ();
  init_refs ();
  init_cells ();
  init_fonts ();
  init_info ();

#ifdef USE_DLD
  if (!index (argv_name, '/'))
    {
      char *name;

      name = dld_find_executable (argv_name);
      num = dld_init (name);
      free (name);
    }
  else
    num = dld_init (argv_name);
  if (num)
    io_error_msg ("dld_init() failed: %s", (dld_errno < 0 || dld_errno > dld_nerr) ? "Unknown error" : dld_errlst[dld_errno]);
  dld_search_path = ":/usr/local/lib/oleo:/lib:/usr/lib:/usr/local/lib";
#endif



  /* Find the init files. 
   * This is done even if ignore_init_file is true because
   * it effects whether the disclaimer will be shown.
   */
    {
      char *ptr, *home;
      
      home = getenv ("HOME");
      if (home)
	{
	  ptr = mk_sprintf ("%s/%s", home, RCFILE);
	  init_fp[init_fpc] = fopen (ptr, "r");
	  init_file_names[init_fpc] = ptr;
	  if (init_fp[init_fpc])
	    ++init_fpc;
	}
      
      init_fp[init_fpc] = fopen (RCFILE, "r");
      if (init_fp[init_fpc])
	++init_fpc;
    }

  if (!init_fpc && !spread_quietly)
    {
      char ** msg;
      fputs (oleo_version_string, stdout);
      for (msg = disclaimer; *msg; ++msg)
	fputs (*msg, stdout);
      fflush (stdout);
    }

  FD_ZERO (&read_fd_set);
  FD_ZERO (&read_pending_fd_set);
  FD_ZERO (&exception_fd_set);
  FD_ZERO (&exception_pending_fd_set);

#ifdef HAVE_X11_X_H
  if (!no_x)
    get_x11_args (&argc, argv);
  if (!no_x && io_x11_display_name)
    {
      x11_graphics ();
      using_x = 1;
    }
  else
#endif
    {
      tty_graphics ();
      using_curses = 1;
      /* Allow the disclaimer to be read. */
      if (!init_fpc && !spread_quietly)
	sleep (5);
    }

  io_open_display ();

  init_graphing ();

  if (setjmp (error_exception))
    {
      fprintf (stderr, "Error in the builtin init scripts (a bug!).");
      exit (69);
    }
  else
    {
      init_maps ();
      init_named_macro_strings ();
      run_init_cmds ();
    }

  if (argc - optind > 1)
    {
      show_usage ();
      exit (1);
    }

  /* These probably don't all need to be ifdef, but
   * it is harmless.
   */
#ifdef SIGCONT
  signal (SIGCONT, continue_oleo);
#endif
#ifdef SIGINT
  signal (SIGINT, got_sig);
#endif
#ifdef SIGQUIT
  signal (SIGQUIT, got_sig);
#endif
#ifdef SIGILL
  signal (SIGILL, got_sig);
#endif
#ifdef SIGEMT
  signal (SIGEMT, got_sig);
#endif
#ifdef SIGBUS
  signal (SIGBUS, got_sig);
#endif
#ifdef SIGSEGV
  signal (SIGSEGV, got_sig);
#endif
#ifdef SIGPIPE
  signal (SIGPIPE, got_sig);
#endif

  /* Read the init file. */
  {
    volatile int x;
    for (x = 0; x < init_fpc; ++x)
      {
	if (setjmp (error_exception))
	  {
	    fprintf (stderr, "   error occured in init file %s near line %d.",
		     init_file_names [x], sneaky_linec);
	  }
	else
	  if (!ignore_init_file)
	    read_cmds_cmd (init_fp[x]);
	fclose (init_fp[x]);
      }
  }


  if (argc - optind == 1)
    {
      FILE * fp;
      /* fixme: record file name */
      ++optind;
      if (fp = fopen (argv[1], "r"))
	{
	  if (setjmp (error_exception))
	    fprintf (stderr, "  error occured reading %s", argv[1]);
	  else
	    read_file_and_run_hooks (fp, 0, argv[1]);
	  fclose (fp);
	}
      else
	fprintf (stderr, "Can't open %s: %s", argv[1], err_msg ());
    }
  /* Force the command frame to be rebuilt now that the keymaps exist. */
  {
    struct command_frame * last_of_the_old = the_cmd_frame->next;
    while (the_cmd_frame != last_of_the_old)
      free_cmd_frame (the_cmd_frame);
    free_cmd_frame (last_of_the_old);
  }
  io_recenter_cur_win ();

  display_opened = 1;

  run_string_as_macro
    ("{with-keymap press-any}{builtin-help _NON_WARRANTY_", 1);
  while (1)
    {
      setjmp (error_exception);
      command_loop (0);
    }
}


