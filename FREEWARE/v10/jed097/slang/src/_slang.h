/* header file for S-Lang internal structures that users do not (should not)
   need.  Use slang.h for that purpose. */
/* 
 * Copyright (c) 1992, 1994 John E. Davis 
 * All rights reserved.
 *
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, and distribute this
 * software and its documentation for any purpose, provided that the
 * above copyright notice and the following two paragraphs appear in
 * all copies of this software.
 *
 * IN NO EVENT SHALL JOHN E. DAVIS BE LIABLE TO ANY PARTY FOR DIRECT,
 * INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF
 * THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF JOHN E. DAVIS
 * HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * JOHN E. DAVIS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS"
 * BASIS, AND JOHN E. DAVIS HAS NO OBLIGATION TO PROVIDE MAINTENANCE,
 * SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 */


#include "config.h"
#ifndef NO_STDLIB_H
# include <stdlib.h>
#endif

#include <string.h>


#ifdef __GO32__
#  define SLANG_SYSTEM_NAME "_IBMPC"
#else
# if defined (msdos) || defined (__os2__)
#  define SLANG_SYSTEM_NAME "_IBMPC"
# else
#  ifdef VMS
#    define SLANG_SYSTEM_NAME "_VMS"
#  else
#    define SLANG_SYSTEM_NAME "_UNIX"
#  endif
# endif
#endif



#define SLANG_MAX_SYMBOLS 500
/* maximum number of global symbols--- slang builtin, functions, global vars */

/* Subtypes */
#define ERROR_BLOCK	0x01
/* gets executed if block encounters error other than stack related */
#define EXIT_BLOCK	0x02
#define USER_BLOCK0	0x03
#define USER_BLOCK1	0x04
#define USER_BLOCK2	0x05
#define USER_BLOCK3	0x06
#define USER_BLOCK4	0x07
/* The user blocks MUST be in the above order */

/* directive subtypes */
#define SLANG_LOOP_MASK	0x80
#define SLANG_LOOP	0x81
#define SLANG_WHILE	0x82
#define SLANG_FOR	0x83
#define SLANG_FOREVER	0x84
#define SLANG_CFOR	0x85
#define SLANG_DOWHILE	0x86

#define SLANG_IF_MASK	0x40
#define SLANG_IF		0x41
#define SLANG_IFNOT	0x42
#define SLANG_ELSE	0x43


/* local, global variable assignments
 * The order here is important.  See interp_variable_eqs to see how this
 * is exploited. */
#define SLANG_EQS_MASK	0x20
/* local variables */
/* Keep these in this order!! */
#define SLANG_LEQS	0x21
#define SLANG_LPEQS	0x22
#define SLANG_LMEQS	0x23
#define SLANG_LPP	0x24
#define SLANG_LMM	0x25
/* globals */
/* Keep this on this order!! */
#define SLANG_GEQS	0x26
#define SLANG_GPEQS	0x27
#define SLANG_GMEQS	0x28
#define SLANG_GPP	0x29
#define SLANG_GMM	0x2A
/* intrinsic variables */
#define SLANG_IEQS	0x2B
#define SLANG_IPEQS	0x2C
#define SLANG_IMEQS	0x2D
#define SLANG_IPP	0x2E
#define SLANG_IMM	0x2F


#define SLANG_ELSE_MASK	0x10
#define SLANG_ANDELSE	0x11
#define SLANG_ORELSE	0x12
#define SLANG_SWITCH	0x13

/* LOGICAL SUBTYPES (operate on integers) */
#define SLANG_MOD	16
#define SLANG_OR	17
#define SLANG_AND	18
#define SLANG_BAND	19
#define SLANG_BOR	20
#define SLANG_BXOR	21
#define SLANG_SHL	22
#define SLANG_SHR	23

/* LUnary Subtypes */
#define SLANG_NOT	24
#define SLANG_BNOT	25

typedef struct SLBlock_Type
  {
     unsigned char main_type;
     unsigned char sub_type;
     union 
       {
	  struct SLBlock_Type *blk;
	  int i_blk;
	  SLang_Name_Type *n_blk;
	  char *s_blk;
	  FLOAT *f_blk;		       /*literal float is a pointer */
	  long l_blk;
       }
     b;
  }
SLBlock_Type;


