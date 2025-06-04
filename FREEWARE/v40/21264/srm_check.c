
/*
 *  VMS BUILD INSTRUCTIONS:
 *
 *		$ cc srm_check + sys$library:sys$lib_c/lib
 *		$ link srm_check
 *
 */

#ifndef MODULE_NAME
#define MODULE_NAME SRM_CHECK
#endif

#ifndef IDENT
#define IDENT "X-7"
#endif

#pragma module MODULE_NAME IDENT

/*
 *   Copyright ©1998 by Digital Equipment Corporation, Maynard, Massachusetts
 *
 *                        All Rights Reserved.
 *
 *  Permission to use, copy, modify, and distribute this software and its
 *  documentation for any purpose and without fee is hereby granted,
 *  provided that the above copyright notice appear in all copies and that
 *  both that copyright notice and this permission notice appear in
 *  supporting documentation, and that the name of Digital not be
 *  used in advertising or publicity pertaining to distribution of the
 *  software without specific, written prior permission.
 *
 *  DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 *  ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 *  DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 *  ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 *  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 *  ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 *  SOFTWARE.
 *
 */

/*
 *++
 *
 *  MODULE:
 *
 *	SRM_CHECK
 *
 *  ABSTRACT:
 *
 *	This module was originally written as a simple application to
 *	display the machine code output from a VMS executable image.
 *	It was substantially changed to do checking for ldx_l/stx_c lock
 *	sequences that violate the Alpha architecture standard.  It
 *	still can display the machine code output, as it was originally
 *	intended to do ;-)
 *	
 *	The default logic is now to analyze the executable and make
 *	sure that ldx_l/stx_c sequences are valid as per the Alpha
 *	Architecture Reference Manual (Alpha SRM), Third Edition,
 *	Order #EY-W938E-DP.  In addition, comments from Rick Kessler
 *	specific to the 21264 chip implementation were used as a guide.
 *	
 *	The brief version of how a ldx_l/stx_c sequence should be formed
 *	is that the stx_c must follow the ldx_l in program order, and
 *	be fallen through to without branching (that is, no "taken"
 *	branches can exist between the ldx_x and stx_c.  In addition,
 *	any other instruction between the ldx_l/stx_c cannot access
 *	memory, or cause a fault (in general, only "operate" instructions
 *	that act on registers).
 *	
 *	A typical good sequence looks like:
 *	
 *	X:	ldx_l
 *		Conditional Branch Y
 *		Modify data from the ldx_l
 *		stx_c
 *		Conditional branch on failure to Z
 *	Y:	Arbitrary code
 *		...
 *
 *	Z:	check retry count
 *		branch X
 *
 *	Note that the conditional branch to Y is taken if the STx_C
 *	does not need to be done, and the branch on a failure of
 *	the STx_C is forward, to code that then branches back to
 *	retry.  A full, and clear explanation of this can be found
 *	in 5.5 (Data Sharing) in the Alpha SRM.
 *
 *	The Alpha 21264 (EV6) architecture is less forgiving than
 *	earlier implementations on mal-formed sequences.  In particular,
 *	4.2.4 (Load Memory Data into Integer Register Locked) spells
 *	out the requirements for sequences, and there are two specific
 *	items that we are looking for:
 *	
 *	 "If any other memory access (ECB, LDx, LDQ_U, STx, 
 *	  STQ_U, WH64) is executed on the given processor
 *        between the LDx_L and the STx_C, the sequence above
 *	  may always fail on some implementations; hence, no
 *	  useful program should do this."
 *	
 *	 "If a branch is taken between the LDx_L and the STx_C,
 *	  the above sequence may always fail on some implementations;
 *	  hence, no useful program should do this.  (CMOVxx may
 *	  be used to avoid branching.)"
 *	
 *	The code general outline of the code is to open a file,
 *	map it into memory, locate the executable portion of the
 *	data, and then pass through a disassembler.  When the
 *	ldx_l or stx_c operations are detected, they are checked
 *	for order, and detected sequences are stored in a list
 *	for additional analysis.  The Alpha SRM also recommends
 *	no more than 40 instructions between a ldx_l/stx_c to
 *	prevent hardware interrupts (like the time source) from
 *	never allowing the sequence to complete - so unterminated
 *	sequences are forced to be terminated after 40 instructions.
 *	
 *	After this check, the code is disassembled a second time
 *	and branch instructions are checked for a displacement
 *	that would take it into any of the detected sequences.
 *	
 *	The main types of problems that have ever been seen in
 *	generated code are:
 *	
 *	1) Rotated loop optimization.  This is a problem seen
 *	   in some ASM (C/C++) code where the sequence is:
 *	
 *	   x:	modify_data
 *		stx_c
 *		cbr	y
 *		ldx_l
 *		cbr	x
 *	   y:	...
 *	
 *	  (cbr = Conditional Branch).
 *	
 *	2) Memory loads in the sequence.  This is a problem
 *	   seen in some MACRO-32 code where the programmer
 *	   tried to hand-code a atomic update:
 *	
 *	   x:	ldx_l
 *		ldx
 *		cbr	y
 *		modify_data
 *		stx_c
 *		cbr	z
 *	   y:	...
 *	   z:
 *	
 *	   An example of this was an attempt to decrement
 *	   the value by doing a subtract of -1 from it.
 *	   The -1 cannot be represented as a literal in
 *	   the opcode, so the compiler placed -1 into a
 *	   data location and fetched it.
 *	
 *	3) Both 1 and 2.  This has been seen when a register
 *	   "spill" occured in MACRO-32, in a BBCCI or BBSSI.
 *	   A register spill occurs when the compiler runs
 *	   out of scratch registers, and is a highly rare
 *	   occurance.  It means a number of highly complex
 *	   MACRO instructions were done in a code block,
 *	   one of which was a complex interlocked instruction.
 *	
 *	The code looks for other non-SRM compliant sequences
 *	that have a low probability of occurance, and that
 *	would not be issued by a normal compiler withtout
 *	the programmer writing a deliberately bad sequence
 *	using ASM, MACRO-32, or MACRO-64.
 *	
 *  AUTHOR:
 *
 *     Fred Kleinsorge
 *
 *  CREATION DATE:     November 1998
 *
 *  MODIFICATION HISTORY:
 *
 *	X-0.1	Check for unpaired ld_x / stx_c -- that is, two
 *		load locked instructions without a store conditional,
 *		or a store conditional without a load locked.  Both
 *		signify a unmatched pair.
 *
 *	X-1	Add check for branches.  Additional code cleanup and
 *		checking.
 *
 *	X-2	- Bad sign extension for negative displacement in decode_br
 *		- Over aggressive check for "non-code" in disassemble
 *
 *	X-3	- Add wildcard scanning using lib$file_scan
 *		- Make the default to create a new output file, and
 *		  change -noappend to -append to allow the user to
 *		  cause output to be appended.
 *
 *	X-4	- Fix process_error to print the expanded name, and signal
 *		  the error.
 *		- Move code around to make the code as non-VMS-specific as
 *		  possible.
 *		- Add decode for floating point (well, some of them).
 *		- Break up function name array into per-opcode arrays to
 *		  speed up lookup, and reduce the size.
 *
 *	X-5	- Check for a VBN of zero, and ignore those sections.
 *
 *	X-6	- Refix X-2 - displacement still bad
 *		- Be less aggressive, and add -aggressive to allow anal retention.
 *		- Do backward branch checks during first pass, looking for
 *		  branches backwards to stx_c sequences.  This more clearly
 *		  shows the user the actual problem, rather than an artifact
 *		  of finding other crap in the sequence.
 *
 *		  As of this edit, we actually find the mal-formed sequences
 *		  *and* properly identify them with a lot of clarity, while
 *		  not finding as many non-errors in VMS images.
 *
 *	X-7	- Typo's and error message content changes to be consistant
 *		- Make sure that the initial contents of the context block
 *		  are zero!
 *		- Print the "Potential Alpha Arhitecture... message once"
 *		  on the first detected error.
 *		- Add braces in print statement in decode_mbr(), found by
 *		  Craig Neth's porting to UNIX.
 *		- Dang.  One-line to print violation for each new file when
 *		  found
 *
 *--
 */

#include <ints.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#ifdef VMS
#include <descrip.h>
#include <eihidef.h>
#include <eihddef.h>
#include <eisddef.h>
#include <lib$routines.h>
#include <rms.h>
#include <secdef.h>
#include <ssdef.h>
#include <starlet.h>
#include <syidef.h>
#include <vadef.h>
#include <va_rangedef.h>
#endif

/*
 *  The SRM says that the number of instructions must be
 *  short enough between a LDx_L/STx_C that it can execute
 *  within the minimum HW interrupt time, or it can always
 *  fail because of an interrupt.  They suggest 40 instructions
 *  as a outside number.  See 4.2.4, page 4-11, 3rd Edition.
 *
 */
#define MAX_INSTRUCTIONS 40

/*
 *  If we find a "naked" STxC (that is, a STx_C without a
 *  preceding LDx_L) we create a SEQ record.  This defines
 *  how much code to include.  By default we will include
 *  2 instructions before, and 1 after (4 instructions).
 *  this symbol is used to subtract from the STx_C address,
 *  and the value * 2 is used as the size of the sequence.
 *
 */
#ifndef NAKED_STC_SEQ_SIZE
#define NAKED_STC_SEQ_SIZE 2
#endif

/*
 *  ctx.command flags,  CHECK_LDL overrides DUMP
 *
 */
#define CHECK_LDL           0x001
#define FOLLOW_BRANCHES     0x002
#define DUMP                0x004
#define VERBOSE             0x008
#define LOG_FILES           0x010
#define DUMP_ALL_SEQUENCES  0x020
#define CHECK_BRANCHES      0x040
#define VERSION_PRINT       0x080
#define AGGRESSIVE          0x100

/*
 *  ctx.flags bits
 *
 */
#define NOT_CODE            0x01
#define BAD_LOCK_SEQ        0x02
#define NAKED_STX_C         0x04

/*
 *  ctx.dflags bits
 *
 */
#define DISPLAYED_ERROR     0x01

/*
 *  Status codes.  Note that at least one of the
 *  opcode types is not fully decoded.
 *
 */
#define ERROR_STATUS        0
#define SUCCESS             1
#define NOTFINISHED         2
#define FOUND_PROBLEM      -1
#define VA_NOT_FOUND        0

/*
 *  Codes for usage error display
 *
 */
#define ERROR_USAGE          0
#define ERROR_NOINPUT       -1
#define ERROR_NOOUTPUTFILE  -2
#define ERROR_NOMAXERR      -3
#define ERROR_STARTVA       -4
#define ERROR_ENDVA         -5
#define ERROR_ENDTOOSMALL   -6
#define ERROR_SEQCNT        -7


/*
 *  The default number of errors before stopping printing
 *
 */
#ifndef MAX_ERRORS
#define MAX_ERRORS	100
#endif

#ifndef MAX_NAME_SIZE
#define MAX_NAME_SIZE   255
#endif

#ifndef MAX_SEQUENCE
#define MAX_SEQUENCE   2000
#endif

typedef struct seq {
    uint32	*start;			/* Starting real VA of last LDL */
    uint32	*va;			/* Starting display VA for above */
    uint32	size;			/* Length in bytes of the sequence */
    uint32	flags;			/* */
} SEQ;

/*
 *  A structure of this type is passed around to all the routines
 *  and contains the current context for processing.
 *
 *  Note that the lib$file_scan VMS routine passes a FAB as the
 *  only argument to the routine.  Rather than using a FAB cell
 *  to contain a pointer to the context, we place the FAB at
 *  the top of the structure.  It must always come first.
 *
 */
