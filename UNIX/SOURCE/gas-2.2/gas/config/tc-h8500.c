/* tc-h8500.c -- Assemble code for the Hitachi H8/500
   Copyright (C) 1993 Free Software Foundation.

   This file is part of GAS, the GNU Assembler.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   GAS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GAS; see the file COPYING.  If not, write to
   the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

/*
  Written By Steve Chamberlain
  sac@cygnus.com
  */

#include <stdio.h>
#include "as.h"
#include "bfd.h"
#include "subsegs.h"
#define DEFINE_TABLE
#define ASSEMBLER_TABLE
#include "../opcodes/h8500-opc.h"
#include <ctype.h>

const char comment_chars[] = "!";
const char line_separator_chars[] = ";";
const char line_comment_chars[] = "!#";

/* This table describes all the machine specific pseudo-ops the assembler
   has to support.  The fields are:
   pseudo-op name without dot
   function to call to execute this pseudo-op
   Integer arg to pass to the function
   */

void cons ();

const pseudo_typeS md_pseudo_table[] =
{
  {"int", cons, 2},
  {"data.b", cons, 1},
  {"data.w", cons, 2},
  {"data.l", cons, 4},
  {"form", listing_psize, 0},
  {"heading", listing_title, 0},
  {"import", s_ignore, 0},
  {"page", listing_eject, 0},
  {"program", s_ignore, 0},
  {0, 0, 0}
};

const int md_reloc_size;

const char EXP_CHARS[] = "eE";

/* Chars that mean this number is a floating point constant */
/* As in 0f12.456 */
/* or    0d1.2345e12 */
const char FLT_CHARS[] = "rRsSfFdDxXpP";

#define C(a,b) ENCODE_RELAX(a,b)
#define ENCODE_RELAX(what,length) (((what) << 2) + (length))

#define GET_WHAT(x) ((x>>2))

#define BYTE_DISP 1
#define WORD_DISP 2
#define UNDEF_BYTE_DISP 0
#define UNDEF_WORD_DISP 3

#define BRANCH  1
#define SCB_F   2
#define SCB_TST 3
#define END 4

#define BYTE_F 127
#define BYTE_B -126
#define WORD_F 32767
#define WORD_B 32768

const relax_typeS md_relax_table[C (END, 0)];

static struct hash_control *opcode_hash_control;	/* Opcode mnemonics */

/*
  This function is called once, at assembler startup time.  This should
  set up all the tables, etc that the MD part of the assembler needs
  */

void
md_begin ()
{
  h8500_opcode_info *opcode;
  char prev_buffer[100];
  int idx = 0;
  register relax_typeS *table;

  opcode_hash_control = hash_new ();
  prev_buffer[0] = 0;

  /* Insert unique names into hash table */
  for (opcode = h8500_table; opcode->name; opcode++)
    {
      if (idx != opcode->idx)
	{
	  hash_insert (opcode_hash_control, opcode->name, (char *) opcode);
	  idx++;
	}
    }

  /* Initialize the relax table.  We use a local variable to avoid
     warnings about modifying a supposedly const data structure.  */
  table = (relax_typeS *) md_relax_table;
  table[C (BRANCH, BYTE_DISP)].rlx_forward = BYTE_F;
  table[C (BRANCH, BYTE_DISP)].rlx_backward = BYTE_B;
  table[C (BRANCH, BYTE_DISP)].rlx_length = 2;
  table[C (BRANCH, BYTE_DISP)].rlx_more = C (BRANCH, WORD_DISP);

  table[C (BRANCH, WORD_DISP)].rlx_forward = WORD_F;
  table[C (BRANCH, WORD_DISP)].rlx_backward = WORD_B;
  table[C (BRANCH, WORD_DISP)].rlx_length = 3;
  table[C (BRANCH, WORD_DISP)].rlx_more = 0;

  table[C (SCB_F, BYTE_DISP)].rlx_forward = BYTE_F;
  table[C (SCB_F, BYTE_DISP)].rlx_backward = BYTE_B;
  table[C (SCB_F, BYTE_DISP)].rlx_length = 3;
  table[C (SCB_F, BYTE_DISP)].rlx_more = C (SCB_F, WORD_DISP);

  table[C (SCB_F, WORD_DISP)].rlx_forward = WORD_F;
  table[C (SCB_F, WORD_DISP)].rlx_backward = WORD_B;
  table[C (SCB_F, WORD_DISP)].rlx_length = 8;
  table[C (SCB_F, WORD_DISP)].rlx_more = 0;

  table[C (SCB_TST, BYTE_DISP)].rlx_forward = BYTE_F;
  table[C (SCB_TST, BYTE_DISP)].rlx_backward = BYTE_B;
  table[C (SCB_TST, BYTE_DISP)].rlx_length = 3;
  table[C (SCB_TST, BYTE_DISP)].rlx_more = C (SCB_TST, WORD_DISP);

  table[C (SCB_TST, WORD_DISP)].rlx_forward = WORD_F;
  table[C (SCB_TST, WORD_DISP)].rlx_backward = WORD_B;
  table[C (SCB_TST, WORD_DISP)].rlx_length = 10;
  table[C (SCB_TST, WORD_DISP)].rlx_more = 0;

}

