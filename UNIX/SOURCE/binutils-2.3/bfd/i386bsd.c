/* BFD back-end for i386 a.out binaries under BSD.
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

/* This data should be correct for the format used under all the various
   BSD ports for 386 machines.  */

#define	BYTES_IN_WORD	4
#define	ARCH	32

/* ZMAGIC files never have the header in the text.  */
#define	N_HEADER_IN_TEXT(x)	0

/* ZMAGIC files start at address 0.  This does not apply to QMAGIC.  */
#define TEXT_START_ADDR 0
#define N_SHARED_LIB(x) 0

#define	PAGE_SIZE	4096
#define	SEGMENT_SIZE	PAGE_SIZE

#define	DEFAULT_ARCH	bfd_arch_i386
#define MACHTYPE_OK(mtype) ((mtype) == M_386 || (mtype) == M_UNKNOWN)

#define MY(OP) CAT(i386bsd_,OP)
#define TARGETNAME "a.out-i386-bsd"

#include "bfd.h"
#include "sysdep.h"
#include "libbfd.h"
#include "libaout.h"

#include "aout-target.h"