typedef struct context {
#ifdef VMS
  struct FAB	sfab;			/* search FAB -- MUST BE FIRST FIELD! */
  struct NAM	snam;			/* search NAM */
  int		search_context;		/* context for wildcard search */
#endif
  uint32	command;		/* Command bits */
  int32		size;			/* Size in bytes to process */
  uint32	*pInput;		/* Pointer to the code to process */
  uint32	*va;			/* Displayed virtual address base */
  uint32	*start_va;		/* Starting VA - from user command */
  uint32	*end_va;		/* Ending VA   - ditto */
  FILE		*ofile;			/* Output FILE */
  uint32	ofile_arg;		/* Command line arg for ofile */
  FILE		*ifile;			/* Input FILE (unused */
  uint32	ifile_arg;		/* Command line arg # for ifile */
  uint32	*addr;			/* Current display VA in processing */
  uint32	*current_address;	/* Current actual VA in code */
  uint32	opcode;			/* Current OPCODE */
  uint32	current_context;	/* Last identified "interesting" item */
  uint32	flags;			/* Processing flags */
  uint32	dflags;			/* Display flags */
  uint32	*sequence_start;	/* Starting real VA of last LDL */
  uint32	*sequence_va;		/* Starting display VA for above */
  int32		sequence_size;		/* Length in bytes of the sequence */
  uint32	seq_avl;		/* Number available */
  uint32	seq_cnt;		/* Number used */
  SEQ           *seq_map;		/* Sequences */
  uint32	*error_address;		/* Actual VA of identified problem */
  int32		error;			/* Error code */
  int32		error_count;		/* Errors found so far */
  int32		max_errors;		/* Max errors to tolerate */
  int32		final_status;		/* Final status from disassemble */
  int		argc;			/* Input argument count */
  char		**argv;			/* Parsed UNIX command line */
  char		ispec[MAX_NAME_SIZE+1];	/* Input file spec */
  char		espec[MAX_NAME_SIZE+1];	/* Expanded input file spec (from RMS) */
  char		rspec[MAX_NAME_SIZE+1];	/* Result input file spec from file_scan */
} CONTEXT;

/*
 *  Routine directory
 *
 */
#ifdef VMS
static int   process_vms_file(CONTEXT *ctx);
static int   process_vms_file_error(CONTEXT *ctx);
#endif
static int32 process_image_section(CONTEXT *ctx, uint32 *base, uint32 *va, int32 size);
static void  print_help();
static int   parse_command_line(CONTEXT *ctx);
static void  usage(CONTEXT *ctx, int what);
static void  dump_branch_sequence(CONTEXT *ctx, int id);
static void  dump_sequence(CONTEXT *ctx);
static int32 disassemble_image(CONTEXT *ctx);
static void  decode_unknown(CONTEXT *ctx);
static void  decode_mem    (CONTEXT *ctx);
static void  decode_op     (CONTEXT *ctx);
static void  decode_pal    (CONTEXT *ctx);
static void  decode_br     (CONTEXT *ctx);
static void  decode_mfc    (CONTEXT *ctx);
static void  decode_fop    (CONTEXT *ctx);
static void  decode_mbr    (CONTEXT *ctx);

/*
 *  These are the 64 opcodes defined by the SRM
 *
 */
static char *opcode_names[64] = {
  "call_pal", "reserved", "reserved", "reserved", "reserved", "reserved", "reserved", "reserved", /* 00-07 */
  "lda",      "ldah",     "ldb_u",    "ldq_u",    "ldw_u",    "stw",      "stb",      "stq_u",    /* 08-0F */
  "inta",     "intl",     "ints",     "intm",     "itfp",     "fltv",     "flti",     "fltl",     /* 10-17 */
  "misc",     "*pal",     "jsr",      "*pal",     "fpti",     "*pal",     "*pal",     "*pal",     /* 18-1F */
  "ldf",      "ldg",      "lds",      "ldt",      "stf",      "stg",      "sts",      "stt",      /* 20-27 */
  "ldl",      "ldq",      "ldl_l",    "ldq_l",    "stl",      "stq",      "stl_c",    "stq_c",    /* 28-2F */
  "br",       "fbeq",     "fblt",     "fble",     "bsr",      "fbne",     "fbge",     "fbgt",     /* 30-37 */
  "blbc",     "beq",      "blt",      "ble",      "blbs",     "bne",      "bge",      "bgt"       /* 38-3F */
};

/*
 *  There are different types of opcodes,
 *  and the data os parsed differently depending
 *  on the opcode type.  The type in here is =
 *  looked up by the opcode (0-63) and the
 *  result is used to look up the routine to decode
 *  the instruction in decodedef.
 *
 */
static char optypes[64] = {
  3,0,0,0,0,0,0,0, /* 00-07 */
  1,1,1,1,1,1,1,1, /* 08-0F */
  2,2,2,2,6,6,6,6, /* 10-17 */
  5,0,7,0,0,0,0,0, /* 18-1F */
  1,1,1,1,1,1,1,1, /* 20-27 */
  1,1,1,1,1,1,1,1, /* 28-2F */
  4,4,4,4,4,4,4,4, /* 30-37 */
  4,4,4,4,4,4,4,4  /* 38-3F */
};

/*
 *  This is indexed by the optype from the array
 *  above, and finds the proper routine to decode
 *  a instruction
 *
 */
static struct decodedef {
  uint32 type;
  void (*routine)(CONTEXT *ctx);
  } decoder[8] = {
  {0, &decode_unknown},	/* Unimplemented or PAL */
  {1, &decode_mem},	/* Memory Format 3.3.1 */
  {2, &decode_op},	/* Operate Format 3.3.3 */
  {3, &decode_pal},	/* Call PAL 3.3.5 */
  {4, &decode_br},	/* Branch 3.3.2 */
  {5, &decode_mfc},	/* Memory Format with Function 3.3.1.1 */
  {6, &decode_fop},	/* Floating Point 3.3.4 */
  {7, &decode_mbr}	/* Memory Branch Format 3.3.1.2 */
};

/*
 *  Well, this is gross.  Some instructions are actually
 *  a "class" type opcode, and another field is the actual
 *  "function".  An array of pointers (one for each opcode)
 *  is used to lookup, and then search for the function
 *  ( a -1, -1 end it).  Even then, depending on the
 *  *specific* function, sometimes there is a format
 *  difference (which is what the "flags" is used for.
 *
 *  This list is not complete.
 *
 */
typedef struct decopfuncdef {
  uint32  function;
  uint32  flags;
  char *name;
  } DECOPFUNC;

static DECOPFUNC opc00[] = {
  {0x00a1, 0, "amovrm"},
  {0x00a0, 0, "amovrr"},
  {0x0080, 0, "bpt"},
  {0x0081, 0, "bugcheck"},
  {0x0083, 0, "chmk"},
  {0x0082, 0, "chme"},
  {0x0084, 0, "chms"},
  {0x0085, 0, "chmu"},
  {0x00aa, 0, "gentrap"},
  {0x0086, 0, "imb"},
  {0x0087, 0, "insqhil"},
  {0x00a2, 0, "insqhilr"},
  {0x0089, 0, "insqhiq"},
  {0x00a4, 0, "insqhiqr"},
  {0x0088, 0, "insqtil"},
  {0x00a3, 0, "insqtilr"},
  {0x008a, 0, "insqtiq"},
  {0x00a5, 0, "insqtiqr"},
  {0x008b, 0, "insquel"},
  {0x008d, 0, "insquel/d"},
  {0x008c, 0, "insqueq"},
  {0x008e, 0, "insqueq/d"},
  {0x008f, 0, "prober"},
  {0x0090, 0, "probew"},
  {0x0091, 0, "rd_ps"},
  {0x009e, 0, "read_unq"},
  {0x0092, 0, "rei"},
  {0x0093, 0, "remqhil"},
  {0x00a6, 0, "remqhilr"},
  {0x0095, 0, "remqhiq"},
  {0x00a8, 0, "remqhiqr"},
  {0x0094, 0, "remqtil"},
  {0x00a7, 0, "remqtilr"},
  {0x0096, 0, "remqtiq"},
  {0x00a9, 0, "remqtiqr"},
  {0x0097, 0, "remquel"},
  {0x0099, 0, "remquel/d"},
  {0x0098, 0, "remqueq"},
  {0x009a, 0, "remqueq/d"},
  {0x009d, 0, "rscc"},
  {0x009b, 0, "swasten"},
  {0x009f, 0, "write_unq"},
  {0x009c, 0, "wr_ps_sw"},
  {0x0001, 0, "cflush"},
  {0x0009, 0, "cserve"},
  {0x0002, 0, "draina"},
  {0x0000, 0, "halt"},
  {0x0003, 0, "ldqp"},
  {0x0006, 0, "mfpr_asn"},
  {0x001e, 0, "mfpr_esp"},
  {0x000b, 0, "mfpr_fen"},
  {0x000e, 0, "mfpr_ipl"},
  {0x0010, 0, "mfpr_mces"},
  {0x0012, 0, "mfpr_pcbb"},
  {0x0013, 0, "mfpr_prbr"},
  {0x0015, 0, "mfpr_ptbr"},
  {0x0016, 0, "mfpr_scbb"},
  {0x0019, 0, "mfpr_sisr"},
  {0x0020, 0, "mfpr_ssp"},
  {0x001a, 0, "mfpr_tbchk"},
  {0x0022, 0, "mfpr_usp"},
  {0x0029, 0, "mfpr_vptb"},
  {0x003f, 0, "mfpr_whami"},
  {0x0026, 0, "mtpr_asten"},
  {0x0027, 0, "mtpr_astsr"},
  {0x002e, 0, "mtpr_datfx"},
  {0x001f, 0, "mtpr_esp"},
  {0x000b, 0, "mtpr_fen"},
  {0x000d, 0, "mtpr_ipir"},
  {0x000f, 0, "mtpr_ipl"},
  {0x0011, 0, "mtpr_mces"},
  {0x002b, 0, "mtpr_perfmon"},
  {0x0014, 0, "mtpr_prbr"},
  {0x0017, 0, "mtpr_scbb"},
  {0x0018, 0, "mtpr_sirr"},
  {0x0021, 0, "mtpr_ssp"},
  {0x001b, 0, "mtpr_tbia"},
  {0x001c, 0, "mtpr_tbiap"},
  {0x001d, 0, "mtpr_tbis"},
  {0x0024, 0, "mtpr_tbisd"},
  {0x0025, 0, "mtpr_tbisi"},
  {0x0023, 0, "mtpr_usp"},
  {0x002a, 0, "mtpr_vptb"},
  {0x0004, 0, "stqp"},
  {0x0005, 0, "swpctx"},
  {0x000a, 0, "swap_pal"},
  {-1,    -1, ""}
};

static DECOPFUNC opc10[] = {
  {0x00,   0, "addl"},
  {0x02,   0, "s4addl"},
  {0x09,   0, "subl"},
  {0x0b,   0, "s4subl"},
  {0x0f,   0, "cmpbge"},
  {0x12,   0, "s8addl"},
  {0x1b,   0, "s8subl"},
  {0x1d,   0, "cmpult"},
  {0x20,   0, "addq"},
  {0x22,   0, "s4addq"},
  {0x29,   0, "subq"},
  {0x2b,   0, "s4subq"},
  {0x2d,   0, "cmpeq"},
  {0x32,   0, "s8addq"},
  {0x3b,   0, "s8subq"},
  {0x3d,   0, "cmpule"},
  {0x40,   0, "addl/v"},
  {0x49,   0, "subl/v"},
  {0x4d,   0, "cmplt"},
  {0x60,   0, "addq/v"},
  {0x69,   0, "subq/v"},
  {0x6d,   0, "cmple"},
  {-1,    -1, ""}
};

static DECOPFUNC opc11[] = {
  {0x00,   0, "and"},
  {0x08,   0, "bic"},
  {0x14,   0, "cmovlbs"},
  {0x16,   0, "cmovlbc"},
  {0x20,   0, "bis"},
  {0x24,   0, "cmoveq"},
  {0x26,   0, "cmovne"},
  {0x28,   0, "ornot"},
  {0x40,   0, "xor"},
  {0x44,   0, "cmovlt"},
  {0x46,   0, "cmovge"},
  {0x48,   0, "eqv"},
  {0x64,   0, "cmovle"},
  {0x66,   0, "cmovgt"},
  {-1,    -1, ""}
};

