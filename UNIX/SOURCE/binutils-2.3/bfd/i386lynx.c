/* BFD back-end for i386 a.out binaries under Lynx.
   Copyright (C) 1990, 1991, 1992 Free Software Foundation, Inc.

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

#define BYTES_IN_WORD 4
#define ARCH 32
#define N_SHARED_LIB(x) 0

#define TEXT_START_ADDR 0
#define PAGE_SIZE 4096
#define SEGMENT_SIZE PAGE_SIZE
#define DEFAULT_ARCH bfd_arch_i386

#define MY(OP) CAT(i386lynx_aout_,OP)
#define TARGETNAME "a.out-i386-lynx"

#include "bfd.h"
#include "sysdep.h"
#include "libbfd.h"

#ifndef WRITE_HEADERS
#define WRITE_HEADERS(abfd, execp)					      \
      {									      \
	bfd_size_type text_size; /* dummy vars */			      \
	file_ptr text_end;						      \
	if (adata(abfd).magic == undecided_magic)			      \
	  NAME(aout,adjust_sizes_and_vmas) (abfd, &text_size, &text_end);     \
    									      \
	execp->a_syms = bfd_get_symcount (abfd) * EXTERNAL_NLIST_SIZE;	      \
	execp->a_entry = bfd_get_start_address (abfd);			      \
    									      \
	execp->a_trsize = ((obj_textsec (abfd)->reloc_count) *		      \
			   obj_reloc_entry_size (abfd));		      \
	execp->a_drsize = ((obj_datasec (abfd)->reloc_count) *		      \
			   obj_reloc_entry_size (abfd));		      \
	NAME(aout,swap_exec_header_out) (abfd, execp, &exec_bytes);	      \
									      \
	bfd_seek (abfd, (file_ptr) 0, SEEK_SET);			      \
	bfd_write ((PTR) &exec_bytes, 1, EXEC_BYTES_SIZE, abfd);	      \
	/* Now write out reloc info, followed by syms and strings */	      \
  									      \
	if (bfd_get_symcount (abfd) != 0) 				      \
	    {								      \
	      bfd_seek (abfd, (file_ptr)(N_SYMOFF(*execp)), SEEK_SET);	      \
									      \
	      NAME(aout,write_syms)(abfd);				      \
									      \
	      bfd_seek (abfd, (file_ptr)(N_TRELOFF(*execp)), SEEK_SET);	      \
									      \
	      if (!NAME(lynx,squirt_out_relocs) (abfd, obj_textsec (abfd))) return false; \
	      bfd_seek (abfd, (file_ptr)(N_DRELOFF(*execp)), SEEK_SET);	      \
									      \
	      if (!NAME(lynx,squirt_out_relocs)(abfd, obj_datasec (abfd))) return false; \
	    }								      \
      }									      
#endif

#include "libaout.h"
#include "aout/aout64.h"

#ifdef HOST_LYNX

char *lynx_core_file_failing_command();
int lynx_core_file_failing_signal();
boolean lynx_core_file_matches_executable_p();
bfd_target *lynx_core_file_p();

#define	MY_core_file_failing_command lynx_core_file_failing_command
#define	MY_core_file_failing_signal lynx_core_file_failing_signal
#define	MY_core_file_matches_executable_p lynx_core_file_matches_executable_p
#define	MY_core_file_p lynx_core_file_p

#endif /* HOST_LYNX */


#define KEEPIT flags

extern reloc_howto_type aout_32_ext_howto_table[];
extern reloc_howto_type aout_32_std_howto_table[];

/* Standard reloc stuff */
/* Output standard relocation information to a file in target byte order. */

void
DEFUN(NAME(lynx,swap_std_reloc_out),(abfd, g, natptr),
      bfd *abfd AND
      arelent *g AND
      struct reloc_std_external *natptr)
{
  int r_index;
  asymbol *sym = *(g->sym_ptr_ptr);
  int r_extern;
  unsigned int r_length;
  int r_pcrel;
  int r_baserel, r_jmptable, r_relative;
  unsigned int r_addend;
  asection *output_section = sym->section->output_section;

  PUT_WORD(abfd, g->address, natptr->r_address);

  r_length = g->howto->size ;	/* Size as a power of two */
  r_pcrel  = (int) g->howto->pc_relative; /* Relative to PC? */
  /* r_baserel, r_jmptable, r_relative???  FIXME-soon */
  r_baserel = 0;
  r_jmptable = 0;
  r_relative = 0;
    
  r_addend = g->addend + (*(g->sym_ptr_ptr))->section->output_section->vma;
    
  /* name was clobbered by aout_write_syms to be symbol index */

  /* If this relocation is relative to a symbol then set the 
     r_index to the symbols index, and the r_extern bit.

     Absolute symbols can come in in two ways, either as an offset
     from the abs section, or as a symbol which has an abs value.
     check for that here
     */
     

