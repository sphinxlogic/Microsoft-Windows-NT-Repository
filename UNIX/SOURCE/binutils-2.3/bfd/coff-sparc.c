/* BFD back-end for Sparc COFF files.
   Copyright 1990, 1991, 1992, 1993 Free Software Foundation, Inc.
   Written by Cygnus Support.

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
#include "coff/sparc.h"
#include "coff/internal.h"
#include "libcoff.h"

#define BADMAG(x) ((x).f_magic != SPARCMAGIC)

static void
DEFUN (rtype2howto, (internal, dst),
       arelent * internal AND
       struct internal_reloc *dst)
{
  switch (dst->r_type)
    {
    default:
      fprintf (stderr, "BAD 0x%x\n", dst->r_type);
    }
}

#define RTYPE2HOWTO(internal, relocentry) rtype2howto(internal,relocentry)

#define __A_MAGIC_SET__

#include "coffcode.h"

bfd_target
#ifdef TARGET_SYM
  TARGET_SYM =
#else
  sparccoff_vec =
#endif
{
#ifdef TARGET_NAME
  TARGET_NAME,
#else
  "coff-sparc",			/* name */
#endif
  bfd_target_coff_flavour,
  true,			/* data byte order is big */
  true,			/* header byte order is big */

  (HAS_RELOC | EXEC_P |		/* object flags */
   HAS_LINENO | HAS_DEBUG |
   HAS_SYMS | HAS_LOCALS | DYNAMIC | WP_TEXT),

  (SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD | SEC_RELOC), /* section flags */
  0,				/* leading underscore */
  '/',				/* ar_pad_char */
  15,				/* ar_max_namelen */

  2,				/* minimum alignment power */
  bfd_getb64, bfd_getb_signed_64, bfd_putb64,
     bfd_getb32, bfd_getb_signed_32, bfd_putb32,
     bfd_getb16, bfd_getb_signed_16, bfd_putb16, /* data */
  bfd_getb64, bfd_getb_signed_64, bfd_putb64,
     bfd_getb32, bfd_getb_signed_32, bfd_putb32,
     bfd_getb16, bfd_getb_signed_16, bfd_putb16, /* hdrs */

/* Note that we allow an object file to be treated as a core file as well. */
    {_bfd_dummy_target, coff_object_p, /* bfd_check_format */
       bfd_generic_archive_p, coff_object_p},
    {bfd_false, coff_mkobject, _bfd_generic_mkarchive, /* bfd_set_format */
       bfd_false},
    {bfd_false, coff_write_object_contents, /* bfd_write_contents */
       _bfd_write_archive_contents, bfd_false},

  JUMP_TABLE(coff),
  COFF_SWAP_TABLE,
};

