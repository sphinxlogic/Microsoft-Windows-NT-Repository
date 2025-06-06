/* m88k.c -- Assembler for the Motorola 88000
   Contributed by Devon Bowen of Buffalo University
   and Torbjorn Granlund of the Swedish Institute of Computer Science.
   Copyright (C) 1989, 1990, 1991, 1993 Free Software Foundation, Inc.

This file is part of GAS, the GNU Assembler.

GAS is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

GAS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GAS; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include <ctype.h>
#include "as.h"
#include "m88k-opcode.h"

struct field_val_assoc
{
  char *name;
  unsigned val;
};

struct field_val_assoc cr_regs[] =
{
  {"PID", 0},
  {"PSR", 1},
  {"EPSR", 2},
  {"SSBR", 3},
  {"SXIP", 4},
  {"SNIP", 5},
  {"SFIP", 6},
  {"VBR", 7},
  {"DMT0", 8},
  {"DMD0", 9},
  {"DMA0", 10},
  {"DMT1", 11},
  {"DMD1", 12},
  {"DMA1", 13},
  {"DMT2", 14},
  {"DMD2", 15},
  {"DMA2", 16},
  {"SR0", 17},
  {"SR1", 18},
  {"SR2", 19},
  {"SR3", 20},

  {NULL, 0},
};

struct field_val_assoc fcr_regs[] =
{
  {"FPECR", 0},
  {"FPHS1", 1},
  {"FPLS1", 2},
  {"FPHS2", 3},
  {"FPLS2", 4},
  {"FPPT", 5},
  {"FPRH", 6},
  {"FPRL", 7},
  {"FPIT", 8},

  {"FPSR", 62},
  {"FPCR", 63},

  {NULL, 0},
};

struct field_val_assoc cmpslot[] =
{
/* Integer	Floating point */
  {"nc", 0},
  {"cp", 1},
  {"eq", 2},
  {"ne", 3},
  {"gt", 4},
  {"le", 5},
  {"lt", 6},
  {"ge", 7},
  {"hi", 8},	{"ou", 8},
  {"ls", 9},	{"ib", 9},
  {"lo", 10},	{"in", 10},
  {"hs", 11},	{"ob", 11},
  {"be", 12},	{"ue", 12},
  {"nb", 13},	{"lg", 13},
  {"he", 14},	{"ug", 14},
  {"nh", 15},	{"ule", 15},
		{"ul", 16},
		{"uge", 17},

  {NULL, 0},
};

struct field_val_assoc cndmsk[] =
{
  {"gt0", 1},
  {"eq0", 2},
  {"ge0", 3},
  {"lt0", 12},
  {"ne0", 13},
  {"le0", 14},

  {NULL, 0},
};

struct m88k_insn
{
  unsigned long opcode;
  expressionS exp;
  enum reloc_type reloc;
};

static char *get_bf PARAMS ((char *param, unsigned *valp));
static char *get_cmp PARAMS ((char *param, unsigned *valp));
static char *get_cnd PARAMS ((char *param, unsigned *valp));
static char *get_cr PARAMS ((char *param, unsigned *regnop));
static char *get_fcr PARAMS ((char *param, unsigned *regnop));
static char *get_imm16 PARAMS ((char *param, struct m88k_insn *insn));
static char *get_o6 PARAMS ((char *param, unsigned *valp));
static char *get_reg PARAMS ((char *param, unsigned *regnop, int reg_prefix));
static char *get_vec9 PARAMS ((char *param, unsigned *valp));
static char *getval PARAMS ((char *param, unsigned int *valp));

static char *get_pcr PARAMS ((char *param, struct m88k_insn *insn,
		      enum reloc_type reloc));

static int calcop PARAMS ((struct m88k_opcode *format,
			   char *param, struct m88k_insn *insn));


extern char *myname;
static struct hash_control *op_hash = NULL;

/* These bits should be turned off in the first address of every segment */
int md_seg_align = 7;

