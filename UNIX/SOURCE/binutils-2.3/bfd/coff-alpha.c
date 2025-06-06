/* BFD back-end for ALPHA Extended-Coff files.
   Copyright 1993 Free Software Foundation, Inc.
   Modified from coff-mips.c by Steve Chamberlain <sac@cygnus.com> and
   Ian Lance Taylor <ian@cygnus.com>.

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
#include "seclet.h"
#include "coff/internal.h"
#include "coff/sym.h"
#include "coff/symconst.h"
#include "coff/ecoff.h"
#include "coff/alpha.h"
#include "libcoff.h"
#include "libecoff.h"

/* Prototypes for static functions.  */

static boolean alpha_ecoff_bad_format_hook PARAMS ((bfd *abfd, PTR filehdr));
static void alpha_ecoff_swap_reloc_in PARAMS ((bfd *, PTR,
					      struct internal_reloc *));
static void alpha_ecoff_swap_reloc_out PARAMS ((bfd *,
					       const struct internal_reloc *,
					       PTR));
static void alpha_adjust_reloc_in PARAMS ((bfd *,
					   const struct internal_reloc *,
					   arelent *));
static void alpha_adjust_reloc_out PARAMS ((bfd *, const arelent *,
					    struct internal_reloc *));
static bfd_byte *alpha_ecoff_get_relocated_section_contents
  PARAMS ((bfd *abfd, struct bfd_seclet *seclet, bfd_byte *data,
	   boolean relocateable));

/* ECOFF has COFF sections, but the debugging information is stored in
   a completely different format.  ECOFF targets use some of the
   swapping routines from coffswap.h, and some of the generic COFF
   routines in coffgen.c, but, unlike the real COFF targets, do not
   use coffcode.h itself.

   Get the generic COFF swapping routines, except for the reloc,
   symbol, and lineno ones.  Give them ecoff names.  Define some
   accessor macros for the large sizes used for Alpha ECOFF.  */

#define GET_FILEHDR_SYMPTR bfd_h_get_64
#define PUT_FILEHDR_SYMPTR bfd_h_put_64
#define GET_AOUTHDR_TSIZE bfd_h_get_64
#define PUT_AOUTHDR_TSIZE bfd_h_put_64
#define GET_AOUTHDR_DSIZE bfd_h_get_64
#define PUT_AOUTHDR_DSIZE bfd_h_put_64
#define GET_AOUTHDR_BSIZE bfd_h_get_64
#define PUT_AOUTHDR_BSIZE bfd_h_put_64
#define GET_AOUTHDR_ENTRY bfd_h_get_64
#define PUT_AOUTHDR_ENTRY bfd_h_put_64
#define GET_AOUTHDR_TEXT_START bfd_h_get_64
#define PUT_AOUTHDR_TEXT_START bfd_h_put_64
#define GET_AOUTHDR_DATA_START bfd_h_get_64
#define PUT_AOUTHDR_DATA_START bfd_h_put_64
#define GET_SCNHDR_PADDR bfd_h_get_64
#define PUT_SCNHDR_PADDR bfd_h_put_64
#define GET_SCNHDR_VADDR bfd_h_get_64
#define PUT_SCNHDR_VADDR bfd_h_put_64
#define GET_SCNHDR_SIZE bfd_h_get_64
#define PUT_SCNHDR_SIZE bfd_h_put_64
#define GET_SCNHDR_SCNPTR bfd_h_get_64
#define PUT_SCNHDR_SCNPTR bfd_h_put_64
#define GET_SCNHDR_RELPTR bfd_h_get_64
#define PUT_SCNHDR_RELPTR bfd_h_put_64
#define GET_SCNHDR_LNNOPTR bfd_h_get_64
#define PUT_SCNHDR_LNNOPTR bfd_h_put_64

#define ALPHAECOFF

#define NO_COFF_RELOCS
#define NO_COFF_SYMBOLS
#define NO_COFF_LINENOS
#define coff_swap_filehdr_in alpha_ecoff_swap_filehdr_in
#define coff_swap_filehdr_out alpha_ecoff_swap_filehdr_out
#define coff_swap_aouthdr_in alpha_ecoff_swap_aouthdr_in
#define coff_swap_aouthdr_out alpha_ecoff_swap_aouthdr_out
#define coff_swap_scnhdr_in alpha_ecoff_swap_scnhdr_in
#define coff_swap_scnhdr_out alpha_ecoff_swap_scnhdr_out
#include "coffswap.h"

/* Get the ECOFF swapping routines.  */
#define ECOFF_64
#include "ecoffswap.h"

/* How to process the various reloc types.  */

