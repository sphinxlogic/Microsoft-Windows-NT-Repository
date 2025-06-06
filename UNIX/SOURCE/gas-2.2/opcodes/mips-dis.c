/* Print mips instructions for GDB, the GNU debugger, or for objdump.
   Copyright 1989, 1991, 1992 Free Software Foundation, Inc.
   Contributed by Nobuyuki Hikichi(hikichi@sra.co.jp).

This file is part of GDB.

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

#include <ansidecl.h>
#include "sysdep.h"
#include "dis-asm.h"
#include "opcode/mips.h"

/* Mips instructions are never longer than this many bytes.  */
#define MAXLEN 4

/* FIXME: This should be shared with gdb somehow.  */
#define REGISTER_NAMES 	\
    {	"zero",	"at",	"v0",	"v1",	"a0",	"a1",	"a2",	"a3", \
	"t0",	"t1",	"t2",	"t3",	"t4",	"t5",	"t6",	"t7", \
	"s0",	"s1",	"s2",	"s3",	"s4",	"s5",	"s6",	"s7", \
	"t8",	"t9",	"k0",	"k1",	"gp",	"sp",	"s8",	"ra", \
	"sr",	"lo",	"hi",	"bad",	"cause","pc",    \
	"f0",   "f1",   "f2",   "f3",   "f4",   "f5",   "f6",   "f7", \
	"f8",   "f9",   "f10",  "f11",  "f12",  "f13",  "f14",  "f15", \
	"f16",  "f17",  "f18",  "f19",  "f20",  "f21",  "f22",  "f23",\
	"f24",  "f25",  "f26",  "f27",  "f28",  "f29",  "f30",  "f31",\
	"fsr",  "fir",  "fp",   "inx",  "rand", "tlblo","ctxt", "tlbhi",\
	"epc",  "prid"\
    }

static CONST char * CONST reg_names[] = REGISTER_NAMES;

/* subroutine */
static void
print_insn_arg (d, l, pc, info)
     const char *d;
     register unsigned long int l;
     bfd_vma pc;
     struct disassemble_info *info;
{
  int delta;

  switch (*d)
    {
    case ',':
    case '(':
    case ')':
      (*info->fprintf_func) (info->stream, "%c", *d);
      break;

    case 's':
    case 'b':
    case 'r':
    case 'v':
      (*info->fprintf_func) (info->stream, "$%s",
			     reg_names[(l >> OP_SH_RS) & OP_MASK_RS]);
      break;

    case 't':
    case 'w':
      (*info->fprintf_func) (info->stream, "$%s",
			     reg_names[(l >> OP_SH_RT) & OP_MASK_RT]);
      break;

    case 'i':
    case 'u':
      (*info->fprintf_func) (info->stream, "%d",
			(l >> OP_SH_IMMEDIATE) & OP_MASK_IMMEDIATE);
      break;

    case 'j': /* same as i, but sign-extended */
    case 'o':
      delta = (l >> OP_SH_DELTA) & OP_MASK_DELTA;
      if (delta & 0x8000)
	delta |= ~0xffff;
      (*info->fprintf_func) (info->stream, "%d",
			     delta);
      break;

    case 'k':
      (*info->fprintf_func) (info->stream, "0x%x",
			     (unsigned int) ((l >> OP_SH_CACHE)
					     & OP_MASK_CACHE));
      break;

    case 'a':
      (*info->print_address_func)
	(((pc & 0xF0000000) | (((l >> OP_SH_TARGET) & OP_MASK_TARGET) << 2)),
	 info);
      break;

    case 'p':
      /* sign extend the displacement */
      delta = (l >> OP_SH_DELTA) & OP_MASK_DELTA;
      if (delta & 0x8000)
	delta |= ~0xffff;
      (*info->print_address_func)
	((delta << 2) + pc + 4,
	 info);
      break;

    case 'd':
      (*info->fprintf_func) (info->stream, "$%s",
			     reg_names[(l >> OP_SH_RD) & OP_MASK_RD]);
      break;

    case 'z':
      (*info->fprintf_func) (info->stream, "$%s", reg_names[0]);
      break;

    case '<':
      (*info->fprintf_func) (info->stream, "0x%x",
			     (l >> OP_SH_SHAMT) & OP_MASK_SHAMT);
      break;

    case 'c':
      (*info->fprintf_func) (info->stream, "0x%x",
			     (l >> OP_SH_CODE) & OP_MASK_CODE);
      break;

    case 'C':
      (*info->fprintf_func) (info->stream, "0x%x",
			     (l >> OP_SH_COPZ) & OP_MASK_COPZ);
      break;

    case 'B':
      (*info->fprintf_func) (info->stream, "0x%x",
			     (l >> OP_SH_SYSCALL) & OP_MASK_SYSCALL);
      break;

    case 'S':
    case 'V':
      (*info->fprintf_func) (info->stream, "$f%d",
			     (l >> OP_SH_FS) & OP_MASK_FS);
      break;

    case 'T':
    case 'W':
      (*info->fprintf_func) (info->stream, "$f%d",
			     (l >> OP_SH_FT) & OP_MASK_FT);
      break;

    case 'D':
      (*info->fprintf_func) (info->stream, "$f%d",
			     (l >> OP_SH_FD) & OP_MASK_FD);
      break;

    case 'E':
      (*info->fprintf_func) (info->stream, "$%d",
			     (l >> OP_SH_RT) & OP_MASK_RT);
      break;

    case 'G':
      (*info->fprintf_func) (info->stream, "$%d",
			     (l >> OP_SH_RD) & OP_MASK_RD);
      break;

    default:
      (*info->fprintf_func) (info->stream,
			     "# internal error, undefined modifier(%c)", *d);
      break;
    }
}