static int rn;			/* register number used by RN */
static int rs;			/* register number used by RS */
static int rd;			/* register number used by RD */
static int crb;			/* byte size cr */
static int crw;			/* word sized cr */
static int cr;			/* unknown size cr */

static expressionS displacement;/* displacement expression */
static int displacement_size;	/* and size if given */

static int immediate_inpage;
static expressionS immediate;	/* immediate expression */
static int immediate_size;	/* and size if given */

static expressionS absolute;	/* absolute expression */
static int absolute_size;	/* and size if given */

typedef struct
{
  int type;
  int reg;
  expressionS exp;
  int page;
}

h8500_operand_info;

/* try and parse a reg name, returns number of chars consumed */
static int
parse_reg (src, mode, reg)
     char *src;
     int *mode;
     int *reg;
{
  if (src[0] == 'r')
    {
      if (src[1] >= '0' && src[1] <= '7')
	{
	  *mode = RN;
	  *reg = (src[1] - '0');
	  return 2;
	}
    }

  if (src[0] == 's' && src[1] == 'p')
    {
      *mode = RN;
      *reg = 7;
      return 2;
    }
  if (src[0] == 'c' && src[1] == 'c' && src[2] == 'r')
    {
      *mode = CRB;
      *reg = 1;
      return 3;
    }
  if (src[0] == 's' && src[1] == 'r')
    {
      *mode = CRW;
      *reg = 0;
      return 2;
    }

  if (src[0] == 'b' && src[1] == 'r')
    {
      *mode = CRB;
      *reg = 3;
      return 2;
    }

  if (src[0] == 'e' && src[1] == 'p')
    {
      *mode = CRB;
      *reg = 4;
      return 2;
    }

  if (src[0] == 'd' && src[1] == 'p')
    {
      *mode = CRB;
      *reg = 5;
      return 2;
    }

  if (src[0] == 't' && src[1] == 'p')
    {
      *mode = CRB;
      *reg = 7;
      return 2;
    }

  if (src[0] == 'f' && src[1] == 'p')
    {
      *mode = RN;
      *reg = 6;
      return 2;
    }
  return 0;
}

static
char *
parse_exp (s, op, page)
     char *s;
     expressionS *op;
     int *page;
{
  char *save;
  char *new;

  save = input_line_pointer;

  *page = 0;
  if (s[0] == '%')
    {
      if (s[1] == 'p' && s[2] == 'a' && s[3] == 'g' && s[4] == 'e')
	{
	  s += 5;
	  *page = 'p';
	}
      if (s[1] == 'h' && s[2] == 'i' && s[3] == '1' && s[4] == '6')
	{
	  s += 5;
	  *page = 'h';
	}
      else if (s[1] == 'o' && s[2] == 'f' && s[3] == 'f')
	{
	  s += 4;
	  *page = 'o';
	}
    }

  input_line_pointer = s;

  expression (op);
  if (op->X_op == O_absent)
    as_bad ("missing operand");
  new = input_line_pointer;
  input_line_pointer = save;
  return new;
}

typedef enum
  {
    exp_signed, exp_unsigned, exp_sandu
  } sign_type;


static char *
skip_colonthing (sign, ptr, exp, def, size8, size16, size24)
     sign_type sign;
     char *ptr;
     h8500_operand_info *exp;
     int def;
     int size8;
     int size16;
     int size24;
{
  ptr = parse_exp (ptr, &exp->exp, &exp->page);
  if (*ptr == ':')
    {
      ptr++;
      if (*ptr == '8')
	{
	  ptr++;
	  exp->type = size8;
	}
      else if (ptr[0] == '1' & ptr[1] == '6')
	{
	  ptr += 2;
	  exp->type = size16;
	}
      else if (ptr[0] == '2' & ptr[1] == '4')
	{
	  if (!size24)
	    {
	      as_bad (":24 not valid for this opcode");
	    }
	  ptr += 2;
	  exp->type = size24;
	}
      else
	{
	  as_bad ("expect :8,:16 or :24");
	  exp->type = size16;
	}
    }
  else
    {
      if (exp->page == 'p')
	{
	  exp->type = IMM8;
	}
      else if (exp->page == 'h')
	{
	  exp->type = IMM16;
	}
      else
	{
	  /* Let's work out the size from the context */
	  int n = exp->exp.X_add_number;
	  if (size8
	      && exp->exp.X_op == O_constant
	      && ((sign == exp_signed && (n >= -128 && n <= 127))
		  || (sign == exp_unsigned && (n >= 0 && (n <= 255)))
		  || (sign == exp_sandu && (n >= -128 && (n <= 255)))))
	    {
	      exp->type = size8;
	    }
	  else
	    {
	      exp->type = def;
	    }
	}
    }
  return ptr;
}

