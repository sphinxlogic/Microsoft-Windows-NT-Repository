/* bfd back-end for HP PA-RISC SOM objects.
   Copyright (C) 1990, 1991, 1992, 1993 Free Software Foundation, Inc.

   Contributed by the Center for Software Science at the
   University of Utah (pa-gdb-bugs@cs.utah.edu).

   This file is part of BFD, the Binary File Descriptor library.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include "bfd.h"
#include "sysdep.h"

#if defined (HOST_HPPAHPUX) || defined (HOST_HPPABSD)

#include "libbfd.h"
#include "som.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/dir.h>
#include <signal.h>
#include <machine/reg.h>
#include <sys/user.h>		/* After a.out.h  */
#include <sys/file.h>
#include <errno.h>

/* Magic not defined in standard HP-UX header files until 8.0 */

#ifndef CPU_PA_RISC1_0
#define CPU_PA_RISC1_0 0x20B
#endif /* CPU_PA_RISC1_0 */

#ifndef CPU_PA_RISC1_1
#define CPU_PA_RISC1_1 0x210
#endif /* CPU_PA_RISC1_1 */

#ifndef _PA_RISC1_0_ID
#define _PA_RISC1_0_ID CPU_PA_RISC1_0
#endif /* _PA_RISC1_0_ID */

#ifndef _PA_RISC1_1_ID
#define _PA_RISC1_1_ID CPU_PA_RISC1_1
#endif /* _PA_RISC1_1_ID */

#ifndef _PA_RISC_MAXID
#define _PA_RISC_MAXID	0x2FF
#endif /* _PA_RISC_MAXID */

#ifndef _PA_RISC_ID
#define _PA_RISC_ID(__m_num)		\
    (((__m_num) == _PA_RISC1_0_ID) ||	\
     ((__m_num) >= _PA_RISC1_1_ID && (__m_num) <= _PA_RISC_MAXID))
#endif /* _PA_RISC_ID */

/* Forward declarations */

static boolean som_mkobject PARAMS ((bfd *));
static bfd_target * som_object_setup PARAMS ((bfd *,
					      struct header *,
					      struct som_exec_auxhdr *));
static asection * make_unique_section PARAMS ((bfd *, CONST char *, int));
static boolean setup_sections PARAMS ((bfd *, struct header *));
static bfd_target * som_object_p PARAMS ((bfd *));
static boolean som_write_object_contents PARAMS ((bfd *));
static boolean som_slurp_string_table PARAMS ((bfd *));
static unsigned int som_slurp_symbol_table PARAMS ((bfd *));
static unsigned int som_get_symtab_upper_bound PARAMS ((bfd *));
static unsigned int som_canonicalize_reloc PARAMS ((bfd *, sec_ptr,
						    arelent **, asymbol **));
static unsigned int som_get_reloc_upper_bound PARAMS ((bfd *, sec_ptr));
static unsigned int som_get_symtab PARAMS ((bfd *, asymbol **));
static asymbol * som_make_empty_symbol PARAMS ((bfd *));
static void som_print_symbol PARAMS ((bfd *, PTR,
				      asymbol *, bfd_print_symbol_type));
static boolean som_new_section_hook PARAMS ((bfd *, asection *));
static boolean som_set_section_contents PARAMS ((bfd *, sec_ptr, PTR,
						 file_ptr, bfd_size_type));
static boolean som_set_arch_mach PARAMS ((bfd *, enum bfd_architecture,
					  unsigned long));
static boolean som_find_nearest_line PARAMS ((bfd *, asection *,
					      asymbol **, bfd_vma,
					      CONST char **,
					      CONST char **,
					      unsigned int *));
static void som_get_symbol_info PARAMS ((bfd *, asymbol *, symbol_info *));
static asection * som_section_from_subspace_index PARAMS ((bfd *, 
							   unsigned int));
static int log2 PARAMS ((unsigned int));


/* Return the logarithm of X, base 2, considering X unsigned. 
   Abort if X is not a power of two -- this should never happen (FIXME:
   It will happen on corrupt executables.  GDB should give an error, not
   a coredump, in that case).  */

