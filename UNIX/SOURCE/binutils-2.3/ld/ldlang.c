/* Linker command language support.
   Copyright 1991, 1992, 1993 Free Software Foundation, Inc.

This file is part of GLD, the Gnu Linker.

GLD is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

GLD is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GLD; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include "bfd.h"
#include "sysdep.h"

#include "ld.h"
#include "ldmain.h"
#include "ldsym.h"
#include "ldgram.h"
#include "ldwarn.h"
#include "ldlang.h"
#include "ldexp.h"
#include "ldemul.h"
#include "ldlex.h"
#include "ldmisc.h"
#include "ldindr.h"
#include "ldctor.h"

/* FORWARDS */
static void print_statements PARAMS ((void));
static void print_statement PARAMS ((lang_statement_union_type *,
				      lang_output_section_statement_type *));
static lang_statement_union_type *new_statement PARAMS ((enum statement_enum,
							 size_t,
							 lang_statement_list_type*));


/* LOCALS */
static struct obstack stat_obstack;

#define obstack_chunk_alloc ldmalloc
#define obstack_chunk_free free
static CONST char *startup_file;
static lang_statement_list_type input_file_chain;

/* Points to the last statement in the .data section, so we can add
   stuff to the data section without pain */
static lang_statement_list_type end_of_data_section_statement_list;

/* List of statements needed to handle constructors */
extern lang_statement_list_type constructor_list;

static boolean placed_commons = false;
static lang_output_section_statement_type *default_common_section;
static boolean map_option_f;
static bfd_vma print_dot;
static lang_input_statement_type *first_file;
static lang_statement_list_type lang_output_section_statement;
static CONST char *current_target;
static CONST char *output_target;
static size_t longest_section_name = 8;
static section_userdata_type common_section_userdata;
static lang_statement_list_type statement_list;

/* EXPORTS */
boolean relaxing;
lang_output_section_statement_type *abs_output_section;
lang_statement_list_type *stat_ptr = &statement_list;
lang_input_statement_type *script_file = 0;
lang_statement_list_type file_chain =
{0};
CONST char *entry_symbol = 0;
bfd_size_type largest_section = 0;
boolean lang_has_input_file = false;
lang_output_section_statement_type *create_object_symbols = 0;
boolean had_output_filename = false;
boolean lang_float_flag = false;
boolean delete_output_file_on_failure = false;

/* IMPORTS */
extern char *default_target;

extern CONST char *output_filename;
extern char *current_file;
extern bfd *output_bfd;
extern enum bfd_architecture ldfile_output_architecture;
extern unsigned long ldfile_output_machine;
extern char *ldfile_output_machine_name;
extern ldsym_type *symbol_head;
extern unsigned int commons_pending;
extern args_type command_line;
extern ld_config_type config;
extern boolean write_map;
extern int g_switch_value;


etree_type *base; /* Relocation base - or null */


#ifdef __STDC__
#define cat(a,b) a##b
#else
#define cat(a,b) a/**/b
#endif

#define new_stat(x,y) (cat(x,_type)*) new_statement(cat(x,_enum), sizeof(cat(x,_type)),y)

#define outside_section_address(q) ( (q)->output_offset + (q)->output_section->vma)

#define outside_symbol_address(q) ((q)->value +   outside_section_address(q->section))

void lang_add_data PARAMS ((int type, union etree_union * exp));

PTR
stat_alloc (size)
     size_t size;
{
  return obstack_alloc (&stat_obstack, size);
}
static void
print_size (value)
     size_t value;
{
  fprintf (config.map_file, "%5x", (unsigned) value);
}
static void
print_alignment (value)
     unsigned int value;
{
  fprintf (config.map_file, "2**%1u", value);
}
static void
DEFUN (print_fill, (value),
     fill_type value)
{
  fprintf (config.map_file, "%04x", (unsigned) value);
}


static void
print_section (name)
     CONST char *CONST name;
{
  fprintf (config.map_file, "%*s", -longest_section_name, name);
}

/*----------------------------------------------------------------------
  lang_for_each_statement walks the parse tree and calls the provided
  function for each node
*/

static void
lang_for_each_statement_worker (func, s)
     void (*func) ();
     lang_statement_union_type *s;
{
  for (; s != (lang_statement_union_type *) NULL; s = s->next)
    {
      func (s);

      switch (s->header.type)
	{
	case lang_constructors_statement_enum:
	  lang_for_each_statement_worker (func, constructor_list.head);
	  break;
	case lang_output_section_statement_enum:
	  lang_for_each_statement_worker
	    (func,
	     s->output_section_statement.children.head);
	  break;
	case lang_wild_statement_enum:
	  lang_for_each_statement_worker
	    (func,
	     s->wild_statement.children.head);
	  break;
	case lang_data_statement_enum:
	case lang_object_symbols_statement_enum:
	case lang_output_statement_enum:
	case lang_target_statement_enum:
	case lang_input_section_enum:
	case lang_input_statement_enum:
	case lang_assignment_statement_enum:
	case lang_padding_statement_enum:
	case lang_address_statement_enum:
	  break;
	default:
	  FAIL ();
	  break;
	}
    }
}

void
lang_for_each_statement (func)
     void (*func) ();
{
  lang_for_each_statement_worker (func,
				  statement_list.head);
}

/*----------------------------------------------------------------------*/
void
lang_list_init (list)
     lang_statement_list_type *list;
{
  list->head = (lang_statement_union_type *) NULL;
  list->tail = &list->head;
}

/*----------------------------------------------------------------------

  build a new statement node for the parse tree

 */

static
lang_statement_union_type *
new_statement (type, size, list)
     enum statement_enum type;
     size_t size;
     lang_statement_list_type * list;
{
  lang_statement_union_type *new = (lang_statement_union_type *)
  stat_alloc (size);

  new->header.type = type;
  new->header.next = (lang_statement_union_type *) NULL;
  lang_statement_append (list, new, &new->header.next);
  return new;
}

/*
  Build a new input file node for the language. There are several ways
  in which we treat an input file, eg, we only look at symbols, or
  prefix it with a -l etc.

  We can be supplied with requests for input files more than once;
  they may, for example be split over serveral lines like foo.o(.text)
  foo.o(.data) etc, so when asked for a file we check that we havn't
  got it already so we don't duplicate the bfd.

 */
static lang_input_statement_type *
new_afile (name, file_type, target)
     CONST char *CONST name;
     CONST lang_input_file_enum_type file_type;
     CONST char *CONST target;
{

  lang_input_statement_type *p = new_stat (lang_input_statement,
					   stat_ptr);

  lang_has_input_file = true;
  p->target = target;
  p->complained = false;
  switch (file_type)
    {
    case lang_input_file_is_symbols_only_enum:
      p->filename = name;
      p->is_archive = false;
      p->real = true;
      p->local_sym_name = name;
      p->just_syms_flag = true;
      p->search_dirs_flag = false;
      break;
    case lang_input_file_is_fake_enum:
      p->filename = name;
      p->is_archive = false;
      p->real = false;
      p->local_sym_name = name;
      p->just_syms_flag = false;
      p->search_dirs_flag = false;
      break;
    case lang_input_file_is_l_enum:
      p->is_archive = true;
      p->filename = name;
      p->real = true;
      p->local_sym_name = concat ("-l", name, "");
      p->just_syms_flag = false;
      p->search_dirs_flag = true;
      break;
    case lang_input_file_is_search_file_enum:
    case lang_input_file_is_marker_enum:
      p->filename = name;
      p->is_archive = false;
      p->real = true;
      p->local_sym_name = name;
      p->just_syms_flag = false;
      p->search_dirs_flag = true;
      break;
    case lang_input_file_is_file_enum:
      p->filename = name;
      p->is_archive = false;
      p->real = true;
      p->local_sym_name = name;
      p->just_syms_flag = false;
      p->search_dirs_flag = false;
      break;
    default:
      FAIL ();
    }
  p->asymbols = (asymbol **) NULL;
  p->superfile = (lang_input_statement_type *) NULL;
  p->next_real_file = (lang_statement_union_type *) NULL;
  p->next = (lang_statement_union_type *) NULL;
  p->symbol_count = 0;
  p->common_output_section = (asection *) NULL;
  lang_statement_append (&input_file_chain,
			 (lang_statement_union_type *) p,
			 &p->next_real_file);
  return p;
}

lang_input_statement_type *
lang_add_input_file (name, file_type, target)
     CONST char *name;
     lang_input_file_enum_type file_type;
     CONST char *target;
{
  /* Look it up or build a new one */
  lang_has_input_file = true;

#if 0
  lang_input_statement_type *p;

  for (p = (lang_input_statement_type *) input_file_chain.head;
       p != (lang_input_statement_type *) NULL;
       p = (lang_input_statement_type *) (p->next_real_file))
    {
      /* Sometimes we have incomplete entries in here */
      if (p->filename != (char *) NULL)
	{
	  if (strcmp (name, p->filename) == 0)
	    return p;
	}

    }
#endif
  return new_afile (name, file_type, target);
}