static int
parse_reglist (src, op)
     char *src;
     h8500_operand_info *op;
{
  int mode;
  int rn;
  int mask = 0;
  int rm;
  int idx = 1;			/* skip ( */

  while (src[idx] && src[idx] != ')')
    {
      int done = parse_reg (src + idx, &mode, &rn);

      if (done)
	{
	  idx += done;
	  mask |= 1 << rn;
	}
      else
	{
	  as_bad ("syntax error in reg list");
	  return 0;
	}
      if (src[idx] == '-')
	{
	  idx++;
	  done = parse_reg (src + idx, &mode, &rm);
	  if (done)
	    {
	      idx += done;
	      while (rn <= rm)
		{
		  mask |= 1 << rn;
		  rn++;
		}
	    }
	  else
	    {
	      as_bad ("missing final register in range");
	    }
	}
      if (src[idx] == ',')
	idx++;
    }
  idx++;
  op->exp.X_add_symbol = 0;
  op->exp.X_op_symbol = 0;
  op->exp.X_add_number = mask;
  op->exp.X_op = O_constant;
  op->exp.X_unsigned = 1;
  op->type = IMM8;
  return idx;

}

/* The many forms of operand:

   Rn			Register direct
   @Rn			Register indirect
   @(disp[:size], Rn)	Register indirect with displacement
   @Rn+
   @-Rn
   @aa[:size]		absolute
   #xx[:size]		immediate data

   */

static void
get_operand (ptr, op, ispage)
     char **ptr;
     h8500_operand_info *op;
     char ispage;
{
  char *src = *ptr;
  int mode;
  unsigned int num;
  unsigned int len;
  op->page = 0;
  if (src[0] == '(' && src[1] == 'r')
    {
      /* This is a register list */
      *ptr = src + parse_reglist (src, op);
      return;
    }

  len = parse_reg (src, &op->type, &op->reg);

  if (len)
    {
      *ptr = src + len;
      return;
    }

  if (*src == '@')
    {
      src++;
      if (*src == '-')
	{
	  src++;
	  len = parse_reg (src, &mode, &num);
	  if (len == 0)
	    {
	      /* Oops, not a reg after all, must be ordinary exp */
	      src--;
	      /* must be a symbol */
	      *ptr = skip_colonthing (exp_unsigned, src,
				      op, ABS16, ABS8, ABS16, ABS24);
	      return;
	    }

	  op->type = RNDEC;
	  op->reg = num;
	  *ptr = src + len;
	  return;
	}
      if (*src == '(')
	{
	  /* Disp */
	  src++;

	  src = skip_colonthing (exp_signed, src, 
				 op, RNIND_D16, RNIND_D8, RNIND_D16, 0);

	  if (*src != ',')
	    {
	      as_bad ("expected @(exp, Rn)");
	      return;
	    }
	  src++;
	  len = parse_reg (src, &mode, &op->reg);
	  if (len == 0 || mode != RN)
	    {
	      as_bad ("expected @(exp, Rn)");
	      return;
	    }
	  src += len;
	  if (*src != ')')
	    {
	      as_bad ("expected @(exp, Rn)");
	      return;
	    }
	  *ptr = src + 1;
	  return;
	}
      len = parse_reg (src, &mode, &num);

      if (len)
	{
	  src += len;
	  if (*src == '+')
	    {
	      src++;
	      if (mode != RN)
		{
		  as_bad ("@Rn+ needs word register");
		  return;
		}
	      op->type = RNINC;
	      op->reg = num;
	      *ptr = src;
	      return;
	    }
	  if (mode != RN)
	    {
	      as_bad ("@Rn needs word register");
	      return;
	    }
	  op->type = RNIND;
	  op->reg = num;
	  *ptr = src;
	  return;
	}
      else
	{
	  /* must be a symbol */
	  *ptr =
	    skip_colonthing (exp_unsigned, src, op,
			     ispage ? ABS24 : ABS16, ABS8, ABS16, ABS24);
	  return;
	}
    }

  if (*src == '#')
    {
      src++;
      *ptr = skip_colonthing (exp_sandu, src, op, IMM16, IMM8, IMM16, ABS24);
      return;
    }
  else
    {
      *ptr = skip_colonthing (exp_signed, src, op,
			      ispage ? ABS24 : PCREL8, PCREL8, PCREL16, ABS24);
    }
}

static
char *
get_operands (info, args, operand)
     h8500_opcode_info *info;
     char *args;
     h8500_operand_info *operand;

{
  char *ptr = args;

  switch (info->nargs)
    {
    case 0:
      operand[0].type = 0;
      operand[1].type = 0;
      break;

    case 1:
      ptr++;
      get_operand (&ptr, operand + 0, info->name[0] == 'p');
      operand[1].type = 0;
      break;

    case 2:
      ptr++;
      get_operand (&ptr, operand + 0, 0);
      if (*ptr == ',')
	ptr++;
      get_operand (&ptr, operand + 1, 0);
      break;

    default:
      abort ();
    }

  return ptr;
}