/* These chars start a comment anywhere in a source file (except inside
   another comment */
const char comment_chars[] = ";";

/* These chars only start a comment at the beginning of a line. */
const char line_comment_chars[] = "#";

const char line_separator_chars[] = "";

/* Chars that can be used to separate mant from exp in floating point nums */
const char EXP_CHARS[] = "eE";

/* Chars that mean this number is a floating point constant */
/* as in 0f123.456 */
/* or    0H1.234E-12 (see exp chars above) */
const char FLT_CHARS[] = "dDfF";

extern void float_cons (), cons (), s_globl (), s_space (),
  s_set (), s_lcomm ();

const pseudo_typeS md_pseudo_table[] =
{
  {"align", s_align_bytes, 4},
  {"def", s_set, 0},
  {"dfloat", float_cons, 'd'},
  {"ffloat", float_cons, 'f'},
  {"global", s_globl, 0},
  {"half", cons, 2},
  {"bss", s_lcomm, 1},
  {"string", stringer, 0},
  {"word", cons, 4},
  {"zero", s_space, 0},
  /* Force set to be treated as an instruction.  */
  {"set", NULL, 0},
  {0}
};

void
md_begin ()
{
  const char *retval = NULL;
  unsigned int i = 0;

  /* initialize hash table */

  op_hash = hash_new ();
  if (op_hash == NULL)
    as_fatal ("Could not initialize hash table");

  /* loop until you see the end of the list */

  while (*m88k_opcodes[i].name)
    {
      char *name = m88k_opcodes[i].name;

      /* hash each mnemonic and record its position */

      retval = hash_insert (op_hash, name, &m88k_opcodes[i]);

      if (retval != NULL)
	as_fatal ("Can't hash instruction '%s':%s",
		  m88k_opcodes[i].name, retval);

      /* skip to next unique mnemonic or end of list */

      for (i++; !strcmp (m88k_opcodes[i].name, name); i++)
	;
    }
}

int
md_parse_option (argP, cntP, vecP)
     char **argP;
     int *cntP;
     char ***vecP;
{
  return 0;
}

void
md_assemble (op)
     char *op;
{
  char *param, *thisfrag;
  char c;
  struct m88k_opcode *format;
  struct m88k_insn insn;

  assert (op);

  /* skip over instruction to find parameters */

  for (param = op; *param != 0 && !isspace (*param); param++)
    ;
  c = *param;
  *param++ = '\0';

  /* try to find the instruction in the hash table */

  if ((format = (struct m88k_opcode *) hash_find (op_hash, op)) == NULL)
    {
      as_bad ("Invalid mnemonic '%s'", op);
      return;
    }

  /* try parsing this instruction into insn */

  insn.exp.X_add_symbol = 0;
  insn.exp.X_op_symbol = 0;
  insn.exp.X_add_number = 0;
  insn.exp.X_op = O_illegal;
  insn.reloc = NO_RELOC;

  while (!calcop (format, param, &insn))
    {
      /* if it doesn't parse try the next instruction */

      if (!strcmp (format[0].name, format[1].name))
	format++;
      else
	{
	  as_fatal ("Parameter syntax error");
	  return;
	}
    }

  /* grow the current frag and plop in the opcode */

  thisfrag = frag_more (4);
  md_number_to_chars (thisfrag, insn.opcode, 4);

  /* if this instruction requires labels mark it for later */

  switch (insn.reloc)
    {
    case NO_RELOC:
      break;

    case RELOC_LO16:
    case RELOC_HI16:
      fix_new_exp (frag_now,
		   thisfrag - frag_now->fr_literal + 2,
		   2,
		   &insn.exp,
		   0,
		   insn.reloc);
      break;

    case RELOC_IW16:
      fix_new_exp (frag_now,
		   thisfrag - frag_now->fr_literal,
		   4,
		   &insn.exp,
		   0,
		   insn.reloc);
      break;

    case RELOC_PC16:
      fix_new_exp (frag_now,
		   thisfrag - frag_now->fr_literal + 2,
		   2,
		   &insn.exp,
		   1,
		   insn.reloc);
      break;

    case RELOC_PC26:
      fix_new_exp (frag_now,
		   thisfrag - frag_now->fr_literal,
		   4,
		   &insn.exp,
		   1,
		   insn.reloc);
      break;

    default:
      as_fatal ("Unknown relocation type");
      break;
    }
}