static DECOPFUNC opc12[] = {
  {0x02,   0, "mskbl"},
  {0x06,   0, "extbl"},
  {0x0b,   0, "insbl"},
  {0x12,   0, "mskwl"},
  {0x16,   0, "extwl"},
  {0x1b,   0, "inswl"},
  {0x22,   0, "mskll"},
  {0x26,   0, "extll"},
  {0x2b,   0, "insll"},
  {0x30,   0, "zap"},
  {0x31,   0, "zapnot"},
  {0x32,   0, "mskql"},
  {0x34,   0, "srl"},
  {0x36,   0, "extql"},
  {0x39,   0, "sll"},
  {0x3b,   0, "insql"},
  {0x3c,   0, "sra"},
  {0x52,   0, "mskwh"},
  {0x57,   0, "inswh"},
  {0x5a,   0, "extwh"},
  {0x62,   0, "msklh"},
  {0x67,   0, "inslh"},
  {0x6a,   0, "extlh"},
  {0x72,   0, "mskqh"},
  {0x77,   0, "insqh"},
  {0x7a,   0, "extqh"},
  {-1,    -1, ""}
};

static DECOPFUNC opc13[] = {
  {0x00,   0, "mull"},
  {0x20,   0, "mulq"},
  {0x30,   0, "umulh"},
  {0x40,   0, "mull/v"},
  {0x60,   0, "mulq/v"},
  {-1,    -1, ""}
};

static DECOPFUNC opc14[] = {
  {0x04,   0x102, "itofs"},
  {0x0A,   0x110, "sqrtf/c"},
  {0x0B,   0x110, "sqrts/c"},
  {0x14,   0x102, "itoff"},
  {0x24,   0x102, "itoft"},
  {0x2A,   0x110, "sqrtg/c"},
  {0x2b,   0x110, "sqrtt/c"},
  {0x4b,   0x110, "sqrts/m"},
  {0x6b,   0x110, "sqrtt/m"},
  {0x8a,   0x110, "sqrtf"},
  {0x8b,   0x110, "sqrts"},
  {0xaa,   0x110, "sqrtg"},
  {0xab,   0x110, "sqrtt"},
  {0xcb,   0x110, "sqrts/d"},
  {0xeb,   0x110, "sqrtt/d"},
  {0x10a,  0x110, "sqrtf/uc"},
  {0x10b,  0x110, "sqrts/uc"},
  {0x12a,  0x110, "sqrtg/uc"},
  {0x12b,  0x110, "sqrtt/uc"},
  {0x14b,  0x110, "sqrts/um"},
  {0x16b,  0x110, "sqrtt/um"},
  {0x18a,  0x110, "sqrtf/u"},
  {0x18b,  0x110, "sqrts/u"},
  {0x1aa,  0x110, "sqrtg/u"},
  {0x1ab,  0x110, "sqrtt/u"},
  {0x1cb,  0x110, "sqrts/ud"},
  {0x1eb,  0x110, "sqrtt/ud"},
  {0x40a,  0x110, "sqrtf/sc"},
  {0x42a,  0x110, "sqrtg/sc"},
  {0x48a,  0x110, "sqrtf/s"},
  {0x4aa,  0x110, "sqrtg/s"},
  {0x50a,  0x110, "sqrtf/suc"},
  {0x50b,  0x110, "sqrts/suc"},
  {0x52a,  0x110, "sqrtg/suc"},
  {0x52b,  0x110, "sqrtt/suc"},
  {0x54b,  0x110, "sqrts/sum"},
  {0x56b,  0x110, "sqrtt/sum"},
  {0x58a,  0x110, "sqrtf/su"},
  {0x58b,  0x110, "sqrts/su"},
  {0x5aa,  0x110, "sqrtg/su"},
  {0x5ab,  0x110, "sqrtt/su"},
  {0x5cb,  0x110, "sqrtf/sud"},
  {0x5eb,  0x110, "sqrtt/sud"},
  {0x70b,  0x110, "sqrts/suic"},
  {0x72b,  0x110, "sqrtt/suic"},
  {0x74b,  0x110, "sqrts/suim"},
  {0x76b,  0x110, "sqrtt/suim"},
  {0x78b,  0x110, "sqrts/sui"},
  {0x7ab,  0x110, "sqrtt/sui"},
  {0x7cb,  0x110, "sqrts/suid"},
  {0x7eb,  0x110, "sqrtt/suid"},
  {-1,    -1, ""}
};

static DECOPFUNC opc15[] = {
  {0x000,  0x111, "addf/c"},
  {0x001,  0x111, "subf/c"},
  {0x002,  0x111, "mulf/c"},
  {0x003,  0x111, "divf/c"},
  {0x01e,  0x111, "cvtdg/c"},
  {0x020,  0x111, "addg/c"},
  {0x021,  0x111, "subg/c"},
  {0x022,  0x111, "mulg/c"},
  {0x023,  0x111, "divg/c"},
  {0x02c,  0x111, "cvtgf/c"},
  {0x02d,  0x111, "cvtgd/c"},
  {0x02f,  0x111, "cvtgq/c"},
  {0x03c,  0x111, "cvtqf/c"},
  {0x03e,  0x111, "cvtqg/c"},
  {0x080,  0x111, "addf"},
  {0x081,  0x111, "subf"},
  {0x082,  0x111, "mulf"},
  {0x083,  0x111, "divf"},
  {0x09e,  0x111, "cvtdg"},
  {0x0a0,  0x111, "addg"},
  {0x0a1,  0x111, "subg"},
  {0x002,  0x111, "mulg"},
  {0x0a3,  0x111, "divg"},
  {0x0a5,  0x111, "cmpgeq"},
  {0x0a6,  0x111, "cmpglt"},
  {0x0a7,  0x111, "cmpgle"},
  {0x0ac,  0x111, "cvtgf"},
  {0x0ad,  0x111, "cvtgd"},
  {0x0af,  0x111, "cvtgq"},
  {0x0bc,  0x111, "cvtqf"},
  {0x1be,  0x111, "cvtqg"},
  {0x100,  0x111, "addf/uc"},
  {0x101,  0x111, "subf/uc"},
  {0x102,  0x111, "mulf/uc"},
  {0x103,  0x111, "divf/uc"},
  {0x11e,  0x111, "cvtdg/uc"},
  {0x120,  0x111, "addg/uc"},
  {0x121,  0x111, "subg/uc"},
  {0x122,  0x111, "mulg/uc"},
  {0x123,  0x111, "divg/uc"},
  {0x12c,  0x111, "cvtgf/uc"},
  {0x12d,  0x111, "cvtgd/uc"},
  {0x12f,  0x111, "cvtgq/vc"},
  {0x180,  0x111, "addf/u"},
  {0x181,  0x111, "subf/u"},
  {0x182,  0x111, "mulf/u"},
  {0x183,  0x111, "divf/u"},
  {0x19e,  0x111, "cvtdg/u"},
  {0x1a0,  0x111, "addg/u"},
  {0x1a1,  0x111, "subg/u"},
  {0x1a2,  0x111, "mulg/u"},
  {0x1a3,  0x111, "divg/u"},
  {0x1ac,  0x111, "cvtgf/u"},
  {0x1ad,  0x111, "cvtgd/u"},
  {0x1af,  0x111, "cvtgq/v"},
  {0x400,  0x111, "addf/sc"},
  {0x401,  0x111, "subf/sc"},
  {0x402,  0x111, "mulf/sc"},
  {0x403,  0x111, "divf/sc"},
  {0x41e,  0x111, "cvtdg/sc"},
  {0x420,  0x111, "addg/sc"},
  {0x421,  0x111, "subg/sc"},
  {0x422,  0x111, "mulg/sc"},
  {0x423,  0x111, "divg/sc"},
  {0x42c,  0x111, "cvtgf/sc"},
  {0x42d,  0x111, "cvtgd/sc"},
  {0x42f,  0x111, "cvtgq/sc"},
  {0x480,  0x111, "addf/s"},
  {0x481,  0x111, "subf/s"},
  {0x482,  0x111, "mulf/s"},
  {0x483,  0x111, "divf/s"},
  {0x49e,  0x111, "cvtdg/s"},
  {0x4a0,  0x111, "addg/s"},
  {0x4a1,  0x111, "subg/s"},
  {0x4a2,  0x111, "mulg/s"},
  {0x4a3,  0x111, "divg/s"},
  {0x4a5,  0x111, "cmpgeq/s"},
  {0x4a6,  0x111, "cmpglt/s"},
  {0x4a7,  0x111, "cmpgle/s"},
  {0x4ac,  0x111, "cvtgf/s"},
  {0x4ad,  0x111, "cvtgd/s"},
  {0x4af,  0x111, "cvtgq/s"},
  {0x500,  0x111, "addf/suc"},
  {0x501,  0x111, "subf/suc"},
  {0x502,  0x111, "mulf/suc"},
  {0x503,  0x111, "divf/suc"},
  {0x51e,  0x111, "cvtdg/suc"},
  {0x520,  0x111, "addg/suc"},
  {-1,    -1, ""}
};

static DECOPFUNC opc18[] = {
  {0x00,   0, "trapb"},
  {0x400,  0, "excb"},
  {0x4000, 0, "mb"},
  {0x4400, 0, "wmb"},
  {0x8000, 1, "fetch"},
  {0xa000, 1, "fetch_m"},
  {0xc000, 1, "rpcc"},
  {0xe000, 1, "rc"},
  {0xe800, 1, "ecb"},
  {0xf000, 1, "rs"},
  {0xf800, 1, "wh64"},
  {-1,    -1, ""}
};

static DECOPFUNC opc1a[] = {
  {0x00,   1, "jmp"},
  {0x01,   1, "jsr"},
  {0x02,   0, "ret"},
  {0x03,   0, "jsr_coroutine"},
  {-1,    -1, ""}
};

static DECOPFUNC opcxx[] = {
  {-1,    -1, ""}
};

static DECOPFUNC *decode_ops[64] = {
  &opc00[0],  &opcxx[0],  &opcxx[0],  &opcxx[0],  &opcxx[0],  &opcxx[0],  &opcxx[0],  &opcxx[0], 
  &opcxx[0],  &opcxx[0],  &opcxx[0],  &opcxx[0],  &opcxx[0],  &opcxx[0],  &opcxx[0],  &opcxx[0],
  &opc10[0],  &opc11[0],  &opc12[0],  &opc13[0],  &opc14[0],  &opc15[0],  &opcxx[0],  &opcxx[0], 
  &opc18[0],  &opcxx[0],  &opc1a[0],  &opcxx[0],  &opcxx[0],  &opcxx[0],  &opcxx[0],  &opcxx[0],
  &opcxx[0],  &opcxx[0],  &opcxx[0],  &opcxx[0],  &opcxx[0],  &opcxx[0],  &opcxx[0],  &opcxx[0], 
  &opcxx[0],  &opcxx[0],  &opcxx[0],  &opcxx[0],  &opcxx[0],  &opcxx[0],  &opcxx[0],  &opcxx[0], 
  &opcxx[0],  &opcxx[0],  &opcxx[0],  &opcxx[0],  &opcxx[0],  &opcxx[0],  &opcxx[0],  &opcxx[0], 
  &opcxx[0],  &opcxx[0],  &opcxx[0],  &opcxx[0],  &opcxx[0],  &opcxx[0],  &opcxx[0],  &opcxx[0]
};

/*
 * Macro's
 *
 */
#define DISPLAY_FIRST_MESSAGE() {\
  if (!(ctx->dflags & DISPLAYED_ERROR)) {\
    fprintf( ctx->ofile, "** Potential Alpha Architecture Violation(s) found in file %s\n", &ctx->rspec[0]);\
    ctx->dflags |= DISPLAYED_ERROR;\
  }\
}


/*
 *  Routine:	main
 *
 *  Abstract:	Entry point for program.  Parse the command line,
 *		and call the processing routine for each file.
 *
 *  Inputs:	command line args
 *
 *  Outputs:	success
 *
 */
