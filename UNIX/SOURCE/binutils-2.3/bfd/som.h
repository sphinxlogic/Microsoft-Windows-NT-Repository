/* HP PA-RISC SOM object file format:  definitions internal to BFD.
   Copyright (C) 1990-1991 Free Software Foundation, Inc.

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

#ifndef _SOM_H
#define _SOM_H

#include "../bfd/sysdep.h"

#include <a.out.h>


#ifdef HOST_HPPABSD
/* BSD uses a completely different scheme for object file identification.
   so for now, define _PA_RISC_ID to accept any random value for a model
   number.  */
#undef _PA_RISC_ID
#define _PA_RISC_ID(__m_num) 1
#endif /* HOST_HPPABSD */

#define FILE_HDR_SIZE sizeof(struct header)
#define AUX_HDR_SIZE sizeof(struct som_exec_auxhdr)

typedef struct som_symbol
  {
    asymbol symbol;
    short desc;
    char other;
    unsigned char type;
  }
som_symbol_type;

struct somdata
  {
    struct header *file_hdr;
    som_symbol_type *symtab;
    char *stringtab;

    /* We remember these offsets so that after check_file_format, we have
       no dependencies on the particular format of the exec_hdr.  */

    file_ptr sym_filepos;
    file_ptr str_filepos;
    file_ptr reloc_filepos;

    unsigned stringtab_size;
  };

struct som_data_struct
  {
    struct somdata a;
  };

/* Used to keep extra SOM specific information for a given section.

   reloc_size holds the size of the relocation stream, note this
   is very different from the number of relocations as SOM relocations
   are variable length. 

   reloc_stream is the actual stream of relocation entries.

   The BFD section index may not exactly match a SOM subspace index,
   for this reason we keep track of the original SOM subspace index
   when a subspace is turned into a BFD section.  */

struct som_section_data_struct
  {
    unsigned int reloc_size;
    char *reloc_stream;
    unsigned int subspace_index;
  };

#define somdata(bfd)			((bfd)->tdata.som_data->a)
#define obj_som_file_hdr(bfd)		(somdata(bfd).file_hdr)
#define obj_som_symtab(bfd)		(somdata(bfd).symtab)
#define obj_som_stringtab(bfd)		(somdata(bfd).stringtab)
#define obj_som_sym_filepos(bfd)	(somdata(bfd).sym_filepos)
#define obj_som_str_filepos(bfd)	(somdata(bfd).str_filepos)
#define obj_som_stringtab_size(bfd)	(somdata(bfd).stringtab_size)
#define obj_som_reloc_filepos(bfd)	(somdata(bfd).reloc_filepos)
#define som_section_data(sec) \
  ((struct som_section_data_struct *)sec->used_by_bfd)

#endif /* _SOM_H */