static int
calcop (format, param, insn)
     struct m88k_opcode *format;
     char *param;
     struct m88k_insn *insn;
{
  char *fmt = format->op_spec;
  int f;
  unsigned val;
  unsigned opcode;
  int reg_prefix = 'r';

  insn->opcode = format->opcode;
  opcode = 0;

  for (;;)
    {
      if (param == 0)
	return 0;
      f = *fmt++;
      switch (f)
	{
	case 0:
	  insn->opcode |= opcode;
	  return (*param == 0 || *param == '\n');

	default:
	  if (f != *param++)
	    return 0;
	  break;

	case 'd':
	  param = get_reg (param, &val, reg_prefix);
	  reg_prefix = 'r';
	  opcode |= val << 21;
	  break;

	case 'o':
	  param = get_o6 (param, &val);
	  opcode |= ((val >> 2) << 7);
	  break;

	case 'x':
	  reg_prefix = 'x';
	  break;

	case '1':
	  param = get_reg (param, &val, reg_prefix);
	  reg_prefix = 'r';
	  opcode |= val << 16;
	  break;

	case '2':
	  param = get_reg (param, &val, reg_prefix);
	  reg_prefix = 'r';
	  opcode |= val;
	  break;

	case '3':
	  param = get_reg (param, &val, 'r');
	  opcode |= (val << 16) | val;
	  break;

	case 'I':
	  param = get_imm16 (param, insn);
	  break;

	case 'b':
	  param = get_bf (param, &val);
	  opcode |= val;
	  break;

	case 'p':
	  param = get_pcr (param, insn, RELOC_PC16);
	  break;

	case 'P':
	  param = get_pcr (param, insn, RELOC_PC26);
	  break;

	case 'B':
	  param = get_cmp (param, &val);
	  opcode |= val;
	  break;

	case 'M':
	  param = get_cnd (param, &val);
	  opcode |= val;
	  break;

	case 'c':
	  param = get_cr (param, &val);
	  opcode |= val << 5;
	  break;

	case 'f':
	  param = get_fcr (param, &val);
	  opcode |= val << 5;
	  break;

	case 'V':
	  param = get_vec9 (param, &val);
	  opcode |= val;
	  break;

	case '?':
	  /* Having this here repeats the warning somtimes.
	   But can't we stand that?  */
	  as_warn ("Use of obsolete instruction");
	  break;
	}
    }
}

static char *
match_name (param, assoc_tab, valp)
     char *param;
     struct field_val_assoc *assoc_tab;
     unsigned *valp;
{
  int i;
  char *name;
  int name_len;

  for (i = 0;; i++)
    {
      name = assoc_tab[i].name;
      if (name == NULL)
	return NULL;
      name_len = strlen (name);
      if (!strncmp (param, name, name_len))
	{
	  *valp = assoc_tab[i].val;
	  return param + name_len;
	}
    }
}

static char *
get_reg (param, regnop, reg_prefix)
     char *param;
     unsigned *regnop;
     int reg_prefix;
{
  unsigned c;
  unsigned regno;

  c = *param++;
  if (c == reg_prefix)
    {
      regno = *param++ - '0';
      if (regno < 10)
	{
	  if (regno == 0)
	    {
	      *regnop = 0;
	      return param;
	    }
	  c = *param - '0';
	  if (c < 10)
	    {
	      regno = regno * 10 + c;
	      if (c < 32)
		{
		  *regnop = regno;
		  return param + 1;
		}
	    }
	  else
	    {
	      *regnop = regno;
	      return param;
	    }
	}
      return NULL;
    }
  else if (c == 's' && param[0] == 'p')
    {
      *regnop = 31;
      return param + 1;
    }

  return 0;
}