int
main(int argc, char **argv) {

  int status = 0;
  CONTEXT ctx = {0};

 /*
  *  Put the args in the context block
  *
  */
  ctx.argc = argc;
  ctx.argv = argv;

 /*
  *  Parse the command line
  *
  */
  status = parse_command_line(&ctx);
  if (!status) return (status);

 /*
  *  Copy the initial file spec into these buffers.
  *
  *  NOTE: The rspec[] MUST be filled in for the code
  *  to work.  Even for non-VMS, since DISPLAY_FIRST_MESSAGE()
  *  displays the filename.  On VMS, this is filled in by
  *  the LIB$FILE_SCAN routine.  I'm filling it in here with
  *  the command line input file name just in case for non-VMS
  *  ports.
  *
  */
  strcpy (&ctx.ispec[0], ctx.argv[ctx.ifile_arg]);
  strcpy (&ctx.rspec[0], ctx.argv[ctx.ifile_arg]);

/*
 *  VMS specific code to read files, and call the processing
 *  routines.  In our case, we call lib$file_scan which will
 *  allow wildcard lookups, and will call process_vms_file
 *  for each file that is found, process_vms_file_error will
 *  be called for files that can't be accessed at a high level.
 *  The process_vms_file routine will then open the file and
 *  load it into memory, then call the common process_image_file
 *  to handle the specific image data (independent of loading
 *  and reading the file in).
 *
 */
#ifdef VMS

  {
    char    default_file[] = "*.EXE";
    struct  FAB *sFAB;
    struct  NAM *sNAM;

   /*
    *  initialize the search FAB and NAM block for the file scan
    *
    */
    ctx.sfab           = cc$rms_fab;
    ctx.snam           = cc$rms_nam;

    sFAB               = &ctx.sfab;
    sNAM               = &ctx.snam;
    sFAB->fab$l_nam    = &ctx.snam;
    sNAM->nam$l_esa    = &ctx.espec[0];
    sNAM->nam$b_ess    = MAX_NAME_SIZE;
    sNAM->nam$l_rsa    = &ctx.rspec[0];
    sNAM->nam$b_rss    = MAX_NAME_SIZE;
    sNAM->nam$b_rsl    = 0;
    ctx.search_context = 0;
  
    sFAB->fab$l_fna = &ctx.ispec[0];
    sFAB->fab$b_fns = strlen(&ctx.ispec[0]);
    sFAB->fab$l_dna = default_file;
    sFAB->fab$b_dns = sizeof (default_file);
    sFAB->fab$l_fop = FAB$M_UFO;
    sFAB->fab$b_fac = FAB$M_GET;

    lib$file_scan((unsigned int *) sFAB,
                  &process_vms_file,
                  &process_vms_file_error,
                  &ctx.search_context);
  }

#endif

 /*
  *  Close the output file on the way out...
  */
  fclose (ctx.ofile);

  return (SUCCESS);
}

/*
 *  Routine:	process_vms_file_error
 *
 *  Abstract:	Called when the lib$file_scan service can't open
 *		a file.  Print an error message, and signal the
 *		error.  VMS only routine.
 *
 *  Inputs:	Context block (start is overlaid with the FAB)
 *
 *  Outputs:	success (status ignored).
 *
 */
#ifdef VMS

static int
process_vms_file_error(CONTEXT *ctx) {

  struct NAM *sNAM = &ctx->snam;

  ctx->rspec[sNAM->nam$b_rsl] = 0;

  fprintf( ctx->ofile, "Could not open the file %s\n", &ctx->rspec[0]);
  lib$signal(ctx->sfab.fab$l_stv);

  return (SUCCESS);
}
#endif

/*
 *  Routine:	process_vms_file
 *
 *  Abstract:	Called by lib$file_scan for each matching
 *		file.  This routine opens the file, maps
 *		it as a private section, decodes the image
 *		header, and then calls the code that processes
 *		the image (EXE) sections.  VMS-only routine.
 *
 *  Inputs:	context block
 *
 *  Outputs:	success
 *
 */
#ifdef VMS

static int
process_vms_file(CONTEXT *ctx) {

  extern int sys$asctim();

  int    status = 0, j, i = 0, searched = 0, imtimlen = 0;
  EIHD   *pImageHeader;
  EIHI   *pImageIdent;
  EISD   *pImageSection;
  struct _va_range addr_base, use_P0;

  struct NAM *sNAM;

  char link_time[80], isd_flags[80];
  struct dsc$descriptor_s imtime = {79, DSC$K_DTYPE_T, DSC$K_CLASS_S, 0};

 /*
  *  Bit names for section flags
  *
  */
  typedef struct _names {
    int   bits;
    char *name;
  } NAMES;

  static NAMES secbits[] = {
   EISD$M_GBL,         "GBL ",
   EISD$M_CRF,         "CRF ",
   EISD$M_DZRO,        "DZRO ",
   EISD$M_WRT,         "WRT ",
   EISD$M_INITALCODE,  "INITALCODE ",
   EISD$M_BASED,       "BASED ",
   EISD$M_FIXUPVEC,    "FIXUPVEC ",
   EISD$M_RESIDENT,    "RESIDENT ",
   EISD$M_VECTOR,      "VECTOR ",
   EISD$M_PROTECT,     "PROTECT ",
   EISD$M_LASTCLU,     "LASTCLU ",
   EISD$M_EXE,         "EXE ",
   EISD$M_NONSHRADR,   "NONSHRADR ",
   0,                  "UNKNOWN "
  };

  sNAM = &ctx->snam;

 /*
  *   Give the input section a P0 base
  */
  use_P0.va_range$ps_end_va = use_P0.va_range$ps_start_va = (void *) 0x200;
  addr_base.va_range$ps_end_va = addr_base.va_range$ps_start_va = (void *) 0x0;

 /*
  *  Open input file.
  *
  */
  ctx->rspec[sNAM->nam$b_rsl] = 0;

  status = sys$open( &ctx->sfab);

  if (!(status & 1)) {
    fprintf( ctx->ofile, "Could not open the file %s\n", &ctx->rspec[0]);
    lib$signal (status);
    return (status);
  }

 /*
  *  Map it as a section file
  *
  */
  status = sys$crmpsc( &use_P0,                     /* inadr */
                       &addr_base,                  /* retadr */
                       0,                           /* acmode */
                       SEC$M_EXPREG | SEC$M_CRF,    /* flags */
                       0,                           /* gsdnam */
                       0,                           /* ident */
                       0,                           /* relpag */
                       ctx->sfab.fab$l_stv,         /* chan */
                       0,                           /* pagcnt */
                       0,                           /* vbn */
                       0xEEEE,                      /* prot */
                       0);                          /* pfc */
  
  if (!(status & 1)) {
    fprintf( ctx->ofile, "Could not map the file %s\n", &ctx->rspec[0]);
    lib$signal (status);
    sys$close(&ctx->sfab);
    return (status);
  }

 /*
  *  Rummage through the image header.
  */
  pImageHeader  = (EIHD *) ((char *) addr_base.va_range$ps_start_va);
  pImageIdent   = (EIHI *) ((char *) addr_base.va_range$ps_start_va + pImageHeader->eihd$l_imgidoff);
  pImageSection = (EISD *) ((char *) addr_base.va_range$ps_start_va + pImageHeader->eihd$l_isdoff);

 /*
  *  Make sure it's an Alpha image
  */
  if (pImageHeader->eihd$l_majorid == EIHD$K_MAJORID) {

    imtime.dsc$a_pointer = link_time;
    status = sys$asctim(&imtimlen, &imtime, &pImageIdent->eihi$q_linktime, 0);
    link_time[imtimlen] = 0;

   /*
    *  Log the file name
    */
    if ((ctx->command & LOG_FILES) && (ctx->command & CHECK_LDL))
      fprintf( ctx->ofile, "Checking: %s\n", &ctx->rspec[0]);

    i = 0;

   /*
    *  Loop for each image section descriptor
    */
    while (pImageSection->eisd$l_eisdsize) {

     /*
      *  Only care about sections labeled executable (and ones that exist in the image)
      */
      if (pImageSection->eisd$v_exe && pImageSection->eisd$l_vbn) {

       /*
        *  Build the flag string
        *
        */
        isd_flags[0] = 0;

        for (j=0; secbits[j].bits; j += 1) {
          if (pImageSection->eisd$l_flags & secbits[j].bits) {
            strcat(isd_flags, secbits[j].name);
          }
        }

       /*
        *  If verbose, print the section we are about to look at
        *
        */
        if (ctx->command & VERBOSE) {          
          fprintf( ctx->ofile, "Image Section: %d, vbn: %d, va: 0x%X, flags: %s(0x%X)\n", i, pImageSection->eisd$l_vbn, pImageSection->eisd$l_virt_addr, isd_flags, pImageSection->eisd$l_flags);
        }

       /*
        *  Now process the image section
        *
        */
        status =
          process_image_section(ctx,
                                (uint32 *) ((char *) addr_base.va_range$ps_start_va + ((pImageSection->eisd$l_vbn-1) * 512) ),
                                (uint32 *) pImageSection->eisd$l_virt_addr,
                                pImageSection->eisd$l_secsize >> 2);

       /*
        *  Keep track of sections that were searched
        *
        */
        if (status == VA_NOT_FOUND) searched = 1;

       /*
        *  A problem was reported!  Tell the user.
        */
        if (ctx->final_status == FOUND_PROBLEM) {
          fprintf( ctx->ofile, "Image Name:    %-.*s\n", pImageIdent->eihi$t_imgnam[0], &pImageIdent->eihi$t_imgnam[1]);
          fprintf( ctx->ofile, "Image Ident:   %-.*s\n", pImageIdent->eihi$t_imgid[0],  &pImageIdent->eihi$t_imgid[1]);
          fprintf( ctx->ofile, "Link Time:     %-s\n",  &link_time[0]);
          fprintf( ctx->ofile, "Build Ident:   %-.*s\n", pImageIdent->eihi$t_imgbid[0], &pImageIdent->eihi$t_imgbid[1]);
          fprintf( ctx->ofile, "Header Size:   %d\n",   pImageHeader->eihd$l_size);
          fprintf( ctx->ofile, "Image Section: %d, vbn: %d, va: 0x%X, flags: %s(0x%X)\n", i, pImageSection->eisd$l_vbn, pImageSection->eisd$l_virt_addr, isd_flags, pImageSection->eisd$l_flags);
        }
      }

     /*
      *  Next ISD
      *
      */
      pImageSection = (EISD *) ((char *) pImageSection + pImageSection->eisd$l_eisdsize);

     /*
      *   They don't allow spanning of a *disk* block boundry here... so round up if you see -1 as the length
      */  
      if (pImageSection->eisd$l_eisdsize == -1) {
        pImageSection = (EISD *) (((int) pImageSection + 0x1FF) & ~0x1FF);
      }

      i++;
    }

   /*
    *  See if we did anything
    *
    */
    if ((ctx->start_va || ctx->end_va) && !searched) {
      printf("VA range not found: %8.8X:%8.8X\n", ctx->start_va, ctx->end_va);
    }
  }
  else {

   /*
    *  The first longword is the major Ident.  Better never get as large
    *  as the VAX header size ;-)
    *
    */
    if (ctx->command & VERBOSE) fprintf( ctx->ofile, "*  File is not an Alpha executable\n");
  }

 /*
  *  Delete the VA
  */
  status = sys$deltva( &addr_base, 0, 0);

 /*
  *  Close the section channel and disconnect the file
  */
  status = sys$dassgn(ctx->sfab.fab$l_stv);
  sys$close(&ctx->sfab);

  return (SUCCESS);
}
#endif

/*
 *  Routine:	process_image_section
 *
 *  Abstract:	Top level routine for processing an image
 *		section (a mapped range containing what we
 *		think is Alpha instructions).  It calls
 *		the disassemble routine to dump, or check
 *		the image.  It also calls down to check
 *		branches in ldl/stc sequences.
 *
 *		NOTE:  The code to be looked at MUST be all
 *		in memory.  The code assumes that it can look
 *		at anything in the range without any I/O.  So
 *		it must be completely read into memory.
 *
 *  Inputs:	context block, base address of mapped image,
 *		user VA (i.e. the VA used when displaying),
 *		and the size in bytes to process.
 *
 *  Outputs:	success
 *
 */