static int
log2 (x)
     unsigned int x;
{
  int log = 0;

  /* Test for 0 or a power of 2.  */
  if (x == 0 || x != (x & -x))
    abort();

  while ((x >>= 1) != 0)
    log++;
  return log;
}

/* Perform some initialization for an object.  Save results of this
   initialization in the BFD.  */

static bfd_target *
som_object_setup (abfd, file_hdrp, aux_hdrp)
     bfd *abfd;
     struct header *file_hdrp;
     struct som_exec_auxhdr *aux_hdrp;
{
  asection *text, *data, *bss;

  /* som_mkobject will set bfd_error if som_mkobject fails.  */
  if (som_mkobject (abfd) != true)
    return 0;

  /* Make the standard .text, .data, and .bss sections so that tools
     which assume those names work (size for example).  They will have
     no contents, but the sizes and such will reflect those of the
     $CODE$, $DATA$, and $BSS$ subspaces respectively.

     FIXME:  Should check return status from bfd_make_section calls below.  */

  text = bfd_make_section (abfd, ".text");
  data = bfd_make_section (abfd, ".data");
  bss = bfd_make_section (abfd, ".bss");

  text->_raw_size = aux_hdrp->exec_tsize;
  data->_raw_size = aux_hdrp->exec_dsize;
  bss->_raw_size = aux_hdrp->exec_bsize;

  text->flags = (SEC_ALLOC | SEC_LOAD | SEC_HAS_CONTENTS | SEC_CODE);
  data->flags = (SEC_ALLOC | SEC_LOAD | SEC_HAS_CONTENTS);
  bss->flags = (SEC_ALLOC | SEC_IS_COMMON);

  /* The virtual memory addresses of the sections */
  text->vma = aux_hdrp->exec_tmem;
  data->vma = aux_hdrp->exec_dmem;
  bss->vma = aux_hdrp->exec_bfill;

  /* The file offsets of the sections */
  text->filepos = aux_hdrp->exec_tfile;
  data->filepos = aux_hdrp->exec_dfile;

  /* The file offsets of the relocation info */
  text->rel_filepos = 0;
  data->rel_filepos = 0;

  /* Set BFD flags based on what information is available in the SOM.  */
  abfd->flags = NO_FLAGS;
  if (! file_hdrp->entry_offset)
    abfd->flags |= HAS_RELOC;
  else
    abfd->flags |= EXEC_P;
  if (file_hdrp->symbol_total)
    abfd->flags |= HAS_LINENO | HAS_DEBUG | HAS_SYMS | HAS_LOCALS;

  bfd_get_start_address (abfd) = aux_hdrp->exec_entry;
  bfd_default_set_arch_mach (abfd, bfd_arch_hppa, 0);
  bfd_get_symcount (abfd) = file_hdrp->symbol_total;

  /* Initialize the saved symbol table and string table to NULL.  
     Save important offsets and sizes from the SOM header into
     the BFD.  */
  obj_som_stringtab (abfd) = (char  *) NULL;
  obj_som_symtab (abfd) = (som_symbol_type *) NULL;
  obj_som_stringtab_size (abfd) = file_hdrp->symbol_strings_size;
  obj_som_sym_filepos (abfd) = file_hdrp->symbol_location;
  obj_som_str_filepos (abfd) = file_hdrp->symbol_strings_location;
  obj_som_reloc_filepos (abfd) = file_hdrp->fixup_request_location;

  return abfd->xvec;
}

/* Create a new BFD section for NAME.  If NAME already exists, then create a
   new unique name, with NAME as the prefix.  This exists because SOM .o files
   may have more than one $CODE$ subspace.  */

static asection *
make_unique_section (abfd, name, num)
     bfd *abfd;
     CONST char *name;
     int num;
{
  asection *sect;
  char *newname;
  char altname[100];

  sect = bfd_make_section (abfd, name);
  while (!sect)
    {
      sprintf (altname, "%s-%d", name, num++);
      sect = bfd_make_section (abfd, altname);
    }

  newname = bfd_alloc (abfd, strlen (sect->name) + 1);
  strcpy (newname, sect->name);

  sect->name = newname;
  return sect;
}

