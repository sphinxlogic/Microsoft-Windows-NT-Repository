/* BFD back-end for Hitachi H8/300 COFF binaries.
   Copyright 1990, 1991, 1992, 1993 Free Software Foundation, Inc.
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
#include "coff/h8300.h"
#include "coff/internal.h"
#include "seclet.h"
#include "libcoff.h"
extern bfd_error_vector_type bfd_error_vector;

static reloc_howto_type howto_table[] =
{
  HOWTO (R_RELBYTE, 0, 0, 8, false, 0, complain_overflow_bitfield, 0, "8", true, 0x000000ff, 0x000000ff, false),
  HOWTO (R_RELWORD, 0, 1, 16, false, 0, complain_overflow_bitfield, 0, "16", true, 0x0000ffff, 0x0000ffff, false),
  HOWTO (R_RELLONG, 0, 2, 32, false, 0, complain_overflow_bitfield, 0, "32", true, 0xffffffff, 0xffffffff, false),
  HOWTO (R_PCRBYTE, 0, 0, 8, true, 0, complain_overflow_signed, 0, "DISP8", true, 0x000000ff, 0x000000ff, true),
  HOWTO (R_PCRWORD, 0, 1, 16, true, 0, complain_overflow_signed, 0, "DISP16", true, 0x0000ffff, 0x0000ffff, true),
  HOWTO (R_PCRLONG, 0, 2, 32, true, 0, complain_overflow_signed, 0, "DISP32", true, 0xffffffff, 0xffffffff, true),
  HOWTO (R_MOVB1, 0, 1, 16, false, 0, complain_overflow_bitfield, 0, "16/8", true, 0x0000ffff, 0x0000ffff, false),
  HOWTO (R_MOVB2, 0, 1, 16, false, 0, complain_overflow_bitfield, 0, "8/16", true, 0x0000ffff, 0x0000ffff, false),
  HOWTO (R_JMP1, 0, 1, 16, false, 0, complain_overflow_bitfield, 0, "16/pcrel", true, 0x0000ffff, 0x0000ffff, false),
  HOWTO (R_JMP2, 0, 0, 8, false, 0, complain_overflow_bitfield, 0, "pcrecl/16", true, 0x000000ff, 0x000000ff, false),


  HOWTO (R_JMPL1, 0, 1, 32, false, 0, complain_overflow_bitfield, 0, "24/pcrell", true, 0x0000ffff, 0x0000ffff, false),
  HOWTO (R_JMPL_B8, 0, 0, 8, false, 0, complain_overflow_bitfield, 0, "pc8/24", true, 0x000000ff, 0x000000ff, false),

  HOWTO (R_MOVLB1, 0, 1, 16, false, 0, complain_overflow_bitfield, 0, "24/8", true, 0x0000ffff, 0x0000ffff, false),
  HOWTO (R_MOVLB2, 0, 1, 16, false, 0, complain_overflow_bitfield, 0, "8/24", true, 0x0000ffff, 0x0000ffff, false),

};


/* Turn a howto into a reloc number */

#define SELECT_RELOC(x,howto) \
  { x = select_reloc(howto); }

#define BADMAG(x) (H8300BADMAG(x)&& H8300HBADMAG(x))
#define H8300 1			/* Customize coffcode.h */
#define __A_MAGIC_SET__



/* Code to swap in the reloc */
#define SWAP_IN_RELOC_OFFSET   bfd_h_get_32
#define SWAP_OUT_RELOC_OFFSET bfd_h_put_32
#define SWAP_OUT_RELOC_EXTRA(abfd, src, dst) \
  dst->r_stuff[0] = 'S'; \
  dst->r_stuff[1] = 'C';


static int
select_reloc (howto)
     reloc_howto_type *howto;
{
  return howto->type;
}

/* Code to turn a r_type into a howto ptr, uses the above howto table
   */