void
lang_add_keepsyms_file (filename)
     CONST char *filename;
{
  extern strip_symbols_type strip_symbols;
  if (keepsyms_file != 0)
    info_msg ("%X%P: error: duplicated keep-symbols-file value\n");
  keepsyms_file = filename;
  if (strip_symbols != STRIP_NONE)
    info_msg ("%P: `-keep-only-symbols-file' overrides `-s' and `-S'\n");
  strip_symbols = STRIP_SOME;
}

/* Build enough state so that the parser can build its tree */
void
lang_init ()
{
  obstack_begin (&stat_obstack, 1000);

  stat_ptr = &statement_list;

  lang_list_init (stat_ptr);

  lang_list_init (&input_file_chain);
  lang_list_init (&lang_output_section_statement);
  lang_list_init (&file_chain);
  first_file = lang_add_input_file ((char *) NULL,
				    lang_input_file_is_marker_enum,
				    (char *) NULL);
  abs_output_section = lang_output_section_statement_lookup (BFD_ABS_SECTION_NAME);

  abs_output_section->bfd_section = &bfd_abs_section;

}

/*----------------------------------------------------------------------
 A region is an area of memory declared with the
 MEMORY {  name:org=exp, len=exp ... }
 syntax.

 We maintain a list of all the regions here

 If no regions are specified in the script, then the default is used
 which is created when looked up to be the entire data space
*/

static lang_memory_region_type *lang_memory_region_list;
static lang_memory_region_type **lang_memory_region_list_tail = &lang_memory_region_list;

lang_memory_region_type *
lang_memory_region_lookup (name)
     CONST char *CONST name;
{

  lang_memory_region_type *p = lang_memory_region_list;

  for (p = lang_memory_region_list;
       p != (lang_memory_region_type *) NULL;
       p = p->next)
    {
      if (strcmp (p->name, name) == 0)
	{
	  return p;
	}
    }
  if (strcmp (name, "*default*") == 0)
    {
      /* This is the default region, dig out first one on the list */
      if (lang_memory_region_list != (lang_memory_region_type *) NULL)
	{
	  return lang_memory_region_list;
	}
    }
  {
    lang_memory_region_type *new =
    (lang_memory_region_type *) stat_alloc ((bfd_size_type) (sizeof (lang_memory_region_type)));

    new->name = buystring (name);
    new->next = (lang_memory_region_type *) NULL;

    *lang_memory_region_list_tail = new;
    lang_memory_region_list_tail = &new->next;
    new->origin = 0;
    new->length = ~(bfd_size_type)0;
    new->current = 0;
    new->had_full_message = false;

    return new;
  }
}


lang_output_section_statement_type *
lang_output_section_find (name)
     CONST char *CONST name;
{
  lang_statement_union_type *u;
  lang_output_section_statement_type *lookup;

  for (u = lang_output_section_statement.head;
       u != (lang_statement_union_type *) NULL;
       u = lookup->next)
    {
      lookup = &u->output_section_statement;
      if (strcmp (name, lookup->name) == 0)
	{
	  return lookup;
	}
    }
  return (lang_output_section_statement_type *) NULL;
}

lang_output_section_statement_type *
lang_output_section_statement_lookup (name)
     CONST char *CONST name;
{
  lang_output_section_statement_type *lookup;

  lookup = lang_output_section_find (name);
  if (lookup == (lang_output_section_statement_type *) NULL)
    {

      lookup = (lang_output_section_statement_type *)
	new_stat (lang_output_section_statement, stat_ptr);
      lookup->region = (lang_memory_region_type *) NULL;
      lookup->fill = 0;
      lookup->block_value = 1;
      lookup->name = name;

      lookup->next = (lang_statement_union_type *) NULL;
      lookup->bfd_section = (asection *) NULL;
      lookup->processed = false;
      lookup->loadable = 1;
      lookup->addr_tree = (etree_type *) NULL;
      lang_list_init (&lookup->children);

      lookup->memspec = (CONST char *) NULL;
      lookup->flags = 0;
      lookup->subsection_alignment = -1;
      lookup->section_alignment = -1;
      lookup->load_base = (union etree_union *) NULL;

      lang_statement_append (&lang_output_section_statement,
			     (lang_statement_union_type *) lookup,
			     &lookup->next);
    }
  return lookup;
}

/*ARGSUSED*/
static void
print_flags (ignore_flags)
     int *ignore_flags;
{
  fprintf (config.map_file, "(");
#if 0
  if (flags->flag_read)
    fprintf (outfile, "R");
  if (flags->flag_write)
    fprintf (outfile, "W");
  if (flags->flag_executable)
    fprintf (outfile, "X");
  if (flags->flag_loadable)
    fprintf (outfile, "L");
#endif
 fprintf (config.map_file, ")");
}

void
lang_map ()
{
  lang_memory_region_type *m;

  fprintf (config.map_file, "**MEMORY CONFIGURATION**\n\n");
#ifdef HOST_64_BIT
  fprintf (config.map_file, "name\t\torigin\t\tlength\t\tattributes\n");
#else
  fprintf (config.map_file,
	   "name\t\torigin   length   r_size   c_size    is    attributes\n");

#endif
  for (m = lang_memory_region_list;
       m != (lang_memory_region_type *) NULL;
       m = m->next)
    {
      fprintf (config.map_file, "%-16s", m->name);
      print_address (m->origin);
      print_space ();
      print_address ((bfd_vma)m->length);
      print_space ();
      print_address ((bfd_vma)m->old_length);
      print_space();
      print_address (m->current - m->origin);
      print_space();
      if (m->old_length)
       fprintf(config.map_file," %2d%%  ", ( m->current - m->origin) * 100 / m->old_length);
      print_flags (&m->flags);
      fprintf (config.map_file, "\n");
    }
  fprintf (config.map_file, "\n\n**LINK EDITOR MEMORY MAP**\n\n");
  fprintf (config.map_file, "output   input     virtual\n");
  fprintf (config.map_file, "section  section   address    tsize\n\n");

  print_statements ();

}

/*
 *
 */
static void
init_os (s)
     lang_output_section_statement_type * s;
{
/*  asection *section = bfd_get_section_by_name(output_bfd, s->name);*/
  section_userdata_type *new =
  (section_userdata_type *)
  stat_alloc ((bfd_size_type) (sizeof (section_userdata_type)));

  s->bfd_section = bfd_get_section_by_name (output_bfd, s->name);
  if (s->bfd_section == (asection *) NULL)
    s->bfd_section = bfd_make_section (output_bfd, s->name);
  if (s->bfd_section == (asection *) NULL)
    {
      einfo ("%P%F: output format %s cannot represent section called %s\n",
	     output_bfd->xvec->name, s->name);
    }
  s->bfd_section->output_section = s->bfd_section;
/*  s->bfd_section->flags = s->flags;*/

  /* We initialize an output sections output offset to minus its own */
  /* vma to allow us to output a section through itself */
  s->bfd_section->output_offset = 0;
  get_userdata (s->bfd_section) = (PTR) new;

}

/***********************************************************************
  The wild routines.

  These expand statements like *(.text) and foo.o to a list of
  explicit actions, like foo.o(.text), bar.o(.text) and
  foo.o(.text,.data) .

  The toplevel routine, wild, takes a statement, section, file and
  target. If either the section or file is null it is taken to be the
  wildcard. Seperate lang_input_section statements are created for
  each part of the expanstion, and placed after the statement provided.

*/

static void
wild_doit (ptr, section, output, file)
     lang_statement_list_type * ptr;
     asection * section;
     lang_output_section_statement_type * output;
     lang_input_statement_type * file;
{
  if (output->bfd_section == (asection *) NULL)
  {
    init_os (output);
    /* Initialize the vma and size to the existing section.  This will
       be overriden in lang_size_sections unless SEC_NEVER_LOAD gets
       set.  */
    if (section != (asection *) NULL)
    {
      bfd_set_section_vma (0, output->bfd_section,
			   bfd_section_vma (0, section));
      output->bfd_section->_raw_size = section->_raw_size;
    }
  }

  if (section != (asection *) NULL
      && section->output_section == (asection *) NULL)
  {
    /* Add a section reference to the list */
    lang_input_section_type *new = new_stat (lang_input_section, ptr);

    new->section = section;
    new->ifile = file;
    section->output_section = output->bfd_section;

    /* Be selective about what the output section inherits from the
       input section */

    if ((section->flags & SEC_SHARED_LIBRARY) != 0)
      section->output_section->flags |= section->flags;
    else
      section->output_section->flags |= section->flags & ~SEC_NEVER_LOAD;

    if (!output->loadable) 
    {
      /* Turn off load flag */
      output->bfd_section->flags &= ~SEC_LOAD;
      output->bfd_section->flags |= SEC_NEVER_LOAD;
    }
    if (section->alignment_power > output->bfd_section->alignment_power)
    {
      output->bfd_section->alignment_power = section->alignment_power;
    }
    /* If supplied an aligmnet, then force it */
    if (output->section_alignment != -1)
    {
      output->bfd_section->alignment_power = output->section_alignment;
    }
  }
}