/* Convert all of the space and subspace info into BFD sections.  Each space
   contains a number of subspaces, which in turn describe the mapping between
   regions of the exec file, and the address space that the program runs in.
   BFD sections which correspond to spaces will overlap the sections for the
   associated subspaces.  */

static boolean
setup_sections (abfd, file_hdr)
     bfd *abfd;
     struct header *file_hdr;
{
  char *space_strings;
  int space_index;
  unsigned int total_subspaces = 0;

  /* First, read in space names */

  space_strings = alloca (file_hdr->space_strings_size);
  if (!space_strings)
    return false;

  if (bfd_seek (abfd, file_hdr->space_strings_location, SEEK_SET) < 0)
    return false;
  if (bfd_read (space_strings, 1, file_hdr->space_strings_size, abfd)
      != file_hdr->space_strings_size)
    return false;

  /* Loop over all of the space dictionaries, building up sections */
  for (space_index = 0; space_index < file_hdr->space_total; space_index++)
    {
      struct space_dictionary_record space;
      struct subspace_dictionary_record subspace, save_subspace;
      int subspace_index;
      asection *space_asect;

      /* Read the space dictionary element */
      if (bfd_seek (abfd, file_hdr->space_location
		    + space_index * sizeof space, SEEK_SET) < 0)
	return false;
      if (bfd_read (&space, 1, sizeof space, abfd) != sizeof space)
	return false;

      /* Setup the space name string */
      space.name.n_name = space.name.n_strx + space_strings;

      /* Make a section out of it */
      space_asect = make_unique_section (abfd, space.name.n_name, space_index);
      if (!space_asect)
	return false;

      /* Now, read in the first subspace for this space */
      if (bfd_seek (abfd, file_hdr->subspace_location
		    + space.subspace_index * sizeof subspace,
		    SEEK_SET) < 0)
	return false;
      if (bfd_read (&subspace, 1, sizeof subspace, abfd) != sizeof subspace)
	return false;
      /* Seek back to the start of the subspaces for loop below */
      if (bfd_seek (abfd, file_hdr->subspace_location
		    + space.subspace_index * sizeof subspace,
		    SEEK_SET) < 0)
	return false;

      /* Setup the start address and file loc from the first subspace record */
      space_asect->vma = subspace.subspace_start;
      space_asect->filepos = subspace.file_loc_init_value;
      space_asect->alignment_power = log2 (subspace.alignment);

      /* Initialize save_subspace so we can reliably determine if this
	 loop placed any useful values into it.  */
      bzero (&save_subspace, sizeof (struct subspace_dictionary_record));

      /* Loop over the rest of the subspaces, building up more sections */
      for (subspace_index = 0; subspace_index < space.subspace_quantity;
	   subspace_index++)
	{
	  asection *subspace_asect;

	  /* Read in the next subspace */
	  if (bfd_read (&subspace, 1, sizeof subspace, abfd)
	      != sizeof subspace)
	    return false;

	  /* Setup the subspace name string */
	  subspace.name.n_name = subspace.name.n_strx + space_strings;

	  /* Make a section out of this subspace */
	  subspace_asect = make_unique_section (abfd, subspace.name.n_name,
				     space.subspace_index + subspace_index);

	  if (!subspace_asect)
	    return false;

	  /* Keep an easy mapping between subspaces and sections.  */
	  som_section_data (subspace_asect)->subspace_index 
	    = total_subspaces++;

	  /* Set SEC_READONLY and SEC_CODE/SEC_DATA as specified
	     by the access_control_bits in the subspace header.  */
	  switch (subspace.access_control_bits >> 4)
	    {
	    /* Readonly data.  */  
	    case 0x0:
	      subspace_asect->flags |= SEC_DATA | SEC_READONLY;
	      break;

	    /* Normal data.  */  
	    case 0x1:
	      subspace_asect->flags |= SEC_DATA;
	      break;

	    /* Readonly code and the gateways.
	       Gateways have other attributes which do not map
	       into anything BFD knows about.  */
	    case 0x2:
	    case 0x4:
	    case 0x5:
	    case 0x6:
	    case 0x7:
	      subspace_asect->flags |= SEC_CODE | SEC_READONLY;
	      break;

	    /* dynamic (writable) code.  */
	    case 0x3:
	      subspace_asect->flags |= SEC_CODE;
	      break;
	    }
	  
	  if (subspace.dup_common || subspace.is_common) 
	    subspace_asect->flags |= SEC_IS_COMMON;
	  else
	    subspace_asect->flags |= SEC_HAS_CONTENTS;
	  if (subspace.is_loadable)
	    subspace_asect->flags |= SEC_ALLOC | SEC_LOAD;
	  if (subspace.code_only)
	    subspace_asect->flags |= SEC_CODE;

	  /* This subspace has relocations.
	     The fixup_request_quantity is a byte count for the number of
	     entries in the relocation stream; it is not the actual number
	     of relocations in the subspace.  */
	  if (subspace.fixup_request_quantity != 0)
	    {
	      subspace_asect->flags |= SEC_RELOC;
	      subspace_asect->rel_filepos = subspace.fixup_request_index;
	      som_section_data (subspace_asect)->reloc_size
		= subspace.fixup_request_quantity;
	      /* We can not determine this yet.  When we read in the 
		 relocation table the correct value will be filled in.  */
	      subspace_asect->reloc_count = -1;
	    }

	  /* Update save_subspace if appropriate.  */
	  if (subspace.file_loc_init_value > save_subspace.file_loc_init_value)
	    save_subspace = subspace;

	  subspace_asect->vma = subspace.subspace_start;
	  subspace_asect->_cooked_size = subspace.subspace_length;
	  subspace_asect->_raw_size = subspace.initialization_length;
	  subspace_asect->alignment_power = log2 (subspace.alignment);
	  subspace_asect->filepos = subspace.file_loc_init_value;
	}

      /* Yow! there is no subspace within the space which actually 
         has initialized information in it; this should never happen
         as far as I know.  */
      if (!save_subspace.file_loc_init_value)
	abort ();

      /* Setup the sizes for the space section based upon the info in the
         last subspace of the space.  */
      space_asect->_cooked_size = save_subspace.subspace_start
	- space_asect->vma + save_subspace.subspace_length;
      space_asect->_raw_size = save_subspace.file_loc_init_value
	- space_asect->filepos + save_subspace.initialization_length;
    }
  return true;
}

