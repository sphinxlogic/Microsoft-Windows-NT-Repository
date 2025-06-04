#include <ssdef.h>
#include <stdio.h>

#ifdef __DECC
# pragma message disable (VARIANTEXT,GLOBALEXT,ADDRCONSTEXT,NEEDCONSTEXT)
#endif

#include "string_def.h"

/* Okay, it's time for the grand rewrite of VERB. Eeeha.

   This file was converted (by hand) from the microfiche CLITABDEF.SDL
   in the DCL facility.

	A CLI table contains all of the information that DCL and MCR need
 to parse DCL commands. The tables are composed of a set of blocks,
 each of which describes one or more command items. This header file
 defines all of the blocks.

 Each block begins with a standard header, which is formatted as follows:

		+------------------------------+
		| subtype | type |    length   |
		+------------------------------+
		| TRO Count      |    Flags    |
		+------------------------------+

 All references to other blocks are made via Table-Relative Offsets (TRO).
 The TRO count specifies how many such references there are, and the
 reference longwords always follow the header immediately. The rest of
 each block contains other information necessary for the definition of
 the item. Following the fixed portion of the block is a variable
 portion, which contains any variable-length strings. Each of these
 strings is references from the fixed portion of the block by a
 word Block-Relative Offset (BRO).

*/

/* The following list defines all of the valid block types */
#define BLOCK_K_VECTOR       1
#define BLOCK_K_COMMAND      2
#define BLOCK_K_TYPE         3
#define BLOCK_K_ENTITY       4
#define BLOCK_K_EXPRESSION   5
#define BLOCK_K_CDU_VISITED  6

/* the following pages define the various block formats. Many of the field
   names are wierd, but have been retained for compatability with the
   previous block formats.
*/

/* VECTOR BLOCKS
**
** The primary vector block appears at the beginning of a CLI table,
** and contains references to all other blocks and block lists
*/
typedef struct vector_block
{
  unsigned short vec_w_size;
  unsigned char vec_b_type;
  unsigned char vec_b_subtype;
#define VEC_K_DCL      	1
#define VEC_K_MCR      	2
#define VEC_K_VERB     	3
#define VEC_K_COMMAND	4
  variant_union
  {
    unsigned short vec_w_flags;
    unsigned char vec_b_strlvl;
#define VEC_K_STRLVL	6
  } vec_r_flags_overlay;
  unsigned short vec_w_tro_count;
#define VEC_K_HEADER_LENGTH	8
  unsigned long vec_l_verbtbl;		/* TRO of verb name table */
  unsigned long vec_l_comdptr;		/* TRO of command block pointer table */
  unsigned long vec_l_table_size;	/* overall size of CLI Table */
} VectorBlock;
#define VEC_K_LENGTH 20

/* The verb name table is composed of the standard header, followed by one
** longword for each verb or synonym. The longword contains the first four
** characters of the verb name, padded with NULLs if necessary.
*/

typedef struct verb_table_block	/* this is a home spun block, not from .SDL */
{
  unsigned short verb_w_size; /* size of this block, including fixed header */
  unsigned char  verb_b_type;
  unsigned char  verb_b_subtype;
  unsigned short verb_w_flags;
  unsigned short verb_w_tro_count;
  unsigned long  verb_l_name[MAXARRAY];/* 4 byte verb name (null padded if needed) */
} VerbTableBlock;
#define VERB_K_HEADER_LENGTH 8

typedef struct command_table_block	/* this is a home spun block, not from .SDL */
{
  unsigned short comd_w_size;
  unsigned char  comd_b_type;
  unsigned char  comd_b_subtype;
  unsigned short comd_w_flags;
  unsigned short comd_w_tro_count;
  unsigned long  comd_l_cmdtro[MAXARRAY];	/* TRO of command block */
} CommandTableBlock;

/* The command block pointer table is composed of the standard header,
** followed by one longword for each entry in the verb table. This
** longword contains the TRO of the corresponding command block.
*/

/* COMMAND BLOCK
**
** A command block is used to define a verb or a syntax change brought about
** by a parameter or qualifier. There is one command block for each verb
** (but not for its synonyms), and one for each syntax change within a verb.
*/

#define CMD_M_ABBREV	0x00000001
#define CMD_M_NOSTAT 	0x00000002
#define CMD_M_FOREIGN 	0x00000004
#define CMD_M_IMMED 	0x00000008
#define CMD_M_MCRPARSE 	0x00000010
#define CMD_M_PARMS 	0x00000020
#define CMD_M_QUALS 	0x00000040
#define CMD_M_DISALLOWS	0x00000080
#define CMD_S_MAXPARM 	0x00000004
#define CMD_S_MINPARM 	0x00000004