/* Passed a pointer to a list of opcodes which use different
   addressing modes, return the opcode which matches the opcodes
   provided
   */

int pcrel8;			/* Set when we've seen a pcrel operand */

static
h8500_opcode_info *
get_specific (opcode, operands)
     h8500_opcode_info *opcode;
     h8500_operand_info *operands;
{
  h8500_opcode_info *this_try = opcode;
  int found = 0;
  unsigned int noperands = opcode->nargs;

  unsigned int this_index = opcode->idx;

  while (this_index == opcode->idx && !found)
    {
      unsigned int i;

      this_try = opcode++;

      /* look at both operands needed by the opcodes and provided by
       the user*/
      for (i = 0; i < noperands; i++)
	{
	  h8500_operand_info *user = operands + i;

	  switch (this_try->arg_type[i])
	    {
	    case FPIND_D8:
	      /* Opcode needs (disp:8,fp) */
	      if (user->type == RNIND_D8 && user->reg == 6)
		{
		  displacement = user->exp;
		  continue;
		}
	      break;
	    case RDIND_D16:
	      if (user->type == RNIND_D16)
		{
		  displacement = user->exp;
		  rd = user->reg;
		  continue;
		}
	      break;
	    case RDIND_D8:
	      if (user->type == RNIND_D8)
		{
		  displacement = user->exp;
		  rd = user->reg;
		  continue;
		}
	      break;
	    case RNIND_D16:
	    case RNIND_D8:
	      if (user->type == this_try->arg_type[i])
		{
		  displacement = user->exp;
		  rn = user->reg;
		  continue;
		}
	      break;

	    case SPDEC:
	      if (user->type == RNDEC && user->reg == 7)
		{
		  continue;
		}
	      break;
	    case SPINC:
	      if (user->type == RNINC && user->reg == 7)
		{
		  continue;
		}
	      break;
	    case ABS16:
	      if (user->type == ABS16)
		{
		  absolute = user->exp;
		  continue;
		}
	      break;
	    case ABS8:
	      if (user->type == ABS8)
		{
		  absolute = user->exp;
		  continue;
		}
	      break;
	    case ABS24:
	      if (user->type == ABS24)
		{
		  absolute = user->exp;
		  continue;
		}
	      break;

	    case CRB:
	      if ((user->type == CRB || user->type == CR) && user->reg != 0)
		{
		  crb = user->reg;
		  continue;
		}
	      break;
	    case CRW:
	      if ((user->type == CRW || user->type == CR) && user->reg == 0)
		{
		  crw = user->reg;
		  continue;
		}
	      break;
	    case DISP16:
	      if (user->type == DISP16)
		{
		  displacement = user->exp;
		  continue;
		}
	      break;
	    case DISP8:
	      if (user->type == DISP8)
		{
		  displacement = user->exp;
		  continue;
		}
	      break;
	    case FP:
	      if (user->type == RN && user->reg == 6)
		{
		  continue;
		}
	      break;
	    case PCREL16:
	      if (user->type == PCREL16)
		{
		  displacement = user->exp;
		  continue;
		}
	      break;
	    case PCREL8:
	      if (user->type == PCREL8)
		{
		  displacement = user->exp;
		  pcrel8 = 1;
		  continue;
		}
	      break;

	    case IMM16:
	      if (user->type == IMM16
		  || user->type == IMM8)
		{
		  immediate_inpage = user->page;
		  immediate = user->exp;
		  continue;
		}
	      break;
	    case RLIST:
	    case IMM8:
	      if (user->type == IMM8)
		{
		  immediate_inpage = user->page;
		  immediate = user->exp;
		  continue;
		}
	      break;
	    case IMM4:
	      if (user->type == IMM8)
		{
		  immediate_inpage = user->page;
		  immediate = user->exp;
		  continue;
		}
	      break;
	    case QIM:
	      if (user->type == IMM8
		  && user->exp.X_op == O_constant
		  &&
		  (user->exp.X_add_number == -2
		   || user->exp.X_add_number == -1
		   || user->exp.X_add_number == 1
		   || user->exp.X_add_number == 2))
		{
		  immediate_inpage = user->page;
		  immediate = user->exp;
		  continue;
		}
	      break;
	    case RD:
	      if (user->type == RN)
		{
		  rd = user->reg;
		  continue;
		}
	      break;
	    case RS:
	      if (user->type == RN)
		{
		  rs = user->reg;
		  continue;
		}
	      break;
	    case RDIND:
	      if (user->type == RNIND)
		{
		  rd = user->reg;
		  continue;

		}
	      break;
	    case RNINC:
	    case RNIND:
	    case RNDEC:
	    case RN:

	      if (user->type == this_try->arg_type[i])
		{
		  rn = user->reg;
		  continue;
		}
	      break;
	    case SP:
	      if (user->type == RN && user->reg == 7)
		{
		  continue;
		}
	      break;
	    default:
	      printf ("unhandled %d\n", this_try->arg_type[i]);
	      break;
	    }

	  /* If we get here this didn't work out */
	  goto fail;
	}
      found = 1;
    fail:;

    }

  if (found)
    return this_try;
  else
    return 0;
}