/* Read in a SOM object and make it into a BFD.  */

static bfd_target *
som_object_p (abfd)
     bfd *abfd;
{
  struct header file_hdr;
  struct som_exec_auxhdr aux_hdr;

  if (bfd_read ((PTR) & file_hdr, 1, FILE_HDR_SIZE, abfd) != FILE_HDR_SIZE)
    {
      bfd_error = system_call_error;
      return 0;
    }

  if (!_PA_RISC_ID (file_hdr.system_id))
    {
      bfd_error = wrong_format;
      return 0;
    }

  switch (file_hdr.a_magic)
    {
    case RELOC_MAGIC:
    case EXEC_MAGIC:
    case SHARE_MAGIC:
    case DEMAND_MAGIC:
#ifdef DL_MAGIC
    case DL_MAGIC:
#endif
#ifdef SHL_MAGIC
    case SHL_MAGIC:
#endif
#ifdef EXECLIBMAGIC
    case EXECLIBMAGIC:
#endif
      break;
    default:
      bfd_error = wrong_format;
      return 0;
    }

  if (file_hdr.version_id != VERSION_ID
      && file_hdr.version_id != NEW_VERSION_ID)
    {
      bfd_error = wrong_format;
      return 0;
    }

  /* If the aux_header_size field in the file header is zero, then this
     object is an incomplete executable (a .o file).  Do not try to read
     a non-existant auxiliary header.  */
  bzero (&aux_hdr, sizeof (struct som_exec_auxhdr));
  if (file_hdr.aux_header_size != 0)
    {
      if (bfd_read ((PTR) & aux_hdr, 1, AUX_HDR_SIZE, abfd) != AUX_HDR_SIZE)
	{
	  bfd_error = wrong_format;
	  return 0;
	}
    }

  if (!setup_sections (abfd, &file_hdr))
    {
      /* setup_sections does not bubble up a bfd error code.  */
      bfd_error = bad_value;
      return 0;
    }

  /* This appears to be a valid SOM object.  Do some initialization.  */
  return som_object_setup (abfd, &file_hdr, &aux_hdr);
}