static char *
get_imm16 (param, insn)
     char *param;
     struct m88k_insn *insn;
{
  enum reloc_type reloc = NO_RELOC;
  unsigned int val;
  char *save_ptr;

  if (!strncmp (param, "hi16", 4) && !isalnum (param[4]))
    {
      reloc = RELOC_HI16;
      param += 4;
    }
  else if (!strncmp (param, "lo16", 4) && !isalnum (param[4]))
    {
      reloc = RELOC_LO16;
      param += 4;
    }
  else if (!strncmp (param, "iw16", 4) && !isalnum (param[4]))
    {
      reloc = RELOC_IW16;
      param += 4;
    }

  save_ptr = input_line_pointer;
  input_line_pointer = param;
  expression (&insn->exp);
  param = input_line_pointer;
  input_line_pointer = save_ptr;

  val = insn->exp.X_add_number;

  if (insn->exp.X_op == O_constant)
    {
      /* Insert the value now, and reset reloc to NO_RELOC.  */
      if (reloc == NO_RELOC)
	{
	  /* Warn about too big expressions if not surrounded by xx16.  */
	  if (val > 0xffff)
	    as_warn ("Expression truncated to 16 bits");
	}

      if (reloc == RELOC_HI16)
	val >>= 16;

      insn->opcode |= val & 0xffff;
      reloc = NO_RELOC;
    }
  else if (reloc == NO_RELOC)
    /* We accept a symbol even without lo16, hi16, etc, and assume
       lo16 was intended.  */
    reloc = RELOC_LO16;

  insn->reloc = reloc;

  return param;
}

static char *
get_pcr (param, insn, reloc)
     char *param;
     struct m88k_insn *insn;
     enum reloc_type reloc;
{
  char *saveptr, *saveparam;

  saveptr = input_line_pointer;
  input_line_pointer = param;

  expression (&insn->exp);

  saveparam = input_line_pointer;
  input_line_pointer = saveptr;

  /* Botch: We should relocate now if O_constant.  */
  insn->reloc = reloc;

  return saveparam;
}

static char *
get_cmp (param, valp)
     char *param;
     unsigned *valp;
{
  unsigned int val;
  char *save_ptr;

  save_ptr = param;

  param = match_name (param, cmpslot, valp);
  val = *valp;

  if (param == NULL)
    {
      param = save_ptr;

      save_ptr = input_line_pointer;
      input_line_pointer = param;
      val = get_absolute_expression ();
      param = input_line_pointer;
      input_line_pointer = save_ptr;

      if (val >= 32)
	{
	  as_warn ("Expression truncated to 5 bits");
	  val %= 32;
	}
    }

  *valp = val << 21;
  return param;
}

static char *
get_cnd (param, valp)
     char *param;
     unsigned *valp;
{
  unsigned int val;

  if (isdigit (*param))
    {
      param = getval (param, &val);

      if (val >= 32)
	{
	  as_warn ("Expression truncated to 5 bits");
	  val %= 32;
	}
    }
  else
    {
      if (isupper (*param))
	*param = tolower (*param);

      if (isupper (param[1]))
	param[1] = tolower (param[1]);

      param = match_name (param, cndmsk, valp);

      if (param == NULL)
	return NULL;

      val = *valp;
    }

  *valp = val << 21;
  return param;
}

static char *
get_bf2 (param, bc)
     char *param;
     int bc;
{
  int depth = 0;
  int c;

  for (;;)
    {
      c = *param;
      if (c == 0)
	return param;
      else if (c == '(')
	depth++;
      else if (c == ')')
	depth--;
      else if (c == bc && depth <= 0)
	return param;
      param++;
    }
}