  if (bfd_is_com_section (output_section)
      || output_section == &bfd_abs_section
      || output_section == &bfd_und_section) 
    {
      if (bfd_abs_section.symbol == sym)
      {
	/* Whoops, looked like an abs symbol, but is really an offset
	   from the abs section */
	r_index = 0;
	r_extern = 0;
       }
      else 
      {
	/* Fill in symbol */
	r_extern = 1;
	r_index =  stoi((*(g->sym_ptr_ptr))->KEEPIT);
     
      }
    }
  else 
    {
      /* Just an ordinary section */
      r_extern = 0;
      r_index  = output_section->target_index;      
    }

  /* now the fun stuff */
  if (abfd->xvec->header_byteorder_big_p != false) {
      natptr->r_index[0] = r_index >> 16;
      natptr->r_index[1] = r_index >> 8;
      natptr->r_index[2] = r_index;
      natptr->r_type[0] =
       (r_extern?    RELOC_STD_BITS_EXTERN_BIG: 0)
	| (r_pcrel?     RELOC_STD_BITS_PCREL_BIG: 0)
	 | (r_baserel?   RELOC_STD_BITS_BASEREL_BIG: 0)
	  | (r_jmptable?  RELOC_STD_BITS_JMPTABLE_BIG: 0)
	   | (r_relative?  RELOC_STD_BITS_RELATIVE_BIG: 0)
	    | (r_length <<  RELOC_STD_BITS_LENGTH_SH_BIG);
    } else {
	natptr->r_index[2] = r_index >> 16;
	natptr->r_index[1] = r_index >> 8;
	natptr->r_index[0] = r_index;
	natptr->r_type[0] =
	 (r_extern?    RELOC_STD_BITS_EXTERN_LITTLE: 0)
	  | (r_pcrel?     RELOC_STD_BITS_PCREL_LITTLE: 0)
	   | (r_baserel?   RELOC_STD_BITS_BASEREL_LITTLE: 0)
	    | (r_jmptable?  RELOC_STD_BITS_JMPTABLE_LITTLE: 0)
	     | (r_relative?  RELOC_STD_BITS_RELATIVE_LITTLE: 0)
	      | (r_length <<  RELOC_STD_BITS_LENGTH_SH_LITTLE);
      }
}


/* Extended stuff */
/* Output extended relocation information to a file in target byte order. */

void
DEFUN(NAME(lynx,swap_ext_reloc_out),(abfd, g, natptr),
      bfd *abfd AND
      arelent *g AND
      register struct reloc_ext_external *natptr)
{
  int r_index;
  int r_extern;
  unsigned int r_type;
  unsigned int r_addend;
  asymbol *sym = *(g->sym_ptr_ptr);    
  asection *output_section = sym->section->output_section;
  
  PUT_WORD (abfd, g->address, natptr->r_address);
    
  r_type = (unsigned int) g->howto->type;
    
  r_addend = g->addend + (*(g->sym_ptr_ptr))->section->output_section->vma;


  /* If this relocation is relative to a symbol then set the 
     r_index to the symbols index, and the r_extern bit.

     Absolute symbols can come in in two ways, either as an offset
     from the abs section, or as a symbol which has an abs value.
     check for that here
     */
     
  if (bfd_is_com_section (output_section)
      || output_section == &bfd_abs_section
      || output_section == &bfd_und_section)
  {
    if (bfd_abs_section.symbol == sym)
    {
      /* Whoops, looked like an abs symbol, but is really an offset
	 from the abs section */
      r_index = 0;
      r_extern = 0;
     }
    else 
    {
      r_extern = 1;
      r_index =  stoi((*(g->sym_ptr_ptr))->KEEPIT);
    }
  }
  else 
  {
    /* Just an ordinary section */
    r_extern = 0;
    r_index  = output_section->target_index;      
  }
	 
	 
  /* now the fun stuff */
  if (abfd->xvec->header_byteorder_big_p != false) {
    natptr->r_index[0] = r_index >> 16;
    natptr->r_index[1] = r_index >> 8;
    natptr->r_index[2] = r_index;
    natptr->r_type[0] =
     (r_extern? RELOC_EXT_BITS_EXTERN_BIG: 0)
      | (r_type << RELOC_EXT_BITS_TYPE_SH_BIG);
  } else {
    natptr->r_index[2] = r_index >> 16;
    natptr->r_index[1] = r_index >> 8;
    natptr->r_index[0] = r_index;
    natptr->r_type[0] =
     (r_extern? RELOC_EXT_BITS_EXTERN_LITTLE: 0)
      | (r_type << RELOC_EXT_BITS_TYPE_SH_LITTLE);
  }

  PUT_WORD (abfd, r_addend, natptr->r_addend);
}