/* Create a SOM object.  */

static boolean
som_mkobject (abfd)
     bfd *abfd;
{
  /* Allocate memory to hold backend information.  */
  abfd->tdata.som_data = (struct som_data_struct *)
    bfd_zalloc (abfd, sizeof (struct som_data_struct));
  if (abfd->tdata.som_data == NULL)
    {
      bfd_error = no_memory;
      return false;
    }
  obj_som_file_hdr (abfd) = bfd_zalloc (abfd, sizeof (struct header));
  if (obj_som_file_hdr (abfd) == NULL)

    {
      bfd_error = no_memory;
      return false;
    }
  return true;
}

boolean
som_write_object_contents (abfd)
     bfd *abfd;
{
  fprintf (stderr, "som_write_object_contents unimplemented\n");
  fflush (stderr);
  abort ();
  return (false);
}
/* Read and save the string table associated with the given BFD.  */

static boolean
som_slurp_string_table (abfd)
     bfd *abfd;
{
  char *stringtab;

  /* Use the saved version if its available.  */
  if (obj_som_stringtab (abfd) != NULL)
    return true;

  /* Allocate and read in the string table.  */
  stringtab = bfd_zalloc (abfd, obj_som_stringtab_size (abfd));
  if (stringtab == NULL)
    {
      bfd_error = no_memory;
      return false;
    }

  if (bfd_seek (abfd, obj_som_str_filepos (abfd), SEEK_SET) < 0)
    {
      bfd_error = system_call_error;
      return false;
    }
  
  if (bfd_read (stringtab, obj_som_stringtab_size (abfd), 1, abfd)
      != obj_som_stringtab_size (abfd))
    {
      bfd_error = system_call_error;
      return false;
    }

  /* Save our results and return success. */
  obj_som_stringtab (abfd) = stringtab;
  return true;
}

/* Return the amount of data (in bytes) required to hold the symbol
   table for this object.  */

static unsigned int
som_get_symtab_upper_bound (abfd)
     bfd *abfd;
{
  if (!som_slurp_symbol_table (abfd))
    return 0;

  return (bfd_get_symcount (abfd) + 1) * (sizeof (som_symbol_type *));
}

/* Convert from a SOM subspace index to a BFD section.  */

static asection *
som_section_from_subspace_index (abfd, index)
     bfd *abfd;
     unsigned int index;
{
  asection *section;

  for (section = abfd->sections; section != NULL; section = section->next)
    if (som_section_data (section)->subspace_index == index)
      return section;

  /* Should never happen.  */
  abort();
}

/* Read and save the symbol table associated with the given BFD.  */