int
check (operand, low, high)
     expressionS *operand;
     int low;
     int high;
{
  if (operand->X_op != O_constant
      || operand->X_add_number < low
      || operand->X_add_number > high)
    {
      as_bad ("operand must be absolute in range %d..%d", low, high);
    }
  return operand->X_add_number;
}

static
void
insert (output, index, exp, reloc, pcrel)
     char *output;
     int index;
     expressionS *exp;
     int reloc;
     int pcrel;
{
  fix_new_exp (frag_now,
	       output - frag_now->fr_literal + index,
	       4,	       	/* always say size is 4, but we know better */
	       exp,
	       pcrel,
	       reloc);
}

void
build_relaxable_instruction (opcode, operand)
     h8500_opcode_info *opcode;
     h8500_operand_info *operand;
{
  /* All relaxable instructions start life as two bytes but can become
     three bytes long if a lonely branch and up to 9 bytes if long scb
     */
  char *p;
  int len;
  int type;

  if (opcode->bytes[0].contents == 0x01)
    {
      type = SCB_F;
    }
  else if (opcode->bytes[0].contents == 0x06
	   || opcode->bytes[0].contents == 0x07)
    {
      type = SCB_TST;
    }
  else
    {
      type = BRANCH;
    }

  p = frag_var (rs_machine_dependent,
		md_relax_table[C (type, WORD_DISP)].rlx_length,
		len = md_relax_table[C (type, BYTE_DISP)].rlx_length,
		C (type, UNDEF_BYTE_DISP),
		displacement.X_add_symbol,
		displacement.X_add_number,
		0);

  p[0] = opcode->bytes[0].contents;
  if (type != BRANCH)
    {
      p[1] = opcode->bytes[1].contents | rs;
    }
}

/* Now we know what sort of opcodes it is, lets build the bytes -
 */
static void
build_bytes (opcode, operand)
     h8500_opcode_info *opcode;
     h8500_operand_info *operand;

{
  int index;

  if (pcrel8)
    {
      pcrel8 = 0;
      build_relaxable_instruction (opcode, operand);
    }
  else
    {
      char *output = frag_more (opcode->length);

      memset (output, 0, opcode->length);
      for (index = 0; index < opcode->length; index++)
	{
	  output[index] = opcode->bytes[index].contents;

	  switch (opcode->bytes[index].insert)
	    {
	    default:
	      printf ("failed for %d\n", opcode->bytes[index].insert);
	      break;
	    case 0:
	      break;
	    case RN:
	      output[index] |= rn;
	      break;
	    case RD:
	    case RDIND:
	      
	      output[index] |= rd;
	      break;
	    case RS:
	      output[index] |= rs;
	      break;
	    case DISP16:
	      insert (output, index, &displacement, R_H8500_IMM16, 0);
	      index++;
	      break;
	    case DISP8:
	    case FPIND_D8:
	      insert (output, index, &displacement, R_H8500_IMM8, 0);
	      break;

	    case IMM16:
	      {
		int p;
		switch (immediate_inpage) {
		case 'p':
		  p = R_H8500_LOW16;
		  break;
		case 'h':		
		  p = R_H8500_HIGH16;
		  break;
		default:
		  p = R_H8500_IMM16;
		  break;
		}
		
		insert (output, index, &immediate,p, 0);
	      }
		
	      index++;
	      break;
	    case RLIST:
	    case IMM8:
	      if (immediate_inpage)
		{
		  insert (output, index, &immediate, R_H8500_HIGH8, 0);
		}
	      else
		{
		  insert (output, index, &immediate, R_H8500_IMM8, 0);
		}
	      break;
	    case PCREL16:
	      insert (output, index, &displacement, R_H8500_PCREL16, 1);
	      index++;
	      break;
	    case PCREL8:
	      insert (output, index, &displacement, R_H8500_PCREL8, 1);
	      break;
	    case IMM4:
	      output[index] |= check (&immediate, 0, 15);
	      break;
	    case CR:

	      output[index] |= cr;
	      if (cr == 0)
		{
		  output[0] |= 0x8;
		}
	      else
		{
		  output[0] &= ~0x8;
		}

	      break;

	    case CRB:
	      output[index] |= crb;
	      output[0] &= ~0x8;
	      break;
	    case CRW:
	      output[index] |= crw;
	      output[0] |= 0x8;
	      break;
	    case ABS24:
	      insert (output, index, &absolute, R_H8500_IMM24, 0);
	      index += 2;
	      break;
	    case ABS16:
	      insert (output, index, &absolute, R_H8500_IMM16, 0);
	      index++;
	      break;
	    case ABS8:
	      insert (output, index, &absolute, R_H8500_IMM8, 0);
	      break;
	    case QIM:
	      switch (immediate.X_add_number)
		{
		case -2:
		  output[index] |= 0x5;
		  break;
		case -1:
		  output[index] |= 0x4;
		  break;
		case 1:
		  output[index] |= 0;
		  break;
		case 2:
		  output[index] |= 1;
		  break;
		}
	      break;
	    }
	}
    }
}

