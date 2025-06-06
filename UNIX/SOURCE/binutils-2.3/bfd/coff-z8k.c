/* BFD back-end for Zilog Z800n COFF binaries.
   Copyright 1992, 1993 Free Software Foundation, Inc.
   Contributed by Cygnus Support.
   Written by Steve Chamberlain, <sac@cygnus.com>.

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
#include "libbfd.h"
#include "obstack.h"
#include "coff/z8k.h"
#include "coff/internal.h"
#include "libcoff.h"
#include "seclet.h"

extern bfd_error_vector_type bfd_error_vector;



static reloc_howto_type r_imm32 =
HOWTO (R_IMM32, 0, 1, 32, false, 0,
       complain_overflow_bitfield, 0, "r_imm32", true, 0xffffffff,
       0xffffffff, false);

static reloc_howto_type r_imm4l =
HOWTO (R_IMM4L, 0, 1, 4, false, 0,
       complain_overflow_bitfield, 0, "r_imm4l", true, 0xf, 0xf, false);

static reloc_howto_type r_da =
HOWTO (R_IMM16, 0, 1, 16, false, 0,
       complain_overflow_bitfield, 0, "r_da", true, 0x0000ffff, 0x0000ffff,
       false);

static reloc_howto_type r_imm8 =
HOWTO (R_IMM8, 0, 1, 8, false, 0,
       complain_overflow_bitfield, 0, "r_imm8", true, 0x000000ff, 0x000000ff,
       false);

static reloc_howto_type r_jr =
HOWTO (R_JR, 0, 1, 8, true, 0, complain_overflow_signed, 0,
       "r_jr", true, 0, 0, true);

/* Turn a howto into a reloc number */

static int 
coff_z8k_select_reloc (howto)
     reloc_howto_type *howto;
{
  return howto->type;
}

#define SELECT_RELOC(x,howto) x= coff_z8k_select_reloc(howto)


#define BADMAG(x) Z8KBADMAG(x)
#define Z8K 1			/* Customize coffcode.h */
#define __A_MAGIC_SET__



/* Code to swap in the reloc */
#define SWAP_IN_RELOC_OFFSET   bfd_h_get_32
#define SWAP_OUT_RELOC_OFFSET bfd_h_put_32
#define SWAP_OUT_RELOC_EXTRA(abfd, src, dst) \
  dst->r_stuff[0] = 'S'; \
  dst->r_stuff[1] = 'C';

/* Code to turn a r_type into a howto ptr, uses the above howto table
   */

static void
DEFUN (rtype2howto, (internal, dst),
       arelent * internal AND
       struct internal_reloc *dst)
{
  switch (dst->r_type)
    {
      default:
      fprintf (stderr, "BAD 0x%x\n", dst->r_type);
    case R_IMM8:
      internal->howto = &r_imm8;
      break;
     case R_IMM16:
      internal->howto = &r_da;
      break;
    case R_JR:
      internal->howto = &r_jr;
      break;
    case R_IMM32:
      internal->howto = &r_imm32;
      break;
    case R_IMM4L:
      internal->howto = &r_imm4l;
      break;
    }
}

#define RTYPE2HOWTO(internal, relocentry) rtype2howto(internal,relocentry)


/* Perform any necessaru magic to the addend in a reloc entry */


#define CALC_ADDEND(abfd, symbol, ext_reloc, cache_ptr) \
 cache_ptr->addend =  ext_reloc.r_offset;


#define RELOC_PROCESSING(relent,reloc,symbols,abfd,section) \
 reloc_processing(relent, reloc, symbols, abfd, section)

static void 
DEFUN (reloc_processing, (relent, reloc, symbols, abfd, section),
       arelent * relent AND
       struct internal_reloc *reloc AND
       asymbol ** symbols AND
       bfd * abfd AND
       asection * section)
{
  relent->address = reloc->r_vaddr;
  rtype2howto (relent, reloc);

  if (reloc->r_symndx > 0)
    {
      relent->sym_ptr_ptr = symbols + obj_convert (abfd)[reloc->r_symndx];
    }
  else
    {
      relent->sym_ptr_ptr = &(bfd_abs_symbol);
    }


  relent->addend = reloc->r_offset;
  relent->address -= section->vma;
}