/* BFD deals internally with all things based from the section they're
   in. so, something in 10 bytes into a text section  with a base of
   50 would have a symbol (.text+10) and know .text vma was 50. 

   Aout keeps all it's symbols based from zero, so the symbol would
   contain 60. This macro subs the base of each section from the value
   to give the true offset from the section */


#define MOVE_ADDRESS(ad)       						\
  if (r_extern) {							\
   /* undefined symbol */						\
     cache_ptr->sym_ptr_ptr = symbols + r_index;			\
     cache_ptr->addend = ad;						\
     } else {								\
    /* defined, section relative. replace symbol with pointer to    	\
       symbol which points to section  */				\
    switch (r_index) {							\
    case N_TEXT:							\
    case N_TEXT | N_EXT:						\
      cache_ptr->sym_ptr_ptr  = obj_textsec(abfd)->symbol_ptr_ptr;	\
      cache_ptr->addend = ad  - su->textsec->vma;			\
      break;								\
    case N_DATA:							\
    case N_DATA | N_EXT:						\
      cache_ptr->sym_ptr_ptr  = obj_datasec(abfd)->symbol_ptr_ptr;	\
      cache_ptr->addend = ad - su->datasec->vma;			\
      break;								\
    case N_BSS:								\
    case N_BSS | N_EXT:							\
      cache_ptr->sym_ptr_ptr  = obj_bsssec(abfd)->symbol_ptr_ptr;	\
      cache_ptr->addend = ad - su->bsssec->vma;				\
      break;								\
    default:								\
    case N_ABS:								\
    case N_ABS | N_EXT:							\
     cache_ptr->sym_ptr_ptr = bfd_abs_section.symbol_ptr_ptr;	\
      cache_ptr->addend = ad;						\
      break;								\
    }									\
  }     								\

void
DEFUN(NAME(lynx,swap_ext_reloc_in), (abfd, bytes, cache_ptr, symbols),
      bfd *abfd AND
      struct reloc_ext_external *bytes AND
      arelent *cache_ptr AND
      asymbol **symbols)
{
  int r_index;
  int r_extern;
  unsigned int r_type;
  struct aoutdata *su = &(abfd->tdata.aout_data->a);

  cache_ptr->address = (GET_SWORD (abfd, bytes->r_address));

  r_index = (bytes->r_type[0] << 16)
    | (bytes->r_index[2] << 8)
      |  bytes->r_index[1];
  r_extern = (0 != (bytes->r_index[0] & RELOC_EXT_BITS_EXTERN_BIG));
  r_type   =       (bytes->r_index[0] & RELOC_EXT_BITS_TYPE_BIG)
      >> RELOC_EXT_BITS_TYPE_SH_BIG;

  cache_ptr->howto =  aout_32_ext_howto_table + r_type;
  MOVE_ADDRESS(GET_SWORD(abfd, bytes->r_addend));
}

void
DEFUN(NAME(lynx,swap_std_reloc_in), (abfd, bytes, cache_ptr, symbols),
  bfd *abfd AND
  struct reloc_std_external *bytes AND
  arelent *cache_ptr AND
  asymbol **symbols)
{
  int r_index;
  int r_extern;
  unsigned int r_length;
  int r_pcrel;
  int r_baserel, r_jmptable, r_relative;
  struct aoutdata  *su = &(abfd->tdata.aout_data->a);

  cache_ptr->address = bfd_h_get_32 (abfd, bytes->r_address);

  r_index = (bytes->r_type[0] << 16)
    | (bytes->r_index[2] << 8)
      |  bytes->r_index[1];
  r_extern  = (0 != (bytes->r_index[0] & RELOC_STD_BITS_EXTERN_BIG));
  r_pcrel   = (0 != (bytes->r_index[0] & RELOC_STD_BITS_PCREL_BIG));
  r_baserel = (0 != (bytes->r_index[0] & RELOC_STD_BITS_BASEREL_BIG));
  r_jmptable= (0 != (bytes->r_index[0] & RELOC_STD_BITS_JMPTABLE_BIG));
  r_relative= (0 != (bytes->r_index[0] & RELOC_STD_BITS_RELATIVE_BIG));
  r_length  =       (bytes->r_index[0] & RELOC_STD_BITS_LENGTH_BIG) 
    >> RELOC_STD_BITS_LENGTH_SH_BIG;

  cache_ptr->howto =  aout_32_std_howto_table + r_length + 4 * r_pcrel;
  /* FIXME-soon:  Roll baserel, jmptable, relative bits into howto setting */

  MOVE_ADDRESS(0);
}

/* Reloc hackery */