static int32
process_image_section(CONTEXT *ctx, uint32 *base, uint32 *va, int32 size) {

  int status;

 /*
  *  Set up the image section information for the disassembly
  *
  */
  ctx->dflags      &= ~DISPLAYED_ERROR;
  ctx->final_status = SUCCESS;
  ctx->error_count  = 0;  
  ctx->pInput       = base;
  ctx->size         = size;
  ctx->va           = va;
  ctx->seq_cnt      = 0;

 /*
  *  See if this is a limited range dump...
  *
  */
  if (ctx->start_va || ctx->end_va) {
    uint32 *va_end = ctx->va + ctx->size;
    int32 offset = (int32) (ctx->start_va - ctx->va);

    if (ctx->end_va == 0) ctx->end_va = va_end;

    if ((ctx->start_va < ctx->va) || (ctx->start_va > va_end)) return (VA_NOT_FOUND);

   /*
    *  If the ending VA is past the end of the section, truncate it
    *
    */
    if (ctx->end_va > va_end) {
      fprintf(ctx->ofile, "-- end_va of %8.8X is past end of section at %8.8X, minimizing\n", ctx->end_va, va_end);
      ctx->end_va = va_end;
    }

   /*
    *  Adjust the context to limit the search
    *
    */
    ctx->va   = ctx->start_va;
    ctx->size = (int32) (ctx->end_va - ctx->start_va) + 1;
    ctx->pInput = ctx->pInput + offset;
  }

 /*
  * * * * * * * * * * * * * * * * * * * * * * * * *
  *   D I S A S S E M B L E   T H E   I M A G E   *
  * * * * * * * * * * * * * * * * * * * * * * * * *
  */
  status = disassemble_image(ctx);

 /*
  *  If directed to, we can dump all of the sequences that were
  *  discovered in the search of the image.
  *
  */
  if ((ctx->command & CHECK_LDL) && (ctx->command & DUMP_ALL_SEQUENCES)) {
    SEQ *pSeq = ctx->seq_map;
    int scnt, i;

    fprintf( ctx->ofile, "Found %d LDxL/STx_C sequences\n", ctx->seq_cnt );

    if (ctx->seq_cnt > ctx->seq_avl) {
      scnt = ctx->seq_avl;
    }
    else scnt = ctx->seq_cnt;

    for (i = 0; i < scnt; i += 1) {
      fprintf( ctx->ofile, "seq %2.2d, start %8.8X, size %8.8X, flags = %X\n", i, pSeq[i].va, pSeq[i].size << 2, pSeq[i].flags);
      dump_branch_sequence(ctx, i);
    }
  }

 /*
  *  Follow branches to find degenrate cases of a
  *  branch into a ldl/stc sequence.  Only need to
  *  do this if we are looking for bad sequences, and
  *  if we found any ldl/stc sequences.
  *
  */
  if ((ctx->command & CHECK_LDL) && (ctx->command & FOLLOW_BRANCHES) && (ctx->seq_cnt)) {
    uint32 save_command;

    if (ctx->seq_cnt > ctx->seq_avl) {
      fprintf( ctx->ofile, "** Could not log all ldl/stc, re-run with -seqcnt %d\n", ctx->seq_cnt+1);
      fprintf( ctx->ofile, "   Checking the first %d\n", ctx->seq_avl);
    }

   /*
    *  Save the command, we need to restore it for multiple files
    *  Change the command to check for branches.
    *
    */
    save_command = ctx->command;
    ctx->command = CHECK_BRANCHES;

   /*
    *  Now check the branches
    *
    */
    status = disassemble_image(ctx);
    ctx->command = save_command;
  }
  return (SUCCESS);
}

/*
 *  Routine:	dump_branch_sequence
 *
 *  Abstract:	Called to do a formatted dump of
 *		a code sequence using information
 *		stored in a SEQ entry.
 *
 *  Inputs:	context blick and index (id) of sequence.
 *
 *  Outputs:	none
 *
 */
static void
dump_branch_sequence(CONTEXT *ctx, int id) {
  CONTEXT print_ctx;
  SEQ *pSeq = ctx->seq_map;

 /*
  *  Create a print context
  *
  */
  print_ctx.ofile   = ctx->ofile;
  print_ctx.pInput  = pSeq[id].start;
  print_ctx.size    = pSeq[id].size;
  print_ctx.va      = pSeq[id].va;
  print_ctx.command = DUMP;

 /*
  *  Print the sequence
  */
  disassemble_image(&print_ctx);
}

/*
 *  Routine:	dump_sequence
 *
 *  Abstract:	Does a formatted dump of the current ldl/stc
 *		sequence.
 *
 *  Inputs:	contect block
 *
 *  Outputs:	none
 *
 */
/*
 *  Print the sequence
 *
 */
static void
dump_sequence(CONTEXT *ctx) {

  CONTEXT print_ctx;

 /*
  *  Create a print context
  *
  */
  print_ctx.ofile   = ctx->ofile;
  print_ctx.pInput  = ctx->sequence_start;
  print_ctx.size    = ctx->sequence_size;
  print_ctx.va      = ctx->sequence_va;
  print_ctx.command = DUMP;

 /*
  *  Print the sequence
  */
  disassemble_image(&print_ctx);
}

/*
 *  Routine:	disassemble_image
 *
 *  Abstract:	Main loop for image decoding.  For each
 *		longword in the range, it gets the opcode,
 *		and calls the appropriate decoder function.
 *		On return it increments throught the image,
 *		checking for returned errors.
 *
 *  Inputs:	context block
 *
 *  Outputs:	status from decode, generally SUCCESS, or FOUND_PROBLEM
 *
 */
/*
 *   Decode the instructions in the image
 *
 */
static int32
disassemble_image(CONTEXT *ctx) {
  int32 size = ctx->size;

 /*
  *  Initialize some working context
  *
  */
  ctx->current_address  = ctx->pInput;
  ctx->current_context  = 0;
  ctx->addr             = ctx->va;
  ctx->error_count      = 0;
  ctx->sequence_size    = 0;
  ctx->flags            = 0;

  while (size > 0) {

   /*
    *  Get the opcode
    */
    ctx->opcode = (*ctx->current_address >> 26) & 0x3F;

   /*
    *  Start with success
    *
    */
    ctx->error  = SUCCESS;

   /*
    *  Decode the instruction
    */
    (decoder[optypes[ctx->opcode]].routine)(ctx);

   /*
    *  If were looking for LDx_L/STx_C check the status
    */
    if (ctx->command & CHECK_LDL) {

     /*
      *  Make sure to bounds check the number of instructions in a LDx_L/STx_C pair
      *  as per SRM Architecture reference 3rd edition, 5.5.2, pg 5-6 bottom of page.
      *  And A.5, pg A-16 "At most 40 operate or conditional branch (not taken)
      *  instructions... between LDx_L/STx_C."
      */
      if ((ctx->current_context) &&
          (ctx->sequence_size > MAX_INSTRUCTIONS)) {

       /*
        *  Print the sequence - unless we already did it.  Ignore it
        *  if we think it's not code!
        *
        */
        if (!(ctx->flags & BAD_LOCK_SEQ) && (!ctx->flags & NOT_CODE)) {

          fprintf( ctx->ofile, "** More than %d instructions following a LDx_L at %8.8X\n", MAX_INSTRUCTIONS, ctx->addr );

          ctx->error_count += 1;
          dump_sequence(ctx);
         /*
          *  Report error
          */
          ctx->flags |= BAD_LOCK_SEQ;
          ctx->error  = FOUND_PROBLEM;
          ctx->error_address = ctx->current_address;
        }
        else {
          ctx->current_context = 0;
        }
      }

     /*
      *  See if we found a problem...
      *
      */
      if (ctx->error == FOUND_PROBLEM) {
        ctx->final_status = FOUND_PROBLEM;
        if (ctx->error_count >= ctx->max_errors) {
          fprintf( ctx->ofile, "** Found %d or more potential problems... skipping rest of image section\n", ctx->max_errors);
          return (ctx->final_status);
        }
      }
    }

   /*
    *  Next address..
    */
    *ctx->current_address++;
    *ctx->addr ++;
    ctx->sequence_size += 1;
    size -= 1;
  }
  return (ctx->final_status);
}

/*
 *  Routine:	decode_unknown
 *
 *  Abstract:	Decode a unknown (or unimplemented) instruction.
 *		The NOT_CODE flag tells ldl/stc processing that
 *		it's probably not real code.
 *
 */
void
decode_unknown(CONTEXT *ctx) {
  if (ctx->command & DUMP) fprintf( ctx->ofile, "%8.8X   %8.8X     %s\n", ctx->addr, *ctx->current_address, opcode_names[ctx->opcode]);
  ctx->flags |= NOT_CODE; /* Let a search know we found crap */
  ctx->current_context = 0;
}

/*
 *  Routine:	decode_mem
 *
 *  Abstract:	Decode a memory instruction format opcode (SRM 3.3.1)
 *
 */
void
decode_mem(CONTEXT *ctx) {
  uint32 ra = 0, rb = 0, disp = 0, dflags = ctx->command;
  char ras[10] = "SP", rbs[10] = "SP";
  uint32 data = *ctx->current_address;
  SEQ *pSeq = ctx->seq_map;

  ra   = (data >> 21) & 0x1F;
  rb   = (data >> 16) & 0x1F;

  if (ra != 30) sprintf(&ras[0], "R%d", ra);
  if (rb != 30) sprintf(&rbs[0], "R%d", rb);

 /*
  *  Load locked check.  See if there are any MEM instructions
  *  between LD_L/ST_C as per SRM "no useful program shall..."
  */
  if (ctx->command & CHECK_LDL) {

    switch (ctx->opcode) {

     /*
      *  Check LDx_L
      *
      */
      case 42:
      case 43:

       /*
        *   Did we already see one without a stx_c?
        */
        if ((ctx->current_context == 42) || (ctx->current_context == 43)) {

         /*
          *  OK.  This is technically not a error.  But it "could" signify some
          *  bad code.  If the user sets verbose, then complain.  Otherwise let
          *  it go.
          *
          */
          if (ctx->command & VERBOSE) {
            DISPLAY_FIRST_MESSAGE();
            fprintf( ctx->ofile, "** Expected a %s, but found a %s at %8.8X.\n",
                     opcode_names[ctx->current_context+4], opcode_names[ctx->opcode], ctx->addr);

           /*
            *  Print the sequence
            */
            dump_sequence(ctx);

            ctx->error = FOUND_PROBLEM;
            ctx->error_address = ctx->current_address;
          }
        }

        ctx->current_context  = ctx->opcode;
        ctx->flags           &= ~(NOT_CODE | BAD_LOCK_SEQ);
        ctx->sequence_start   = ctx->current_address;
        ctx->sequence_va      = ctx->addr;
        ctx->sequence_size    = 1;

        break;

     /*
      *  Check STx_C
      *
      */
      case 46:
      case 47:

       /*
        *  If we see a STx_C without a LDx_L - it is probably an
        *  error - except when a "real" HW mailbox is written.
        *  Cause a one-liner to print, plus dump the stx_c line.
        *
        */
        if ((!ctx->current_context) && (ctx->command & AGGRESSIVE)) {
          DISPLAY_FIRST_MESSAGE();
          fprintf( ctx->ofile, "** Found an unexpected %s at %8.8X with no preceding %s\n",
               opcode_names[ctx->opcode], ctx->addr, opcode_names[ctx->opcode-4]);
          dflags |= DUMP;
          ctx->error = FOUND_PROBLEM;
          ctx->error_address = ctx->current_address;
        }

        if ((ctx->current_context) && (!(ctx->flags & NOT_CODE))) {

         /*
          *  Build descriptors of all sequences found
          *
          */
          if (ctx->seq_cnt < ctx->seq_avl) {
            pSeq[ctx->seq_cnt].start = ctx->sequence_start;
            pSeq[ctx->seq_cnt].va    = ctx->sequence_va;
            pSeq[ctx->seq_cnt].size  = ctx->sequence_size;
            pSeq[ctx->seq_cnt].flags = ctx->flags;
          }
          ctx->seq_cnt += 1;
        }
        else {
         /*
          *  Naked STx_C.  Save it as a sequence to help detect bad branches.
          *  Use the instruction before and after as part of the sequence.
          */
          if (!ctx->current_context) {
            if (ctx->seq_cnt < ctx->seq_avl) {
              pSeq[ctx->seq_cnt].start = ctx->current_address-NAKED_STC_SEQ_SIZE;
              pSeq[ctx->seq_cnt].va    = ctx->addr-NAKED_STC_SEQ_SIZE;
              pSeq[ctx->seq_cnt].size  = NAKED_STC_SEQ_SIZE * 2;
              pSeq[ctx->seq_cnt].flags = NAKED_STX_C;
            }
            ctx->seq_cnt += 1;
          }
        }

       /*
        *  Clear flags and context
        *
        */
        ctx->flags &= ~(NOT_CODE | BAD_LOCK_SEQ);
        ctx->current_context  = 0;

        break;

     /*
      *  Ignore LDAx (used to load a literal, does not effect memory)
      *
      */
      case 8:
      case 9:
        break;

     /*
      *  Other memory instructions
      *
      */
      default:

       /*
        *  Ignore UNOPs (Ra = R31 - that is a load to R31 is ignored, and safe)
        *
        */
        if ((ctx->current_context) && (ra != 31)) {

         /*
          *  MEMORY OPERATION FOUND
          *
          */
          if (ctx->flags & NOT_CODE) {
            if ((ctx->command & VERBOSE) || (ctx->command & AGGRESSIVE)) {
              DISPLAY_FIRST_MESSAGE();
              fprintf( ctx->ofile, "** Likely data segment false error, found an %s at %8.8X\n", opcode_names[ctx->opcode], ctx->addr);
            }
          }
          else {
	    if (!(ctx->flags & BAD_LOCK_SEQ)) {
              DISPLAY_FIRST_MESSAGE();
              fprintf( ctx->ofile, "** Found an unexpected %s at %8.8X\n",
                       opcode_names[ctx->opcode], ctx->addr);
            }
          }

         /*
          *  Dump the sequence if it had no unknown instructions, or if verbose was set
          */
          if ((!(ctx->flags & NOT_CODE)) || (ctx->command & VERBOSE)) {

           /*
            *  Print the sequence - unless we already did it.
            */
            if (!(ctx->flags & BAD_LOCK_SEQ)) {
              ctx->error_count += 1;
              dump_sequence(ctx);
            }
            ctx->flags |= BAD_LOCK_SEQ;
          }

         /*
          *  Set the error
          */
          ctx->error = FOUND_PROBLEM;
          ctx->error_address = ctx->current_address;

        }

        break;
    } /* End of switch statement */
  } /* End of:  if (ctx->command & CHECK_LDL) { */

  disp = data & 0xFFFF;

  if (disp) {
    if (dflags & DUMP) fprintf( ctx->ofile, "%8.8X   %8.8X     %-14s %s, 0x%X(%s)\n", ctx->addr, data, opcode_names[ctx->opcode], &ras[0], disp, &rbs[0]);
  }
  else {
    if (dflags & DUMP) fprintf( ctx->ofile, "%8.8X   %8.8X     %-14s %s, (%s)\n", ctx->addr, data, opcode_names[ctx->opcode], &ras[0], &rbs[0]);
  }
}