static void
extra_case (in_abfd, seclet, reloc, data, src_ptr, dst_ptr)
     bfd *in_abfd;
     bfd_seclet_type *seclet;
     arelent *reloc;
     bfd_byte *data;
     unsigned int *src_ptr;
     unsigned int *dst_ptr;
{
  switch (reloc->howto->type)
    {
    case R_IMM8:
      bfd_put_8 (in_abfd, bfd_coff_reloc16_get_value (reloc, seclet),
		 data + *dst_ptr);
      (*dst_ptr) += 1;
      (*src_ptr) += 1;
      break;

    case R_IMM32:
      bfd_put_32 (in_abfd, bfd_coff_reloc16_get_value (reloc, seclet),
		  data + *dst_ptr);
      (*dst_ptr) += 4;
      (*src_ptr) += 4;
      break;

    case R_IMM4L:
      bfd_put_8 (in_abfd,
		 (bfd_get_8 (in_abfd, data + *dst_ptr) & 0xf0) 
		 | (0x0f & bfd_coff_reloc16_get_value (reloc, seclet)),
		 data + *dst_ptr);
      (*dst_ptr) += 1;
      (*src_ptr) += 1;
      break;

    case R_IMM16:
      bfd_put_16 (in_abfd, bfd_coff_reloc16_get_value (reloc, seclet),
		  data + *dst_ptr);
      (*dst_ptr) += 2;
      (*src_ptr) += 2;
      break;

    case R_JR:
      {
	bfd_vma dst = bfd_coff_reloc16_get_value (reloc, seclet);
	bfd_vma dot = seclet->offset
	+ *dst_ptr
	+ seclet->u.indirect.section->output_section->vma;
	int gap = dst - dot - 1;/* -1 since were in the odd byte of the
				    word and the pc's been incremented */

	if (gap & 1)
	  abort ();
	gap /= 2;
	if (gap > 128 || gap < -128)
	  {
	    bfd_error_vector.reloc_value_truncated (reloc, seclet);
	  }
	bfd_put_8 (in_abfd, gap, data + *dst_ptr);
	(*dst_ptr)++;
	(*src_ptr)++;
	break;
      }
    default:
      abort ();
    }
}

#define coff_reloc16_extra_cases extra_case

#include "coffcode.h"


#undef  coff_bfd_get_relocated_section_contents
#undef coff_bfd_relax_section
#define  coff_bfd_get_relocated_section_contents bfd_coff_reloc16_get_relocated_section_contents
#define coff_bfd_relax_section bfd_coff_reloc16_relax_section

bfd_target z8kcoff_vec =
{
  "coff-z8k",			/* name */
  bfd_target_coff_flavour,
  true,				/* data byte order is big */
  true,				/* header byte order is big */

  (HAS_RELOC | EXEC_P |		/* object flags */
   HAS_LINENO | HAS_DEBUG |
   HAS_SYMS | HAS_LOCALS | WP_TEXT),

  (SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD | SEC_RELOC),	/* section flags */
  '_',				/* leading symbol underscore */
  '/',				/* ar_pad_char */
  15,				/* ar_max_namelen */
  1,				/* minimum section alignment */
  bfd_getb64, bfd_getb_signed_64, bfd_putb64,
     bfd_getb32, bfd_getb_signed_32, bfd_putb32,
     bfd_getb16, bfd_getb_signed_16, bfd_putb16,	/* data */
  bfd_getb64, bfd_getb_signed_64, bfd_putb64,
     bfd_getb32, bfd_getb_signed_32, bfd_putb32,
     bfd_getb16, bfd_getb_signed_16, bfd_putb16,	/* hdrs */

  {_bfd_dummy_target, coff_object_p,	/* bfd_check_format */
   bfd_generic_archive_p, _bfd_dummy_target},
  {bfd_false, coff_mkobject, _bfd_generic_mkarchive,	/* bfd_set_format */
   bfd_false},
  {bfd_false, coff_write_object_contents,	/* bfd_write_contents */
   _bfd_write_archive_contents, bfd_false},

  JUMP_TABLE (coff),
  COFF_SWAP_TABLE,
};
