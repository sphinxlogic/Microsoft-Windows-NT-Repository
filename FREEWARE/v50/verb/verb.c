#define module_name	VERB
#define module_ident	"V2.2"
/* verb.c

	This is the main entry point for the VERB utility.

   V2.2		Hunter Goatley		31-AUG-1995 12:46
	Added bug fix to VERB_ENTITY.C from Derek Dongray.
	Added arbitrary version number to keep track of versions.

   V2.1		Luke Brennan		??-AUG-1995
	Modified to compile with DEC C on VAX; fixed bug.

*/

#ifdef __DECC
#pragma module module_name module_ident
#else
#module module_name module_ident
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lnmdef.h>
#include <psldef.h>
#include <strdef.h>
#include <starlet.h>
#include <lib$routines.h>
#include <str$routines.h>
#include <ots$routines.h>

#include "verb.h"

extern int cli$present();
extern int cli$get_value();

extern int verb_print_command();
extern int init_cli();
extern int load_file();
int check_block();
int do_verbs();
int set_logical();
int print_verb();

#ifndef __GNUC__
  globalvalue VERB_CLD;
#else
  extern VERB_CLD;	/* this is probably wrong? Someone let me know! */
#endif

typedef struct ITEM_LIST_2
{
  unsigned short buflen;
  unsigned short item_code;
  unsigned long *bufadr;
} item_list;

main(int argc, char *argv[])
{
  globalref VectorBlock *ctl$ag_clitable;
  static $DESCRIPTOR(p1,"P1");
  static $DESCRIPTOR(q_all,"ALL");
  static $DESCRIPTOR(q_process,"PROCESS");
  static $DESCRIPTOR(q_table,"TABLE");
  static $DESCRIPTOR(q_output,"OUTPUT");
  static $DESCRIPTOR(q_width,"WIDTH");
  static $DESCRIPTOR(q_image, "IMAGE");
  static $DESCRIPTOR(q_symbol, "SYMBOL");
  static $DESCRIPTOR(q_list, "LIST");
  static $DESCRIPTOR(q_file, "FILE");
  static $DESCRIPTOR(q_count, "COUNT");
  static $DESCRIPTOR(load_image, "VERB_TABLE");
  static $DESCRIPTOR(load_symbol, "DCL$AL_TAB_VEC");
  static DYNAMIC_STRING(result);
  static DYNAMIC_STRING(file);
  static DYNAMIC_STRING(symbol);
  VectorBlock *cmd_table;
  char filename[255];
  int status;

  status = init_cli(VERB_CLD, "VERB");

  if (cli$get_value(&q_output, &result) & 1)
  {
    strncpy(filename, SPTR(result), SLEN(result));
    filename[SLEN(result)] = '\0';

    freopen(filename, "w", stdout, "dna=.CLD", "rfm=var", "rat=cr");
  }

  if (cli$present(&q_table) & 1)
  {
    cli$get_value(&q_table, &result);

    status = set_logical(&load_image, &result);
    if (!(status & 1)) return status;

    status = lib$find_image_symbol(&load_image, &load_symbol, &cmd_table);
    if (!(status & 1)) return status;
  }
  else
  {
    if (cli$present(&q_image) & 1)
    {
      cli$get_value(&q_image, &result);

      status = set_logical(&load_image, &result);
      if (!(status & 1)) return status;

      cli$get_value(&q_symbol, &result);
      status = lib$find_image_symbol(&load_image, &result, &cmd_table);
      if (!(status & 1)) return status;
    }
    else
    {
      if (cli$present(&q_file) & 1)
      {
        int count = 1;

        if (cli$present(&q_count) & 1)
        {
          status = cli$get_value(&q_count, &result);
          if (!(status & 1)) return status;

          status = ots$cvt_ti_l(&result, &count);
          if (!(status & 1)) return status;
        }
        cli$get_value(&q_file, &result);

        status = load_file(&result, &cmd_table, count);
        if (!(status & 1)) return status;
      }
      else
      { /* assume /process */
        cmd_table = ctl$ag_clitable;
      }
    }
  }

  if (cli$present(&q_list) & 1)
  { /* all we need to do is list the commands! */
    if ((cli$present(&q_all) & 1) || (!(cli$present(&p1) & 1)))
    { /* just list all the commands! */
      do_verbs(cmd_table, "*", 1, 1);
    }
    else
    { /* just give 'em what they asked for */
      while(cli$get_value(&p1, &result) & 1)
        do_verbs(cmd_table, SPTR(result), SLEN(result), 1);
    }
  }
  else
  {
    if ((cli$present(&q_all) & 1) || (!(cli$present(&p1) & 1)))
    { /* just display all the commands! */
      do_verbs(cmd_table, "*", 1, 0);
    }
    else
    { /* just give 'em what they asked for */
      while(cli$get_value(&p1, &result) & 1)
        do_verbs(cmd_table, SPTR(result), SLEN(result), 0);
    }
  }
}