/*
 *  Routine:	decode_op
 *
 *  Abstract:	Decode an Operate Instruction Format opcode (SRM 3.3.3)
 *
 */
void
decode_op(CONTEXT *ctx) {
  uint32 ra = 0, rb = 0, rc = 0, lit = 0, func = 0, j = 0;
  char ras[10] = "SP", rbs[10] = "SP", rcs[10] = "SP";
  char *func_name = opcode_names[ctx->opcode];
  uint32 data = *ctx->current_address;
  DECOPFUNC *decode_opfunction = decode_ops[ctx->opcode];
  
  ra   = (data >> 21) & 0x1F;
  func = (data >> 5)  & 0x7F;
  rc   = data & 0x1F;

  if (ra != 30)
    sprintf(&ras[0], "R%d", ra);

  if (rc != 30)
    sprintf(&rcs[0], "R%d", rc);

  while (decode_opfunction[j].function != -1) {
   if (decode_opfunction[j].function == func) {
     func_name = decode_opfunction[j].name;
     break;
   }
   j += 1;
  }

  if (data & (1<<12)) {
    lit = (data >> 13) & 0xFF;
    if (ctx->command & DUMP) fprintf( ctx->ofile, "%8.8X   %8.8X     %-14s %s, 0x%X, %s\n", ctx->addr, data, func_name, &ras[0], lit, &rcs[0]);
  }
  else {
    rb = (data >> 16) & 0x1F;
    if (rb != 30)
      sprintf(&rbs[0], "R%d", rb);
    if (ctx->command & DUMP) fprintf( ctx->ofile, "%8.8X   %8.8X     %-14s %s, %s, %s\n", ctx->addr, data, func_name, &ras[0], &rbs[0], &rcs[0]);
  }
}

/*
 *  Routine:	decode_pal
 *
 *  Abstract:	Decode a PAL Instruction (SRM 3.3.5)
 *
 */
void
decode_pal(CONTEXT *ctx) {
  int func, j = 0;
  char fbuf[10];
  char *func_name = opcode_names[ctx->opcode];
  uint32 data = *ctx->current_address;
  DECOPFUNC *decode_opfunction = decode_ops[ctx->opcode];

  func = data & 0x03FFFFFF;
  sprintf(&fbuf[0], "%d", func);

  while (decode_opfunction[j].function != -1) {
   if (decode_opfunction[j].function == func) {
     func_name = decode_opfunction[j].name;
     fbuf[0] = 0;
     break;
   }
   j += 1;
  }

 /*
  *  If we are looking for valid LDL/STC sequences, then see if we have a CALL_PAL
  *  in the fall through path between the LDL/STC
  *
  *  If we are not aggressively looking (we've yet to find a sequence like this)
  *  then assume this isn't code.
  *
  */
  if ((ctx->command & CHECK_LDL) && (!(ctx->command & AGGRESSIVE)) && (ctx->current_context)) {
     ctx->flags |= NOT_CODE;
     ctx->current_context = 0;
  }

  if ((ctx->command & CHECK_LDL) && (ctx->current_context)) {

   /*
    *  Dump the sequence if it had no unknown instructions, or if verbose was set
    */
    if ((!(ctx->flags & NOT_CODE)) || (ctx->command & VERBOSE)) {

     /*
      *  Print the sequence - unless we already did it.
      */
      if (!(ctx->flags & BAD_LOCK_SEQ)) {

        DISPLAY_FIRST_MESSAGE();
        fprintf( ctx->ofile, "** Found an unexpected %s at %8.8X\n", opcode_names[ctx->opcode], ctx->addr);

        ctx->error_count += 1;
        dump_sequence(ctx);
      }
      ctx->flags |= BAD_LOCK_SEQ;
    }

   /*
    *  Skip the rest of the sequence
    */
    ctx->error = FOUND_PROBLEM;
    ctx->error_address = ctx->current_address;
  }

  if (ctx->command & DUMP) fprintf( ctx->ofile, "%8.8X   %8.8X     %-14s %s\n", ctx->addr, data, func_name, &fbuf[0]);
}

/*
 *  Routine:	decode_br
 *
 *  Abstract:	Decode a Branch Instruction Format opcode (SRM 3.3.2)
 *
 */
void
decode_br(CONTEXT *ctx) {

  uint32 ra = 0;
  int32 disp = 0, i;
  uint32 *naddr = 0, *end_addr = 0;
  uint32 data = *ctx->current_address;
  uint32 dflags = ctx->command;
  SEQ *pSeq = ctx->seq_map;

  ra   = (data >> 21) & 0x1F;

 /*
  *  The displacement is a 21-bit field that is
  *  sign extended, and shifted two to form a
  *  byte offset.  This is then added to the
  *  updated PC to form the VA for the branch.
  *
  */
  disp = data & 0x1FFFFF;
  if (disp & 0x100000){
    disp |= ~0x1FFFFF;
  }

  naddr = ctx->addr + (disp+1);

 /*
  *  If we are checking load/lock, look for a branch into an existing sequence.  This will
  *  find the rotated loop GEM bug, since we will see the backward branch, and we are counting
  *  the naked STx_C as a one instruction "lock sequence".  Pretty specific, but it works.
  *  We'll flag it at an error, so we won't print multiple complaints later.
  *
  *  In addition, we could be making the second pass to check for branches into a sequence
  *  that aren't picked up.  These would be the ones with forward branches that we can't
  *  detect in the first pass, since we aren't following the code paths forward... but we
  *  are saving up all the sequences and checking all the good ones that are left for
  *  problems
  */
  if (((ctx->command & CHECK_LDL) && (ctx->current_context)) || (ctx->command & CHECK_BRANCHES)) {

   /*
    *  See if this branch enters any load locked blocks
    *
    */
    for (i = 0; i < ctx->seq_cnt; i += 1) {

      end_addr = pSeq[i].va + pSeq[i].size;

     /*
      *  If a destination is in a sequence...
      *
      */
      if ((naddr > pSeq[i].va) && (naddr < end_addr)) {

       /*
        *  We have found a branch into a sequence.  See if the current
        *  VA is inside the sequence (i.e. probably a forward branch
        *  inside the sequence), as opposed to a branch to a different
        *  sequence - usually this is the rotation error.
        *
        */
        if ((ctx->addr > pSeq[i].va) && (ctx->addr < end_addr)) {

         /*
          *  Is the branch itself inside the sequence?  That is, we
          *  have found a forward branch to a stx_c.  We can only do
          *  this in the branch check pass.
          *
          */
          if (ctx->command & CHECK_BRANCHES) {
           /*
            *  Yes.  Before we complain, check the flags to see if
            *  this is already a bad sequence.
            *
            */
            if (!(pSeq[i].flags & BAD_LOCK_SEQ)) {
              DISPLAY_FIRST_MESSAGE();
              fprintf( ctx->ofile, "** Found an illegal branch inside a LDx_L/STx_C sequence at %8.8X\n", ctx->addr);
              fprintf( ctx->ofile, "%8.8X   %8.8X     %-14s R%d, %8.8X\n", ctx->addr, data, opcode_names[ctx->opcode], ra, naddr);
              fprintf( ctx->ofile, "Full sequence:\n");
              if ((ctx->command & CHECK_LDL) && (ctx->current_context)) dump_sequence(ctx);
              dump_branch_sequence(ctx, i);
            }
            ctx->error_count += 1;
            ctx->error = FOUND_PROBLEM;
            ctx->error_address = ctx->current_address;
          }
        }
        else {
          if (!(pSeq[i].flags & BAD_LOCK_SEQ)) {
            if ((ctx->command & CHECK_LDL) && (ctx->current_context)) {
              DISPLAY_FIRST_MESSAGE();
              if (pSeq[i].va < ctx->addr) 
                fprintf( ctx->ofile, "** Found a backward branch from %8.8X to a STx_C sequence at %8.8X\n", ctx->addr, naddr);
              else
                fprintf( ctx->ofile, "** Found a forward branch from %8.8X to a STx_C sequence at %8.8X\n", ctx->addr, naddr);
              dump_branch_sequence(ctx, i);
              if (end_addr != ctx->sequence_va) fprintf( ctx->ofile, ". . .\n");
              dump_sequence(ctx);

             /*
              *  Terminate the current sequence here.  This is usually code rotation.
              *
              */
              if (ctx->seq_cnt < ctx->seq_avl) {
                pSeq[ctx->seq_cnt].start = ctx->sequence_start;
                pSeq[ctx->seq_cnt].va    = ctx->sequence_va;
                pSeq[ctx->seq_cnt].size  = ctx->sequence_size;
                pSeq[ctx->seq_cnt].flags = ctx->flags | BAD_LOCK_SEQ;
              }
              ctx->seq_cnt += 1;
              pSeq[i].flags |= BAD_LOCK_SEQ;
              ctx->flags  &= ~(NOT_CODE | BAD_LOCK_SEQ);
              ctx->current_context = 0;
            }
            else {
              DISPLAY_FIRST_MESSAGE();
              fprintf( ctx->ofile, "** Possible branch into a LDx_L/STx_C sequence found at %8.8X\n", ctx->addr);
              fprintf( ctx->ofile, "%8.8X   %8.8X     %-14s R%d, %8.8X\n", ctx->addr, data, opcode_names[ctx->opcode], ra, naddr);
              fprintf( ctx->ofile, "Branches into:\n");
              dump_branch_sequence(ctx, i);
            }
            ctx->error_count += 1;
            ctx->error = FOUND_PROBLEM;
            ctx->error_address = ctx->current_address;
          }           
          else {
            if (ctx->command & VERBOSE) {
              DISPLAY_FIRST_MESSAGE();
              fprintf( ctx->ofile, "** Possible branch into a LDx_L/STx_C sequence found at %8.8X\n", ctx->addr);
              fprintf( ctx->ofile, "...Probably the result of another error in the sequence at %8.8X\n", pSeq[i].va);
              fprintf( ctx->ofile, "%8.8X   %8.8X     %-14s R%d, %8.8X\n", ctx->addr, data, opcode_names[ctx->opcode], ra, naddr);
              ctx->error_count += 1;
              ctx->error = FOUND_PROBLEM;
              ctx->error_address = ctx->current_address;
            }
          }
        }
      }
    }
  }

 /*
  *  If we are looking for valid LDL/STC sequences, then see if we have a BSR
  *  in the fall through path between the LDL/STC
  *
  */
  if ((ctx->command & CHECK_LDL) && (ctx->current_context) && (ctx->opcode == 0x34)) {

   /*
    *  Dump the sequence if it had no unknown instructions, or if verbose was set
    */
    if ((!(ctx->flags & NOT_CODE)) || (ctx->command & VERBOSE)) {

     /*
      *  Print the sequence - unless we already did it.
      */
      if (!(ctx->flags & BAD_LOCK_SEQ)) {

        DISPLAY_FIRST_MESSAGE();
        fprintf( ctx->ofile, "** Found an unexpected %s at %8.8X\n", opcode_names[ctx->opcode], ctx->addr);

        ctx->error_count += 1;
        dump_sequence(ctx);
      }
      ctx->flags |= BAD_LOCK_SEQ;
    }

   /*
    *  Skip the rest of the sequence
    */
    ctx->error = FOUND_PROBLEM;
    ctx->error_address = ctx->current_address;
  }

  if (dflags & DUMP)
     fprintf( ctx->ofile, "%8.8X   %8.8X     %-14s R%d, %8.8X\n", ctx->addr, data, opcode_names[ctx->opcode], ra, naddr);
}