static unsigned int
som_slurp_symbol_table (abfd)
     bfd *abfd;
{
  int symbol_count = bfd_get_symcount (abfd);
  int symsize = sizeof (struct symbol_dictionary_record);
  char *stringtab;
  struct symbol_dictionary_record *buf, *bufp, *endbufp;
  som_symbol_type *sym, *symbase;

  /* Return saved value if it exists.  */
  if (obj_som_symtab (abfd) != NULL)
    return true;

  /* Sanity checking.  Make sure there are some symbols and that 
     we can read the string table too.  */
  if (symbol_count == 0)
    {
      bfd_error = no_symbols;
      return false;
    }

  if (!som_slurp_string_table (abfd))
    return false;

  stringtab = obj_som_stringtab (abfd);

  symbase = (som_symbol_type *)
    bfd_zalloc (abfd, symbol_count * sizeof (som_symbol_type));
  if (symbase == NULL)
    {
      bfd_error = no_memory;
      return false;
    }

  /* Read in the external SOM representation.  */
  buf = alloca (symbol_count * symsize);
  if (buf == NULL)
    {
      bfd_error = no_memory;
      return false;
    }
  if (bfd_seek (abfd, obj_som_sym_filepos (abfd), SEEK_SET) < 0)
    {
      bfd_error = system_call_error;
      return false;
    }
  if (bfd_read (buf, symbol_count * symsize, 1, abfd) 
      != symbol_count * symsize)
    {
      bfd_error = no_symbols;
      return (false);
    }

  /* Iterate over all the symbols and internalize them.  */
  endbufp = buf + symbol_count;
  for (bufp = buf, sym = symbase; bufp < endbufp; ++bufp)
    {

      /* I don't think we care about these.  */
      if (bufp->symbol_type == ST_SYM_EXT
	  || bufp->symbol_type == ST_ARG_EXT)
	continue;

      /* Some reasonable defaults.  */
      sym->symbol.the_bfd = abfd;
      sym->symbol.name = bufp->name.n_strx + stringtab;
      sym->symbol.value = bufp->symbol_value;
      sym->symbol.section = 0;
      sym->symbol.flags = 0;

      switch (bufp->symbol_type)
	{
	case ST_ENTRY:
	  sym->symbol.flags |= BSF_FUNCTION;
	  sym->symbol.value &= ~0x3;
	  break;

	case ST_PRI_PROG:
	case ST_SEC_PROG:
	case ST_STUB:
	case ST_MILLICODE:
	case ST_CODE:
	  sym->symbol.value &= ~0x3;

	default:
	  break;
	}

      /* Handle scoping and section information.  */
      switch (bufp->symbol_scope)
	{
	/* symbol_info field is undefined for SS_EXTERNAL and SS_UNSAT symbols,
	   so the section associated with this symbol can't be known.  */
	case SS_EXTERNAL:
	case SS_UNSAT:
	  sym->symbol.flags |= (BSF_EXPORT | BSF_GLOBAL);
	  break;

	case SS_UNIVERSAL:
	  sym->symbol.flags |= (BSF_EXPORT | BSF_GLOBAL);
	  sym->symbol.section
	    = som_section_from_subspace_index (abfd, bufp->symbol_info);
	  sym->symbol.value -= sym->symbol.section->vma;
	  break;

#if 0
	/* SS_GLOBAL and SS_LOCAL are two names for the same thing.
	   Sound dumb?  It is.  */
	case SS_GLOBAL:
#endif
	case SS_LOCAL:
	  sym->symbol.flags |= BSF_LOCAL;
	  sym->symbol.section
	    = som_section_from_subspace_index (abfd, bufp->symbol_info);
	  sym->symbol.value -= sym->symbol.section->vma;
	  break;
	}

      /* Mark symbols left around by the debugger.  */
      if (strlen (sym->symbol.name) >= 3
	  && sym->symbol.name[0] == 'L'
	  && (sym->symbol.name[2] == '$' || sym->symbol.name[3] == '$'))
	sym->symbol.flags |= BSF_DEBUGGING;

      /* Note increment at bottom of loop, since we skip some symbols
         we can not include it as part of the for statement.  */
      sym++;
    }

  /* Save our results and return success.  */
  obj_som_symtab (abfd) = symbase;
  return (true);
}

/* Canonicalize a SOM symbol table.  Return the number of entries
   in the symbol table.  */

static unsigned int
som_get_symtab (abfd, location)
     bfd *abfd;
     asymbol **location;
{
  int i;
  som_symbol_type *symbase;

  if (!som_slurp_symbol_table (abfd))
    return 0;

  i = bfd_get_symcount (abfd);
  symbase = obj_som_symtab (abfd);

  for (; i > 0; i--, location++, symbase++)
    *location = &symbase->symbol;

  /* Final null pointer.  */
  *location = 0;
  return (bfd_get_symcount (abfd));
}