typedef struct command_block
{
  unsigned short cmd_w_size;
  unsigned char cmd_b_type;
  unsigned char cmd_b_subtype;
#define CMD_K_VERB	1
#define CMD_K_SYNTAX	2
  variant_union
  {
    unsigned short cmd_w_flags;
    variant_struct
    {
      unsigned cmd_v_abbrev:1;	/* verb may be abbreviated non-uniquely */
      unsigned cmd_v_nostat:1;	/* command does not return a status */
      unsigned cmd_v_foreign:1;	/* command requests unparsed line */
      unsigned cmd_v_immed:1;	/* immediate command, uses internal parsing */
      unsigned cmd_v_mcrparse:1;/* MCR style (output = input) */
      unsigned cmd_v_parms:1;	/* parameter info is relavent */
      unsigned cmd_v_quals:1;	/* qualifier info is relavent */
      unsigned cmd_v_disallows:1;/* Disallow info is relavent */
    } cmd_r_flags_bits;
  } cmd_r_flags_overlay;
  unsigned short cmd_w_tro_count;
  unsigned long cmd_l_parms;	/* TRO of first parameter entity block */
#define CMD_K_MAX_PARMS 8
  unsigned long cmd_l_quals;	/* TRO of first qualifier */
  unsigned long cmd_l_disallow;	/* TRO of top-level disallow */
  unsigned char cmd_b_handler;	/* How does CLI handle command? */
#define CMD_K_NONE	0	/* it doesn't */
#define CMD_K_CLI	1	/* calls a CLI routine */
#define CMD_K_USER	2	/* calls a user routine */
#define CMD_K_IMAGE	3	/* invokes an image */
#define CMD_K_SAME	4	/* for syntax change, same handling as verb */
  variant_union
  {
    unsigned char cmd_b_parmcnt;
    variant_struct
    {
      unsigned cmd_v_minparm:CMD_S_MINPARM;	/* min required params */
      unsigned cmd_v_maxparm:CMD_S_MAXPARM;	/* max required params */
    } cmd_r_parmcnt_bits;
  } cmd_r_parmcnt_overlay;
  unsigned char cmd_b_verbtyp;	/* verb type code for use with old CLI interface */
  unsigned char cmd_b_pad1;
  unsigned short cmd_w_name;	/* BRO of verb or syntax name */
  unsigned short cmd_w_image;	/* BRO of routine or image reference */
  unsigned short cmd_w_outputs;	/* BRO of outputs list */
  unsigned short cmd_w_prefix;	/* BRO of prefix string */
  char cmd_z_variable[MAXARRAY];	/* beginning of variable part */
} CommandBlock;
#define CMD_K_LENGTH 		0x00000020
#define CMD_K_MAX_NAME 		0x00000020
#define CMD_K_MAX_IMAGE 	0x00000040
#define CMD_K_MAX_OUTPUTS 	0x00000008
#define CMD_K_MAX_PREFIX 	0x00000020

/* following the fixed portion, the verb name(s) are stored as a sequence
** of ASCIC strings within an overall ASCIC string. Or, the syntax name is
** stored as a single ASCIC string.
**
** The routine or image reference is stored as follows:
**	CLI Routine	Routine name as ASCIC string
**	user routine	Longword routine address, then name as ASCIC string
**	image		Image specification as ASCIC string
**
** The outputs list consists of a counted sequence of bytes. Each byte
** contains either the negative of the parameter number, or the qualifier
** number.
**
** The prefix string is stored as an ASCIC string
*/


/* TYPE BLOCK
**
** A type block is used as the header of a chain of entity blocks that
** describe type keywords. There is one type block for each user-specified
** type definition.
*/

typedef struct type_block
{
  unsigned short type_w_size;
  unsigned char type_b_type;
  unsigned char type_b_subtype;
#define TYPE_K_TYPE	1
  unsigned short type_w_flags;
  unsigned short type_w_tro_count;
  unsigned long type_l_keywords;	/* TRO of first keyword entity block */
  unsigned short type_w_name;		/* BRO of type name */
  unsigned short type_w_prefix;		/* BRO of prefix string */
  char type_z_variable[MAXARRAY];		/* beginning of variable part */
} TypeBlock;
#define TYPE_K_LENGTH		16
#define TYPE_K_MAX_NAME		32
#define TYPE_K_MAX_PREFIX	32

/* following the fixed portion, the type name is stored as an ASCIC string.
** So is the prefix string.
*/

/* ENTITY BLOCK
**
** An entity block is used to define each parameter, qualifier, and data
** type keyword. These blocks are linked off of the command block for
** the verb, in the case of parameters and qualifiers, or off of a type
** block, in the case of type keywords.
*/

#define ENT_M_VAL                        0x00000001
#define ENT_M_NEG                        0x00000002
#define ENT_M_DEFTRUE                    0x00000004
#define ENT_M_BATDEF                     0x00000008
#define ENT_M_VALREQ                     0x00000010
#define ENT_M_LIST                       0x00000020
#define ENT_M_CONCAT                     0x00000040
#define ENT_M_IMPCAT                     0x00000080
#define ENT_M_VERB                       0x00000100
#define ENT_M_PARM                       0x00000200
#define ENT_M_MCROPTDELIM                0x00000400
#define ENT_M_MCRIGNORE                  0x00000800