static asection *
our_bfd_get_section_by_name (abfd, section)
     bfd * abfd;
     CONST char *section;
{
  return bfd_get_section_by_name (abfd, section);
}

static void
wild_section (ptr, section, file, output)
     lang_wild_statement_type * ptr;
     CONST char *section;
     lang_input_statement_type * file;
     lang_output_section_statement_type * output;
{
  asection *s;

  if (file->just_syms_flag == false)
    {
      if (section == (char *) NULL)
	{
	  /* Do the creation to all sections in the file */
	  for (s = file->the_bfd->sections; s != (asection *) NULL; s = s->next)
	  {
	    /* except for bss */
	    if ((s->flags & SEC_IS_COMMON)  == 0)
	    {
	      wild_doit (&ptr->children, s, output, file);
	    }
	  }
	}
      else
	{
	  /* Do the creation to the named section only */
	  wild_doit (&ptr->children,
		     our_bfd_get_section_by_name (file->the_bfd, section),
		     output, file);
	}
    }
}

/* passed a file name (which must have been seen already and added to
   the statement tree. We will see if it has been opened already and
   had its symbols read. If not then we'll read it.

   Archives are pecuilar here. We may open them once, but if they do
   not define anything we need at the time, they won't have all their
   symbols read. If we need them later, we'll have to redo it.
   */
static
lang_input_statement_type *
lookup_name (name)
     CONST char *CONST name;
{
  lang_input_statement_type *search;

  for (search = (lang_input_statement_type *) input_file_chain.head;
       search != (lang_input_statement_type *) NULL;
       search = (lang_input_statement_type *) search->next_real_file)
    {
      if (search->filename == (char *) NULL && name == (char *) NULL)
	{
	  return search;
	}
      if (search->filename != (char *) NULL && name != (char *) NULL)
	{
	  if (strcmp (search->filename, name) == 0)
	    {
	      ldmain_open_file_read_symbol (search);
	      return search;
	    }
	}
    }

  /* There isn't an afile entry for this file yet, this must be
     because the name has only appeared inside a load script and not
     on the command line  */
  search = new_afile (name, lang_input_file_is_file_enum, default_target);
  ldmain_open_file_read_symbol (search);
  return search;


}

static void
wild (s, section, file, target, output)
     lang_wild_statement_type * s;
     CONST char *CONST section;
     CONST char *CONST file;
     CONST char *CONST target;
     lang_output_section_statement_type * output;
{
  lang_input_statement_type *f;

  if (file == (char *) NULL)
    {
      /* Perform the iteration over all files in the list */
      for (f = (lang_input_statement_type *) file_chain.head;
	   f != (lang_input_statement_type *) NULL;
	   f = (lang_input_statement_type *) f->next)
	{
	  wild_section (s, section, f, output);
	}
      /* Once more for the script file */
      wild_section(s, section, script_file, output);
    }
  else
    {
      /* Perform the iteration over a single file */
      wild_section (s, section, lookup_name (file), output);
    }
  if (section != (char *) NULL
      && strcmp (section, "COMMON") == 0
   && default_common_section == (lang_output_section_statement_type *) NULL)
    {
      /* Remember the section that common is going to incase we later
         get something which doesn't know where to put it */
      default_common_section = output;
    }
}

/*
  read in all the files
  */

static bfd *
open_output (name)
     CONST char *CONST name;
{
  extern unsigned long ldfile_output_machine;
  extern enum bfd_architecture ldfile_output_architecture;

  bfd *output;

  if (output_target == (char *) NULL)
    {
      if (current_target != (char *) NULL)
	output_target = current_target;
      else
	output_target = default_target;
    }
  output = bfd_openw (name, output_target);

  if (output == (bfd *) NULL)
    {
      if (bfd_error == invalid_target)
	{
	  einfo ("%P%F: target %s not found\n", output_target);
	}
      einfo ("%P%F: cannot open output file %s: %E\n", name);
    }

  delete_output_file_on_failure = 1;

  /*  output->flags |= D_PAGED;*/

  if (! bfd_set_format (output, bfd_object))
    einfo ("%P%F:%s: can not make object file: %E\n", name);
  if (! bfd_set_arch_mach (output,
			   ldfile_output_architecture,
			   ldfile_output_machine))
    einfo ("%P%F:%s: can not set architecture: %E\n", name);

  bfd_set_gp_size (output, g_switch_value);
  return output;
}




static void
ldlang_open_output (statement)
     lang_statement_union_type * statement;
{
  switch (statement->header.type)
    {
      case lang_output_statement_enum:
      output_bfd = open_output (statement->output_statement.name);
      ldemul_set_output_arch ();
      if (config.magic_demand_paged && !config.relocateable_output)
	output_bfd->flags |= D_PAGED;
      else
	output_bfd->flags &= ~D_PAGED;
      if (config.text_read_only)
	output_bfd->flags |= WP_TEXT;
      else
	output_bfd->flags &= ~WP_TEXT;
      break;

    case lang_target_statement_enum:
      current_target = statement->target_statement.target;
      break;
    default:
      break;
    }
}

static void
open_input_bfds (statement)
     lang_statement_union_type * statement;
{
  switch (statement->header.type)
    {
      case lang_target_statement_enum:
      current_target = statement->target_statement.target;
      break;
    case lang_wild_statement_enum:
      /* Maybe we should load the file's symbols */
      if (statement->wild_statement.filename)
	{
	  (void) lookup_name (statement->wild_statement.filename);
	}
      break;
    case lang_input_statement_enum:
      if (statement->input_statement.real == true)
	{
	  statement->input_statement.target = current_target;
	  lookup_name (statement->input_statement.filename);
	}
      break;
    default:
      break;
    }
}

/* If there are [COMMONS] statements, put a wild one into the bss section */

static void
lang_reasonable_defaults ()
{



#if 0
  lang_output_section_statement_lookup (".text");
  lang_output_section_statement_lookup (".data");

  default_common_section =
    lang_output_section_statement_lookup (".bss");


  if (placed_commons == false)
    {
      lang_wild_statement_type *new =
      new_stat (lang_wild_statement,
		&default_common_section->children);

      new->section_name = "COMMON";
      new->filename = (char *) NULL;
      lang_list_init (&new->children);
    }
#endif

}

/*
 Add the supplied name to the symbol table as an undefined reference.
 Remove items from the chain as we open input bfds
 */
typedef struct ldlang_undef_chain_list
{
  struct ldlang_undef_chain_list *next;
  char *name;
}                       ldlang_undef_chain_list_type;

static ldlang_undef_chain_list_type *ldlang_undef_chain_list_head;

void
ldlang_add_undef (name)
     CONST char *CONST name;
{
  ldlang_undef_chain_list_type *new =
  (ldlang_undef_chain_list_type
   *) stat_alloc ((bfd_size_type) (sizeof (ldlang_undef_chain_list_type)));

  new->next = ldlang_undef_chain_list_head;
  ldlang_undef_chain_list_head = new;

  new->name = buystring (name);
}

/* Run through the list of undefineds created above and place them
   into the linker hash table as undefined symbols belonging to the
   script file.
*/
static void
lang_place_undefineds ()
{
  ldlang_undef_chain_list_type *ptr = ldlang_undef_chain_list_head;

  while (ptr != (ldlang_undef_chain_list_type *) NULL)
    {
      asymbol *def;
      asymbol **def_ptr = (asymbol **) stat_alloc ((bfd_size_type) (sizeof (asymbol **)));

      def = (asymbol *) bfd_make_empty_symbol (script_file->the_bfd);
      *def_ptr = def;
      def->name = ptr->name;
      def->section = &bfd_und_section;
      enter_global_ref (def_ptr, ptr->name);
      ptr = ptr->next;
    }
}

/* Copy important data from out internal form to the bfd way. Also
   create a section for the dummy file
 */

static void
lang_create_output_section_statements ()
{
  lang_statement_union_type *os;

  for (os = lang_output_section_statement.head;
       os != (lang_statement_union_type *) NULL;
       os = os->output_section_statement.next)
    {
      lang_output_section_statement_type *s =
      &os->output_section_statement;

      init_os (s);
    }

}

static void
lang_init_script_file ()
{
  script_file = lang_add_input_file ("command line",
				     lang_input_file_is_fake_enum,
				     (char *) NULL);
  script_file->the_bfd = bfd_create ("command line", output_bfd);
  script_file->symbol_count = 0;
  script_file->the_bfd->sections = 0;

  /* The user data of a bfd points to the input statement attatched */
  script_file->the_bfd->usrdata  = (void *)script_file;
  script_file->common_section =
   bfd_make_section(script_file->the_bfd,"COMMON");

  abs_output_section =
   lang_output_section_statement_lookup (BFD_ABS_SECTION_NAME);

  abs_output_section->bfd_section = &bfd_abs_section;

}