/* This is the guts of the machine-dependent assembler.  STR points to a
   machine dependent instruction.  This funciton is supposed to emit
   the frags/bytes it assembles to.
   */

void
DEFUN (md_assemble, (str),
       char *str)
{
  char *op_start;
  char *op_end;
  h8500_operand_info operand[2];
  h8500_opcode_info *opcode;
  h8500_opcode_info *prev_opcode;
  char name[11];

  int nlen = 0;

  /* Drop leading whitespace */
  while (*str == ' ')
    str++;

  /* find the op code end */
  for (op_start = op_end = str;
       *op_end &&
       !is_end_of_line[*op_end] && *op_end != ' ';
       op_end++)
    {
      if (			/**op_end != '.'
	  && *op_end != ':'
	   	   	   	  && */ nlen < 10)
	{
	  name[nlen++] = *op_end;
	}
    }
  name[nlen] = 0;

  if (op_end == op_start)
    {
      as_bad ("can't find opcode ");
    }

  opcode = (h8500_opcode_info *) hash_find (opcode_hash_control, name);

  if (opcode == NULL)
    {
      as_bad ("unknown opcode");
      return;
    }

  input_line_pointer = get_operands (opcode, op_end, operand);
  prev_opcode = opcode;

  opcode = get_specific (opcode, operand);

  if (opcode == 0)
    {
      /* Couldn't find an opcode which matched the operands */
      char *where = frag_more (2);

      where[0] = 0x0;
      where[1] = 0x0;
      as_bad ("invalid operands for opcode");
      return;
    }

  build_bytes (opcode, operand);

}

void
DEFUN (tc_crawl_symbol_chain, (headers),
       object_headers * headers)
{
  printf ("call to tc_crawl_symbol_chain \n");
}

symbolS *
DEFUN (md_undefined_symbol, (name),
       char *name)
{
  return 0;
}

void
DEFUN (tc_headers_hook, (headers),
       object_headers * headers)
{
  printf ("call to tc_headers_hook \n");
}

void
DEFUN_VOID (md_end)
{
}

/* Various routines to kill one day */
/* Equal to MAX_PRECISION in atof-ieee.c */
#define MAX_LITTLENUMS 6

/* Turn a string in input_line_pointer into a floating point constant of type
   type, and store the appropriate bytes in *litP.  The number of LITTLENUMS
   emitted is stored in *sizeP .  An error message is returned, or NULL on OK.
   */
char *
md_atof (type, litP, sizeP)
     char type;
     char *litP;
     int *sizeP;
{
  int prec;
  LITTLENUM_TYPE words[MAX_LITTLENUMS];
  LITTLENUM_TYPE *wordP;
  char *t;
  char *atof_ieee ();

  switch (type)
    {
    case 'f':
    case 'F':
    case 's':
    case 'S':
      prec = 2;
      break;

    case 'd':
    case 'D':
    case 'r':
    case 'R':
      prec = 4;
      break;

    case 'x':
    case 'X':
      prec = 6;
      break;

    case 'p':
    case 'P':
      prec = 6;
      break;

    default:
      *sizeP = 0;
      return "Bad call to MD_ATOF()";
    }
  t = atof_ieee (input_line_pointer, type, words);
  if (t)
    input_line_pointer = t;

  *sizeP = prec * sizeof (LITTLENUM_TYPE);
  for (wordP = words; prec--;)
    {
      md_number_to_chars (litP, (long) (*wordP++), sizeof (LITTLENUM_TYPE));
      litP += sizeof (LITTLENUM_TYPE);
    }
  return 0;
}

int
md_parse_option (argP, cntP, vecP)
     char **argP;
     int *cntP;
     char ***vecP;

{
  return 0;
}

int md_short_jump_size;

void
tc_aout_fix_to_chars ()
{
  printf ("call to tc_aout_fix_to_chars \n");
  abort ();
}

void
md_create_short_jump (ptr, from_addr, to_addr, frag, to_symbol)
     char *ptr;
     addressT from_addr;
     addressT to_addr;
     fragS *frag;
     symbolS *to_symbol;
{
  as_fatal ("failed sanity check.");
}

void
md_create_long_jump (ptr, from_addr, to_addr, frag, to_symbol)
     char *ptr;
     addressT from_addr, to_addr;
     fragS *frag;
     symbolS *to_symbol;
{
  as_fatal ("failed sanity check.");
}