typedef struct entity_block
{
  unsigned short ent_w_size;
  unsigned char ent_b_type;
  unsigned char ent_b_subtype;
#define ENT_K_PARAMETER	1
#define ENT_K_QUALIFIER	2
#define ENT_K_KEYWORD	3
  variant_union
  {
    unsigned short ent_w_flags;
    variant_struct
    {
      unsigned ent_v_val:1;	/* can take a value */
      unsigned ent_v_neg:1;	/* can be negated with NO */
      unsigned ent_v_deftrue:1;	/* present by default */
      unsigned ent_v_batdef:1;	/* present by default if batch */
      unsigned ent_v_valreq:1;	/* a value is required */
      unsigned ent_v_list:1;	/* can be a list of values */
      unsigned ent_v_concat:1;	/* can be concatenated list */
      unsigned ent_v_impcat:1;	/* implicit concatenated list (old CLI only) */
      unsigned ent_v_verb:1;	/* global placement */
      unsigned ent_v_parm:1;	/* local placement */ /* both means positional */
      unsigned ent_v_mcroptdelim:1;	/* MCR SET UIC Kludge */
      unsigned ent_v_mcrignore:1;	/* MCR Ignores this entity */
    } ent_r_flags_bits;
  } ent_r_flags_overlay;
  unsigned short ent_w_tro_count;
  unsigned long ent_l_next;	/* TRO of next entity block in chain */
  unsigned long ent_l_syntax;	/* TRO of syntax change command block */
  unsigned long ent_l_user_type;/* TRO of type block for user-defined types */
  unsigned char ent_b_number;	/* entity number, CLI should only use for parameters */
  unsigned char ent_b_valtype;
#define ENT_K_USER_DEFINED	0
#define ENT_K_INFILE            1
#define ENT_K_OUTFILE           2
#define ENT_K_NUMBER            3
#define ENT_K_PRIVILEGE         4
#define ENT_K_DATETIME          5
#define ENT_K_PROTECTION        6
#define ENT_K_PROCESS           7
#define ENT_K_INLOG             8
#define ENT_K_OUTLOG            9
#define ENT_K_INSYM             10
#define ENT_K_OUTSYM            11
#define ENT_K_NODE              12
#define ENT_K_DEVICE            13
#define ENT_K_DIR               14
#define ENT_K_UIC               15
#define ENT_K_RESTOFLINE        16
#define ENT_K_PARENVALUE        17
#define ENT_K_DELTATIME         18
#define ENT_K_QUOTEDSTRING      19
#define ENT_K_FILE              20
#define ENT_K_EXPRESSION        21
#define ENT_K_TEST1             22
#define ENT_K_TEST2             23
#define ENT_K_TEST3             24
#define ENT_K_ACL               25
#define ENT_K_MAX_VALTYPE	25
  unsigned short ent_w_name;
  unsigned short ent_w_label;
  unsigned short ent_w_prompt;
  unsigned short ent_w_defval;
  char ent_z_variable[MAXARRAY];
} EntityBlock;
#define ENT_K_LENGTH            0x0000001E

#define ENT_K_MAX_NAME          32
#define ENT_K_MAX_LABEL         32
#define ENT_K_MAX_PROMPT        32
#define ENT_K_MAX_DEFVAL        96

/* The entity name, label, prompt, and default values appear after the fixed
** portion of the entity block. They are stored as ASCIC strings. The
** defaut values are stored as a sequence of ASCIC strings within the
** overall ASCIC string.
*/


/* EXPRESSION BLOCK
**
** An expression block is used to represent, within a boolean expression,
** one operator and its operands. The operands are themselves expression
** blocks, either subexpressions or paths. Paths represent the hierarchical
** path to an entity whose presence is to be determined.
*/

typedef struct expression_block
{
  unsigned short exp_w_size;
  unsigned char exp_b_type;
  unsigned char exp_b_subtype;
#define EXP_K_PATH	1
#define EXP_K_NOT       2
#define EXP_K_ANY2      3
#define EXP_K_AND       4
#define EXP_K_OR        5
#define EXP_K_XOR       6
#define EXP_K_NEG       7
  unsigned short exp_w_flags;
  unsigned short exp_w_tro_count;
  unsigned long exp_l_operand_list[MAXARRAY];	/* a TRO for each of the operands or path entities. */
} ExpressionBlock;
#define EXP_K_LENGTH		8
#define EXP_K_MAX_PATH_ENTITIES	8	/* max number of entities in a path */


#define TRO(table,offset) ((offset)?((void *)((char *)(table) + (offset))):0)
#define BRO(block,offset) TRO(block,offset)