/* Make a SOM symbol.  There is nothing special to do here.  */

static asymbol *
som_make_empty_symbol (abfd)
     bfd *abfd;
{
  som_symbol_type *new =
  (som_symbol_type *) bfd_zalloc (abfd, sizeof (som_symbol_type));
  if (new == NULL)
    {
      bfd_error = no_memory;
      return 0;
    }
  new->symbol.the_bfd = abfd;

  return &new->symbol;
}

/* Print symbol information.  */

static void
som_print_symbol (ignore_abfd, afile, symbol, how)
     bfd *ignore_abfd;
     PTR afile;
     asymbol *symbol;
     bfd_print_symbol_type how;
{
  FILE *file = (FILE *) afile;
  switch (how)
    {
    case bfd_print_symbol_name:
      fprintf (file, "%s", symbol->name);
      break;
    case bfd_print_symbol_more:
      fprintf (file, "som ");
      fprintf_vma (file, symbol->value);
      fprintf (file, " %lx", (long) symbol->flags);
      break;
    case bfd_print_symbol_all:
      {
	CONST char *section_name;
	section_name = symbol->section ? symbol->section->name : "(*none*)";
	bfd_print_symbol_vandf ((PTR) file, symbol);
	fprintf (file, " %s\t%s", section_name, symbol->name);
	break;
      }
    }
}

static unsigned int
som_get_reloc_upper_bound (abfd, asect)
     bfd *abfd;
     sec_ptr asect;
{
  fprintf (stderr, "som_get_reloc_upper_bound unimplemented\n");
  fflush (stderr);
  abort ();
  return (0);
}

static unsigned int
som_canonicalize_reloc (abfd, section, relptr, symbols)
     bfd *abfd;
     sec_ptr section;
     arelent **relptr;
     asymbol **symbols;
{
  fprintf (stderr, "som_canonicalize_reloc unimplemented\n");
  fflush (stderr);
  abort ();
}

extern bfd_target som_vec;

/* A hook to set up object file dependent section information.  */

static boolean
som_new_section_hook (abfd, newsect)
     bfd *abfd;
     asection *newsect;
{
  newsect->used_by_bfd = (struct som_section_data_struct *)
    bfd_zalloc (abfd, sizeof (struct som_section_data_struct));
  newsect->alignment_power = 3;

  /* Initialize the subspace_index field to -1 so that it does
     not match a subspace with an index of 0.  */
  som_section_data (newsect)->subspace_index = -1;

  /* We allow more than three sections internally */
  return true;
}

static boolean
som_set_section_contents (abfd, section, location, offset, count)
     bfd *abfd;
     sec_ptr section;
     PTR location;
     file_ptr offset;
     bfd_size_type count;
{
  fprintf (stderr, "som_set_section_contents unimplimented\n");
  fflush (stderr);
  abort ();
  return false;
}

static boolean
som_set_arch_mach (abfd, arch, machine)
     bfd *abfd;
     enum bfd_architecture arch;
     unsigned long machine;
{
  fprintf (stderr, "som_set_arch_mach unimplemented\n");
  fflush (stderr);
  /* Allow any architecture to be supported by the som backend */
  return bfd_default_set_arch_mach (abfd, arch, machine);
}

static boolean
som_find_nearest_line (abfd, section, symbols, offset, filename_ptr,
			functionname_ptr, line_ptr)
     bfd *abfd;
     asection *section;
     asymbol **symbols;
     bfd_vma offset;
     CONST char **filename_ptr;
     CONST char **functionname_ptr;
     unsigned int *line_ptr;
{
  fprintf (stderr, "som_find_nearest_line unimplemented\n");
  fflush (stderr);
  abort ();
  return (false);
}

static int
som_sizeof_headers (abfd, reloc)
     bfd *abfd;
     boolean reloc;
{
  fprintf (stderr, "som_sizeof_headers unimplemented\n");
  fflush (stderr);
  abort ();
  return (0);
}

