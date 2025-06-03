
/* Copyright (C) 1991 Free Software Foundation, Inc.

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

/*
 ldfile.c

 look after all the file stuff

 */

#include "bfd.h"
#include "sysdep.h"

#include "ldmisc.h"
#include "ldlang.h"
#include "ldfile.h"
#include <ctype.h>
/* EXPORT */
char *ldfile_input_filename;
CONST char * ldfile_output_machine_name ="";
unsigned long ldfile_output_machine;
enum bfd_architecture ldfile_output_architecture;

/* IMPORT */

extern boolean had_script;
extern boolean trace_file_tries;


#ifdef VMS
char *slash = "";
#else
char *slash = "/";
#endif




/* LOCAL */
typedef struct search_dirs 
{
  char *name;
  struct search_dirs *next;
} search_dirs_type;

static search_dirs_type *search_head;
static search_dirs_type **search_tail_ptr = &search_head;

typedef struct search_arch 
{
  char *name; 
  struct search_arch *next;
} search_arch_type;

static search_arch_type *search_arch_head;
static search_arch_type **search_arch_tail_ptr = &search_arch_head;
 


void
ldfile_add_library_path(name)
char *name;
{
  search_dirs_type *new =
    (search_dirs_type *)ldmalloc((bfd_size_type)(sizeof(search_dirs_type)));
  new->name = name;
  new->next = (search_dirs_type*)NULL;
  *search_tail_ptr = new;
  search_tail_ptr = &new->next;
}


static bfd*
cached_bfd_openr(attempt,entry)
char *attempt;
lang_input_statement_type  *entry;
{
  entry->the_bfd = bfd_openr(attempt, entry->target);
  if (trace_file_tries == true ) {
    info_msg ("attempt to open %s %s\n", attempt,
		(entry->the_bfd == (bfd *)NULL) ? "failed" : "succeeded" );
  }
  return entry->the_bfd;
}

static bfd *
open_a(arch, entry, lib, suffix)
char *arch;
lang_input_statement_type *entry;
char *lib;
char *suffix;
{
  bfd*desc;
  search_dirs_type *search ;
  for (search = search_head;
       search != (search_dirs_type *)NULL;
       search = search->next) 
    {
      char buffer[1000];
      char *string;
      if (entry->is_archive == true) {
	sprintf(buffer,
		"%s%s%s%s%s%s",
		search->name,
		slash,
		lib,
		entry->filename, arch, suffix);
      }
      else {
	if (entry->filename[0] == '/' || entry->filename[0] == '.') {
	  strcpy(buffer, entry->filename);
	} else {
	  sprintf(buffer,"%s%s%s",search->name, slash, entry->filename);
	} 
      }
      string = buystring(buffer);      
      desc = cached_bfd_openr (string, entry);
      if (desc)
	{
	  entry->filename = string;
	  entry->search_dirs_flag = false;
	  entry->the_bfd =  desc;
	  return desc;
	}
      free(string);
    }
  return (bfd *)NULL;
}

/* Open the input file specified by 'entry', and return a descriptor.
   The open file is remembered; if the same file is opened twice in a row,
   a new open is not actually done.  */

void
ldfile_open_file (entry)
lang_input_statement_type *entry;
{

  if (entry->superfile)
    ldfile_open_file (entry->superfile);

  if (entry->search_dirs_flag)
    {
      search_arch_type *arch;
      /* Try to open <filename><suffix> or lib<filename><suffix>.a */
  
      for (arch = search_arch_head;
	   arch != (search_arch_type *)NULL;
	   arch = arch->next) {
	if (open_a(arch->name,entry,"lib",".a") != (bfd *)NULL) {
	  return;
	}
#ifdef VMS
	if (open_a(arch->name,entry,":lib",".a") != (bfd *)NULL) {
	  return;
	}
#endif

      }

    }
  else {
    entry->the_bfd = cached_bfd_openr (entry->filename, entry);
  }

  if (!entry->the_bfd)
    einfo("%F%P: cannot open %s: %E\n", entry->local_sym_name);
}


/* Try to open NAME; if that fails, try NAME with EXTEN appended to it.  */