static reloc_howto_type alpha_howto_table[] =
{
  /* Reloc type 0 is ignored by itself.  However, it appears after a
     GPDISP reloc to identify the location where the low order 16 bits
     of the gp register are loaded.  */
  HOWTO (ALPHA_R_IGNORE,	/* type */
	 0,			/* rightshift */
	 0,			/* size (0 = byte, 1 = short, 2 = long) */
	 8,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 0,			/* special_function */
	 "IGNORE",		/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 false),		/* pcrel_offset */

  /* A 32 bit reference to a symbol.  */
  HOWTO (ALPHA_R_REFLONG,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 0,			/* special_function */
	 "REFLONG",		/* name */
	 true,			/* partial_inplace */
	 0xffffffff,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* A 64 bit reference to a symbol.  */
  HOWTO (ALPHA_R_REFQUAD,	/* type */
	 0,			/* rightshift */
	 4,			/* size (0 = byte, 1 = short, 2 = long) */
	 64,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 0,			/* special_function */
	 "REFQUAD",		/* name */
	 true,			/* partial_inplace */
	 0xffffffffffffffff,	/* src_mask */
	 0xffffffffffffffff,	/* dst_mask */
	 false),		/* pcrel_offset */

  /* A 32 bit GP relative offset.  This is just like REFLONG except
     that when the value is used the value of the gp register will be
     added in.  */
  HOWTO (ALPHA_R_GPREL32,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 0,			/* special_function */
	 "GPREL32",		/* name */
	 true,			/* partial_inplace */
	 0xffffffff,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* Used for an instruction that refers to memory off the GP
     register.  The offset is 16 bits of the 32 bit instruction.  This
     reloc always seems to be against the .lita section.  */
  HOWTO (ALPHA_R_LITERAL,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 0,			/* special_function */
	 "LITERAL",		/* name */
	 true,			/* partial_inplace */
	 0xffff,		/* src_mask */
	 0xffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* This reloc only appears immediately following a LITERAL reloc.
     It identifies a use of the literal.  It seems that the linker can
     use this to eliminate a portion of the .lita section.  The symbol
     index is special: 1 means the literal address is in the base
     register of a memory format instruction; 2 means the literal
     address is in the byte offset register of a byte-manipulation
     instruction; 3 means the literal address is in the target
     register of a jsr instruction.  This does not actually do any
     relocation.  */
  HOWTO (ALPHA_R_LITUSE,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 0,			/* special_function */
	 "LITUSE",		/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 false),		/* pcrel_offset */

  /* Load the gp register.  This is always used for a ldah instruction
     which loads the upper 16 bits of the gp register.  The next reloc
     will be an IGNORE reloc which identifies the location of the lda
     instruction which loads the lower 16 bits.  The symbol index of
     the GPDISP instruction appears to actually be the number of bytes
     between the ldah and lda instructions.  This gives two different
     ways to determine where the lda instruction is; I don't know why
     both are used.  The value to use for the relocation is the
     difference between the GP value and the current location; the
     load will always be done against a register holding the current
     address.  */
  HOWTO (ALPHA_R_GPDISP,	/* type */
	 16,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 true,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 0,			/* special_function */
	 "GPDISP",		/* name */
	 true,			/* partial_inplace */
	 0xffff,		/* src_mask */
	 0xffff,		/* dst_mask */
	 true),			/* pcrel_offset */

  /* A 21 bit branch.  The native assembler generates these for
     branches within the text segment, and also fills in the PC
     relative offset in the instruction.  It seems to me that this
     reloc, unlike the others, is not partial_inplace.  */
  HOWTO (ALPHA_R_BRADDR,	/* type */
	 2,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 21,			/* bitsize */
	 true,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 0,			/* special_function */
	 "BRADDR",		/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0x1fffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* A hint for a jump to a register.  */
  HOWTO (ALPHA_R_HINT,		/* type */
	 2,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 14,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 0,			/* special_function */
	 "HINT",		/* name */
	 true,			/* partial_inplace */
	 0x3fff,		/* src_mask */
	 0x3fff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* 16 bit PC relative offset.  */
  HOWTO (ALPHA_R_SREL16,	/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 true,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 0,			/* special_function */
	 "SREL16",		/* name */
	 true,			/* partial_inplace */
	 0xffff,		/* src_mask */
	 0xffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* 32 bit PC relative offset.  */
  HOWTO (ALPHA_R_SREL32,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 true,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 0,			/* special_function */
	 "SREL32",		/* name */
	 true,			/* partial_inplace */
	 0xffffffff,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* A 64 bit PC relative offset.  */
  HOWTO (ALPHA_R_SREL64,	/* type */
	 0,			/* rightshift */
	 4,			/* size (0 = byte, 1 = short, 2 = long) */
	 64,			/* bitsize */
	 true,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 0,			/* special_function */
	 "SREL64",		/* name */
	 true,			/* partial_inplace */
	 0xffffffffffffffff,	/* src_mask */
	 0xffffffffffffffff,	/* dst_mask */
	 false),		/* pcrel_offset */

  /* Push a value on the reloc evaluation stack.  */
  HOWTO (ALPHA_R_OP_PUSH,	/* type */
	 0,			/* rightshift */
	 0,			/* size (0 = byte, 1 = short, 2 = long) */
	 0,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 0,			/* special_function */
	 "OP_PUSH",		/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 false),		/* pcrel_offset */

  /* Store the value from the stack at the given address.  Store it in
     a bitfield of size r_size starting at bit position r_offset.  */
  HOWTO (ALPHA_R_OP_STORE,	/* type */
	 0,			/* rightshift */
	 4,			/* size (0 = byte, 1 = short, 2 = long) */
	 64,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 0,			/* special_function */
	 "OP_STORE",		/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffffffffffffffff,	/* dst_mask */
	 false),		/* pcrel_offset */

  /* Subtract the reloc address from the value on the top of the
     relocation stack.  */
  HOWTO (ALPHA_R_OP_PSUB,	/* type */
	 0,			/* rightshift */
	 0,			/* size (0 = byte, 1 = short, 2 = long) */
	 0,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 0,			/* special_function */
	 "OP_PSUB",		/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 false),		/* pcrel_offset */

  /* Shift the value on the top of the relocation stack right by the
     given value.  */
  HOWTO (ALPHA_R_OP_PRSHIFT,	/* type */
	 0,			/* rightshift */
	 0,			/* size (0 = byte, 1 = short, 2 = long) */
	 0,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 0,			 /* special_function */
	 "OP_PRSHIFT",		/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 false),		/* pcrel_offset */

  /* Adjust the GP value for a new range in the object file.  */
  HOWTO (ALPHA_R_GPVALUE,	/* type */
	 0,			/* rightshift */
	 0,			/* size (0 = byte, 1 = short, 2 = long) */
	 0,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 0,			/* special_function */
	 "GPVALUE",		/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 false)			/* pcrel_offset */
};

/* See whether the magic number matches.  */

static boolean
alpha_ecoff_bad_format_hook (abfd, filehdr)
     bfd *abfd;
     PTR filehdr;
{
  struct internal_filehdr *internal_f = (struct internal_filehdr *) filehdr;

  if (ALPHA_ECOFF_BADMAG (*internal_f))
    return false;

  return true;
}

/* Reloc handling.  */

/* Swap a reloc in.  */

static void
alpha_ecoff_swap_reloc_in (abfd, ext_ptr, intern)
     bfd *abfd;
     PTR ext_ptr;
     struct internal_reloc *intern;
{
  const RELOC *ext = (RELOC *) ext_ptr;

  intern->r_vaddr = bfd_h_get_64 (abfd, (bfd_byte *) ext->r_vaddr);
  intern->r_symndx = bfd_h_get_32 (abfd, (bfd_byte *) ext->r_symndx);

  BFD_ASSERT (abfd->xvec->header_byteorder_big_p == false);

  intern->r_type = ((ext->r_bits[0] & RELOC_BITS0_TYPE_LITTLE)
		    >> RELOC_BITS0_TYPE_SH_LITTLE);
  intern->r_extern = (ext->r_bits[1] & RELOC_BITS1_EXTERN_LITTLE) != 0;
  intern->r_offset = ((ext->r_bits[1] & RELOC_BITS1_OFFSET_LITTLE)
		      >> RELOC_BITS1_OFFSET_SH_LITTLE);
  /* Ignored the reserved bits.  */
  intern->r_size = ((ext->r_bits[3] & RELOC_BITS3_SIZE_LITTLE)
		    >> RELOC_BITS3_SIZE_SH_LITTLE);

  if (intern->r_type == ALPHA_R_LITUSE
      || intern->r_type == ALPHA_R_GPDISP)
    {
      /* Handle the LITUSE and GPDISP relocs specially.  Its symndx
	 value is not actually a symbol index, but is instead a
	 special code.  We put the code in the r_size field, and
	 clobber the symndx.  */
      if (intern->r_size != 0)
	abort ();
      intern->r_size = intern->r_symndx;
      intern->r_symndx = RELOC_SECTION_NONE;
    }
  else if (intern->r_type == ALPHA_R_IGNORE)
    {
      /* The IGNORE reloc generally follows a GPDISP reloc, and is
	 against the .lita section.  The section is irrelevant.  */
      if (! intern->r_extern &&
	  (intern->r_symndx == RELOC_SECTION_NONE
	   || intern->r_symndx == RELOC_SECTION_ABS))
	abort ();
      if (! intern->r_extern && intern->r_symndx == RELOC_SECTION_LITA)
	intern->r_symndx = RELOC_SECTION_NONE;
    }
}

/* Swap a reloc out.  */

static void
alpha_ecoff_swap_reloc_out (abfd, intern, dst)
     bfd *abfd;
     const struct internal_reloc *intern;
     PTR dst;
{
  RELOC *ext = (RELOC *) dst;
  long symndx;
  unsigned char size;

  /* Undo the hackery done in swap_reloc_in.  */
  if (intern->r_type == ALPHA_R_LITUSE
      || intern->r_type == ALPHA_R_GPDISP)
    {
      symndx = intern->r_size;
      size = 0;
    }
  else if (intern->r_type == ALPHA_R_IGNORE
	   && ! intern->r_extern
	   && intern->r_symndx == RELOC_SECTION_NONE)
    {
      symndx = RELOC_SECTION_LITA;
      size = intern->r_size;
    }
  else
    {
      symndx = intern->r_symndx;
      size = intern->r_size;
    }

  BFD_ASSERT (intern->r_extern
	      || (intern->r_symndx >= 0 && intern->r_symndx <= 14));

  bfd_h_put_64 (abfd, intern->r_vaddr, (bfd_byte *) ext->r_vaddr);
  bfd_h_put_32 (abfd, symndx, (bfd_byte *) ext->r_symndx);

  BFD_ASSERT (abfd->xvec->header_byteorder_big_p == false);

  ext->r_bits[0] = ((intern->r_type << RELOC_BITS0_TYPE_SH_LITTLE)
		    & RELOC_BITS0_TYPE_LITTLE);
  ext->r_bits[1] = ((intern->r_extern ? RELOC_BITS1_EXTERN_LITTLE : 0)
		    | ((intern->r_offset << RELOC_BITS1_OFFSET_SH_LITTLE)
		       & RELOC_BITS1_OFFSET_LITTLE));
  ext->r_bits[2] = 0;
  ext->r_bits[3] = ((size << RELOC_BITS3_SIZE_SH_LITTLE)
		    & RELOC_BITS3_SIZE_LITTLE);
}

/* Finish canonicalizing a reloc.  Part of this is generic to all
   ECOFF targets, and that part is in ecoff.c.  The rest is done in
   this backend routine.  It must fill in the howto field.  */

static void
alpha_adjust_reloc_in (abfd, intern, rptr)
     bfd *abfd;
     const struct internal_reloc *intern;
     arelent *rptr;
{
  if (intern->r_type > ALPHA_R_GPVALUE)
    abort ();

  switch (intern->r_type)
    {
    case ALPHA_R_BRADDR:
    case ALPHA_R_SREL16:
    case ALPHA_R_SREL32:
    case ALPHA_R_SREL64:
      /* The PC relative relocs do not seem to use the section VMA as
	 a negative addend.  */
      rptr->addend = 0;
      break;

    case ALPHA_R_GPREL32:
    case ALPHA_R_LITERAL:
      /* Copy the gp value for this object file into the addend, to
	 ensure that we are not confused by the linker.  */
      if (! intern->r_extern)
	rptr->addend += ecoff_data (abfd)->gp;
      break;

    case ALPHA_R_LITUSE:
    case ALPHA_R_GPDISP:
      /* The LITUSE and GPDISP relocs do not use a symbol, or an
	 addend, but they do use a special code.  Put this code in the
	 addend field.  */
      rptr->addend = intern->r_size;
      break;

    case ALPHA_R_OP_STORE:
      /* The STORE reloc needs the size and offset fields.  We store
	 them in the addend.  */
      BFD_ASSERT (intern->r_offset <= 256 && intern->r_size <= 256);
      rptr->addend = (intern->r_offset << 8) + intern->r_size;
      break;

    case ALPHA_R_OP_PUSH:
    case ALPHA_R_OP_PSUB:
    case ALPHA_R_OP_PRSHIFT:
      /* The PUSH, PSUB and PRSHIFT relocs do not actually use an
	 address.  I believe that the address supplied is really an
	 addend.  */
      rptr->addend = intern->r_vaddr;
      break;

    case ALPHA_R_GPVALUE:
      /* Set the addend field to the new GP value.  */
      rptr->addend = intern->r_symndx + ecoff_data (abfd)->gp;
      break;

    case ALPHA_R_IGNORE:
      /* If the type is ALPHA_R_IGNORE, make sure this is a reference
	 to the absolute section so that the reloc is ignored.  For
	 some reason the address of this reloc type is not adjusted by
	 the section vma.  We record the gp value for this object file
	 here, for convenience when doing the GPDISP relocation.  */
      rptr->sym_ptr_ptr = bfd_abs_section.symbol_ptr_ptr;
      rptr->address = intern->r_vaddr;
      rptr->addend = ecoff_data (abfd)->gp;
      break;

    default:
      break;
    }

  rptr->howto = &alpha_howto_table[intern->r_type];
}

/* When writing out a reloc we need to pull some values back out of
   the addend field into the reloc.  This is roughly the reverse of
   alpha_adjust_reloc_in, except that there are several changes we do
   not need to undo.  */

static void
alpha_adjust_reloc_out (abfd, rel, intern)
     bfd *abfd;
     const arelent *rel;
     struct internal_reloc *intern;
{
  switch (intern->r_type)
    {
    case ALPHA_R_LITUSE:
    case ALPHA_R_GPDISP:
      intern->r_size = rel->addend;
      break;

    case ALPHA_R_OP_STORE:
      intern->r_size = rel->addend & 0xff;
      intern->r_offset = (rel->addend >> 8) & 0xff;
      break;

    case ALPHA_R_OP_PUSH:
    case ALPHA_R_OP_PSUB:
    case ALPHA_R_OP_PRSHIFT:
      intern->r_vaddr = rel->addend;
      break;

    case ALPHA_R_IGNORE:
      intern->r_vaddr = rel->address;
      if (intern->r_symndx == RELOC_SECTION_ABS)
	intern->r_symndx = RELOC_SECTION_NONE;
      break;

    default:
      break;
    }
}

/* The size of the stack for the relocation evaluator.  */
#define RELOC_STACKSIZE (10)

/* Alpha ECOFF relocs have a built in expression evaluator as well as
   other interdependencies.  Rather than use a bunch of special
   functions and global variables, we use a single routine to do all
   the relocation for a section.  I haven't yet worked out how the
   assembler is going to handle this.  */

static bfd_byte *
alpha_ecoff_get_relocated_section_contents (abfd, seclet, data,
					    relocateable)
     bfd *abfd;
     struct bfd_seclet *seclet;
     bfd_byte *data;
     boolean relocateable;
{
  extern bfd_error_vector_type bfd_error_vector;
  bfd *input_bfd = seclet->u.indirect.section->owner;
  asection *input_section = seclet->u.indirect.section;
  asection *output_section = input_section->output_section;
  size_t reloc_size = bfd_get_reloc_upper_bound (input_bfd, input_section);
  arelent **reloc_vector = (arelent **) alloca (reloc_size);
  bfd *output_bfd = relocateable ? abfd : (bfd *) NULL;
  bfd_vma gp;
  int gp_undefined;
  int gp_warned = 0;
  bfd_vma stack[RELOC_STACKSIZE];
  int tos = 0;

  if (! bfd_get_section_contents (input_bfd, input_section, data,
				  (file_ptr) 0, input_section->_raw_size))
    return NULL;

  /* The section size is not going to change.  */
  input_section->_cooked_size = input_section->_raw_size;
  input_section->reloc_done = true;

  if (bfd_canonicalize_reloc (input_bfd, input_section, reloc_vector,
			      seclet->u.indirect.symbols)
      == 0)
    return data;

  /* Get the GP value for the output BFD.  */
  gp_undefined = 0;
  if (ecoff_data (abfd)->gp == 0)
    {
      if (relocateable != false)
	{
	  asection *sec;
	  bfd_vma lo;

	  /* Make up a value.  */
	  lo = (bfd_vma) -1;
	  for (sec = abfd->sections; sec != NULL; sec = sec->next)
	    {
	      if (sec->vma < lo
		  && (strcmp (sec->name, ".sbss") == 0
		      || strcmp (sec->name, ".sdata") == 0
		      || strcmp (sec->name, ".lit4") == 0
		      || strcmp (sec->name, ".lit8") == 0
		      || strcmp (sec->name, ".lita") == 0))
		lo = sec->vma;
	    }
	  ecoff_data (abfd)->gp = lo + 0x8000;
	}
      else
	{
	  unsigned int count;
	  asymbol **sym;
	  unsigned int i;

	  count = bfd_get_symcount (abfd);
	  sym = bfd_get_outsymbols (abfd);

	  if (sym == (asymbol **) NULL)
	    i = count;
	  else
	    {
	      for (i = 0; i < count; i++, sym++)
		{
		  register CONST char *name;

		  name = bfd_asymbol_name (*sym);
		  if (*name == '_' && strcmp (name, "_gp") == 0)
		    {
		      ecoff_data (abfd)->gp = bfd_asymbol_value (*sym);
		      break;
		    }
		}
	    }

	  if (i >= count)
	    gp_undefined = 1;
	}
    }
  gp = ecoff_data (abfd)->gp;

  for (; *reloc_vector != (arelent *) NULL; reloc_vector++)
    {
      arelent *rel;
      bfd_reloc_status_type r;

      rel = *reloc_vector;
      r = bfd_reloc_ok;
      switch (rel->howto->type)
	{
	case ALPHA_R_IGNORE:
	  rel->address += input_section->output_offset;
	  break;

	case ALPHA_R_REFLONG:
	case ALPHA_R_REFQUAD:
	case ALPHA_R_BRADDR:
	case ALPHA_R_HINT:
	case ALPHA_R_SREL16:
	case ALPHA_R_SREL32:
	case ALPHA_R_SREL64:
	  if (relocateable
	      && ((*rel->sym_ptr_ptr)->flags & BSF_SECTION_SYM) == 0)
	    {
	      rel->address += input_section->output_offset;
	      break;
	    }
	  r = bfd_perform_relocation (input_bfd, rel, data, input_section,
				      output_bfd);
	  break;

	case ALPHA_R_GPREL32:
	  /* This relocation is used in a switch table.  It is a 32
	     bit offset from the current GP value.  We must adjust it
	     by the different between the original GP value and the
	     current GP value.  The original GP value is stored in the
	     addend.  We adjust the addend and let
	     bfd_perform_relocation finish the job.  */
	  rel->addend -= gp;
	  r = bfd_perform_relocation (input_bfd, rel, data, input_section,
				      output_bfd);
	  if (r == bfd_reloc_ok && gp_undefined)
	    r = bfd_reloc_dangerous;
	  break;

	case ALPHA_R_LITERAL:
	  /* This is a reference to a literal value, generally
	     (always?) in the .lita section.  This is a 16 bit GP
	     relative relocation.  Sometimes the subsequent reloc is a
	     LITUSE reloc, which indicates how this reloc is used.
	     This sometimes permits rewriting the two instructions
	     referred to by the LITERAL and the LITUSE into different
	     instructions which do not refer to .lita.  This can save
	     a memory reference, and permits removing a value from
	     .lita thus saving GP relative space.

	     We do not these optimizations.  To do them we would need
	     to arrange to link the .lita section first, so that by
	     the time we got here we would know the final values to
	     use.  This would not be particularly difficult, but it is
	     not currently implemented.  */

	  {
	    unsigned long insn;

	    /* I believe that the LITERAL reloc will only apply to a
	       ldq instruction, so check my assumption.  */
	    insn = bfd_get_32 (input_bfd, data + rel->address);
	    BFD_ASSERT (((insn >> 26) & 0x3f) == 0x29);

	    rel->addend -= gp;
	    r = bfd_perform_relocation (input_bfd, rel, data, input_section,
					output_bfd);
	    if (r == bfd_reloc_ok && gp_undefined)
	      r = bfd_reloc_dangerous;
	  }
	  break;

	case ALPHA_R_LITUSE:
	  /* See ALPHA_R_LITERAL above for the uses of this reloc.  It
	     does not cause anything to happen, itself.  */
	  rel->address += input_section->output_offset;
	  break;
	    
	case ALPHA_R_GPDISP:
	  /* This marks the ldah of an ldah/lda pair which loads the
	     gp register with the difference of the gp value and the
	     current location.  The second of the pair is r_size bytes
	     ahead, and is marked with an ALPHA_R_IGNORE reloc.  */
	  {
	    unsigned long insn1, insn2;
	    bfd_vma addend;

	    BFD_ASSERT (reloc_vector[1] != NULL
			&& reloc_vector[1]->howto->type == ALPHA_R_IGNORE
			&& (rel->address + rel->addend
			    == reloc_vector[1]->address));

	    /* Get the two instructions.  */
	    insn1 = bfd_get_32 (input_bfd, data + rel->address);
	    insn2 = bfd_get_32 (input_bfd, data + rel->address + rel->addend);

	    BFD_ASSERT (((insn1 >> 26) & 0x3f) == 0x09); /* ldah */
	    BFD_ASSERT (((insn2 >> 26) & 0x3f) == 0x08); /* lda */

	    /* Get the existing addend.  We must account for the sign
	       extension done by lda and ldah.  */
	    addend = ((insn1 & 0xffff) << 16) + (insn2 & 0xffff);
	    if (insn1 & 0x8000)
	      {
		addend -= 0x80000000;
		addend -= 0x80000000;
	      }
	    if (insn2 & 0x8000)
	      addend -= 0x10000;

	    /* The existing addend includes the different between the
	       gp of the input BFD and the address in the input BFD.
	       Subtract this out.  */
	    addend -= (reloc_vector[1]->addend
		       - (input_section->vma + rel->address));

	    /* Now add in the final gp value, and subtract out the
	       final address.  */
	    addend += (gp
		       - (input_section->output_section->vma
			  + input_section->output_offset
			  + rel->address));

	    /* Change the instructions, accounting for the sign
	       extension, and write them out.  */
	    if (addend & 0x8000)
	      addend += 0x10000;
	    insn1 = (insn1 & 0xffff0000) | ((addend >> 16) & 0xffff);
	    insn2 = (insn2 & 0xffff0000) | (addend & 0xffff);

	    bfd_put_32 (input_bfd, (bfd_vma) insn1, data + rel->address);
	    bfd_put_32 (input_bfd, (bfd_vma) insn2,
			data + rel->address + rel->addend);

	    rel->address += input_section->output_offset;
	  }
	  break;
	  
	case ALPHA_R_OP_PUSH:
	  /* Push a value on the reloc evaluation stack.  */
	  {
	    asymbol *symbol;
	    bfd_vma relocation;

	    if (relocateable)
	      {
		rel->address += input_section->output_offset;
		break;
	      }

	    /* Figure out the relocation of this symbol.  */
	    symbol = *rel->sym_ptr_ptr;

	    if (symbol->section == &bfd_und_section)
	      r = bfd_reloc_undefined;

	    if (bfd_is_com_section (symbol->section))
	      relocation = 0;
	    else
	      relocation = symbol->value;
	    relocation += symbol->section->output_section->vma;
	    relocation += symbol->section->output_offset;
	    relocation += rel->addend;

	    if (tos >= RELOC_STACKSIZE)
	      abort ();

	    stack[tos++] = relocation;
	  }
	  break;

	case ALPHA_R_OP_STORE:
	  /* Store a value from the reloc stack into a bitfield.  */
	  {
	    bfd_vma val;
	    int offset, size;

	    if (relocateable)
	      {
		rel->address += input_section->output_offset;
		break;
	      }

	    if (tos == 0)
	      abort ();

	    /* The offset and size for this reloc are encoded into the
	       addend field by alpha_adjust_reloc_in.  */
	    offset = (rel->addend >> 8) & 0xff;
	    size = rel->addend & 0xff;

	    val = bfd_get_64 (abfd, data + rel->address);
	    val &=~ (((1 << size) - 1) << offset);
	    val |= (stack[--tos] & ((1 << size) - 1)) << offset;
	    bfd_put_64 (abfd, val, data + rel->address);
	  }
	  break;

	case ALPHA_R_OP_PSUB:
	  /* Subtract a value from the top of the stack.  */
	  {
	    asymbol *symbol;
	    bfd_vma relocation;

	    if (relocateable)
	      {
		rel->address += input_section->output_offset;
		break;
	      }

	    /* Figure out the relocation of this symbol.  */
	    symbol = *rel->sym_ptr_ptr;

	    if (symbol->section == &bfd_und_section)
	      r = bfd_reloc_undefined;

	    if (bfd_is_com_section (symbol->section))
	      relocation = 0;
	    else
	      relocation = symbol->value;
	    relocation += symbol->section->output_section->vma;
	    relocation += symbol->section->output_offset;
	    relocation += rel->addend;

	    if (tos == 0)
	      abort ();

	    stack[tos - 1] -= relocation;
	  }
	  break;

	case ALPHA_R_OP_PRSHIFT:
	  /* Shift the value on the top of the stack.  */
	  {
	    asymbol *symbol;
	    bfd_vma relocation;

	    if (relocateable)
	      {
		rel->address += input_section->output_offset;
		break;
	      }

	    /* Figure out the relocation of this symbol.  */
	    symbol = *rel->sym_ptr_ptr;

	    if (symbol->section == &bfd_und_section)
	      r = bfd_reloc_undefined;

	    if (bfd_is_com_section (symbol->section))
	      relocation = 0;
	    else
	      relocation = symbol->value;
	    relocation += symbol->section->output_section->vma;
	    relocation += symbol->section->output_offset;
	    relocation += rel->addend;

	    if (tos == 0)
	      abort ();

	    stack[tos - 1] >>= relocation;
	  }
	  break;
	    
	case ALPHA_R_GPVALUE:
	  /* I really don't know if this does the right thing.  */
	  gp = rel->addend;
	  gp_undefined = 0;
	  break;

	default:
	  abort ();
	}

      if (relocateable)
	{
	  asection *os = input_section->output_section;

	  /* A partial link, so keep the relocs.  */
	  os->orelocation[os->reloc_count] = rel;
	  os->reloc_count++;
	}

      if (r != bfd_reloc_ok) 
	{
	  switch (r)
	    {
	    case bfd_reloc_undefined:
	      bfd_error_vector.undefined_symbol (rel, seclet);
	      break;
	    case bfd_reloc_dangerous: 
	      if (! gp_warned)
		{
		  /* FIXME: How can we get the program name here?  */
		  fprintf (stderr,
			   "gp relative relocation when gp not defined");
		  gp_warned = 1;
		}
	      bfd_error_vector.reloc_dangerous (rel, seclet);
	      break;
	    case bfd_reloc_outofrange:
	    case bfd_reloc_overflow:
	      bfd_error_vector.reloc_value_truncated (rel, seclet);
	      break;
	    default:
	      abort ();
	      break;
	    }
	}
    }

  if (tos != 0)
    abort ();

  return data;
}

/* Get the howto structure for a generic reloc type.  */

static CONST struct reloc_howto_struct *
alpha_bfd_reloc_type_lookup (abfd, code)
     bfd *abfd;
     bfd_reloc_code_real_type code;
{
  int alpha_type;

  switch (code)
    {
    case BFD_RELOC_32:
      alpha_type = ALPHA_R_REFLONG;
      break;
    default:
      return (CONST struct reloc_howto_struct *) NULL;
    }

  return &alpha_howto_table[alpha_type];
}

#define ecoff_core_file_p _bfd_dummy_target
#define ecoff_core_file_failing_command	_bfd_dummy_core_file_failing_command
#define ecoff_core_file_failing_signal _bfd_dummy_core_file_failing_signal
#define ecoff_core_file_matches_executable_p \
  _bfd_dummy_core_file_matches_executable_p

/* This is the ECOFF backend structure.  The backend field of the
   target vector points to this.  */

static const struct ecoff_backend_data alpha_ecoff_backend_data =
{
  /* COFF backend structure.  */
  {
    (void (*) PARAMS ((bfd *,PTR,int,int,PTR))) bfd_void, /* aux_in */
    (void (*) PARAMS ((bfd *,PTR,PTR))) bfd_void, /* sym_in */
    (void (*) PARAMS ((bfd *,PTR,PTR))) bfd_void, /* lineno_in */
    (unsigned (*) PARAMS ((bfd *,PTR,int,int,PTR))) bfd_void, /* aux_out */
    (unsigned (*) PARAMS ((bfd *,PTR,PTR))) bfd_void, /* sym_out */
    (unsigned (*) PARAMS ((bfd *,PTR,PTR))) bfd_void, /* lineno_out */
    (unsigned (*) PARAMS ((bfd *,PTR,PTR))) bfd_void, /* reloc_out */
    alpha_ecoff_swap_filehdr_out, alpha_ecoff_swap_aouthdr_out,
    alpha_ecoff_swap_scnhdr_out,
    FILHSZ, AOUTSZ, SCNHSZ, 0, 0, 0, true,
    alpha_ecoff_swap_filehdr_in, alpha_ecoff_swap_aouthdr_in,
    alpha_ecoff_swap_scnhdr_in, alpha_ecoff_bad_format_hook,
    ecoff_set_arch_mach_hook, ecoff_mkobject_hook,
    ecoff_styp_to_sec_flags, ecoff_make_section_hook, ecoff_set_alignment_hook,
    ecoff_slurp_symbol_table, NULL, NULL
  },
  /* Supported architecture.  */
  bfd_arch_alpha,
  /* Symbol table magic number.  */
  magicSym2,
  /* Initial portion of armap string.  */
  "________64",
  /* Alignment of debugging information.  E.g., 4.  */
  8,
  /* The page boundary used to align sections in a demand-paged
     executable file.  E.g., 0x1000.  */
  0x2000,
  /* True if the .rdata section is part of the text segment, as on the
     Alpha.  False if .rdata is part of the data segment, as on the
     MIPS.  */
  true,
  /* Bitsize of constructor entries.  */
  64,
  /* Reloc to use for constructor entries.  */
  &alpha_howto_table[ALPHA_R_REFQUAD],
  /* Sizes of external symbolic information.  */
  sizeof (struct hdr_ext),
  sizeof (struct dnr_ext),
  sizeof (struct pdr_ext),
  sizeof (struct sym_ext),
  sizeof (struct opt_ext),
  sizeof (struct fdr_ext),
  sizeof (struct rfd_ext),
  sizeof (struct ext_ext),
  /* Functions to swap in external symbolic data.  */
  ecoff_swap_hdr_in,
  ecoff_swap_dnr_in,
  ecoff_swap_pdr_in,
  ecoff_swap_sym_in,
  ecoff_swap_opt_in,
  ecoff_swap_fdr_in,
  ecoff_swap_rfd_in,
  ecoff_swap_ext_in,
  /* Functions to swap out external symbolic data.  */
  ecoff_swap_hdr_out,
  ecoff_swap_dnr_out,
  ecoff_swap_pdr_out,
  ecoff_swap_sym_out,
  ecoff_swap_opt_out,
  ecoff_swap_fdr_out,
  ecoff_swap_rfd_out,
  ecoff_swap_ext_out,
  /* External reloc size.  */
  RELSZ,
  /* Reloc swapping functions.  */
  alpha_ecoff_swap_reloc_in,
  alpha_ecoff_swap_reloc_out,
  /* Backend reloc tweaking.  */
  alpha_adjust_reloc_in,
  alpha_adjust_reloc_out
};

/* Looking up a reloc type is Alpha specific.  */
#define ecoff_bfd_reloc_type_lookup alpha_bfd_reloc_type_lookup

/* So is getting relocated section contents.  */
#define ecoff_bfd_get_relocated_section_contents \
  alpha_ecoff_get_relocated_section_contents

bfd_target ecoffalpha_little_vec =
{
  "ecoff-littlealpha",		/* name */
  bfd_target_ecoff_flavour,
  false,			/* data byte order is little */
  false,			/* header byte order is little */

  (HAS_RELOC | EXEC_P |		/* object flags */
   HAS_LINENO | HAS_DEBUG |
   HAS_SYMS | HAS_LOCALS | WP_TEXT | D_PAGED),

  (SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD | SEC_RELOC), /* sect
							    flags */
  0,				/* leading underscore */
  ' ',				/* ar_pad_char */
  15,				/* ar_max_namelen */
  4,				/* minimum alignment power */
  bfd_getl64, bfd_getl_signed_64, bfd_putl64,
     bfd_getl32, bfd_getl_signed_32, bfd_putl32,
     bfd_getl16, bfd_getl_signed_16, bfd_putl16, /* data */
  bfd_getl64, bfd_getl_signed_64, bfd_putl64,
     bfd_getl32, bfd_getl_signed_32, bfd_putl32,
     bfd_getl16, bfd_getl_signed_16, bfd_putl16, /* hdrs */

  {_bfd_dummy_target, coff_object_p, /* bfd_check_format */
     ecoff_archive_p, _bfd_dummy_target},
  {bfd_false, ecoff_mkobject,  /* bfd_set_format */
     _bfd_generic_mkarchive, bfd_false},
  {bfd_false, ecoff_write_object_contents, /* bfd_write_contents */
     _bfd_write_archive_contents, bfd_false},
  JUMP_TABLE (ecoff),
  (PTR) &alpha_ecoff_backend_data
};