static char *
get_bf_offset_expression (param, offsetp)
     char *param;
     unsigned *offsetp;
{
  unsigned offset;

  if (isalpha (param[0]))
    {
      if (isupper (param[0]))
	param[0] = tolower (param[0]);
      if (isupper (param[1]))
	param[1] = tolower (param[1]);

      param = match_name (param, cmpslot, offsetp);

      return param;
    }
  else
    {
      input_line_pointer = param;
      offset = get_absolute_expression ();
      param = input_line_pointer;
    }

  *offsetp = offset;
  return param;
}

static char *
get_bf (param, valp)
     char *param;
     unsigned *valp;
{
  unsigned offset = 0;
  unsigned width = 0;
  char *xp;
  char *save_ptr;

  xp = get_bf2 (param, '<');

  save_ptr = input_line_pointer;
  input_line_pointer = param;
  if (*xp == 0)
    {
      /* We did not find '<'.  We have an offset (width implicitly 32).  */
      param = get_bf_offset_expression (param, &offset);
      input_line_pointer = save_ptr;
      if (param == NULL)
	return NULL;
    }
  else
    {
      *xp++ = 0;		/* Overwrite the '<' */
      param = get_bf2 (xp, '>');
      if (*param == 0)
	return NULL;
      *param++ = 0;		/* Overwrite the '>' */

      width = get_absolute_expression ();
      xp = get_bf_offset_expression (xp, &offset);
      input_line_pointer = save_ptr;

      if (xp + 1 != param)
	return NULL;
    }

  *valp = ((width % 32) << 5) | (offset % 32);

  return param;
}

static char *
get_cr (param, regnop)
     char *param;
     unsigned *regnop;
{
  unsigned regno;
  unsigned c;

  if (!strncmp (param, "cr", 2))
    {
      param += 2;

      regno = *param++ - '0';
      if (regno < 10)
	{
	  if (regno == 0)
	    {
	      *regnop = 0;
	      return param;
	    }
	  c = *param - '0';
	  if (c < 10)
	    {
	      regno = regno * 10 + c;
	      if (c < 64)
		{
		  *regnop = regno;
		  return param + 1;
		}
	    }
	  else
	    {
	      *regnop = regno;
	      return param;
	    }
	}
      return NULL;
    }

  param = match_name (param, cr_regs, regnop);

  return param;
}

static char *
get_fcr (param, regnop)
     char *param;
     unsigned *regnop;
{
  unsigned regno;
  unsigned c;

  if (!strncmp (param, "fcr", 3))
    {
      param += 3;

      regno = *param++ - '0';
      if (regno < 10)
	{
	  if (regno == 0)
	    {
	      *regnop = 0;
	      return param;
	    }
	  c = *param - '0';
	  if (c < 10)
	    {
	      regno = regno * 10 + c;
	      if (c < 64)
		{
		  *regnop = regno;
		  return param + 1;
		}
	    }
	  else
	    {
	      *regnop = regno;
	      return param;
	    }
	}
      return NULL;
    }

  param = match_name (param, fcr_regs, regnop);

  return param;
}

static char *
get_vec9 (param, valp)
     char *param;
     unsigned *valp;
{
  unsigned val;
  char *save_ptr;

  save_ptr = input_line_pointer;
  input_line_pointer = param;
  val = get_absolute_expression ();
  param = input_line_pointer;
  input_line_pointer = save_ptr;

  if (val >= 1 << 9)
    as_warn ("Expression truncated to 9 bits");

  *valp = val % (1 << 9);

  return param;
}

static char *
get_o6 (param, valp)
     char *param;
     unsigned *valp;
{
  unsigned val;
  char *save_ptr;

  save_ptr = input_line_pointer;
  input_line_pointer = param;
  val = get_absolute_expression ();
  param = input_line_pointer;
  input_line_pointer = save_ptr;

  if (val & 0x3)
    as_warn ("Removed lower 2 bits of expression");

  *valp = val;

  return(param);
}