/* Return information about SOM symbol SYMBOL in RET.  */

static void
som_get_symbol_info (ignore_abfd, symbol, ret)
     bfd *ignore_abfd;		/* Ignored.  */
     asymbol *symbol;
     symbol_info *ret;
{
  bfd_symbol_info (symbol, ret);
}

/* End of miscellaneous support functions. */

#define som_bfd_debug_info_start        bfd_void
#define som_bfd_debug_info_end          bfd_void
#define som_bfd_debug_info_accumulate   (PROTO(void,(*),(bfd*, struct sec *))) bfd_void

#define som_openr_next_archived_file    bfd_generic_openr_next_archived_file
#define som_generic_stat_arch_elt       bfd_generic_stat_arch_elt
#define som_slurp_armap                  bfd_false
#define som_slurp_extended_name_table    _bfd_slurp_extended_name_table
#define som_truncate_arname              (void (*)())bfd_nullvoidptr
#define som_write_armap                  0

#define som_get_lineno                   (struct lineno_cache_entry *(*)())bfd_nullvoidptr
#define	som_close_and_cleanup	           bfd_generic_close_and_cleanup
#define som_get_section_contents          bfd_generic_get_section_contents

#define som_bfd_get_relocated_section_contents \
 bfd_generic_get_relocated_section_contents
#define som_bfd_relax_section bfd_generic_relax_section
#define som_bfd_seclet_link bfd_generic_seclet_link
#define som_bfd_reloc_type_lookup \
  ((CONST struct reloc_howto_struct *(*) PARAMS ((bfd *, bfd_reloc_code_real_type))) bfd_nullvoidptr)
#define som_bfd_make_debug_symbol \
  ((asymbol *(*) PARAMS ((bfd *, void *, unsigned long))) bfd_nullvoidptr)

/* Core file support is in the hpux-core backend.  */
#define som_core_file_failing_command	_bfd_dummy_core_file_failing_command
#define som_core_file_failing_signal	_bfd_dummy_core_file_failing_signal
#define som_core_file_matches_executable_p	_bfd_dummy_core_file_matches_executable_p

bfd_target som_vec =
{
  "som",			/* name */
  bfd_target_som_flavour,
  true,				/* target byte order */
  true,				/* target headers byte order */
  (HAS_RELOC | EXEC_P |		/* object flags */
   HAS_LINENO | HAS_DEBUG |
   HAS_SYMS | HAS_LOCALS | WP_TEXT | D_PAGED),
  (SEC_CODE | SEC_DATA | SEC_ROM | SEC_HAS_CONTENTS
   | SEC_ALLOC | SEC_LOAD | SEC_RELOC),		/* section flags */

/* leading_symbol_char: is the first char of a user symbol
   predictable, and if so what is it */
  0,
  ' ',				/* ar_pad_char */
  16,				/* ar_max_namelen */
  3,				/* minimum alignment */
  bfd_getb64, bfd_getb_signed_64, bfd_putb64,
  bfd_getb32, bfd_getb_signed_32, bfd_putb32,
  bfd_getb16, bfd_getb_signed_16, bfd_putb16,	/* data */
  bfd_getb64, bfd_getb_signed_64, bfd_putb64,
  bfd_getb32, bfd_getb_signed_32, bfd_putb32,
  bfd_getb16, bfd_getb_signed_16, bfd_putb16,	/* hdrs */
  {_bfd_dummy_target,
   som_object_p,		/* bfd_check_format */
   bfd_generic_archive_p,
   _bfd_dummy_target
  },
  {
    bfd_false,
    som_mkobject,
    _bfd_generic_mkarchive,
    bfd_false
  },
  {
    bfd_false,
    som_write_object_contents,
    _bfd_write_archive_contents,
    bfd_false,
  },
#undef som
  JUMP_TABLE (som),
  (PTR) 0
};

#endif /* HOST_HPPAHPUX || HOST_HPPABSD */