/*
 *  Routine:	decode_mfc
 *
 *  Abstract:	Decode a Memory Instruction Format with Function
 *		Code opcode (SRM 3.1.1.1)
 *
 *  Inputs:	
 *
 *  Outputs:	
 *
 */
void
decode_mfc(CONTEXT *ctx) {

  uint32 ra = 0, rb = 0, func = 0, j = 0, tflags = 0;
  char *func_name = opcode_names[ctx->opcode];
  char ras[10] = "SP", rbs[10] = "SP";
  uint32 data = *ctx->current_address;
  DECOPFUNC *decode_opfunction = decode_ops[ctx->opcode];

  ra   = (data >> 21) & 0x1F;
  rb   = (data >> 16) & 0x1F;
  func = data & 0xFFFF;

  if (ra != 30)
    sprintf(&ras[0], "R%d", ra);

  if (rb != 30)
    sprintf(&rbs[0], "R%d", rb);

  while (decode_opfunction[j].function != -1) {
   if (decode_opfunction[j].function == func) {
     func_name = decode_opfunction[j].name;
     tflags = decode_opfunction[j].flags;
     break;
   }
   j += 1;
  }

 /*
  *  If we are looking for valid LDL/STC sequences, then see if we have a
  *  opcode of 18, and anything other than a MB or WMB... since the others
  *  will cause a sequence to fail.
  *
  */
  if ((ctx->command & CHECK_LDL) && (ctx->current_context)) {

   /*
    *  If its a WMB or a MB it's OK, but all other opcode 0x18's are bad
    *
    */
    if ((ctx->opcode == 0x18) && (!((func == 0x4000) || (func == 0x4400)))) {

     /*
      *  Dump the sequence if it had no unknown instructions, or if verbose was set
      */
      if ((!(ctx->flags & NOT_CODE)) || (ctx->command & VERBOSE)) {

       /*
        *  Print the sequence - unless we already did it.
        */
        if (!(ctx->flags & BAD_LOCK_SEQ)) {

          DISPLAY_FIRST_MESSAGE();
          fprintf( ctx->ofile, "** Found an unexpected %s at %8.8X\n", func_name, ctx->addr);

          ctx->error_count += 1;
          dump_sequence(ctx);
        }
        ctx->flags |= BAD_LOCK_SEQ;
      }

     /*
      *  Set the error
      */
      ctx->error = FOUND_PROBLEM;
      ctx->error_address = ctx->current_address;
    }
  }


  if (tflags & 1) {
    if (ctx->command & DUMP) fprintf( ctx->ofile, "%8.8X   %8.8X     %-14s %s\n", ctx->addr, data, func_name, &ras[0]);
  }
  else {
    if (ctx->command & DUMP) fprintf( ctx->ofile, "%8.8X   %8.8X     %-14s \n", ctx->addr, data, func_name);
  }
}

/*
 *  Routine:	decode_fop
 *
 *  Abstract:	Decode a Floatin Point Operate Instruction opcode
 *		(SRM 3.3.4)
 *
 */
void
decode_fop(CONTEXT *ctx) {

  uint32 fr[3], j = 0, func, tflags = 0xFFFF, tmp;
  char *func_name = opcode_names[ctx->opcode];
  char buf0[8] = {0}, buf1[8] = {0}, buf2[8] = {0}, *f[3];
  uint32 data = *ctx->current_address;
  DECOPFUNC *decode_opfunction = decode_ops[ctx->opcode];

  f[0] = &buf0[0];
  f[1] = &buf1[0];
  f[2] = &buf2[0];

  fr[0]   = (data >> 21) & 0x1F;
  fr[1]   = (data >> 16) & 0x1F;
  fr[2]   = data & 0x1F;
  func    = (data >>  5) & 0x07FF;

  while (decode_opfunction[j].function != -1) {
   if (decode_opfunction[j].function == func) {
     func_name = decode_opfunction[j].name;
     tflags = decode_opfunction[j].flags;
     break;
   }
   j += 1;
  }

  for (j = 0; j < 3; j += 1) {
    tmp = (tflags >> j) & 0xFF;
    if (!tmp) {
      if (tmp == 1) sprintf(f[j], "F%d, ", fr[j]);
      if (tmp == 2) sprintf(f[j], "R%d, ", fr[j]);
    }
  }

 /*
  *  So far, I've never seen a floating point op inline that wasn't a garbage
  *  sequence.  Only do this if being aggressive.
  *
  */
  if ((ctx->command & CHECK_LDL) && (!(ctx->command & AGGRESSIVE)) && (ctx->current_context)) {
     ctx->flags |= NOT_CODE;
     ctx->current_context = 0;
  }

 /*
  *  If we are looking for valid LDL/STC sequences, then see if we have a floating point operation
  *  in the fall through path between the LDL/STC
  *
  */
  if ((ctx->command & CHECK_LDL) && (ctx->command & VERBOSE) && (ctx->current_context)) {

   /*
    *  Dump the sequence if it had no unknown instructions, or if verbose was set
    */
    if ((!(ctx->flags & NOT_CODE)) || (ctx->command & VERBOSE)) {

     /*
      *  Print the sequence - unless we already did it.
      */
      if (!(ctx->flags & BAD_LOCK_SEQ)) {

        DISPLAY_FIRST_MESSAGE();
        fprintf( ctx->ofile, "** Found an unexpected %s at %8.8X\n", opcode_names[ctx->opcode], ctx->addr);

        ctx->error_count += 1;
        dump_sequence(ctx);
      }
      ctx->flags |= BAD_LOCK_SEQ;
    }

   /*
    *  Skip the rest of the sequence
    */
    ctx->error = FOUND_PROBLEM;
    ctx->error_address = ctx->current_address;
  }


  if (ctx->command & DUMP) {
    if (tflags == 0xFFFF) {
      fprintf( ctx->ofile, "%8.8X   %8.8X     %s\n", ctx->addr, *ctx->current_address, opcode_names[ctx->opcode]);
    }
    else {
      fprintf( ctx->ofile, "%8.8X   %8.8X     %-14s %s%s%s\n", ctx->addr, data, func_name, f[0],f[1],f[2]);
    }
  }
}

/*
 *  Routine:	decode_mbr
 *
 *  Abstract:	Decode a Memory Branch Insttruction opcode (SRM 3.3.1.2)
 *
 */
void
decode_mbr(CONTEXT *ctx) {

  uint32 ra = 0, rb = 0, j = 0, func, *naddr;
  int32  disp = 0;
  char *func_name = opcode_names[ctx->opcode];
  uint32 data = *ctx->current_address;
  DECOPFUNC *decode_opfunction = decode_ops[ctx->opcode];

  ra   = (data >> 21) & 0x1F;
  rb   = (data >> 16) & 0x1F;
  func = (data >> 14) & 0x03;

 /*
  *  Branch prediction value, low 14 bits.
  *
  */
  disp = data & 0x3FFF;
  if (data & 0x2000){
    disp = data | 0xFFFFC000;
  }
  naddr = ctx->addr + (1+disp);

  while (decode_opfunction[j].function != -1) {
   if (decode_opfunction[j].function == func) {
     func_name = decode_opfunction[j].name;
     break;
   }
   j += 1;
  }

 /*
  *  If we are looking for valid LDL/STC sequences, then see if we have a JSR/JMP
  *  in the fall through path between the LDL/STC
  *
  */
  if ((ctx->command & CHECK_LDL) && (ctx->current_context)) {

   /*
    *  Dump the sequence if it had no unknown instructions, or if verbose was set
    */
    if ((!(ctx->flags & NOT_CODE)) || (ctx->command & VERBOSE)) {

     /*
      *  Print the sequence - unless we already did it.
      */
      if (!(ctx->flags & BAD_LOCK_SEQ)) {

        DISPLAY_FIRST_MESSAGE();
        fprintf( ctx->ofile, "** Found an unexpected %s at %8.8X\n", opcode_names[ctx->opcode], ctx->addr);

        ctx->error_count += 1;
        dump_sequence(ctx);
      }
      ctx->flags |= BAD_LOCK_SEQ;
    }

   /*
    *  Skip the rest of the sequence
    */
    ctx->error = FOUND_PROBLEM;
    ctx->error_address = ctx->current_address;
  }

  if (func < 2) {
    if (ctx->command & DUMP) fprintf( ctx->ofile, "%8.8X   %8.8X     %-14s R%d, (R%d), 0x%X\n", ctx->addr, data, func_name, ra, rb, naddr);
  }
  else {
    if (ctx->command & DUMP) fprintf( ctx->ofile, "%8.8X   %8.8X     %-14s R%d, (R%d)\n", ctx->addr, data, func_name, ra, rb);
  }
}

/*
 *  Routine:	print_help
 *
 *  Abstract:	Give the user some information about the application in
 *		response to the -help option.
 *
 *  Inputs:	none
 *
 *  Outputs:	none
 *
 */