static
void
wordify_scb (buffer, disp_size, inst_size)
     char *buffer;
     int *disp_size;
     int *inst_size;
{
  int rn = buffer[1] & 0x7;

  switch (buffer[0])
    {
    case 0x0e:			/* BSR */
    case 0x20:
    case 0x21:
    case 0x22:
    case 0x23:
    case 0x24:
    case 0x25:
    case 0x26:
    case 0x27:
    case 0x28:
    case 0x29:
    case 0x2a:
    case 0x2b:
    case 0x2c:
    case 0x2d:
    case 0x2e:
    case 0x2f:
      buffer[0] |= 0x10;
      buffer[1] = 0;
      buffer[2] = 0;
      *disp_size = 2;
      *inst_size = 1;
      return;
    default:
      abort ();

    case 0x01:
      *inst_size = 6;
      *disp_size = 2;
      break;
    case 0x06:
      *inst_size = 8;
      *disp_size = 2;

      *buffer++ = 0x26;		/* bne + 8 */
      *buffer++ = 0x08;
      break;
    case 0x07:
      *inst_size = 8;
      *disp_size = 2;
      *buffer++ = 0x27;		/* bne + 8 */
      *buffer++ = 0x08;
      break;

    }
  *buffer++ = 0xa8 | rn;	/* addq -1,rn */
  *buffer++ = 0x0c;
  *buffer++ = 0x04;		/* cmp #0xff:8, rn */
  *buffer++ = 0xff;
  *buffer++ = 0x70 | rn;
  *buffer++ = 0x36;		/* bne ... */
  *buffer++ = 0;
  *buffer++ = 0;
}

/*
called after relaxing, change the frags so they know how big they are
*/
void
md_convert_frag (headers, fragP)
     object_headers *headers;
     fragS *fragP;

{
  int disp_size = 0;
  int inst_size = 0;
  char *buffer = fragP->fr_fix + fragP->fr_literal;

  switch (fragP->fr_subtype)
    {
    case C (BRANCH, BYTE_DISP):
      disp_size = 1;
      inst_size = 1;
      break;

    case C (SCB_F, BYTE_DISP):
    case C (SCB_TST, BYTE_DISP):
      disp_size = 1;
      inst_size = 2;
      break;

      /* Branches to a known 16 bit displacement */

      /* Turn on the 16bit bit */
    case C (BRANCH, WORD_DISP):
    case C (SCB_F, WORD_DISP):
    case C (SCB_TST, WORD_DISP):
      wordify_scb (buffer, &disp_size, &inst_size);
      break;

    case C (BRANCH, UNDEF_WORD_DISP):
    case C (SCB_F, UNDEF_WORD_DISP):
    case C (SCB_TST, UNDEF_WORD_DISP):
      /* This tried to be relaxed, but didn't manage it, it now needs a
       fix */
      wordify_scb (buffer, &disp_size, &inst_size);

      /* Make a reloc */
      fix_new (fragP,
	       fragP->fr_fix + inst_size,
	       4,
	       fragP->fr_symbol,
	       fragP->fr_offset,
	       0,
	       R_H8500_PCREL16);

      fragP->fr_fix += disp_size + inst_size;
      fragP->fr_var = 0;
      return;
      break;
    default:
      abort ();
    }
  if (inst_size)
    {
      /* Get the address of the end of the instruction */
      int next_inst = fragP->fr_fix + fragP->fr_address + disp_size + inst_size;
      int targ_addr = (S_GET_VALUE (fragP->fr_symbol) +
		       fragP->fr_offset);
      int disp = targ_addr - next_inst;

      md_number_to_chars (buffer + inst_size, disp, disp_size);
      fragP->fr_fix += disp_size + inst_size;
      fragP->fr_var = 0;
    }

}

valueT
md_section_align (seg, size)
     segT seg ;
     valueT size;
{
  return ((size + (1 << section_alignment[(int) seg]) - 1) 
	  & (-1 << section_alignment[(int) seg]));

}

void
md_apply_fix (fixP, val)
     fixS *fixP;
     long val;
{
  char *buf = fixP->fx_where + fixP->fx_frag->fr_literal;

  if (fixP->fx_r_type == 0)
    {
      fixP->fx_r_type = fixP->fx_size == 4 ? R_H8500_IMM32 : R_H8500_IMM16;
    }

  switch (fixP->fx_r_type)
    {

    case R_H8500_IMM8:
    case R_H8500_PCREL8:
      *buf++ = val;
      break;
    case R_H8500_IMM16:
    case R_H8500_LOW16:
    case R_H8500_PCREL16:
      *buf++ = (val >> 8);
      *buf++ = val;
      break;
    case R_H8500_HIGH8:
      *buf++ = val >> 16;
      break;
    case R_H8500_HIGH16:
      *buf++ = val >> 24;
      *buf++ = val >> 16;
      break;
    case R_H8500_IMM24:
      *buf++ = (val >> 16);
      *buf++ = (val >> 8);
      *buf++ = val;
      break;
    case R_H8500_IMM32:
      *buf++ = (val >> 24);
      *buf++ = (val >> 16);
      *buf++ = (val >> 8);
      *buf++ = val;
      break;
    default:
      abort ();

    }
}