typedef struct SLang_Object_Type
{
   unsigned char main_type;	       /* block, intrinsic... */
   unsigned char sub_type;	       /* SLANG_WHILE, SLANG_DATA, ... */
   union
     {
	long l_val;
	char *s_val;
	int i_val;
	SLuser_Object_Type *uobj;
	SLang_Name_Type *n_val;
#ifdef FLOAT_TYPE
	FLOAT f_val;
#endif
     } v;
}  SLang_Object_Type;


extern int SLang_pop_non_object (SLang_Object_Type *);

extern void SLcompile(char *);
extern void (*SLcompile_ptr)(char *);

typedef struct Lang_Name2_Type
{
   char *name;  int type;
} Lang_Name2_Type;

extern void SLstupid_hash(void);

typedef struct SLName_Table
{
   struct SLName_Table *next;	       /* next table */
   SLang_Name_Type *table;	       /* pointer to table */
   int n;			       /* entries in this table */
   char name[32];		       /* name of table */
   int ofs[256];		       /* offsets into table */
} SLName_Table;

extern SLName_Table *SLName_Table_Root;
extern SLang_Name_Type SLang_Name_Table[SLANG_MAX_SYMBOLS];

extern int SL_eqs_name(char *, Lang_Name2_Type *);
extern Lang_Name2_Type SL_Binary_Ops [];
extern SLang_Object_Type *SLStack_Pointer;
extern char *SLbyte_compile_name(char *);
extern int SLang_pop(SLang_Object_Type *);
extern char *SLexpand_escaped_char(char *, char *);
extern void SLexpand_escaped_string (char *, char *, char *);

extern SLang_Object_Type *SLreverse_stack(int *);
extern SLang_Name_Type *SLang_locate_name(char *);

/* returns a pointer to a MALLOCED string */
extern char *SLstringize_object (SLang_Object_Type *);

/* array types */
typedef struct SLArray_Type
{
   unsigned char type;		       /* int, float, etc... */
   int dim;			       /* # of dims (max 3) */
   int x,y,z;			       /* actual dims */
   union
     {
	unsigned char *c_ptr;
	unsigned char **s_ptr;
	int *i_ptr;
#ifdef FLOAT_TYPE
	FLOAT *f_ptr;
#endif
	SLuser_Object_Type **u_ptr;
     }
   buf;
   unsigned char flags;		       /* readonly, etc... */
} SLArray_Type;


/* Callback to delete array */
extern void SLarray_free_array (long *);


/* maximum size of run time stack */
#ifdef msdos
#define SLANG_MAX_STACK_LEN 500
#else
#define SLANG_MAX_STACK_LEN 1000
#endif

extern SLang_Object_Type SLRun_Stack[SLANG_MAX_STACK_LEN];
extern SLang_Object_Type *SLStack_Pointer;

extern int SLang_Trace;
extern int SLstack_depth(void);

extern void SLang_trace_fun(char *);
extern void SLexecute_function(SLang_Name_Type *);
extern char *SLmake_string (char *);

/* useful macro to tell if string should be freed after its use. */

/*  Note that strings appear on the stack in 2 forms: literal and
 *  dynamic. Literal strings are constants.  Dynamic ones are created by,
 *  say, dup, etc. They are freed only by routines which eat them.  These
 *  routines must check to see if they are not literal types before freeing
 *  them.  The only other way they are freed is when they are on the local
 *  variable stack, e.g., (assigned to local variables) and the function
 *  exits freeing them.
 */
#define IS_DATA_STRING(obj) (((obj).sub_type == STRING_TYPE) && ((obj).main_type == SLANG_DATA))

extern int slang_eqs_name(char *, Lang_Name2_Type *);
extern void SLang_push(SLang_Object_Type *);
extern void SLang_push_float(FLOAT);
extern void SLadd_variable(char *);
extern int SLatoi(unsigned char *);
extern void SLang_clear_error(void);
extern int SLPreprocess_Only;		        /* preprocess instead of 
						 * bytecompiling
						 */

extern unsigned int SLsys_getkey (void);
extern int SLsys_input_pending (int);
extern unsigned char slang_guess_type (char *);

#ifdef msdos
#define MAX_INPUT_BUFFER_LEN 40
#else
#define MAX_INPUT_BUFFER_LEN 1024
#endif
extern unsigned char SLang_Input_Buffer [MAX_INPUT_BUFFER_LEN];
extern int SLang_Input_Buffer_Len;

#ifndef pc_system
extern char *SLtt_Graphics_Char_Pairs;
#endif