int do_verbs(VectorBlock *table, char *name, int len, int listflag)
{
  static CommandBlock **displayed_commands = NULL;
  static int next_saved_command = 0;
  string candidate = {0, DSC$K_DTYPE_T, DSC$K_CLASS_S, 0},
         pattern   = {len, DSC$K_DTYPE_T, DSC$K_CLASS_S, name};
  CommandTableBlock *cmds;
  VerbTableBlock *verbs;
  CommandBlock *cmd;
  int i, verb_count;
  char *command;

  if (table->vec_b_strlvl != VEC_K_STRLVL) return SS$_ABORT; /* sanity check */

  verbs = TRO(table, table->vec_l_verbtbl);
  cmds  = TRO(table, table->vec_l_comdptr);

  verb_count = (verbs->verb_w_size - VERB_K_HEADER_LENGTH) /
               sizeof(verbs->verb_l_name[0]);

  if (displayed_commands == NULL)
  {
    displayed_commands = malloc(sizeof (CommandBlock *) * verb_count);
    if (displayed_commands == NULL) return SS$_INSFMEM;
    next_saved_command = 0;
  }

  for (i = 0; i < verb_count; ++i)
  {
    int overall, start, length, first, match;

    cmd = TRO(table, cmds->comd_l_cmdtro[i]);

    if (cmd->cmd_b_subtype != CMD_K_VERB) return SS$_ABORT; /* sanity check */

    command = BRO(cmd, cmd->cmd_w_name);

    overall = command[0];
    first   = command[1];
    start  = 1;
    match = 0;
    while (!match && (start < overall))
    {
      length = command[start]; /* length portion of ASCIC string */
      ++start;

      if (length)
      {
        /* if pre-fix, then "length" may be truncated to size of verb */
        if (length > first)
        {
          /* verify that there are no count bytes in between (in case it
             was shortened on us), and that the end isn't past the end of
             the overall string */
          int i;
          for (i = start; i < start+length; ++i)
          {
            if ((command[i] < ' ') || i > overall)
            {
                      length = i - start;
            }
          }
        }
        SLEN(candidate) = length;
        SPTR(candidate) = &command[start];
        match = (str$match_wild(&candidate, &pattern) == STR$_MATCH);
        start += length;
      }
    }

    if (match)
    {
      int j;

      for (j = 0; match && (j < next_saved_command); ++j)
      {
        if (displayed_commands[j] == cmd) match = 0;
      }

      if (match)
      {
        displayed_commands[next_saved_command] = cmd;
        ++next_saved_command;
        print_verb(table, cmd, listflag);
      }
    }
  }
}

int print_verb(VectorBlock *table, CommandBlock *cmd, int listflag)
{
  int sts;

  sts = check_block(cmd, BLOCK_K_COMMAND);
  if (!(sts & 1)) return sts;

  if (cmd->cmd_b_subtype != CMD_K_VERB) return SS$_ABORT; /* sanity check */

  if (listflag)
  {
    int overall, start, length, first, truncated;
    char *command;

    command = BRO(cmd, cmd->cmd_w_name);
    /* This is only passed a verb (not a syntax clause) */
    overall = command[0]; /* length portion of "overall" ASCIC string */
    first   = command[1];
    start  = 1;
    while (start < overall)
    {
      truncated = 0;
      length = command[start]; /* length portion of ASCIC string */
      ++start;
      if (length)
      {
        /* if pre-fix, then "length" may be truncated to size of verb */
        if (length > first)
        {
          /* verify that there are no count bytes in between (in case it
             was shortened on us), and that the end isn't past the end of
             the overall string */
          int i;
          for (i = start; i < start+length; ++i)
          {
            if ((command[i] < ' ') || i > overall)
            {
              length = i - start;
              truncated = 1;
            }
          }
        }
        printf("%s%.*s%s", (start > 2)?", ":"",
                           length, &command[start],
                           truncated?" (truncated by CDU)":"");
        start += length;
      }
    }
    printf("\n");
  }
  else
  {
    verb_print_command(table, cmd, cmd);
  }
}

/* the following is sort of a lie, all the different blocks start out
   the same way, so this just uses a command block to check the block type */
check_block(CommandBlock *tst, int block_type)
{
  static char *types[] = {"VECTOR", "COMMAND", "TYPE", "ENTITY", "EXPRESSION"};

  if (tst->cmd_b_type == block_type) return 1;
  if ((tst->cmd_b_type < 1) || (tst->cmd_b_type > 5))
    printf("Illegal block type (%d)!\n", tst->cmd_b_type);
  else
    printf("Unexpected block type encountered (%s)!\n",types[tst->cmd_b_type]);

  return SS$_ABORT;
}

int set_logical(string *log, string *equiv)
{
  static $DESCRIPTOR(process_table, "LNM$PROCESS");
  item_list itm[2];

  itm[0].buflen    = PLEN(equiv);
  itm[0].item_code = LNM$_STRING;
  itm[0].bufadr    = (unsigned long *) PPTR(equiv);
  itm[1].buflen    = 0;
  itm[1].item_code = 0;
  itm[1].bufadr    = 0;

  return sys$crelnm(0, &process_table, log, &PSL$C_USER, itm);
}