#define hexval(z) \
  (isdigit (z) ? (z) - '0' :						\
   islower (z) ? (z) - 'a' + 10 : 					\
   isupper (z) ? (z) - 'A' + 10 : -1)

static char *
getval (param, valp)
     char *param;
     unsigned int *valp;
{
  unsigned int val = 0;
  unsigned int c;

  c = *param++;
  if (c == '0')
    {
      c = *param++;
      if (c == 'x' || c == 'X')
	{
	  c = *param++;
	  c = hexval (c);
	  while (c < 16)
	    {
	      val = val * 16 + c;
	      c = *param++;
	      c = hexval (c);
	    }
	}
      else
	{
	  c -= '0';
	  while (c < 8)
	    {
	      val = val * 8 + c;
	      c = *param++ - '0';
	    }
	}
    }
  else
    {
      c -= '0';
      while (c < 10)
	{
	  val = val * 10 + c;
	  c = *param++ - '0';
	}
    }

  *valp = val;
  return param - 1;
}

void
md_number_to_chars (buf, val, nbytes)
     char *buf;
     valueT val;
     int nbytes;
{
  switch (nbytes)
    {
    case 4:
      *buf++ = val >> 24;
      *buf++ = val >> 16;
    case 2:
      *buf++ = val >> 8;
    case 1:
      *buf = val;
      break;

    default:
      abort ();
    }
}

#if 0

/* This routine is never called.  What is it for?
   Ian Taylor, Cygnus Support 13 Jul 1993 */

void
md_number_to_imm (buf, val, nbytes, fixP, seg_type)
     unsigned char *buf;
     unsigned int val;
     int nbytes;
     fixS *fixP;
     int seg_type;
{
  if (seg_type != N_TEXT || fixP->fx_r_type == NO_RELOC)
    {
      switch (nbytes)
	{
	case 4:
	  *buf++ = val >> 24;
	  *buf++ = val >> 16;
	case 2:
	  *buf++ = val >> 8;
	case 1:
	  *buf = val;
	  break;

	default:
	  abort ();
	}
      return;
    }

  switch (fixP->fx_r_type)
    {
    case RELOC_IW16:
      buf[2] = val >> 8;
      buf[3] = val;
      break;

    case RELOC_LO16:
      buf[0] = val >> 8;
      buf[1] = val;
      break;

    case RELOC_HI16:
      buf[0] = val >> 24;
      buf[1] = val >> 16;
      break;

    case RELOC_PC16:
      val += 4;
      buf[0] = val >> 10;
      buf[1] = val >> 2;
      break;

    case RELOC_PC26:
      val += 4;
      buf[0] |= (val >> 26) & 0x03;
      buf[1] = val >> 18;
      buf[2] = val >> 10;
      buf[3] = val >> 2;
      break;

    case RELOC_32:
      buf[0] = val >> 24;
      buf[1] = val >> 16;
      buf[2] = val >> 8;
      buf[3] = val;
      break;

    default:
      as_fatal ("Bad relocation type");
      break;
    }
}

#endif /* 0 */

void
md_number_to_disp (buf, val, nbytes)
     char *buf;
     int val;
     int nbytes;
{
  as_fatal ("md_number_to_disp not defined");
  md_number_to_chars (buf, val, nbytes);
}

void
md_number_to_field (buf, val, nbytes)
     char *buf;
     int val;
     int nbytes;
{
  as_fatal ("md_number_to_field not defined");
  md_number_to_chars (buf, val, nbytes);
}

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

int md_short_jump_size = 4;

void
md_create_short_jump (ptr, from_addr, to_addr, frag, to_symbol)
     char *ptr;
     addressT from_addr, to_addr;
     fragS *frag;
     symbolS *to_symbol;
{
  ptr[0] = (char) 0xc0;
  ptr[1] = 0x00;
  ptr[2] = 0x00;
  ptr[3] = 0x00;
  fix_new (frag,
	   ptr - frag->fr_literal,
	   4,
	   to_symbol,
	   (offsetT) 0,
	   0,
	   RELOC_PC26);		/* Botch: Shouldn't this be RELOC_PC16? */
}