static void
print_help() {

  printf("  SRM_CHECK\n");
  printf("\n");
  printf("  This application is provided to assist in locating illegal\n");
  printf("  lock sequences in a binary image.  In particular, it looks\n");
  printf("  for:\n");
  printf("\n");
  printf("     1) Instructions that should not be placed between a load\n");
  printf("        locked, and store conditional.  In specific, memory\n");
  printf("        fetch or stores.\n");
  printf("\n");
  printf("     2) Taken branches between a LDx_L and STx_C.  In simple\n");
  printf("        terms - the STx_C must be fallen through to from the\n");
  printf("        LDx_L and cannot be branched to.\n");
  printf("\n");
  printf("     3) Other degenerate cases - such as a branch into a LDx_L\n");
  printf("        STx_C sequence.\n");
  printf("\n");
  printf("  These sequences could have been generated by old compliers\n");
  printf("  (BLISS in particular), rare occurances in MACRO32, and ASM\n");
  printf("  code in C/C++.  Previous Alpha processors tolerated these\n");
  printf("  sequences, although they are violations of the Alpha Architecture.\n");
  printf("  But starting with the 21264 (EV6), these sequences may fail, \n");
  printf("  and in future processors will always fail.\n");
  printf("\n");
  printf("  More detailed specifics on proper LDx_C/STxC use can be\n");
  printf("  found in the Alpha Architecture Reference Manual (Third\n");
  printf("  Edition, ISBN 1-55558-202-8) in Chapter 5.5, Appendix A,\n");
  printf("  and in the descriptions of LDx_L and STxC.\n");
  printf("\n");
  printf("  To run the tool, define the image as a foreign command,\n");
  printf("  and provide an input file specification (for example):\n");
  printf("\n");
  printf("     $ DEF DCL$PATH sys$system, []\n");
  printf("     $ srm_check myimage.exe\n");
  printf("\n");
  printf("  If this tool produces a message, it will display the virtual\n");
  printf("  address in the image of the failing code.  This address can\n");
  printf("  be used to locate the module in the MAP file for the image.\n");
  printf("\n");
  printf("  The normal response to an error is to recompile, and re-run\n");
  printf("  the tool.  In some rare cases, the source code may need to be\n");
  printf("  modified to resolve the problem.\n");
  printf("\n");
  printf("  NOTE:  Often it is not possible to distinguish between code\n");
  printf("  in an image file, and data which has been given the EXE\n");
  printf("  attribute in its PSECT.  This typically happens when a compiler\n");
  printf("  or programmer is trying to allow address fixups for non-code\n");
  printf("  (very often you see this for BLISS PLITs).  The tool attempts\n");
  printf("  to filter suspected data disguised as code - but is not always\n");
  printf("  successful.  Many times its obvious to a human looking at the\n");
  printf("  displayed code, but often only looking at the MAP or LIS file\n");
  printf("  can resolve this.\n");
  printf("\n");
  printf("  The tool also provides the ability to selectively dump portions\n");
  printf("  of the image using a -dump and -start xx -end xx directives.\n");
  printf("  This can be handy to look at the code near and around the flagged\n");
  printf("  sequences.\n");
  printf("\n");
  printf("  Other command options are available (use: srm_check -? for a list)\n");
  printf("  The default settings are typically the best to stay with unless asked\n");
  printf("  to do otherwise.\n");
  printf("\n");
  printf("  Wildcards are allowed to search multiple files, the -log\n");
  printf("  qualifier can be used to print out the name of each file as\n");
  printf("  it is checked.  For example:\n");
  printf("\n");
  printf("     $ srm_check [*...] -output check.dat -log -max_error 10\n");
  printf("\n");
  printf("  will check all the files in [*...]*.exe and place the results\n");
  printf("  into check.dat in the current directory.  It will log a message\n");
  printf("  for each file checked, and it will stop displaying errors if there\n");
  printf("  are more than 10 in a particular image section.\n");
  printf("\n");
  printf("  If you use the output from this tool in communications with\n");
  printf("  Compaq, please also use the -version switch to provide us with\n");
  printf("  information on the version of the tool used.\n");
  printf("\n");
}

/*
 *  Routine:	parse_command_line
 *
 *  Abstract:	Parse the command line options, and fill in the
 *		context block with information from the command line.
 *
 *  Inputs:	contect block (argc and argv are passed in the block)
 *
 *  Outputs:	A course success/fail code.
 *
 */
static int
parse_command_line(CONTEXT *ctx) {

  int i, j;
  char appndo[2] = {'w',0},  *dummy; 

  typedef struct cmd_names {
    int   value;
    char *name;
    int   match;
  } CMD_NAMES;

  enum {
    cmdCHECK_LDL,
    cmdVERBOSE,
    cmdAGGRESSIVE,
    cmdVERSION,
    cmdMAX_ERRORS,
    cmdOUTPUT,
    cmdINPUT,
    cmdAPPEND,
    cmdDUMP,
    cmdSTART_VA,
    cmdEND_VA,
    cmdLOG,
    cmdSEQCNT,
    cmdSEQDUMP,
    cmdNOBRANCHES,
    cmdHELP,
    cmdUSAGE,
    cmdUNKNOWN,
    cmdEOT
  };

 static CMD_NAMES commands[] = {
    cmdCHECK_LDL,       "-check_ldl",  2,
    cmdVERBOSE,         "-verbose",    5,
    cmdAGGRESSIVE,      "-aggressive", 3,
    cmdVERSION,         "-version",    5,
    cmdMAX_ERRORS,      "-max_errors", 6,
    cmdOUTPUT,          "-output",     2,
    cmdINPUT,           "-input",      2,
    cmdAPPEND,          "-append",     3,
    cmdDUMP,            "-dump",       2,
    cmdSTART_VA,        "-start_va",   3,
    cmdEND_VA,          "-end_va",     2,
    cmdLOG,             "-log",        2,
    cmdSEQCNT,          "-seqcnt",     5,
    cmdSEQDUMP,         "-seqdump",    5,
    cmdNOBRANCHES,      "-nobranches", 4,
    cmdHELP,            "-help",       2,
    cmdUSAGE,           "-?",          2,
    cmdUSAGE,           "?",           1,
    cmdUNKNOWN,         "-",           1,
    cmdEOT,             "end_of_table",0
  };

 /*
  *  Init defaults
  *
  */
  ctx->command    = CHECK_LDL | FOLLOW_BRANCHES;
  ctx->max_errors = MAX_ERRORS;
  ctx->start_va   = 0;
  ctx->end_va     = 0;
  ctx->ifile_arg  = 0;
  ctx->ofile_arg  = 0;
  ctx->seq_cnt    = 0;
  ctx->seq_avl    = 0;
  ctx->seq_map    = 0;

 /*
  *  No command!
  *
  */
  if (ctx->argc < 2) {
    usage( ctx, ERROR_USAGE);
    return (ERROR_STATUS);
  }

 /*
  *  Parse the command line.  Pretty gross inline brute force.
  *
  */
  for (i = 1; i < ctx->argc; i += 1) {

    j = 0;
    while (commands[j].value != cmdEOT) {
      if (strncmp(ctx->argv[i], commands[j].name, commands[j].match) == 0) {
        break;
      }
      j += 1;
    }

    switch (commands[j].value) {

      case cmdCHECK_LDL:

        ctx->command = CHECK_LDL;
        break;

      case cmdVERBOSE:

        ctx->command |= VERBOSE | LOG_FILES | VERSION_PRINT;
        break;

      case cmdAGGRESSIVE:

        ctx->command |= AGGRESSIVE;
        break;

      case cmdVERSION:

        ctx->command |= VERSION_PRINT;
        break;

      case cmdLOG:

        ctx->command |= LOG_FILES;
        break;

      case cmdMAX_ERRORS:

        i += 1;
        if (i >= ctx->argc) {
          usage( ctx, ERROR_NOMAXERR);
          return (ERROR_STATUS);
        }
        ctx->max_errors = strtoul(ctx->argv[i], &dummy, 0);
        break;

      case cmdOUTPUT:

        i += 1;
        if (i >= ctx->argc) {
          usage( ctx, ERROR_NOOUTPUTFILE);
          return (ERROR_STATUS);
        }
        ctx->ofile_arg = i;
        break;

      case cmdAPPEND:

        appndo[0] = 'a';
        break;

      case cmdDUMP:

        ctx->command = DUMP;
        break;

      case cmdSTART_VA:

        i += 1;
        if (i >= ctx->argc) {
          usage( ctx, ERROR_STARTVA);
          return (ERROR_STATUS);
        }
        ctx->start_va = (uint32 *) (strtoul(ctx->argv[i], &dummy, 16) & ~3);
        break;

      case cmdEND_VA:

        i += 1;
        if (i >= ctx->argc) {
          usage( ctx, ERROR_ENDVA);
          return (ERROR_STATUS);
        }
        ctx->end_va = (uint32 *) ((strtoul(ctx->argv[i], &dummy, 16) + 3) & ~3);
        break;

      case cmdSEQCNT:

        i += 1;
        if (i >= ctx->argc) {
          usage( ctx, ERROR_SEQCNT);
          return (ERROR_STATUS);
        }
        ctx->seq_avl = (uint32) strtoul(ctx->argv[i], &dummy, 0) + 1;

        break;

      case cmdSEQDUMP:

        ctx->command |= DUMP_ALL_SEQUENCES;
        break;

      case cmdNOBRANCHES:

        ctx->command &= ~FOLLOW_BRANCHES;
        break;

      case cmdHELP:

	print_help();
        return (ERROR_STATUS);
        break;

      case cmdUSAGE:

        usage( ctx, ERROR_USAGE);
        return (ERROR_STATUS);
        break;

      case cmdUNKNOWN:
        usage( ctx, i);
        return (ERROR_STATUS);
        break;

      case cmdINPUT:

        i += 1;
        if (i >= ctx->argc) {
          usage( ctx, ERROR_NOINPUT);
          return (ERROR_STATUS);
        }

      case cmdEOT:

        ctx->ifile_arg = i;
        break;
    }
  }

 /*
  *  No output file - open SYS$OUTPUT
  *
  */
  if (ctx->ofile_arg == 0) {
    ctx->ofile = fopen("sys$output:", &appndo[0]);
    if (!ctx->ofile) {
      printf("Could not open sys$output:\n");
      return (ERROR_STATUS);
    }
  }
  else {
   /*
    *  Open the specified output file
    *
    */
    ctx->ofile = fopen(ctx->argv[ctx->ofile_arg], &appndo[0]);
    if (!ctx->ofile) {
      printf("Could not open output file %s\n", ctx->argv[ctx->ofile_arg]);
      return (ERROR_STATUS);
    }
  }

 /*
  *  Print version
  *
  */
  if (ctx->command & VERSION_PRINT) {
    fprintf( ctx->ofile, "SRM_CHECK Version %s\n", IDENT);
  }

 /*
  *  Check for no input file
  *
  */
  if (ctx->ifile_arg == 0) {
    if (ctx->command & VERSION_PRINT) return (SUCCESS);
    usage( ctx, ERROR_NOINPUT);
    return (ERROR_STATUS);
  }

 /*
  *  Make sure that if there is a end_va - that it is
  *  bigger than the end va!
  *
  */
  if ((ctx->end_va != 0) && (ctx->start_va > ctx->end_va)) {
    usage( ctx, ERROR_ENDTOOSMALL);
    return (ERROR_STATUS);
  }

 /*
  *  Allocate memory for LDL sequence map
  */
  if (ctx->command & CHECK_LDL) {
    ctx->seq_cnt = 0;
    if (!ctx->seq_avl) ctx->seq_avl = MAX_SEQUENCE + 1;
    ctx->seq_map = (SEQ *) malloc(ctx->seq_avl * sizeof(SEQ));
  }

  return (SUCCESS);
}

/*
 *  Routine:	usage
 *
 *  Abstract:	Print a usage message in response to a bad option,
 *		or to a -? or ? on the command line.
 *
 *  Inputs:	context block, error code (if > 1, it's the bad
 *		argument number).
 *
 *  Outputs:	none
 *
 */
static void
usage( CONTEXT *ctx, int what) {

  switch (what) {
    case ERROR_USAGE:
      break;
    case ERROR_NOINPUT:
      printf("** No input filename supplied.\n\n");
      break;
    case ERROR_NOOUTPUTFILE:
      printf("** Missing filename after -output.\n\n");
      break;
    case ERROR_NOMAXERR:
      printf("** Missing error max after -max_errors.\n\n");
      break;
    case ERROR_STARTVA:
      printf("** Missing VA after -start_va.\n\n");
      break;
    case ERROR_ENDVA:
      printf("** Missing VA after -end_va.\n\n");
      break;
    case ERROR_ENDTOOSMALL:
      printf("** The -end_va must be larger than the -start_va.\n\n");
      break;
    case ERROR_SEQCNT:
      printf("** Missing arg after -seqcnt.\n\n");
      break;
    default:
      printf("** Unknown argument %s\n\n", ctx->argv[what]);
      break;
  }

  printf( "Usage:   srm_check filename\n\n");
  printf( "options:    -check_ldl          Check for LDL_L/ST_C correctness: DEFAULT\n");
  printf( "            -verbose            Do not supress any output (displays lots of false and duplicate errors)\n");
  printf( "            -aggressive         Do aggressive checking (finds lots of false and duplicate errors)\n");
  printf( "            -max_errors <n>     Max errors before skipping image section (default %d)\n", MAX_ERRORS);
  printf( "            -output filename    Write output to file instead of terminal\n");
  printf( "            -input filename     Input file\n");
  printf( "            -append             Append output to file\n");
  printf( "            -dump               Do a formatted image dump instead of check_ldl\n");
  printf( "            -start <va>         Start at this VA\n");
  printf( "            -end <va>           End at this VA\n");
  printf( "            -log                Display name of file being checked\n");
  printf( "            -version            Print srm_check version\n");
  printf( "            -nobranch           Don't trace branches\n");
  printf( "            -seqdump            Display all ldx_l/stx_c sequences\n");
  printf( "            -seqcnt <n>         Allocation count for branch checks\n");
  printf( "            -help               Display info on srm_check use\n");

}