static FILE *
try_open(name, exten)
char *name;
char *exten;
{
  FILE *result;
  char buff[1000];

  result = fopen(name, "r");
  if (trace_file_tries == true) {
    if (result == (FILE *)NULL) {
      info_msg ("cannot find ");
    }
    info_msg ("%s\n",name);
  }
  if (result != (FILE *)NULL) {
    return result;
  }

  if (*exten) {
    sprintf(buff, "%s%s", name, exten);
    result = fopen(buff, "r");
    if (trace_file_tries == true) {
      if (result == (FILE *)NULL) {
	info_msg ("cannot find ");
      }
      info_msg ("%s\n", buff);
    }
  }
  return result;
}

/* Try to open NAME; if that fails, look for it in any directories
   specified with -L, without and with EXTEND apppended.  */

FILE *
ldfile_find_command_file(name, extend)
char *name;
char *extend;
{
  search_dirs_type *search;
  FILE *result;
  char buffer[1000];

  /* First try raw name */
  result = try_open(name,"");
  if (result == (FILE *)NULL) {
    /* Try now prefixes */
    for (search = search_head;
	 search != (search_dirs_type *)NULL;
	 search = search->next) {
      sprintf(buffer,"%s/%s", search->name, name);
      result = try_open(buffer, extend);
      if (result)break;
    }
  }
  return result;
}

void
ldfile_open_command_file(name)
char *name;
{
  FILE *ldlex_input_stack;
  ldlex_input_stack = ldfile_find_command_file(name, "");

  if (ldlex_input_stack == (FILE *)NULL) {
    einfo("%P%F: cannot open linker script file %s: %E\n",name);
  }
  lex_push_file(ldlex_input_stack, name);
  
  ldfile_input_filename = name;
  had_script = true;
}





#ifdef GNU960
static
char *
gnu960_map_archname( name )
char *name;
{
  struct tabentry { char *cmd_switch; char *arch; };
  static struct tabentry arch_tab[] = {
	"",   "",
	"KA", "ka",
	"KB", "kb",
	"KC", "mc",	/* Synonym for MC */
	"MC", "mc",
	"CA", "ca",
	"SA", "ka",	/* Functionally equivalent to KA */
	"SB", "kb",	/* Functionally equivalent to KB */
	NULL, ""
  };
  struct tabentry *tp;
  

  for ( tp = arch_tab; tp->cmd_switch != NULL; tp++ ){
    if ( !strcmp(name,tp->cmd_switch) ){
      break;
    }
  }

  if ( tp->cmd_switch == NULL ){
    einfo("%P%F: unknown architecture: %s\n",name);
  }
  return tp->arch;
}



void
ldfile_add_arch(name)
char *name;
{
  search_arch_type *new =
    (search_arch_type *)ldmalloc((bfd_size_type)(sizeof(search_arch_type)));


  if (*name != '\0') {
    if (ldfile_output_machine_name[0] != '\0') {
      einfo("%P%F: target architecture respecified\n");
      return;
    }
    ldfile_output_machine_name = name;
  }

  new->next = (search_arch_type*)NULL;
  new->name = gnu960_map_archname( name );
  *search_arch_tail_ptr = new;
  search_arch_tail_ptr = &new->next;

}

#else	/* not GNU960 */


void
ldfile_add_arch (in_name)
     CONST char * in_name;
{
  char *name = buystring(in_name);
  search_arch_type *new =
    (search_arch_type *)ldmalloc((bfd_size_type)(sizeof(search_arch_type)));

  ldfile_output_machine_name = in_name;

  new->name = name;
  new->next = (search_arch_type*)NULL;
  while (*name) {
    if (isupper(*name)) *name = tolower(*name);
    name++;
  }
  *search_arch_tail_ptr = new;
  search_arch_tail_ptr = &new->next;

}
#endif

/* Set the output architecture */
void
ldfile_set_output_arch (string)
     CONST char *string;
{
  bfd_arch_info_type *arch = bfd_scan_arch(string);

  if (arch) {
    ldfile_output_architecture = arch->arch;
    ldfile_output_machine = arch->mach;
    ldfile_output_machine_name = arch->printable_name;
  }
  else {
    einfo("%P%F: cannot represent machine `%s'\n", string);
  }
}