/* Open input files and attatch to output sections */
static void
map_input_to_output_sections (s, target, output_section_statement)
     lang_statement_union_type * s;
     CONST char *target;
     lang_output_section_statement_type * output_section_statement;
{
  for (; s != (lang_statement_union_type *) NULL; s = s->next)
    {
      switch (s->header.type)
	{


	case lang_wild_statement_enum:
	  wild (&s->wild_statement, s->wild_statement.section_name,
		s->wild_statement.filename, target,
		output_section_statement);

	  break;
	case lang_constructors_statement_enum:
	  map_input_to_output_sections (constructor_list.head,
					target,
					output_section_statement);
	  break;
	case lang_output_section_statement_enum:
	  map_input_to_output_sections (s->output_section_statement.children.head,
					target,
					&s->output_section_statement);
	  break;
	case lang_output_statement_enum:
	  break;
	case lang_target_statement_enum:
	  target = s->target_statement.target;
	  break;
	case lang_fill_statement_enum:
	case lang_input_section_enum:
	case lang_object_symbols_statement_enum:
	case lang_data_statement_enum:
	case lang_assignment_statement_enum:
	case lang_padding_statement_enum:
	  break;
	case lang_afile_asection_pair_statement_enum:
	  FAIL ();
	  break;
	case lang_address_statement_enum:
	  /* Mark the specified section with the supplied address */
	  {
	    lang_output_section_statement_type *os =
	    lang_output_section_statement_lookup
	    (s->address_statement.section_name);

	    os->addr_tree = s->address_statement.address;
	    if (os->bfd_section == (asection *) NULL)
	      {
		einfo ("%P%F: cannot set the address of undefined section %s\n",
		       s->address_statement.section_name);
	      }
	  }
	  break;
	case lang_input_statement_enum:
	  /* A standard input statement, has no wildcards */
	  /*	ldmain_open_file_read_symbol(&s->input_statement);*/
	  break;
	}
    }
}





static void
print_output_section_statement (output_section_statement)
     lang_output_section_statement_type * output_section_statement;
{
  asection *section = output_section_statement->bfd_section;

  print_nl ();
  print_section (output_section_statement->name);


  if (section)
  {
    print_dot = section->vma;
    print_space ();
    print_section ("");
    print_space ();
    print_address (section->vma);
    print_space ();
    print_size (section->_raw_size);
    print_space();
    print_size(section->_cooked_size);
    print_space ();
    print_alignment (section->alignment_power);
    print_space ();
#if 0
    fprintf (config.map_file, "%s flags", output_section_statement->region->name);
    print_flags (stdout, &output_section_statement->flags);
#endif
    if (section->flags & SEC_LOAD)
     fprintf (config.map_file, "load ");
    if (section->flags & SEC_ALLOC)
     fprintf (config.map_file, "alloc ");
    if (section->flags & SEC_RELOC)
     fprintf (config.map_file, "reloc ");
    if (section->flags & SEC_HAS_CONTENTS)
     fprintf (config.map_file, "contents ");

  }
  else
  {
    fprintf (config.map_file, "No attached output section");
  }
  print_nl ();
  if (output_section_statement->load_base)
    {
      int b = exp_get_value_int(output_section_statement->load_base,
				0, "output base", lang_final_phase_enum);
      printf("Output address   %08x\n", b);
    }
  if (output_section_statement->section_alignment >= 0
      || output_section_statement->section_alignment >= 0) 
  {
    printf("\t\t\t\t\tforced alignment ");
    if ( output_section_statement->section_alignment >= 0) 
    {
      printf("section 2**%d ",output_section_statement->section_alignment );
    }
    if ( output_section_statement->subsection_alignment >= 0) 
    {
      printf("subsection 2**%d ",output_section_statement->subsection_alignment );
    }
  
    print_nl ();
  }
  print_statement (output_section_statement->children.head,
		   output_section_statement);

}

static void
print_assignment (assignment, output_section)
     lang_assignment_statement_type * assignment;
     lang_output_section_statement_type * output_section;
{
  etree_value_type result;

  print_section ("");
  print_space ();
  print_section ("");
  print_space ();
  print_address (print_dot);
  print_space ();
  result = exp_fold_tree (assignment->exp->assign.src,
			  output_section,
			  lang_final_phase_enum,
			  print_dot,
			  &print_dot);

  if (result.valid)
    {
      print_address (result.value);
    }
  else
    {
      fprintf (config.map_file, "*undefined*");
    }
  print_space ();
  exp_print_tree (assignment->exp);

  fprintf (config.map_file, "\n");
}

static void
print_input_statement (statm)
     lang_input_statement_type * statm;
{
  if (statm->filename != (char *) NULL)
    {
      fprintf (config.map_file, "LOAD %s\n", statm->filename);
    }
}

static void
print_symbol (q)
     asymbol * q;
{
  print_section ("");
  fprintf (config.map_file, " ");
  print_section ("");
  fprintf (config.map_file, " ");
  print_address (outside_symbol_address (q));
  fprintf (config.map_file, "              %s", q->name ? q->name : " ");
  if (q->flags & BSF_WEAK)
    fprintf (config.map_file, " *weak*");
  print_nl ();
}

static void
print_input_section (in)
     lang_input_section_type * in;
{
  asection *i = in->section;
  int size = i->reloc_done ?
  bfd_get_section_size_after_reloc (i) :
  bfd_get_section_size_before_reloc (i);

  if (size != 0)
    {
      print_section ("");
      fprintf (config.map_file, " ");
      print_section (i->name);
      fprintf (config.map_file, " ");
      if (i->output_section)
	{
	  print_address (i->output_section->vma + i->output_offset);
	  fprintf (config.map_file, " ");
	  print_size (i->_raw_size);
	  fprintf (config.map_file, " ");
	  print_size(i->_cooked_size);
	  fprintf (config.map_file, " ");
	  print_alignment (i->alignment_power);
	  fprintf (config.map_file, " ");
	  if (in->ifile)
	    {

	      bfd *abfd = in->ifile->the_bfd;

	      if (in->ifile->just_syms_flag == true)
		{
		  fprintf (config.map_file, "symbols only ");
		}

	      fprintf (config.map_file, " %s ", abfd->xvec->name);
	      if (abfd->my_archive != (bfd *) NULL)
		{
		  fprintf (config.map_file, "[%s]%s", abfd->my_archive->filename,
			   abfd->filename);
		}
	      else
		{
		  fprintf (config.map_file, "%s", abfd->filename);
		}
	      fprintf (config.map_file, "(overhead %d bytes)", (int) bfd_alloc_size (abfd));
	      print_nl ();

	      /* Find all the symbols in this file defined in this section */

	      if (in->ifile->symbol_count)
		{
		  asymbol **p;

		  for (p = in->ifile->asymbols; *p; p++)
		    {
		      asymbol *q = *p;

		      if (bfd_get_section (q) == i
			  && (q->flags & (BSF_GLOBAL | BSF_WEAK)) != 0)
			{
			  print_symbol (q);
			}
		    }
		}
	    }
	  else
	    {
	      print_nl ();
	    }


	  print_dot = outside_section_address (i) + size;
	}
      else
	{
	  fprintf (config.map_file, "No output section allocated\n");
	}
    }
}

static void
print_fill_statement (fill)
     lang_fill_statement_type * fill;
{
  fprintf (config.map_file, "FILL mask ");
  print_fill (fill->fill);
}

static void
print_data_statement (data)
     lang_data_statement_type * data;
{
/*  bfd_vma value; */
  print_section ("");
  print_space ();
  print_section ("");
  print_space ();
/*  ASSERT(print_dot == data->output_vma);*/

  print_address (data->output_vma + data->output_section->vma);
  print_space ();
  print_address (data->value);
  print_space ();
  switch (data->type)
    {
    case BYTE:
      fprintf (config.map_file, "BYTE ");
      print_dot += BYTE_SIZE;
      break;
    case SHORT:
      fprintf (config.map_file, "SHORT ");
      print_dot += SHORT_SIZE;
      break;
    case LONG:
      fprintf (config.map_file, "LONG ");
      print_dot += LONG_SIZE;
      break;
    }

  exp_print_tree (data->exp);

  fprintf (config.map_file, "\n");
}


static void
print_padding_statement (s)
     lang_padding_statement_type * s;
{
  print_section ("");
  print_space ();
  print_section ("*fill*");
  print_space ();
  print_address (s->output_offset + s->output_section->vma);
  print_space ();
  print_size (s->size);
  print_space ();
  print_fill (s->fill);
  print_nl ();

  print_dot = s->output_offset + s->output_section->vma + s->size;

}