void
DEFUN (md_operand, (expressionP), expressionS * expressionP)
{
}

int md_long_jump_size;

/*
called just before address relaxation, return the length
by which a fragment must grow to reach it's destination
*/
int
md_estimate_size_before_relax (fragP, segment_type)
     register fragS *fragP;
     register segT segment_type;
{
  int what = GET_WHAT (fragP->fr_subtype);

  switch (fragP->fr_subtype)
    {
    default:
      abort ();
    case C (BRANCH, UNDEF_BYTE_DISP):
    case C (SCB_F, UNDEF_BYTE_DISP):
    case C (SCB_TST, UNDEF_BYTE_DISP):
      /* used to be a branch to somewhere which was unknown */
      if (S_GET_SEGMENT (fragP->fr_symbol) == segment_type)
	{
	  /* Got a symbol and it's defined in this segment, become byte
	 sized - maybe it will fix up */
	  fragP->fr_subtype = C (what, BYTE_DISP);
	  fragP->fr_var = md_relax_table[C (what, BYTE_DISP)].rlx_length;
	}
      else
	{
	  /* Its got a segment, but its not ours, so it will always be long */
	  fragP->fr_subtype = C (what, UNDEF_WORD_DISP);
	  fragP->fr_var = md_relax_table[C (what, WORD_DISP)].rlx_length;
	  return md_relax_table[C (what, WORD_DISP)].rlx_length;
	}
    }
  return fragP->fr_var;
}

/* Put number into target byte order */

void
md_number_to_chars (ptr, use, nbytes)
     char *ptr;
     valueT use;
     int nbytes;
{
  switch (nbytes)
    {
    case 4:
      *ptr++ = (use >> 24) & 0xff;
    case 3:
      *ptr++ = (use >> 16) & 0xff;
    case 2:
      *ptr++ = (use >> 8) & 0xff;
    case 1:
      *ptr++ = (use >> 0) & 0xff;
      break;
    default:
      abort ();
    }
}
long
md_pcrel_from (fixP)
     fixS *fixP;

{
  return fixP->fx_size + fixP->fx_where + fixP->fx_frag->fr_address;
}

void
tc_coff_symbol_emit_hook ()
{
}

short
tc_coff_fix2rtype (fix_ptr)
     fixS *fix_ptr;
{
  if (fix_ptr->fx_r_type == RELOC_32)
    {
      /* cons likes to create reloc32's whatever the size of the reloc..
     */
      switch (fix_ptr->fx_size)
	{
	case 2:
	  return R_H8500_IMM16;
	  break;
	case 1:
	  return R_H8500_IMM8;
	  break;
	default:
	  abort ();
	}
    }
  return fix_ptr->fx_r_type;
}

void
tc_reloc_mangle (fix_ptr, intr, base)
     fixS *fix_ptr;
     struct internal_reloc *intr;
     bfd_vma base;

{
  symbolS *symbol_ptr;

  symbol_ptr = fix_ptr->fx_addsy;

  /* If this relocation is attached to a symbol then it's ok
     to output it */
  if (fix_ptr->fx_r_type == RELOC_32)
    {
      /* cons likes to create reloc32's whatever the size of the reloc..
       */
      switch (fix_ptr->fx_size)
	{
	case 2:
	  intr->r_type = R_IMM16;
	  break;
	case 1:
	  intr->r_type = R_IMM8;
	  break;
	default:
	  abort ();
	}
    }
  else
    {
      intr->r_type = fix_ptr->fx_r_type;
    }

  intr->r_vaddr = fix_ptr->fx_frag->fr_address + fix_ptr->fx_where + base;
  intr->r_offset = fix_ptr->fx_offset;

  /* Turn the segment of the symbol into an offset.  */
  if (symbol_ptr)
    {
      symbolS *dot;

      dot = segment_info[S_GET_SEGMENT (symbol_ptr)].dot;
      if (dot)
	{
	  /*	  intr->r_offset -=
	    segment_info[S_GET_SEGMENT(symbol_ptr)].scnhdr.s_paddr;*/
	  intr->r_offset += S_GET_VALUE (symbol_ptr);
	  intr->r_symndx = dot->sy_number;
	}
      else
	{
	  intr->r_symndx = symbol_ptr->sy_number;
	}

    }
  else
    {
      intr->r_symndx = -1;
    }

}



int
start_label (ptr)
     char *ptr;
{
  /* Check for :s.w */
  if (isalpha (ptr[1]) && ptr[2] == '.')
    return 0;
  /* Check for :s */
  if (isalpha (ptr[1]) && !isalpha (ptr[2]))
    return 0;
  return 1;
}

/* end of tc-h8500.c */