int md_long_jump_size = 4;

void
md_create_long_jump (ptr, from_addr, to_addr, frag, to_symbol)
     char *ptr;
     addressT from_addr, to_addr;
     fragS *frag;
     symbolS *to_symbol;
{
  ptr[0] = (char) 0xc0;
  ptr[1] = 0x00;
  ptr[2] = 0x00;
  ptr[3] = 0x00;
  fix_new (frag,
	   ptr - frag->fr_literal,
	   4,
	   to_symbol,
	   (offsetT) 0,
	   0,
	   RELOC_PC26);
}

int
md_estimate_size_before_relax (fragP, segment_type)
     fragS *fragP;
     segT segment_type;
{
  as_fatal ("Relaxation should never occur");
  return (-1);
}

const relax_typeS md_relax_table[] =
{0};

void
md_end ()
{
}

#if 0

/* As far as I can tell, this routine is never called.  What is it
   doing here?
   Ian Taylor, Cygnus Support 13 Jul 1993 */


/*
 * Risc relocations are completely different, so it needs
 * this machine dependent routine to emit them.
 */
void
emit_relocations (fixP, segment_address_in_file)
     fixS *fixP;
     relax_addressT segment_address_in_file;
{
  struct reloc_info_m88k ri;
  symbolS *symbolP;
  extern char *next_object_file_charP;

  bzero ((char *) &ri, sizeof (ri));
  for (; fixP; fixP = fixP->fx_next)
    {
      if (fixP->fx_r_type >= NO_RELOC)
	{
	  fprintf (stderr, "fixP->fx_r_type = %d\n", fixP->fx_r_type);
	  abort ();
	}

      if ((symbolP = fixP->fx_addsy) != NULL)
	{
	  ri.r_address = fixP->fx_frag->fr_address +
	    fixP->fx_where - segment_address_in_file;
	  if ((symbolP->sy_type & N_TYPE) == N_UNDF)
	    {
	      ri.r_extern = 1;
	      ri.r_symbolnum = symbolP->sy_number;
	    }
	  else
	    {
	      ri.r_extern = 0;
	      ri.r_symbolnum = symbolP->sy_type & N_TYPE;
	    }
	  if (symbolP && symbolP->sy_frag)
	    {
	      ri.r_addend = symbolP->sy_frag->fr_address;
	    }
	  ri.r_type = fixP->fx_r_type;
	  if (fixP->fx_pcrel)
	    {
	      ri.r_addend -= ri.r_address;
	    }
	  else
	    {
	      ri.r_addend = fixP->fx_addnumber;
	    }

	  append (&next_object_file_charP, (char *) &ri, sizeof (ri));
	}
    }
  return;
}

#endif /* 0 */

#if 0

/* This routine can be subsumed by s_lcomm in read.c.
   Ian Taylor, Cygnus Support 13 Jul 1993 */