static void
print_wild_statement (w, os)
     lang_wild_statement_type * w;
     lang_output_section_statement_type * os;
{
  fprintf (config.map_file, " from ");
  if (w->filename != (char *) NULL)
    {
      fprintf (config.map_file, "%s", w->filename);
    }
  else
    {
      fprintf (config.map_file, "*");
    }
  if (w->section_name != (char *) NULL)
    {
      fprintf (config.map_file, "(%s)", w->section_name);
    }
  else
    {
      fprintf (config.map_file, "(*)");
    }
  print_nl ();
  print_statement (w->children.head, os);

}
static void
print_statement (s, os)
     lang_statement_union_type * s;
     lang_output_section_statement_type * os;
{
  while (s)
    {
      switch (s->header.type)
	{
	  case lang_constructors_statement_enum:
	  fprintf (config.map_file, "constructors:\n");
	  print_statement (constructor_list.head, os);
	  break;
	case lang_wild_statement_enum:
	  print_wild_statement (&s->wild_statement, os);
	  break;
	default:
	  fprintf (config.map_file, "Fail with %d\n", s->header.type);
	  FAIL ();
	  break;
	case lang_address_statement_enum:
	  fprintf (config.map_file, "address\n");
	  break;
	case lang_object_symbols_statement_enum:
	  fprintf (config.map_file, "object symbols\n");
	  break;
	case lang_fill_statement_enum:
	  print_fill_statement (&s->fill_statement);
	  break;
	case lang_data_statement_enum:
	  print_data_statement (&s->data_statement);
	  break;
	case lang_input_section_enum:
	  print_input_section (&s->input_section);
	  break;
	case lang_padding_statement_enum:
	  print_padding_statement (&s->padding_statement);
	  break;
	case lang_output_section_statement_enum:
	  print_output_section_statement (&s->output_section_statement);
	  break;
	case lang_assignment_statement_enum:
	  print_assignment (&s->assignment_statement,
			    os);
	  break;
	case lang_target_statement_enum:
	  fprintf (config.map_file, "TARGET(%s)\n", s->target_statement.target);
	  break;
	case lang_output_statement_enum:
	  fprintf (config.map_file, "OUTPUT(%s %s)\n",
		   s->output_statement.name,
		   output_target ? output_target : "");
	  break;
	case lang_input_statement_enum:
	  print_input_statement (&s->input_statement);
	  break;
	case lang_afile_asection_pair_statement_enum:
	  FAIL ();
	  break;
	}
      s = s->next;
    }
}


static void
print_statements ()
{
  print_statement (statement_list.head,
		   abs_output_section);

}

static bfd_vma
DEFUN (insert_pad, (this_ptr, fill, power, output_section_statement, dot),
     lang_statement_union_type ** this_ptr AND
     fill_type fill AND
     unsigned int power AND
     asection * output_section_statement AND
     bfd_vma dot)
{
  /* Align this section first to the
     input sections requirement, then
     to the output section's requirement.
     If this alignment is > than any seen before,
     then record it too. Perform the alignment by
     inserting a magic 'padding' statement.
     */

  unsigned int alignment_needed = align_power (dot, power) - dot;

  if (alignment_needed != 0)
    {
      lang_statement_union_type *new =
      (lang_statement_union_type *)
      stat_alloc ((bfd_size_type) (sizeof (lang_padding_statement_type)));

      /* Link into existing chain */
      new->header.next = *this_ptr;
      *this_ptr = new;
      new->header.type = lang_padding_statement_enum;
      new->padding_statement.output_section = output_section_statement;
      new->padding_statement.output_offset =
	dot - output_section_statement->vma;
      new->padding_statement.fill = fill;
      new->padding_statement.size = alignment_needed;
    }


  /* Remember the most restrictive alignment */
  if (power > output_section_statement->alignment_power)
    {
      output_section_statement->alignment_power = power;
    }
  output_section_statement->_raw_size += alignment_needed;
  return alignment_needed + dot;

}

/* Work out how much this section will move the dot point */
static bfd_vma
DEFUN (size_input_section, (this_ptr, output_section_statement, fill, dot, relax),
     lang_statement_union_type ** this_ptr AND
     lang_output_section_statement_type * output_section_statement AND
     unsigned short fill AND
     bfd_vma dot AND
     boolean relax)
{
  lang_input_section_type *is = &((*this_ptr)->input_section);
  asection *i = is->section;

  if (is->ifile->just_syms_flag == false)
    {
      if (output_section_statement->subsection_alignment != -1)
       i->alignment_power =
	output_section_statement->subsection_alignment;

      dot = insert_pad (this_ptr, fill, i->alignment_power,
			output_section_statement->bfd_section, dot);

      /* remember the largest size so we can malloc the largest area
         needed for the output stage. Only remember the size of sections
         which we will actually allocate  */
      if ((i->flags & SEC_HAS_CONTENTS) != 0
	  && (bfd_get_section_size_before_reloc (i) > largest_section))
	{
	  largest_section = bfd_get_section_size_before_reloc (i);
	}

      /* Remember where in the output section this input section goes */

      i->output_offset = dot - output_section_statement->bfd_section->vma;

      /* Mark how big the output section must be to contain this now
	 */
      if (relax)
	{
	  dot += i->_cooked_size;
	}
      else
	{
	  dot += i->_raw_size;
	}
      output_section_statement->bfd_section->_raw_size = dot - output_section_statement->bfd_section->vma;
    }
  else
    {
      i->output_offset = i->vma - output_section_statement->bfd_section->vma;
    }

  return dot;
}

/* Sizing happens in two passes, first pass we allocate worst case
   stuff. The second pass (if relaxing), we use what we learnt to
   change the size of some relocs from worst case to better
   */
static boolean had_relax;

static bfd_vma
DEFUN (lang_size_sections, (s, output_section_statement, prev, fill, dot, relax),
     lang_statement_union_type * s AND
     lang_output_section_statement_type * output_section_statement AND
     lang_statement_union_type ** prev AND
     unsigned short fill AND
     bfd_vma dot AND
     boolean relax)
{
  /* Size up the sections from their constituent parts */
  for (; s != (lang_statement_union_type *) NULL; s = s->next)
  {
    switch (s->header.type)
    {

     case lang_output_section_statement_enum:
     {
       bfd_vma after;
       lang_output_section_statement_type *os = &s->output_section_statement;

       /* If this is a shared library section, don't change the size
	  and address.  */
       if (os->bfd_section->flags & SEC_SHARED_LIBRARY)
	 break;

       if (os->bfd_section == &bfd_abs_section)
       {
	 /* No matter what happens, an abs section starts at zero */
	 bfd_set_section_vma (0, os->bfd_section, 0);
       }
       else
       {
	 if (os->addr_tree == (etree_type *) NULL)
	 {
	   /* No address specified for this section, get one
	      from the region specification
	      */
	   if (os->region == (lang_memory_region_type *) NULL)
	   {
	     os->region = lang_memory_region_lookup ("*default*");
	   }
	   dot = os->region->current;
	 }
	 else
	 {
	   etree_value_type r;

	   r = exp_fold_tree (os->addr_tree,
			      abs_output_section,
			      lang_allocating_phase_enum,
			      dot, &dot);
	   if (r.valid == false)
	   {
	     einfo ("%F%S: non constant address expression for section %s\n",
		    os->name);
	   }
	   dot = r.value;
	 }
	 /* The section starts here */
	 /* First, align to what the section needs */


	 dot = align_power (dot, os->bfd_section->alignment_power);
	 bfd_set_section_vma (0, os->bfd_section, dot);
	 
	 if (os->load_base) {
	   os->bfd_section->lma 
	     = exp_get_value_int(os->load_base, 0,"load base", lang_final_phase_enum);
	 }
       }


       os->bfd_section->output_offset = 0;

       (void) lang_size_sections (os->children.head, os, &os->children.head,
				  os->fill, dot, relax);
       /* Ignore the size of the input sections, use the vma and size to */
       /* align against */

       after = ALIGN_N (os->bfd_section->vma +
			os->bfd_section->_raw_size,
			/* The coercion here is important, see ld.h.  */
			(bfd_vma) os->block_value);

       os->bfd_section->_raw_size = after - os->bfd_section->vma;
       dot = os->bfd_section->vma + os->bfd_section->_raw_size;
       os->processed = true;

       /* Replace into region ? */
       if (os->addr_tree == (etree_type *) NULL
	   && os->region != (lang_memory_region_type *) NULL)
       {
	 os->region->current = dot;
	 /* Make sure this isn't silly */
	 if (( os->region->current
              > os->region->origin + os->region->length)
	     || ( os->region->origin > os->region->current ))
	   {
	     einfo ("%X%P: region %s is full (%B section %s)\n",
		    os->region->name,
		    os->bfd_section->owner,
		    os->bfd_section->name);
	     /* Reset the region pointer */
	     os->region->current = 0;

	   }

       }
     }

      break;
     case lang_constructors_statement_enum:
      dot = lang_size_sections (constructor_list.head,
				output_section_statement,
				&s->wild_statement.children.head,
				fill,
				dot, relax);
      break;

     case lang_data_statement_enum:
     {
       unsigned int size = 0;

       s->data_statement.output_vma = dot - output_section_statement->bfd_section->vma;
       s->data_statement.output_section =
	output_section_statement->bfd_section;

       switch (s->data_statement.type)
       {
	case LONG:
	 size = LONG_SIZE;
	 break;
	case SHORT:
	 size = SHORT_SIZE;
	 break;
	case BYTE:
	 size = BYTE_SIZE;
	 break;

       }
       dot += size;
       output_section_statement->bfd_section->_raw_size += size;
     }
      break;

     case lang_wild_statement_enum:

      dot = lang_size_sections (s->wild_statement.children.head,
				output_section_statement,
				&s->wild_statement.children.head,

				fill, dot, relax);

      break;

     case lang_object_symbols_statement_enum:
      create_object_symbols = output_section_statement;
      break;
     case lang_output_statement_enum:
     case lang_target_statement_enum:
      break;
     case lang_input_section_enum:
      if (relax)
      {
	relaxing = true;

	if( relax_section (prev))
	 had_relax = true;
	relaxing = false;

      }
      else  {
	(*prev)->input_section.section->_cooked_size = 
	 (*prev)->input_section.section->_raw_size ;

      }
      dot = size_input_section (prev,
				output_section_statement,
				output_section_statement->fill,
				dot, relax);
      break;
     case lang_input_statement_enum:
      break;
     case lang_fill_statement_enum:
      s->fill_statement.output_section = output_section_statement->bfd_section;

      fill = s->fill_statement.fill;
      break;
     case lang_assignment_statement_enum:
     {
       bfd_vma newdot = dot;

       exp_fold_tree (s->assignment_statement.exp,
		      output_section_statement,
		      lang_allocating_phase_enum,
		      dot,
		      &newdot);

       if (newdot != dot && !relax)
	/* We've been moved ! so insert a pad */
       {
	 lang_statement_union_type *new =
	  (lang_statement_union_type *)
	   stat_alloc ((bfd_size_type) (sizeof (lang_padding_statement_type)));

	 /* Link into existing chain */
	 new->header.next = *prev;
	 *prev = new;
	 new->header.type = lang_padding_statement_enum;
	 new->padding_statement.output_section =
	  output_section_statement->bfd_section;
	 new->padding_statement.output_offset =
	  dot - output_section_statement->bfd_section->vma;
	 new->padding_statement.fill = fill;
	 new->padding_statement.size = newdot - dot;
	 output_section_statement->bfd_section->_raw_size +=
	  new->padding_statement.size;
	 dot = newdot;
       }
     }

      break;
     default:
      FAIL ();
      break;
      /* This can only get here when relaxing is turned on */
     case lang_padding_statement_enum:

     case lang_address_statement_enum:
      break;
    }
    prev = &s->header.next;
  }
  return dot;
}