boolean
DEFUN(NAME(lynx,slurp_reloc_table),(abfd, asect, symbols),
      bfd *abfd AND
      sec_ptr asect AND
      asymbol **symbols)
{
  unsigned int count;
  bfd_size_type reloc_size;
  PTR relocs;
  arelent *reloc_cache;
  size_t each_size;

  if (asect->relocation) return true;

  if (asect->flags & SEC_CONSTRUCTOR) return true;

  if (asect == obj_datasec (abfd)) {
    reloc_size = exec_hdr(abfd)->a_drsize;
    goto doit;
  }

  if (asect == obj_textsec (abfd)) {
    reloc_size = exec_hdr(abfd)->a_trsize;
    goto doit;
  }

  bfd_error = invalid_operation;
  return false;

 doit:
  bfd_seek (abfd, asect->rel_filepos, SEEK_SET);
  each_size = obj_reloc_entry_size (abfd);

  count = reloc_size / each_size;


  reloc_cache = (arelent *) bfd_zalloc (abfd, (size_t)(count * sizeof
						       (arelent)));
  if (!reloc_cache) {
nomem:
    bfd_error = no_memory;
    return false;
  }

  relocs = (PTR) bfd_alloc (abfd, reloc_size);
  if (!relocs) {
    bfd_release (abfd, reloc_cache);
    goto nomem;
  }

  if (bfd_read (relocs, 1, reloc_size, abfd) != reloc_size) {
    bfd_release (abfd, relocs);
    bfd_release (abfd, reloc_cache);
    bfd_error = system_call_error;
    return false;
  }

  if (each_size == RELOC_EXT_SIZE) {
    register struct reloc_ext_external *rptr = (struct reloc_ext_external *) relocs;
    unsigned int counter = 0;
    arelent *cache_ptr = reloc_cache;

    for (; counter < count; counter++, rptr++, cache_ptr++) {
      NAME(lynx,swap_ext_reloc_in)(abfd, rptr, cache_ptr, symbols);
    }
  } else {
    register struct reloc_std_external *rptr = (struct reloc_std_external*) relocs;
    unsigned int counter = 0;
    arelent *cache_ptr = reloc_cache;

    for (; counter < count; counter++, rptr++, cache_ptr++) {
	NAME(lynx,swap_std_reloc_in)(abfd, rptr, cache_ptr, symbols);
    }

  }

  bfd_release (abfd,relocs);
  asect->relocation = reloc_cache;
  asect->reloc_count = count;
  return true;
}



/* Write out a relocation section into an object file.  */

boolean
DEFUN(NAME(lynx,squirt_out_relocs),(abfd, section),
      bfd *abfd AND
      asection *section)
{
  arelent **generic;
  unsigned char *native, *natptr;
  size_t each_size;

  unsigned int count = section->reloc_count;
  size_t natsize;

  if (count == 0) return true;

  each_size = obj_reloc_entry_size (abfd);
  natsize = each_size * count;
  native = (unsigned char *) bfd_zalloc (abfd, natsize);
  if (!native) {
    bfd_error = no_memory;
    return false;
  }

  generic = section->orelocation;

  if (each_size == RELOC_EXT_SIZE) 
    {
      for (natptr = native;
	   count != 0;
	   --count, natptr += each_size, ++generic)
	NAME(lynx,swap_ext_reloc_out) (abfd, *generic, (struct reloc_ext_external *)natptr);
    }
  else 
    {
      for (natptr = native;
	   count != 0;
	   --count, natptr += each_size, ++generic)
	NAME(lynx,swap_std_reloc_out)(abfd, *generic, (struct reloc_std_external *)natptr);
    }

  if ( bfd_write ((PTR) native, 1, natsize, abfd) != natsize) {
    bfd_release(abfd, native);
    return false;
  }
  bfd_release (abfd, native);

  return true;
}

/* This is stupid.  This function should be a boolean predicate */
unsigned int
DEFUN(NAME(lynx,canonicalize_reloc),(abfd, section, relptr, symbols),
      bfd *abfd AND
      sec_ptr section AND
      arelent **relptr AND
      asymbol **symbols)
{
  arelent *tblptr = section->relocation;
  unsigned int count;

  if (!(tblptr || NAME(lynx,slurp_reloc_table)(abfd, section, symbols)))
    return 0;

  if (section->flags & SEC_CONSTRUCTOR) {
    arelent_chain *chain = section->constructor_chain;
    for (count = 0; count < section->reloc_count; count ++) {
      *relptr ++ = &chain->relent;
      chain = chain->next;
    }
  }
  else {
    tblptr = section->relocation;
    if (!tblptr) return 0;

    for (count = 0; count++ < section->reloc_count;) 
      {
	*relptr++ = tblptr++;
      }
  }
  *relptr = 0;

  return section->reloc_count;
}

#define MY_canonicalize_reloc NAME(lynx,canonicalize_reloc)

#include "aout-target.h"