static void
s_bss ()
{
  char *name;
  char c;
  char *p;
  int temp, bss_align;
  symbolS *symbolP;

  name = input_line_pointer;
  c = get_symbol_end ();
  p = input_line_pointer;
  *p = c;
  SKIP_WHITESPACE ();
  if (*input_line_pointer != ',')
    {
      as_warn ("Expected comma after name");
      ignore_rest_of_line ();
      return;
    }
  input_line_pointer++;
  if ((temp = get_absolute_expression ()) < 0)
    {
      as_warn ("BSS length (%d.) <0! Ignored.", temp);
      ignore_rest_of_line ();
      return;
    }
  *p = 0;
  symbolP = symbol_find_or_make (name);
  *p = c;
  if (*input_line_pointer == ',')
    {
      input_line_pointer++;
      bss_align = get_absolute_expression ();
    }
  else
    bss_align = 0;

  if (!S_IS_DEFINED(symbolP)
      || S_GET_SEGMENT(symbolP) == SEG_BSS)
    {
      if (! need_pass_2)
	{
	  char *p;
	  segT current_seg = now_seg;
	  subsegT current_subseg = now_subseg;

	  subseg_set (SEG_BSS, 1); /* switch to bss	*/

	  if (bss_align)
	    frag_align (bss_align, 0);

	  /* detach from old frag */
	  if (symbolP->sy_type == N_BSS && symbolP->sy_frag != NULL)
	    symbolP->sy_frag->fr_symbol = NULL;

	  symbolP->sy_frag  = frag_now;
	  p = frag_var (rs_org, 1, 1, (relax_substateT)0, symbolP,
			temp, (char *)0);
	  *p = 0;
	  S_SET_SEGMENT (symbolP, SEG_BSS);

	  subseg_set (current_seg, current_subseg);
	}
    }
  else
    {
      as_warn ("Ignoring attempt to re-define symbol %s.", name);
    }

  while (!is_end_of_line[*input_line_pointer])
    {
      input_line_pointer++;
    }

  return;
}

#endif /* 0 */

#ifdef M88KCOFF

/* These functions are needed if we are linking with obj-coffbfd.c.
   That file may be replaced by a more BFD oriented version at some
   point.  If that happens, these functions should be rexamined.

   Ian Lance Taylor, Cygnus Support, 13 July 1993.  */

/* Given a fixS structure (created by a call to fix_new, above),
   return the BFD relocation type to use for it.  */

short
tc_coff_fix2rtype (fixp)
     fixS *fixp;
{
  switch (fixp->fx_r_type)
    {
    case RELOC_LO16:
      return R_LVRT16;
    case RELOC_HI16:
      return R_HVRT16;
    case RELOC_PC16:
      return R_PCR16L;
    case RELOC_PC26:
      return R_PCR26L;
    case RELOC_32:
      return R_VRT32;
    case RELOC_IW16:
      return R_VRT16;
    default:
      abort ();
    }
}

/* Apply a fixS to the object file.  Since COFF does not use addends
   in relocs, the addend is actually stored directly in the object
   file itself.  */

void
md_apply_fix (fixp, val)
     fixS *fixp;
     long val;
{
  char *buf;

  buf = fixp->fx_frag->fr_literal + fixp->fx_where;
  fixp->fx_offset = 0;

  switch (fixp->fx_r_type)
    {
    case RELOC_IW16:
      fixp->fx_offset = val >> 16;
      buf[2] = val >> 8;
      buf[3] = val;
      break;

    case RELOC_LO16:
      fixp->fx_offset = val >> 16;
      buf[0] = val >> 8;
      buf[1] = val;
      break;

    case RELOC_HI16:
      fixp->fx_offset = val >> 16;
      buf[0] = val >> 8;
      buf[1] = val;
      break;

    case RELOC_PC16:
      buf[0] = val >> 10;
      buf[1] = val >> 2;
      break;

    case RELOC_PC26:
      buf[0] |= (val >> 26) & 0x03;
      buf[1] = val >> 18;
      buf[2] = val >> 10;
      buf[3] = val >> 2;
      break;

    case RELOC_32:
      buf[0] = val >> 24;
      buf[1] = val >> 16;
      buf[2] = val >> 8;
      buf[3] = val;
      break;

    default:
      abort ();
    }
}

/* Where a PC relative offset is calculated from.  On the m88k they
   are calculated from just after the instruction.  */

long
md_pcrel_from (fixp)
     fixS *fixp;
{
  switch (fixp->fx_r_type)
    {
    case RELOC_PC16:
      return fixp->fx_frag->fr_address + fixp->fx_where - 2;
    case RELOC_PC26:
      return fixp->fx_frag->fr_address + fixp->fx_where;
    default:
      abort ();
    }
  /*NOTREACHED*/
}

#endif /* M88KCOFF */