static bfd_vma
DEFUN (lang_do_assignments, (s, output_section_statement, fill, dot),
     lang_statement_union_type * s AND
     lang_output_section_statement_type * output_section_statement AND
     unsigned short fill AND
     bfd_vma dot)
{

  for (; s != (lang_statement_union_type *) NULL; s = s->next)
    {
      switch (s->header.type)
	{
	case lang_constructors_statement_enum:
	  dot = lang_do_assignments (constructor_list.head,
				     output_section_statement,
				     fill,
				     dot);
	  break;

	case lang_output_section_statement_enum:
	  {
	    lang_output_section_statement_type *os =
	    &(s->output_section_statement);

	    dot = os->bfd_section->vma;
	    (void) lang_do_assignments (os->children.head, os, os->fill, dot);
	    dot = os->bfd_section->vma + os->bfd_section->_raw_size;
	  }
	  break;
	case lang_wild_statement_enum:

	  dot = lang_do_assignments (s->wild_statement.children.head,
				     output_section_statement,
				     fill, dot);

	  break;

	case lang_object_symbols_statement_enum:
	case lang_output_statement_enum:
	case lang_target_statement_enum:
#if 0
	case lang_common_statement_enum:
#endif
	  break;
	case lang_data_statement_enum:
	  {
	    etree_value_type value;

	    value = exp_fold_tree (s->data_statement.exp,
				   abs_output_section,
				   lang_final_phase_enum, dot, &dot);
	    s->data_statement.value = value.value;
	    if (value.valid == false)
	      einfo ("%F%P: invalid data statement\n");
	  }
	  switch (s->data_statement.type)
	    {
	    case LONG:
	      dot += LONG_SIZE;
	      break;
	    case SHORT:
	      dot += SHORT_SIZE;
	      break;
	    case BYTE:
	      dot += BYTE_SIZE;
	      break;
	    }
	  break;
	case lang_input_section_enum:
	  {
	    asection *in = s->input_section.section;

	    dot += bfd_get_section_size_before_reloc (in);
	  }
	  break;

	case lang_input_statement_enum:
	  break;
	case lang_fill_statement_enum:
	  fill = s->fill_statement.fill;
	  break;
	case lang_assignment_statement_enum:
	  {
	    exp_fold_tree (s->assignment_statement.exp,
			   output_section_statement,
			   lang_final_phase_enum,
			   dot,
			   &dot);
	  }

	  break;
	case lang_padding_statement_enum:
	  dot += s->padding_statement.size;
	  break;
	default:
	  FAIL ();
	  break;
	case lang_address_statement_enum:
	  break;
	}

    }
  return dot;
}



static void
lang_relocate_globals ()
{
  /*
    Each ldsym_type maintains a chain of pointers to asymbols which
    references the definition.  Replace each pointer to the referenence
    with a pointer to only one place, preferably the definition. If
    the defintion isn't available then the common symbol, and if
    there isn't one of them then choose one reference.
    */

  FOR_EACH_LDSYM (lgs)
  {
    asymbol *it;

    /* Skip indirect symbols.  */
    if (lgs->flags & SYM_INDIRECT)
      continue;

    if (lgs->sdefs_chain)
      {
	it = *(lgs->sdefs_chain);
      }
    else if (lgs->scoms_chain != (asymbol **) NULL)
      {
	it = *(lgs->scoms_chain);
      }
    else if (lgs->srefs_chain != (asymbol **) NULL)
      {
	it = *(lgs->srefs_chain);
      }
    else
      {
	/* This can happen when the command line asked for a symbol to
	   be -u */
	it = (asymbol *) NULL;
      }
    if (it != (asymbol *) NULL)
      {
	asymbol **prev = 0;
	asymbol **ptr = lgs->srefs_chain;;
	if (lgs->flags & SYM_WARNING)
	  {
	    produce_warnings (lgs, it);
	  }

	while (ptr != (asymbol **) NULL
	       && ptr != prev)
	  {
	    asymbol *ref = *ptr;
	    prev = ptr;
	    *ptr = it;
	    ptr = (asymbol **) (ref->udata);
	  }
      }
  }
}



static void
lang_finish ()
{
  ldsym_type *lgs;
  int warn = config.relocateable_output != true;
  if (entry_symbol == (char *) NULL)
  {
    /* No entry has been specified, look for start, but don't warn */
    entry_symbol = "start";
    warn =0;
  }
  lgs = ldsym_get_soft (entry_symbol);
  if (lgs && lgs->sdefs_chain)
  {
    asymbol *sy = *(lgs->sdefs_chain);

    /* We can set the entry address*/
    bfd_set_start_address (output_bfd,
			   outside_symbol_address (sy));

  }
  else
  {
    /* Cannot find anything reasonable,
       use the first address in the text section
       */
    asection *ts = bfd_get_section_by_name (output_bfd, ".text");
    if (ts)
    {
      if (warn)
       einfo ("%P: warning: cannot find entry symbol %s, defaulting to %V\n",
	      entry_symbol, ts->vma);

      bfd_set_start_address (output_bfd, ts->vma);
    }
    else 
    {
      if (warn)
       einfo ("%P: warning: cannot find entry symbol %s, not setting start address\n",
	      entry_symbol);
    }
  }
}