/* Print the mips instruction at address MEMADDR in debugged memory,
   on using INFO.  Returns length of the instruction, in bytes, which is
   always 4.  BIGENDIAN must be 1 if this is big-endian code, 0 if
   this is little-endian code.  */

int
_print_insn_mips (memaddr, word, info)
     bfd_vma memaddr;
     struct disassemble_info *info;
     unsigned long int word;
{
  register int i;
  register const char *d;

  for (i = 0; i < NUMOPCODES; i++)
    {
      if (mips_opcodes[i].pinfo != INSN_MACRO)
	{
	  register unsigned int match = mips_opcodes[i].match;
	  register unsigned int mask = mips_opcodes[i].mask;
	  if ((word & mask) == match)
	    break;
	}
    }

  /* Handle undefined instructions.  */
  if (i == NUMOPCODES)
    {
      (*info->fprintf_func) (info->stream, "0x%x", word);
      return 4;
    }

  (*info->fprintf_func) (info->stream, "%s", mips_opcodes[i].name);

  if (!(d = mips_opcodes[i].args))
    return 4;

  (*info->fprintf_func) (info->stream, " ");

  while (*d)
    print_insn_arg (d++, word, memaddr, info);

  return 4;
}

int
print_insn_big_mips (memaddr, info)
     bfd_vma memaddr;
     struct disassemble_info *info;
{
  bfd_byte buffer[4];
  int status = (*info->read_memory_func) (memaddr, buffer, 4, info);
  if (status == 0)
    return _print_insn_mips (memaddr, bfd_getb32 (buffer), info);
  else
    {
      (*info->memory_error_func) (status, memaddr, info);
      return -1;
    }
}

int
print_insn_little_mips (memaddr, info)
     bfd_vma memaddr;
     struct disassemble_info *info;
{
  bfd_byte buffer[4];
  int status = (*info->read_memory_func) (memaddr, buffer, 4, info);
  if (status == 0)
    return _print_insn_mips (memaddr, bfd_getl32 (buffer), info);
  else
    {
      (*info->memory_error_func) (status, memaddr, info);
      return -1;
    }
}