static void
rtype2howto (internal, dst)
     arelent *internal;
     struct internal_reloc *dst;
{
  switch (dst->r_type)
    {
    case R_RELBYTE:
      internal->howto = howto_table + 0;
      break;
    case R_RELWORD:
      internal->howto = howto_table + 1;
      break;
    case R_RELLONG:
      internal->howto = howto_table + 2;
      break;
    case R_PCRBYTE:
      internal->howto = howto_table + 3;
      break;
    case R_PCRWORD:
      internal->howto = howto_table + 4;
      break;
    case R_PCRLONG:
      internal->howto = howto_table + 5;
      break;
    case R_MOVB1:
      internal->howto = howto_table + 6;
      break;
    case R_MOVB2:
      internal->howto = howto_table + 7;
      break;
    case R_JMP1:
      internal->howto = howto_table + 8;
      break;
    case R_JMP2:
      internal->howto = howto_table + 9;
      break;
    case R_JMPL1:
      internal->howto = howto_table + 10;
      break;
    case R_JMPL_B8:
      internal->howto = howto_table + 11;
      break;
    case R_MOVLB1:
      internal->howto = howto_table + 12;
      break;
    case R_MOVLB2:
      internal->howto = howto_table + 13;
      break;
    default:
      fprintf (stderr, "Bad reloc\n");
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

  if (((int) reloc->r_symndx) > 0)
    {
      relent->sym_ptr_ptr = symbols + obj_convert (abfd)[reloc->r_symndx];
    }
  else
    {
      relent->sym_ptr_ptr = &(bfd_abs_symbol);
    }



  relent->addend = reloc->r_offset;

  relent->address -= section->vma;
  /*  relent->section = 0;*/
}


static int
h8300_reloc16_estimate(input_section, symbols, reloc, shrink)
     asection *input_section;
     asymbol **symbols;
     arelent *reloc;
     unsigned int shrink;
{
  bfd_vma value;  
  bfd_vma dot;
  bfd_vma gap;

  /* The address of the thing to be relocated will have moved back by 
   the size of the shrink  - but we don't change reloc->address here,
   since we need it to know where the relocation lives in the source
   uncooked section */

  /*  reloc->address -= shrink;   conceptual */

  bfd_vma address = reloc->address - shrink;
  

  switch (reloc->howto->type)
    {     
    case R_MOVB2:
    case R_JMP2:
      shrink+=2;
      break;

      /* Thing is a move one byte */
    case R_MOVB1:
      value = bfd_coff_reloc16_get_value(reloc,0);
	
      if (value >= 0xff00)
	{ 

	  /* Change the reloc type from 16bit, possible 8 to 8bit
	     possible 16 */
	  reloc->howto = reloc->howto + 1;	  
	  /* The place to relc moves back by one */
	  /* This will be two bytes smaller in the long run */
	  shrink +=2 ;
	  bfd_perform_slip(symbols, 2, input_section, address);
	}      

      break;
      /* This is the 24 bit branch which could become an 8 bitter, 
       the relocation points to the first byte of the insn, not the
       actual data */

    case R_JMPL1:
      value = bfd_coff_reloc16_get_value(reloc, 0);
	
      dot = input_section->output_section->vma +
	input_section->output_offset + address;
  
      /* See if the address we're looking at within 127 bytes of where
	 we are, if so then we can use a small branch rather than the
	 jump we were going to */

      gap = value - dot ;
  
      if (-120 < (long)gap && (long)gap < 120 )
	{ 

	  /* Change the reloc type from 24bit, possible 8 to 8bit
	     possible 32 */
	  reloc->howto = reloc->howto + 1;	  
	  /* This will be two bytes smaller in the long run */
	  shrink +=2 ;
	  bfd_perform_slip(symbols, 2, input_section, address);
	}
      break;

    case R_JMP1:

      value = bfd_coff_reloc16_get_value(reloc, 0);
	
      dot = input_section->output_section->vma +
	input_section->output_offset + address;
  
      /* See if the address we're looking at within 127 bytes of where
	 we are, if so then we can use a small branch rather than the
	 jump we were going to */

      gap = value - (dot - shrink);
  

      if (-120 < (long)gap && (long)gap < 120 )
	{ 

	  /* Change the reloc type from 16bit, possible 8 to 8bit
	     possible 16 */
	  reloc->howto = reloc->howto + 1;	  
	  /* The place to relc moves back by one */

	  /* This will be two bytes smaller in the long run */
	  shrink +=2 ;
	  bfd_perform_slip(symbols, 2, input_section, address);
	}
      break;
    }

  
  return shrink;
}


/* First phase of a relaxing link */

/* Reloc types
   large		small
   R_MOVB1		R_MOVB2		mov.b with 16bit or 8 bit address
   R_JMP1		R_JMP2		jmp or pcrel branch
   R_JMPL1		R_JMPL_B8	24jmp or pcrel branch
   R_MOVLB1		R_MOVLB2	24 or 8 bit reloc for mov.b

*/

static void
h8300_reloc16_extra_cases (abfd, seclet, reloc, data, src_ptr, dst_ptr)
     bfd *abfd;
     struct bfd_seclet *seclet;
     arelent *reloc;
     bfd_byte *data;
     unsigned int *src_ptr;
     unsigned int *dst_ptr;
{
  unsigned int src_address = *src_ptr;
  unsigned int dst_address = *dst_ptr;

  switch (reloc->howto->type)
    {
      /* A 24 bit branch which could be a 8 bit pcrel, really pointing to
	 the byte before the 24bit hole, so we can treat it as a 32bit pointer */
    case R_PCRBYTE:
      {
	bfd_vma dot = seclet->offset 
	  + dst_address 
	    + seclet->u.indirect.section->output_section->vma;
	int gap = bfd_coff_reloc16_get_value (reloc, seclet) - dot;
	if (gap > 127 || gap < -128)
	  {
	    bfd_error_vector.reloc_value_truncated (reloc, seclet);
	  }

	bfd_put_8 (abfd, gap, data + dst_address);
	dst_address++;
	src_address++;

	break;
      }

    case R_RELBYTE:
      {
	unsigned int gap = bfd_coff_reloc16_get_value (reloc, seclet);
	if (gap > 0xff && gap < ~0xff)
	  {
	    bfd_error_vector.reloc_value_truncated (reloc, seclet);
	  }

	bfd_put_8 (abfd, gap, data + dst_address);
	dst_address += 1;
	src_address += 1;


      }
      break;
    case R_JMP1:
      /* A relword which would have like to have been a pcrel */
    case R_MOVB1:
      /* A relword which would like to have been modified but
	     didn't make it */
    case R_RELWORD:
      bfd_put_16 (abfd, bfd_coff_reloc16_get_value (reloc, seclet),
		  data + dst_address);
      dst_address += 2;
      src_address += 2;
      break;
    case R_RELLONG:
      bfd_put_32 (abfd, bfd_coff_reloc16_get_value (reloc, seclet),
		  data + dst_address);
      dst_address += 4;
      src_address += 4;
      break;

    case R_MOVB2:
      /* Special relaxed type, there will be a gap between where we
	     get stuff from and where we put stuff to now
	
	     for a mov.b @aa:16 -> mov.b @aa:8
	     opcode 0x6a 0x0y offset
	     ->     0x2y off
	     */
      if (data[dst_address - 1] != 0x6a)
	abort ();
      switch (data[src_address] & 0xf0)
	{
	case 0x00:
	  /* Src is memory */
	  data[dst_address - 1] = (data[src_address] & 0xf) | 0x20;
	  break;
	case 0x80:
	  /* Src is reg */
	  data[dst_address - 1] = (data[src_address] & 0xf) | 0x30;
	  break;
	default:
	  abort ();
	}

      /* the offset must fit ! after all, what was all the relaxing
	     about ? */

      bfd_put_8 (abfd, bfd_coff_reloc16_get_value (reloc, seclet),
		 data + dst_address);

      /* Note the magic - src goes up by two bytes, but dst by only
	     one */
      dst_address += 1;
      src_address += 3;

      break;

    case R_JMP2:
      
      /* Speciial relaxed type */
      {
	bfd_vma dot = seclet->offset
	+ dst_address
	+ seclet->u.indirect.section->output_section->vma;

	int gap = bfd_coff_reloc16_get_value (reloc, seclet) - dot - 1;

	if ((gap & ~0xff) != 0 && ((gap & 0xff00) != 0xff00))
	  abort ();

	bfd_put_8 (abfd, gap, data + dst_address);

	switch (data[dst_address - 1])
	  {
	  case 0x5e:
	    /* jsr -> bsr */
	    bfd_put_8 (abfd, 0x55, data + dst_address - 1);
	    break;
	  case 0x5a:
	    /* jmp ->bra */
	    bfd_put_8 (abfd, 0x40, data + dst_address - 1);
	    break;

	  default:
	    abort ();
	  }
	dst_address++;
	src_address += 3;

	break;
      }
      break;
      
    case R_JMPL_B8: /* 24 bit branch which is now 8 bits */
      
      /* Speciial relaxed type */
      {
	bfd_vma dot = seclet->offset
	+ dst_address
	+ seclet->u.indirect.section->output_section->vma;

	int gap = bfd_coff_reloc16_get_value (reloc, seclet) - dot - 2;

	if ((gap & ~0xff) != 0 && ((gap & 0xff00) != 0xff00))
	  abort ();

	switch (data[src_address])
	  {
	  case 0x5e:
	    /* jsr -> bsr */
	    bfd_put_8 (abfd, 0x55, data + dst_address);
	    break;
	  case 0x5a:
	    /* jmp ->bra */
	    bfd_put_8 (abfd, 0x40, data + dst_address);
	    break;

	  default:
	    bfd_put_8 (abfd, 0xde, data + dst_address);
	    break;
	  }

	bfd_put_8 (abfd, gap, data + dst_address + 1);
	dst_address += 2;
	src_address += 4;

	break;
      }

    case R_JMPL1:
      {
	int v = bfd_coff_reloc16_get_value (reloc, seclet);
	int o = bfd_get_32 (abfd, data + src_address);
	v = (v & 0x00ffffff) | (o & 0xff000000);
	bfd_put_32 (abfd, v, data + dst_address);
	dst_address += 4;
	src_address += 4;
      }

      break;


      /* A 24 bit mov  which could be an 8 bit move, really pointing to
	 the byte before the 24bit hole, so we can treat it as a 32bit pointer */
    case R_MOVLB1:
      {
	int v = bfd_coff_reloc16_get_value (reloc, seclet);
	int o = bfd_get_32 (abfd, data + dst_address);
	v = (v & 0x00ffffff) | (o & 0xff000000);
	bfd_put_32 (abfd, v, data + dst_address);
	dst_address += 4;
	src_address += 4;
      }

      break;
    default:

      abort ();
      break;

    }
  *src_ptr = src_address;
  *dst_ptr = dst_address;

}

#define coff_reloc16_extra_cases h8300_reloc16_extra_cases
#define coff_reloc16_estimate h8300_reloc16_estimate

#include "coffcode.h"


#undef coff_bfd_get_relocated_section_contents
#undef coff_bfd_relax_section
#define  coff_bfd_get_relocated_section_contents bfd_coff_reloc16_get_relocated_section_contents
#define coff_bfd_relax_section bfd_coff_reloc16_relax_section



bfd_target h8300coff_vec =
{
  "coff-h8300",			/* name */
  bfd_target_coff_flavour,
  true,				/* data byte order is big */
  true,				/* header byte order is big */

  (HAS_RELOC | EXEC_P |		/* object flags */
   HAS_LINENO | HAS_DEBUG |
   HAS_SYMS | HAS_LOCALS | WP_TEXT | BFD_IS_RELAXABLE ),
  (SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD | SEC_RELOC),	/* section flags */
  '_',				/* leading char */
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