/* By now we know the target architecture, and we may have an */
/* ldfile_output_machine_name */
static void
lang_check ()
{
  lang_statement_union_type *file;
  bfd *input_bfd;
  unsigned long input_machine;
  enum bfd_architecture input_architecture;
  CONST bfd_arch_info_type *compatible;

  for (file = file_chain.head;
       file != (lang_statement_union_type *) NULL;
       file = file->input_statement.next)
    {
      input_bfd = file->input_statement.the_bfd;

      input_machine = bfd_get_mach (input_bfd);
      input_architecture = bfd_get_arch (input_bfd);


      /* Inspect the architecture and ensure we're linking like with
         like */

      compatible = bfd_arch_get_compatible (input_bfd,
					    output_bfd);
 
      if (compatible)
	{
	  ldfile_output_machine = compatible->mach;
	  ldfile_output_architecture = compatible->arch;
	}
      else
	{

	  info_msg ("%P: warning: %s architecture of input file `%B' is incompatible with %s output\n",
		bfd_printable_name (input_bfd), input_bfd,
		bfd_printable_name (output_bfd));

	  if (! bfd_set_arch_mach (output_bfd,
				   input_architecture,
				   input_machine))
	    einfo ("%P%F:%s: can't set architecture: %E\n",
		   bfd_get_filename (output_bfd));
	}

    }
}

/*
 * run through all the global common symbols and tie them
 * to the output section requested.
 *
 As an experiment we do this 4 times, once for all the byte sizes,
 then all the two  bytes, all the four bytes and then everything else
  */

static void
lang_common ()
{
  ldsym_type *lgs;
  size_t power;

  if (config.relocateable_output == false ||
      command_line.force_common_definition == true)
    {
      for (power = 1; (config.sort_common == true && power == 1) || (power <= 16); power <<= 1)
	{
	  for (lgs = symbol_head;
	       lgs != (ldsym_type *) NULL;
	       lgs = lgs->next)
	    {
	      asymbol *com;
	      unsigned int power_of_two;
	      size_t size;
	      size_t align;

	      if (lgs->scoms_chain != (asymbol **) NULL)
		{
		  com = *(lgs->scoms_chain);
		  size = com->value;
		  switch (size)
		    {
		    case 0:
		    case 1:
		      align = 1;
		      power_of_two = 0;
		      break;
		    case 2:
		      power_of_two = 1;
		      align = 2;
		      break;
		    case 3:
		    case 4:
		      power_of_two = 2;
		      align = 4;
		      break;
		    case 5:
		    case 6:
		    case 7:
		    case 8:
		      power_of_two = 3;
		      align = 8;
		      break;
		    default:
		      power_of_two = 4;
		      align = 16;
		      break;
		    }
		  if (config.sort_common == false || align == power)
		    {
		      bfd *symbfd;

		      /* Change from a common symbol into a definition of
			 a symbol */
		      lgs->sdefs_chain = lgs->scoms_chain;
		      lgs->scoms_chain = (asymbol **) NULL;
		      commons_pending--;

		      /* Point to the correct common section */
		      symbfd = bfd_asymbol_bfd (com);
		      if (com->section == &bfd_com_section)
			com->section =
			  ((lang_input_statement_type *) symbfd->usrdata)
			    ->common_section;
		      else
			{
			  CONST char *name;
			  asection *newsec;

			  name = bfd_get_section_name (symbfd,
						       com->section);
			  newsec = bfd_get_section_by_name (symbfd,
							    name);
			  /* BFD backend must provide this section. */
			  if (newsec == (asection *) NULL)
			    einfo ("%P%F: no output section %s", name);
			  com->section = newsec;
			}

		      /*  Fix the size of the common section */

		      com->section->_raw_size =
			ALIGN_N (com->section->_raw_size,
				 /* The coercion here is important, see ld.h.  */
				 (bfd_vma) align);

		      /* Remember if this is the biggest alignment ever seen */
		      if (power_of_two > com->section->alignment_power)
			{
			  com->section->alignment_power = power_of_two;
			}

		      /* Symbol stops being common and starts being global, but
			 we remember that it was common once. */

		      com->flags = BSF_EXPORT | BSF_GLOBAL | BSF_OLD_COMMON;
		      com->value = com->section->_raw_size;

		      if (write_map && config.map_file)
			{
			  fprintf (config.map_file, "Allocating common %s: %x at %x %s\n",
				   lgs->name,
				   (unsigned) size,
				   (unsigned) com->value,
				   bfd_asymbol_bfd(com)->filename);
			}

		      com->section->_raw_size += size;

		    }
		}

	    }
	}
    }


}

/*
run through the input files and ensure that every input
section has somewhere to go. If one is found without
a destination then create an input request and place it
into the statement tree.
*/

static void
lang_place_orphans ()
{
  lang_input_statement_type *file;

  for (file = (lang_input_statement_type *) file_chain.head;
       file != (lang_input_statement_type *) NULL;
       file = (lang_input_statement_type *) file->next)
    {
      asection *s;

      for (s = file->the_bfd->sections;
	   s != (asection *) NULL;
	   s = s->next)
	{
	  if (s->output_section == (asection *) NULL)
	    {
	      /* This section of the file is not attatched, root
	         around for a sensible place for it to go */

	      if (file->common_section == s)
		{
		  /* This is a lonely common section which must
		     have come from an archive. We attatch to the
		     section with the wildcard  */
		  if (config.relocateable_output != true
		      && command_line.force_common_definition == false)
		    {
		      if (default_common_section ==
			  (lang_output_section_statement_type *) NULL)
			{
			  info_msg ("%P: no [COMMON] command, defaulting to .bss\n");

			  default_common_section =
			    lang_output_section_statement_lookup (".bss");

			}
		      wild_doit (&default_common_section->children, s,
				 default_common_section, file);
		    }
		}
	      else
		{
		  lang_output_section_statement_type *os =
		  lang_output_section_statement_lookup (s->name);

		  wild_doit (&os->children, s, os, file);
		}
	    }
	}
    }
}


void
lang_set_flags (ptr, flags)
     int *ptr;
     CONST char *flags;
{
  boolean state = false;

  *ptr = 0;
  while (*flags)
    {
      if (*flags == '!')
	{
	  state = false;
	  flags++;
	}
      else
	state = true;
      switch (*flags)
	{
	case 'R':
	  /*	  ptr->flag_read = state; */
	  break;
	case 'W':
	  /*	  ptr->flag_write = state; */
	  break;
	case 'X':
	  /*	  ptr->flag_executable= state;*/
	  break;
	case 'L':
	case 'I':
	  /*	  ptr->flag_loadable= state;*/
	  break;
	default:
	  einfo ("%P%F: invalid syntax in flags\n");
	  break;
	}
      flags++;
    }
}



void
lang_for_each_file (func)
     void (*func) PARAMS ((lang_input_statement_type *));
{
  lang_input_statement_type *f;

  for (f = (lang_input_statement_type *) file_chain.head;
       f != (lang_input_statement_type *) NULL;
       f = (lang_input_statement_type *) f->next)
    {
      func (f);
    }
}


void
lang_for_each_input_section (func)
     void (*func) PARAMS ((bfd * ab, asection * as));
{
  lang_input_statement_type *f;

  for (f = (lang_input_statement_type *) file_chain.head;
       f != (lang_input_statement_type *) NULL;
       f = (lang_input_statement_type *) f->next)
    {
      asection *s;

      for (s = f->the_bfd->sections;
	   s != (asection *) NULL;
	   s = s->next)
	{
	  func (f->the_bfd, s);
	}
    }
}



void
ldlang_add_file (entry)
     lang_input_statement_type * entry;
{

  lang_statement_append (&file_chain,
			 (lang_statement_union_type *) entry,
			 &entry->next);
}

void
lang_add_output (name, from_script)
     CONST char *name;
     int from_script;
{
  /* Make -o on command line override OUTPUT in script.  */
  if (had_output_filename == false || !from_script)
    {
      output_filename = name;
      had_output_filename = true;
    }
}


static lang_output_section_statement_type *current_section;

static int topower(x)
 int x;
{
  unsigned  int i = 1;
  int l;
  if (x < 0) return -1;
  for (l = 0; l < 32; l++) 
  {
    if (i >= x) return l;
    i<<=1;
  }
  return 0;
}
void
lang_enter_output_section_statement (output_section_statement_name,
				     address_exp, flags, block_value,
				     align, subalign, base)
     char *output_section_statement_name;
     etree_type * address_exp;
     int flags;
     bfd_vma block_value;
     etree_type *align;
     etree_type *subalign;
     etree_type *base;
{
  lang_output_section_statement_type *os;

  current_section =
   os =
    lang_output_section_statement_lookup (output_section_statement_name);



  /* Add this statement to tree */
  /*  add_statement(lang_output_section_statement_enum,
      output_section_statement);*/
  /* Make next things chain into subchain of this */

  if (os->addr_tree ==
      (etree_type *) NULL)
  {
    os->addr_tree =
     address_exp;
  }
  os->flags = flags;
  if (flags & SEC_NEVER_LOAD)
   os->loadable = 0;
  else
   os->loadable = 1;
  os->block_value = block_value ? block_value : 1;
  stat_ptr = &os->children;

  os->subsection_alignment = topower(
   exp_get_value_int(subalign, -1,
		     "subsection alignment",
		     0));
  os->section_alignment = topower(
   exp_get_value_int(align, -1,
		     "section alignment", 0));

  os->load_base = base;
}


void
lang_final ()
{
  lang_output_statement_type *new =
    new_stat (lang_output_statement, stat_ptr);

  new->name = output_filename;
}

/* Reset the current counters in the regions */
static void
reset_memory_regions ()
{
  lang_memory_region_type *p = lang_memory_region_list;

  for (p = lang_memory_region_list;
       p != (lang_memory_region_type *) NULL;
       p = p->next)
    {
      p->old_length = (bfd_size_type) (p->current - p->origin);
      p->current = p->origin;
    }
}



asymbol *
DEFUN (create_symbol, (name, flags, section),
     CONST char *name AND
     flagword flags AND
     asection * section)
{
  extern lang_input_statement_type *script_file;
  asymbol **def_ptr = (asymbol **) stat_alloc ((bfd_size_type) (sizeof (asymbol **)));

  /* Add this definition to script file */
  asymbol *def = (asymbol *) bfd_make_empty_symbol (script_file->the_bfd);
  def->name = buystring (name);
  def->udata = 0;
  def->flags = flags;
  def->section = section;
  *def_ptr = def;
  enter_global_ref (def_ptr, name);
  return def;
}

void
lang_process ()
{
  lang_reasonable_defaults ();
  current_target = default_target;

  lang_for_each_statement (ldlang_open_output);	/* Open the output file */
  /* For each output section statement, create a section in the output
     file */
  lang_create_output_section_statements ();

  ldemul_create_output_section_statements ();

  /* Create a dummy bfd for the script */
  lang_init_script_file ();

  /* Add to the hash table all undefineds on the command line */
  lang_place_undefineds ();

  /* Create a bfd for each input file */
  current_target = default_target;
  lang_for_each_statement (open_input_bfds);

  /* Run through the contours of the script and attatch input sections
     to the correct output sections
     */
  find_constructors ();
  map_input_to_output_sections (statement_list.head, (char *) NULL,
				(lang_output_section_statement_type *) NULL);


  /* Find any sections not attatched explicitly and handle them */
  lang_place_orphans ();

  /* Size up the common data */
  lang_common ();

  ldemul_before_allocation ();


#if 0
  had_relax = true;
  while (had_relax)
    {

      had_relax = false;

      lang_size_sections (statement_list.head,
			  (lang_output_section_statement_type *) NULL,
			  &(statement_list.head), 0, (bfd_vma) 0, true);
      /* FIXME. Until the code in relax is fixed so that it only reads in
         stuff once, we cant iterate since there is no way for the linker to
         know what has been patched and what hasn't */
      break;

    }
#endif

  /* Now run around and relax if we can */
  if (command_line.relax)
    {
      /* First time round is a trial run to get the 'worst case'
	 addresses of the objects if there was no relaxing.  */
      lang_size_sections (statement_list.head,
			  (lang_output_section_statement_type *) NULL,
			  &(statement_list.head), 0, (bfd_vma) 0, false);

      /* Move the global symbols around so the second pass of relaxing
	 can see them.  */
      lang_relocate_globals ();

      reset_memory_regions ();

      /* Do all the assignments, now that we know the final resting
	 places of all the symbols.  */

      lang_do_assignments (statement_list.head,
			   abs_output_section,
			   0, (bfd_vma) 0);

      /* Perform another relax pass - this time we know where the
	 globals are, so can make better guess.  */
      lang_size_sections (statement_list.head,
			  (lang_output_section_statement_type *) NULL,
			  &(statement_list.head), 0, (bfd_vma) 0, true);
    }
  else
    {
      /* Size up the sections.  */
      lang_size_sections (statement_list.head,
			  abs_output_section,
			  &(statement_list.head), 0, (bfd_vma) 0, false);
    }

  /* See if anything special should be done now we know how big
     everything is.  */
  ldemul_after_allocation ();

  /* Do all the assignments, now that we know the final restingplaces
     of all the symbols */

  lang_do_assignments (statement_list.head,
		       abs_output_section,
		       0, (bfd_vma) 0);


  /* Move the global symbols around */
  lang_relocate_globals ();

  /* Make sure that we're not mixing architectures */

  lang_check ();

  /* Final stuffs */

  ldemul_finish ();
  /* Size up the sections.  */
  lang_size_sections (statement_list.head,
		      abs_output_section,
		      &(statement_list.head), 0, (bfd_vma) 0, false);
  lang_finish ();
}

/* EXPORTED TO YACC */

void
lang_add_wild (section_name, filename)
     CONST char *CONST section_name;
     CONST char *CONST filename;
{
  lang_wild_statement_type *new = new_stat (lang_wild_statement,
					    stat_ptr);

  if (section_name != (char *) NULL && strcmp (section_name, "COMMON") == 0)
    {
      placed_commons = true;
    }
  if (filename != (char *) NULL)
    {
      lang_has_input_file = true;
    }
  new->section_name = section_name;
  new->filename = filename;
  lang_list_init (&new->children);
}

void
lang_section_start (name, address)
     CONST char *name;
     etree_type * address;
{
  lang_address_statement_type *ad = new_stat (lang_address_statement, stat_ptr);

  ad->section_name = name;
  ad->address = address;
}

void
lang_add_entry (name)
     CONST char *name;
{
  entry_symbol = name;
}

void
lang_add_target (name)
     CONST char *name;
{
  lang_target_statement_type *new = new_stat (lang_target_statement,
					      stat_ptr);

  new->target = name;

}

void
lang_add_map (name)
     CONST char *name;
{
  while (*name)
    {
      switch (*name)
	{
	  case 'F':
	  map_option_f = true;
	  break;
	}
      name++;
    }
}

void
lang_add_fill (exp)
     int exp;
{
  lang_fill_statement_type *new = new_stat (lang_fill_statement,
					    stat_ptr);

  new->fill = exp;
}

void
lang_add_data (type, exp)
     int type;
     union etree_union *exp;
{

  lang_data_statement_type *new = new_stat (lang_data_statement,
					    stat_ptr);

  new->exp = exp;
  new->type = type;

}

void
lang_add_assignment (exp)
     etree_type * exp;
{
  lang_assignment_statement_type *new = new_stat (lang_assignment_statement,
						  stat_ptr);

  new->exp = exp;
}

void
lang_add_attribute (attribute)
     enum statement_enum attribute;
{
  new_statement (attribute, sizeof (lang_statement_union_type), stat_ptr);
}

void
lang_startup (name)
     CONST char *name;
{
  if (startup_file != (char *) NULL)
    {
      einfo ("%P%Fmultiple STARTUP files\n");
    }
  first_file->filename = name;
  first_file->local_sym_name = name;

  startup_file = name;
}

void
lang_float (maybe)
     boolean maybe;
{
  lang_float_flag = maybe;
}

void
lang_leave_output_section_statement (fill, memspec)
     bfd_vma fill;
     CONST char *memspec;
{
  current_section->fill = fill;
  current_section->region = lang_memory_region_lookup (memspec);
  stat_ptr = &statement_list;

  /* We remember if we are closing a .data section, since we use it to
     store constructors in */
  if (strcmp (current_section->name, ".data") == 0)
    {
      end_of_data_section_statement_list = statement_list;

    }
}

/*
 Create an absolute symbol with the given name with the value of the
 address of first byte of the section named.

 If the symbol already exists, then do nothing.
*/
void
lang_abs_symbol_at_beginning_of (section, name)
     CONST char *section;
     CONST char *name;
{
  if (ldsym_undefined (name))
    {
      asection *s = bfd_get_section_by_name (output_bfd, section);
      asymbol *def = create_symbol (name,
				    BSF_GLOBAL | BSF_EXPORT,
				    &bfd_abs_section);

      if (s != (asection *) NULL)
	{
	  def->value = s->vma;
	}
      else
	{
	  def->value = 0;
	}
    }
}

/*
 Create an absolute symbol with the given name with the value of the
 address of the first byte after the end of the section named.

 If the symbol already exists, then do nothing.
*/
void
lang_abs_symbol_at_end_of (section, name)
     CONST char *section;
     CONST char *name;
{
  if (ldsym_undefined (name))
    {
      asection *s = bfd_get_section_by_name (output_bfd, section);

      /* Add a symbol called _end */
      asymbol *def = create_symbol (name,
				    BSF_GLOBAL | BSF_EXPORT,
				    &bfd_abs_section);

      if (s != (asection *) NULL)
	{
	  def->value = s->vma + s->_raw_size;
	}
      else
	{
	  def->value = 0;
	}
    }
}

void
lang_statement_append (list, element, field)
     lang_statement_list_type * list;
     lang_statement_union_type * element;
     lang_statement_union_type ** field;
{
  *(list->tail) = element;
  list->tail = field;
}

/* Set the output format type.  -oformat overrides scripts.  */
void
lang_add_output_format (format, from_script)
     CONST char *format;
     int from_script;
{
  if (output_target == NULL || !from_script)
    output_target = format;
}
